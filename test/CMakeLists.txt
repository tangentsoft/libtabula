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

macro(add_test_executable basename)
	add_executable(test_${basename} ${basename}.cpp)
	target_link_libraries(test_${basename} tabula ${MYSQL_C_API_LIBRARY})
	if (CMAKE_USE_PTHREADS_INIT)
		target_link_libraries(test_${basename} pthread)
	endif()
endmacro(add_test_executable)

foreach(basename array_index cpool datetime insertpolicy inttypes manip 
				 null_comparison qssqls qstream sqlstream ssqls2 string
				 tcp uds wnp)
	add_test_executable(${basename})
endforeach(basename)

# Add extra libraries to our needier targets
target_link_libraries(test_ssqls2 ssqls2parse tabula)

if(TEST_NULL_UNIQUENESS)
	# This test purposely includes bad code that should fail to compile.
	# A thorough platform test will include an attempt to build this,
	# and the test must be considered a failure if it *does* build.  The
	# test is making sure that a build-time check works as expected.
	add_test_executable(null_uniqueness)
endif()
