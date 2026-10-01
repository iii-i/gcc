/* struct-ret: composites of up to 16 bytes are returned in %r2/%r3.  */

/* { dg-do run } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=struct-ret" } */

#include "kernel-abi-run.h"

typedef struct { unsigned char c[1]; } b1;
typedef struct { unsigned char c[2]; } b2;
typedef struct { unsigned char c[3]; } b3;
typedef struct { unsigned char c[4]; } b4;
typedef struct { unsigned char c[5]; } b5;
typedef struct { unsigned char c[6]; } b6;
typedef struct { unsigned char c[7]; } b7;
typedef struct { unsigned char c[8]; } b8;
typedef struct { unsigned char c[9]; } b9;
typedef struct { unsigned char c[10]; } b10;
typedef struct { unsigned char c[11]; } b11;
typedef struct { unsigned char c[12]; } b12;
typedef struct { unsigned char c[13]; } b13;
typedef struct { unsigned char c[14]; } b14;
typedef struct { unsigned char c[15]; } b15;
typedef struct { unsigned char c[16]; } b16;
typedef struct { unsigned int a, b, c; } i3;
typedef struct { unsigned long a, b; } l2;
typedef struct { unsigned short a; unsigned char b; } __attribute__ ((packed)) p3;
typedef struct { unsigned long a; unsigned int b; } __attribute__ ((packed)) p12;
typedef union { unsigned int a; unsigned char b[6]; } __attribute__ ((packed)) u6;
typedef unsigned char a7[7];
typedef struct { a7 x; } wa7;
typedef _Complex char cc;
typedef _Complex short cs;
typedef _Complex int ci;
typedef _Complex long cl;
typedef _Complex float cf;
typedef _Complex double cd;
typedef struct { _Complex float f; } scf;

#define TEST(T)								\
  __attribute__ ((noipa)) T						\
  ret_##T (void)							\
  {									\
    T v;								\
    __builtin_memcpy (&v, kabi_pattern, sizeof (T));			\
    return v;								\
  }									\
									\
  static void								\
  check_##T (void)							\
  {									\
    T v;								\
									\
    kabi_clear ();							\
    kabi_invoke (ret_##T);						\
    kabi_check_image (kabi_ret, kabi_pattern, sizeof (T));		\
									\
    kabi_clear ();							\
    kabi_image (kabi_pattern, sizeof (T), kabi_ret);			\
    v = ((T (*) (void)) (void *) kabi_dump) ();				\
    KABI_CHECK (__builtin_memcmp (&v, kabi_pattern, sizeof (T)) == 0);	\
  }

TEST (b1) TEST (b2) TEST (b3) TEST (b4) TEST (b5) TEST (b6) TEST (b7)
TEST (b8) TEST (b9) TEST (b10) TEST (b11) TEST (b12) TEST (b13) TEST (b14)
TEST (b15) TEST (b16) TEST (i3) TEST (l2) TEST (p3) TEST (p12) TEST (u6)
TEST (wa7) TEST (cc) TEST (cs) TEST (ci) TEST (cl) TEST (cf) TEST (cd)
TEST (scf)

/* The examples from the specification, with padding.  */

struct ex1 { int a; char b; };
struct ex2 { long a; int b; };
struct ex3 { long n; int d[]; };

__attribute__ ((noipa)) struct ex1
mk_ex1 (void)
{
  return (struct ex1) { 0x12345678, 0x9a };
}

__attribute__ ((noipa)) struct ex2
mk_ex2 (void)
{
  return (struct ex2) { 0x1122334455667788L, 0x13579bdf };
}

__attribute__ ((noipa)) struct ex3
mk_ex3 (void)
{
  return (struct ex3) { 0x0123456789abcdefL };
}

/* No register and no buffer: a caller which expects one would pass its
   address in %r2, and the callee would store through it.  */

struct empty { };

__attribute__ ((noipa)) struct empty
mk_empty (void)
{
  struct empty e;

  return e;
}

static void
check_examples (void)
{
  struct ex1 e1;
  struct ex2 e2;
  struct ex3 e3;

  kabi_clear ();
  kabi_invoke (mk_ex1);
  KABI_CHECK (kabi_ret[0] >> 24 == 0x123456789aUL);

  kabi_clear ();
  kabi_invoke (mk_ex2);
  KABI_CHECK (kabi_ret[0] == 0x1122334455667788UL);
  KABI_CHECK (kabi_ret[1] >> 32 == 0x13579bdfUL);

  kabi_clear ();
  kabi_invoke (mk_ex3);
  KABI_CHECK (kabi_ret[0] == 0x0123456789abcdefUL);

  kabi_clear ();
  kabi_ret[0] = 0xfedcba9876543210UL;
  e1 = ((struct ex1 (*) (void)) (void *) kabi_dump) ();
  KABI_CHECK (e1.a == (int) 0xfedcba98 && e1.b == 0x76);

  kabi_clear ();
  kabi_ret[0] = 0x0011223344556677UL;
  kabi_ret[1] = 0x8899aabbccddeeffUL;
  e2 = ((struct ex2 (*) (void)) (void *) kabi_dump) ();
  KABI_CHECK (e2.a == 0x0011223344556677L && e2.b == (int) 0x8899aabb);

  kabi_clear ();
  kabi_ret[0] = 0x0123456789abcdefUL;
  e3 = ((struct ex3 (*) (void)) (void *) kabi_dump) ();
  KABI_CHECK (e3.n == 0x0123456789abcdefL);

  kabi_clear ();
  kabi_regs[0] = (unsigned long) &kabi_stack[0];
  kabi_invoke (mk_empty);
  KABI_CHECK (kabi_stack[0] == KABI_JUNK);
}

int
main (void)
{
  check_b1 (); check_b2 (); check_b3 (); check_b4 (); check_b5 ();
  check_b6 (); check_b7 (); check_b8 (); check_b9 (); check_b10 ();
  check_b11 (); check_b12 (); check_b13 (); check_b14 (); check_b15 ();
  check_b16 (); check_i3 (); check_l2 (); check_p3 (); check_p12 ();
  check_u6 (); check_wa7 (); check_cc (); check_cs (); check_ci ();
  check_cl (); check_cf (); check_cd (); check_scf ();
  check_examples ();
  return 0;
}
