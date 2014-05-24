/***********************************************************************
 tquery3.cpp - Similar to tquery1.cpp but uses unquoted parameters.
	It's here more for code test coverage than because it shows
	something interesting.  We've historically had a problem with
	tqueries with just one parameter; we cover the quoted case in
	resetdb and the other tquery examples, so we get the unquoted
	one here.

 Copyright © 1998 by Kevin Atkinson, © 1999-2001 by MySQL AB, and
 © 2004-2009 by Educational Technology Resources, Inc.  Others may
 also hold copyrights on code in this file.  See the CREDITS.txt file
 in the top directory of the distribution for details.

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

#include "cmdline.h"
#include "printdata.h"

#include <iostream>

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

		// Build a template query to retrieve item names for stock
		// entries with a quantity over some threshold.
		libtabula::Query query = con.query(
				"select item from stock where num > %0");
		query.parse();

		// Get a list of things we have lots of in stock
		if (libtabula::StoreQueryResult res = query.store(80)) {
			cout << "Stuff we have a lot of in stock:" << endl;
			for (size_t i = 0; i < res.num_rows(); ++i) {
				cout << '\t' << res[i]["item"] << endl;
			}
		}
		else {
			cerr << "Failed to get item list: " << query.error() << endl;
			return 1;
		}
	}
	catch (const libtabula::BadQuery& er) {
		// Handle any query errors
		cerr << "Query error: " << er.what() << endl;
		return 2;
	}
	catch (const libtabula::Exception& er) {
		// Catch-all for any other Libtabula exceptions
		cerr << "Error: " << er.what() << endl;
		return 2;
	}

	return 0;
}
