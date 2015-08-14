/***********************************************************************
 printdata.h - Declares utility routines for printing out data in
	common forms, used by most of the example programs.

 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB, and
 © 2004-2009 by Educational Technology Resources, Inc.  Others may
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

#if !defined(LIBTABULA_PRINTDATA_H)
#define LIBTABULA_PRINTDATA_H

#include <libtabula.h>

void print_stock_header(size_t rows);
void print_stock_row(const libtabula::Row& r);
void print_stock_row(const libtabula::sql_char& item,
		libtabula::sql_bigint num, libtabula::sql_double weight,
		libtabula::sql_decimal_null price, const libtabula::sql_date& date);
void print_stock_rows(libtabula::StoreQueryResult& res);
void print_stock_table(libtabula::Query& query);

#endif // !defined(LIBTABULA_PRINTDATA_H)

