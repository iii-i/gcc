/* struct-arg: composites of up to 16 bytes are passed in one or two GPRs,
   a PAIR which does not fit goes to the stack as a whole and leaves the
   remaining register to later arguments, and an EMPTY takes nothing.  */

/* { dg-do run } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=struct-arg -Wno-psabi" } */

#include <stdarg.h>
#include "kernel-abi-run.h"

typedef struct { unsigned char c[3]; } b3;
typedef struct { unsigned char c[7]; } b7;
typedef struct { unsigned char c[9]; } b9;
typedef struct { unsigned char c[12]; } b12;
typedef struct { unsigned char c[15]; } b15;
typedef struct { unsigned char c[16]; } b16;
typedef struct { } empty;

#define L1 0x0101010101010101L
#define L2 0x0202020202020202L
#define L3 0x0303030303030303L
#define L4 0x0404040404040404L
#define L5 0x0505050505050505L
#define L6 0x0606060606060606L

static b3 v3;
static b7 v7;
static b9 v9;
static b12 v12;
static b15 v15;
static b16 v16;

#define DUMP(TYPE) ((TYPE) (void *) kabi_dump)

/* Caller side: where do the arguments go?  */

static void
check_caller (void)
{
  /* A PAIR after N scalars, for every N.  */
  kabi_clear ();
  DUMP (void (*) (b12, long)) (v12, L1);
  kabi_check_image (&kabi_regs[0], &v12, 12);
  KABI_CHECK (kabi_regs[2] == L1);

  kabi_clear ();
  DUMP (void (*) (long, b12, long)) (L1, v12, L2);
  kabi_check_image (&kabi_regs[1], &v12, 12);
  KABI_CHECK (kabi_regs[3] == L2);

  kabi_clear ();
  DUMP (void (*) (long, long, b12, long)) (L1, L2, v12, L3);
  kabi_check_image (&kabi_regs[2], &v12, 12);
  KABI_CHECK (kabi_regs[4] == L3);

  kabi_clear ();
  DUMP (void (*) (long, long, long, b15)) (L1, L2, L3, v15);
  kabi_check_image (&kabi_regs[3], &v15, 15);

  /* Only %r6 is left: the PAIR goes to the stack as its memory image and
     %r6 is taken by the next scalar.  */
  kabi_clear ();
  DUMP (void (*) (long, long, long, long, b12, long, long))
    (L1, L2, L3, L4, v12, L5, L6);
  KABI_CHECK (kabi_regs[4] == L5);
  KABI_CHECK (__builtin_memcmp (&kabi_stack[0], &v12, 12) == 0);
  KABI_CHECK (kabi_stack[2] == L6);

  /* ... and by the next ONE.  */
  kabi_clear ();
  DUMP (void (*) (long, long, long, long, b9, b3, b16))
    (L1, L2, L3, L4, v9, v3, v16);
  kabi_check_image (&kabi_regs[4], &v3, 3);
  KABI_CHECK (__builtin_memcmp (&kabi_stack[0], &v9, 9) == 0);
  KABI_CHECK (__builtin_memcmp (&kabi_stack[2], &v16, 16) == 0);

  /* A ONE on the stack is right-justified.  */
  kabi_clear ();
  DUMP (void (*) (long, long, long, long, long, b3, b7, long))
    (L1, L2, L3, L4, L5, v3, v7, L6);
  KABI_CHECK (__builtin_memcmp ((char *) &kabi_stack[0] + 5, &v3, 3) == 0);
  KABI_CHECK (__builtin_memcmp ((char *) &kabi_stack[1] + 1, &v7, 7) == 0);
  KABI_CHECK (kabi_stack[2] == L6);

  /* EMPTY takes neither a register nor a stack slot.  */
  kabi_clear ();
  DUMP (void (*) (empty, long, empty, b9, empty)) ((empty) { }, L1,
						    (empty) { }, v9,
						    (empty) { });
  KABI_CHECK (kabi_regs[0] == L1);
  kabi_check_image (&kabi_regs[1], &v9, 9);

  kabi_clear ();
  DUMP (void (*) (long, long, long, long, long, empty, long, empty, long))
    (L1, L2, L3, L4, L5, (empty) { }, L6, (empty) { }, L1);
  KABI_CHECK (kabi_stack[0] == L6);
  KABI_CHECK (kabi_stack[1] == L1);

  /* Complex values are composites.  */
  _Complex int ci;
  _Complex long cl;
  __real__ ci = 1;
  __imag__ ci = 2;
  __real__ cl = 3;
  __imag__ cl = 4;
  kabi_clear ();
  DUMP (void (*) (_Complex int, long, _Complex long)) (ci, L1, cl);
  KABI_CHECK (kabi_regs[0] == 0x0000000100000002UL);
  KABI_CHECK (kabi_regs[1] == L1);
  KABI_CHECK (kabi_regs[2] == 3 && kabi_regs[3] == 4);
}

/* Callee side: where are the arguments expected?  */

static long r1, r2, r3, r4;
static b3 x3;
static b9 x9;
static b12 x12;
static b16 x16;
static _Complex long xcl;

__attribute__ ((noipa)) void
f1 (long a, b12 x, long b)
{
  r1 = a;
  x12 = x;
  r2 = b;
}

__attribute__ ((noipa)) void
f2 (long a, long b, long c, long d, b9 x, b3 y, b16 z, long e)
{
  r1 = a + b + c + d;
  x9 = x;
  x3 = y;
  x16 = z;
  r2 = e;
}

__attribute__ ((noipa)) void
f3 (empty a, long b, empty c, _Complex long d, empty e, long f)
{
  r1 = b;
  xcl = d;
  r2 = f;
}

