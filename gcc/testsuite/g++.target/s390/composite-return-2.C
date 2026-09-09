// The C++ ABI requires the decimal classes from ISO/IEC TR 24733 to be
// passed and returned like the decimal scalar type they wrap.  This is
// not affected by -freg-struct-return.

// { dg-do compile { target dfp } }
// { dg-options "-O2 -march=z13 -fno-section-anchors -freg-struct-return" }
// { dg-final { check-function-bodies "**" "" } }

namespace std {
namespace decimal {

class decimal32
{
public:
  typedef float __dec32 __attribute__ ((mode (SD)));
  __dec32 __val;
};

class decimal64
{
public:
  typedef float __dec64 __attribute__ ((mode (DD)));
  __dec64 __val;
};

}
}

std::decimal::decimal32 g32;
std::decimal::decimal64 g64;

extern "C" {

/*
** get32:
**	larl	%r1,g32
**	lde	%f0,0\(%r1\)
**	br	%r14
*/
std::decimal::decimal32
get32 (void)
{
  return g32;
}

/*
** get64:
**	larl	%r1,g64
**	ld	%f0,0\(%r1\)
**	br	%r14
*/
std::decimal::decimal64
get64 (void)
{
  return g64;
}

}
