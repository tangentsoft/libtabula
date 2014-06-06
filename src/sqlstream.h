/// \file sqlstream.h
/// \brief Defines a class for building quoted and escaped SQL text.

/***********************************************************************
 Copyright © 2008 by AboveNet, Inc.  Others may also hold copyrights
 on code in this file.  See the CREDITS file in the top directory of
 the distribution for details.

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

#if !defined(LIBTABULA_SQLSTREAM_H)
#define LIBTABULA_SQLSTREAM_H

#include "common.h"

#include <sstream>

namespace libtabula {

#if !defined(DOXYGEN_IGNORE)
// Make Doxygen ignore this
class LIBTABULA_EXPORT Connection;
#endif

/// \brief A class for building SQL-formatted strings.
///
/// See the user manual for more details about these options.

class LIBTABULA_EXPORT SQLStream :
public std::ostringstream
{
public:
	/// \brief Create a new stream object attached to a connection.
	///
	/// \param c connection used for escaping text
	/// \param pstr an optional initial string
	SQLStream(Connection* c, const char* pstr = 0);

	/// \brief Create a new stream object as a copy of another.
	///
	/// This is a traditional copy ctor.
	SQLStream(const SQLStream& s);

	/// \brief Return a SQL-escaped version of a character buffer
	///
	/// \param ps pointer to C++ string to hold escaped version; if
	/// original is 0, also holds the original data to be escaped
	/// \param original if given, pointer to the character buffer to
	/// escape instead of contents of *ps
	/// \param length if you give original, the number of
	/// characters to escape; otherwise, uses ps->length()
	///
	/// \retval number of characters placed in *ps
	///
	/// \see comments for escape_string(char*, const char*, size_t)
	/// and DBDriver::escape_string(std::string*, const char *, size_t)
	/// for further details.
	size_t escape_string(std::string* ps, const char* original = 0,
			size_t length = 0) const;

	/// \brief Return a SQL-escaped version of the given character
	/// buffer
	///
	/// \param escaped character buffer to hold escaped version; must
	/// point to at least (length * 2 + 1) bytes
	/// \param original pointer to the character buffer to escape
	/// \param length number of characters to escape
	///
	/// \retval number of characters placed in escaped
	///
	/// This function uses DBDriver::escape_string() if there is a
	/// valid connection, but falls back to escape_string_generic()
	/// otherwise.
	///
	/// \see DBDriver::escape_string(char*, const char*, size_t)
	/// for further details.
	size_t escape_string(char* escaped, const char* original,
			size_t length) const;

	/// \brief SQL-escape a string using DBMS-independent rules
	///
	/// \see escape_string(std::string*, const char*, size_t) for the
	/// meaning of the parameters and the return value
	static size_t escape_string_generic(std::string* ps,
			const char* original = 0, size_t length = 0);

	/// \brief SQL-escape a string using DBMS-independent rules
	///
	/// This method exists primarily to SQL-escape strings while there
	/// is no active DBMS connection, since we need that in order to
	/// know the current character set, which affects how we do SQL
	/// escaping.
	/// 
	/// You probably should not use this function directly, but
	/// instead use the mysqlpp::escape manipulator with SQLStream,
	/// since that always does the best possible thing in the given
	/// circumstances.
	///
	/// In fact, you probably shouldn't use this *at all* because it
	/// implies that you're trying to build valid SQL for an invalid
	/// DB connection.  It also means you're willing to settle for our
	/// generic rules, while your DBMS's escaping mechanism may give
	/// better results, being tuned to that DBMS's expectations.
	///
	/// \see escape_string(char*, const char*, size_t) for the meaning
	/// of the parameters and the return value
	static size_t escape_string_generic(char* escaped,
			const char* original, size_t length);

	/// \brief Assigns contents of another SQLStream to this one
	SQLStream& operator=(const SQLStream& rhs);

	/// \brief Connection to send queries through
	Connection* conn_;
};


/// \brief Insert raw string into the given stream.
///
/// This is just syntactic sugar for SQLStream::str(void)
inline std::ostream& operator <<(std::ostream& os, SQLStream& s)
{
	return os << s.str();
}


} // end namespace libtabula

#endif // !defined(LIBTABULA_SQLSTREAM_H)

