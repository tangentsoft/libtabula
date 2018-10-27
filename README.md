A libtabula Primer
====

What It Is
----

libtabula is a C++ wrapper for multiple C database APIs including
SQLite, MariaDB, and MySQL's Connector/C.  It is built around STL
principles to make dealing with the database as easy as dealing with
an STL container.  libtabula relieves the programmer of dealing with
cumbersome C data structures, generation of repetitive SQL statements,
and manual creation of C++ data structures to mirror the database
schema.

libtabula's home page is [`http://libtabula.org/`][0]

libtabula forked from [MySQL++][1] 3.2.1 in May 2014. The name change
reflects the fact that libtabula works with more than just MySQL and
its API-compatible forks.


Prerequisites
----

To build a release version of libtabula, you must have the C API
development files installed for the DBMSes you want libtabula to
work with.

To build a version checked out from the Fossil repository, see
[the HACKERS file](HACKERS.md).


Additional Things to Read
----

Each major platform we support has a dedicated `README-*.md` file for
it containing information specific to that platform.  Please read it:

*   [Cygwin](README-Cygwin.md)
*   [Linux](README-Linux.md)
*   [macOS](README-macOS.md)
*   [MinGW](README-MinGW.md)
*   [Solaris](README-Solaris.md)
*   [Unix (generic)](README-Unix.md)
*   [Visual C++](README-Visual-C%2b%2b.md)

For authorship information, see [the CREDITS file](CREDITS.md).

For license information, see [the COPYING file](COPYING.md).

If you want to change libtabula, see [the HACKERS file](HACKERS.md).

You should have received a user manual and a reference manual with
libtabula.  If not, you can [read a recent version online][2].

Search [the libtabula mailing list archives][3] if you have more
questions.


Building the Library
----

libtabula uses [CMake][4] to generate platform-specific build files
from a platform-independent set of build rules.  We have successfully
used it to build libtabula under these native build system types:

*   **Makefiles**: For Unixy platforms, including Linux, macOS,
    Cygwin, and MinGW, in addition to the "real" Unices.
    
    See [the Unix README](README-Unix.md) for general instructions.

    There are supplementary platform-specific `README` files for most
    popular Unixy platforms:

    -   [Cygwin](README-Cygwin.md)
    -   [Linux](README-Linux.md)
    -   [macOS](README-macOS.md)
    -   [MinGW](README-MinGW.md)
    -   [Solaris](README-Solaris.md)

    TODO: Write README-Free/Net/OpenBSD.md, if only to mention the
    location for C API files.

    If your platform of choice doesn't have a `README` dedicated
    to it, little is probably known about builing libtabula on
    and for it.  Consider writing such a file and posting it to
    the mailing list.

*   **Visual C++**: CMake will generate Visual Studio solution and
    project files for any version of Visual C++ you have installed,
    going back to (and beyond!) the oldest version libtabula
    actually works with, Visual C++ 2003.
    
    See [the Visual C++ README](README-Visual-C%2b%2b.md) for more
    details.

*   **Xcode**: CMake will generate an Xcode project file instead of
    Makefiles on OS X if you ask nicely.

    See [the macOS README](README-macOS.md) for more the proper
    incantation.

CMake can generate many other kinds of IDE project files, but we have
either not tried it, or have not succeeded with it.  Success reports
and patches are welcome on the mailing list.[3]


Example Programs
----

You may want to try out the programs in the examples subdirectory
to ensure that libtabula is working correctly in its own right
as well as working properly with your DBMS.

These examples give many examples of the proper use of libtabula.
See [the examples README](README-examples.md) for further details.


Unsupported Compliers
----

If you're on Windows but want to use some other compiler besides Visual
C++ or GCC, you are currently on your own.  There have been past efforts
to port libtabula to other Windows compilers, but for one reason or
another, all of these ports have died.

On Unixy systems, GCC and Clang work best.  "Native" compilers and
third-party compilers may work, but you're on your own to get it
working.

We have nothing in particular against these unsupported systems.  We
just lack the time and interest to support everything ourselves.  If you
are sufficiently motivated to get libtabula working on one of these
alternate systems, see [the HACKERS file](HACKERS.md) first for
guidance.  If you follow the advice in that file, your patch will be
more likely to be accepted.


If You Want to Hack on libtabula...
----

If you intend to change the library or example code, please read
[the HACKERS file](HACKERS.md).

If you want to change the user manual, read [the doc
README](doc/userman/README.md)

If you want to change the reference manual, see [the Doxygen
manual][5].


[0]: http://libtabula.org/
[1]: http://tangentsoft.com/mysql++/
[2]: http://libtabula.org/doc/
[3]: http://libtabula.org/ml/
[4]: http://cmake.org/
[5]: http://www.stack.nl/~dimitri/doxygen/manual/
