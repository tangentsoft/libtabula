/***********************************************************************
 mysql/driver.cpp - Implements the MySQLDriver class, child of DBDriver.

 Copyright © 2005-2009, 2014 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the
 CREDITS.txt file in the top directory of the distribution for details.

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
#include "driver.h"

// An argument was added to mysql_shutdown() in MySQL 4.1.3 and 5.0.1.
#if ((MYSQL_VERSION_ID >= 40103) && (MYSQL_VERSION_ID <= 49999)) || (MYSQL_VERSION_ID >= 50001)
#	define SHUTDOWN_ARG ,SHUTDOWN_DEFAULT
#else
#	define SHUTDOWN_ARG
#endif

using namespace std;

namespace libtabula {

MySQLDriver::MySQLDriver(bool te) :
DBDriver(te)
{
	// We have to init the mysql_ object in the ctors even though we
	// never call mysql_*() until after connect() because some of the
	// set_option() paths modify mysql_.client_flag.
	mysql_init(&mysql_);
}


MySQLDriver::~MySQLDriver()
{
	if (connected()) {
		disconnect();
	}
}


DBDriver*
MySQLDriver::clone()
{
	MySQLDriver* other = new MySQLDriver(throw_exceptions());
	mysql_init(&other->mysql_);
	other->is_connected_ =
			mysql_real_connect(&mysql_, mysql_.host, mysql_.user,
				mysql_.passwd, mysql_.db, mysql_.port,
				mysql_.unix_socket, mysql_.client_flag);
	return other;
}


bool
MySQLDriver::connect(const char* host, const char* socket_name,
		unsigned int port, const char* db, const char* user,
		const char* password)
{
	disconnect();			// no-op if already disconnected
	return is_connected_ =
			apply_pending_options(false) &&	// some opts modify client_flag
			(mysql_real_connect(&mysql_, host, user, password, db,
				port, socket_name, mysql_.client_flag) == &mysql_) &&
			DBDriver::connect(host, socket_name, port, db, user, password);
}


void
MySQLDriver::disconnect()
{
	if (is_connected_) {
		mysql_close(&mysql_);
		memset(&mysql_, 0, sizeof(mysql_));
		is_connected_ = false;
	}
}

size_t
MySQLDriver::escape_string(std::string* ps, const char* original,
		size_t length)
{
	if (ps == 0) {
		// Can't do any real work!
		return 0;
	}
	else if (original == 0) {
		// ps must point to the original data as well as to the
		// receiving string, so get the pointer and the length from it.
		original = ps->data();
		length = ps->length();
	}
	else if (length == 0) {
		// We got a pointer to a C++ string just for holding the result
		// and also a C string pointing to the original, so find the
		// length of the original.
		length = strlen(original);
	}

	char* escaped = new char[length * 2 + 1];
	length = escape_string(escaped, original, length);
	ps->assign(escaped, length);
	delete[] escaped;

	return length;
}


void
MySQLDriver::fetch_fields(Fields& fl, ResultBase::Impl& impl) const
{
	MYSQL_RES* pres = MYSQL_RES_FROM_IMPL(impl);
	Fields::size_type c = mysql_num_fields(pres);
	fl.reserve(c);

	for (Fields::size_type i = 0; i < c; ++i) {
		if (const MYSQL_FIELD* pf = mysql_fetch_field(pres)) {
			fl.push_back(pf);
		}
		else {
			break;
		}
	}

	mysql_field_seek(pres, 0);	// semantics break otherwise!
}


string
MySQLDriver::query_info()
{
	const char* i = mysql_info(&mysql_);
	return i ? string(i) : string();
}


Option::Error
MySQLDriver::set_multi_statements_option(bool enable)
{
#if MYSQL_VERSION_ID >= 40101
	enum_mysql_set_option val = enable ?
			MYSQL_OPTION_MULTI_STATEMENTS_ON :
			MYSQL_OPTION_MULTI_STATEMENTS_OFF;
	return mysql_set_server_option(&mysql_, val) == 0 ?
			Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


// Low-level C API wrappers for setting a particular type of Option.
// Called from some of the set_option_impl() overloads below.

bool
MySQLDriver::set_option_internal(mysql_option moption, const void* arg)
{
	return !mysql_options(&mysql_, moption,
			static_cast<const char*>(arg));
}

bool
MySQLDriver::set_option_internal(unsigned int o, bool arg)
{
	// If we get through this loop and n is 1, only one bit is set in
	// the option value, which is as it should be.
	int n = o;
	while (n && ((n & 1) == 0)) {
		n >>= 1;
	}
	
	if ((n == 1) &&
			(o >= CLIENT_LONG_PASSWORD) &&
#if MYSQL_VERSION_ID > 40000	// highest flag value varies by version
			(o <= CLIENT_MULTI_RESULTS)
#else
			(o <= CLIENT_TRANSACTIONS)
#endif
			) {
		// Option value seems sane, so go ahead and set/clear the flag
		if (arg) {
			mysql_.client_flag |= o;
		}
		else {
			mysql_.client_flag &= ~o;
		}

		return true;
	}
	else {
		// Option value is outside the range we understand, or caller
		// erroneously passed a value with multiple bits set.
		return false;
	}
}


Option::Error
MySQLDriver::set_option_impl(const CompressOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_OPT_COMPRESS) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const ConnectTimeoutOption& opt)
{
	unsigned arg = opt.arg();
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_OPT_CONNECT_TIMEOUT, &arg) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const FoundRowsOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(CLIENT_FOUND_ROWS, opt.arg()) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const GuessConnectionOption& opt)
{
#if MYSQL_VERSION_ID >= 40101
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_OPT_GUESS_CONNECTION) ?
				Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const IgnoreSpaceOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(CLIENT_IGNORE_SPACE, opt.arg()) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const InitCommandOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_INIT_COMMAND, opt.arg().c_str()) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const InteractiveOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(CLIENT_INTERACTIVE, opt.arg()) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const LocalFilesOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(CLIENT_LOCAL_FILES, opt.arg()) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const LocalInfileOption& opt)
{
	unsigned arg = opt.arg();
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_OPT_LOCAL_INFILE, &arg) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const MultiResultsOption& opt)
{
#if MYSQL_VERSION_ID >= 40101
	if (connected()) {
		set_multi_statements_option(opt.arg());
	}
	else {
		return set_option_internal(CLIENT_MULTI_RESULTS, opt.arg()) ?
				Option::err_NONE : Option::err_api_reject;
	}
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const MultiStatementsOption& opt)
{
#if MYSQL_VERSION_ID >= 40101
	if (connected()) {
		set_multi_statements_option(opt.arg());
	}
	else {
		return set_option_internal(CLIENT_MULTI_STATEMENTS, opt.arg()) ?
				Option::err_NONE : Option::err_api_reject;
	}
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const NamedPipeOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_OPT_NAMED_PIPE) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const NoSchemaOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(CLIENT_NO_SCHEMA, opt.arg()) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const ProtocolOption& opt)
{
#if MYSQL_VERSION_ID > 40000		// only in 4.0 +
	unsigned arg = opt.arg();
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_OPT_PROTOCOL, &arg) ?
				Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const ReadDefaultFileOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_READ_DEFAULT_FILE, opt.arg().c_str()) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const ReadDefaultGroupOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_READ_DEFAULT_GROUP, opt.arg().c_str()) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const ReadTimeoutOption& opt)
{
#if MYSQL_VERSION_ID >= 40101
	unsigned arg = opt.arg();
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_OPT_READ_TIMEOUT, &arg) ?
				Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const ReconnectOption& opt)
{
	bool arg = opt.arg();
#if MYSQL_VERSION_ID >= 50106
	// Option fixed in this version to work correctly whether set before
	// connection comes up, or after
	return set_option_internal(MYSQL_OPT_RECONNECT, &arg) ?
			Option::err_NONE : Option::err_api_reject;
#elif MYSQL_VERSION_ID >= 50013
	// Between the time the option was created in 5.0.13 and when it was
	// fixed in 5.1.6, it only worked correctly if set after initial
	// connection.  So, don't accept it if disconnected, even though API
	// does accept it; option gets reset when the connection comes up.
	return connected() ?
			set_option_internal(MYSQL_OPT_RECONNECT, &arg) ?
				Option::err_NONE : Option::err_api_reject :
				Option::err_disconnected;
#else
	(void)arg;
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const ReportDataTruncationOption& opt)
{
#if MYSQL_VERSION_ID >= 50003
	bool arg = opt.arg();
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_REPORT_DATA_TRUNCATION, &arg) ?
				Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const SecureAuthOption& opt)
{
#if MYSQL_VERSION_ID >= 40101
	bool arg = opt.arg();
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_SECURE_AUTH, &arg) ?
				Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const SetCharsetDirOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_SET_CHARSET_DIR, opt.arg().c_str()) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const SetCharsetNameOption& opt)
{
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_SET_CHARSET_NAME, opt.arg().c_str()) ?
				Option::err_NONE : Option::err_api_reject;
}


Option::Error
MySQLDriver::set_option_impl(const SetClientIpOption& opt)
{
#if MYSQL_VERSION_ID >= 40101
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_SET_CLIENT_IP, opt.arg().c_str()) ?
				Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const SharedMemoryBaseNameOption& opt)
{
#if MYSQL_VERSION_ID >= 40100
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_SHARED_MEMORY_BASE_NAME, opt.arg().c_str()) ?
				Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const SslOption& opt)
{
#if defined(HAVE_MYSQL_SSL_SET)
	return connected() ? Option::err_connected :
			mysql_ssl_set(&mysql_, opt.key(), opt.cert(),
				opt.ca(), opt.capath(), opt.cipher()) == 0 ?
			Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const UseEmbeddedConnectionOption& opt)
{
#if MYSQL_VERSION_ID >= 40101
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_OPT_USE_EMBEDDED_CONNECTION) ?
				Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const UseRemoteConnectionOption& opt)
{
#if MYSQL_VERSION_ID >= 40101
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_OPT_USE_REMOTE_CONNECTION) ?
				Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


Option::Error
MySQLDriver::set_option_impl(const WriteTimeoutOption& opt)
{
#if MYSQL_VERSION_ID >= 40101
	unsigned arg = opt.arg();
	return connected() ? Option::err_connected :
			set_option_internal(MYSQL_OPT_WRITE_TIMEOUT, &arg) ?
				Option::err_NONE : Option::err_api_reject;
#else
	return Option::err_api_limit;
#endif
}


bool
MySQLDriver::shutdown()
{
	return mysql_shutdown(&mysql_ SHUTDOWN_ARG);
}


bool
MySQLDriver::thread_aware()
{
#if defined(LIBTABULA_PLATFORM_WINDOWS) || defined(HAVE_PTHREAD) || defined(HAVE_SYNCH_H)
	// Okay, good, libtabula itself is thread-aware, but only return true
	// if the underlying C API library is also thread-aware.
	return mysql_thread_safe();
#else
	// libtabula itself isn't thread-aware, so we don't need to do any
	// further tests.  All pieces must be thread-aware to return true.
	return false;	
#endif
}

} // end namespace libtabula

