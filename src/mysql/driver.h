/// \file mysql/driver.h
/// \brief Declares the MySQLDriver class.

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

#if !defined(LIBTABULA_MYSQLDRIVER_H)
#define LIBTABULA_MYSQLDRIVER_H

#include "dbdriver.h"

namespace libtabula {

#define MYSQL_SET_OPTION_IMPL(T) Option::Error set_option_impl(const T& opt);
#define MYSQL_RES_FROM_IMPL(R) dynamic_cast<ResultImpl&>(R)

// Functor to call mysql_free_result() on the pointer you pass it.
//
// This overrides RefCountedPointer's default destroyer, which uses
// operator delete; it annoys the C API when you nuke its data
// structures this way. :)
template <>
struct RefCountedPointerDestroyer<MYSQL_RES>
{
	/// \brief Functor implementation
	void operator()(MYSQL_RES* doomed) const
	{
		if (doomed) {
			mysql_free_result(doomed);
		}
	}
};


/// \brief Implements a DBDriver derivative for MySQL and DBMSes with
/// compatible C APIs such as MariaDB.
///
/// This is the original "driver" from MySQL++, so its interface maps
/// pretty tightly to the MySQL C API, a.k.a. Connector/C.
///
/// As with any DBDriver derivative, end-user code should not create
/// these objects directly.  Call Connection::driver() to retrieve the
/// DBDriver instance that Connection created based on the connection
/// parameters your code passed.
///
/// This class does add a few methods to the parent class's interface,
/// wrapping some Connector/C functions that are too MySQL-specific to
/// expose through the higher-level interface.  If you need to call one
/// of these, the accepted usage pattern is
///
/// \code dynamic_cast<MySQLDriver*>(conn->driver())->thread_id()
///
/// Most of the time, however, end-user code will use the driver
/// indirectly via public methods in Connection, Query, etc.

class LIBTABULA_EXPORT MySQLDriver : public DBDriver
{
private:
#if !defined(DOXYGEN_IGNORE)
	// MySQL-specific result set info
	class ResultImpl : public ResultBase::Impl
	{
	public:
		ResultImpl(MYSQL_RES* res) : res_(res) { }
		ResultImpl(RefCountedPointer<MYSQL_RES>& res) : res_(res) { }
		~ResultImpl() { /* implicit mysql_free_result(res_.raw()) */  }
		operator MYSQL_RES*() const { return res_.raw(); }
		ResultImpl* clone() const { return new ResultImpl(res_); }

	private:
		// Has to be mutable because so many Connector/C APIs take
		// non-const MYSQL_RES*, and we call those from const methods.
		mutable RefCountedPointer<MYSQL_RES> res_;
	};
#endif

public:
	/// \brief Create object
	MySQLDriver();

	/// \brief Destroy object
	virtual ~MySQLDriver();

	/// \brief Return the number of rows affected by the last query
	///
	/// Wraps \c mysql_affected_rows() in the MySQL C API.
	ulonglong affected_rows()
	{
		return mysql_affected_rows(&mysql_);
	}

	/// \brief Get database client library version
	///
	/// Wraps \c mysql_get_client_info() in the MySQL C API.
	std::string client_version() const
	{
		return mysql_get_client_info();
	}

	/// \brief Connect to database server
	///
	/// If you call this method on an object that is already connected
	/// to a database server, the previous connection is dropped and a
	/// new connection is established.
	virtual bool connect(const char* host, const char* socket_name,
			unsigned int port, const char* db, const char* user,
			const char* password);

	/// \brief Return a new driver object connected to this same DB
	/// within this same DBMS.
	DBDriver* clone();

	/// \brief Ask the database server to create a database
	///
	/// \param db name of database to create
	///
	/// \return true if database was created successfully
	bool create_db(const char* db) const;

