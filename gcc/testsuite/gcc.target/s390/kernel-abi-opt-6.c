/* { dg-do compile } */
/* { dg-options "-msoft-float -mexperimental-kernel-abi=r7-arg" } */
/* { dg-error ".r7-arg. in .-mexperimental-kernel-abi=. requires .r6-clobbered." "" { target *-*-* } 0 } */

int x;
