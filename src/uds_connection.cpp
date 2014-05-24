/***********************************************************************
 uds_connection.cpp - Implements the UnixDomainSocketConnection class.

 Copyright © 2007-2008 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the
 CREDITS.txt file in the top directory of the distribution for details.

 This file is part of Libtabula.

 Libtabula is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.

 Libtabula is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with Libtabula; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
 USA
***********************************************************************/

#define LIBTABULA_NOT_HEADER
#include "common.h"
#include "uds_connection.h"

#include "exceptions.h"

#if !defined(LIBTABULA_PLATFORM_WINDOWS)
#	include <unistd.h>
#	include <sys/stat.h>
#endif

using namespace std;

namespace libtabula {

static const char* common_complaint =
		"UnixDomainSocketConnection only works on POSIX systems";

bool
UnixDomainSocketConnection::connect(const char* path,
		const char* db, const char* user, const char* pass)
{
#if !defined(LIBTABULA_PLATFORM_WINDOWS)
	if (is_socket(path, &error_message_)) {
		return Connection::connect(db, path, user, pass);
	}
	(void)common_complaint;
#else
	error_message_ = common_complaint;
#endif

	if (throw_exceptions()) {
		throw ConnectionFailed(error_message_.c_str());
	}
	else {
		return false;
	}
}


bool
UnixDomainSocketConnection::is_socket(const char* path, std::string* error)
{
#if !defined(LIBTABULA_PLATFORM_WINDOWS)
	if (path) {
		struct stat fi;

		if (access(path, F_OK) != 0) {
			if (error) {
				*error = path;
				*error += " does not exist";
			}
		}
		else if (access(path, R_OK | W_OK) != 0) {
			if (error) {
				*error = "Don't have read-write permission for ";
				*error += path;
			}
		}
		else if (stat(path, &fi) != 0) {
			if (error) {
				*error = "Failed to get information for ";
				*error += path;
			}
		}
		else if (!S_ISSOCK(fi.st_mode)) {
			if (error) {
				*error = path;
				*error += " is not a Unix domain socket";
			}
		}
		else {
			// It's a socket, and we have permission to use it
			if (error) {
				error->clear();
			}
			return true;
		}
	}
	else
#endif
	if (error) {
#if !defined(LIBTABULA_PLATFORM_WINDOWS)
		*error = "NULL is not a valid Unix domain socket";
#else
		*error = common_complaint;
#endif
	}

	return false;
}


} // end namespace libtabula

