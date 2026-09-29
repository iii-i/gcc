/* Check that with -freg-struct-return a composite value is returned in the
   register it would be passed in as the first argument, and in memory if
   it would be passed by reference.  */

/* { dg-do compile } */
/* { dg-options "-O2 -march=z13 -freg-struct-return" } */
/* { dg-final { check-function-bodies "**" "" } } */

struct s4 { int a; };
struct s8 { int x; char y; };
struct s12 { int a, b, c; };
struct s16 { long a, b; };
struct d1 { double a; };
struct f1 { float a; };
struct fam { long a; int b[]; };
struct empty { };

extern struct s4 get4 (void);
extern struct s8 get8 (void);
extern struct d1 getd (void);
extern void take4 (struct s4);
extern void taked (struct d1);

/*
** mk4:
**	br	%r14
*/
struct s4
mk4 (struct s4 s)
{
  return s;
}

/*
** mk8:
**	br	%r14
*/
struct s8
mk8 (struct s8 s)
{
  return s;
}

/*
** get8y:
**	stmg	%r14,%r15,112\(%r15\)
**	lay	%r15,-160\(%r15\)
**	brasl	%r14,get8@PLT
**	lmg	%r14,%r15,272\(%r15\)
**	risbgn	%r2,%r2,64-8,128\+63,32\+8
**	br	%r14
*/
int
get8y (void)
{
  return get8 ().y;
}

/*
** mkd:
**	br	%r14
*/
struct d1
mkd (double a)
{
  struct d1 s = { a };

  return s;
}

/*
** mkf:
**	br	%r14
*/
struct f1
mkf (float a)
{
  struct f1 s = { a };

  return s;
}

/*
** usedf:
**	stmg	%r14,%r15,112\(%r15\)
**	lay	%r15,-160\(%r15\)
**	brasl	%r14,getd@PLT
**	lmg	%r14,%r15,272\(%r15\)
**	br	%r14
*/
double
usedf (void)
{
  return getd ().a;
}

/* The value returned by one function is in the right place for being
   passed on to another one.  */
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

/*
** passd:
**	stmg	%r14,%r15,112\(%r15\)
**	lay	%r15,-160\(%r15\)
**	brasl	%r14,getd@PLT
**	lmg	%r14,%r15,272\(%r15\)
**	jg	taked@PLT
*/
void
passd (void)
{
  taked (getd ());
}

/* Composites which are passed by reference are returned in memory.  */
/*
** mk12:
**	st	%r5,8\(%r2\)
**	...
**	std	%v0,0\(%r2\)
**	br	%r14
*/
struct s12
mk12 (int a, int b, int c)
{
  struct s12 s = { a, b, c };

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
** mkfam:
**	stg	%r3,0\(%r2\)
**	br	%r14
*/
struct fam
mkfam (long a)
{
  struct fam s = { a };

  return s;
}

/*
** mkempty:
**	br	%r14
*/
struct empty
mkempty (void)
{
  struct empty e;

  return e;
}
