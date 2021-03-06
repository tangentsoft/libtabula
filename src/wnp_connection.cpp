/***********************************************************************
 wnp_connection.cpp - Implements the WindowsNamedPipeConnection class.

 Copyright © 2007-2008 by Educational Technology Resources, Inc.
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

#define LIBTABULA_NOT_HEADER
#include "common.h"
#include "wnp_connection.h"

#include "exceptions.h"

using namespace std;

namespace libtabula {

static const char* common_complaint =
		"WindowsNamedPipeConnection only works on Windows";


bool
WindowsNamedPipeConnection::connect(const char* db, const char* user,
		const char* pass)
{
#if defined(LIBTABULA_PLATFORM_WINDOWS)
	return Connection::connect(db, ".", user, pass);
#else
	(void)db;
	(void)user;
	(void)pass;
	error_message_ = common_complaint;
	if (throw_exceptions()) {
		throw ConnectionFailed(error_message_.c_str());
	}
	else {
		return false;
	}
#endif
}


bool
WindowsNamedPipeConnection::is_wnp(const char* server)
{
#if defined(LIBTABULA_PLATFORM_WINDOWS)
	return server && (strcmp(server, ".") == 0);
#else
	(void)server;
	return false;
#endif
}

} // end namespace libtabula

