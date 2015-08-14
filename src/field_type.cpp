/***********************************************************************
 field_type.cpp - Implements the FieldType class, as well as some
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
#include "field_type.h"

#include "datetime.h"
#include "myset.h"
#include "sql_types.h"

#include <string>

using namespace std;

namespace libtabula {

#if !defined(DOXYGEN_IGNORE)
// Doxygen will not generate documentation for this section.

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
typedef detail::AnnotatedFT AFT;
static const AFT types_[] = {
	// Basic non-nullable type set
	AFT("TINYINT NOT NULL", typeid(sql_tinyint),
			FieldType::ft_integer, FieldType::tf_default, false),
	AFT("TINYINT UNSIGNED NOT NULL", typeid(sql_tinyint_unsigned),
			FieldType::ft_integer, FieldType::tf_unsigned, false),
	AFT("SMALLINT NOT NULL", typeid(sql_smallint),
			FieldType::ft_integer, FieldType::tf_default, false),
	AFT("SMALLINT UNSIGNED NOT NULL", typeid(sql_smallint_unsigned),
			FieldType::ft_integer, FieldType::tf_unsigned, false),
	AFT("MEDIUMINT NOT NULL", typeid(sql_mediumint),
			FieldType::ft_integer, FieldType::tf_unsigned, false),
	AFT("MEDIUMINT UNSIGNED NOT NULL", typeid(sql_mediumint_unsigned),
			FieldType::ft_integer, FieldType::tf_unsigned, false),
	AFT("INT NOT NULL", typeid(sql_int), FieldType::ft_integer),
	AFT("INT UNSIGNED NOT NULL", typeid(sql_int_unsigned),
			FieldType::ft_integer, FieldType::tf_unsigned),
	AFT("BIGINT NOT NULL", typeid(sql_bigint), 
			FieldType::ft_integer, FieldType::tf_default, false),
	AFT("BIGINT UNSIGNED NOT NULL", typeid(sql_bigint_unsigned),
			FieldType::ft_integer, FieldType::tf_unsigned, false),
	AFT("FLOAT NOT NULL", typeid(sql_float), 
			FieldType::ft_real, FieldType::tf_default, false),
	AFT("FLOAT UNSIGNED NOT NULL", typeid(sql_float),
			FieldType::ft_real, FieldType::tf_unsigned, false),
	AFT("DOUBLE NOT NULL", typeid(sql_double), FieldType::ft_real),
	AFT("DOUBLE UNSIGNED NOT NULL", typeid(sql_double),
			FieldType::ft_real, FieldType::tf_unsigned),
	AFT("DECIMAL NOT NULL", typeid(sql_decimal), FieldType::ft_decimal),
	AFT("TIMESTAMP NOT NULL", typeid(sql_timestamp), FieldType::ft_timestamp),
	AFT("DATE NOT NULL", typeid(sql_date), FieldType::ft_date),
	AFT("TIME NOT NULL", typeid(sql_time), FieldType::ft_time),
	AFT("DATETIME NOT NULL", typeid(sql_datetime), FieldType::ft_datetime),
	AFT("ENUM NOT NULL", typeid(sql_enum), FieldType::ft_enum),
	AFT("SET NOT NULL", typeid(sql_set), FieldType::ft_set),
	AFT("TINYBLOB NOT NULL", typeid(sql_tinyblob),
			FieldType::ft_blob, FieldType::tf_default, false),
	AFT("MEDIUMBLOB NOT NULL", typeid(sql_mediumblob), 
			FieldType::ft_blob, FieldType::tf_default, false),
	AFT("LONGBLOB NOT NULL", typeid(sql_longblob),
			FieldType::ft_blob, FieldType::tf_default, false),
	AFT("BLOB NOT NULL", typeid(sql_blob), FieldType::ft_blob),
	AFT("VARCHAR NOT NULL", typeid(sql_varchar), FieldType::ft_text),
	AFT("CHAR NOT NULL", typeid(sql_char), 
			FieldType::ft_text, FieldType::tf_default, false),
	AFT("NULL NOT NULL", typeid(void),
			FieldType::ft_null, FieldType::tf_default),

	// Nullable versions of above
	AFT("TINYINT NULL", typeid(Null<sql_tinyint>),
			FieldType::ft_integer, FieldType::tf_null, false),
	AFT("TINYINT UNSIGNED NULL", typeid(Null<sql_tinyint_unsigned>),
			FieldType::ft_integer,
			FieldType::tf_null | FieldType::tf_unsigned, false),
	AFT("SMALLINT NULL", typeid(Null<sql_smallint>),
			FieldType::ft_integer, FieldType::tf_null, false),
	AFT("SMALLINT UNSIGNED NULL", typeid(Null<sql_smallint_unsigned>),
			FieldType::ft_integer,
			FieldType::tf_null | FieldType::tf_unsigned, false),
	AFT("MEDIUMINT NULL", typeid(Null<sql_mediumint>),
			FieldType::ft_integer, FieldType::tf_null, false),
	AFT("MEDIUMINT UNSIGNED NULL", typeid(Null<sql_mediumint_unsigned>), 
			FieldType::ft_integer,
			FieldType::tf_null | FieldType::tf_unsigned, false),
	AFT("INT NULL", typeid(Null<sql_int>),
			FieldType::ft_integer, FieldType::tf_null),
	AFT("INT UNSIGNED NULL", typeid(Null<sql_int_unsigned>),
			FieldType::ft_integer, FieldType::tf_null | FieldType::tf_unsigned),
	AFT("BIGINT NULL", typeid(Null<sql_bigint>),
			FieldType::ft_integer, FieldType::tf_null, false),
	AFT("BIGINT UNSIGNED NULL", typeid(Null<sql_bigint_unsigned>),
			FieldType::ft_integer,
			FieldType::tf_null | FieldType::tf_unsigned, false),
	AFT("FLOAT NULL", typeid(Null<sql_float>),
			FieldType::ft_real, FieldType::tf_null, false),
	AFT("FLOAT UNSIGNED NULL", typeid(Null<sql_float>),
			FieldType::ft_real,
			FieldType::tf_null | FieldType::tf_unsigned, false),
	AFT("DOUBLE NULL", typeid(Null<sql_double>),
			FieldType::ft_real, FieldType::tf_null),
	AFT("DOUBLE UNSIGNED NULL", typeid(Null<sql_double>),
			FieldType::ft_real, FieldType::tf_null | FieldType::tf_unsigned),
	AFT("DECIMAL NULL", typeid(Null<sql_decimal>),
			FieldType::ft_decimal, FieldType::tf_null),
	AFT("TIMESTAMP NULL", typeid(Null<sql_timestamp>),
			FieldType::ft_timestamp),
	AFT("DATE NULL", typeid(Null<sql_date>),
			FieldType::ft_date, FieldType::tf_null),
	AFT("TIME NULL", typeid(Null<sql_time>),
			FieldType::ft_time, FieldType::tf_null),
	AFT("DATETIME NULL", typeid(Null<sql_datetime>),
			FieldType::ft_datetime, FieldType::tf_null),
	AFT("ENUM NULL", typeid(Null<sql_enum>),
			FieldType::ft_enum, FieldType::tf_null),
	AFT("SET NULL", typeid(Null<sql_set>),
			FieldType::ft_set, FieldType::tf_null),
	AFT("TINYBLOB NULL", typeid(Null<sql_tinyblob>),
			FieldType::ft_blob, FieldType::tf_null, false),
	AFT("MEDIUMBLOB NULL", typeid(Null<sql_mediumblob>),
			FieldType::ft_blob, FieldType::tf_null, false),
	AFT("LONGBLOB NULL", typeid(Null<sql_longblob>),
			FieldType::ft_blob, FieldType::tf_null, false),
	AFT("BLOB NULL", typeid(Null<sql_blob>),
			FieldType::ft_blob, FieldType::tf_null),
	AFT("VARCHAR NULL", typeid(Null<sql_varchar>),
			FieldType::ft_text, FieldType::tf_null),
	AFT("CHAR NULL", typeid(Null<sql_char>),
			FieldType::ft_text, FieldType::tf_null, false),
	AFT("NULL NOT NULL", typeid(Null<void>),
			FieldType::ft_null, FieldType::tf_default),
};

// Number of elements in types_[]
static const int num_types_ = sizeof(types_) / sizeof(types_[0]);


// Set up an "index" into types_[] keyed on std::type_info, to make
// the FieldType(type_info) conversion ctor faster.
class TypeMap
{
private:
	struct Cmp
	{
		bool operator() (const type_info* lhs, const type_info* rhs) const
				{ return lhs < rhs; }
	};
	typedef map<const type_info*, size_t, Cmp> map_type;

	map_type map_;

public:
	TypeMap()
	{
		for (map_type::mapped_type i = 0; i < num_types_; ++i) {
			map_[types_[i].c_type_] = i;
		}
	}
	const map_type::mapped_type operator [](const type_info& ti) const
	{
		// Try for an exact match on the C++ std::type_info value
		map_type::const_iterator it = map_.find(&ti);
		if (it != map_.end()) {
			return it->second;
		}
		else {
			// Can't find it.  Caller must be passing a C++ data type
			// that simply isn't represented in the types_ array.  Wah.
			ostringstream outs;
			outs << "Failed to find libtabula type info for " << ti.name();
			throw TypeLookupFailed(outs.str());
		}
	}
};
static const TypeMap type_map_;


// std::type_info conversion ctor.  Looks up the statically-defined
// libtabula type info in types_[] and copies that.
FieldType::FieldType(const type_info& t)
{
	size_t i = type_map_[t];
	base_type_ = types_[i].base_type_;
	flags_ = types_[i].flags_;
}


// "Annotate" this type by looking up the C++ and SQL type info in the
// types_[] table based on our libtabula {Base,Flags} pair.
const AFT&
FieldType::annotate() const
{
	// Make sure we got fully-initted first.
	if (base_type_ == ft_unsupported) {
		throw TypeLookupFailed("FieldType::annotate() called before "
				"object fully initted");
	}

	// Try to find an entry in types_[] matching our {Base,Flags} pair
	int guess = -1;
	for (size_t i = 0; i < num_types_; ++i) {
		const AFT& cand = types_[i];
		if (base_type_ == cand.base_type_) {
			// Found a possible match.
			if (flags_ == cand.flags_) {
				// It's a good match, but is it the best?
				guess = i;
				if (cand.best_guess_) return cand;
			}
			else if ((cand.flags_ & flags_) == cand.flags_) {
				// All flags on the candidate are present in the
				// caller's value, so keep this one in mind, but
				// don't go with it unless we find no better option.
				guess = i;
			}
			// else, it's only a weak match; hold out for better
		}
	}

	// Did we find one?
	if (guess >= 0) {
		return types_[guess];
	}
	else {
		ostringstream outs;
		outs << "Failed to find C++ and SQL type info for libtabula "
				"type {" << base_type_ << ',' << flags_ << '}';
		throw TypeLookupFailed(outs.str());
	}
}


// Pass calls down to our equivalent AnnotatedFT object
const char* FieldType::name() const { return annotate().c_type_->name(); }
const char* FieldType::sql_name() const { return annotate().sql_name_; }
const std::type_info& FieldType::c_type() const { return *annotate().c_type_; }

#endif // !defined(DOXYGEN_IGNORE)

} // end namespace libtabula

