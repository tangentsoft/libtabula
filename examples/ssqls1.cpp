/***********************************************************************
 ssqls1.cpp - Example that produces the same results as simple1, but it
	uses a Specialized SQL Structure to store the results instead of a
	libtabula Result object.
 
 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB, and
 © 2004-2009 by Educational Technology Resources, Inc.  Others may
 also hold copyrights on code in this file.  See the CREDITS.md file
 in the top directory of the distribution for details.

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

		// Retrieve a subset of the stock table's columns, and store
		// the data in a vector of 'stock' SSQLS structures.  See the
		// user manual for the consequences arising from this quiet
		// ability to store a subset of the table in the stock SSQLS.
		libtabula::Query query = con.query("select item,description from stock");
		vector<stock> res;
		query.storein(res);

		// Display the items
		cout << "We have:" << endl;
		vector<stock>::iterator it;
		for (it = res.begin(); it != res.end(); ++it) {
			cout << '\t' << it->item;
			if (it->description != libtabula::null) {
				cout << " (" << it->description << ")";
			}
			cout << endl;
		}
	}
	catch (const libtabula::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return -1;
	}
	catch (const libtabula::BadConversion& er) {
		// Handle bad conversions; e.g. type mismatch populating 'stock'
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
