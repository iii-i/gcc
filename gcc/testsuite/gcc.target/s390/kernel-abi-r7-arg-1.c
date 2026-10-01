/* r7-arg: %r7 is a sixth argument register, and call-clobbered.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=r6-clobbered,r7-arg" } */
/* { dg-final { check-function-bodies "**" "" } } */
/* { dg-final { scan-assembler-not "\\.cfi_offset 7," } } */

extern long t6 (long, long, long, long, long, long);

/*
** six:
**	lgr	%r2,%r7
**	br	%r14
*/
long
six (long a, long b, long c, long d, long e, long f)
{
  return f;
}

/*
** call6:
**	lgr	%r7,%r2
**	lghi	%r6,5
**	lghi	%r5,4
**	lghi	%r4,3
**	lghi	%r3,2
**	lghi	%r2,1
**	jg	t6@PLT
*/
long
call6 (long x)
{
  return t6 (1, 2, 3, 4, 5, x);
}

/*
** seventh:
**	lg	%r2,160\(%r15\)
**	br	%r14
*/
long
seventh (long a, long b, long c, long d, long e, long f, long g)
{
  return g;
}

/*
** clob:
**	lghi	%r7,0
**	br	%r14
*/
void
clob (void)
{
  asm volatile ("lghi\t%%r7,0" ::: "r7");
}

/* %r7 is saved for va_arg, but not described as a call-saved register.  */
long
va (int n, ...)
{
  __builtin_va_list ap;
  long r;

  __builtin_va_start (ap, n);
  r = __builtin_va_arg (ap, long);
  __builtin_va_end (ap);
  return r;
}
