@echo off
if not exist vc2003 mkdir vc2003
if not exist vc2005 mkdir vc2005
if not exist vc2008 mkdir vc2008

bakefile_gen %*
if errorlevel 1 exit
if not exist vc2003\libtabula.sln goto no_bakefile
if not exist vc2005\libtabula.sln goto no_bakefile
if not exist vc2008\libtabula.sln goto no_bakefile

cd lib
perl querydef.pl
if errorlevel 1 exit
if not exist querydef.h goto no_perl
perl ssqls.pl
if errorlevel 1 exit
if not exist ssqls.h goto no_perl

if not exist libtabula.h goto no_libtabula_h
cd ..

exit

:no_bakefile
echo.
echo Bakefile doesn't seem to be installed on this system.  Download it
echo from http://bakefile.org/  You need version 0.2.3 or newer.
echo.
exit

:no_perl
echo.
echo You need a Perl interpreter installed on your system, somewhere in
echo the PATH.  Any recent version or flavor should work; we don't use
echo any special extensions.  The easiest to install on Windows would be
echo ActivePerl, from http://activestate.com/Products/activeperl/
echo If you're familiar with Unix, you might like Cygwin better instead:
echo http://cygwin.com/setup.exe
echo.
cd ..
exit

:no_libtabula_h
echo.
echo WARNING: Can't make lib/libtabula.h
echo.
echo On Unixy systems, autoconf creates lib/libtabula.h from lib/libtabula.h.in
echo but there is no easy way to do this on Windows.  You can do it manually:
echo just copy the file to the new name, and edit the LIBTABULA_HEADER_VERSION
echo definition to put the proper version number parts into the macro.  It
echo needs to look something like this:
echo.
echo #define LIBTABULA_HEADER_VERSION MYSQLPP_VERSION(3, 0, 0)
echo.
echo It's important that the three numbers match the actual library version
echo number, or else programs that check this (like resetdb) will fail.
echo.
echo Alternately, if you've also got Libtabula installed on some Unixy type
echo system, you can let its bootstrap procedure create libtabula.h and then
echo copy it to the Windows machine.
echo.
cd ..
