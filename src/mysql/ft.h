/// \file mysql/ft.h
/// \brief Implements the abstract FieldType interface for MySQL.

/***********************************************************************
 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB,
 and © 2004-2008, 2014 by Educational Technology Resources, Inc.
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

#if !defined(LIBTABULA_TYPE_INFO_H)
#define LIBTABULA_TYPE_INFO_H

#include "common.h"

#include "exceptions.h"

#include <map>
#include <sstream>
#include <typeinfo>

namespace libtabula {

#if !defined(DOXYGEN_IGNORE)
// Doxygen will not generate documentation for this section.

class LIBTABULA_EXPORT MySQLFieldType;
class LIBTABULA_EXPORT MySQLFieldTypeLookup;

class LIBTABULA_EXPORT MySQLFieldTypeInfo
{
private:
	friend class MySQLFieldType;
	friend class MySQLFieldTypeLookup;

	MySQLFieldTypeInfo& operator=(const MySQLFieldTypeInfo& other);
	
	// Not initting base_type_ and default_ because only MySQLFieldType
	// can create them.  There *must* be only one copy of each.
	MySQLFieldTypeInfo() :
	sql_name_(0),
	c_type_(0),
	base_type_(
#if MYSQL_VERSION_ID > 40000
		MYSQL_TYPE_NULL
#else
		FIELD_TYPE_NULL
#endif
	),
	flags_(0)
	{
	}
	
	MySQLFieldTypeInfo(const char* s,
			const std::type_info& t, const enum_field_types bt,
			const unsigned int flags = FieldType::tf_default) :
	sql_name_(s),
	c_type_(&t),
	base_type_(bt),
	flags_(flags)
	{
	}

	bool is_default() const { return flags_ == FieldType::tf_default; }
	bool is_null() const { return flags_ & FieldType::tf_null; }
	bool is_unsigned() const { return flags_ & FieldType::tf_unsigned; }

	const char* sql_name_;
	const std::type_info* c_type_;
	const enum_field_types base_type_;
	const unsigned char flags_;
};


struct MySQLTypeInfoCmp
{
	bool operator() (const std::type_info* lhs,
			const std::type_info* rhs) const
			{ return lhs < rhs; }
};

class LIBTABULA_EXPORT MySQLFieldTypeLookup
{
private:
	friend class MySQLFieldType;

	typedef MySQLFieldTypeInfo MySQLFieldTypeInfo;
	typedef std::map<const std::type_info*, unsigned char, MySQLTypeInfoCmp>
			map_type;

	MySQLFieldTypeLookup(const MySQLFieldTypeInfo types[],
			const int size);

	const unsigned char& operator [](
			const std::type_info& ti) const
	{
		map_type::const_iterator it = map_.find(&ti);
		if (it != map_.end()) {
			return it->second;
		}
		else {
			std::ostringstream outs;
			outs << "Failed to find MySQL C API type ID for " << ti.name();
			throw TypeLookupFailed(outs.str());
		}
	}

	map_type map_;
};

#endif // !defined(DOXYGEN_IGNORE)


/// \brief SQL field type information
///
/// \internal Used within libtabula for mapping SQL types to C++ types
/// and vice versa.
class LIBTABULA_EXPORT MySQLFieldType : public FieldType
{
public:
	/// \brief Extended version of FieldType::Base, adding
	/// MySQL-specific data types
	enum Base {
		// Dupe base class values, since C++ can't subclass an enum.
		ft_integer =		FieldType::ft_integer,
		ft_real =			FieldType::ft_real,
		ft_text =			FieldType::ft_text,
		ft_blob =			FieldType::ft_blob,

		// MySQL-specific data types
		ft_datetime =		ft_FIRST_UNUSED + 0,
		// TODO: finish filling out
	};

	/// \brief Extended version of FieldType::Flags, adding
	/// MySQL-specific type flags
	enum Flag {
		// See above
		tf_default = 		FieldType::tf_default,
		tf_unsigned =		FieldType::tf_unsigned,
		tf_null = 			FieldType::tf_null,

		// MySQL-specific type flags, from MYSQL_FIELD.flags
		tf_auto_increment = tf_FIRST_UNUSED << 0,
		tf_binary = 		tf_FIRST_UNUSED << 1,
		tf_blob =			tf_FIRST_UNUSED << 2,
		tf_enumeration =	tf_FIRST_UNUSED << 3,
		tf_multiple_key =	tf_FIRST_UNUSED << 4,
		tf_no_default =		tf_FIRST_UNUSED << 5,
		tf_primary_key =	tf_FIRST_UNUSED << 6,
		tf_set_type =		tf_FIRST_UNUSED << 7,
		tf_timestamp =		tf_FIRST_UNUSED << 8,
		tf_unique_key =		tf_FIRST_UNUSED << 9,
		tf_zerofill =		tf_FIRST_UNUSED << 10,
	};

	/// \brief Standard constructor
	///
	/// \see FieldType::FieldType(FieldType::Base, FieldType::Flags)
	MySQLFieldType(Base b = static_cast<Base>(-1), Flag f = tf_default) :
	FieldType(static_cast<FieldType::Base>(b), 
	          static_cast<FieldType::Flag>(f))
	{
	}

	/// \brief Create object from MySQL C API type info
	///
	/// \param t the underlying MySQL C API type ID for this type
	/// \param is_unsigned if true, this is the unsigned version of the type
	/// \param is_null if true, this type can hold a SQL null
	MySQLFieldType(enum_field_types t, bool is_unsigned = false,
			bool is_null = false) :
	FieldType(static_cast<FieldType::Base>(type(t)),
			  static_cast<FieldType::Flag>(FieldType::tf_default | 
					(is_null ? FieldType::tf_null : 0) |
					(is_unsigned ? FieldType::tf_unsigned : 0)))
	{
	}

	/// \brief Create object as a copy of another
	MySQLFieldType(const MySQLFieldType& other) : FieldType(other) { } 

	/// \brief Create a MySQLFieldType object from a C++ type_info object
	///
	/// This tries to map a C++ type to the closest MySQL data type.
	/// It is necessarily somewhat approximate.  For one thing, we
	/// ignore integer signedness.  We also make the fully-warranted
	/// assumption that C++ hasn't grown a SQL-like "nullable" type
	/// attribute between the time this code was written and the time
	/// your compiler was last improved.
	MySQLFieldType(const std::type_info& t) :
	FieldType(static_cast<FieldType::Base>(lookups[t]),
			FieldType::tf_default)
	{
	}

	/// \brief Returns an implementation-defined name of the C++ type.
	///
	/// Returns the name that would be returned by typeid().name() for
	/// the C++ type associated with the SQL type.
	const char* name() const { return deref().c_type_->name(); }

	/// \brief Returns the name of the SQL type.
	///
	/// Returns the SQL name for the type.
	const char* sql_name() const { return deref().sql_name_; }

	/// \brief Returns the type_info for the C++ type associated with
	/// the SQL type.
	///
	/// Returns the C++ type_info record corresponding to the SQL type.
	const std::type_info& c_type() const { return *deref().c_type_; }

	/// \brief Returns the type_info for the C++ type inside of the
	/// libtabula::Null type.
	///
	/// Returns the type_info for the C++ type inside the libtabula::Null
	/// type.  If the type is not Null then this is the same as c_type().
	const FieldType base_type() const
	{
		return deref().base_type_;
	}

	/// \brief Returns true if the SQL type is of a type that needs to
	/// be quoted.
	///
	/// \return true if the type needs to be quoted for syntactically
	/// correct SQL.
	bool quote_q() const;

	/// \brief Returns true if the SQL type is of a type that needs to
	/// be escaped.
	///
	/// \return true if the type needs to be escaped for syntactically
	/// correct SQL.
	bool escape_q() const;

private:
	static const MySQLFieldTypeInfo types[];
	static const int base_type_types;

	static const MySQLFieldTypeLookup lookups;

	/// \brief Return an index into MySQLFieldType::types array given
	/// MySQL type information.
	///
	/// This function is used in mapping from MySQL type information
	/// (a type enum, and flags indicating whether it is unsigned and
	/// whether it can be 'null') to the closest C++ types available
	/// within libtabula.  Notice that nulls have to be handled specially:
	/// the SQL null concept doesn't map directly onto the C++ type
	/// system.  See null.h for details.
	///
	/// \param t Underlying MySQL C API type constant
	///
	/// While libtabula is tied to MySQL, \c t is just an abstraction
	/// of enum_field_types from mysql_com.h.
	static Base type(enum_field_types t);

	const MySQLFieldTypeInfo& deref() const
	{
		return types[base_type_];
	}
};

} // end namespace libtabula

#endif // !defined(LIBTABULA_TYPE_INFO_H)

