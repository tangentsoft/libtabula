/***********************************************************************
 test/tcp.cpp - Tests the address parser/verifier in TCPConnection.

 Copyright © 2007 by Educational Technology Resources, Inc.
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

#include <exceptions.h>
#include <tcp_connection.h>

#include <iostream>
#include <sstream>


static void
test(const char* addr_svc, unsigned int port, const char* exp_addr,
		unsigned int exp_port)
{
	std::string addr(addr_svc), error;

	libtabula::TCPConnection::parse_address(addr, port, error);
	if (error.size()) {
		throw libtabula::SelfTestFailed("TCP address parse error: " +
				error);
	}
	else if (addr.compare(exp_addr) != 0) {
		std::ostringstream outs;
		outs << "TCP address parse mismatch: '" << addr << "' != '" <<
				exp_addr << "'";
		throw libtabula::SelfTestFailed(outs.str());
	}
	else if (port != exp_port) {
		std::ostringstream outs;
		outs << "TCP port parse mismatch: '" << port << "' != '" <<
				exp_port << "'";
		throw libtabula::SelfTestFailed(outs.str());
	}
}


static void
fail(const char* addr_svc, unsigned int port, const char* exp_addr,
		unsigned int exp_port)
{
	try {
		test(addr_svc, port, exp_addr, exp_port);
	}
	catch (...) {
		return;		// eat expected error
	}

	std::ostringstream outs;
	outs << "'" << addr_svc << "' == ('" << exp_addr <<
			"', " << exp_port << ") but should not.";
	throw libtabula::SelfTestFailed(outs.str());
}


int
main()
{
	try {
		// Domain name and IPv4 literal tests
		test(":", 0, "", 0);
		test("1.2.3.4", 0, "1.2.3.4", 0);
		test("1.2.3.4:", 0, "1.2.3.4", 0);
		test("1.2.3.4:567", 0, "1.2.3.4", 567);
		test("1.2.3.4", 890, "1.2.3.4", 890);
		test("1.2.3.4:telnet", 0, "1.2.3.4", 23);
		test("a.b.com", 0, "a.b.com", 0);
		test("a.b.com", 987, "a.b.com", 987);
		fail("@", 0, "@", 0);
		fail("::", 0, "", 0);
		fail(":", 0, "1.2.3.4", 45);
		fail("a.b.com::", 0, "a.b.com", 0);
		fail("a.b:com:1", 0, "a.b.com", 1);

		// IPv6 literal tests
		test("[]:123", 0, "", 123);
		test("[::]:telnet", 0, "::", 23);

		std::cout << "TCP address parsing passed." << std::endl;
		return 0;
	}
	catch (libtabula::SelfTestFailed& e) {
		std::cerr << "TCP address parse error: " << e.what() << std::endl;
		return 1;
	}
	catch (std::exception& e) {
		std::cerr << "Unexpected test failure: " << e.what() << std::endl;
		return 2;
	}
}
