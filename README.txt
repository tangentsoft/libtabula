What It Is
~~~~~~~~~~
    libtabula is a C++ wrapper for multiple C database APIs including
    SQLite, MariaDB, and MySQL's Connector/C.  It is built around STL
    principles, to make dealing with the database as easy as dealing
    with an STL container.  libtabula relieves the programmer of
    dealing with cumbersome C data structures, generation of repetitive
    SQL statements, and manual creation of C++ data structures to
    mirror the database schema.

    libtabula's home page is http://libtabula.org/

    libtabula forked from the MySQL-only C++ wrapper MySQL++ 3.2.1
    in May 2014.


Prerequisites
~~~~~~~~~~~~~
    To build a release version of libtabula, you must have the C API
    development files installed for the DBMSes you want libtabula to
    work with.

    To build a version checked out from the Fossil repository, see
    the HACKERS.txt file.


Additional Things to Read
~~~~~~~~~~~~~~~~~~~~~~~~~
    Each major platform we support has a dedicated README-*.txt
    file for it containing information specific to that platform.
    Please read it.

    For authorship information, see the CREDITS.txt file.

    For license information, see the COPYING.txt file.

    If you want to change libtabula, see the HACKERS.txt file.

    You should have received a user manual and a reference manual
    with libtabula.  If not, you can read a recent version online:

        http://libtabula.org/doc/

    Search the libtabula mailing list archives if you have more
    questions:

        http://libtabula.org/ml/


Building the Library
~~~~~~~~~~~~~~~~~~~~
    libtabula uses Bakefile (http://bakefile.org/) to generate
    platform-specific project files and makefiles from a single set
    of input files.  We currently support these build systems:

    autoconf:
        For Unixy platforms, including Linux, Mac OS X, and Cygwin, in
        addition to the "real" Unices.  See README-Unix.txt for general
        instructions.  Supplementary platform-specific details are
        in README-Cygwin.txt, README-Linux.txt, README-Mac-OS-X.txt,
        and README-Solaris.txt.

    MinGW:
        We ship Makefile.mingw for MinGW.  It currently only builds the
        static version of the library for technical reasons.  This has
        licensing ramifications.  See README-MinGW.txt for details.

    Visual C++:
        We ship Visual Studio 2003, 2005, and 2008 project files.
        No older version of Visual C++ will build libtabula, due to
        compiler limitations.  See README-Visual-Studio.txt for more
        details.

    Xcode:
        We ship an Xcode v2 project file.  It hasn't been tested
        much yet, since the autoconf method works just fine on OS X.
        As a result, we need both success and failure reports on the
        mailing list.  See README-Mac-OS-X.txt for more information.


Example Programs
~~~~~~~~~~~~~~~~
    You may want to try out the programs in the examples subdirectory
    to ensure that libtabula is working correctly in its own right
    as well as working properly with your DBMS.

    These examples give many examples of the proper use of libtabula.
    See README-examples.txt for further details.


Unsupported Compliers
~~~~~~~~~~~~~~~~~~~~~
    If you're on Windows but want to use some other compiler besides
    Visual C++ or GCC, you are currently on your own.  There have
    been past efforts to port libtabula to other Windows compilers,
    but for one reason or another, all of these ports have died.

    On Unixy systems, GCC and Clang work best.  "Native" compilers
    and third-party compilers may work, but you're on your own to
    get it working.

    We have nothing in particular against these unsupported systems.
    We just lack the time and resources to support everything
    ourselves.  If you are sufficiently motivated to get libtabula
    working on one of these alternate systems, see the HACKERS.txt
    file first for guidance.  If you follow the advice in that file,
    your patch will be more likely to be accepted.


If You Want to Hack on libtabula...
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    If you intend to change the library or example code, please read
    the HACKERS.txt file.

    If you want to change the user manual, read doc/userman/README.txt

    If you want to change the reference manual, see the Doxygen manual:
    http://www.stack.nl/~dimitri/doxygen/manual.html
