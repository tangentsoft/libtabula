Building libtabula on Solaris
====


For the most part, Solaris is just another Unix variant as far as
libtabula is concerned.  See [the Unix README](README-Unix.md) for
most of what you need to know to build and use libtabula.


Prerequisite: Install the Database Engine Development Files
----

libtabula wraps the C API for multiple database engines.
The development files for the DBMSes you want to use need to be
installed before you try to build libtabula.

With the current version of Solaris at the time of this writing,
a MySQL package is included on the operating system disk, but
not installed by default.  To install it, pull down the GNOME
System menu, go to Administration, and then to Package Manager.
Search for "mysql5" and install those packages.  While there,
you may also need to install the gcc packages, if you haven't
done that already.  You may need to install Perl as well.

Don't search for just "mysql" in Package Manager, as that will
also bring up legacy MySQL 4.0 packages.  libtabula may build
against 4.0 still; it's been a while since we've tested it.
What is certain is that the examples won't run against 4.0 without
modification, as they assume the availability of UTF-8 character
set support, which was added in 4.1.

It's no doubt possible to use the official binaries from mysql.com
instead, or to build from source.  We don't do that ourselves,
though, and don't have reports from those who have, so we can't
advise on how to do it.


C API Development File Directories
----

Sun's MySQL package installs the development files in relatively
uncommon locations.  The libraries are in `/usr/mysql/lib/mysql`, and
the headers are in `/usr/mysql/include/mysql`.  Way to be redundant,
guys. :)
