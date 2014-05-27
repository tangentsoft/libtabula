#-######################################################################
# libtabula.m4 - Example autoconf macro showing how to find libtabula
#	library and header files.
#
# Copyright © 2004-2009 by Educational Technology Resources, Inc.
#
# This file is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as published
# by the Free Software Foundation; either version 2.1 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with libtabula; if not, write to the Free Software
# Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
# USA
#-######################################################################

dnl @synopsis LIBTABULA_DEVEL
dnl 
dnl This macro tries to find the libtabula library and header files.
dnl
dnl We define the following configure script flags:
dnl
dnl		--with-libtabula: Give prefix for both library and headers, and try
dnl			to guess subdirectory names for each.  (e.g. tack /lib and
dnl			/include onto given dir name, and other common schemes.)
dnl		--with-libtabula-lib: Similar to --with-libtabula, but for library only.
dnl		--with-libtabula-include: Similar to --with-libtabula, but for headers
dnl			only.
dnl
dnl This macro depends on having the default compiler and linker flags
dnl set up for building programs against the MySQL C API.  The mysql.m4
dnl macro in this directory fits this bill; run it first.
dnl
dnl @version 1.3, 2009/11/22
dnl @author Warren Young <libtabula@etr-usa.com>

AC_DEFUN([LIBTABULA_DEVEL],
[
	dnl
	dnl Set up configure script macros
	dnl
	AC_ARG_WITH(libtabula,
		[  --with-libtabula=<path>     path containing libtabula header and library subdirs],
		[LIBTABULA_lib_check="$with_libtabula/lib64 $with_libtabula/lib $with_libtabula/lib64/libtabula $with_libtabula/lib/libtabula"
		  LIBTABULA_inc_check="$with_libtabula/include $with_libtabula/include/libtabula"],
		[LIBTABULA_lib_check="/usr/local/libtabula/lib64 /usr/local/libtabula/lib /usr/local/lib64/libtabula /usr/local/lib/libtabula /opt/libtabula/lib64 /opt/libtabula/lib /usr/lib64/libtabula /usr/lib/libtabula /usr/local/lib64 /usr/local/lib /usr/lib64 /usr/lib"
		  LIBTABULA_inc_check="/usr/local/libtabula/include /usr/local/include/libtabula /opt/libtabula/include /usr/local/include/libtabula /usr/local/include /usr/include/libtabula /usr/include"])
	AC_ARG_WITH(libtabula-lib,
		[  --with-libtabula-lib=<path> directory path of libtabula library],
		[LIBTABULA_lib_check="$with_libtabula_lib $with_libtabula_lib/lib64 $with_libtabula_lib/lib $with_libtabula_lib/lib64/mysql $with_libtabula_lib/lib/mysql"])
	AC_ARG_WITH(libtabula-include,
		[  --with-libtabula-include=<path> directory path of libtabula headers],
		[LIBTABULA_inc_check="$with_libtabula_include $with_libtabula_include/include $with_libtabula_include/include/mysql"])

	dnl
	dnl Look for libtabula library
	dnl
	AC_CACHE_CHECK([for libtabula library location], [ac_cv_libtabula_lib],
	[
		for dir in $LIBTABULA_lib_check
		do
			if test -d "$dir" && \
				( test -f "$dir/liblibtabula.so" ||
				  test -f "$dir/liblibtabula.a" )
			then
				ac_cv_libtabula_lib=$dir
				break
			fi
		done

		if test -z "$ac_cv_libtabula_lib"
		then
			AC_MSG_ERROR([Didn't find the libtabula library dir in '$LIBTABULA_lib_check'])
		fi

		case "$ac_cv_libtabula_lib" in
			/* ) ;;
			* )  AC_MSG_ERROR([The libtabula library directory ($ac_cv_libtabula_lib) must be an absolute path.]) ;;
		esac
	])
	AC_SUBST([LIBTABULA_LIB_DIR],[$ac_cv_libtabula_lib])

	dnl
	dnl Look for libtabula header file directory
	dnl
	AC_CACHE_CHECK([for libtabula include path], [ac_cv_libtabula_inc],
	[
		for dir in $LIBTABULA_inc_check
		do
			if test -d "$dir" && test -f "$dir/libtabula.h"
			then
				ac_cv_libtabula_inc=$dir
				break
			fi
		done

		if test -z "$ac_cv_libtabula_inc"
		then
			AC_MSG_ERROR([Didn't find the libtabula header dir in '$LIBTABULA_inc_check'])
		fi

		case "$ac_cv_libtabula_inc" in
			/* ) ;;
			* )  AC_MSG_ERROR([The libtabula header directory ($ac_cv_libtabula_inc) must be an absolute path.]) ;;
		esac
	])
	AC_SUBST([LIBTABULA_INC_DIR],[$ac_cv_libtabula_inc])

	dnl
	dnl Now check that the above checks resulted in -I and -L flags that
	dnl let us build actual programs against libtabula.
	dnl
	case "$ac_cv_libtabula_lib" in
	  /usr/lib) ;;
	  *) LDFLAGS="$LDFLAGS -L${ac_cv_libtabula_lib}" ;;
	esac
	CPPFLAGS="$CPPFLAGS -I${ac_cv_libtabula_inc} -I${MYSQL_C_INC_DIR}"
	AC_MSG_CHECKING([that we can build libtabula programs])
	AC_COMPILE_IFELSE(
		[AC_LANG_PROGRAM([#include <libtabula.h>],
			[libtabula::Connection c(false)])],
		AC_MSG_RESULT([yes]),
		AC_MSG_ERROR([no]))
]) dnl End LIBTABULA_DEVEL

