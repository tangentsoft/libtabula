Building libtabula on Cygwin
====

Prerequisite: Install Database Engine Development Files
----

Before you can build libtabula, you need to install the development
files for the database engines you want it to wrap.  As of this
writing, libtabula on Cygwin supports MySQL and SQLite via the
`libmysqlclient-devel` and `libsqlite3-devel` packages which you can
install with Cygwin's `setup*.exe`.


Building the Library and Example Programs
----

Having done that, Cygwin behaves like any other Unixy system.
See the instructions in [the Unix README](README-Unix.md).
