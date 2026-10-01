/* struct-32: %r4 and %r5 may hold a return value, so the epilogue must not
   load the return address into them.  */

/* { dg-do run } */
/* { dg-options "-O2 -march=z10 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=struct-ret,struct-arg,struct-32" } */

struct s { unsigned long a[3]; };
struct s s, a[5];

__attribute__ ((noipa)) struct s
check (struct s arg0, struct s *arg1, struct s arg2)
{
  struct s ret;

  __builtin_memset (&ret, 0, sizeof (ret));
  if (arg0.a[2] != s.a[2] || arg2.a[2] != a[2].a[2] || arg1 != &a[1])
    __builtin_abort ();
  ret.a[2] = s.a[2];
  return ret;
}

__attribute__ ((noipa)) void
checkx (struct s arg)
{
  if (arg.a[2] != s.a[2])
    __builtin_abort ();
}

int
main (void)
{
  s.a[2] = 0x1234;
  a[2].a[2] = 77;
  checkx (check (s, &a[1], a[2]));
  return 0;
}
