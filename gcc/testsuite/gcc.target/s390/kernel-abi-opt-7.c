/* { dg-do compile } */
/* { dg-options "-msoft-float -fsplit-stack -mexperimental-kernel-abi=r6-clobbered,r7-arg" } */
/* { dg-error ".r7-arg. in .-mexperimental-kernel-abi=. is not supported with .-fsplit-stack." "" { target *-*-* } 0 } */

int x;
