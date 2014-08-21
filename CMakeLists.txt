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
# - Update README-* with new build info
# - Replace old bootstrap script options:
#   - nodoc
#   - noex
#   - nolib
#   - noopt
#   - pedantic
#
# ---------------------

cmake_minimum_required(VERSION 2.8)

project(libtabula)
set(LIBTABULA_VERSION_MAJOR  3)
set(LIBTABULA_VERSION_MINOR  9)
set(LIBTABULA_VERSION_BUGFIX 9)
set(LIBTABULA_VERSION_TWEAK  1)

configure_file(src/libtabula.h.in src/libtabula.h)

include_directories(src "${PROJECT_BINARY_DIR}/src" /usr/local/include/mysql)

add_subdirectory(src)
add_subdirectory(examples)