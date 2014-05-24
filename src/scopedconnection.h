/// \file scopedconnection.h
/// \brief Declares the ScopedConnection class.
///
/// This class lets you grab a connection from a ConnectionPool in a
/// scoped and therefore RAII way.  The Connection object will always be
/// returned to the pool when the scope block ends, plugging a potential
/// leak in the pool.

/***********************************************************************
 Copyright © 2010 by Joel Fielder.  Others may also hold copyrights
 on code in this file.  See the CREDITS.txt file in the top directory
 of the distribution for details.

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

#if !defined(LIBTABULA_SCOPEDCONNECTION_H)
#define LIBTABULA_SCOPEDCONNECTION_H

#include "common.h"

namespace libtabula {

#if !defined(DOXYGEN_IGNORE)
// Make Doxygen ignore this
class LIBTABULA_EXPORT Connection;
class LIBTABULA_EXPORT ConnectionPool;
#endif

/// \brief Grabs a Connection from a ConnectionPool on construction
/// and releases it back to the pool on destruction, and provides access
/// to the relevant Connection pointer.
class LIBTABULA_EXPORT ScopedConnection
{
public:
	/// \brief Standard constructor
	///
	/// Grabs a Connection from the specified pool.
	///
	/// \internal Note that there is no default ctor on purpose.  RAII.
	///
	/// \param pool The ConnectionPool to use.
	/// \param safe By default, we get the connection from the pool with
	/// ConnectionPool::grab(), but we can call safe_grab() instead.
	explicit ScopedConnection(ConnectionPool& pool, bool safe = false);

	/// \brief Destructor
	///
	/// Releases the Connection back to the ConnectionPool.
	~ScopedConnection();

	/// \brief Access the Connection pointer
	Connection* operator->() const { return connection_; }

	/// \brief Dereference
	Connection& operator*() const { return *connection_; }

	/// \brief Truthiness operator
	operator void*() const { return connection_; }

private:
	// ScopedConnection objects cannot be copied.  We want them to be
	// tightly scoped to their use point, not put in containers or
	// passed around promiscuously.
	ScopedConnection(const ScopedConnection& no_copies);   
	const ScopedConnection& operator=(const ScopedConnection& no_copies);

	ConnectionPool& pool_;
	Connection* const connection_;
};

} // end namespace libtabula

#endif // !defined(LIBTABULA_SCOPEDCONNECTION_H)