	/// \brief Seeks to a particular row within the result set
	///
	/// Wraps mysql_data_seek() in MySQL C API.
	void data_seek(ResultBase::Impl& impl, ulonglong offset) const
	{
		mysql_data_seek(MYSQL_RES_FROM_IMPL(impl), offset);
	}

	/// \brief Drop the connection to the database server
	///
	/// This method should only be used by libtabula library internals.
	/// Unless you use the default constructor, this object should
	/// always be connected.
	void disconnect();

	/// \brief Drop a database
	///
	/// \param db name of database to destroy
	///
	/// \return true if database was dropped successfully
	bool drop_db(const std::string& db) const;

	/// \brief Return error message for last MySQL error associated with
	/// this connection.
	///
	/// Returns the last libtabula-specific error message, if any.
	/// Otherwise, it wraps \c mysql_error() in the MySQL C API.
	const char* error()
	{
		return mysql_error(&mysql_);
	}

	/// \brief Return last MySQL error number associated with this
	/// connection
	///
	/// Wraps \c mysql_errno() in the MySQL C API.
	int errnum() { return mysql_errno(&mysql_); }

	/// \brief Return a SQL-escaped version of the given character
	/// buffer
	///
	/// \see DBDriver::escape_string(char*, const char*, size_t)
	size_t escape_string(char* to, const char* from, size_t length)
	{
		return mysql_real_escape_string(&mysql_, to, from, 
				static_cast<unsigned long>(length));
	}

	/// \brief Return a SQL-escaped version of a character buffer
	///
	/// \see DBDriver::escape_string(std::string*, const char*, size_t)
	size_t escape_string(std::string* ps, const char* original,
			size_t length);

	/// \brief Executes the given query string
	///
	/// Wraps \c mysql_real_query() in the MySQL C API.
	bool execute(const char* qstr, size_t length)
	{
		return !mysql_real_query(&mysql_, qstr,
				static_cast<unsigned long>(length));
	}

	/// \brief Returns the next DB row from the given result set.
	///
	/// Wraps \c mysql_fetch_row() in MySQL C API.
	Row fetch_row(ResultBase& res, ResultBase::Impl& impl)
	{
		if (MYSQL_ROW raw = mysql_fetch_row(MYSQL_RES_FROM_IMPL(impl))) {
			// FIXME: Create MySQLRowImpl here from MYSQL_ROW, and
			//        make Row() take a RowImpl* as the first param.
			return Row(raw, &res, fetch_lengths(impl),
					res.throw_exceptions());
		}
	}

	/// \brief Returns the lengths of the fields in the current row
	///
	/// Wraps \c mysql_fetch_lengths() in MySQL C API.
	const unsigned long* fetch_lengths(ResultBase::Impl& impl) const
	{
		return mysql_fetch_lengths(MYSQL_RES_FROM_IMPL(impl));
	}

	/// \brief Fill out a Fields list from the given MySQL result
	void fetch_fields(Fields& fl, ResultBase::Impl& impl) const;
	
	/// \brief Releases memory used by a result set
	///
	/// Wraps \c mysql_free_result() in MySQL C API.
	void free_result(ResultBase::Impl& impl) const
	{
		mysql_free_result(MYSQL_RES_FROM_IMPL(impl));
	}

	/// \brief Return the connection options object
	st_mysql_options get_options() const { return mysql_.options; }

	/// \brief Get information about the IPC connection to the
	/// database server
	///
	/// String contains info about type of connection (e.g. TCP/IP,
	/// named pipe, Unix socket...) and the server hostname.
	///
	/// Wraps \c mysql_get_host_info() in the MySQL C API.
	std::string ipc_info()
	{
		return mysql_get_host_info(&mysql_);
	}

	/// \brief Get ID generated for an AUTO_INCREMENT column in the
	/// previous INSERT query.
	///
	/// \retval 0 if the previous query did not generate an ID.  Use
	/// the SQL function LAST_INSERT_ID() if you need the last ID
	/// generated by any query, not just the previous one.  This
	/// applies to stored procedure calls because this function returns
	/// the ID generated by the last query, which was a CALL statement,
	/// and CALL doesn't generate IDs.  You need to use LAST_INSERT_ID()
	/// to get the ID in this case.
	ulonglong insert_id()
	{
		return mysql_insert_id(&mysql_);
	}

