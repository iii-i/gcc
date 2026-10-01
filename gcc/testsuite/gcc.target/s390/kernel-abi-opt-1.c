/* { dg-do compile } */
/* { dg-options "-msoft-float -mexperimental-kernel-abi=struct-ret,bogus" } */
/* { dg-error "unknown keyword .bogus. in .-mexperimental-kernel-abi=." "" { target *-*-* } 0 } */

int x;
