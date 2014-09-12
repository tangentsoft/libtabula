# test/CMakeLists.txt - Tells CMake how to build the libtabula unit tests.
#      See ../CMakeLists.txt for system-wide matters.
#
# Created 2014.09.12 by Warren Young
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

add_executable(test_array_index array_index.cpp)
target_link_libraries(test_array_index tabula mysqlclient_r)

add_executable(test_cpool cpool.cpp)
target_link_libraries(test_cpool tabula mysqlclient_r)

add_executable(test_datetime datetime.cpp)
target_link_libraries(test_datetime tabula mysqlclient_r)

add_executable(test_insertpolicy insertpolicy.cpp)
target_link_libraries(test_insertpolicy tabula mysqlclient_r)

add_executable(test_inttypes inttypes.cpp)
target_link_libraries(test_inttypes tabula mysqlclient_r)

add_executable(test_manip manip.cpp)
target_link_libraries(test_manip tabula mysqlclient_r)

add_executable(test_null_comparison null_comparison.cpp)
target_link_libraries(test_null_comparison tabula mysqlclient_r)

if(TEST_NULL_UNIQUENESS)
	# This test purposely includes bad code that should fail to compile.
	# A thorough platform test will include an attempt to build this,
	# and the test must be considered a failure if it *does* build.  The
	# test is making sure that a build-time check works as expected.
	add_executable(test_null_uniqueness null_uniqueness.cpp)
	target_link_libraries(test_null_uniqueness tabula mysqlclient_r)
endif()

add_executable(test_qssqls qssqls.cpp)
target_link_libraries(test_qssqls tabula mysqlclient_r)

add_executable(test_qstream qstream.cpp)
target_link_libraries(test_qstream tabula mysqlclient_r)

add_executable(test_query_copy query_copy.cpp)
target_link_libraries(test_query_copy tabula mysqlclient_r)

add_executable(test_sqlstream sqlstream.cpp)
target_link_libraries(test_sqlstream tabula mysqlclient_r)

add_executable(test_ssqls2 ssqls2.cpp)
target_link_libraries(test_ssqls2 tabula ssqls2parse mysqlclient_r)

add_executable(test_string string.cpp)
target_link_libraries(test_string tabula mysqlclient_r)

add_executable(test_tcp tcp.cpp)
target_link_libraries(test_tcp tabula mysqlclient_r)

add_executable(test_uds uds.cpp)
target_link_libraries(test_uds tabula mysqlclient_r)

add_executable(test_wnp wnp.cpp)
target_link_libraries(test_wnp tabula mysqlclient_r)
