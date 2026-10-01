/* { dg-do compile } */
/* { dg-options "-msoft-float -mexperimental-kernel-abi=even-pairs,struct-ret" } */
/* { dg-error ".even-pairs. in .-mexperimental-kernel-abi=. requires one of .struct-arg,int128." "" { target *-*-* } 0 } */

int x;
