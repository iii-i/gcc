/* no-ext: the bits of a register above a narrow integer argument or return
   value are unspecified, _Bool is 0 or 1 in bits 56-63.  */

/* { dg-do run } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=no-ext" } */

#include <stdarg.h>
#include "kernel-abi-run.h"

#define DUMP(TYPE) ((TYPE) (void *) kabi_dump)

static long r;

__attribute__ ((noipa)) void
sum (signed char a, unsigned char b, short c, unsigned short d, int e,
     unsigned int f)
{
  r = (long) a + b + c + d + e + f;
}

__attribute__ ((noipa)) void
cond (_Bool a, long b, long c)
{
  r = a ? b : c;
}

__attribute__ ((noipa)) int
inc (int x)
{
  return x + 1;
}

__attribute__ ((noipa)) void
va (int n, ...)
{
  va_list ap;

  va_start (ap, n);
  r = n + va_arg (ap, int);
  r += va_arg (ap, unsigned int);
  va_end (ap);
}

int
main (void)
{
  kabi_clear ();
  kabi_regs[0] = KABI_JUNK & ~0xffUL | 0xff;		/* -1 */
  kabi_regs[1] = KABI_JUNK & ~0xffUL | 0xff;		/* 255 */
  kabi_regs[2] = KABI_JUNK & ~0xffffUL | 0xfffe;	/* -2 */
  kabi_regs[3] = KABI_JUNK & ~0xffffUL | 0xfffe;	/* 65534 */
  kabi_regs[4] = KABI_JUNK & ~0xffffffffUL | 0xfffffffd;	/* -3 */
  kabi_stack[0] = KABI_JUNK & ~0xffffffffUL | 0xfffffffd;	/* 4294967293 */
  kabi_invoke (sum);
  KABI_CHECK (r == -1 + 255 - 2 + 65534 - 3 + 4294967293L);

  kabi_clear ();
  kabi_regs[0] = KABI_JUNK & ~0xffUL;
  kabi_regs[1] = 1;
  kabi_regs[2] = 2;
  kabi_invoke (cond);
  KABI_CHECK (r == 2);
  kabi_regs[0] = KABI_JUNK & ~0xffUL | 1;
  kabi_invoke (cond);
  KABI_CHECK (r == 1);

  kabi_clear ();
  kabi_regs[0] = KABI_JUNK & ~0xffffffffUL | 0x7fffffff;
  kabi_invoke (inc);
  KABI_CHECK ((unsigned int) kabi_ret[0] == 0x80000000);

  kabi_clear ();
  kabi_ret[0] = KABI_JUNK & ~0xffffffffUL | 0x80000000;
  r = DUMP (int (*) (void)) ();
  KABI_CHECK (r == -0x80000000L);
  r = DUMP (unsigned int (*) (void)) ();
  KABI_CHECK (r == 0x80000000L);
  kabi_ret[0] = KABI_JUNK & ~0xffUL | 0x80;
  r = DUMP (signed char (*) (void)) ();
  KABI_CHECK (r == -0x80);
  kabi_ret[0] = KABI_JUNK & ~0xffUL | 1;
  r = DUMP (_Bool (*) (void)) ();
  KABI_CHECK (r == 1);

  kabi_clear ();
  kabi_regs[0] = KABI_JUNK & ~0xffffffffUL | 1;
  kabi_regs[1] = KABI_JUNK & ~0xffffffffUL | 0xffffffff;
  kabi_regs[2] = KABI_JUNK & ~0xffffffffUL | 0xffffffff;
  kabi_invoke (va);
  KABI_CHECK (r == 1 - 1 + 0xffffffffL);

  return 0;
}
