------------------------------------------------------------------------------
--                                                                          --
--                         GNAT COMPILER COMPONENTS                         --
--                                                                          --
--                      S Y S T E M . A R I T H _ 6 4                       --
--                                                                          --
--                                 S p e c                                  --
--                                                                          --
--          Copyright (C) 1992-2025, Free Software Foundation, Inc.         --
--                                                                          --
-- GNAT is free software;  you can  redistribute it  and/or modify it under --
-- terms of the  GNU General Public License as published  by the Free Soft- --
-- ware  Foundation;  either version 3,  or (at your option) any later ver- --
-- sion.  GNAT is distributed in the hope that it will be useful, but WITH- --
-- OUT ANY WARRANTY;  without even the  implied warranty of MERCHANTABILITY --
-- or FITNESS FOR A PARTICULAR PURPOSE.                                     --
--                                                                          --
-- As a special exception under Section 7 of GPL version 3, you are granted --
-- additional permissions described in the GCC Runtime Library Exception,   --
-- version 3.1, as published by the Free Software Foundation.               --
--                                                                          --
-- You should have received a copy of the GNU General Public License and    --
-- a copy of the GCC Runtime Library Exception along with this program;     --
-- see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see    --
-- <http://www.gnu.org/licenses/>.                                          --
--                                                                          --
-- GNAT was originally developed  by the GNAT team at  New York University. --
-- Extensive contributions were provided by Ada Core Technologies Inc.      --
--                                                                          --
------------------------------------------------------------------------------

--  This unit provides software routines for doing arithmetic on 64-bit
--  signed integer values in cases where either overflow checking is
--  required, or intermediate results are longer than 64 bits.

pragma Restrictions (No_Elaboration_Code);
--  Allow direct call from gigi generated code

--  Preconditions in this unit are meant for analysis only, not for run-time
--  checking, so that the expected exceptions are raised. This is enforced
--  by setting the corresponding assertion policy to Ignore. Postconditions
--  and contract cases should not be executed at runtime as well, in order
--  not to slow down the execution of these functions.

pragma Assertion_Policy (Pre            => Ignore,
                         Post           => Ignore,
                         Contract_Cases => Ignore,
                         Ghost          => Ignore);

with Ada.Numerics.Big_Numbers.Big_Integers_Ghost;
with Interfaces;

package System.Arith_64
  with Pure, SPARK_Mode
