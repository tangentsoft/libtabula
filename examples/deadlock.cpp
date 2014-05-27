/***********************************************************************
 deadlock.cpp - Demonstrates how libtabula's deadlock detection interacts
	with libtabula's Transaction class an exception handling mechanism.
	Run one copy of this program with the -m1 command line switch, then
	while it's waiting for you to press Enter, run another copy with -m2
	instead.

 Copyright © 2007 by Jim Wallace and © 2007-2009 by Educational
 Technology Resources, Inc.  Others may also hold copyrights on code
 in this file.  See the CREDITS.txt file in the top directory of the
 distribution for details.

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

#include <libtabula.h>
#include <mysqld_error.h>

#include <iostream>

using namespace std;

// Bring in global holding the value given to the -m switch
extern int run_mode;


int
main(int argc, char *argv[])
{
	// Get database access parameters from command line
	libtabula::examples::CommandLine cmdline(argc, argv);
	if (!cmdline) {
		return 1;
	}

	// Check that the mode parameter was also given and it makes sense
	const int run_mode = cmdline.run_mode();
	if ((run_mode != 1) && (run_mode != 2)) {
		cerr << argv[0] << " must be run with -m1 or -m2 as one of "
				"its command-line arguments." << endl;
		return 1;
	}

	libtabula::Connection con;
	try {
		// Establish the connection to the database server
		libtabula::Connection con(libtabula::examples::db_name,
				cmdline.server(), cmdline.user(), cmdline.pass());

		// Start a transaction set.  Transactions create mutex locks on
		// modified rows, so if two programs both touch the same pair of
		// rows but in opposite orders at the wrong time, one of the two
		// programs will deadlock.  The libtabula server knows how to detect
		// this situation, and its error return causes libtabula to throw
		// a BadQuery exception.  The point of this example is that if
		// you want to detect this problem, you would check the value of
		// BadQuery::errnum(), not Connection::errnum(), because the
		// transaction rollback process executes a query which succeeds,
		// setting the libtabula C API's "last error number" value to 0.
		// The exception object carries its own copy of the error number
		// at the point the exception was thrown for this very reason.
		libtabula::Query query = con.query();
		libtabula::Transaction trans(con);

		// Build and run the queries, with the order depending on the -m
		// flag, so that a second copy of the program will deadlock if
		// run while the first is waiting for Enter.
		char dummy[100];
		for (int i = 0; i < 2; ++i) {
			int lock = run_mode + (run_mode == 1 ? i : -i);
			cout << "Trying lock " << lock << "..." << endl;

			query << "select * from deadlock_test" << lock << 
					" where x = " << lock << " for update";
			query.store();

			cout << "Acquired lock " << lock << ".  Press Enter to ";
			cout << (i == 0 ? "try next lock" : "exit");
			cout << ": " << flush;
			cin.getline(dummy, sizeof(dummy));
		}
	}
	catch (libtabula::BadQuery e) {
		if (e.errnum() == ER_LOCK_DEADLOCK) {
			cerr << "Transaction deadlock detected!" << endl;
			cerr << "Connection::errnum = " << con.errnum() <<
					", BadQuery::errnum = " << e.errnum() << endl;
		}
		else {
			cerr << "Unexpected query error: " << e.what() << endl;
		}
		return 1;
	}
	catch (libtabula::Exception e) {
		cerr << "General error: " << e.what() << endl;		
		return 1;
	}

	return 0;
}
