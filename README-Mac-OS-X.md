Building libtabula on Mac OS X
====

There are two major ways to build libtabula on Mac OS X: from the
command line or from within the Xcode IDE.

libtabula has its roots in Unix and Linux. As a result, the
most well-supported way to build libtabula is from the command
line, or Terminal as Apple likes to call it. See [the Unix
README](README-Unix.md) for the generic instructions.  Further
Mac-specific details are given elsewhere in this file.

We haven't heard many complaints about the Xcode build option since
adding it to MySQL++ (libtabula's predecessor) many years ago. We
don't know if this is because it works flawlessly or instead that no
one uses it. All we know is that the primary developer of libtabula
does not use it on his own Mac, so if there are problems, it is up
to you to find them. Even if it is perfectly functional, Xcode is
not as flexible as building from the command line.

If you try the Xcode method and find that it doesn't work, the
easiest way around that roadblock is to build from the command line
instead.  If you're the adventurous sort and want to contribute to
the development of libtabula, see the [HACKERS file](HACKERS.md)
for more info on fixing the source file used as input in the project
file generation process.  We don't want fixed project files, we want
a process that lets us consistently generate correct project files.


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

-   The simplest option is to [download the MySQL server DMG][1]. In
    addition to the C API files you absolultely must have, this gives
    you a nice Mac-like installation procedure and a preference pane
    for bringing the server up and down and setting it to start on
    system boot.

-   If you really only want the C API development files, Oracle offers
    a package called Connector/C.  As of this writing, you get the
    files as a tarball, and you have to copy its contents to some
    suitable location on your hard drive.  If you're using Xcode to
    build libtabula, you'll want to put them under `/usr/local/mysql`.
    libtabula's command line build system is far more tolerant,
    looking there and in many other typical locations.

-   If you use Homebrew, you can install the MySQL C API files with:

        $ brew install mysql-dev

    If you also want a local MySQL server, say this instead:

        $ brew install mysql mysql-dev


Dealing with the 64-Bit Transition in Snow Leopard
----

Xcode for Snow Leopard installs two independent versions of the GNU
Compiler Collection.  The default is GCC 4.2, and it is set up to
build 64-bit executables by default, even if your system is booted
into 32-bit mode.  You also get GCC 4.0, which builds 32-bit
executables by default.  On top of that, you have the confusion
added by Apple's decision to make all 64-bit capable machines boot
into 32-bit mode by default, except for the Xserves.

The first symptom most people run into as a result of this mess is
that the "configure" script fails, yelling something about being
unable to link to libmysqlclient, the libtabula C API client library.
It's because the library was probably built as a 32-bit executable
and you're using the default compiler which tries to build a 64-bit
test executable against this library and fails.

There are many ways out of this tarpit.  Here are the ones I prefer:

First, you can force GCC 4.2 to build 32-bit binaries:

    ./configure CFLAGS=-m32 CXXFLAGS=-m32 LDFLAGS=-m32 --other-flags-here

Second, you can make the libtabula build system use GCC 4.0 instead:

    ./configure CC=gcc-4.0 CXX=g++-4.0 --other-flags-here

Last, you could just take Apple's implied advice and start
booting your Mac into 64-bit mode, if it will support it.
([Details](http://macperformanceguide.com/SnowLeopard-64bit.html))

I'm aware of other solutions to the problem, but I expect one among
these will work for you.


Making Universal Binaries
----

By default, the command line build system will generate libraries
that only work with the platform you build libtabula on.  It can be
convinced to build "universal" binaries instead by configuring
the library like so:

    $ ./configure --disable-dependency-tracking CXXFLAGS='-arch ppc -arch i386'

This builds the library for the two 32-bit OS X architectures, and
is what most people have traditionally thought of as "universal".
However, you may also want a 64-bit build, meaning there are four
different architectures, and thus four -arch flags needed:

    $ ./configure --disable-dependency-tracking CXXFLAGS='-arch ppc -arch ppc64 -arch i386 -arch x86_64'

These are single commands, with the line broken to keep the line
lengths in this document reasonable.

The first command doubles build time relative to the default
configuration, and the second quadruples it.  It also makes the
resulting binaries larger, which increases the amount of time it
takes to start a program.  Build libtabula like this only if you must.

The `--disable-dependency-tracking` flag is necessary because, when
building universal binaries, it has to rebuild each source module
multiple times, which confuses the logic that tries to tell when a
given module needs rebuiding based on its dependencies on other files.


[1]: http://dev.mysql.com/downloads/mysql/
