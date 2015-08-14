/***********************************************************************
 simple1.cpp - Example showing the simplest way to get data from a libtabula
	table with libtabula.

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

#include <libtabula.h>

#include <iostream>
#include <iomanip>

using namespace std;

int
main(int argc, char *argv[])
{
	// Get database access parameters from command line
	libtabula::examples::CommandLine cmdline(argc, argv);
	if (!cmdline) {
		return 1;
	}

	// Connect to the sample database.
	libtabula::Connection conn(false);
	if (conn.connect(libtabula::examples::db_name, cmdline.server(),
			cmdline.user(), cmdline.pass())) {
		// Retrieve a subset of the sample stock table set up by resetdb
		// and display it.
		libtabula::Query query = conn.query("select item from stock");
		if (libtabula::StoreQueryResult res = query.store()) {
			cout << "We have:" << endl;
			libtabula::StoreQueryResult::const_iterator it;
			for (it = res.begin(); it != res.end(); ++it) {
				libtabula::Row row = *it;
				cout << '\t' << row[0] << endl;
			}
		}
		else {
			cerr << "Failed to get item list: " << query.error() << endl;
			return 1;
		}

		return 0;
	}
	else {
		cerr << "DB connection failed: " << conn.error() << endl;
		return 1;
	}
}
