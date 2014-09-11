/// \file dbdriver.h
/// \brief Declares the DBDriver class.

/***********************************************************************
 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB, and
 © 2004-2009, 2014 by Educational Technology Resources, Inc.  Others may
 also hold copyrights on code in this file.  See the CREDITS.txt file
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

#if !defined(LIBTABULA_DBDRIVER_H)
#define LIBTABULA_DBDRIVER_H

#include "common.h"

#include "field.h"
#include "options.h"
#include "result.h"

namespace libtabula {

#if !defined(DOXYGEN_IGNORE)
class Row;
#endif

#define DBD_SET_OPTION_IMPL(T) \
	virtual Option::Error set_option_impl(const T& opt) \
	{ return Option::err_NONE; }

/// \brief Define a generic database "driver" layer.
///
/// This class is abstract, providing only some generic mechanisms
/// that are common to all database engines we support.  Leaf classes
/// override most of the methods we define.  See the documentation
/// for the drivers your code will use; sometimes there is DBMS-specific
/// information in the leaf class's documentation.
///
/// End user code should not access the driver directly.  It is an
/// implementation-level class, primarily wrapped by Connection and
/// Query.

class LIBTABULA_EXPORT DBDriver : public OptionalExceptions
{
public:
	/// \brief Result code returned by next_result()
	enum nr_code {
		nr_more_results,	///< success, with more results to come
		nr_last_result,		///< success, last result received
		nr_error,			///< problem retrieving next result
		nr_not_supported	///< DBMS doesn't support "next result"
	};

	/// \brief Create object
	///
	/// \param te If true, the driver throws exceptions on error.
	/// Otherwise, it attempts to return error codes or tests as false
	/// in bool context to indicate problems.  This value is inherited
	/// from the corresponding flag set on the Connection object when
	/// it creates the DBDriver leaf class instance.  DBDriver doesn't
	/// throw many exceptions itself, but we need the flag to pass on
	/// to other objects that may do so.
	DBDriver(bool te = true);

	/// \brief Destroy object
	virtual ~DBDriver();

	/// \brief Return the number of rows affected by the last query
	virtual ulonglong affected_rows() = 0;

	/// \brief Get database client library version
	virtual std::string client_version() const = 0;

	/// \brief Connect to database server
	///
	/// If you call this method on an object that is already connected
	/// to a database server, the previous connection is dropped and a
	/// new connection is established.
	///
	/// Leaf classes must overload this, then call us after they have
	/// brought the connection up.  (If they call us after trying and
	/// *failing* to bring the conn up, the call-back is a no-op.)
	virtual bool connect(const char* host, const char* socket_name,
			unsigned int port, const char* db, const char* user,
			const char* password) = 0;

	/// \brief Return true if we have an active connection to the
	/// database server.
	///
	/// This does not actually check whether the connection is viable,
	/// it just indicates whether there was previously a successful
	/// connect() call and no disconnect().  Call ping() to actually
	/// test the connection's viability.
	virtual bool connected() const { return is_connected_; }

	/// \brief Return a new DBDriver connected to the same DB in the
	/// same DBMS as this object.
	virtual DBDriver* clone() = 0;

	/// \brief Drop the connection to the database server
	///
	/// This method should only be used by libtabula library internals.
	virtual void disconnect() = 0;

	/// \brief Return error message for the last C API failure.
	virtual const char* error() = 0;

	/// \brief Return last error number associated with this connection
	///
	/// The values returned are specific to the leaf class.
	virtual int errnum() = 0;

	/// \brief Return a SQL-escaped version of the given character
	/// buffer
	///
	/// \param to character buffer to hold escaped version; must
	/// point to at least (length * 2 + 1) bytes
	/// \param from pointer to the character buffer to escape
	/// \param length number of characters to escape
	///
	/// \retval number of characters placed in escaped
	///
	/// This function only works when we are connected to the DBMS;
	/// escaping is often context-dependent.  The underlying C API
	/// library probably needs to know the current character set, for
	/// example, and with some DBMSes that varies depending on which
	/// DB in the DBMS is currently selected.
	virtual size_t escape_string(char* to, const char* from, 
			size_t length) = 0;

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
	/// This method has three basic operation modes:
	///
	/// - Pass just a pointer to a C++ string containing the original
	///   data to escape, plus act as receptacle for escaped version
	/// - Pass a pointer to a C++ string to receive escaped string plus
	///   a pointer to a C string to be escaped
	/// - Pass nonzero for all parameters, taking original to be a
	///   pointer to an array of char with given length; does not treat
	///   null characters as special
	///
	/// There's a degenerate fourth mode, where ps is zero: simply
	/// returns 0, because there is nowhere to store the result.
	///
	/// Note that if original is 0, we always ignore the length
	/// parameter even if it is nonzero.  Length always comes from
	/// ps->length() in this case.
	///
	/// ps is a pointer because if it were a reference, the other
	/// overload would be impossible to call: the compiler would
	/// complain that the two overloads are ambiguous because
	/// std::string has a char* conversion ctor. A nice bonus is that
	/// pointer syntax makes it clearer that the first parameter is an
	/// "out" parameter.
	///
	/// \see comments for escape_string(char*, const char*, size_t)
	/// for further details.
	virtual size_t escape_string(std::string* ps, const char* original,
			size_t length) = 0;

	/// \brief Executes the given query string
	virtual bool execute(const char* qstr, size_t length) = 0;

	/// \brief Fill out a Fields list from the given MySQL result
	virtual void fetch_fields(Fields& fl, ResultBase::Impl& impl) const = 0;
	
	/// \brief Returns the lengths of the fields in the current row
	virtual const unsigned long* fetch_lengths(
			ResultBase::Impl& impl) const = 0;

	/// \brief Returns the next row from the given "use" query result set.
	virtual Row fetch_row(ResultBase& res, ResultBase::Impl& impl) = 0;

	/// \brief Releases memory used by a result set
	virtual void free_result(ResultBase::Impl& impl) const = 0;

	/// \brief Get the ID last generated for an INSERT by the DBMS.
	///
	/// The semantic meaning of this call varies among DBMSes because
	/// it reflects features not specified by ANSI SQL.  See the
	/// corresponding documentation for the leaf class's implemenetation
	/// of this method to learn how each driver you use interprets this.
	virtual ulonglong insert_id() = 0;

	/// \brief Returns true if there are unconsumed results from the
	/// most recent query.
	virtual bool more_results() = 0;

	/// \brief Moves to the next result set from a multi-query
	///
	/// \return A code indicating whether we successfully found another
	/// result, there were no more results (but still success) or
	/// encountered an error trying to find the next result set.
	virtual nr_code next_result() = 0;

	/// \brief Returns the number of fields in the given result set
	virtual int num_fields(ResultBase::Impl& impl) const = 0;

	/// \brief Returns the number of rows in the given result set
	virtual ulonglong num_rows(ResultBase::Impl& impl) const = 0;

	/// \brief Returns the last Option::Error code caused by set_option()
	///
	/// You may be asking why set_option() doesn't just return this
	/// value.  It is because of the "pending options" mechanism: if
	/// the connection is down and you pass set_option() an Option type
	/// that can only be set once the connection is up, DBDriver will
	/// save that object until the connection does come up.  If applying
	/// that option then fails, you need a way to get the error code
	/// that resulted, since Option::set() currently doesn't throw
	/// exceptions.
	///
	/// You can infer that an option has been put off until connection
	/// time when set_option() returns true, but this method returns
	/// Option::err_disconnected.  If an option is successfully applied
	/// immediately, set_option() also returns true, but this method
	/// returns Option::err_NONE.
	Option::Error option_error() const { return option_error_; }

	/// \brief "Pings" the DBMS engine, reconnecting if necessary
	/// 
	/// \retval true if server is responding, regardless of whether we
	/// had to reconnect or not
	///
	/// \retval false if the connection is down and we cannot
	/// re-establish it, or if the server did not respond to
	/// the ping and we could not re-establish the connection.
	virtual bool ping() = 0;

	/// \brief Returns true if the most recent result set was empty
	virtual bool result_empty() = 0;

	/// \brief Asks the database server to switch to a different database
	virtual bool select_db(const char* db) = 0;

	/// \brief Get the database server's version number
	virtual std::string server_version() = 0;

	/// \brief Sets a connection option
	///
	/// \see Connection::set_option(Option*) for the high-level
	/// description of what this method does.  This method implements
	/// the common option setting mechanism, calling down to our
	/// leaf class to do the actual low-level (DBMS C API) option
	/// setting calls.
	virtual bool set_option(Option* o);

	/// \brief Same as set_option() except that it won't override
	/// a previously-set option.
	virtual bool set_option_default(Option* o)
	{
		const std::type_info& ti = typeid(o);
		for (OptionList::const_iterator it = applied_options_.begin();
				it != applied_options_.end();
				++it) {
			if (typeid(*it) == ti) {
				delete o;
				return true;		// option of this type already set
			}
		}

		return set_option(o);
	}

#if !defined(DOXYGEN_IGNORE)
	// set_option_impl() declarations, one for each leaf class of Option.
	// Hidden from Doxygen because they're only public to allow those
	// Option::set() implementations to call us without making each leaf
	// class of Option a friend of DBDriver.  End user code should not
	// call these.  Call Connection::set_option() instead.
	DBD_SET_OPTION_IMPL(CompressOption);
	DBD_SET_OPTION_IMPL(ConnectTimeoutOption);
	DBD_SET_OPTION_IMPL(FoundRowsOption);
	DBD_SET_OPTION_IMPL(GuessConnectionOption);
	DBD_SET_OPTION_IMPL(IgnoreSpaceOption);
	DBD_SET_OPTION_IMPL(InitCommandOption);
	DBD_SET_OPTION_IMPL(InteractiveOption);
	DBD_SET_OPTION_IMPL(LocalFilesOption);
	DBD_SET_OPTION_IMPL(LocalInfileOption);
	DBD_SET_OPTION_IMPL(MultiResultsOption);
	DBD_SET_OPTION_IMPL(MultiStatementsOption);
	DBD_SET_OPTION_IMPL(NamedPipeOption);
	DBD_SET_OPTION_IMPL(NoSchemaOption);
	DBD_SET_OPTION_IMPL(ProtocolOption);
	DBD_SET_OPTION_IMPL(ReadDefaultFileOption);
	DBD_SET_OPTION_IMPL(ReadDefaultGroupOption);
	DBD_SET_OPTION_IMPL(ReadTimeoutOption);
	DBD_SET_OPTION_IMPL(ReconnectOption);
	DBD_SET_OPTION_IMPL(ReportDataTruncationOption);
	DBD_SET_OPTION_IMPL(SecureAuthOption);
	DBD_SET_OPTION_IMPL(SetCharsetDirOption);
	DBD_SET_OPTION_IMPL(SetCharsetNameOption);
	DBD_SET_OPTION_IMPL(SetClientIpOption);
	DBD_SET_OPTION_IMPL(SharedMemoryBaseNameOption);
	DBD_SET_OPTION_IMPL(SslOption);
	DBD_SET_OPTION_IMPL(UseEmbeddedConnectionOption);
	DBD_SET_OPTION_IMPL(UseRemoteConnectionOption);
	DBD_SET_OPTION_IMPL(WriteTimeoutOption);
#endif

	/// \brief Saves the results of the query just execute()d in memory
	///
	/// \sa use_result()
	virtual ResultBase::Impl* store_result() = 0;

	/// \brief Returns true if libtabula and the C API underlying this
	/// driver were both compiled with thread awareness.
	virtual bool thread_aware() = 0;

	/// \brief Tells the underlying C API library that this thread
	/// is done using the library.
	virtual void thread_end() = 0;

	/// \brief Tells the underlying C API library that the current
	/// thread will be using the library's services.
	///
	/// \retval True if there was no problem
	virtual bool thread_start() = 0;

	/// \brief Returns a result set from the last-executed query which
	/// we can walk through in linear fashion, which doesn't store all
	/// result sets in memory.
	///
	/// \sa store_result
	virtual ResultBase::Impl* use_result() = 0;

protected:
	/// \brief Called after a connection is established, this sends
	/// all the queued Option objects to the leaf class's set_option()
	/// implementation.
	virtual bool apply_pending_options(bool quit_on_first_failure);

	/// \brief Returns true if connect() has successfully been called
	/// on this object without a following disconnect().
	///
	/// \sa connected()
	bool is_connected_;

private:
	/// \brief Data type of the list of applied connection options
	typedef std::deque<Option*> OptionList;

	/// \brief Iterator into an OptionList
	typedef OptionList::iterator OptionListIt;

	// Hidden assignment operator and copy ctor; you must call
	// clone() to create a new driver connected to the same DBMS.
	DBDriver& operator=(const DBDriver&) { return *this; }
	DBDriver(const DBDriver&) { }

	OptionList applied_options_;
	OptionList pending_options_;
	Option::Error option_error_;
};


} // end namespace libtabula

#endif // !defined(LIBTABULA_DBDRIVER_H)

