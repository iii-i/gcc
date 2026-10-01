/* r6-clobbered: %r6 is a call-clobbered argument register.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=r6-clobbered" } */
/* { dg-final { check-function-bodies "**" "" } } */
/* { dg-final { scan-assembler-not "\\.cfi_offset 6," } } */
/* { dg-final { scan-assembler-not "%r6,72\\(%r15\\)" } } */

extern long t5 (long, long, long, long, long);
extern void g (void);

/* A sibling call may change the fifth argument.  */
/*
** s:
**	lgr	%r0,%r3
**	lgr	%r1,%r6
**	lgr	%r3,%r5
**	lgr	%r6,%r2
**	lgr	%r5,%r0
**	lgr	%r2,%r1
**	jg	t5@PLT
*/
long
s (long a, long b, long c, long d, long e)
{
  return t5 (e, d, c, b, a);
}

/* %r6 need not be saved.  */
/*
** clob:
**	lghi	%r6,0
**	br	%r14
*/
void
clob (void)
{
  asm volatile ("lghi\t%%r6,0" ::: "r6");
}

/* A value which lives across a call does not go into %r6.  */
/*
** keep:
**	stmg	%r12,%r15,120\(%r15\)
**	...
**	lgr	%r12,%r2
**	...
**	brasl	%r14,g@PLT
**	lgr	%r2,%r12
**	...
*/
long
keep (long x)
{
  g ();
  return x;
}

/* %r6 is saved for va_arg, but not described as a call-saved register.  */
int
va (int n, ...)
{
  __builtin_va_list ap;
  int r;

  __builtin_va_start (ap, n);
  r = __builtin_va_arg (ap, int);
  __builtin_va_end (ap);
  return r;
}
