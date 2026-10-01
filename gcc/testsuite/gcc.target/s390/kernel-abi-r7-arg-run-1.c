/* r7-arg: %r7 is a sixth argument register.  */

/* { dg-do run } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=r6-clobbered,r7-arg,struct-arg,int128 -Wno-psabi" } */

#include <stdarg.h>
#include "kernel-abi-run.h"

#define DUMP(TYPE) ((TYPE) (void *) kabi_dump)

typedef struct { unsigned char c[12]; } b12;

static long r[8];
static b12 v12, x12;
static unsigned __int128 x128;

__attribute__ ((noipa)) void
f7 (long a, long b, long c, long d, long e, long f, long g)
{
  r[0] = a; r[1] = b; r[2] = c; r[3] = d; r[4] = e; r[5] = f; r[6] = g;
}

__attribute__ ((noipa)) void
fp (long a, long b, long c, long d, b12 p, long e)
{
  r[0] = a + b + c + d;
  x12 = p;
  r[1] = e;
}

__attribute__ ((noipa)) void
fq (long a, long b, long c, long d, long e, unsigned __int128 p, long f)
{
  r[0] = a + b + c + d + e;
  x128 = p;
  r[1] = f;
}

__attribute__ ((noipa)) void
va (int n, ...)
{
  va_list ap;

  va_start (ap, n);
  for (int i = 0; i < n; i++)
    r[i] = va_arg (ap, long);
  x12 = va_arg (ap, b12);
  r[n] = va_arg (ap, long);
  va_end (ap);
}

extern void trash (void);

asm ("	.text\n"
     "	.globl	trash\n"
     "	.type	trash,@function\n"
     "trash:\n"
     "	lghi	%r6,-1\n"
     "	lghi	%r7,-1\n"
     "	br	%r14\n"
     "	.size	trash,.-trash\n");

__attribute__ ((noipa)) long
keep (long a, long b, long c, long d, long e, long f)
{
  long s = 0;

  for (int i = 0; i < 3; i++)
    {
      trash ();
      s += a * 3 + b * 5 + c * 7 + d * 11 + e * 13 + f * 17;
    }
  return s;
}

int
main (void)
{
  __builtin_memcpy (&v12, kabi_pattern, sizeof v12);

  kabi_clear ();
  DUMP (void (*) (long, long, long, long, long, long, long))
    (1, 2, 3, 4, 5, 6, 7);
  for (int i = 0; i < 6; i++)
    KABI_CHECK (kabi_regs[i] == i + 1);
  KABI_CHECK (kabi_stack[0] == 7);

  kabi_clear ();
  for (int i = 0; i < 6; i++)
    kabi_regs[i] = i + 1;
  kabi_stack[0] = 7;
  kabi_invoke (f7);
  for (int i = 0; i < 7; i++)
    KABI_CHECK (r[i] == i + 1);

  /* A PAIR in %r6:%r7.  */
  kabi_clear ();
  DUMP (void (*) (long, long, long, long, b12, long)) (1, 2, 3, 4, v12, 5);
  kabi_check_image (&kabi_regs[4], &v12, 12);
  KABI_CHECK (kabi_stack[0] == 5);

  kabi_clear ();
  kabi_regs[0] = 1;
  kabi_regs[1] = 2;
  kabi_regs[2] = 3;
  kabi_regs[3] = 4;
  kabi_image (&v12, 12, &kabi_regs[4]);
  kabi_stack[0] = 5;
  kabi_invoke (fp);
  KABI_CHECK (r[0] == 10 && r[1] == 5);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);

  /* A pair which does not fit into %r7 alone, which then takes the next
     argument.  */
  kabi_clear ();
  for (int i = 0; i < 5; i++)
    kabi_regs[i] = i + 1;
  kabi_stack[0] = 0x0123456789abcdefUL;
  kabi_stack[1] = 0xfedcba9876543210UL;
  kabi_regs[5] = 6;
  kabi_invoke (fq);
  KABI_CHECK (r[0] == 15 && r[1] == 6);
  KABI_CHECK (x128 == (((unsigned __int128) 0x0123456789abcdefUL << 64)
		       | 0xfedcba9876543210UL));

  /* va_arg takes %r7 from the register save area.  */
  kabi_clear ();
  kabi_regs[0] = 4;
  kabi_regs[1] = 11;
  kabi_regs[2] = 12;
  kabi_regs[3] = 13;
  kabi_regs[4] = 14;
  __builtin_memcpy (&kabi_stack[0], &v12, 12);
  kabi_regs[5] = 15;
  kabi_invoke (va);
  KABI_CHECK (r[0] == 11 && r[1] == 12 && r[2] == 13 && r[3] == 14
	      && r[4] == 15);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);

  va (3, 11L, 12L, 13L, v12, 14L);
  KABI_CHECK (r[0] == 11 && r[1] == 12 && r[2] == 13 && r[3] == 14);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);

  KABI_CHECK (keep (1, 2, 3, 4, 5, 6) == 3 * (3 + 10 + 21 + 44 + 65 + 102));

  return 0;
}
