/* no-ext: integer arguments and return values are not extended.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=no-ext" } */
/* { dg-final { check-function-bodies "**" "" } } */

extern void f (int);
extern int geti (void);

/*
** k:
**	br	%r14
*/
int
k (int x)
{
  return x;
}

/*
** l:
**	lgfr	%r2,%r2
**	br	%r14
*/
long
l (int x)
{
  return x;
}

/*
** ul:
**	llgfr	%r2,%r2
**	br	%r14
*/
unsigned long
ul (unsigned int x)
{
  return x;
}

/*
** g:
**	jg	f@PLT
*/
void
g (long x)
{
  f (x);
}

/*
** add:
**	ar	%r2,%r3
**	br	%r14
*/
int
add (int a, int b)
{
  return a + b;
}

/*
** useg:
**	...
**	brasl	%r14,geti@PLT
**	lmg	%r14,%r15,160\(%r15\)
**	lgfr	%r2,%r2
**	br	%r14
*/
long
useg (void)
{
  return geti ();
}

/*
** isb:
**	llcr	%r2,%r2
**	br	%r14
*/
int
isb (_Bool b)
{
  return b;
}

/*
** sc:
**	lgbr	%r2,%r2
**	br	%r14
*/
long
sc (signed char c)
{
  return c;
}
