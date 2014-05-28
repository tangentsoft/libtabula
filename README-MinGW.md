Building libtabula on MinGW
====

Prerequisite: GCC Version
----

If your MinGW version isn't using at least GCC 3.4.5, it needs
to be updated.  Older versions are known to not work with libtabula.

As of libtabula 3.1.1, the required version might need to be even
newer, as we are now depending on improvements to the MinGW linker
which probably don't go back that far.


Prerequisite: MySQL C Development Files
----

libtabula is built atop MySQL's C API library.  The easiest way to
get that is to [install Connector/C][1] on your development system.
The libtabula distribution assumes these files are in:

    C:\Program Files\libtabula\libtabula Connector C 6.1\

There are a number of reasons why that path may not work for you:

*   You have a newer version of Connector/C installed

*   You're on a 64-bit system, but have the 32-bit versions of
    Connector/C and MinGW installed and wish to build a 32-bit binary.
    In that case, the path will look like this instead:

        C:\Program Files (x86)\libtabula\libtabula Connector C 6.1\

*   You may have the MySQL Server on your system and installed the
    development files along with it, and therefore don't want to
    install Connector/C separately.  In that case, the path will look
    like this instead:

        C:\Program Files\libtabula\libtabula Server 5.6\

Regardless of the reason you have for changing this path, there are
two ways that work:

*   The easy way is to do a global search and replace on the path
    in `Makefile.mingw`.  This is a generated file, but if that's the
    only change to libtabula you need, it works fine.

*   If you're doing deeper work on libtabula, you should change the
    `MYSQL_WIN_DIR` variable at the top of `libtabula.bkl` instead.

    Having done that, you can generate `Makefile.mingw` from that
    file using the Windows port of [Bakefile](http://bakefile.org/):

        bakefile_gen -f mingw


Building the Library and Example Programs
----

With the prerequisites above taken care of, you can build libtabula
with this command:

    mingw32-make -f Makefile.mingw

Notice that we're using the MinGW-specific version of GNU `make`, not
the Cygwin or MSYS versions.  Many things will break otherwise: path
separator handling, shell commands used by the `Makefile`, etc.

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
so that its tools are found first.  If you use Cygwin's `bash`
as a command shell in preference to the DOS-like `cmd.exe`, you
can use this shell script to temporarily set the environment to
"MinGW mode" and make it easy to get back to "Cygwin mode":

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

You might wish to build libtabula with MinGW because you're
not actually running Windows, but need Windows executables.
The thought being that this lets you use GCC, the same compiler
you're probably using to make native executables.  There are
indeed ways to make this work.

The most "native" way to do this is to run MinGW under Wine.
Leonti Bielski provided these instructions:

1. Install MinGW through Wine:

        $ wine MinGW-5.1.6.exe

2.  Add the MinGW directory to Wine's PATH with [Wine regedit][2].

3.  Install libtabula under Wine, or at least unpack the Windows
    ZIP file version of libtabula in a place where Wine can find it.
    You don't need to run a Windows libtabula server under Wine.
    We're only doing this to get the libtabula C API library and
    its headers, which libtabula builds against.  The resulting
    MinGW build of libtabula can talk to a native libtabula server
    out in Wine's host environment or on some other machine.

4.  Modify Makefile.mingw to match the install location for
    the libtabula C API files.

5.  Build libtabula with:

        $ wine mingw32-make -f Makefile.mingw

Another way is to build a Windows virtual machine, such as with
VMware or VirtualBox.  In that case, you'd use the regular build
instructions at the top of this document.

You might think to avoid the need for Wine or Windows by use of a
MinGW cross-compiler:

    $ ./configure --target=mingw32
    $ make

Unfortunately, that currently doesn't work.

The reason is that our Autoconf build system assumes a typical POSIX
type target, which MinGW is not.  We made this assumption because
we have a perfectly good MinGW build option, `Makefile.mingw`.  But,
that also won't work on a POSIX system because that `Makefile` assumes
external commands run under `cmd.exe`, not some Unixy shell.  Thus the
advice to build with `Makefile.mingw` under Windows or something
sufficiently close to it.

If you really wanted to, you could extend the Autoconf build system
to make it realize when it's being used to cross-compile for MinGW.
Patches thoughtfully considered; see [the HACKERS file](HACKERS.md).


[1]: http://dev.mysql.com/downloads/mysql/
[2]: http://winehq.org/site/docs/wineusr-guide/environment-variables
