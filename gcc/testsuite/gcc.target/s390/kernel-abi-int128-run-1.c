/* int128: __int128 is passed and returned in a GPR pair, high doubleword
   first, which may start on an odd register.  */

/* { dg-do run } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=int128" } */

#include <stdarg.h>
#include "kernel-abi-run.h"

#define HI 0x0123456789abcdefUL
#define LO 0xfedcba9876543210UL
#define V (((unsigned __int128) HI << 64) | LO)
#define L1 0x1111111111111111L
#define L2 0x2222222222222222L
#define L3 0x3333333333333333L
#define L4 0x4444444444444444L

#define DUMP(TYPE) ((TYPE) (void *) kabi_dump)

static unsigned __int128 x1, x2;
static long r1, r2;

__attribute__ ((noipa)) unsigned __int128
ret (void)
{
  return V;
}

__attribute__ ((noipa)) void
f1 (long a, unsigned __int128 x, long b, unsigned __int128 y)
{
  r1 = a;
  x1 = x;
  r2 = b;
  x2 = y;
}

__attribute__ ((noipa)) void
va (int n, ...)
{
  va_list ap;

  va_start (ap, n);
  x1 = va_arg (ap, unsigned __int128);
  r1 = va_arg (ap, long);
  x2 = va_arg (ap, unsigned __int128);
  r2 = va_arg (ap, long);
  va_end (ap);
}

int
main (void)
{
  unsigned __int128 v;

  kabi_clear ();
  kabi_invoke (ret);
  KABI_CHECK (kabi_ret[0] == HI && kabi_ret[1] == LO);

  kabi_clear ();
  kabi_ret[0] = HI;
  kabi_ret[1] = LO;
  v = DUMP (unsigned __int128 (*) (void)) ();
  KABI_CHECK (v == V);

  /* %r2, %r3:%r4, %r5, and the second one does not fit into %r6.  */
  kabi_clear ();
  DUMP (void (*) (long, unsigned __int128, long, unsigned __int128, long))
    (L1, V, L2, ~V, L3);
  KABI_CHECK (kabi_regs[0] == L1);
  KABI_CHECK (kabi_regs[1] == HI && kabi_regs[2] == LO);
  KABI_CHECK (kabi_regs[3] == L2);
  KABI_CHECK (kabi_regs[4] == L3);
  KABI_CHECK (kabi_stack[0] == ~HI && kabi_stack[1] == ~LO);

  kabi_clear ();
  kabi_regs[0] = L1;
  kabi_regs[1] = HI;
  kabi_regs[2] = LO;
  kabi_regs[3] = L2;
  kabi_stack[0] = ~HI;
  kabi_stack[1] = ~LO;
  kabi_invoke (f1);
  KABI_CHECK (r1 == L1 && x1 == V && r2 == L2 && x2 == ~V);

  /* n in %r2, the first value in %r3:%r4, a long in %r5, then the second
     value on the stack, and the last long in %r6.  */
  kabi_clear ();
  kabi_regs[1] = HI;
  kabi_regs[2] = LO;
  kabi_regs[3] = L1;
  kabi_stack[0] = ~HI;
  kabi_stack[1] = ~LO;
  kabi_regs[4] = L2;
  kabi_invoke (va);
  KABI_CHECK (x1 == V && r1 == L1 && x2 == ~V && r2 == L2);

  x1 = x2 = 0;
  r1 = r2 = 0;
  va (0, V, L1, ~V, L2);
  KABI_CHECK (x1 == V && r1 == L1 && x2 == ~V && r2 == L2);

  return 0;
}
