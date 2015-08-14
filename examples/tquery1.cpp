/***********************************************************************
 tquery1.cpp - Example similar to ssqls3.cpp, except that it uses
	template queries instead of SSQLS.

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

		// Build a template query to retrieve a stock item given by
		// item name.
		libtabula::Query query = con.query(
				"select * from stock where item = %0q");
		query.parse();

		// Retrieve an item added by resetdb; it won't be there if
		// tquery* or ssqls3 is run since resetdb.
		libtabula::StoreQueryResult res1 = query.store("Nürnberger Brats");
		if (res1.empty()) {
			throw libtabula::BadQuery("UTF-8 bratwurst item not found in "
					"table, run resetdb");
		}

		// Replace the proper German name with a 7-bit ASCII
		// approximation using a different template query.
		query.reset();		// forget previous template query data
		query << "update stock set item = %0q where item = %1q";
		query.parse();
		libtabula::SimpleResult res2 = query.execute("Nuerenberger Bratwurst",
				res1[0][0].c_str());

		// Print the new table contents.
		print_stock_table(query);
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
