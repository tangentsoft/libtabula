/***********************************************************************
 field_type.cpp - Implements the FieldType class, as well as some
 	internal helper classes.

 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB,
 and © 2004-2007, 2014-2015 by Educational Technology Resources, Inc.
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
#include "field_type.h"

#include "datetime.h"
#include "myset.h"
#include "sql_types.h"

#include <string>

using namespace std;

namespace libtabula {

// This table maps generic C++ type information to libtabula specific
// type information.  It is part of the transformation that allows
// plain C++ data types to map to driver-specific data types.
//
// The second half of the table parallels the first, to handle null-able
// versions of the types in the first half.  This is required because
// SQL's 'null' concept does not map neatly into the C++ type system, so
// null-able versions of these types have to have a different C++ type,
// implemented using the Null template.  See null.h for further details.
//
// This mapping is lossy.  There is one entry for every \c sql_* data type
// defined in libtabula, but it maps to a non-unique {Base, Flag} pair.
// 
// Because the converse is also true -- a given {Base, Flag} pair could
// map to any of several libtabula data types -- we have the notion of a
// "best" conversion, which yields the most likely C++ data type for a
// given pair.  This lossiness matters most with numeric data types,
// where there may be multiple sizes.  We use the largest "native" size.
// 
// This table does not necessarily honor fine type distinctions made at
// the DBMS layer.  For example, we paper over the UNSIGNED FLOAT mess.
//
// This table is inverted for reverse lookup as the static type_map_
// object.
typedef FieldType::TypeInfo FTTI;
const FTTI FieldType::types_[] = {
	// Basic non-nullable type set
	FTTI("TINYINT NOT NULL", typeid(sql_tinyint),
			FieldType::ft_integer, FieldType::tf_default, false),
	FTTI("TINYINT UNSIGNED NOT NULL", typeid(sql_tinyint_unsigned),
			FieldType::ft_integer, FieldType::tf_unsigned, false),
	FTTI("SMALLINT NOT NULL", typeid(sql_smallint),
			FieldType::ft_integer, FieldType::tf_default, false),
	FTTI("SMALLINT UNSIGNED NOT NULL", typeid(sql_smallint_unsigned),
			FieldType::ft_integer, FieldType::tf_unsigned, false),
	FTTI("MEDIUMINT NOT NULL", typeid(sql_mediumint),
			FieldType::ft_integer, FieldType::tf_unsigned, false),
	FTTI("MEDIUMINT UNSIGNED NOT NULL", typeid(sql_mediumint_unsigned),
			FieldType::ft_integer, FieldType::tf_unsigned, false),
	FTTI("INT NOT NULL", typeid(sql_int), FieldType::ft_integer),
	FTTI("INT UNSIGNED NOT NULL", typeid(sql_int_unsigned),
			FieldType::ft_integer, FieldType::tf_unsigned),
	FTTI("BIGINT NOT NULL", typeid(sql_bigint), 
			FieldType::ft_integer, FieldType::tf_default, false),
	FTTI("BIGINT UNSIGNED NOT NULL", typeid(sql_bigint_unsigned),
			FieldType::ft_integer, FieldType::tf_unsigned, false),
	FTTI("FLOAT NOT NULL", typeid(sql_float), 
			FieldType::ft_real, FieldType::tf_default, false),
	FTTI("FLOAT UNSIGNED NOT NULL", typeid(sql_float),
			FieldType::ft_real, FieldType::tf_unsigned, false),
	FTTI("DOUBLE NOT NULL", typeid(sql_double), FieldType::ft_real),
	FTTI("DOUBLE UNSIGNED NOT NULL", typeid(sql_double),
			FieldType::ft_real, FieldType::tf_unsigned),
	FTTI("DECIMAL NOT NULL", typeid(sql_decimal), FieldType::ft_decimal),
	FTTI("TIMESTAMP NOT NULL", typeid(sql_timestamp), FieldType::ft_timestamp),
	FTTI("DATE NOT NULL", typeid(sql_date), FieldType::ft_date),
	FTTI("TIME NOT NULL", typeid(sql_time), FieldType::ft_time),
	FTTI("DATETIME NOT NULL", typeid(sql_datetime), FieldType::ft_datetime),
	FTTI("ENUM NOT NULL", typeid(sql_enum), FieldType::ft_enum),
	FTTI("SET NOT NULL", typeid(sql_set), FieldType::ft_set),
	FTTI("TINYBLOB NOT NULL", typeid(sql_tinyblob),
			FieldType::ft_blob, FieldType::tf_default, false),
	FTTI("MEDIUMBLOB NOT NULL", typeid(sql_mediumblob), 
			FieldType::ft_blob, FieldType::tf_default, false),
	FTTI("LONGBLOB NOT NULL", typeid(sql_longblob),
			FieldType::ft_blob, FieldType::tf_default, false),
	FTTI("BLOB NOT NULL", typeid(sql_blob), FieldType::ft_blob),
	FTTI("VARCHAR NOT NULL", typeid(sql_varchar), FieldType::ft_text),
	FTTI("CHAR NOT NULL", typeid(sql_char), 
			FieldType::ft_text, FieldType::tf_default, false),
	FTTI("NULL NOT NULL", typeid(void),
			FieldType::ft_null, FieldType::tf_default),

	// Nullable versions of above
	FTTI("TINYINT NULL", typeid(Null<sql_tinyint>),
			FieldType::ft_integer, FieldType::tf_null, false),
	FTTI("TINYINT UNSIGNED NULL", typeid(Null<sql_tinyint_unsigned>),
			FieldType::ft_integer,
			FieldType::tf_null | FieldType::tf_unsigned, false),
	FTTI("SMALLINT NULL", typeid(Null<sql_smallint>),
			FieldType::ft_integer, FieldType::tf_null, false),
	FTTI("SMALLINT UNSIGNED NULL", typeid(Null<sql_smallint_unsigned>),
			FieldType::ft_integer,
			FieldType::tf_null | FieldType::tf_unsigned, false),
	FTTI("MEDIUMINT NULL", typeid(Null<sql_mediumint>),
			FieldType::ft_integer, FieldType::tf_null, false),
	FTTI("MEDIUMINT UNSIGNED NULL", typeid(Null<sql_mediumint_unsigned>), 
			FieldType::ft_integer,
			FieldType::tf_null | FieldType::tf_unsigned, false),
	FTTI("INT NULL", typeid(Null<sql_int>),
			FieldType::ft_integer, FieldType::tf_null),
	FTTI("INT UNSIGNED NULL", typeid(Null<sql_int_unsigned>),
			FieldType::ft_integer, FieldType::tf_null | FieldType::tf_unsigned),
	FTTI("BIGINT NULL", typeid(Null<sql_bigint>),
			FieldType::ft_integer, FieldType::tf_null, false),
	FTTI("BIGINT UNSIGNED NULL", typeid(Null<sql_bigint_unsigned>),
			FieldType::ft_integer,
			FieldType::tf_null | FieldType::tf_unsigned, false),
	FTTI("FLOAT NULL", typeid(Null<sql_float>),
			FieldType::ft_real, FieldType::tf_null, false),
	FTTI("FLOAT UNSIGNED NULL", typeid(Null<sql_float>),
			FieldType::ft_real,
			FieldType::tf_null | FieldType::tf_unsigned, false),
	FTTI("DOUBLE NULL", typeid(Null<sql_double>),
			FieldType::ft_real, FieldType::tf_null),
	FTTI("DOUBLE UNSIGNED NULL", typeid(Null<sql_double>),
			FieldType::ft_real, FieldType::tf_null | FieldType::tf_unsigned),
	FTTI("DECIMAL NULL", typeid(Null<sql_decimal>),
			FieldType::ft_decimal, FieldType::tf_null),
	FTTI("TIMESTAMP NULL", typeid(Null<sql_timestamp>),
			FieldType::ft_timestamp),
	FTTI("DATE NULL", typeid(Null<sql_date>),
			FieldType::ft_date, FieldType::tf_null),
	FTTI("TIME NULL", typeid(Null<sql_time>),
			FieldType::ft_time, FieldType::tf_null),
	FTTI("DATETIME NULL", typeid(Null<sql_datetime>),
			FieldType::ft_datetime, FieldType::tf_null),
	FTTI("ENUM NULL", typeid(Null<sql_enum>),
			FieldType::ft_enum, FieldType::tf_null),
	FTTI("SET NULL", typeid(Null<sql_set>),
			FieldType::ft_set, FieldType::tf_null),
	FTTI("TINYBLOB NULL", typeid(Null<sql_tinyblob>),
			FieldType::ft_blob, FieldType::tf_null, false),
	FTTI("MEDIUMBLOB NULL", typeid(Null<sql_mediumblob>),
			FieldType::ft_blob, FieldType::tf_null, false),
	FTTI("LONGBLOB NULL", typeid(Null<sql_longblob>),
			FieldType::ft_blob, FieldType::tf_null, false),
	FTTI("BLOB NULL", typeid(Null<sql_blob>),
			FieldType::ft_blob, FieldType::tf_null),
	FTTI("VARCHAR NULL", typeid(Null<sql_varchar>),
			FieldType::ft_text, FieldType::tf_null),
	FTTI("CHAR NULL", typeid(Null<sql_char>),
			FieldType::ft_text, FieldType::tf_null, false),
	FTTI("NULL NOT NULL", typeid(Null<void>),
			FieldType::ft_null, FieldType::tf_default),
};

const int FieldType::num_types_ =
		sizeof(FieldType::types_) / sizeof(FieldType::types_[0]);

const FieldType::TypeMap FieldType::type_map_;

#if !defined(DOXYGEN_IGNORE)
// Doxygen will not generate documentation for this section.

FieldType::TypeMap::TypeMap()
{
	for (map_type::mapped_type i = 0; i < num_types_; ++i) {
		map_[FieldType::types_[i].c_type_] = i;
	}
}

FieldType::TypeMap::map_type::mapped_type 
FieldType::TypeMap::operator [](const std::type_info& ti) const
{
	// Try for an exact match on the C++ std::type
	map_type::const_iterator it = map_.find(&ti);
	if (it != map_.end()) {
		return it->second;
	}
	else {
		// Can't find it.  Caller must be passing a C++ data type that
		// simply isn't represented in the types_ array.  Wah.
		std::ostringstream outs;
		outs << "Failed to find libtabula type info for " << ti.name();
		throw TypeLookupFailed(outs.str());
	}
}

#endif // !defined(DOXYGEN_IGNORE)

} // end namespace libtabula

