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
# - Add libssqlsxlat convenience library
# - Add libexcommon convenience library
# - Add examples/*
# - Add doc/*
# - Update README-* with new build info
# - Allow separate build/debug building, with VC++ and XCode at least
# - Add configure_file calls for the rest of *.in
# - Get unit tests and bmark.txt testing working
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
#   - Library, header and function checks
#   - Let user specifiy location of same
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
#   - Ubuntu 14.04 LTS
#   - CentOS 7
#   - CentOS 5
#   - CentOS 3?
#   - XCode
#   - OpenIndiana
#
# - For each success above with CMake < 2.8, lower minimum requirement
#
# - Convert non-core top-level targets
#   - tags
#   - dist
#   - rpm/deb/ebuild; CPack?
#   - abicheck
#
# ---------------------

cmake_minimum_required(VERSION 2.8)

include(CheckSymbolExists)

project(libtabula)
set(LIBTABULA_VERSION_MAJOR  3)
set(LIBTABULA_VERSION_MINOR  9)
set(LIBTABULA_VERSION_BUGFIX 9)
set(LIBTABULA_VERSION_TWEAK  1)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/modules)

find_package(MySQL)
if (NOT MYSQL_FOUND)
	message(FATAL_ERROR "MySQL/MariaDB development files not found!")
endif()

CHECK_SYMBOL_EXISTS(getopt unistd.h HAVE_POSIX_GETOPT)
if (NOT HAVE_POSIX_GETOPT)
	CHECK_SYMBOL_EXISTS(getopt libiberty.h HAVE_LIBIBERTY_GETOPT)
endif()

configure_file(src/libtabula.h.in src/libtabula.h)
configure_file(src/config.h.in src/config.h)

include_directories(src ${PROJECT_BINARY_DIR}/src ${MYSQL_INCLUDE_DIR})
link_directories(${MYSQL_LIBRARY})

add_subdirectory(src)
add_subdirectory(examples)
