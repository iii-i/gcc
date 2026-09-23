/* Check that composite values of various shapes survive a round trip
   through the return value registers with -freg-struct-return.
   Included by composite-return-run-2.c and composite-return-run-3.c,
   which build this at different optimization levels.  */

extern void abort (void);

static const unsigned char pattern[20] =
  {
    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
    0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff, 0x01,
    0x02, 0x03, 0x04, 0x05
  };

/* Types without padding: every byte belongs to the value, hence the whole
   object can be compared.  The call goes through a volatile function
   pointer, so that the compiler cannot bypass the calling convention.  */
#define TEST_TYPE(TYPE, NAME)						\
  TYPE									\
  NAME (void)								\
  {									\
    TYPE v;								\
    __builtin_memcpy (&v, pattern, sizeof (TYPE));			\
    return v;								\
  }									\
									\
  TYPE (* volatile NAME##_p) (void) = NAME;				\
									\
  static void								\
  check_##NAME (void)							\
  {									\
    TYPE v = NAME##_p ();						\
									\
    if (__builtin_memcmp (&v, pattern, sizeof (TYPE)) != 0)		\
      abort ();								\
  }

struct b1 { unsigned char a[1]; };
struct b2 { unsigned char a[2]; };
struct b3 { unsigned char a[3]; };
struct b5 { unsigned char a[5]; };
struct b7 { unsigned char a[7]; };
struct b8 { unsigned char a[8]; };
struct b9 { unsigned char a[9]; };
struct b11 { unsigned char a[11]; };
struct b15 { unsigned char a[15]; };
struct b16 { unsigned char a[16]; };
struct ints { unsigned int a, b, c; };
struct longs { unsigned long a, b; };
struct dbl { double a; };
struct flt { float a, b; };
struct ldbl { long double a; };
struct cplx { _Complex float a; };
struct nested { struct b3 a; struct b2 b; unsigned char c; };
struct packed { unsigned long a; unsigned int b; } __attribute__ ((packed));
union un { unsigned long a; unsigned char b[8]; };

TEST_TYPE (struct b1, mk_b1)
TEST_TYPE (struct b2, mk_b2)
TEST_TYPE (struct b3, mk_b3)
TEST_TYPE (struct b5, mk_b5)
TEST_TYPE (struct b7, mk_b7)
TEST_TYPE (struct b8, mk_b8)
TEST_TYPE (struct b9, mk_b9)
TEST_TYPE (struct b11, mk_b11)
TEST_TYPE (struct b15, mk_b15)
TEST_TYPE (struct b16, mk_b16)
TEST_TYPE (struct ints, mk_ints)
TEST_TYPE (struct longs, mk_longs)
TEST_TYPE (struct dbl, mk_dbl)
TEST_TYPE (struct flt, mk_flt)
TEST_TYPE (struct ldbl, mk_ldbl)
TEST_TYPE (struct cplx, mk_cplx)
TEST_TYPE (struct nested, mk_nested)
TEST_TYPE (struct packed, mk_packed)
TEST_TYPE (union un, mk_un)

/* A type with padding.  Only the members are checked, the padding bytes
   are not part of the value.  */
struct padded { unsigned char a; unsigned long b; };

struct padded
mk_padded (void)
{
  struct padded v = { 0x11, 0x2233445566778899UL };

  return v;
}

struct padded (* volatile mk_padded_p) (void) = mk_padded;

static void
check_mk_padded (void)
{
  struct padded v = mk_padded_p ();

  if (v.a != 0x11 || v.b != 0x2233445566778899UL)
    abort ();
}

/* Composites which are too large keep being returned in memory.  */
struct b17 { unsigned char a[17]; };

TEST_TYPE (struct b17, mk_b17)

int
main (void)
{
  check_mk_b1 ();
  check_mk_b2 ();
  check_mk_b3 ();
  check_mk_b5 ();
  check_mk_b7 ();
  check_mk_b8 ();
  check_mk_b9 ();
  check_mk_b11 ();
  check_mk_b15 ();
  check_mk_b16 ();
  check_mk_ints ();
  check_mk_longs ();
  check_mk_dbl ();
  check_mk_flt ();
  check_mk_ldbl ();
  check_mk_cplx ();
  check_mk_nested ();
  check_mk_packed ();
  check_mk_un ();
  check_mk_padded ();
  check_mk_b17 ();

  return 0;
}
