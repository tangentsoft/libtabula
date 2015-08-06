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

#include "field_type.h"

namespace libtabula {

/// \brief MySQL-specific field type information
///
/// \internal Used within libtabula for mapping SQL types to C++ types
/// and vice versa.
class LIBTABULA_EXPORT MySQLFieldType : public FieldType
{
public:
	/// \brief Standard constructor
	///
	/// \see FieldType::FieldType(Base, Flags)
	MySQLFieldType(Base b = ft_unsupported, Flag f = tf_default) :
	FieldType(b, f)
	{
	}

	/// \brief Promotion constructor
	///
	/// Converts generic libtabula data type info to MySQL-specific
	/// info.  This ctor lets you create a base class instance then
	/// later convert it to a DBDriver-specific instance.
	///
	/// FIXME: This may be a case of YAGNI.  The idea is that this will
	/// effectively let us convert enum Base to enum_field_types, and
	/// enum Flags to a MySQL C API "flags" bitfield.  But, we may find
	/// that this conversion should happen elsewhere in the DBDriver
	/// code, so the promotion ctor isn't necessary.
	MySQLFieldType(const FieldType& ft) :
	FieldType(ft)
	{
	}

	/// \brief Create object from MySQL C API type info
	///
	/// \param t the underlying MySQL C API type ID for this type,
	/// typically given as MYSQL_FIELD::type
	/// \param flags is the MYSQL_FIELD::flags value
	MySQLFieldType(enum_field_types t, unsigned int flags) :
	FieldType(base_type(t),
			FieldType::tf_default | 
			(flags & UNSIGNED_FLAG ? FieldType::tf_unsigned : 0) |
			(flags & NOT_NULL_FLAG ? 0 : FieldType::tf_null))
	{
	}

	/// \brief Create object as a copy of another
	MySQLFieldType(const MySQLFieldType& other) : FieldType(other) { } 

	/// \brief Returns an implementation-defined name of the C++ type.
	///
	/// Returns the name that would be returned by typeid().name() for
	/// the C++ type associated with the SQL type.
	const char* name() const { return type_info().c_type_->name(); }

	/// \brief Returns the name of the SQL type.
	///
	/// Returns the SQL name for the type.
	const char* sql_name() const { return type_info().sql_name_; }

	/// \brief Returns the type_info for the C++ type associated with
	/// the SQL type.
	///
	/// Returns the C++ type_info record corresponding to the SQL type.
	const std::type_info& c_type() const { return *type_info().c_type_; }

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
	static Base base_type(enum_field_types t);
};

} // end namespace libtabula

#endif // !defined(LIBTABULA_TYPE_INFO_H)

