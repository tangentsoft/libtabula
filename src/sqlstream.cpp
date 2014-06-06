/***********************************************************************
 sqlstream.cpp - Implements the SQLStream class.

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

#include "sqlstream.h"

#include "dbdriver.h"
#include "connection.h"

#include <memory>
#include <string>

namespace libtabula {

SQLStream::SQLStream(Connection* c, const char* pstr) :
std::ostringstream(),
conn_(c)
{
	if (pstr != 0) {
		str(pstr);
	}
}


SQLStream::SQLStream(const SQLStream& s) :
std::ostringstream(s.str()),
conn_(s.conn_)
{
}


size_t
SQLStream::escape_string(std::string* ps, const char* original,
		size_t length) const
{
	if (conn_ && *conn_) {
		// Normal case: we have a valid conn, hence we can ask the
		// driver to do this in a DBMS and charset-aware fashion.
		return conn_->driver()->escape_string(ps, original, length);
	}
	else {
		// Fall back to generic libtabula routine
		return escape_string_generic(ps, original, length);
	}
}


size_t
SQLStream::escape_string(char* escaped, const char* original,
		size_t length) const
{
	if (conn_ && *conn_) {
		// Normal case
		return conn_->driver()->escape_string(escaped, original, length);
	}
	else {
		// Fall-back case
		return escape_string_generic(escaped, original, length);
	}
}


size_t
SQLStream::escape_string_generic(std::string* ps,
		const char* original, size_t length)
{
	std::auto_ptr<char> escaped(new char[length * 2 + 1]);
	size_t outlen = escape_string_generic(escaped.get(), 
			original ? original : ps->data(),
			original ? length : ps->length());
	ps->assign(escaped.get(), outlen);
	return outlen;
}


size_t
SQLStream::escape_string_generic(char* escaped,
		const char* original, size_t length)
{
	const char* oe = escaped;

	for (size_t i = 0; i < length; ++i) {
		switch (char c = *original++) {
			case 0:
				*escaped++ = '\\';
				*escaped++ = '0';
				break;

			case '\\':
				*escaped++ = '\\';
				*escaped++ = '\\';
				break;

			case '\'':
				*escaped++ = '\\';
				*escaped++ = '\'';
				break;

			case '"':
				*escaped++ = '\\';
				*escaped++ = '"';
				break;

			case '\n':
				*escaped++ = '\\';
				*escaped++ = 'n';
				break;

			case '\r':
				*escaped++ = '\\';
				*escaped++ = 'r';
				break;

			default:
				*escaped++ = c;
				break;
		}
	}

	*escaped = '\0';

	return escaped - oe;
}


SQLStream&
SQLStream::operator=(const SQLStream& rhs)
{
	conn_ = rhs.conn_;
	str(rhs.str());

	return *this;
}


} // end namespace libtabula

