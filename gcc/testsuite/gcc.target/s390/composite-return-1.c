/* Check that composite values of up to 16 bytes are returned in %r2 and
   %r3 with -freg-struct-return, and that the value is placed in the
   registers the same way as it would be placed in an argument register.
   Composing, forwarding, and picking apart such a value therefore does
   not require any shifting.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z13 -freg-struct-return" } */
/* { dg-final { check-function-bodies "**" "" } } */

struct s4 { int a; };
struct s12 { int a, b, c; };
struct s16 { long a, b; };
struct s17 { long a, b; char c; };

extern struct s4 get4 (void);
extern struct s16 get16 (void);
extern void take4 (struct s4);

/*
** mk4:
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
**	br	%r14
*/
struct s16
mk16 (long a, long b)
{
  struct s16 s = { a, b };

  return s;
}

/*
** fwd16:
**	jg	get16@PLT
*/
struct s16
fwd16 (void)
{
  return get16 ();
}

/* The second word of the value is returned in %r3.  */
/*
** use16:
**	stmg	%r14,%r15,112\(%r15\)
**	lay	%r15,-160\(%r15\)
**	brasl	%r14,get16@PLT
**	lmg	%r14,%r15,272\(%r15\)
**	lgr	%r2,%r3
**	br	%r14
*/
long
use16 (void)
{
  return get16 ().b;
}

/* A composite returned in %r2 is already in the right place for being
   passed as an argument.  */
/*
** pass4:
**	stmg	%r14,%r15,112\(%r15\)
**	lay	%r15,-160\(%r15\)
**	brasl	%r14,get4@PLT
**	lmg	%r14,%r15,272\(%r15\)
**	jg	take4@PLT
*/
void
pass4 (void)
{
  take4 (get4 ());
}

/* Composites of more than 16 bytes keep being returned in a buffer whose
   address the caller passes in %r2.  */
/*
** mk17:
**	vlvgp	%v0,%r3,%r4
**	mvi	16\(%r2\),0
**	vst	%v0,0\(%r2\),3
**	br	%r14
*/
struct s17
mk17 (long a, long b)
{
  struct s17 s = { a, b, 0 };

  return s;
}

/* Argument passing is not affected: composites of a size other than 1, 2,
   4, or 8 bytes are still passed by reference.  */
/*
** arg12:
**	lgf	%r2,0\(%r2\)
**	br	%r14
*/
int
arg12 (struct s12 s)
{
  return s.a;
}
