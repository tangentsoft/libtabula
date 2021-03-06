/// \file sql_buffer.h
/// \brief Declares the SQLBuffer class

/***********************************************************************
 Copyright © 2007-2008 by Educational Technology Resources, Inc. and
 © 2007 by Jonathan Wakely.  Others may also hold copyrights on
 code in this file.  See the CREDITS.md file in the top directory
 of the distribution for details.

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

#if !defined(LIBTABULA_SQL_BUFFER_H)
#define LIBTABULA_SQL_BUFFER_H

#include "field_type.h"
#include "refcounted.h"

#include <string>

namespace libtabula {

/// \brief Holds SQL data in string form plus type information for use
/// in converting the string to compatible C++ data types.

class SQLBuffer
{
public:
	/// \brief Type of length values
	typedef size_t size_type;

	/// \brief Generic DBMS-independent "string" type definition
	///
	/// For consistency, the type is nullable, since SQLBuffer can
	/// always store a SQL null.  There is no non-nullable variant of
	/// SQLBuffer.  It either holds a null or it does not.
	static FieldType string_type;

	/// \brief Initialize object as a copy of a raw data buffer
	///
	/// Copies the string into a new buffer one byte longer than
	/// the length value given, using that to hold a C string null
	/// terminator, just for safety.  The length value we keep does
	/// not include this extra byte, allowing this same mechanism
	/// to work for both C strings and binary data.
	SQLBuffer(const char* data, size_type length, FieldType type,
			bool is_null) : data_(), length_(), type_(type),
			is_null_(is_null)
			{ replace_buffer(data, length); }

	/// \brief Initialize object as a copy of a C++ string object
	SQLBuffer(const std::string& s, FieldType type, bool is_null) :
			data_(), length_(), type_(type), is_null_(is_null)
	{
		replace_buffer(s.data(), static_cast<size_type>(s.length()));
	}

	/// \brief Destructor
	~SQLBuffer() { delete[] data_; }

	/// \brief Replace contents of buffer with copy of given C string
	SQLBuffer& assign(const char* data, size_type length,
			FieldType type = string_type, bool is_null = false);

	/// \brief Replace contents of buffer with copy of given C++ string
	SQLBuffer& assign(const std::string& s,
			FieldType type = string_type, bool is_null = false);

	/// \brief Return pointer to raw data buffer
	const char* data() const { return data_; }

	/// \brief Returns true if we were initialized with a data type
	/// that must be escaped when used in a SQL query
	bool escape_q() const { return type_.escape_q(); }

	/// \brief Return number of bytes in data buffer
	///
	/// Count does not include the trailing null we tack on to our
	/// copy of the buffer for ease of use in C string contexts.
	/// We do this because we can be holding binary data just as
	/// easily as a C string.
	size_type length() const { return length_; }

	/// \brief Returns true if type of buffer's contents is string
	bool is_string() { return type_ == string_type; }

	/// \brief Return true if buffer's contents represent a SQL
	/// null.
	///
	/// The buffer's actual content will probably be "NULL" or
	/// something like it, but in the SQL data type system, a SQL
	/// null is distinct from a plain string with value "NULL".
	bool is_null() const { return is_null_; }

	/// \brief Returns true if we were initialized with a data type
	/// that must be quoted when used in a SQL query
	bool quote_q() const;

	/// \brief Sets the internal SQL null flag
	void set_null() { is_null_ = true; }

	/// \brief Return the SQL type of the data held in the buffer
	const FieldType& type() const { return type_; }

private:
	SQLBuffer(const SQLBuffer&);
	SQLBuffer& operator=(const SQLBuffer&);

	/// \brief Common initialization for ctors
	void init(const char* pd, size_type len, FieldType type,
			bool is_null);
	/// \brief Implementation detail of assign() and init()
	void replace_buffer(const char* pd, size_type length);

	const char* data_;		///< pointer to the raw data buffer
	size_type length_;		///< bytes in buffer, without trailing null
	FieldType type_;		///< type of data in the buffer
	bool is_null_;			///< if true, string represents a SQL null
};


/// \brief Reference-counted version of SQLBuffer.
///
/// No one uses SQLBuffer directly.  It exists only for use in a
/// RefCountedPointer wrapper.
typedef RefCountedPointer<SQLBuffer> RefCountedBuffer;

} // end namespace libtabula

#endif // !defined(LIBTABULA_SQL_BUFFER_H)

