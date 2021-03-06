/***********************************************************************
 beemutex.cpp - Implements the BeecryptMutex class.  The name comes
	from the fact that we lifted this essentially intact from the
	Beecrypt library, which is also LGPL.  See beecrypt.h for the list
	of changes we made on integrating it into libtabula.

 Copyright © 2004 Beeyond Software Holding BV and © 2007, 2018 by
 Educational Technology Resources, Inc.  Others may also hold
 copyrights on code in this file.  See the CREDITS.md file in the
 top directory of the distribution for details.

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
#include "beemutex.h"

#include "common.h"

#include <errno.h>
#include <string.h>


namespace libtabula {

#define ACTUALLY_DOES_SOMETHING
#if defined(HAVE_PTHREAD)
	typedef pthread_mutex_t bc_mutex_t;
#elif defined(HAVE_SYNCH_H)
#	include <synch.h>
	typedef mutex_t bc_mutex_t;
#elif defined(LIBTABULA_PLATFORM_WINDOWS)
	typedef HANDLE bc_mutex_t;
#else
// No supported mutex type found, so class becomes a no-op.
#	undef ACTUALLY_DOES_SOMETHING
#endif

#if defined(ACTUALLY_DOES_SOMETHING)
	static bc_mutex_t* impl_ptr(void* p)
			{ return static_cast<bc_mutex_t*>(p); }
#	if defined(LIBTABULA_PLATFORM_WINDOWS)
		static bc_mutex_t impl_val(void* p)
				{ return *static_cast<bc_mutex_t*>(p); }
#	endif
#endif


BeecryptMutex::BeecryptMutex()
#if defined(ACTUALLY_DOES_SOMETHING)
	: pmutex_(new bc_mutex_t)
#endif
{
#if defined(LIBTABULA_PLATFORM_WINDOWS)
	*impl_ptr(pmutex_) = CreateMutex((LPSECURITY_ATTRIBUTES) 0, FALSE,
			(LPCTSTR) 0);
	if (!impl_val(pmutex_))
		throw MutexFailed("CreateMutex failed");
#else
#	if HAVE_SYNCH_H || HAVE_PTHREAD
	register int rc;
#	endif
#	if HAVE_PTHREAD
		if ((rc = pthread_mutex_init(impl_ptr(pmutex_), 0)))
			throw MutexFailed(strerror(rc));
#	elif HAVE_SYNCH_H
		if ((rc = mutex_init(impl_ptr(pmutex_), USYNC_THREAD, 0)))
			throw MutexFailed(strerror(rc));
#	endif
#endif
}


BeecryptMutex::~BeecryptMutex()
{
#if defined(ACTUALLY_DOES_SOMETHING)
#	if defined(LIBTABULA_PLATFORM_WINDOWS)
		CloseHandle(impl_val(pmutex_));
#	elif HAVE_PTHREAD
		pthread_mutex_destroy(impl_ptr(pmutex_));
#	elif HAVE_SYNCH_H
		mutex_destroy(impl_ptr(pmutex_));
#	endif

	delete impl_ptr(pmutex_);
#endif
}


void
BeecryptMutex::lock()
{
#if defined(LIBTABULA_PLATFORM_WINDOWS)
	if (WaitForSingleObject(impl_val(pmutex_), INFINITE) == WAIT_OBJECT_0)
		return;
	throw MutexFailed("WaitForSingleObject failed");
#else
#	if HAVE_SYNCH_H || HAVE_PTHREAD
	register int rc;
#	endif
#	if HAVE_PTHREAD
		if ((rc = pthread_mutex_lock(impl_ptr(pmutex_))))
			throw MutexFailed(strerror(rc));
#	elif HAVE_SYNCH_H
		if ((rc = mutex_lock(impl_ptr(pmutex_))))
			throw MutexFailed(strerror(rc));
#	endif
#endif
}


bool
BeecryptMutex::trylock()
{
#if defined(ACTUALLY_DOES_SOMETHING)
#	if defined(LIBTABULA_PLATFORM_WINDOWS)
		switch (WaitForSingleObject(impl_val(pmutex_), 0)) {
			case WAIT_TIMEOUT:
				return false;
			case WAIT_OBJECT_0:
				return true;
			default:
				throw MutexFailed("WaitForSingleObbject failed");
		}
#	else
		register int rc;
#		if HAVE_PTHREAD
			if ((rc = pthread_mutex_trylock(impl_ptr(pmutex_))) == 0)
				return true;
			if (rc == EBUSY)
				return false;
			throw MutexFailed(strerror(rc));
#		elif HAVE_SYNCH_H
			if ((rc = mutex_trylock(impl_ptr(pmutex_))) == 0)
				return true;
			if (rc == EBUSY)
				return false;
			throw MutexFailed(strerror(rc));
#		endif
#	endif
#else
	return true;		// no-op build, so always succeed
#endif
}


void
BeecryptMutex::unlock()
{
#if defined(LIBTABULA_PLATFORM_WINDOWS)
	if (!ReleaseMutex(impl_val(pmutex_)))
		throw MutexFailed("ReleaseMutex failed");
#else
#	if HAVE_SYNCH_H || HAVE_PTHREAD
		register int rc;
#	endif
#	if HAVE_PTHREAD
		if ((rc = pthread_mutex_unlock(impl_ptr(pmutex_))))
			throw MutexFailed(strerror(rc));
#	elif HAVE_SYNCH_H
		if ((rc = mutex_unlock(impl_ptr(pmutex_))))
			throw MutexFailed(strerror(rc));
#	endif
#endif
}

} // end namespace libtabula