	/// \brief Kill a MySQL server thread
	///
	/// \param tid ID of thread to kill
	///
	/// Wraps \c mysql_kill() in the MySQL C API.
	///
	/// \see thread_id()
	bool kill(unsigned long tid)
	{
		return !mysql_kill(&mysql_, tid);
	}

	/// \brief Returns true if there are unconsumed results from the
	/// most recent query.
	///
	/// Wraps \c mysql_more_results() in the MySQL C API.
	bool more_results()
	{
		#if MYSQL_VERSION_ID > 41000		// only in MySQL v4.1 +
			return mysql_more_results(&mysql_);
		#else
			return false;
		#endif
	}

	/// \brief Moves to the next result set from a multi-query
	///
	/// \return A code indicating whether we successfully found another
	/// result, there were no more results (but still success) or
	/// encountered an error trying to find the next result set.
	///
	/// Wraps \c mysql_next_result() in the MySQL C API, with
	/// translation of its return value from magic integers to nr_code
	/// enum values.
	nr_code next_result()
	{
		#if MYSQL_VERSION_ID > 41000		// only in MySQL v4.1 +
			switch (mysql_next_result(&mysql_)) {
				case 0:  return nr_more_results;
				case -1: return nr_last_result;
				default: return nr_error;
			}
		#else
			return nr_not_supported;
		#endif
	}

	/// \brief Returns the number of fields in the given result set
	///
	/// Wraps \c mysql_num_fields() in MySQL C API.
	int num_fields(ResultBase::Impl& impl) const
	{
		return mysql_num_fields(MYSQL_RES_FROM_IMPL(impl));
	}

	/// \brief Returns the number of rows in the given result set
	///
	/// Wraps \c mysql_num_rows() in MySQL C API.
	ulonglong num_rows(ResultBase::Impl& impl) const
	{
		return mysql_num_rows(MYSQL_RES_FROM_IMPL(impl));
	}

	/// \brief "Pings" the MySQL database
	///
	/// This function will try to reconnect to the server if the 
	/// connection has been dropped.  Wraps \c mysql_ping() in the MySQL C API.
	/// 
	/// \retval true if server is responding, regardless of whether we had
	/// to reconnect or not
	/// \retval false if either we already know the connection is down
	/// and cannot re-establish it, or if the server did not respond to
	/// the ping and we could not re-establish the connection.
	bool ping()
	{
		return !mysql_ping(&mysql_);
	}

	/// \brief Returns version number of MySQL protocol this connection
	/// is using
	///
	/// Wraps \c mysql_get_proto_info() in the MySQL C API.
	int protocol_version()
	{
		return mysql_get_proto_info(&mysql_);
	}

	/// \brief Returns information about the last executed query
	///
	/// Wraps \c mysql_info() in the MySQL C API
	std::string query_info();

	/// \brief Asks the database server to refresh certain internal data
	/// structures.
	///
	/// Wraps \c mysql_refresh() in the MySQL C API.  There is no
	/// corresponding interface for this in higher level libtabula classes
	/// because it was undocumented until recently, and it's a pretty
	/// low-level thing.  It's designed for things like MySQL
	/// Administrator.
	bool refresh(unsigned options)
	{
		return !mysql_refresh(&mysql_, options);
	}

	/// \brief Returns true if the most recent result set was empty
	bool result_empty() { return mysql_field_count(&mysql_) == 0; }

	/// \brief Asks the database server to switch to a different database
	bool select_db(const char* db)
	{
		return !mysql_select_db(&mysql_, db);
	}

