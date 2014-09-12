# src/CMakeLists.txt - Tells CMake how to build libtabula proper
#    See ../CMakeLists.txt for system-wide matters.
#
# Created 2014.08.21 by Warren Young
#
# Copyright © 2014 by Warren Young
#
# Others may also hold copyrights on code in this file.  See the
# CREDITS.md file in the top directory of the distribution for details.
#
# This file is part of libtabula.
#
# libtabula is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation; either version 2.1 of the License, or
# (at your option) any later version.
#
# libtabula is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
# License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with libtabula; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
# USA

set(QUERYDEF_PL "${CMAKE_SOURCE_DIR}/src/querydef.pl")
add_custom_command(OUTPUT querydef.h
	COMMAND ${QUERYDEF_PL}
	DEPENDS ${QUERYDEF_PL}
	COMMENT "Generating querydef.h..."
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/src")

set(SSQLS_PL "${CMAKE_SOURCE_DIR}/src/ssqls.pl")
add_custom_command(OUTPUT ssqls.h
	COMMAND ${SSQLS_PL}
	DEPENDS ${SSQLS_PL}
	COMMENT "Generating ssqls.h..."
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/src")

add_library(tabula SHARED
    beemutex.cpp
    cmdline.cpp
    connection.cpp
    cpool.cpp
    datetime.cpp
    dbdriver.cpp
    field_names.cpp
    field_types.cpp
    libtabula.cpp
    manip.cpp
    myset.cpp
    mystring.cpp
	mysql/driver.cpp
    null.cpp
    options.cpp
    qparms.cpp
    query.cpp
	querydef.h
    result.cpp
    row.cpp
    scopedconnection.cpp
    sql_buffer.cpp
    sqlstream.cpp
    ssqls.h
    ssqls2.cpp
    stadapter.cpp
    tcp_connection.cpp
    transaction.cpp
    type_info.cpp
    uds_connection.cpp
    utility.cpp
    vallist.cpp
    wnp_connection.cpp
)
target_link_libraries(tabula mysqlclient_r)
