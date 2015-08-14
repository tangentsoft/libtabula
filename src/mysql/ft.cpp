/***********************************************************************
 mysql/ft.cpp - Implements the MySQLFieldType class, as well as some
 	internal helper classes.

 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB,
 and © 2004-2007, 2014-2015 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the
 CREDITS.md file in the top directory of the distribution for details.

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

#include "common.h"
#include "ft.h"

#include "datetime.h"
#include "myset.h"
#include "sql_types.h"

#if defined(LIBTABULA_MYSQL_HEADERS_BURIED)
#	include <mysql/mysql.h>
#else
#	include <mysql.h>
#endif

#include <string>

using namespace std;

namespace libtabula {

MySQLFieldType::Base MySQLFieldType::base_type(enum_field_types t)
{
	switch (t) {
		case MYSQL_TYPE_TINY:
		case MYSQL_TYPE_SHORT:
		case MYSQL_TYPE_INT24:
		case MYSQL_TYPE_LONG:
		case MYSQL_TYPE_LONGLONG:
			return ft_integer;

		case MYSQL_TYPE_DECIMAL:
		case MYSQL_TYPE_NEWDECIMAL:
		case MYSQL_TYPE_FLOAT:
		case MYSQL_TYPE_DOUBLE:
			return ft_real;

		case MYSQL_TYPE_DATE:
#if MYSQL_VERSION_ID >= 50000
		case MYSQL_TYPE_NEWDATE:
#endif
		case MYSQL_TYPE_YEAR:
			return ft_date;

		case MYSQL_TYPE_TIME:
			return ft_time;

		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_TIMESTAMP:
			return ft_datetime;

		case MYSQL_TYPE_ENUM:
			return ft_enum;

		case MYSQL_TYPE_SET:
			return ft_set;

		case MYSQL_TYPE_BLOB:
		case MYSQL_TYPE_TINY_BLOB:
		case MYSQL_TYPE_MEDIUM_BLOB:
		case MYSQL_TYPE_LONG_BLOB:
			return ft_blob;

		case MYSQL_TYPE_VARCHAR:
		case MYSQL_TYPE_VAR_STRING:
		case MYSQL_TYPE_STRING:
			return ft_text;

		// MySQL 5.6+ compatibility types, never sent from the server to
		// us, but we have to handle them to avoid a compiler warning.
#if MYSQL_VERSION_ID >= 50600
		case MYSQL_TYPE_TIME2:
		case MYSQL_TYPE_DATETIME2:
		case MYSQL_TYPE_TIMESTAMP2:
			return ft_unsupported;
#endif

		// TODO: Add C++ data types to the library to allow us to
		// represent these SQL data types.
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_GEOMETRY:
		case MYSQL_TYPE_NULL:		// BS!
			return ft_unsupported;

		// No default!  We want the compiler to warn us if the C API
		// adds another data type.
	}
}

} // end namespace libtabula