	/// \brief Get the database server's version number
	///
	/// Wraps \c mysql_get_server_info() in the MySQL C API.
	std::string server_version()
	{
		return mysql_get_server_info(&mysql_);
	}

	/// \brief Ask database server to shut down.
	///
	/// User must have the "shutdown" privilege.
	///
	/// Wraps \c mysql_shutdown() in the MySQL C API.
	bool shutdown();

	/// \brief Returns the database server's status
	///
	/// String is similar to that returned by the \c mysqladmin
	/// \c status command.  Among other things, it contains uptime 
	/// in seconds, and the number of running threads, questions
	/// and open tables.
	///
	/// Wraps \c mysql_stat() in the MySQL C API.
	std::string server_status()
	{
		return mysql_stat(&mysql_);
	}

	/// \brief Saves the results of the query just execute()d in memory
	/// and returns a pointer to the MySQL C API data structure the
	/// results are stored in.
	///
	/// \sa use_result()
	///
	/// Wraps \c mysql_store_result() in the MySQL C API.
	StoreQueryResult store_result()
	{
		if (MYSQL_RES* pres = mysql_store_result(&mysql_)) {
			// FIXME: We've let the te parameter default
			return StoreQueryResult(new ResultImpl(pres), this,
					mysql_num_rows(pres));
		}
		else {
			return StoreQueryResult();
		}
	}

	/// \brief Returns true if libtabula and the underlying MySQL C API
	/// library were both compiled with thread awareness.
	///
	/// This is based in part on a MySQL C API function
	/// mysql_thread_safe().  We deliberately don't call this wrapper
	/// thread_safe() because it's a misleading name: linking to
	/// thread-aware versions of the libtabula and C API libraries doesn't
	/// automatically make your program "thread-safe".  See the
	/// <a href="../userman/threads.html">chapter on threads</a> in the
	/// user manual for more information and guidance.
	bool thread_aware();

	/// \brief Tells the underlying MySQL C API library that this thread
	/// is done using the library.
	///
	/// This exists because the MySQL C API library allocates some per-thread
	/// memory which it doesn't release until you call this.
	void thread_end()
	{
		#if MYSQL_VERSION_ID > 40000		// only in MySQL v4.0 +
			mysql_thread_end();
		#endif
	}

	/// \brief Returns the MySQL server thread ID for this connection
	///
	/// This has nothing to do with threading on the client side. It's
	/// a server-side thread ID, to be used with kill().
	unsigned long thread_id()
	{
		return mysql_thread_id(&mysql_);
	}

	/// \brief Tells the underlying C API library that the current
	/// thread will be using the library's services.
	///
	/// \retval True if there was no problem
	///
	/// The libtabula user manual's <a href="../userman/threads.html">chapter
	/// on threads</a> details two major strategies for dealing with
	/// connections in the face of threads.  If you take the simpler
	/// path, creating one MySQLDriver object per thread, it is never
	/// necessary to call this function; the underlying C API will call it
	/// for you when you establish the first database server connection
	/// from that thread.  If you use a more complex connection
	/// management strategy where it's possible for one thread to
	/// establish a connection that another thread uses, you must call
	/// this from each thread that can use the database before it creates
	/// any libtabula objects.  If you use a MySQLDriverPool object, this
	/// applies; MySQLDriverPool isn't smart enough to call this for you,
	/// and the MySQL C API won't do it, either.
	bool thread_start()
	{
		#if MYSQL_VERSION_ID > 40000		// only in MySQL v4.0 +
			return !mysql_thread_init();
		#else
			return false;
		#endif
	}

