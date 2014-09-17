Building libtabula on MinGW
====

Prerequisite: GCC Version
----

MinGW has a long history of weird bugs that prevent libtabula from
working correctly.  Always use a recent version of MinGW's GCC package;
by preference, the very latest stable version.  You can get away with
an ancient copy of GCC 3.0.1 on Linux, but not on MinGW.


Prerequisite: C API Development Files for Your DBMS of Choice
----

libtabula is built atop the C API libraries of multiple DBMSes.
It can be built against all of them at once, just one, or some mix.

The easiest and best-supported option currently is the [Connector/C][1]
package for MySQL.  libtabula's build system knows how to find these
files in the normal case.  If you have a newer version than libtabula
knows about, or have installed them in a nonstandard location, see
the CMake information below.


Building the Library and Example Programs
----

libtabula uses the CMake build system.  If your system only has the
MinGW compiler installed, and nothing else, CMake should find it and
use it without help.  The standard command sequence is:

    c:\> cd c:\wherever\you\unpacked\libtabula
    c:\...\libtabula> mkdir build
    c:\...\libtabula> cd build
    c:\...\libtabula> cmake ..
    c:\...\libtabula> make

This assumes you told CMake to put itself into the Windows PATH,
and that you don't need to specifiy `mingw32-make` on your system to
get the right version of GNU make.

The CMake build system tries to find your database's C API development
files automatically.  If it fails, edit the `modules/FindMySQL.cmake`
file, adding the proper include and library directories to the top
of the Windows directory lists you find within.

You don't have to create the `build` subdirectory if you don't want to.
If you say `cmake .` from within the libtabula source tree, CMake and
the subsequent `make` command will create their outputs intermixed
with the source files, creating a tangle for you to deal with.
Using a separate build directory is simply cleaner.  The name of the
build directory doesn't matter, and it doesn't have to be underneath
the libtabula source root.

If you also have Cygwin or Visual Studio installed, things get a
bit trickier.

The native Windows version of CMake uses Visual Studio by preference,
if it finds it.  To force it to use MinGW, change the `cmake ..`
command above to:

    c:\...\libtabula> cmake -G 'MinGW Makefiles` ..

or to:

    c:\...\libtabula> cmake -G 'MSYS Makefiles` ..

The correct invocation depends on whether you're using the native
Win32 version of GNU make (a.k.a. `mingw32-make`) from the MinGW
project or the MSYS port of GNU make.

(The difference is in which command shell each uses.  The native
Windows port of GNU make interprets target commands with `cmd.exe`,
while the MSYS port uses the GNU Bash shell, which has a very different
syntax.  Also, generated commands like "copy" and "ren" must change,
in this case to "cp" and "mv".)

If you've got both Cygwin and MinGW installed, or you are using
Cygwin with the MinGW cross-compilers, you have similar traps to
watch out for:

-   If you have the Cygwin version of `cmake.exe` installed, it will
    be in the `PATH` ahead of the native Windows version of
    `cmake.exe`, so it will generate `Makefiles` that assume you're
    building under Cygwin, *for* Cygwin.  The Cygwin version of CMake
    does not include the MinGW generators, so you will need to give
    the full path to the native CMake executable:

        $ "/cygdrive/c/Program Files (x86)/CMake/bin/cmake" ..

    You might need to give a `-G` flag to force it to generate MinGW
    `Makefiles`, as above.

-   As with the MinGW vs MSYS `Makefile` issue brought up above, the
    Cygwin version of `make.exe` will misinterpret the MinGW
    `Makefiles`.  Be sure you're using the right tools!

Speaking of Cygwin and MSYS, if you have either these or any other
Unix emulation environment installed, be sure their executables
aren't in the `PATH` when building libtabula.  MinGW's version of GNU
`make` does some funny things if it thinks it's running in the
presence of Unixy tools which will break the libtabula build.

Once the library is built, you should run the examples.  At minimum,
run `resetdb` and `simple1`.

Once you're satisfied that the library is working correctly, you
can run `install.hta` to automatically install the library files and
headers in subdirectories under `c:\libtabula`.


Cygwin and MinGW Coexistence
----

It's possible to have both Cygwin and MinGW installed and build
with the MinGW tools without interference from the Cygwin bits.
The main thing you have to take care of is that MinGW's `bin`
directory must precede the Cygwin `bin` directory in the `PATH`,
so that its tools are found first.  If you use Cygwin's Bash as a
command shell in preference to the DOS-like `cmd.exe`, you can use
this shell script to temporarily set the environment to "MinGW mode"
and make it easy to get back to "Cygwin mode":

    #!/bin/sh
    PATH=/cygdrive/c/mingw/bin:/cygdrive/c/windows:/cygdrive/c/windows/system32:/cygdrive/c/cygwin/bin
    echo "Say 'exit' to leave MinGW shell and restore Cygwin environment."
    /usr/bin/bash --rcfile ~/.mingwrc

I recommend having at least this in the `~/.mingwrc` file:

    alias make=mingw32-make
    PS1='MinGW: \W \$ '

The prompt change reminds you that you are in a sub-shell set up
for MinGW.  The alias for `make` ensures you don't accidentally run
Cygwin's `make`, which won't work with `Makefile.mingw`.  We could
just leave `/cygdrive/c/cygwin/bin` out of the environment, but
there are Cygwin tools we want access to, like `vim`.  As long as all
the MinGW ones override those Cygwin also provides, we don't need to
worry about having both in the `PATH`.  Besides, having the alias is
nice for those who have `make` committed to muscle memory.


Building on Linux
----

You might wish to build libtabula with MinGW because you're not
actually running Windows, but need Windows executables.  The thought
being that this lets you use GCC, the same compiler you're probably
using to make native executables.  There are indeed ways to make
this work.

The most "native" way to do this is to use the MinGW cross-compilers,
which may already be included in the package repository for your
version of Linux.  CMake doesn't fall out of the box knowing how to
do this, but [it can be taught][2], if you want to go that route.

The second most Linux-like way to do this is to run MinGW under Wine.
Leonti Bielski provided these instructions:

1. Install MinGW through Wine:

        $ wine MinGW-5.1.6.exe

2.  Add the MinGW directory to Wine's `PATH` with [Wine regedit][3].

3.  Install the Windows version of the C API development files for
    your DBMS in a directory Wine can see.  (You do not need to
    install the DBMS itself under Wine.)

    Ideally, install them in the same directory that you would use
    in a native Windows system, so that `modules/FindMySQL.cmake`
    will find them automatically.  If you can't do that, edit that
    file and add the include and library directories to the directory
    lists you find within.

4.  Build libtabula with:

        $ wine mingw32-make

A third way is to build a Windows virtual machine, such as with VMware
or VirtualBox.  In that case, you'd use the regular build instructions
at the top of this document.


[1]: http://dev.mysql.com/downloads/mysql/
[2]: http://www.cmake.org/Wiki/CmakeMingw
[3]: http://winehq.org/site/docs/wineusr-guide/environment-variables
