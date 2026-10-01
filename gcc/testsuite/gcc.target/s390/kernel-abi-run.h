/* Helpers for checking -mexperimental-kernel-abi= register and stack
   images against hand-written assembly.

   kabi_dump () stores %r2-%r7 into kabi_regs and the first 256 bytes of
   the parameter area into kabi_stack, then returns kabi_ret in %r2-%r5.
   Called through a pointer of the appropriate type, it shows where a
   caller puts its arguments, and it returns any register image.

   kabi_invoke (FN) calls FN with %r2-%r7 taken from kabi_regs and the
   parameter area taken from kabi_stack, then stores %r2-%r5 into
   kabi_ret.  It shows where a callee expects its arguments and where it
   puts its return value.

   Both assume the kernel stack layout (-mpacked-stack -mbackchain).  */

#ifdef __S390_EXPERIMENTAL_KERNEL_ABI_NO_RSA__
#define KABI_RSA "120"
#define KABI_SAVE_R6 "32"
#define KABI_FRAME "376"
#define KABI_RESTORE_R6 "408"
#define KABI_BACKCHAIN "112"
#else
#define KABI_RSA "160"
#define KABI_SAVE_R6 "72"
#define KABI_FRAME "416"
#define KABI_RESTORE_R6 "488"
#define KABI_BACKCHAIN "152"
#endif

unsigned long kabi_regs[6];
unsigned long kabi_stack[32];
unsigned long kabi_ret[4];

extern char kabi_dump[];
extern void kabi_invoke (void *);

asm ("	.text\n"
     "	.globl	kabi_dump\n"
     "	.type	kabi_dump,@function\n"
     "kabi_dump:\n"
     "	larl	%r1,kabi_regs\n"
     "	stmg	%r2,%r7,0(%r1)\n"
     "	larl	%r1,kabi_stack\n"
     "	mvc	0(256,%r1)," KABI_RSA "(%r15)\n"
     "	larl	%r1,kabi_ret\n"
     "	lmg	%r2,%r5,0(%r1)\n"
     "	br	%r14\n"
     "	.size	kabi_dump,.-kabi_dump\n"
     "	.globl	kabi_invoke\n"
     "	.type	kabi_invoke,@function\n"
     "kabi_invoke:\n"
     "	stmg	%r6,%r15," KABI_SAVE_R6 "(%r15)\n"
     "	lgr	%r1,%r15\n"
     "	aghi	%r15,-" KABI_FRAME "\n"
     "	stg	%r1," KABI_BACKCHAIN "(%r15)\n"
     "	lgr	%r12,%r2\n"
     "	larl	%r1,kabi_stack\n"
     "	mvc	" KABI_RSA "(256,%r15),0(%r1)\n"
     "	larl	%r1,kabi_regs\n"
     "	lmg	%r2,%r7,0(%r1)\n"
     "	basr	%r14,%r12\n"
     "	larl	%r1,kabi_ret\n"
     "	stmg	%r2,%r5,0(%r1)\n"
     "	lmg	%r6,%r15," KABI_RESTORE_R6 "(%r15)\n"
     "	br	%r14\n"
     "	.size	kabi_invoke,.-kabi_invoke\n");

/* Garbage for the bits which the convention leaves unspecified.  */
#define KABI_JUNK 0xdeadbeefdeadbeefUL

/* The low SIZE bytes of X.  */
#define KABI_LOW(X, SIZE)						\
  ((SIZE) >= 8 ? (unsigned long) (X)					\
   : (unsigned long) (X) & ((1UL << ((SIZE) * 8)) - 1))

#define KABI_CHECK(X)							\
  do									\
    {									\
      if (!(X))								\
	__builtin_abort ();						\
    }									\
  while (0)

static inline void
kabi_clear (void)
{
  for (int i = 0; i < 6; i++)
    kabi_regs[i] = KABI_JUNK;
  for (int i = 0; i < 32; i++)
    kabi_stack[i] = KABI_JUNK;
  for (int i = 0; i < 4; i++)
    kabi_ret[i] = KABI_JUNK;
}

static const unsigned char kabi_pattern[40] =
  {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xf1, 0x02,
    0x13, 0x24, 0x35, 0x46, 0x57, 0x68, 0x79, 0x8a,
    0x9b, 0xac, 0xbd, 0xce, 0xdf, 0xe0, 0xf2, 0x03,
    0x14, 0x25, 0x36, 0x47, 0x58, 0x69, 0x7a, 0x8b
  };

/* Store into IMG the register image of the SIZE bytes at P: words in
   memory order, a partial last word right-justified, junk above it.  */

static inline void
kabi_image (const void *p, unsigned long size, unsigned long *img)
{
  const unsigned char *b = (const unsigned char *) p;

  for (unsigned long i = 0; i * 8 < size; i++)
    {
      unsigned long n = size - i * 8 < 8 ? size - i * 8 : 8;
      unsigned long v = n < 8 ? KABI_JUNK : 0;

      for (unsigned long j = 0; j < n; j++)
	v = (v << 8) | b[i * 8 + j];
      img[i] = v;
    }
}

/* Check that the NREGS words at REGS hold the register image of the SIZE
   bytes at P, ignoring the unspecified bits.  */

static inline void
kabi_check_image (const unsigned long *regs, const void *p,
		  unsigned long size)
{
  unsigned long img[4];

  kabi_image (p, size, img);
  for (unsigned long i = 0; i * 8 < size; i++)
    {
      unsigned long n = size - i * 8 < 8 ? size - i * 8 : 8;
      KABI_CHECK (KABI_LOW (regs[i], n) == KABI_LOW (img[i], n));
    }
}
