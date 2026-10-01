/* no-rsa: the register save area is 120 bytes, with the backchain at 112
   and the parameter area at 120.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=no-rsa" } */
/* { dg-final { check-function-bodies "**" "" } } */

extern void t7 (long, long, long, long, long, long, long);

/*
** sixth:
**	lg	%r2,120\(%r15\)
**	br	%r14
*/
long
sixth (long a, long b, long c, long d, long e, long f)
{
  return f;
}

/*
** call7:
**	...
**	stmg	%r6,%r15,32\(%r15\)
**	lgr	%r14,%r15
**	lghi	%r6,5
**	lay	%r15,-104\(%r15\)
**	stg	%r14,112\(%r15\)
**	mvghi	128\(%r15\),7
**	mvghi	120\(%r15\),6
**	lghi	%r2,1
**	brasl	%r14,t7@PLT
**	lmg	%r6,%r15,136\(%r15\)
**	br	%r14
*/
void
call7 (void)
{
  t7 (1, 2, 3, 4, 5, 6, 7);
}

/* %r3 goes into its slot at 8, va_arg reads it through the register save
   area pointer, which points 16 bytes below the slot of %r2.  */
/*
** va:
**	stmg	%r3,%r15,8\(%r15\)
**	...
**	la	%r2,128\(%r15\)
**	...
**	lg	%r2,24\(%r2\)
**	...
*/
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

/*
** fa:
**	la	%r2,112\(%r15\)
**	br	%r14
*/
void *
fa (void)
{
  return __builtin_frame_address (0);
}

/*
** ra1:
**	lg	%r1,112\(%r15\)
**	lg	%r2,96\(%r1\)
**	br	%r14
*/
void *
ra1 (void)
{
  return __builtin_return_address (1);
}
