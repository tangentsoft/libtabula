/***********************************************************************
 store_if.cpp - Demonstrates Query::store_if(), showing only the rows
	from the sample table with prime quantities.  This isn't intended
	to be useful, only to show how you can do result set filtering that
	outstrips the power of SQL.

 Copyright © 2005-2010 by Educational Technology Resources, Inc.
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

#include "cmdline.h"
#include "printdata.h"
#include "stock.h"

#include <libtabula.h>

#include <iostream>

#include <math.h>


// Define a functor for testing primality.
struct is_prime
{
	bool operator()(const stock& s)
	{
		if ((s.num == 2) || (s.num == 3)) {
			return true;	// 2 and 3 are trivial cases
		}
		else if ((s.num < 2) || ((s.num % 2) == 0)) {
			return false;	// can't be prime if < 2 or even
		}
		else {
			// The only possibility left is that it's divisible by an
			// odd number that's less than or equal to its square root.
			for (int i = 3; i <= sqrt(double(s.num)); i += 2) {
				if ((s.num % i) == 0) {
					return false;
				}
			}
			return true;
		}
	}
};


int
main(int argc, char *argv[])
{
	// Get database access parameters from command line
	libtabula::examples::CommandLine cmdline(argc, argv);
	if (!cmdline) {
		return 1;
	}

	try {
		// Establish the connection to the database server.
		libtabula::Connection con(libtabula::examples::db_name,
				cmdline.server(), cmdline.user(), cmdline.pass());

		// Collect the stock items with prime quantities
		std::vector<stock> results;
		libtabula::Query query = con.query();
		query.store_if(results, stock(), is_prime());

		// Show the results
		print_stock_header(results.size());
		std::vector<stock>::const_iterator it;
		for (it = results.begin(); it != results.end(); ++it) {
			print_stock_row(it->item.c_str(), it->num, it->weight,
					it->price, it->sDate);
		}
	}
	catch (const libtabula::BadQuery& e) {
		// Something went wrong with the SQL query.
		std::cerr << "Query failed: " << e.what() << std::endl;
		return 1;
	}
	catch (const libtabula::Exception& er) {
		// Catch-all for any other libtabula exceptions
		std::cerr << "Error: " << er.what() << std::endl;
		return 1;
	}

	return 0;
}
