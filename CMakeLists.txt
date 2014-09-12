# CMakeLists.txt - Tells CMake how to build the libtabula system on 
#      supported platforms.  See */CMakeLists.txt for build details of
#      each element of the system.
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

# ---------------------
#
# TODO:
#
# - Update README-* with new build info
# - Allow separate build/debug building, with VC++ and XCode at least
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
#   - Does FindMySQL allow user to specify a custom location?
#   - Detect existence of <slist> etc.
#   - Thread awareness selection
#   - SSL support detection in mysqlclient
#
# - Add other build options
#   - 32/64 bit
#   - Move --with-field-limit from configure.ac to bootstrap
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
#   - CentOS 7
#   - CentOS 3?
#   - XCode
#   - OpenIndiana
#
# - For each success above with CMake < 2.6, lower minimum requirement
#
# - Convert non-core top-level targets
#   - tags
#   - dist
#   - rpm/deb/ebuild; CPack?
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

set(MYSQL_FIND_REQUIRED 1)
find_package(MySQL)

CHECK_SYMBOL_EXISTS(getopt unistd.h HAVE_POSIX_GETOPT)
if (NOT HAVE_POSIX_GETOPT)
	CHECK_SYMBOL_EXISTS(getopt libiberty.h HAVE_LIBIBERTY_GETOPT)
endif()

CHECK_FUNCTION_EXISTS(localtime_r HAVE_LOCALTIME_R)

configure_file(abi.xml.in abi.xml)
configure_file(install.hta.in install.hta)
configure_file(libtabula.spec.in libtabula.spec)
configure_file(version.in version)
configure_file(doc/userman/userman.dbx.in doc/userman/userman.dbx)
configure_file(src/config.h.in src/config.h)
configure_file(src/Doxyfile.in src/Doxyfile)
configure_file(src/libtabula.h.in src/libtabula.h)
configure_file(src/ssx/Doxyfile.in src/ssx/Doxyfile)

include_directories(src ${PROJECT_BINARY_DIR}/src ${MYSQL_INCLUDE_DIR})
link_directories(${MYSQL_LIBRARY})

add_subdirectory(doc)
add_subdirectory(src)
add_subdirectory(src/ssx)
add_subdirectory(test)
add_subdirectory(examples)

add_custom_target(dtest
	COMMAND ${PROJECT_SOURCE_DIR}/dtest -u test -p test)
