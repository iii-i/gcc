// An empty C++ class has a size of 1 and is passed and returned in a
// register; a class passed by invisible reference keeps that.

// { dg-do compile }
// { dg-options "-O2 -march=z196 -msoft-float -mexperimental-kernel-abi=struct-ret,struct-arg" }
// { dg-final { check-function-bodies "**" "" } }

struct E { };
struct NT { long a, b; NT (const NT &); };
struct P { long a, b; };

/*
** _Z5afterl1El:
**	lgr	%r2,%r4
**	br	%r14
*/
long
after (long, E, long x)
{
  return x;
}

/*
** _Z3get2NT:
**	lg	%r2,8\(%r2\)
**	br	%r14
*/
long
get (NT n)
{
  return n.b;
}

/*
** _Z2id1P:
**	br	%r14
*/
P
id (P p)
{
  return p;
}
