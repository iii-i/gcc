/* VMS support needed only by Fortran frontends.
   Copyright (C) 2012-2025 Free Software Foundation, Inc.

This file is part of GCC.

GCC is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 3, or (at your option)
any later version.

GCC is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GCC; see the file COPYING3.  If not see
<http://www.gnu.org/licenses/>.  */

#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "tm.h"

void
vms_c_register_includes (const char *sysroot ATTRIBUTE_UNUSED,
                         const char *iprefix ATTRIBUTE_UNUSED,
			 int stdinc ATTRIBUTE_UNUSED)
{
  /* No-op for fortran.  */
}
