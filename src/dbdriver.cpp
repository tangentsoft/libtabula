/***********************************************************************
 dbdriver.cpp - Implements the DBDriver class.

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
#include "dbdriver.h"

//#include "exceptions.h"

//#include <cstring>
//#include <memory>
//#include <sstream>

using namespace std;

namespace libtabula {

DBDriver::DBDriver() :
is_connected_(false),
option_error_(Option::err_NONE)
{
}


DBDriver::~DBDriver()
{
	OptionList::const_iterator it;
	for (it = applied_options_.begin(); it != applied_options_.end(); ++it) {
		delete *it;
	}
}


bool
DBDriver::apply_pending_options()
{
	option_error_ = Option::err_NONE;
	for (OptionListIt it = pending_options_.begin();
			it != pending_options_.end(); ++it) {
		if (!set_option(*it)) return false;
	}

	// The pending Option object pointers got moved to applied_options_,
	// so don't delete them.  They'll get deleted in ~DBDriver().
	pending_options_.clear();
	return true;
}


bool
DBDriver::connect(const char*, const char*, unsigned int, 
		const char*, const char*, const char*)
{
	return is_connected_ && apply_pending_options();
}


// High-level option setting mechanism, called by Connection::set_option()
// and by apply_pending_options().  Lower levels are in the leaf classes.

bool
DBDriver::set_option(Option* o)
{
	// Ensure o gets destroyed if there is a fatal error or exception
	std::auto_ptr<Option> cleanup(o);

	option_error_ = o->set(this);
	if (option_error_ == Option::err_NONE) {
		// It was applied successfully, so save it until destruction of
		// the driver so we know what options have already been applied.
		applied_options_.push_back(o);
		cleanup.release();
		return true;
	}
	else if (option_error_ == Option::err_disconnected) {
		// We can't set this while disconnected, so queue it up to be
		// applied when the connection does come up.  We will return
		// true since there hasn't been a failure per se, but caller can
		// call option_error() to distinguish this case from previous.
        pending_options_.push_back(o);
		cleanup.release();
		return true;
	}
	else {
		return false;
	}
}

} // end namespace libtabula

