/***********************************************************************
 test/insertpolicy.cpp - Checks that the *InsertPolicy objects work
 	as expected.

 Copyright © 2009 by Educational Technology Resources, Inc.
 Others may also hold copyrights on code in this file.  See the
 CREDITS.md file in the top directory of the distribution for details.

 This file is part of libtabula

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

#include <libtabula.h>

#include <climits>
#include <iostream>

static const unsigned char nonzero = 4;


template <class InsertPolicy>
static bool
test_policy(InsertPolicy& ip, unsigned char max, bool expected_to_fail)
{
	unsigned char i;
	libtabula::Row dummy;
	for (i = 0; i < UCHAR_MAX; ++i) {
		if (!ip.can_add(i, dummy)) {
			break;
		}
	}

	if (expected_to_fail ? i != max : i == max) {
		return true;
	}
	else {
		std::cerr << typeid(ip).name() << '(' << int(max) << 
				") allowed " << int(i) << " inserts!" << std::endl;
		return false;
	}
}


template <class InsertPolicy>
static bool
test_policy(InsertPolicy& ip, unsigned char expected_allow_count)
{
	return	test_policy(ip, expected_allow_count, false) &&
			test_policy(ip, expected_allow_count + 1, true) &&
			test_policy(ip, expected_allow_count - 1, true) &&
			test_policy(ip, expected_allow_count ? 0 : nonzero, true);
}


static bool
test_row_count_nonzero()
{
	libtabula::Query::RowCountInsertPolicy<> ip_nonzero(nonzero);
	return test_policy(ip_nonzero, nonzero);
}


static bool
test_row_count_zero()
{
	libtabula::Query::RowCountInsertPolicy<> ip_zero(0);
	return test_policy(ip_zero, 0);
}


static bool
test_row_count()
{
	return	test_row_count_nonzero() &&
			test_row_count_zero();
}


int
main()
{
	try {
		return test_row_count() ? 0 : 1;
	}
	catch (...) {
		std::cerr << "Unhandled exception caught by "
				"test/insertpolicy!" << std::endl;
		return 2;
	}
}

