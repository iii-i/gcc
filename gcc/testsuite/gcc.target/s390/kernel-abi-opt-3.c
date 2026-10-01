/* The last -mexperimental-kernel-abi= wins, and each keyword in it
   defines a macro.  */

/* { dg-do compile } */
/* { dg-options "-msoft-float -mexperimental-kernel-abi=bogus -mexperimental-kernel-abi=struct-arg,struct-ret" } */

#if __S390_EXPERIMENTAL_KERNEL_ABI_STRUCT_RET__ != 1
#error
#endif

#if __S390_EXPERIMENTAL_KERNEL_ABI_STRUCT_ARG__ != 1
#error
#endif
