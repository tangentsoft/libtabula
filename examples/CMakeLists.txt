# CMakeLists.txt - Tells CMake how to build the libtabula examples on
#      supported platforms.  See ../CMakeLists.txt for system-wide
#      matters.
#
# Created 2014.08.21 by Warren Young
#
# Copyright © 2014-2015 by Warren Young
# Copyright © 2015 by Educational Technology Resources, Inc.
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

set(STOCK_TXT "${CMAKE_CURRENT_BINARY_DIR}/stock.txt")
if (NOT EXISTS ${STOCK_TXT})
	# CMake 2.8.0 added file(COPY), but we need to support 2.6, so we
	# must hand-roll a "copy" command.
	file(READ "stock.txt" ST_CONTENTS)
	file(WRITE ${STOCK_TXT} "${ST_CONTENTS}")
	message(STATUS "Copied stock.txt to build dir so ssqls6 will run")
endif()

add_library(ltexcomm STATIC printdata.cpp)

macro(add_example_executable basename)
	add_executable(${basename} ${basename}.cpp)
	target_link_libraries(${basename} ltexcomm tabula ${MYSQL_C_API_LIBRARY})
	if (CMAKE_USE_PTHREADS_INIT)
		target_link_libraries(${basename} pthread)
	endif()
endmacro(add_example_executable)

foreach(basename resetdb

				 cgi_jpeg load_jpeg

				 simple1 simple2 simple3
				 ssqls1 ssqls2 ssqls3 ssqls4 ssqls5 ssqls6
				 tquery1 tquery2 tquery3 tquery4

				 cpool dbinfo deadlock fieldinf for_each multiquery
				 store_if transaction
				 )
	add_example_executable(${basename})
endforeach(basename)
