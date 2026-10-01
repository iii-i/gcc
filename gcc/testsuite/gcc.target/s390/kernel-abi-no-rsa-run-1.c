/* no-rsa: the register save area is 120 bytes, with the backchain at 112,
   %rN at 8 * (N - 2), and the parameter area at 120.  */

/* { dg-do run } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=no-rsa,struct-arg,r6-clobbered,r7-arg -Wno-psabi" } */

#include <stdarg.h>
#include "kernel-abi-run.h"

#define DUMP(TYPE) ((TYPE) (void *) kabi_dump)

typedef struct { unsigned char c[12]; } b12;

static long r[10];
static b12 v12, x12;

__attribute__ ((noipa)) void
f9 (long a, long b, long c, long d, long e, long f, long g, b12 h, long i)
{
  r[0] = a; r[1] = b; r[2] = c; r[3] = d; r[4] = e; r[5] = f; r[6] = g;
  x12 = h;
  r[7] = i;
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

/* The backchain and the saved %r14 of the caller.  */

__attribute__ ((noipa)) void *
caller_ra (void)
{
  return __builtin_return_address (1);
}

__attribute__ ((noipa)) void
level1 (void **ra, void **ra1)
{
  *ra = __builtin_return_address (0);
  *ra1 = caller_ra ();
}

int
main (void)
{
  void *ra, *ra1;

  __builtin_memcpy (&v12, kabi_pattern, sizeof v12);

  kabi_clear ();
  DUMP (void (*) (long, long, long, long, long, long, long, b12, long))
    (1, 2, 3, 4, 5, 6, 7, v12, 8);
  for (int i = 0; i < 6; i++)
    KABI_CHECK (kabi_regs[i] == i + 1);
  KABI_CHECK (kabi_stack[0] == 7);
  KABI_CHECK (__builtin_memcmp (&kabi_stack[1], &v12, 12) == 0);
  KABI_CHECK (kabi_stack[3] == 8);

  kabi_clear ();
  for (int i = 0; i < 6; i++)
    kabi_regs[i] = i + 1;
  kabi_stack[0] = 7;
  __builtin_memcpy (&kabi_stack[1], &v12, 12);
  kabi_stack[3] = 8;
  kabi_invoke (f9);
  for (int i = 0; i < 8; i++)
    KABI_CHECK (r[i] == i + 1);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);

  /* n in %r2, four longs in %r3-%r6, the b12 on the stack, the next long
     in %r7, and the last one on the stack again.  */
  kabi_clear ();
  kabi_regs[0] = 5;
  for (int i = 1; i < 5; i++)
    kabi_regs[i] = i + 10;
  __builtin_memcpy (&kabi_stack[0], &v12, 12);
  kabi_regs[5] = 15;
  kabi_stack[2] = 16;
  kabi_invoke (va);
  for (int i = 0; i < 6; i++)
    KABI_CHECK (r[i] == i + 11);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);

  va (6, 11L, 12L, 13L, 14L, 15L, 16L, v12, 17L);
  for (int i = 0; i < 7; i++)
    KABI_CHECK (r[i] == i + 11);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);

  level1 (&ra, &ra1);
  KABI_CHECK (ra == ra1);

  return 0;
}
