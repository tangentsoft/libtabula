/// \file connection.h
/// \brief Declares the Connection class.
///
/// Every program using libtabula must create a Connection object, which
/// manages information about the connection to the database server, and
/// performs connection-related operations once the connection is up.
/// Subordinate classes, such as Query and Row take their defaults as
/// to whether exceptions are thrown when errors are encountered from
/// the Connection object that created them, directly or indirectly.

/***********************************************************************
 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB, and
 © 2004-2007 by Educational Technology Resources, Inc.  Others may
 also hold copyrights on code in this file.  See the CREDITS.md file
 in the top directory of the distribution for details.

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

#if !defined(LIBTABULA_CONNECTION_H)
#define LIBTABULA_CONNECTION_H

#include "common.h"

#include "noexceptions.h"
#include "options.h"

#include <string>

namespace libtabula {

#if !defined(DOXYGEN_IGNORE)
// Make Doxygen ignore this
class LIBTABULA_EXPORT Query;
class DBDriver;
#endif

/// \brief Manages the connection to the database server.
///
/// This class is a thick wrapper around DBDriver, adding high-level
/// error handling, utility functions, and abstraction away from
/// underlying C API details.

class LIBTABULA_EXPORT Connection : public OptionalExceptions
{
private:
	/// \brief Pointer to bool data member, for use by safe bool
	/// conversion operator.
	///
	/// \see http://www.artima.com/cppsource/safebool.html
	typedef bool Connection::*private_bool_type;

public:
	/// \brief Create object without connecting to the database server.
	///
	/// \param te if true, exceptions are thrown on errors
	Connection(bool te = true);

	/// \brief Create object and connect to database server in one step.
	///
	/// This constructor allows you to most fully specify the options
	/// used when connecting to the database server.
	///
	/// \param db name of database to select upon connection
	/// \param server specifies the IPC method and parameters for
	///     contacting the server; see below for details
	/// \param user user name to log in under, or 0 to use the user
	///		name this program is running under
	/// \param password password to use when logging in
	/// \param port TCP port number database server is listening on, or
	///		0 to use default value; note that you may also give this as
	///     part of the \c server parameter
	///
	/// The server parameter can be any of several different forms:
	///
	/// - \b 0: Let the database driver decide how to connect; usually
	///   some sort of localhost IPC method.
	/// - \b ".": On Windows, this means named pipes, if the server
	///   supports it
	/// - \b "/some/domain/socket/path": If the passed string doesn't
	///   match one of the previous alternatives and we're on a system
	///   that supports Unix domain sockets, libtabula will test it to see
	///   if it names one, and use it if we have permission.
	/// - \b "host.name.or.ip:port": If the previous test fails, or if
	///   the system doesn't support Unix domain sockets at all, it
	///   assumes the string is some kind of network address, optionally
	///   followed by a colon and port.  The name can be in dotted quad
	///   form, a host name, or a domain name.  The port can either be a
	///   TCP/IP port number or a symbolic service name.  If a port or
	///   service name is given here and a nonzero value is passed for
	///   the \c port parameter, the latter takes precedence.
	Connection(const char* db, const char* server = 0, const char* user = 0,
			const char* password = 0, unsigned int port = 0);

	/// \brief Establish a new connection using the same parameters as
	/// an existing connection.
	///
	/// \param other existing Connection object
	Connection(const Connection& other);

	/// \brief Destroy object
	virtual ~Connection();

	/// \brief Get version of library underpinning the current database
	/// driver.
	std::string client_version() const;

	/// \brief Connect to database after object is created.
	///
	/// It's better to use the connect-on-create constructor if you can.
	/// See its documentation for the meaning of these parameters.
	///
	/// If you call this method on an object that is already connected
	/// to a database server, the previous connection is dropped and a
	/// new connection is established.
	virtual bool connect(const char* db = 0, const char* server = 0,
			const char* user = 0, const char* password = 0,
			unsigned int port = 0);

	/// \brief Returns true if connection was established successfully
	///
	/// \return true if connection was established successfully
	bool connected() const;

	/// \brief Returns the number of rows in a table
	///
	/// \param table name of table whose rows you want counted
	///
	/// This is syntactic sugar for a \c SELECT \c COUNT(*)
	/// \c FROM \c table SQL query.
	ulonglong count_rows(const std::string& table);

	/// \brief Ask the database server to create a database
	///
	/// \param db name of database to create
	///
	/// \return true if database was created successfully
	bool create_db(const std::string& db);

	/// \brief Drop the connection to the database server
	void disconnect();

	/// \brief Returns a reference to the current database driver
	///
	/// \internal This exists mostly for the benefit of Query, the
	/// ResultBase hierarchy, and SQLStream.  If DBDriver exposes a
	/// feature you want, you can probably get access to it through
	/// one of these higher-level classes instead.  If you truly can't
	/// find an equivalent feature in the library's high-level classes,
	/// send a message to the mailing list asking about it.  Unless
	/// you're doing something very low-level, there should never be
	/// a reason to use the driver directly.
	DBDriver* driver() { return driver_; }

	/// \brief Asks the database server to drop (destroy) a database
	///
	/// \param db name of database to destroy
	///
	/// \return true if database was dropped successfully
	bool drop_db(const std::string& db);

	/// \brief Return last error number associated with this
	/// connection
	int errnum();

	/// \brief Return error message for last error associated with
	/// this connection.
	///
	/// Returns either a libtabula-specific error message if one exists,
	/// or one from the current database driver otherwise.
	const char* error() const;

	/// \brief Test whether any error has occurred within the object.
	///
	/// Allows the object to be used in bool context, like this:
	///
	/// \code
	///	Connection conn;
	///	.... use conn
	///	if (conn) {
	///	    ... nothing bad has happened since last successful use
	///	}
	///	else {
	///	    ... some error has occurred
	///	}
	/// \endcode
	///
	/// Prior to libtabula v3, the object was always falsy when we
	/// weren't connected.  Now a true return simply indicates a lack of
	/// errors.  If you've been using this to test for whether the
	/// connection is still up, you need to call connected() instead.
	operator private_bool_type() const
	{
		return copacetic_ ? &Connection::copacetic_ : 0;
	}

	/// \brief Copy an existing Connection object's state into this
	/// object.
	Connection& operator=(const Connection& rhs);

	/// \brief "Pings" the database server
	///
	/// \retval true if server is responding
	/// \retval false if either we already know the connection is down
	/// and cannot re-establish it, or if the server did not respond to
	/// the ping and we could not re-establish the connection.
	bool ping();

	/// \brief Return a new query object.
	///
	/// The returned query object is tied to this connection object,
	/// so when you call a method like
	/// \link Query::execute() execute() \endlink
	/// on that object, the query is sent to the server this object
	/// is connected to.
	///
	/// \param qstr an optional query string for populating the
	/// new Query object
	Query query(const char* qstr = 0);

	/// \brief Return a new query object
	///
	/// \param qstr initial query string
	Query query(const std::string& qstr);

	/// \brief Change to a different database managed by the
	/// database server we are connected to.
	///
	/// \param db database to switch to
	///
	/// \retval true if we changed databases successfully
	bool select_db(const std::string& db);

	/// \brief Get the database server's version string
	std::string server_version() const;

	/// \brief Sets a connection option
	///
	/// \param o pointer to any derivative of Option allocated on
	/// the heap
	///
	/// Objects passed to this method and successfully set will be
	/// released when this Connection object is destroyed.  If an error
	/// occurs while setting the option the object will be deleted
	/// immediately.
	///
	/// Because there are so many Option subclasses, the actual effect
	/// of this function has a wide range.  This mechanism abstracts
	/// away many things that are unrelated down at the database driver
	/// level, hiding them behind a coherent, type-safe interface.
	///
	/// The rules about which options can be set, when, are up to the
	/// underlying database driver.  Some must be set before the
	/// connection is established because they can only be used during
	/// that connection setup process.  Others can be set at any time
	/// after the connection comes up.  If you get it wrong, you'll get
	/// a BadOption exception.
	///
	/// \retval true if option was successfully set
	bool set_option(Option* o);

	/// \brief Returns true if both libtabula and database driver we're
	/// using were compiled with thread awareness.
	bool thread_aware();

	/// \brief Tells the underlying database driver that this thread
	/// is done using the database.
	///
	/// Some database C APIs allocate per-thread resources when you
	/// start using them in a thread, and don't release them until
	/// you explicitly tell it to.
	void thread_end();

	/// \brief Tells the underlying database driver that the current
	/// thread is now using its services.
	///
	/// Whether you need to call this or not depends on the driver(s)
	/// you are using.  See the function of the same name in the
	/// driver to learn the per-DBMS requirements.
	///
	/// It is safe to call this function from all threads that use
	/// this Connection, even in cases where it provides no benefit.
	/// You could thus be lazy and just call this from each thread 
	/// that uses this Connection before you start using it in that
	/// thread.
	///
	/// \retval True if there was no problem
	bool thread_start();

protected:
	/// \brief Build an error message in the standard form used whenever
	/// one of the methods can't succeed because we're not connected to
	/// the database server.
	void build_error_message(const char* core);

	/// \brief Establish a new connection as a copy of an existing one
	///
	/// \param other the connection to copy
	void copy(const Connection& other);

	/// \brief Extract elements from the server parameter in formats
	/// suitable for passing to DBDriver::connect().
	bool parse_ipc_method(const char* server, std::string& host,
			unsigned int& port, std::string& socket_name);

	//// Subclass data
	mutable std::string error_message_;	///< libtabula specific error, if any

private:
	DBDriver* driver_;
	bool copacetic_;
};


} // end namespace libtabula

// Not strictly required, but bring in our specialization subclasses
#include "tcp_connection.h"
#include "uds_connection.h"
#include "wnp_connection.h"

#endif // !defined(LIBTABULA_CONNECTION_H)

