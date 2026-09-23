/* Check the register layout of composite return values with
   -freg-struct-return: %r2 holds the first word of the value and
   %r3 the remaining bytes, each of them right-justified.  The bits which
   are not covered by the value are unspecified.  Therefore the producers
   written in assembly below fill them with garbage and the checks mask
   them out.  */

/* { dg-do run } */
/* { dg-options "-O2 -freg-struct-return" } */

extern void abort (void);

/* Call FN and store the return value registers %r2 and %r3 to OUT.  */
extern void call_fn (void *fn, unsigned long *out);
asm ("\t.text\n"
     "\t.align\t8\n"
     "\t.globl\tcall_fn\n"
     "\t.type\tcall_fn,@function\n"
     "call_fn:\n"
     "\tstmg\t%r13,%r15,104(%r15)\n"
     "\tlgr\t%r1,%r2\n"
     "\tlgr\t%r13,%r3\n"
     "\taghi\t%r15,-160\n"
     "\tbasr\t%r14,%r1\n"
     "\tstmg\t%r2,%r3,0(%r13)\n"
     "\tlmg\t%r13,%r15,264(%r15)\n"
     "\tbr\t%r14\n"
     "\t.size\tcall_fn,.-call_fn");

/* Define a function NAME which returns with the given values in %r2
   and %r3.  */
#define PRODUCER(NAME, R2VAL, R3VAL)			\
  asm ("\t.section\t.rodata\n"				\
       "\t.align\t8\n"					\
       ".L" #NAME "_data:\n"				\
       "\t.quad\t" #R2VAL "\n"				\
       "\t.quad\t" #R3VAL "\n"				\
       "\t.text\n"					\
       "\t.align\t8\n"					\
       "\t.globl\t" #NAME "\n"				\
       "\t.type\t" #NAME ",@function\n"			\
       #NAME ":\n"					\
       "\tlarl\t%r1,.L" #NAME "_data\n"			\
       "\tlmg\t%r2,%r3,0(%r1)\n"			\
       "\tbr\t%r14\n"					\
       "\t.size\t" #NAME ",.-" #NAME)

struct s1 { unsigned char a; };
struct s3 { unsigned char a[3]; };
struct s4 { unsigned int a; };
struct s8 { unsigned int a, b; };
struct s9 { unsigned long a; unsigned char b; } __attribute__ ((packed));
struct s11 { unsigned char a[11]; };
struct s12 { unsigned int a, b, c; };
struct s16 { unsigned long a, b; };

struct s1 mk1 (void) { struct s1 s = { 0x11 }; return s; }
struct s3 mk3 (void) { struct s3 s = { { 0x11, 0x22, 0x33 } }; return s; }
struct s4 mk4 (void) { struct s4 s = { 0x11223344 }; return s; }
struct s8 mk8 (void) { struct s8 s = { 0x11223344, 0x55667788 }; return s; }
struct s9 mk9 (void) { struct s9 s = { 0x1122334455667788UL, 0x99 }; return s; }
struct s11 mk11 (void) { struct s11 s = { { 0x11, 0x22, 0x33, 0x44, 0x55, 0x66,
					   0x77, 0x88, 0x99, 0xaa, 0xbb } };
			 return s; }
struct s12 mk12 (void) { struct s12 s = { 0x11223344, 0x55667788, 0x99aabbcc };
			 return s; }
struct s16 mk16 (void) { struct s16 s = { 0x1122334455667788UL,
					  0x99aabbccddeeff00UL }; return s; }

extern struct s1 get1 (void);
extern struct s3 get3 (void);
extern struct s4 get4 (void);
extern struct s8 get8 (void);
extern struct s9 get9 (void);
extern struct s11 get11 (void);
extern struct s12 get12 (void);
extern struct s16 get16 (void);

PRODUCER (get1, 0xdeadbeefdeadbe11, 0xdeadbeefdeadbeef);
PRODUCER (get3, 0xdeadbeefde112233, 0xdeadbeefdeadbeef);
PRODUCER (get4, 0xdeadbeef11223344, 0xdeadbeefdeadbeef);
PRODUCER (get8, 0x1122334455667788, 0xdeadbeefdeadbeef);
PRODUCER (get9, 0x1122334455667788, 0xdeadbeefdeadbe99);
PRODUCER (get11, 0x1122334455667788, 0xdeadbeefde99aabb);
PRODUCER (get12, 0x1122334455667788, 0xdeadbeef99aabbcc);
PRODUCER (get16, 0x1122334455667788, 0x99aabbccddeeff00);

int
main (void)
{
  unsigned long out[2];

  /* Values produced by the compiler must show up in %r2 and %r3.  */
  call_fn ((void *) mk1, out);
  if ((out[0] & 0xffUL) != 0x11UL)
    abort ();

  call_fn ((void *) mk3, out);
  if ((out[0] & 0xffffffUL) != 0x112233UL)
    abort ();

  call_fn ((void *) mk4, out);
  if ((out[0] & 0xffffffffUL) != 0x11223344UL)
    abort ();

  call_fn ((void *) mk8, out);
  if (out[0] != 0x1122334455667788UL)
    abort ();

  call_fn ((void *) mk9, out);
  if (out[0] != 0x1122334455667788UL || (out[1] & 0xffUL) != 0x99UL)
    abort ();

  call_fn ((void *) mk11, out);
  if (out[0] != 0x1122334455667788UL || (out[1] & 0xffffffUL) != 0x99aabbUL)
    abort ();

  call_fn ((void *) mk12, out);
  if (out[0] != 0x1122334455667788UL
      || (out[1] & 0xffffffffUL) != 0x99aabbccUL)
    abort ();

  call_fn ((void *) mk16, out);
  if (out[0] != 0x1122334455667788UL || out[1] != 0x99aabbccddeeff00UL)
    abort ();

  /* Values in %r2 and %r3 must be picked up by the compiler, without
     looking at the bits which do not belong to the value.  */
  {
    struct s1 v = get1 ();
    if (v.a != 0x11)
      abort ();
  }
  {
    struct s3 v = get3 ();
    if (v.a[0] != 0x11 || v.a[1] != 0x22 || v.a[2] != 0x33)
      abort ();
  }
  {
    struct s4 v = get4 ();
    if (v.a != 0x11223344)
      abort ();
  }
  {
    struct s8 v = get8 ();
    if (v.a != 0x11223344 || v.b != 0x55667788)
      abort ();
  }
  {
    struct s9 v = get9 ();
    if (v.a != 0x1122334455667788UL || v.b != 0x99)
      abort ();
  }
  {
    struct s11 v = get11 ();
    if (v.a[0] != 0x11 || v.a[7] != 0x88 || v.a[8] != 0x99 || v.a[9] != 0xaa
	|| v.a[10] != 0xbb)
      abort ();
  }
  {
    struct s12 v = get12 ();
    if (v.a != 0x11223344 || v.b != 0x55667788 || v.c != 0x99aabbcc)
      abort ();
  }
  {
    struct s16 v = get16 ();
    if (v.a != 0x1122334455667788UL || v.b != 0x99aabbccddeeff00UL)
      abort ();
  }

  return 0;
}
