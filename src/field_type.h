/// \file field_type.h
/// \brief Declares the FieldType class, which provides a translation
/// between the SQL and C++ type systems.
///
/// This class should be treated as an internal implementation detail.
/// End user code should not have to use this class directly.  The
/// library uses this information to translate C++ type info to and
/// from the DBMS layer, so if you are using appropriate C++ types,
/// the translation to/from the appropriate FieldType should be
/// transparent.  If not, consider it a bug, and report it.

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

#if !defined(LIBTABULA_FIELD_TYPE_H)
#define LIBTABULA_FIELD_TYPE_H

#include "common.h"

#include <typeinfo>

namespace libtabula {

/// \brief SQL field type information
///
/// \internal Used within libtabula for mapping SQL types to C++ types
/// and vice versa.
class LIBTABULA_EXPORT FieldType
{
public:
	/// \brief Elementary SQL data types
	/// 
	/// These are the data types we expect every DBMS supported by
	/// libtabula to understand.  Subclasses can extend this, starting
	/// their enum with ft_FIRST_UNUSED.
	enum Base {
		ft_integer,
		ft_real,
		ft_text,
		ft_blob,
		ft_FIRST_UNUSED,
	};

	/// \brief SQL data type modifier flags
	///
	/// These flags modify the \c ft_* type flag enum.  If this field
	/// has no flags (i.e. it is tf_default) it is implicitly signed
	/// if it's an integer type, and it's non-null in all cases.
	enum Flag {
		tf_default = 0,
		tf_unsigned = 1,
		tf_null = 2,
		tf_FIRST_UNUSED = 4,
	};

	/// \brief Standard constructor
	///
	/// Although this ctor allows you to use it as a default ctor,
	/// we only support that because FieldTypes keeps a vector of these
	/// objects, and std::vector requires a default ctor for its
	/// elements.  If you are creating these objects in code you write,
	/// you need to pass the first parameter at least, or overwrite the
	/// default-initted copy with the assignment operator.  Failure to
	/// do so will trigger the crash we have arranged by casting a bogus
	/// value.
	FieldType(Base b = static_cast<Base>(-1), unsigned int f = tf_default) :
	base_type_(b),
	flags_(f)
	{
	}

	/// \brief Create object as a copy of another
	FieldType(const FieldType& other) :
	base_type_(other.base_type_),
	flags_(other.flags_)
	{
	}

	/// \brief Assign another FieldType object to this object
	FieldType& operator =(const FieldType& t)
	{
		base_type_ = t.base_type_;
		flags_ = t.flags_;
		return *this;
	}

	/// \brief Returns an implementation-defined name of the C++ type.
	///
	/// Returns the name that would be returned by typeid().name() for
	/// the C++ type associated with the SQL type.
	virtual const char* name() const { return 0; }

	/// \brief Returns the name of the SQL type.
	///
	/// Returns the SQL name for the type.
	virtual const char* sql_name() const { return 0; }

	/// \brief Returns the type_info for the C++ type associated with
	/// the SQL type.
	///
	/// Returns the C++ type_info record corresponding to the SQL type.
	virtual const std::type_info& c_type() const { return typeid(void); }

	/// \brief Returns the type_info for the C++ type inside of the
	/// libtabula::Null type.
	///
	/// Returns the type_info for the C++ type inside the libtabula::Null
	/// type.  If the type is not Null then this is the same as c_type().
	virtual const FieldType::Base base_type() const { return base_type_; }

	/// \brief Returns true if the SQL type is of a type that needs to
	/// be quoted.
	///
	/// \return true if the type needs to be quoted for syntactically
	/// correct SQL.
	virtual bool quote_q() const
	{
		return	base_type_ == ft_blob ||
				base_type_ == ft_text;
	}

	/// \brief Returns true if the SQL type is of a type that needs to
	/// be escaped.
	///
	/// \return true if the type needs to be escaped for syntactically
	/// correct SQL.
	virtual bool escape_q() const
	{
		return	base_type_ == ft_blob ||
				base_type_ == ft_text;
	}

	/// \brief Return an opaque composite value that uniquely identifies
	/// this field type.
	///
	/// Though the implementation is right here to be examined, do not
	/// depend on the format of the value.  It could change.
	unsigned short id() const { return (flags_ << 8) | base_type_; }

protected:
	Base base_type_;
	unsigned int flags_;
};

/// \brief Returns true if two FieldType objects are equal.
inline bool operator ==(const FieldType& lhs, const FieldType& rhs)
{
	return lhs.id() == rhs.id();
}

/// \brief Returns true if two FieldType objects are not equal.
inline bool operator !=(const FieldType& lhs, const FieldType& rhs)
{
	return lhs.id() != rhs.id();
}

/// \brief Returns true if a given FieldType object is equal
/// to a given C++ type_info object.
inline bool operator ==(const std::type_info& lhs, const FieldType& rhs)
{
	return lhs == rhs.c_type();
}

/// \brief Returns true if a given FieldType object is not equal
/// to a given C++ type_info object.
inline bool operator !=(const std::type_info& lhs, const FieldType& rhs)
{
	return lhs != rhs.c_type();
}

/// \brief Returns true if a given FieldType object is equal
/// to a given C++ type_info object.
inline bool operator ==(const FieldType& lhs, const std::type_info& rhs)
{
	return lhs.c_type() == rhs;
}

/// \brief Returns true if a given FieldType object is not equal
/// to a given C++ type_info object.
inline bool operator !=(const FieldType& lhs, const std::type_info& rhs)
{
	return lhs.c_type() != rhs;
}

/// \brief Returns true if the lhs FieldType object is "less than" the
/// rhs object, for sorting purposes.
///
/// This only exists to allow you to use FieldType in associative
/// containers.  There is no other meaning to the ordering.
inline bool operator <(const FieldType& lhs, const FieldType& rhs)
{
	return lhs.id() < rhs.id();
}

} // end namespace libtabula

#endif // !defined(LIBTABULA_FIELD_TYPE_H)

