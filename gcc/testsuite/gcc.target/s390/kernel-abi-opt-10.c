/* { dg-do compile } */
/* { dg-options "-msoft-float -mbackchain -mexperimental-kernel-abi=no-rsa" } */
/* { dg-error ".no-rsa. in .-mexperimental-kernel-abi=. requires .-mpacked-stack. and .-mbackchain." "" { target *-*-* } 0 } */

int x;
