// Check that -freg-struct-return only affects classes which may be
// returned in registers at all.  A class with a non-trivial copy
// constructor has to be returned in memory.

// { dg-do compile }
// { dg-options "-O2 -march=z13 -freg-struct-return" }
// { dg-final { check-function-bodies "**" "" } }

struct triv
{
  long a, b;
};

struct nontriv
{
  long a, b;

  nontriv () { }
  nontriv (const nontriv &);
};

extern "C" {

/*
** mk_triv:
**	br	%r14
*/
triv
mk_triv (long a, long b)
{
  triv t = { a, b };

  return t;
}

/*
** mk_nontriv:
**	vlvgp	%v0,%r3,%r4
**	vst	%v0,0\(%r2\),3
**	br	%r14
*/
nontriv
mk_nontriv (long a, long b)
{
  nontriv t;

  t.a = a;
  t.b = b;

  return t;
}

extern triv get_triv (void);
extern nontriv get_nontriv (void);

/*
** use_triv:
**	stmg	%r14,%r15,112\(%r15\)
**	lay	%r15,-160\(%r15\)
**	brasl	%r14,get_triv@PLT
**	lmg	%r14,%r15,272\(%r15\)
**	lgr	%r2,%r3
**	br	%r14
*/
long
use_triv (void)
{
  return get_triv ().b;
}

/*
** use_nontriv:
**	stmg	%r14,%r15,112\(%r15\)
**	lay	%r15,-176\(%r15\)
**	la	%r2,160\(%r15\)
**	brasl	%r14,get_nontriv@PLT
**	lg	%r2,168\(%r15\)
**	lmg	%r14,%r15,288\(%r15\)
**	br	%r14
*/
long
use_nontriv (void)
{
  return get_nontriv ().b;
}

}
