/* Without -freg-struct-return, which is the default, composite values are
   returned in a buffer whose address the caller passes in %r2.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z13" } */
/* { dg-final { check-function-bodies "**" "" } } */

struct s4 { int a; };
struct s16 { long a, b; };

extern struct s16 get16 (void);

/*
** mk4:
**	st	%r3,0\(%r2\)
**	br	%r14
*/
struct s4
mk4 (int x)
{
  struct s4 s = { x };

  return s;
}

/*
** mk16:
**	vlvgp	%v0,%r3,%r4
**	vst	%v0,0\(%r2\),3
**	br	%r14
*/
struct s16
mk16 (long a, long b)
{
  struct s16 s = { a, b };

  return s;
}

/*
** use16:
**	stmg	%r14,%r15,112\(%r15\)
**	lay	%r15,-176\(%r15\)
**	la	%r2,160\(%r15\)
**	brasl	%r14,get16@PLT
**	lg	%r2,168\(%r15\)
**	lmg	%r14,%r15,288\(%r15\)
**	br	%r14
*/
long
use16 (void)
{
  return get16 ().b;
}
