/* struct-32: composites of 17 to 32 bytes are passed and returned in three
   or four GPRs, or on the stack as their memory image.  */

/* { dg-do run } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=struct-ret,struct-arg,struct-32,even-pairs -Wno-psabi" } */

#include <stdarg.h>
#include "kernel-abi-run.h"

#define DUMP(TYPE) ((TYPE) (void *) kabi_dump)

typedef struct { unsigned char c[17]; } b17;
typedef struct { unsigned char c[20]; } b20;
typedef struct { unsigned char c[24]; } b24;
typedef struct { unsigned char c[25]; } b25;
typedef struct { unsigned char c[31]; } b31;
typedef struct { unsigned char c[32]; } b32;
typedef struct { unsigned char c[12]; } b12;

#define RET(T)								\
  __attribute__ ((noipa)) T						\
  ret_##T (void)							\
  {									\
    T v;								\
    __builtin_memcpy (&v, kabi_pattern, sizeof (T));			\
    return v;								\
  }									\
									\
  static void								\
  check_##T (void)							\
  {									\
    T v;								\
									\
    kabi_clear ();							\
    kabi_invoke (ret_##T);						\
    kabi_check_image (kabi_ret, kabi_pattern, sizeof (T));		\
									\
    kabi_clear ();							\
    kabi_image (kabi_pattern, sizeof (T), kabi_ret);			\
    v = DUMP (T (*) (void)) ();						\
    KABI_CHECK (__builtin_memcmp (&v, kabi_pattern, sizeof (T)) == 0);	\
  }

RET (b17) RET (b20) RET (b24) RET (b25) RET (b31) RET (b32)

static b12 v12, x12;
static b17 v17, x17;
static b20 v20, x20;
static b31 v31, x31;
static b32 v32, x32;
static long r[4];

/* b17 does not fit after the pair and the long, so the next long takes
   %r5.  */
__attribute__ ((noipa)) void
f (b12 a, long x, b17 b, long c, b20 d)
{
  x12 = a;
  r[1] = x;
  x17 = b;
  r[0] = c;
  x20 = d;
}

__attribute__ ((noipa)) void
g (long a, b20 b, b31 c)
{
  r[0] = a;
  x20 = b;
  x31 = c;
}

__attribute__ ((noipa)) void
va (int n, ...)
{
  va_list ap;

  va_start (ap, n);
  x17 = va_arg (ap, b17);
  x31 = va_arg (ap, b31);
  r[0] = va_arg (ap, long);
  x32 = va_arg (ap, b32);
  r[1] = va_arg (ap, long);
  va_end (ap);
}

int
main (void)
{
  check_b17 (); check_b20 (); check_b24 (); check_b25 (); check_b31 ();
  check_b32 ();

  __builtin_memcpy (&v12, kabi_pattern + 1, sizeof v12);
  __builtin_memcpy (&v17, kabi_pattern + 2, sizeof v17);
  __builtin_memcpy (&v20, kabi_pattern + 3, sizeof v20);
  __builtin_memcpy (&v31, kabi_pattern + 4, sizeof v31);
  __builtin_memcpy (&v32, kabi_pattern + 5, sizeof v32);

  kabi_clear ();
  DUMP (void (*) (b12, long, b17, long, b20)) (v12, 2, v17, 1, v20);
  kabi_check_image (&kabi_regs[0], &v12, 12);
  KABI_CHECK (kabi_regs[2] == 2);
  KABI_CHECK (kabi_regs[3] == 1);
  KABI_CHECK (__builtin_memcmp (&kabi_stack[0], &v17, 17) == 0);
  KABI_CHECK (__builtin_memcmp (&kabi_stack[3], &v20, 20) == 0);

  kabi_clear ();
  kabi_image (&v12, 12, &kabi_regs[0]);
  kabi_regs[2] = 2;
  kabi_regs[3] = 1;
  __builtin_memcpy (&kabi_stack[0], &v17, 17);
  __builtin_memcpy (&kabi_stack[3], &v20, 20);
  kabi_invoke (f);
  KABI_CHECK (r[0] == 1 && r[1] == 2);
  KABI_CHECK (__builtin_memcmp (&x12, &v12, 12) == 0);
  KABI_CHECK (__builtin_memcmp (&x17, &v17, 17) == 0);
  KABI_CHECK (__builtin_memcmp (&x20, &v20, 20) == 0);

  kabi_clear ();
  DUMP (void (*) (long, b20, b31)) (1, v20, v31);
  KABI_CHECK (kabi_regs[0] == 1);
  kabi_check_image (&kabi_regs[1], &v20, 20);
  KABI_CHECK (__builtin_memcmp (&kabi_stack[0], &v31, 31) == 0);

  kabi_clear ();
  kabi_regs[0] = 1;
  kabi_image (&v20, 20, &kabi_regs[1]);
  __builtin_memcpy (&kabi_stack[0], &v31, 31);
  kabi_invoke (g);
  KABI_CHECK (r[0] == 1);
  KABI_CHECK (__builtin_memcmp (&x20, &v20, 20) == 0);
  KABI_CHECK (__builtin_memcmp (&x31, &v31, 31) == 0);

  /* n in %r2, b17 in %r3-%r5, b31 on the stack, the long in %r6, b32 and
     the last long on the stack.  */
  kabi_clear ();
  kabi_image (&v17, 17, &kabi_regs[1]);
  __builtin_memcpy (&kabi_stack[0], &v31, 31);
  kabi_regs[4] = 11;
  __builtin_memcpy (&kabi_stack[4], &v32, 32);
  kabi_stack[8] = 12;
  kabi_invoke (va);
  KABI_CHECK (r[0] == 11 && r[1] == 12);
  KABI_CHECK (__builtin_memcmp (&x17, &v17, 17) == 0);
  KABI_CHECK (__builtin_memcmp (&x31, &v31, 31) == 0);
  KABI_CHECK (__builtin_memcmp (&x32, &v32, 32) == 0);

  r[0] = r[1] = 0;
  va (0, v17, v31, 11L, v32, 12L);
  KABI_CHECK (r[0] == 11 && r[1] == 12);
  KABI_CHECK (__builtin_memcmp (&x17, &v17, 17) == 0);
  KABI_CHECK (__builtin_memcmp (&x31, &v31, 31) == 0);
  KABI_CHECK (__builtin_memcmp (&x32, &v32, 32) == 0);

  return 0;
}
