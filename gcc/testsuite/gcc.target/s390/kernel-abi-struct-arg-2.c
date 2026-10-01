/* struct-ret and struct-arg together: a small composite passes through
   without touching memory.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=struct-arg,struct-ret" } */
/* { dg-final { check-function-bodies "**" "" } } */

struct pair { long a, b; };

/*
** id:
**	br	%r14
*/
struct pair
id (struct pair x)
{
  return x;
}

/*
** swap:
**	lgr	%r1,%r3
**	lgr	%r3,%r2
**	lgr	%r2,%r1
**	br	%r14
*/
struct pair
swap (struct pair x)
{
  return (struct pair) { x.b, x.a };
}
