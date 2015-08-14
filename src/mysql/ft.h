/// \file mysql/ft.h
/// \brief Implements the abstract FieldType interface for MySQL.

/***********************************************************************
 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB,
 and © 2004-2008, 2014-2015 by Educational Technology Resources, Inc.
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

private:
	/// \brief Return the FieldType::Base value corresponding to a given
	/// MySQL C API data type enum value.
	///
	/// \internal An implementation detail of the enum_field_types ctor.
	///
	/// \param t Underlying MySQL C API type constant
	static Base base_type(enum_field_types t);
};

} // end namespace libtabula

#endif // !defined(LIBTABULA_TYPE_INFO_H)