is
   use type Ada.Numerics.Big_Numbers.Big_Integers_Ghost.Big_Integer;
   use type Interfaces.Integer_64;

   subtype Int64 is Interfaces.Integer_64;

   subtype Big_Integer is
     Ada.Numerics.Big_Numbers.Big_Integers_Ghost.Big_Integer
   with Ghost;

   package Signed_Conversion is new
     Ada.Numerics.Big_Numbers.Big_Integers_Ghost.Signed_Conversions
     (Int => Int64);

   function Big (Arg : Int64) return Big_Integer is
     (Signed_Conversion.To_Big_Integer (Arg))
   with Ghost;

   function In_Int64_Range (Arg : Big_Integer) return Boolean is
     (Ada.Numerics.Big_Numbers.Big_Integers_Ghost.In_Range
       (Arg, Big (Int64'First), Big (Int64'Last)))
   with Ghost;

   function Add_With_Ovflo_Check64 (X, Y : Int64) return Int64
   with
     Pre  => In_Int64_Range (Big (X) + Big (Y)),
     Post => Add_With_Ovflo_Check64'Result = X + Y;
   --  Raises Constraint_Error if sum of operands overflows 64 bits,
   --  otherwise returns the 64-bit signed integer sum.
   --
   --  The sum of ``X`` and ``Y`` is first computed using wrap-around
   --  semantics.
   --
   --  If the sign of ``X`` and ``Y`` are opposed, no overflow is possible and
   --  the result is correct.
   --
   --  Otherwise, ``X`` and ``Y`` have the same sign; if the sign of the result
   --  is not identical to ``X`` (or ``Y``), then an overflow occurred and
   --  the exception *Constraint_Error* is raised; otherwise the result is
   --  correct.

   function Subtract_With_Ovflo_Check64 (X, Y : Int64) return Int64
   with
     Pre  => In_Int64_Range (Big (X) - Big (Y)),
     Post => Subtract_With_Ovflo_Check64'Result = X - Y;
   --  Raises Constraint_Error if difference of operands overflows 64
   --  bits, otherwise returns the 64-bit signed integer difference.
   --
   --  The logic of the implementation is reversed from *Add_With_Ovflo_Check*:
   --  if ``X`` and ``Y`` have the same sign, no overflow is checked, otherwise
   --  a sign of the result is compared with the sign of ``X`` to check for
   --  overflow.

   function Multiply_With_Ovflo_Check64 (X, Y : Int64) return Int64
   with
     Pre  => In_Int64_Range (Big (X) * Big (Y)),
     Post => Multiply_With_Ovflo_Check64'Result = X * Y;
   pragma Export (C, Multiply_With_Ovflo_Check64, "__gnat_mulv64");
   --  Raises Constraint_Error if product of operands overflows 64
   --  bits, otherwise returns the 64-bit signed integer product.
   --  The code generator may also generate direct calls to this routine.
   --
   --  The multiplication is done using pencil and paper algorithm using base
   --  2**32. The multiplication is done on unsigned values, then the correct
   --  signed value is returned. Overflow check is performed by looking at
   --  higher digits.

   function Same_Sign (X, Y : Big_Integer) return Boolean is
     (X = Big (Int64'(0))
        or else Y = Big (Int64'(0))
        or else (X < Big (Int64'(0))) = (Y < Big (Int64'(0))))
   with Ghost;

   function Round_Quotient (X, Y, Q, R : Big_Integer) return Big_Integer with
     Ghost,
     Pre  => Y /= 0 and then Q = X / Y and then R = X rem Y,
     Post => Round_Quotient'Result =
       (if abs R > (abs Y - Big (Int64'(1))) / Big (Int64'(2)) then
         (if Same_Sign (X, Y) then Q + Big (Int64'(1))
          else Q - Big (Int64'(1)))
        else
          Q);

   procedure Scaled_Divide64
     (X, Y, Z : Int64;
      Q, R    : out Int64;
      Round   : Boolean)
   with
     Pre  => Z /= 0
       and then In_Int64_Range
         (if Round then Round_Quotient (Big (X) * Big (Y), Big (Z),
                                        Big (X) * Big (Y) / Big (Z),
                                        Big (X) * Big (Y) rem Big (Z))
          else Big (X) * Big (Y) / Big (Z)),
     Post => Big (R) = Big (X) * Big (Y) rem Big (Z)
       and then
         (if Round then
            Big (Q) = Round_Quotient (Big (X) * Big (Y), Big (Z),
                                      Big (X) * Big (Y) / Big (Z), Big (R))
          else
            Big (Q) = Big (X) * Big (Y) / Big (Z));
   --  Performs the division of (``X`` * ``Y``) / ``Z``, storing the quotient
   --  in ``Q`` and the remainder in ``R``.
   --
   --  Constraint_Error is raised if ``Z`` is zero, or if the quotient does not
   --  fit in 64-bits.
   --
   --  ``Round`` indicates if the result should be rounded. If ``Round`` is
   --  False, then ``Q``, ``R`` are the normal quotient and remainder from a
   --  truncating division. If ``Round`` is True, then ``Q`` is the rounded
   --  quotient. The remainder ``R`` is not affected by the setting of the
   --  ``Round`` flag.
   --
   --  The multiplication is done using pencil and paper algorithm using base
   --  2**32. The multiplication is done on unsigned values. The result is a
   --  128 bit value.
   --
   --  The overflow is detected on the intermediate value.
   --
   --  If Z is a 32 bit value, the division is done using pencil and paper
   --  algorithm.
   --
   --  Otherwise, the division is performed using the algorithm D from section
   --  4.3.1 of "The Art of Computer Programming Vol. 2" [TACP2]. Rounding is
   --  applied on the result.
   --
   --  Finally, the sign is applied to the result and returned.

   procedure Scaled_Divide
     (X, Y, Z : Int64;
      Q, R    : out Int64;
      Round   : Boolean) renames Scaled_Divide64;
   --  Renamed procedure to preserve compatibility with earlier versions

   procedure Double_Divide64
     (X, Y, Z : Int64;
      Q, R    : out Int64;
      Round   : Boolean)
   with
     Pre  => Y /= 0
       and then Z /= 0
       and then In_Int64_Range
         (if Round then Round_Quotient (Big (X), Big (Y) * Big (Z),
                                        Big (X) / (Big (Y) * Big (Z)),
                                        Big (X) rem (Big (Y) * Big (Z)))
          else Big (X) / (Big (Y) * Big (Z))),
     Post => Big (R) = Big (X) rem (Big (Y) * Big (Z))
       and then
         (if Round then
            Big (Q) = Round_Quotient (Big (X), Big (Y) * Big (Z),
                                      Big (X) / (Big (Y) * Big (Z)), Big (R))
          else
            Big (Q) = Big (X) / (Big (Y) * Big (Z)));
   --  Performs the division ``X`` / (``Y`` * ``Z``), storing the quotient in
   --  ``Q`` and the remainder in ``R``. Constraint_Error is raised if ``Y`` or
   --  ``Z`` is zero, or if the quotient does not fit in 64-bits.
   --
   --  ``Round`` indicates if the result should be rounded. If ``Round`` is
   --  False, then ``Q``, ``R`` are the normal quotient and remainder from a
   --  truncating division. If ``Round`` is True, then ``Q`` is the rounded
   --  quotient. The remainder ``R`` is not affected by the setting of the
   --  ``Round`` flag.
   --
   --  Division by 0 is first detected.
   --
   --  The intermediate value ``Y`` * ``Z`` is then computed on 128 bits. The
   --  multiplication is done on unsigned values.
   --
   --  If the high 64 bits of the intermediate value is not 0, then 0 is
   --  returned. The overflow case of the largest negative number divided by
   --  -1 is detected here.
   --
   --  64-bit division is then performed, the result is rounded, its sign is
   --  corrected, and then returned.

   procedure Double_Divide
     (X, Y, Z : Int64;
      Q, R    : out Int64;
      Round   : Boolean) renames Double_Divide64;
   --  Renamed procedure to preserve compatibility with earlier versions

end System.Arith_64;
