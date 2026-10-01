/* r6-clobbered: a callee may clobber %r6 without restoring it.  */

/* { dg-do run } */
/* { dg-options "-O2 -march=z196 -msoft-float -mpacked-stack -mbackchain -mexperimental-kernel-abi=r6-clobbered" } */

extern void trash (void);

asm ("	.text\n"
     "	.globl	trash\n"
     "	.type	trash,@function\n"
     "trash:\n"
     "	lghi	%r0,-1\n"
     "	lghi	%r1,-1\n"
     "	lghi	%r2,-1\n"
     "	lghi	%r3,-1\n"
     "	lghi	%r4,-1\n"
     "	lghi	%r5,-1\n"
     "	lghi	%r6,-1\n"
     "	br	%r14\n"
     "	.size	trash,.-trash\n");

__attribute__ ((noipa)) long
f (long a, long b, long c, long d, long e)
{
  long s = 0;

  for (int i = 0; i < 3; i++)
    {
      trash ();
      s += a * 3 + b * 5 + c * 7 + d * 11 + e * 13;
    }
  return s;
}

int
main (void)
{
  if (f (1, 2, 3, 4, 5) != 3 * (3 + 10 + 21 + 44 + 65))
    __builtin_abort ();
  return 0;
}
