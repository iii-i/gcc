/* struct-arg: composites of up to 16 bytes are passed in one or two
   GPRs.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=struct-arg" } */
/* { dg-final { check-function-bodies "**" "" } } */

struct pair { long a, b; };
struct s3 { char c[3]; };

extern void take_pair (struct pair);
extern void take5 (long, long, long, long, struct pair, long);

/*
** first:
**	br	%r14
*/
long
first (struct pair p)
{
  return p.a;
}

/*
** second:
**	lgr	%r2,%r3
**	br	%r14
*/
long
second (struct pair p)
{
  return p.b;
}

/*
** fwd:
**	lgr	%r2,%r3
**	lgr	%r3,%r4
**	jg	take_pair@PLT
*/
void
fwd (long x, struct pair p)
{
  take_pair (p);
}

/* The PAIR does not fit into %r6 and goes to the stack, %r6 is taken by
   the next argument.  */

/*
** sixth:
**	lgr	%r2,%r6
**	br	%r14
*/
long
sixth (long a, long b, long c, long d, struct pair p, long e)
{
  return e;
}

/*
** on_stack:
**	lg	%r2,168\(%r15\)
**	br	%r14
*/
long
on_stack (long a, long b, long c, long d, struct pair p)
{
  return p.b;
}

/*
** last:
**	jg	take5@PLT
*/
void
last (long a, long b, long c, long d, struct pair p, long e)
{
  take5 (a, b, c, d, p, e);
}
