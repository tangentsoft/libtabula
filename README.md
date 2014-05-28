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
its API compatible forks.


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

* [Cygwin](README-Cygwin.md)
* [Linux](README-Linux.md)
* [Mac OS X](README-Mac-OS-X.md)
* [MinGW](README-MinGW.md)
* [Solaris](README-Solaris.md)
* [Unix (generic)](README-Unix.md)
* [Visual C++](README-Visual-C%2b%2b.md)

For authorship information, see [the CREDITS file](CREDITS.md).

For license information, see [the COPYING file](COPYING.md).

If you want to change libtabula, see [the HACKERS file](HACKERS.md).

You should have received a user manual and a reference manual with
libtabula.  If not, you can [read a recent version online][2].

Search [the libtabula mailing list archives][3] if you have more
questions.


Building the Library
----

libtabula uses [Bakefile][4] to generate platform-specific project
files and makefiles from a single set of input files.  We currently
support these build systems:

*   **Autoconf**: For Unixy platforms, including Linux, Mac OS X,
    and Cygwin, in addition to the "real" Unices.  See [the Unix
    README](README-Unix.md) for general instructions.  Supplementary
    platform-specific details are in the [Cygwin](README-Cygwin.md),
    [Linux](README-Linux.md), [Mac OS X](README-Mac-OS-X.md), and
    [Solaris](README-Solaris.md) `README` files.

*   **MinGW**: We ship `Makefile.mingw` for MinGW.  It currently only
    builds the static version of the library for technical
    reasons.  This has licensing ramifications.  See [the MinGW
    README](README-MinGW.md) for details.

*   **Visual C++**: We ship Visual Studio 2003, 2005, and 2008
    project files.  No older version of Visual C++ will build
    libtabula, due to compiler limitations.  See [the Visual C++
    README](README-Visual-C%2b%2b.md) for more details.

*   **Xcode**: We ship an Xcode v2 project file.  It hasn't been
    well-tested since the Autoconf method works just fine on OS X.
    As a result, we need both success and failure reports on the
    [mailing list][3].  See [the Mac OS X README](README-Mac-OS-X.md)
    for more information.


Example Programs
----

You may want to try out the programs in the examples subdirectory
to ensure that libtabula is working correctly in its own right
as well as working properly with your DBMS.

These examples give many examples of the proper use of libtabula.
See [the examples README](README-examples.md) for further details.


Unsupported Compliers
----

If you're on Windows but want to use some other compiler besides
Visual C++ or GCC, you are currently on your own.  There have been
past efforts to port libtabula to other Windows compilers, but for
one reason or another, all of these ports have died.

On Unixy systems, GCC and Clang work best.  "Native" compilers
and third-party compilers may work, but you're on your own to get
it working.

We have nothing in particular against these unsupported systems.
We just lack the time and resources to support everything ourselves.
If you are sufficiently motivated to get libtabula working on one
of these alternate systems, see [the HACKERS file](HACKERS.md) first
for guidance.  If you follow the advice in that file, your patch will
be more likely to be accepted.


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
[4]: http://bakefile.org/
[5]: http://www.stack.nl/~dimitri/doxygen/manual/
