/* An empty list selects the ELF ABI.  */

/* { dg-do compile } */
/* { dg-options "-O2 -msoft-float -mexperimental-kernel-abi=" } */
/* { dg-final { scan-assembler "stg\t%r5,8\\(%r2\\)" } } */

#ifdef __S390_EXPERIMENTAL_KERNEL_ABI_STRUCT_RET__
#error
#endif

struct pair { long a, b; };

struct pair
f (struct pair *p)
{
  return *p;
}
