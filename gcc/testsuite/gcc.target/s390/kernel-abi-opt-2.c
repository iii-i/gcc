/* { dg-do compile } */
/* { dg-options "-mhard-float -mexperimental-kernel-abi=struct-ret" } */
/* { dg-error ".-mexperimental-kernel-abi=. requires .-msoft-float." "" { target *-*-* } 0 } */

int x;
