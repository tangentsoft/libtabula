Building libtabula with Visual C++
====

Prerequisites
----

You need to have the MySQL or MariaDB C API development files on your
system, since libtabula is built on top of it.

The easiest way to get these files is to [download Connector/C][1].

If you need the DB server on your development system anyway, you may
find that they come with the DBMS, either by default or as an option
during the installation.


Project Files
----

libtabula does not ship with *.sln or *.vc[x]proj files.  Instead,
you generate them on your system using CMake[2], which produces
a set of such files customized to your particular system.

The standard way to use CMake on Windows is to:

1.  Install CMake.

2.  Drop to a command prompt.  Both PowerShell and `cmd.exe` work.

    You can also use the Cygwin Bash shell, but you need to be sure
    you're not using the Cygwin version of `cmake`, since that will
    try to build libtabula under Cygwin, even if your system also has
    Visual Studio installed.  (The Cygwin version of CMake is built
    in such a way that it actually *can't* generate Visual Studio
    project and solution files.)

3.  `cd` to the directory where you unpacked the libtabula sources.

4.  **Optional:** Create a build directory underneath that, and `cd`
    into it.

    You can call it "build" if you have no better name.  The name
    doesn't matter.

    If you will be creating multiple build directories, we suggest that
    you call them "build-*", to clearly distinguish their purpose, or
    nest them under a single "build" directory:

        libtabula-4.0.0/
            build-32/
            build-64/

    or:

        libtabula-4.0.0/
            build/
                vs2010/
                vs2012/
                vs2013/

    If you elect not to do this, the build products will be intermixed
    with the libtabula source files, making it harder to disentangle
    them.  It also makes it more difficult to cope with multiple
    versions of Visual Studio on the same system, since the build
    products will conflict with each other.

5.  Run either `cmake` or `cmake-gui` followed by the relative path to
    the source directory.  If you created a single build directory
    underneath the source directory, the command is `cmake ..`

6.  If that succeeded, you can open the generated `libtabula.sln` file.

    If it failed, it should say why clearly enough that you can see how
    to fix it.  If not, post the CMake output to the [mailing list.][3]


Locating C API Development Files
----

The CMake files try to figure out which C API development files you've
got installed, automatically.

If it cannot find your MySQL/MariaDB development files, edit the
`modules\FindMySQL.cmake` file in a text editor, adding the library
and header file directories to the two directory lists you find within.
Then try again.


Choosing a Different C++ Compiler or CPU Target
----

The native Windows version of CMake normally tries to use Visual C++
if it's available, and it generates 32-bit project files by default.

If CMake finds multiple versions of Visual C++ installed, it will
use the newest one.  To force it to use another, give a command like:

     cmake -G "Visual Studio 11 2012"

Give the `--help` flag to get a list of available generators for the
`-G` option.

To generate 64-bit executables instead, add "Win64" to the end of the
generator name:

     cmake -G "Visual Studio 12 2013 Win64"

If you have MinGW installed alongside Visual Studio and want it to
use MinGW instead, see `README-MinGW.md`.

If you have Cygwin installed alongside Visual Studio and want CMake to
build under Cygwin instead of Visual C++, simply install the Cygwin
version of CMake using Cygwin's `setup*.exe` program.  That build of
CMake assumes you're building under Cygwin, *for* Cygwin, by default.
See `README-Cygwin.md`.


Building the Library and Example Programs
----

You must build both the Debug and Release versions of the library,
because a release build of your program won't work with a Debug
version of the libtabula DLL.  These DLLs get different names, so you
can install them in the same directory if needed: `libtabula_d.dll`
for the Debug version and `libtabula.dll` for the Release version.

With the library built, run at least the `resetdb` and `simple1`
examples to ensure that the library is working correctly.  In addition
to the other generic examples, there are a few Visual C++ specific
examples that you might want to look at in `examples\vstudio`.
See [the examples README](README-examples.md) for further details.

Once you're sure the library is working correctly, you can run
the `install.hta` file at the project root to install the library
files and headers in a directory of your choosing.

(Aside: You may not have come across the `.hta` extension before.
It's for a rarely-used feature of Microsoft's Internet Explorer,
called HTML Applications.  Know what Adobe AIR is?  Kinda like
that, only without the compilation into a single binary blob which
you must install before you can run it.  Just open `install.hta`
in a text editor to see how it works.)


Using libtabula in Your Own Projects
----

This is covered in the user manual, chapter 9.


If You Run Into Problems...
----

Especially if you have linking problems, make sure your project
settings match the above.  Visual C++ is very picky about things
like run time library settings.  When in doubt, try running one
of the example programs.  If it works, the problem is likely in
your project settings, not in libtabula.


[1]: http://dev.mysql.com/downloads/mysql/
[2]: http://cmake.org/
[3]: http://libtabula.org/ml/
