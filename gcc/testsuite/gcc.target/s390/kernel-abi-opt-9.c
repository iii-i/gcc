/* { dg-do compile } */
/* { dg-options "-msoft-float -mexperimental-kernel-abi=struct-32,struct-arg" } */
/* { dg-error ".struct-32. in .-mexperimental-kernel-abi=. requires .struct-ret." "" { target *-*-* } 0 } */

int x;
