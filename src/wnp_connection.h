/// \file wnp_connection.h
/// \brief Declares the WindowsNamedPipeConnection class.

/***********************************************************************
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

#if !defined(LIBTABULA_WNP_CONNECTION_H)
#define LIBTABULA_WNP_CONNECTION_H

#include "connection.h"

namespace libtabula {

/// \brief Specialization of \c Connection for Windows named pipes
///
/// This class just simplifies the connection creation interface of
/// \c Connection.  It does not add new functionality.

class LIBTABULA_EXPORT WindowsNamedPipeConnection : public Connection
{
public:
	/// \brief Create object without connecting it to the MySQL server.
	WindowsNamedPipeConnection() :
	Connection()
	{
	}

	/// \brief Create object and connect to database server over Windows
	/// named pipes in one step.
	///
	/// \param db name of database to use
	/// \param user user name to log in under, or 0 to use the user
	///		name the program is running under
	/// \param password password to use when logging in
	WindowsNamedPipeConnection(const char* db, const char* user = 0,
			const char* password = 0) :
	Connection()
	{
		connect(db, user, password);
	}

	/// \brief Establish a new connection using the same parameters as
	/// an existing connection.
	///
	/// \param other pre-existing connection to clone
	WindowsNamedPipeConnection(const WindowsNamedPipeConnection& other) :
	Connection(other)
	{
	}

	/// \brief Destroy object
	~WindowsNamedPipeConnection() { }

	/// \brief Connect to database after object is created.
	///
	/// It's better to use the connect-on-create constructor if you can.
	/// See its documentation for the meaning of these parameters.
	///
	/// If you call this method on an object that is already connected
	/// to a database server, the previous connection is dropped and a
	/// new connection is established.
	bool connect(const char* db = 0, const char* user = 0,
			const char* password = 0);

	/// \brief Check that given string denotes a Windows named pipe
	/// connection to MySQL
	///
	/// \param server the server address
	///
	/// \return false if server address does not denote a Windows
	/// named pipe connection, or we are not running on Windows
	static bool is_wnp(const char* server);

private:
	/// \brief Provide uncallable versions of the parent class ctors we
	/// don't want to provide so we don't get warnings about hidden
	/// overloads with some compilers
	WindowsNamedPipeConnection(bool) { }
	WindowsNamedPipeConnection(const char*, const char*, const char*,
			const char*, unsigned int) { }

	/// \brief Explicitly override parent class version so we don't get
	/// complaints about hidden overloads with some compilers
	bool connect(const char*, const char*, const char*, const char*,
			unsigned int) { return false; }
};


} // end namespace libtabula

#endif // !defined(LIBTABULA_WNP_CONNECTION_H)

