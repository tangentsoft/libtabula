# CMakeLists.txt - Tells CMake how to build the libtabula examples on
#      supported platforms.  See ../CMakeLists.txt for system-wide
#      matters.
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

add_library(ltexcomm STATIC printdata.cpp)

add_executable(cgi_jpeg cgi_jpeg.cpp)
target_link_libraries(cgi_jpeg tabula mysqlclient_r)

add_executable(cpool cpool.cpp)
target_link_libraries(cpool tabula mysqlclient_r)

add_executable(dbinfo dbinfo.cpp)
target_link_libraries(dbinfo tabula mysqlclient_r)

add_executable(deadlock deadlock.cpp)
target_link_libraries(deadlock tabula mysqlclient_r)

add_executable(fieldinf fieldinf.cpp)
target_link_libraries(fieldinf tabula mysqlclient_r)

add_executable(for_each for_each.cpp)
target_link_libraries(for_each tabula mysqlclient_r)

add_executable(load_jpeg load_jpeg.cpp)
target_link_libraries(load_jpeg tabula mysqlclient_r)

add_executable(multiquery multiquery.cpp)
target_link_libraries(multiquery tabula mysqlclient_r)

add_executable(resetdb resetdb.cpp)
target_link_libraries(resetdb tabula mysqlclient_r)

add_executable(simple1 simple1.cpp)
target_link_libraries(simple1 tabula mysqlclient_r)

add_executable(simple2 simple2.cpp)
target_link_libraries(simple2 tabula mysqlclient_r)

add_executable(simple3 simple3.cpp)
target_link_libraries(simple3 tabula mysqlclient_r)

add_executable(ssqls1 ssqls1.cpp)
target_link_libraries(ssqls1 tabula mysqlclient_r)

add_executable(ssqls2 ssqls2.cpp)
target_link_libraries(ssqls2 ltexcomm tabula mysqlclient_r)

add_executable(ssqls3 ssqls3.cpp)
target_link_libraries(ssqls3 ltexcomm tabula mysqlclient_r)

add_executable(ssqls4 ssqls4.cpp)
target_link_libraries(ssqls4 ltexcomm tabula mysqlclient_r)

add_executable(ssqls5 ssqls5.cpp)
target_link_libraries(ssqls5 tabula mysqlclient_r)

add_executable(ssqls6 ssqls6.cpp)
target_link_libraries(ssqls6 ltexcomm tabula mysqlclient_r)

add_executable(store_if store_if.cpp)
target_link_libraries(store_if ltexcomm tabula mysqlclient_r)

add_executable(tquery1 tquery1.cpp)
target_link_libraries(tquery1 ltexcomm tabula mysqlclient_r)

add_executable(tquery2 tquery2.cpp)
target_link_libraries(tquery2 ltexcomm tabula mysqlclient_r)

add_executable(tquery3 tquery3.cpp)
target_link_libraries(tquery3 tabula mysqlclient_r)

add_executable(tquery4 tquery4.cpp)
target_link_libraries(tquery4 ltexcomm tabula mysqlclient_r)

add_executable(transaction transaction.cpp)
target_link_libraries(transaction ltexcomm tabula mysqlclient_r)

