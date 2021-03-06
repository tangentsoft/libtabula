/// \file exceptions.h
/// \brief Declares the libtabula-specific exception classes.
///
/// When exceptions are enabled for a given libtabula::OptionalExceptions
/// derivative, any of these exceptions can be thrown on error.

/***********************************************************************
 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB, and
 © 2004-2010, 2018 by Educational Technology Resources, Inc.  Others may
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

#if !defined(LIBTABULA_EXCEPTIONS_H)
#define LIBTABULA_EXCEPTIONS_H

#include "options.h"

#include <exception>
#include <string>
#include <sstream>
#include <typeinfo>

namespace libtabula {

/// \brief Base class for all libtabula custom exceptions

class LIBTABULA_EXPORT Exception : public std::exception
{
public:
	/// \brief Create exception object as copy of another
	Exception(const Exception& e) :
	std::exception(e),
	what_(e.what_)
	{
	}

	/// \brief Assign another exception object's contents to this one
	Exception& operator=(const Exception& rhs) throw()
	{
		what_ = rhs.what_;
		return *this;
	}

	/// \brief Destroy exception object
	~Exception() throw() { }

	/// \brief Returns explanation of why exception was thrown
	virtual const char* what() const throw()
	{
		return what_.c_str();
	}

protected:
	/// \brief Create exception object
	Exception(const char* w = "") throw() :
	what_(w)
	{
	}

	/// \brief Create exception object
	Exception(const std::string& w) throw() :
	what_(w)
	{
	}

	/// \brief explanation of why exception was thrown
	std::string what_;
};


/// \brief Exception thrown when a bad type conversion is attempted.

class LIBTABULA_EXPORT BadConversion : public Exception
{
public:
	const char* type_name;	///< name of type we tried to convert to
	std::string data;		///< string form of data we tried to convert
	size_t retrieved;		///< documentation needed!
	size_t actual_size;		///< documentation needed!

	/// \brief Create exception object, building error string
	/// dynamically
	///
	/// \param tn type name we tried to convert to
	/// \param d string form of data we tried to convert
	/// \param r ??
	/// \param a ??
	BadConversion(const char* tn, const char* d,
			size_t r, size_t a) :
	Exception("Bad type conversion: \""),
	type_name(tn),
	data(d),
	retrieved(r),
	actual_size(a)
	{
		what_ += d ? d : "<NULL>";
		what_ += "\" incompatible with \"";
		what_ += tn;
		what_ += "\" type";
	}

	/// \brief Create exception object, given completed error string
	///
	/// \param w the "what" error string
	/// \param tn type name we tried to convert to
	/// \param d string form of data we tried to convert
	/// \param r ??
	/// \param a ??
	BadConversion(const std::string& w, const char* tn,
				  const char* d, size_t r, size_t a) :
	Exception(w),
	type_name(tn),
	data(d),
	retrieved(r),
	actual_size(a)
	{
	}

	/// \brief Create exception object, with error string only
	///
	/// \param w the "what" error string
	///
	/// All other data members are initialize to default values
	explicit BadConversion(const char* w = "") :
	Exception(w),
	type_name("unknown"),
	data(""),
	retrieved(0),
	actual_size(0)
	{
	}

	/// \brief Destroy exception
	~BadConversion() throw() { }
};


/// \brief Exception thrown when a requested named field doesn't exist.
///
/// Thrown by Row::lookup_by_name() when you pass a field name that
/// isn't in the result set.

class LIBTABULA_EXPORT BadFieldName : public Exception
{
public:
	/// \brief Create exception object
	///
	/// \param bad_field name of field the database server didn't like
	explicit BadFieldName(const char* bad_field) :
	Exception(std::string("Unknown field name: ") + bad_field)
	{
	}

	/// \brief Destroy exception
	~BadFieldName() throw() { }
};


/// \brief Exception thrown when an object with operator [] or an
/// at() method gets called with a bad index.

class LIBTABULA_EXPORT BadIndex : public Exception
{
public:
	/// \brief Create exception object
	///
	/// \param what type of object bad index tried on
	/// \param bad_index index value the container didn't like
	/// \param max_index largest legal index value for container
	explicit BadIndex(const char* what, int bad_index, int max_index) :
	Exception()
	{
		std::ostringstream outs;
		outs << "Index " << bad_index << " on " << what <<
				" out of range, max legal index is " << max_index;
		what_ = outs.str();
	}

	/// \brief Destroy exception
	~BadIndex() throw() { }
};


/// \brief Exception thrown when you pass an unrecognized option to
/// Connection::set_option().

class LIBTABULA_EXPORT BadOption : public Exception
{
public:
	/// \brief Create exception object, taking C string
	explicit BadOption(const char* w, const std::type_info& ti) :
	Exception(w),
	ti_(ti)
	{
	}

	/// \brief Create exception object, taking C++ string
	explicit BadOption(const std::string& w, const std::type_info& ti) :
	Exception(w),
	ti_(ti)
	{
	}

	/// \brief Return type information about the option that failed
	///
	/// Because each option has its own C++ type, this lets you
	/// distinguish among BadOption exceptions programmatically.
	const std::type_info& what_option() const { return ti_; }

private:
	const std::type_info& ti_;
};


/// \brief Exception thrown when not enough query parameters are
/// provided.
///
/// This is used in handling template queries.

class LIBTABULA_EXPORT BadParamCount : public Exception
{
public:
	/// \brief Create exception object
	explicit BadParamCount(const char* w = "") :
	Exception(w)
	{
	}

	/// \brief Destroy exception
	~BadParamCount() throw() { }
};

/// \brief Exception thrown when something goes wrong in processing a
/// "use" query.

class LIBTABULA_EXPORT UseQueryError : public Exception
{
public:
	/// \brief Create exception object
	explicit UseQueryError(const char* w = "") :
	Exception(w)
	{
	}
};


/// \brief Exception thrown when the database server encounters a problem
/// while processing your query.
///
/// Unlike most other libtabula exceptions, which carry just an error
/// message, this type carries an error number to preserve
/// Connection::errnum()'s return value at the point the exception is 
/// thrown.  We do this because when using the Transaction class, the
/// rollback process that occurs during stack unwinding issues a query
/// to the database server, overwriting the error value.  This rollback
/// should always succeed, so this effect can fool code that relies on
/// Connection::errnum() into believing that there was no error.
///
/// Beware that in older versions of libtabula, this was effectively the
/// generic exception type.  (This is most especially true in v1.7.x,
/// but it continued to a lesser extent through the v2.x series.)  When
/// converting old code to new versions of libtabula, it's therefore
/// possible to get seemingly "new" exceptions thrown, which could crash
/// your program if you don't also catch a more generic type like
/// libtabula::Exception or std::exception.

class LIBTABULA_EXPORT BadQuery : public Exception
{
public:
	/// \brief Create exception object
	///
	/// \param w explanation for why the exception was thrown
	/// \param e the error number from the underlying database API
	explicit BadQuery(const char* w = "", int e = 0) :
	Exception(w),
	errnum_(e)
	{
	}

	/// \brief Create exception object
	///
	/// \param w explanation for why the exception was thrown
	/// \param e the error number from the underlying database API
	explicit BadQuery(const std::string& w, int e = 0) :
	Exception(w),
	errnum_(e)
	{
	}

	/// \brief Return the error number corresponding to the error
	/// message returned by what()
	///
	/// This may return the same value as Connection::errnum(), but not
	/// always.  See the overview documentation for this class for the
	/// reason for the difference.
	int errnum() const { return errnum_; }
	
private:	
	int	errnum_;	///< error number associated with execption
};


/// \brief Exception thrown when there is a problem related to the
/// database server connection.
///
/// This is thrown not just on making the connection, but also on
/// shutdown and when calling certain of Connection's methods that
/// require a connection when there isn't one.

class LIBTABULA_EXPORT ConnectionFailed : public Exception
{
public:
	/// \brief Create exception object
	///
	/// \param w explanation for why the exception was thrown
	/// \param e the error number from the underlying database API
	explicit ConnectionFailed(const char* w = "", int e = 0) :
	Exception(w),
	errnum_(e)
	{
	}

	/// \brief Return the error number corresponding to the error
	/// message returned by what(), if any.
	///
	/// If the error number is 0, it means that the error message
	/// doesn't come from the underlying database API, but rather from
	/// libtabula itself.  This happens when an error condition is
	/// detected up at this higher level instead of letting the
	/// underlying database API do it.
	int errnum() const { return errnum_; }
	
private:	
	int	errnum_;	///< error number associated with execption
};


/// \brief Exception thrown when the program tries to select a new
/// database and the database server refuses for some reason.

class LIBTABULA_EXPORT DBSelectionFailed : public Exception
{
public:
	/// \brief Create exception object
	///
	/// \param w explanation for why the exception was thrown
	/// \param e the error number from the underlying database API
	explicit DBSelectionFailed(const char* w = "", int e = 0) :
	Exception(w),
	errnum_(e)
	{
	}

	/// \brief Return the error number corresponding to the error
	/// message returned by what(), if any.
	///
	/// If the error number is 0, it means that the error message
	/// doesn't come from the underlying database API, but rather from
	/// libtabula itself.  This happens when an error condition is
	/// detected up at this higher level instead of letting the
	/// underlying database API do it.
	int errnum() const { return errnum_; }
	
private:	
	int	errnum_;	///< error number associated with execption
};


/// \brief Exception thrown when a BeecryptMutex object fails.

class LIBTABULA_EXPORT MutexFailed : public Exception
{
public:
	/// \brief Create exception object
	explicit MutexFailed(const char* w = "lock failed") :
	Exception(w)
	{
	}
};


/// \brief Exception thrown when you try to use an object that isn't
/// completely initialized.

class LIBTABULA_EXPORT ObjectNotInitialized : public Exception
{
public:
	/// \brief Create exception object
	explicit ObjectNotInitialized(const char* w = "") :
	Exception(w)
	{
	}
};


/// \brief Used within libtabula's test harness only.

class LIBTABULA_EXPORT SelfTestFailed : public Exception
{
public:
	/// \brief Create exception object
	explicit SelfTestFailed(const std::string& w) :
	Exception(w)
	{
	}
};


/// \brief Thrown from the C++ to SQL data type conversion routine when
/// it can't figure out how to map the type.
///
/// This exception is not optional.  The only alternatives when this
/// happens are equally drastic: basically, either iterate past the
/// end of an array (crashing the program) or call assert() to crash
/// the program nicely.  At least this way you have some control over
/// how your program ends.  You can even ignore the error and keep on
/// going: this typically happens when building a SQL query, so you can
/// handle it just the same as if the subsequent query execution failed.

class LIBTABULA_EXPORT TypeLookupFailed : public Exception
{
public:
	/// \brief Create exception object
	explicit TypeLookupFailed(const std::string& w) :
	Exception(w)
	{
	}
};


/// \brief Exception thrown when an insert policy is too strict to
/// create a valid INSERT statement.
///
/// Thrown by Query::insertfrom() if it is unable to add VALUES
/// to an empty query.  This means the size threshold or max packet
/// size of the policy is set too small.

class LIBTABULA_EXPORT BadInsertPolicy : public Exception
{
public:
	/// \brief Create exception object
	explicit BadInsertPolicy(const std::string& w) :
	Exception(w)
	{
	}
};


} // end namespace libtabula

#endif // !defined(LIBTABULA_EXCEPTIONS_H)
