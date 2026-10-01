/* { dg-do compile } */
/* { dg-options "-msoft-float -mpacked-stack -mbackchain -fsplit-stack -mexperimental-kernel-abi=no-rsa" } */
/* { dg-error ".no-rsa. in .-mexperimental-kernel-abi=. is not supported with .-fsplit-stack." "" { target *-*-* } 0 } */

int x;
