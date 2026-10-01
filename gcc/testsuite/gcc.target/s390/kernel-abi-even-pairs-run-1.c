/* even-pairs: a register pair starts on an even register, in calls and in
   va_arg.  */

/* { dg-do run } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=struct-arg,int128,even-pairs,r6-clobbered,r7-arg -Wno-psabi" } */

#include <stdarg.h>
#include "kernel-abi-run.h"

#define DUMP(TYPE) ((TYPE) (void *) kabi_dump)

typedef struct { unsigned char c[12]; } b12;

#define HI 0x0123456789abcdefUL
#define LO 0xfedcba9876543210UL
#define V (((unsigned __int128) HI << 64) | LO)

static long r[8];
static b12 v12, x12;
static unsigned __int128 x128;

__attribute__ ((noipa)) void
f (long a, b12 p, long b, unsigned __int128 q, long c)
{
  r[0] = a;
  x12 = p;
  r[1] = b;
  x128 = q;
  r[2] = c;
}

__attribute__ ((noipa)) void
va (int n, ...)
{
  va_list ap;

  va_start (ap, n);
  x12 = va_arg (ap, b12);
  r[0] = va_arg (ap, long);
  x128 = va_arg (ap, unsigned __int128);
  r[1] = va_arg (ap, long);
  r[2] = va_arg (ap, long);
  va_end (ap);
}

int
main (void)
{
  __builtin_memcpy (&v12, kabi_pattern, sizeof v12);

  /* %r2, skip %r3, %r4:%r5, %r6, skip %r7, the int128 on the stack, and
     the last long on the stack as well.  */
  kabi_clear ();
  DUMP (void (*) (long, b12, long, unsigned __int128, long))
    (1, v12, 2, V, 3);
  KABI_CHECK (kabi_regs[0] == 1);
  kabi_check_image (&kabi_regs[2], &v12, 12);
  KABI_CHECK (kabi_regs[4] == 2);
  KABI_CHECK (kabi_stack[0] == HI && kabi_stack[1] == LO);
  KABI_CHECK (kabi_regs[5] == 3);

  kabi_clear ();
  kabi_regs[0] = 1;
  kabi_image (&v12, 12, &kabi_regs[2]);
  kabi_regs[4] = 2;
  kabi_stack[0] = HI;
  kabi_stack[1] = LO;
  kabi_regs[5] = 3;
  kabi_invoke (f);
  KABI_CHECK (r[0] == 1 && r[1] == 2 && r[2] == 3 && x128 == V);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);

  /* n in %r2, skip %r3, b12 in %r4:%r5, a long in %r6, the int128 does not
     fit into %r7 and does not skip it, so the next long takes %r7, and
     the last long is on the stack.  */
  kabi_clear ();
  kabi_regs[0] = 0;
  kabi_image (&v12, 12, &kabi_regs[2]);
  kabi_regs[4] = 11;
  kabi_stack[0] = HI;
  kabi_stack[1] = LO;
  kabi_regs[5] = 12;
  kabi_stack[2] = 13;
  kabi_invoke (va);
  KABI_CHECK (r[0] == 11 && r[1] == 12 && r[2] == 13 && x128 == V);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);

  r[0] = r[1] = r[2] = 0;
  x128 = 0;
  va (0, v12, 11L, V, 12L, 13L);
  KABI_CHECK (r[0] == 11 && r[1] == 12 && r[2] == 13 && x128 == V);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);

  return 0;
}
