/* even-pairs: a register pair starts on an even register.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=struct-arg,int128,even-pairs" } */
/* { dg-final { check-function-bodies "**" "" } } */

struct pair { long a, b; };

/*
** f:
**	lgr	%r2,%r5
**	br	%r14
*/
long
f (long a, struct pair p)
{
  return p.b;
}

/*
** g:
**	lgr	%r2,%r4
**	lgr	%r3,%r5
**	br	%r14
*/
__int128
g (long a, __int128 x)
{
  return x;
}

/* The skipped %r3 is not used by a later argument.  */
/*
** h:
**	lgr	%r2,%r6
**	br	%r14
*/
long
h (long a, struct pair p, long b)
{
  return b;
}

/* A pair which does not get registers does not skip one.  */
/*
** k:
**	lgr	%r2,%r5
**	br	%r14
*/
long
k (long a, long b, long c, struct pair p, long d)
{
  return d;
}

/*
** k2:
**	lg	%r2,168\(%r15\)
**	br	%r14
*/
long
k2 (long a, long b, long c, struct pair p, long d)
{
  return p.b;
}
