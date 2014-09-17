Building libtabula on Linux
====
Linux is basically Unix, so [the Unix README](README-Unix.md) covers
the generic bits.  I'll just cover a few of the issues specific to
Linux here.


Prerequisite: Install the libtabula Development Files
----

libtabula builds on top of the C API for your DBMS(es) of choice, so it
needs the C API development files to build against.

Most Linux distributions package the DBMS and the C API files
separately. This is because it is quite common to run the DBMS server
on a different machine than is used to develop clients for that server.
A single machine often doesn't need to have both packages installed.

There are about as many different ways to get DBMS development files
on your system as there are major Linux distributions, multiplied by
the number of DBMSes libtabula supports at the time you try it.  More
actually, because there are also often direct first-party packages in
addition to your distro's versions, plus possibly third-party packages.
This document cannot possibly list all your options, much less tell you
which option is best for you.

On Red Hat type systems with `yum`, this may work:

    # yum install mysql-devel

On Debian/Ubuntu type systems, somethhing more like this will do:

    $ sudo apt-get install libmysqlclient15-dev

The version number is the ABI version of the MySQL C API library.
ABI version 15 corresponds to MySQL version 5.0, the recommended
stable version as of this writing.

    
Dealing with the Dynamic Linker
----

`ld.so` is the dynamic linker on Linux. It is a system-level
program which is used to run any program that uses shared libraries
(`lib*.so`).  Its job is to find the libraries and link them to the
base executable so it will run.

Because `ld.so` only looks in a few places for libraries on most
systems, a common problem is a program that builds without error
but won't run, complaining about `libtabula.SOMETHING`.

There are a number of ways to deal with this.

First, you could just configure libtabula to install under `/usr`
instead of `/usr/local`, like system-provided packages:

    $ ./configure --prefix=/usr

This isn't recommended practice when building packages from source,
but it does work.

Second, you can add the libtabula library directory to the
`LD_LIBRARY_PATH` environment variable.  This works like the shell's
`PATH` variable: a colon-separated list of directories to search.
This is best when the installation directory is something totally
uncommon, or you don't have root permissions on the box so you
can't do the next option.

Finally, the most robust way to tell `ld.so` about a nonstandard
library directory is to put it in `/etc/ld.so.conf` or in one of
the files included from there.  Then, run `ldconfig` as `root` to
rebuild the cache file `ld.so` uses to find libraries in these
nonstandard directories.  Running `ldconfig` isn't necessary for
the previous two methods.
