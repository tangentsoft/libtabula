/***********************************************************************
 test/inttypes.cpp - Tests whether the integer typedef equivalents for
	SQL types in lib/sql_types.h are correct on this system.  If not,
	you need to change either that file, lib/common.h, or both.

 Copyright © 2008 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the
 CREDITS.txt file in the top directory of the distribution for details.

 This file is part of libtabula

 Libtabula++ is free software; you can redistribute it and/or modify it
 under the terms of the GNU Lesser General Public License as published
 by the Free Software Foundation; either version 2.1 of the License, or
 (at your option) any later version.

 Libtabula++ is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with Libtabula++; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
 USA
***********************************************************************/

#include <sql_types.h>

#include <iostream>

template <typename IntType>
static bool
test_size(const char* desc, IntType value, size_t expected_size)
{
	size_t actual_size = sizeof(value);
	if (actual_size == expected_size) {
		return true;
	}
	else {
		std::cerr << desc << " is sized incorrectly on this "
				"platform:" << std::endl << "\t" << actual_size <<
				" bytes, not " << expected_size << " as expected." <<
				std::endl;
		return false;
	}
}


int
main()
{
	int failures = 0;

	failures += test_size("sql_tinyint",
			libtabula::sql_tinyint(0), 1) == false;
	failures += test_size("sql_tinyint_unsigned",
			libtabula::sql_tinyint_unsigned(0), 1) == false;
	failures += test_size("sql_smallint",
			libtabula::sql_smallint(0), 2) == false;
	failures += test_size("sql_smallint_unsigned",
			libtabula::sql_smallint_unsigned(0), 2) == false;
	failures += test_size("sql_mediumint",
			libtabula::sql_mediumint(0), 4) == false;
	failures += test_size("sql_mediumint_unsigned",
			libtabula::sql_mediumint_unsigned(0), 4) == false;
	failures += test_size("sql_int",
			libtabula::sql_int(0), 4) == false;
	failures += test_size("sql_int_unsigned",
			libtabula::sql_int_unsigned(0), 4) == false;
	failures += test_size("sql_bigint",
			libtabula::sql_bigint(0), 8) == false;
	failures += test_size("sql_bigint_unsigned",
			libtabula::sql_bigint_unsigned(0), 8) == false;
	
	return failures;
}

