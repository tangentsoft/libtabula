/***********************************************************************
 cgi_jpeg.cpp - Example code showing how to fetch JPEG data from a BLOB
 	column and send it back to a browser that requested it by ID.
	
	Use load_jpeg.cpp to load JPEG files into the database we query.

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
#include "images.h"

int
main(int argc, char* argv[])
{
	// Get database access parameters from command line if present, else
	// use hard-coded values for true CGI case.
	libtabula::examples::CommandLine cmdline(argc, argv, "root",
			"nunyabinness");
	if (!cmdline) {
		return 1;
	}

	// Define line ending style used in output, varying depending on
	// whether the output is CGI/HTTP or the bmark.txt file for dtest.
	const char* eol1 = cmdline.dtest_mode() ? "\n" : "\r\n";
	const char* eol2 = cmdline.dtest_mode() ? "\n" : "\r\n\r\n";

	// Parse CGI query string environment variable to get image ID
	unsigned int img_id = 0;
	char* cgi_query = getenv("QUERY_STRING");
	if (cgi_query) {
		if ((strlen(cgi_query) < 4) || memcmp(cgi_query, "id=", 3)) {
			std::cout << "Content-type: text/plain" << std::endl << std::endl;
			std::cout << "ERROR: Bad query string" << std::endl;
			return 1;
		}
		else {
			img_id = atoi(cgi_query + 3);
		}
	}
	else {
		std::cerr << "Put this program into a web server's cgi-bin "
				"directory, then" << std::endl;
		std::cerr << "invoke it with a URL like this:" << std::endl;
		std::cerr << std::endl;
		std::cerr << "    http://server.name.com/cgi-bin/cgi_jpeg?id=2" <<
				std::endl;
		std::cerr << std::endl;
		std::cerr << "This will retrieve the image with ID 2." << std::endl;
		std::cerr << std::endl;
		std::cerr << "You will probably have to change some of the #defines "
				"at the top of" << std::endl;
		std::cerr << "examples/cgi_jpeg.cpp to allow the lookup to work." <<
				std::endl;
		return 1;
	}

	// Retrieve image from DB by ID
	try {
		libtabula::Connection con(libtabula::examples::db_name,
				cmdline.server(), cmdline.user(), cmdline.pass());
		libtabula::Query query = con.query();
		query << "SELECT * FROM images WHERE id = " << img_id;
		libtabula::StoreQueryResult res = query.store();
		if (res && res.num_rows()) {
			images img = res[0];
			if (img.data.is_null) {
				std::cout << "Content-type: text/plain" << eol2;
				std::cout << "No image content!" << eol1;
			}
			else {
				std::cout << "X-Image-Id: " << img_id << eol1; // for debugging
				std::cout << "Content-type: image/jpeg" << eol1;
				std::cout << "Content-length: " <<
						img.data.data.length() << eol2;
				std::cout << img.data;
			}
		}
		else {
			std::cout << "Content-type: text/plain" << eol2;
			std::cout << "ERROR: No image with ID " << img_id << eol1;
		}
	}
	catch (const libtabula::BadQuery& er) {
		// Handle any query errors
		std::cout << "Content-type: text/plain" << eol2;
		std::cout << "QUERY ERROR: " << er.what() << eol1;
		return 1;
	}
	catch (const libtabula::Exception& er) {
		// Catch-all for any other libtabula exceptions
		std::cout << "Content-type: text/plain" << eol2;
		std::cout << "GENERAL ERROR: " << er.what() << eol1;
		return 1;
	}

	return 0;
}
