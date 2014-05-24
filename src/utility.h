/// \file utility.h
/// \brief Declares utility functions used within Libtabula
///
/// None of this is meant to be used outside the library itself.  None
/// of this is considered part of the library interface.  It is subject
/// to change at any time, with no notice.

/***********************************************************************
 Copyright © 2009 by Warren Young.  Others may also hold copyrights
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

#if !defined(LIBTABULA_UTILITY_H)
#define LIBTABULA_UTILITY_H

#include "common.h"

#include <cctype>
#include <cstring>
#include <string>

namespace libtabula {
	/// \brief Namespace for holding things used only within Libtabula
	namespace internal {
		/// \brief Lowercase a C++ string in place
		void LIBTABULA_EXPORT str_to_lwr(std::string& s);

		/// \brief Copy a C string into a C++ string, lowercasing
		/// it along the way
		void LIBTABULA_EXPORT str_to_lwr(std::string& ls, const char* mcs);
	} // end namespace libtabula::internal
} // end namespace libtabula

#endif // !defined(LIBTABULA_UTILITY_H)
