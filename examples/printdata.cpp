/***********************************************************************
 printdata.cpp - Utility functions for printing out data in common
	formats, required by most of the example programs.

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

#include "printdata.h"

#include <iostream>
#include <iomanip>

using namespace std;


//// print_stock_header ////////////////////////////////////////////////
// Display a header suitable for use with print_stock_rows().

void
print_stock_header(size_t rows)
{
	cout << "Records found: " << rows << endl << endl;
	cout.setf(ios::left);
	cout << setw(31) << "Item" <<
			setw(10) << "Num" <<
			setw(10) << "Weight" <<
			setw(10) << "Price" <<
			"Date" << endl << endl;
}


//// print_stock_row ///////////////////////////////////////////////////
// Print out a row of data from the stock table, in a format compatible
// with the header printed out in the previous function.

void
print_stock_row(const libtabula::sql_char& item, libtabula::sql_bigint num,
		libtabula::sql_double weight, libtabula::sql_decimal_null price,
		const libtabula::sql_date& date)
{
	cout << setw(30) << item << ' ' <<
			setw(9) << num << ' ' <<
			setw(9) << weight << ' ' <<
			setw(9) << price << ' ' <<
			date << endl;
}


//// print_stock_row ///////////////////////////////////////////////////
// Take a Row from the example 'stock' table, break it up into fields,
// and call the above version of this function.

void
print_stock_row(const libtabula::Row& row)
{
	print_stock_row(string(row[0]), row[1], row[2], row[3], row[4]);
}


//// print_stock_rows //////////////////////////////////////////////////
// Print out a number of rows from the example 'stock' table.

void
print_stock_rows(libtabula::StoreQueryResult& res)
{
	print_stock_header(res.size());

	// Use the StoreQueryResult class's read-only random access iterator to walk
	// through the query results.
	libtabula::StoreQueryResult::iterator i;
	for (i = res.begin(); i != res.end(); ++i) {
		// Notice that a dereferenced result iterator can be converted
		// to a Row object, which makes for easier element access.
		print_stock_row(*i);
	}
}


//// print_stock_table /////////////////////////////////////////////////
// Simply retrieve and print the entire contents of the stock table.

void
print_stock_table(libtabula::Query& query)
{
	// Reset query object to its pristine state in case it's been used
	// before by our caller for template queries.
	query.reset();

	// Build the query itself, and show it to the user
	query << "select * from stock";
	cout << "Query: " << query << endl;

	// Execute it, and display the results
	libtabula::StoreQueryResult res = query.store();
	print_stock_rows(res);
}
