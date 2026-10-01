/* struct-32: composites of 17 to 32 bytes are passed and returned in three
   or four GPRs.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=struct-ret,struct-arg,struct-32" } */
/* { dg-final { check-function-bodies "**" "" } } */

struct s24 { long a, b, c; };
struct s32 { long a, b, c, d; };
struct s33 { char c[33]; };

extern struct s32 get32 (void);

/*
** fourth:
**	...
**	brasl	%r14,get32@PLT
**	lmg	%r14,%r15,192\(%r15\)
**	lgr	%r2,%r5
**	br	%r14
*/
long
fourth (void)
{
  return get32 ().d;
}

/* A composite which does not fit leaves the remaining registers to later
   arguments.  */
/*
** after:
**	lgr	%r2,%r5
**	br	%r14
*/
long
after (long a, long b, long c, struct s24 x, long d)
{
  return d;
}

/*
** on_stack:
**	lg	%r2,176\(%r15\)
**	br	%r14
*/
long
on_stack (long a, long b, long c, struct s24 x, long d)
{
  return x.c;
}

/* More than 32 bytes still go through memory.  */
/*
** big:
**	mvc	0\(33,%r2\),0\(%r3\)
**	br	%r14
*/
struct s33
big (struct s33 x)
{
  return x;
}

/*
** cld:
**	br	%r14
*/
_Complex long double
cld (_Complex long double x)
{
  return x;
}
