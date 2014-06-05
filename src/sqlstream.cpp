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
		// Normal case
		return conn_->driver()->escape_string(ps, original, length);
	}
	else {
		// We need a connection to escape a SQL string for two reasons:
		//
		// 1. Two different DBMS C APIs might not escape a given string
		//    the same way if their SQL dialects differ enough.
		//
		// 2. Some DBMSes change their escaping rules depending on the
		//    current character set, and that may be context dependent;
		//    e.g. which DB we have selected.
		//
		// Calling this without a working connection is a blatant usage
		// error.  Either we were called out of sequence, or conn was
		// dropped and caller didn't set up automatic reconnect.
		throw ConnectionFailed("Cannot escape SQL without a connection");
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
		// See above for reason we throw unconditionally here
		throw ConnectionFailed("Cannot escape SQL without a connection");
	}
}


SQLStream&
SQLStream::operator=(const SQLStream& rhs)
{
	conn_ = rhs.conn_;
	str(rhs.str());

	return *this;
}


} // end namespace libtabula

