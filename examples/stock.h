/***********************************************************************
 stock.h - Declares the stock SSQLS used by several of the examples.

 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB, and
 © 2004-2010 by Educational Technology Resources, Inc.  Others may
 also hold copyrights on code in this file.  See the CREDITS.md file
 in the top directory of the distribution for details.

 This file is part of libtabula.

 libtabula is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.

 libtabula is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with libtabula; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
 USA
***********************************************************************/

#include <libtabula.h>
#include <ssqls.h>

// The following is calling a very complex macro which will create
// "struct stock", which has the member variables:
//
//   sql_char item;
//   ...
//   sql_mediumtext_null description;
//
// plus methods to help populate the class from a libtabula row.  See the
// SSQLS sections in the user manual for further details.
sql_create_6(stock,
	1, 6, // The meaning of these values is covered in the user manual
	libtabula::sql_char, item,
	libtabula::sql_bigint, num,
	libtabula::sql_double, weight,
	libtabula::sql_double_null, price,
	libtabula::sql_date, sDate,			// SSQLS isn't case-sensitive!
	libtabula::sql_mediumtext_null, description)

