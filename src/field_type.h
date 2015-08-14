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

#if !defined(LIBTABULA_FIELD_TYPE_H)
#define LIBTABULA_FIELD_TYPE_H

#include "common.h"
#include "exceptions.h"

#include <map>
#include <sstream>
#include <typeinfo>

namespace libtabula {

namespace detail {
	class AnnotatedFT;
}

/// \brief SQL field type information
///
/// \internal Used within libtabula for mapping SQL types to C++ types
/// and vice versa.
class LIBTABULA_EXPORT FieldType
{
public:
	/// \brief Basic SQL data types with libtabula support
	/// 
	/// These are ordered roughly by commonality in SQL implementations.
	/// As you go further down the list, the chance decreases that any
	/// given SQL DBMS supports it.
	///
	/// This is called FieldType::Base because these are the basic data
	/// types only, without modifiers.  See FieldType::Flag.
	enum Base {
		// Special data type, giving libtabula a way to flag data of a
		// type that the library itself does not support.
		ft_unsupported,

		// Data types pretty much every SQL implementation knows
		ft_null,		// data type of SQL NULL values; not used as col type!
		ft_integer,
		ft_real,		// a.k.a float, double...
		ft_text,
		ft_blob,

		// Common data types, but not universal.  For example, SQLite
		// only supports date/time values via specially-formatted string
		// columns.  For another, many DBMSes have no formal "boolean"
		// type, but instead require you to use bare integers or enums.
		ft_date = 20,		// skip a bit to allow expansion
		ft_time,
		ft_datetime,
		ft_timestamp,
		ft_decimal,			// fixed-point number; not same as ft_real!
		ft_enum,
		ft_boolean,
		ft_set,
	};

	/// \brief SQL data type modifier flags
	///
	/// These flags modify the \c ft_* type flag enum.  If this field
	/// has no flags (i.e. it is tf_default) it is implicitly signed
	/// if it's an integer type, and it's non-null in all cases.
	///
	/// As with Base, these are ordered by commonality.
	enum Flag {
		// Type modifiers every SQL DBMS should support, even SQLite
		tf_default = 		1 << 0,
		tf_unsigned = 		1 << 1,
		tf_null = 			1 << 2,

		// Common type attributes, but not universal
		tf_primary_key =	1 << 10,
		tf_unique_key =		1 << 11,
		tf_multiple_key =	1 << 12,

		// Nonstandard modifers; mainly MySQL-specific
		tf_auto_increment = 1 << 20,	// nonstandard SQL
		tf_binary = 		1 << 21,	// BINARY TEXT ≅ CHAR sans encoding
		tf_zerofill =		1 << 22,	// 0-padded strings
	};

	/// \brief Standard constructor
	///
	/// Although this ctor allows you to use it as a default ctor,
	/// we only support that because FieldTypes keeps a vector of these
	/// objects, and std::vector requires a default ctor for its
	/// elements.  If you are creating these objects in code you write,
	/// you need to pass the first parameter at least, or overwrite the
	/// default-initted copy with the assignment operator.  Failure to
	/// do so will cause a runtime error.
	FieldType(Base b = ft_unsupported, unsigned int f = tf_default) :
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

	/// \brief Create a FieldType object from a C++ type_info object
	///
	/// This tries to map a C++ type to the closest libtabula data type.
	/// It is necessarily somewhat approximate.  For one thing, we
	/// ignore integer signedness.  We also make the fully-warranted
	/// assumption that C++ hasn't grown a SQL-like "nullable" type
	/// attribute between the time this code was written and the time
	/// your compiler was last improved.
	FieldType(const std::type_info& t);

	/// \brief Assign another FieldType object to this object
	FieldType& operator =(const FieldType& t)
	{
		base_type_ = t.base_type_;
		flags_ = t.flags_;
		return *this;
	}

	/// \brief Returns the libtabula data type enum for this object.
	///
	/// Our return value may simply be the Base value we got in our
	/// ctor, but it could also be a conversion from another form,
	/// either C++ type info via our std::type_info ctor or C DBMS
	/// API type info from a subclass ctor.
	///
	/// This does not encode null-ness.
	const Base base_type() const { return base_type_; }

	/// \brief Returns true if this type describes a SQL column with a
	/// default value.
	///
	/// Always returns false if the object is storing information about
	/// a SQL value on a particular row, rather than column info.
	bool is_default() const { return flags_ == FieldType::tf_default; }

	/// \brief Returns true if this type describes either a null-able SQL
	/// column or a SQL value that is NULL.
	bool is_null() const { return flags_ & FieldType::tf_null; }

	/// \brief Returns true if this type describes an SQL column that
	/// cannot be negative.
	///
	/// Various SQL DBMSes interpret this differently.  Some don't have
	/// a strong notion of unsigned-ness (e.g. SQLite) while others take
	/// the concept rather too far (IMHO) allowing perversions like
	/// "unsigned float." (e.g. MySQL/MariaDB)
	bool is_unsigned() const { return flags_ & FieldType::tf_unsigned; }

	/// \brief Returns an implementation-defined name of the C++ type.
	///
	/// Returns the name that would be returned by typeid().name() for
	/// the C++ type associated with the SQL type.
	const char* name() const;

	/// \brief Returns the name of the SQL type.
	///
	/// Returns the SQL name for the type.
	const char* sql_name() const;

	/// \brief Returns the type_info for the C++ type associated with
	/// the SQL type.
	///
	/// Returns the C++ type_info record corresponding to the SQL type.
	const std::type_info& c_type() const;

	/// \brief Returns true if the SQL type is of a type that needs to
	/// be quoted for syntactically correct SQL.
	bool quote_q() const
	{
		return	base_type_ == ft_blob ||
				base_type_ == ft_date ||
				base_type_ == ft_datetime ||
				base_type_ == ft_set ||
				base_type_ == ft_text ||
				base_type_ == ft_time;
	}

	/// \brief Returns true if the SQL type is of a type that needs to
	/// be escaped for syntactically correct SQL.
	bool escape_q() const
	{
		return	base_type_ == ft_blob ||
				base_type_ == ft_enum ||
				base_type_ == ft_text;
	}

	/// \brief Return an opaque composite value that uniquely identifies
	/// this field type.
	///
	/// Though the implementation is right here to be examined, do not
	/// depend on the format of the value.  It could change.
	unsigned short id() const { return (flags_ << 8) | base_type_; }

private:
	//// Internal support functions
	const detail::AnnotatedFT& annotate() const;

	//// Internal data
	Base base_type_;
	unsigned int flags_;
};

namespace detail {
	/// \brief An extension of FieldType, adding several values that are
	/// implicitly associated with our parent's {Base, Flag} pair.
	///
	/// \internal This type is not for use by end-user code.  It is
	/// public only because libtabula creates a static array of these
	/// for use by FieldType to use in various lookups.
	class AnnotatedFT : public FieldType
	{
	public:
		AnnotatedFT& operator=(const AnnotatedFT& other);
		
		// Parameter order differs from FieldType's ctor on purpose:
		// they're ordered to allow some to default in many cases in
		// the types_[] table initialization.
		AnnotatedFT(const char* s, const std::type_info& t,
				Base bt, unsigned int f = tf_default, bool bg = true) :
		FieldType(bt, f),
		sql_name_(s),
		c_type_(&t),
		best_guess_(bg)
		{
		}

		const char* sql_name_;
		const std::type_info* c_type_;
		const bool best_guess_;
	};
} // end namespace libtabula::detail

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