static void
check_callee (void)
{
  kabi_clear ();
  kabi_regs[0] = L1;
  kabi_image (&v12, 12, &kabi_regs[1]);
  kabi_regs[3] = L2;
  kabi_invoke (f1);
  KABI_CHECK (r1 == L1 && r2 == L2);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);

  kabi_clear ();
  kabi_regs[0] = 1;
  kabi_regs[1] = 2;
  kabi_regs[2] = 3;
  kabi_regs[3] = 4;
  kabi_image (&v3, 3, &kabi_regs[4]);
  __builtin_memcpy (&kabi_stack[0], &v9, 9);
  __builtin_memcpy (&kabi_stack[2], &v16, 16);
  kabi_stack[4] = L5;
  kabi_invoke (f2);
  KABI_CHECK (r1 == 10 && r2 == L5);
  KABI_CHECK (__builtin_memcmp (&x9, &v9, 9) == 0);
  KABI_CHECK (__builtin_memcmp (&x3, &v3, 3) == 0);
  KABI_CHECK (__builtin_memcmp (&x16, &v16, 16) == 0);

  kabi_clear ();
  kabi_regs[0] = L1;
  kabi_regs[1] = 5;
  kabi_regs[2] = 6;
  kabi_regs[3] = L2;
  kabi_invoke (f3);
  KABI_CHECK (r1 == L1 && r2 == L2);
  KABI_CHECK (__real__ xcl == 5 && __imag__ xcl == 6);
}

/* va_arg follows the same rules.  */

__attribute__ ((noipa)) void
va (int n, ...)
{
  va_list ap;

  va_start (ap, n);
  r1 = va_arg (ap, long);
  r2 = va_arg (ap, long);
  x12 = va_arg (ap, b12);
  x3 = va_arg (ap, b3);
  x9 = va_arg (ap, b9);
  r3 = va_arg (ap, long);
  x16 = va_arg (ap, b16);
  (void) va_arg (ap, empty);
  r4 = va_arg (ap, long);
  va_end (ap);
}

static void
check_va (void)
{
  /* n in %r2, two longs in %r3-%r4, the PAIR in %r5-%r6, then everything
     on the stack.  */
  r1 = r2 = r3 = r4 = 0;
  kabi_clear ();
  kabi_regs[1] = L1;
  kabi_regs[2] = L2;
  kabi_image (&v12, 12, &kabi_regs[3]);
  kabi_image (&v3, 3, &kabi_stack[0]);
  __builtin_memcpy (&kabi_stack[1], &v9, 9);
  kabi_stack[3] = L3;
  __builtin_memcpy (&kabi_stack[4], &v16, 16);
  kabi_stack[6] = L4;
  kabi_invoke (va);
  KABI_CHECK (r1 == L1 && r2 == L2 && r3 == L3 && r4 == L4);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);
  KABI_CHECK (__builtin_memcmp (&x3, &v3, 3) == 0);
  KABI_CHECK (__builtin_memcmp (&x9, &v9, 9) == 0);
  KABI_CHECK (__builtin_memcmp (&x16, &v16, 16) == 0);

  /* The same from C, plus a PAIR which does not fit into %r6 alone,
     which is then taken by a long.  */
  r1 = r2 = r3 = r4 = 0;
  va (0, L1, L2, v12, v3, v9, L3, v16, (empty) { }, L4);
  KABI_CHECK (r1 == L1 && r2 == L2 && r3 == L3 && r4 == L4);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);
  KABI_CHECK (__builtin_memcmp (&x3, &v3, 3) == 0);
  KABI_CHECK (__builtin_memcmp (&x9, &v9, 9) == 0);
  KABI_CHECK (__builtin_memcmp (&x16, &v16, 16) == 0);
}

__attribute__ ((noipa)) void
va2 (int n, ...)
{
  va_list ap;

  va_start (ap, n);
  r1 = va_arg (ap, long);
  r2 = va_arg (ap, long);
  r3 = va_arg (ap, long);
  x12 = va_arg (ap, b12);
  r4 = va_arg (ap, long);
  x16 = va_arg (ap, b16);
  va_end (ap);
}

static void
check_va2 (void)
{
  kabi_clear ();
  kabi_regs[1] = L1;
  kabi_regs[2] = L2;
  kabi_regs[3] = L3;
  __builtin_memcpy (&kabi_stack[0], &v12, 12);
  kabi_regs[4] = L4;
  __builtin_memcpy (&kabi_stack[2], &v16, 16);
  kabi_invoke (va2);
  KABI_CHECK (r1 == L1 && r2 == L2 && r3 == L3 && r4 == L4);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);
  KABI_CHECK (__builtin_memcmp (&x16, &v16, 16) == 0);

  r1 = r2 = r3 = r4 = 0;
  va2 (0, L1, L2, L3, v12, L4, v16);
  KABI_CHECK (r1 == L1 && r2 == L2 && r3 == L3 && r4 == L4);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);
  KABI_CHECK (__builtin_memcmp (&x16, &v16, 16) == 0);
}

int
main (void)
{
  __builtin_memcpy (&v3, kabi_pattern, sizeof v3);
  __builtin_memcpy (&v7, kabi_pattern + 3, sizeof v7);
  __builtin_memcpy (&v9, kabi_pattern + 5, sizeof v9);
  __builtin_memcpy (&v12, kabi_pattern + 10, sizeof v12);
  __builtin_memcpy (&v15, kabi_pattern + 20, sizeof v15);
  __builtin_memcpy (&v16, kabi_pattern + 22, sizeof v16);
  check_caller ();
  check_callee ();
  check_va ();
  check_va2 ();
  return 0;
}
