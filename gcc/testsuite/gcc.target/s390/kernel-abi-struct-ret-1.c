/* struct-ret: composites of up to 16 bytes are returned in %r2/%r3.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=struct-ret" } */
/* { dg-final { check-function-bodies "**" "" } } */

struct pair { long a, b; };
struct big { long a, b, c; };

extern struct pair get_pair (void);

/*
** mk_pair:
**	br	%r14
*/
struct pair
mk_pair (long a, long b)
{
  struct pair p = { a, b };
  return p;
}

/*
** fwd_pair:
**	jg	get_pair@PLT
*/
struct pair
fwd_pair (void)
{
  return get_pair ();
}

/*
** second:
**	stmg	%r14,%r15,136\(%r15\)
**	lgr	%r14,%r15
**	lay	%r15,-24\(%r15\)
**	stg	%r14,152\(%r15\)
**	brasl	%r14,get_pair@PLT
**	lmg	%r14,%r15,160\(%r15\)
**	lgr	%r2,%r3
**	br	%r14
*/
long
second (void)
{
  return get_pair ().b;
}

/*
** mk_cd:
**	br	%r14
*/
_Complex double
mk_cd (double re, double im)
{
  return __builtin_complex (re, im);
}

/*
** mk_cf:
**	sllg	%r2,%r2,32
**	algfr	%r2,%r3
**	br	%r14
*/
_Complex float
mk_cf (float re, float im)
{
  return __builtin_complex (re, im);
}

/* More than 16 bytes are still returned in memory.  */
/*
** mk_big:
**	stg	%r3,0\(%r2\)
**	stg	%r4,8\(%r2\)
**	stg	%r5,16\(%r2\)
**	br	%r14
*/
struct big
mk_big (long a, long b, long c)
{
  struct big s = { a, b, c };
  return s;
}
