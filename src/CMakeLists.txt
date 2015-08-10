# src/CMakeLists.txt - Tells CMake how to build libtabula proper
#    See ../CMakeLists.txt for system-wide matters.
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
    tags

    querydef.h
    ssqls.h

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
    result.cpp
    row.cpp
    scopedconnection.cpp
    sql_buffer.cpp
    sqlstream.cpp
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

# Add a top-level "tags" target which includes all files in both
# the build and source versions of src/*.
set_source_files_properties(tags PROPERTIES GENERATED true)
add_custom_command(OUTPUT tags
    COMMAND ctags -R --c++-kinds=+p --fields=+iaS --extra=+q 
       ${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND ln -sf ${CMAKE_CURRENT_BINARY_DIR}/tags ${CMAKE_BINARY_DIR}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

# ...but only make it a dependency of libtabula if the ctags program
# is available, else it will fail to build on Windows.
find_program(CTAGS_PATH ctags)
if(CTAGS_PATH)
    message(STATUS "Found ctags: ${CTAGS_PATH}")
    add_dependencies(tabula tags)
endif(CTAGS_PATH)

target_link_libraries(tabula ${MYSQL_C_API_LIBRARY})