	/// \brief Returns a result set from the last-executed query which
	/// we can walk through in linear fashion, which doesn't store all
	/// result sets in memory.
	///
	/// \sa store_result
	///
	/// Wraps \c mysql_use_result() in the MySQL C API.
	UseQueryResult use_result()
	{
		if (MYSQL_RES* pres = mysql_use_result(&mysql_)) {
			// FIXME: We've let the te parameter default
			return UseQueryResult(new ResultImpl(pres), this);
		}
		else {
			return UseQueryResult();
		}
	}

private:
	/// \brief Enable or disable multi-statements
	///
	/// This enables both multi-statements and multi-results, and it
	/// can only work while the connection is up.  To set just one of
	/// these two values, you have to do it while the conn is down.
	///
	/// \internal Wraps \c mysql_set_server_option() in the C API
	Option::Error set_multi_statements_option(bool enable);

	/// \brief Set MySQL C API connection option, internal form 1
	///
	/// \internal Wraps \c mysql_options() in C API.
	bool set_option_internal(mysql_option moption, const void* arg = 0);

	/// \brief Set MySQL C API connection option, internal form 2
	///
	/// Manipulates the MYSQL.client_flag bit mask.  This allows these
	/// flags to be treated the same way as any other connection option,
	/// even though the C API handles them differently.
	bool set_option_internal(unsigned int option, bool arg);

#if !defined(DOXYGEN_IGNORE)
	// set_option_impl() declarations, one for each leaf class of Option.
	// Hidden from Doxygen because they're only public to allow those
	// Option::set() implementations to call us without making each leaf
	// class of Option a friend of DBDriver.  End user code should not
	// call these.  Call Connection::set_option() instead.
	MYSQL_SET_OPTION_IMPL(CompressOption);
	MYSQL_SET_OPTION_IMPL(ConnectTimeoutOption);
	MYSQL_SET_OPTION_IMPL(FoundRowsOption);
	MYSQL_SET_OPTION_IMPL(GuessConnectionOption);
	MYSQL_SET_OPTION_IMPL(IgnoreSpaceOption);
	MYSQL_SET_OPTION_IMPL(InitCommandOption);
	MYSQL_SET_OPTION_IMPL(InteractiveOption);
	MYSQL_SET_OPTION_IMPL(LocalFilesOption);
	MYSQL_SET_OPTION_IMPL(LocalInfileOption);
	MYSQL_SET_OPTION_IMPL(MultiResultsOption);
	MYSQL_SET_OPTION_IMPL(MultiStatementsOption);
	MYSQL_SET_OPTION_IMPL(NamedPipeOption);
	MYSQL_SET_OPTION_IMPL(NoSchemaOption);
	MYSQL_SET_OPTION_IMPL(ProtocolOption);
	MYSQL_SET_OPTION_IMPL(ReadDefaultFileOption);
	MYSQL_SET_OPTION_IMPL(ReadDefaultGroupOption);
	MYSQL_SET_OPTION_IMPL(ReadTimeoutOption);
	MYSQL_SET_OPTION_IMPL(ReconnectOption);
	MYSQL_SET_OPTION_IMPL(ReportDataTruncationOption);
	MYSQL_SET_OPTION_IMPL(SecureAuthOption);
	MYSQL_SET_OPTION_IMPL(SetCharsetDirOption);
	MYSQL_SET_OPTION_IMPL(SetCharsetNameOption);
	MYSQL_SET_OPTION_IMPL(SetClientIpOption);
	MYSQL_SET_OPTION_IMPL(SharedMemoryBaseNameOption);
	MYSQL_SET_OPTION_IMPL(SslOption);
	MYSQL_SET_OPTION_IMPL(UseEmbeddedConnectionOption);
	MYSQL_SET_OPTION_IMPL(UseRemoteConnectionOption);
	MYSQL_SET_OPTION_IMPL(WriteTimeoutOption);
#endif

	// Hidden assignment operator and copy ctor; you must call
	// clone() to create a new driver connected to the same DBMS.
	MySQLDriver& operator=(const MySQLDriver&) { return *this; }
	MySQLDriver(const MySQLDriver&) { }

	MYSQL mysql_;			///< C API handle for the DBMS connection
};


} // end namespace libtabula

#endif // !defined(LIBTABULA_MYSQLDRIVER_H)

