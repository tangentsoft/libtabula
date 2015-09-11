# CMakeLists.txt - Tells CMake how to build the libtabula system on 
#      supported platforms.  See */CMakeLists.txt for build details of
#      each element of the system.
#
# Created 2014.08.21 by Warren Young
#
# Copyright
#   © 2014-2015 by Warren Young
#   © 2015 by Educational Technology Resources, Inc.
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

# ---------------------
#
# TODO:
#
# - Replace old bootstrap script verbs with CMake options:
#   - nodoc
#   - noex
#   - nolib
#   - noopt
#   - notest
#   - pedantic
#
# - Replicate existing autoconf behavior:
#   - Detect existence of <slist> etc.
#   - SSL support detection in mysqlclient
#
# - Add other build options
#   - Add analog of --with-field-limit to src/CMakeLists.txt, modifying
#     the calls to querydef.pl and ssqls.pl
#   
# - Try to get rid of install.hta on Windows, in favor of CMake's
#   own installation mechanism.
#
# - Bring examples/vstudio into the build system
#
# - Get it working on:
#   - Visual C++ 2003
#   - Visual C++ 2005
#   - Visual C++ 2008 (32- and 64-bit)
#   - Visual C++ 2010 Express
#   - Visual C++ 2012 Express
#   - Visual C++ 2013 (32- and 64-bit)
#   - MinGW
#   - Cygwin
#   - CentOS 3?
#   - OpenIndiana
#
# - For each success above with CMake < 2.6, lower minimum requirement
#
# - Convert non-core top-level targets
#   - dist/rpm/deb/ebuild... via CPack
#   - abicheck
#
# ---------------------

cmake_minimum_required(VERSION 2.6)

include(CheckSymbolExists)
include(CheckFunctionExists)

project(libtabula)
set(LIBTABULA_VERSION_MAJOR  3)
set(LIBTABULA_VERSION_MINOR  9)
set(LIBTABULA_VERSION_BUGFIX 9)
set(LIBTABULA_VERSION_TWEAK  1)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

# Try to find some kind of "stdint" type header.
find_path(LIBTABULA_HAVE_CSTDINT cstdint)
if (LIBTABULA_HAVE_CSTDINT)
	message("found C++11's <cstdint> header")
else()
	find_path(LIBTABULA_HAVE_TR1_CSTDINT tr1/cstdint)
	if (LIBTABULA_HAVE_TR1_CSTDINT)
		message("found <tr1/cstdint>")
	else()
		find_path(LIBTABULA_HAVE_BOOST_CSTDINT boost/cstdint.hpp)
		if (LIBTABULA_HAVE_BOOST_CSTDINT)
			message("found <boost/cstdint.hpp>")
		else()
			find_path(LIBTABULA_HAVE_STDINT_H stdint.h)
			if (LIBTABULA_HAVE_STDINT_H)
				message("found C99's <stdint.h>; hope we can use it in C++")
			else()
				message(SEND_ERROR "WARNING: no 'stdint' type header found!")
			endif()
		endif()
	endif()
endif()

set(MYSQL_FIND_REQUIRED 1)
find_package(MySQL)

find_package(Threads)
if (CMAKE_HAVE_THREADS_LIBRARY)
	set(MYSQL_C_API_LIBRARY mysqlclient_r)
else()
	set(MYSQL_C_API_LIBRARY mysqlclient)
endif()

CHECK_SYMBOL_EXISTS(getopt unistd.h HAVE_POSIX_GETOPT)
if (NOT HAVE_POSIX_GETOPT)
	CHECK_SYMBOL_EXISTS(getopt libiberty.h HAVE_LIBIBERTY_GETOPT)
endif()

CHECK_FUNCTION_EXISTS(localtime_r HAVE_LOCALTIME_R)

include_directories(src ${PROJECT_BINARY_DIR}/src ${MYSQL_INCLUDE_DIR})
get_filename_component(MYSQL_LIBRARY_DIR "${MYSQL_LIBRARY}" PATH)
link_directories(${MYSQL_LIBRARY_DIR})

add_subdirectory(cmake)
add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(src/ssx)
add_subdirectory(test)
add_subdirectory(examples)

# Check for specific post-C++98 features.  Must be after cmake subdir.
CXX_CHECK_FEATURE("long long" HAVE_CXX_LONG_LONG)
CXX_CHECK_FEATURE("cbegin and cend" HAVE_CXX_CBEGIN_CEND)

# Two testing targets based on the dtest script, one running it
# standalone, one under CTest.  The latter suppresses output and 
# only reports pass/fail.  The dtest target can't depend on "all"
# due to a CMake design weakness, while "test" does implicitly,
# since it's integrated into the CMake design.
add_custom_target(dtest
	COMMAND ${PROJECT_SOURCE_DIR}/dtest -u test -p test)
include(CTest)
if (BUILD_TESTING)
    add_test(dtest ${PROJECT_SOURCE_DIR}/dtest -u test -p test)
endif(BUILD_TESTING)

# If ctags is present, add a top-level "tags" target which includes all
# files in both the build and source versions of src/*.  Conditional
# since it isn't always present on *ix, and almost never on Windows.
find_program(CTAGS_PATH ctags)
if (CTAGS_PATH)
    message(STATUS "Found ctags: ${CTAGS_PATH}")
	set_source_files_properties(tags PROPERTIES GENERATED true)
	add_custom_target(tags 
			COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q 
				${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
			COMMENT "Generating tags file..."
			WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
endif(CTAGS_PATH)

configure_file(abi.xml.in abi.xml)
configure_file(install.hta.in install.hta)
configure_file(libtabula.spec.in libtabula.spec)
configure_file(version.in version)
configure_file(doc/userman/userman.dbx.in doc/userman/userman.dbx)
configure_file(src/config.h.in src/config.h)
configure_file(src/Doxyfile.in src/Doxyfile)
configure_file(src/libtabula.h.in src/libtabula.h)
configure_file(src/ssx/Doxyfile.in src/ssx/Doxyfile)
