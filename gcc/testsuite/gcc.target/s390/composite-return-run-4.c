/* Check that __builtin_apply and __builtin_return save and restore both
   registers which may hold a composite return value.  */

/* { dg-do run } */
/* { dg-options "-O2 -freg-struct-return" } */

extern void abort (void);

struct s12 { unsigned int a, b, c; };
struct s16 { unsigned long a, b; };

struct s12 __attribute__ ((noinline))
mk12 (unsigned int x)
{
  struct s12 s = { x, x + 1, x + 2 };

  return s;
}

struct s16 __attribute__ ((noinline))
mk16 (unsigned long x)
{
  struct s16 s = { x, x + 1 };

  return s;
}

struct s12
wrap12 (unsigned int x)
{
  void *args = __builtin_apply_args ();
  void *result = __builtin_apply ((void (*) (void)) mk12, args, 64);

  __builtin_return (result);
}

struct s16
wrap16 (unsigned long x)
{
  void *args = __builtin_apply_args ();
  void *result = __builtin_apply ((void (*) (void)) mk16, args, 64);

  __builtin_return (result);
}

int
main (void)
{
  struct s12 a = wrap12 (0x11223344);
  struct s16 b = wrap16 (0x1122334455667788UL);

  if (a.a != 0x11223344 || a.b != 0x11223345 || a.c != 0x11223346)
    abort ();

  if (b.a != 0x1122334455667788UL || b.b != 0x1122334455667789UL)
    abort ();

  return 0;
}
