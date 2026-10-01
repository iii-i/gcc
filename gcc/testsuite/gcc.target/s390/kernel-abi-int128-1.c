/* int128: __int128 is passed and returned in a GPR pair, which may start
   on an odd register.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=int128" } */
/* { dg-final { check-function-bodies "**" "" } } */

extern void take (long, __int128);

/*
** add:
**	algr	%r3,%r5
**	alcgr	%r2,%r4
**	br	%r14
*/
__int128
add (__int128 a, __int128 b)
{
  return a + b;
}

/*
** odd:
**	lgr	%r2,%r3
**	lgr	%r3,%r4
**	br	%r14
*/
__int128
odd (long x, __int128 y)
{
  return y;
}

/*
** last:
**	lg	%r2,160\(%r15\)
**	lg	%r3,168\(%r15\)
**	br	%r14
*/
__int128
last (long a, long b, long c, long d, __int128 y)
{
  return y;
}

/*
** after:
**	lgr	%r2,%r6
**	br	%r14
*/
long
after (long a, long b, long c, long d, __int128 y, long z)
{
  return z;
}

/*
** mul:
**	mlgr	%r2,%r2
**	br	%r14
*/
unsigned __int128
mul (unsigned long a, unsigned long b)
{
  return (unsigned __int128) a * b;
}

/*
** call:
**	lgr	%r4,%r3
**	lgr	%r3,%r2
**	lghi	%r2,1
**	jg	take@PLT
*/
void
call (__int128 x)
{
  take (1, x);
}

/* Library calls use the same convention.  */
/*
** div:
**	...
**	brasl	%r14,__divti3@PLT
**	lmg	%r14,%r15,160\(%r15\)
**	br	%r14
*/
__int128
div (__int128 a, __int128 b)
{
  return a / b;
}
