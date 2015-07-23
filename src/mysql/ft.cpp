/***********************************************************************
 mysql/ft.cpp - Implements the MySQLFieldType class, as well as some
 	internal helper classes.

 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB,
 and © 2004-2007, 2014 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the
 CREDITS.txt file in the top directory of the distribution for details.

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

// This table maps C++ type information to MySQL type information, using
// the abstract libtabula type system as the bridge.
//
// The second half of the table parallels the first, to handle null-able
// versions of the types in the first half.  This is required because
// SQL's 'null' concept does not map neatly into the C++ type system, so
// null-able versions of these types have to have a different C++ type,
// implemented using the Null template.  See null.h for further details.
//
// The tuple of the MySQL C API type and the type flags (unsigned and
// null, currently) must be unique; that is to say, we do not refer to
// the underlying C API types redundantly.  Multiple such tuples may
// map to the same libtabula type where a distinctions made at the C
// API layer is ignored at the libtabula layer.  For example, libtabula
// has no special type for "UNSIGNED FLOAT"; it uses sql_float
// regardless of signedness.
//
// See the NativeToMySQLTypeMap class instance for the inverse of this
// table, mapping C++ types to C API types.
const MySQLFieldTypeInfo MySQLFieldType::types[] = {
	// Basic non-nullable type set
	MySQLFieldTypeInfo("DECIMAL NOT NULL", typeid(sql_decimal),
#if MYSQL_VERSION_ID >= 50001
			MYSQL_TYPE_NEWDECIMAL
#else
			MYSQL_TYPE_DECIMAL
#endif
			),
	MySQLFieldTypeInfo("TINYINT NOT NULL", typeid(sql_tinyint),
			MYSQL_TYPE_TINY),
	MySQLFieldTypeInfo("TINYINT UNSIGNED NOT NULL", typeid(sql_tinyint_unsigned),
			MYSQL_TYPE_TINY,
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("SMALLINT NOT NULL", typeid(sql_smallint),
			MYSQL_TYPE_SHORT),
	MySQLFieldTypeInfo("SMALLINT UNSIGNED NOT NULL", typeid(sql_smallint_unsigned),
			MYSQL_TYPE_SHORT,
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("INT NOT NULL", typeid(sql_int),
			MYSQL_TYPE_LONG),
	MySQLFieldTypeInfo("INT UNSIGNED NOT NULL", typeid(sql_int_unsigned),
			MYSQL_TYPE_LONG,
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("FLOAT NOT NULL", typeid(sql_float),
			MYSQL_TYPE_FLOAT),
	MySQLFieldTypeInfo("FLOAT UNSIGNED NOT NULL", typeid(sql_float),
			MYSQL_TYPE_FLOAT,
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("DOUBLE NOT NULL", typeid(sql_double),
			MYSQL_TYPE_DOUBLE),
	MySQLFieldTypeInfo("DOUBLE UNSIGNED NOT NULL", typeid(sql_double),
			MYSQL_TYPE_DOUBLE,
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("NULL NOT NULL", typeid(void),
			MYSQL_TYPE_NULL),
	MySQLFieldTypeInfo("TIMESTAMP NOT NULL", typeid(sql_timestamp),
			MYSQL_TYPE_TIMESTAMP),
	MySQLFieldTypeInfo("BIGINT NOT NULL", typeid(sql_bigint),
			MYSQL_TYPE_LONGLONG),
	MySQLFieldTypeInfo("BIGINT UNSIGNED NOT NULL", typeid(sql_bigint_unsigned),
			MYSQL_TYPE_LONGLONG,
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("MEDIUMINT NOT NULL", typeid(sql_mediumint),
			MYSQL_TYPE_INT24, MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("MEDIUMINT UNSIGNED NOT NULL", typeid(sql_mediumint_unsigned),
			MYSQL_TYPE_INT24, MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("DATE NOT NULL", typeid(sql_date),
			MYSQL_TYPE_DATE),
	MySQLFieldTypeInfo("TIME NOT NULL", typeid(sql_time),
			MYSQL_TYPE_TIME),
	MySQLFieldTypeInfo("DATETIME NOT NULL", typeid(sql_datetime),
			MYSQL_TYPE_DATETIME),
	MySQLFieldTypeInfo("ENUM NOT NULL", typeid(sql_enum),
			MYSQL_TYPE_ENUM),
	MySQLFieldTypeInfo("SET NOT NULL", typeid(sql_set),
			MYSQL_TYPE_SET),
	MySQLFieldTypeInfo("TINYBLOB NOT NULL", typeid(sql_tinyblob),
			MYSQL_TYPE_TINY_BLOB),
	MySQLFieldTypeInfo("MEDIUMBLOB NOT NULL", typeid(sql_mediumblob),
			MYSQL_TYPE_MEDIUM_BLOB),
	MySQLFieldTypeInfo("LONGBLOB NOT NULL", typeid(sql_longblob),
			MYSQL_TYPE_LONG_BLOB),
	MySQLFieldTypeInfo("BLOB NOT NULL", typeid(sql_blob),
			MYSQL_TYPE_BLOB),
	MySQLFieldTypeInfo("VARCHAR NOT NULL", typeid(sql_varchar),
			MYSQL_TYPE_VAR_STRING),
	MySQLFieldTypeInfo("CHAR NOT NULL", typeid(sql_char),
			MYSQL_TYPE_STRING),

	// Nullable versions of above
	MySQLFieldTypeInfo("DECIMAL NULL", typeid(Null<sql_decimal>),
#if MYSQL_VERSION_ID >= 50001
			MYSQL_TYPE_NEWDECIMAL
#else
			MYSQL_TYPE_DECIMAL
#endif
			, MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("TINYINT NULL", typeid(Null<sql_tinyint>),
			MYSQL_TYPE_TINY,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("TINYINT UNSIGNED NULL", typeid(Null<sql_tinyint_unsigned>),
			MYSQL_TYPE_TINY,
			MySQLFieldType::tf_null |
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("SMALLINT NULL", typeid(Null<sql_smallint>),
			MYSQL_TYPE_SHORT,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("SMALLINT UNSIGNED NULL", typeid(Null<sql_smallint_unsigned>),
			MYSQL_TYPE_SHORT,
			MySQLFieldType::tf_null |
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("INT NULL", typeid(Null<sql_int>),
			MYSQL_TYPE_LONG,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("INT UNSIGNED NULL", typeid(Null<sql_int_unsigned>),
			MYSQL_TYPE_LONG,
			MySQLFieldType::tf_null |
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("FLOAT NULL", typeid(Null<sql_float>),
			MYSQL_TYPE_FLOAT,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("FLOAT UNSIGNED NULL", typeid(Null<sql_float>),
			MYSQL_TYPE_FLOAT,
			MySQLFieldType::tf_null |
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("DOUBLE NULL", typeid(Null<sql_double>),
			MYSQL_TYPE_DOUBLE,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("DOUBLE UNSIGNED NULL", typeid(Null<sql_double>),
			MYSQL_TYPE_DOUBLE,
			MySQLFieldType::tf_null |
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("NULL NULL", typeid(Null<void>),
			MYSQL_TYPE_NULL, MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("TIMESTAMP NULL", typeid(Null<sql_timestamp>),
			MYSQL_TYPE_TIMESTAMP),
	MySQLFieldTypeInfo("BIGINT NULL", typeid(Null<sql_bigint>),
			MYSQL_TYPE_LONGLONG,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("BIGINT UNSIGNED NULL", typeid(Null<sql_bigint_unsigned>),
			MYSQL_TYPE_LONGLONG,
			MySQLFieldType::tf_null |
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("MEDIUMINT NULL", typeid(Null<sql_mediumint>),
			MYSQL_TYPE_INT24, MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("MEDIUMINT UNSIGNED NULL", typeid(Null<sql_mediumint_unsigned>), 
			MYSQL_TYPE_INT24, MySQLFieldType::tf_null |
			MySQLFieldType::tf_unsigned),
	MySQLFieldTypeInfo("DATE NULL", typeid(Null<sql_date>),
			MYSQL_TYPE_DATE,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("TIME NULL", typeid(Null<sql_time>),
			MYSQL_TYPE_TIME,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("DATETIME NULL", typeid(Null<sql_datetime>),
			MYSQL_TYPE_DATETIME,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("ENUM NULL", typeid(Null<sql_enum>),
			MYSQL_TYPE_ENUM,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("SET NULL", typeid(Null<sql_set>),
			MYSQL_TYPE_SET,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("TINYBLOB NULL", typeid(Null<sql_tinyblob>),
			MYSQL_TYPE_TINY_BLOB, MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("MEDIUMBLOB NULL", typeid(Null<sql_mediumblob>),
			MYSQL_TYPE_MEDIUM_BLOB, MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("LONGBLOB NULL", typeid(Null<sql_longblob>),
			MYSQL_TYPE_LONG_BLOB, MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("BLOB NULL", typeid(Null<sql_blob>),
			MYSQL_TYPE_BLOB,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("VARCHAR NULL", typeid(Null<sql_varchar>),
			MYSQL_TYPE_VAR_STRING,
			MySQLFieldType::tf_null),
	MySQLFieldTypeInfo("CHAR NULL", typeid(Null<sql_char>),
			MYSQL_TYPE_STRING, MySQLFieldType::tf_null)
};

const int MySQLFieldType::num_types =
		sizeof(MySQLFieldType::types) / sizeof(MySQLFieldType::types[0]);

const NativeToMySQLTypeMap MySQLFieldType::native_to_mysql_type_map(
		MySQLFieldType::types, MySQLFieldType::num_types);

#if !defined(DOXYGEN_IGNORE)
// Doxygen will not generate documentation for this section.

NativeToMySQLTypeMap::NativeToMySQLTypeMap(
		const MySQLFieldTypeInfo types[], const int size)
{
	for (int i = 0; i < size; ++i) {
		if (types[i].is_default()) {
			map_[types[i].c_type_] = i;
		}
	}
}

#endif // !defined(DOXYGEN_IGNORE)

MySQLFieldType::Base MySQLFieldType::base_type(enum_field_types t)
{
	switch (t) {
		case MYSQL_TYPE_NULL:
			return ft_null;

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
		case MYSQL_TYPE_TIME2:
			return ft_time;

		case MYSQL_TYPE_DATETIME:
		case MYSQL_TYPE_DATETIME2:
		case MYSQL_TYPE_TIMESTAMP:
		case MYSQL_TYPE_TIMESTAMP2:
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

		// TODO: Add C++ data types to the library to allow us to
		// represent these SQL data types.
		case MYSQL_TYPE_BIT:
		case MYSQL_TYPE_GEOMETRY:
			return ft_unsupported;

		// No default!  We want the compiler to warn us if the C API
		// adds another data type.
	}
}

bool MySQLFieldType::quote_q() const
{
	const type_info& ti = c_type();
	return	ti == typeid(string) ||
			ti == typeid(sql_date) ||
			ti == typeid(sql_time) ||
			ti == typeid(sql_datetime) ||
			ti == typeid(sql_blob) ||
			ti == typeid(sql_tinyblob) ||
			ti == typeid(sql_mediumblob) ||
			ti == typeid(sql_longblob) ||
			ti == typeid(sql_char) ||
			ti == typeid(sql_set);
}

bool MySQLFieldType::escape_q() const
{
	const type_info& ti = c_type();
	return	ti == typeid(string) ||
			ti == typeid(sql_enum) ||
			ti == typeid(sql_blob) ||
			ti == typeid(sql_tinyblob) ||
			ti == typeid(sql_mediumblob) ||
			ti == typeid(sql_longblob) ||
			ti == typeid(sql_char) ||
			ti == typeid(sql_varchar);
}

} // end namespace libtabula

