/***********************************************************************
 ssqls5.cpp - Example showing how to use the equal_list() member of
	some SSQLS types to build SELECT queries with custom WHERE clauses.

 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB, c
 2004-2009 by Educational Technology Resources, Inc., and © 2005 by
 Chris Frey.  Others may also hold copyrights on code in this file.
 See the CREDITS.md file in the top directory of the distribution
 for details.

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

#include <iostream>
#include <vector>

using namespace std;

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

		// Get all the rows in the stock table.
		libtabula::Query query = con.query("select * from stock");
		vector<stock> res;
		query.storein(res);

		if (res.size() > 0) {
			// Build a select query using the data from the first row
			// returned by our previous query.
			query << "select * from stock where " <<
					res[0].equal_list(" and ", stock_weight, stock_price);

			// Display the finished query.
			cout << "Custom query:\n" << query << endl;
		}
	}
	catch (const libtabula::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const libtabula::BadConversion& er) {
		// Handle bad conversions
		cerr << "Conversion error: " << er.what() << endl <<
				"\tretrieved data size: " << er.retrieved <<
				", actual size: " << er.actual_size << endl;
		return -1;
	}
	catch (const libtabula::Exception& er) {
		// Catch-all for any other libtabula exceptions
		cerr << "Error: " << er.what() << endl;
		return -1;
	}

	return 0;
}
