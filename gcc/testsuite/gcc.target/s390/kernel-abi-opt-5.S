/* The macros are defined for assembler-with-cpp as well.  */

/* { dg-do assemble } */
/* { dg-options "-msoft-float -mexperimental-kernel-abi=struct-ret" } */

#if __S390_EXPERIMENTAL_KERNEL_ABI_STRUCT_RET__ != 1
#error
#endif
