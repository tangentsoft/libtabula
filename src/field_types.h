/// \file field_types.h
/// \brief Declares a class to hold a list of SQL field type info.

/***********************************************************************
 Copyright © 1998 by Kevin Atkinson, © 1999, 2000 and 2001 by
 MySQL AB, and © 2004-2007 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the CREDITS
 file in the top directory of the distribution for details.

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

#ifndef LIBTABULA_FIELD_TYPES_H
#define LIBTABULA_FIELD_TYPES_H

#include "field_type.h"

#include <vector>

namespace libtabula {

#if !defined(DOXYGEN_IGNORE)
// Make Doxygen ignore this
class LIBTABULA_EXPORT ResultBase;
#endif

/// \brief A vector of SQL field types.
class FieldTypes : public std::vector<FieldType>
{
public:
	/// \brief Default constructor
	FieldTypes() { }
	
	/// \brief Create list of field types from a result set
	FieldTypes(const ResultBase* res)
	{
		init(res);
	}

	/// \brief Create fixed-size list of uninitialized field types
	FieldTypes(int i) :
	std::vector<FieldType>(i)
	{
	}

	/// \brief Initialize field list based on a result set
	FieldTypes& operator =(const ResultBase* res)
	{
		init(res);
		return *this;
	}

	/// \brief Insert a given number of uninitialized field type
	/// objects at the beginning of the list
	///
	/// \param i number of field type objects to insert
	FieldTypes& operator =(int i)
	{
		insert(begin(), i, FieldType());
		return *this;
	}

private:
	void init(const ResultBase* res);
};

} // end namespace libtabula

#endif
