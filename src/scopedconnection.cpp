/***********************************************************************
 scopedconnection.cpp - Implements the ScopedConnection class.

 Copyright © 2010 by Switchplane, Ltd.  Others may also hold
 copyrights on code in this file.  See the CREDITS.txt file in the
 top directory of the distribution for details.

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

#include "scopedconnection.h"

#include "cpool.h"

namespace libtabula {

ScopedConnection::ScopedConnection(ConnectionPool& pool, bool safe) :
pool_(pool),
connection_(safe ? pool.safe_grab() : pool.grab())
{
}

ScopedConnection::~ScopedConnection()
{
    pool_.release(connection_);
}

} // end namespace libtabula
