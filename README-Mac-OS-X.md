Building libtabula on Mac OS X
====

There are two major ways to build libtabula on Mac OS X: from the
command line or from within the Xcode IDE.

libtabula has its roots in Unix and Linux. As a result, the most
well-supported way to build libtabula is from the command line, or
Terminal as Apple likes to call it. See [the Unix README][1] for the
generic instructions.  Further Mac-specific details are given elsewhere
in this file.

If you try the Xcode method and find that it doesn't work, the easiest
way around that roadblock is to build from the command line instead.


Prerequisite: Install Your DBMS's C API Development Files
----

libtabula builds on top of your database's C development API, so it
needs at least that library and its header files installed.  You don't
need the server itself installed on your development machine, though
it's often helpful for testing.

libtabula supports several different DBMSes, but for simplicity, I will
cover just MySQL here. The basic procedure is similar for other DBMSes.

There are many ways to get the files you need to build libtabula against
MySQL on Mac OS X:

-   The simplest option is to [download the MySQL server DMG][2]. In
    addition to the C API files you absolultely must have, this gives
    you a nice Mac-like installation procedure and a preference pane
    for bringing the server up and down and setting it to start on
    system boot.

-   If you really only want the C API development files, Oracle offers a
    package called Connector/C.  As of this writing, you get the files
    as a tarball, and you have to copy its contents to a suitable
    location on your hard drive.  The libtabula build system searches
    many common locations for these files, including `/usr/local/mysql`,
    which is my recommendation for this case.

    If you must put them somewhere else, either consult the list in
    `modules/FindMySQL.cmake` or modify the directory lists you find
    there to include the path you chose.

-   If you use Homebrew, you can install the MySQL C API files with:

        $ brew install mysql-dev

    If you also want a local MySQL server, say this instead:

        $ brew install mysql mysql-dev


Dealing with the 64-Bit Transition
----

Since Snow Leopard (OS X 10.6), the C and C++ compilers installed with
Xcode build 64-bit executables by default.  If you still need to build
32-bit executables, there are a couple of different ways.  The one I
prefer is:

	$ mkdir build32
	$ cd build32
    $ CFLAGS=-m32 CXXFLAGS=-m32 LDFLAGS=-m32 cmake ..

That lets CMake use whatever compiler it prefers, but adds the `-m32`
flag to the compile and link commands, which forces it to build 32-bit
object code.

Xcode also installs a 32-bit-only compiler for legacy compatibilty.  On
the system I'm using now, the way to force CMake to use that one is:

    $ mkdir build32
	$ cd build32
	$ CC=gcc-4.2 CXX=g++-4.2 cmake ..

To build "Universal" binaries for both 32- and 64-bit Intel, this should
work:

    $ mkdir build-universal
	$ cd build-universal
    $ CXXFLAGS='-arch i686 -arch x86_64' cmake ..


Using the Xcode IDE Instead of Building at the Command Line
----

To create Xcode project files instead of Unix Makefiles:

    $ mkdir build-xcode
	$ cd build-xcode
	$ cmake -G Xcode ..
	$ open libtabula.xcodeproj

Then build as you normally would.  ⌘-Shift-R, etc.


[1]: README-Unix.md
[2]: http://dev.mysql.com/downloads/mysql/
