Building libtabula on Unix
====

Platform Variations
----
This file only covers details common to all Unix variants
supported by libtabula  For platform-specific details, see the
file appropriate to your OS:

*   [Cygwin](README-Cygwin.md)
*   [Linux](README-Linux.md)
*   [Mac OS X](README-Mac-OS-X.md)
*   [Solaris](README-Solaris.md)

There are no special instructions for any other Unix flavors.


Building the Library and Example Programs
----
libtabula uses [GNU Autoconf][1], so you can build it with the
standard commands:

    $ ./configure
    $ make
    $ sudo make install            # OR: su -c 'make install'


Configure Options
----
The configure script takes several interesting options. Say:

    $ ./configure --help

to get a list.  Some of the more interesting flags are:

* `--prefix`:

    If you wish to install libtabula in a root directory other than
    /usr/local, run configure with --prefix=/some/dir/name

* `--with-mysql*`:

    If you installed libtabula in an atypical location, the configure
    script will not be able to find the library and header
    files without help.  The simplest way to clue configure into
    where libtabula is installed is with the `--with-mysql` option.
    Try something like `--with-mysql=/usr/local/mysql`, for
    instance.  The configure script will then try to guess which
    subdirectories under the given directory contain the library
    and include files.

    If that doesn't work, it's because the library and header
    files aren't in typical locations under the directory you gave
    for configure to find them.  So, you need to specify them
    separately with `--with-mysql-include` and `--with-mysql-lib`
    instead.  As with `--with-mysql`, `configure` can often guess
    which subdirectory under the given directory contains the
    needed files, so you don't necessarily have to give the full
    path to these files.

* `--with-field-limit`:

    This lets you increase the maximum field limit for template
    queries and SSQLSes.  By default, both are limited to 25
    fields. The user manual [covers this][2] in detail.

* `--enable-thread-check`:

    Builds libtabula with threading support, if possible.

    This option simply turns on two tests: first, that your system uses
    a compatible threading library; and second, that the thread-safe
    version of your DBMS's C API library (e.g. `libmysqlclient_r`)
    is installed and working.  If both of these are true, you get
    a thread-aware version of libtabula.  "Thread-aware" means that
    the library does make an effort to prevent problems, but we don't
    guarantee that all possible uses of libtabula are thread-safe.

    Note that this is a suggestion, not a command.  If we can't
    figure out the system's threading model or can't find the
    thread-aware build of the C API library, configure won't fail.
    It just reverts to the standard single-thread build.

    See the [threads chapter][3] in the [user manual][4] for more
    details and advice on creating thread-safe programs with
    libtabula.


Building a Static Library
----
As shipped, libtabula only builds a shared library.  It's possible to
change things so you get a static library instead.

Before we get to "how," beware that liking statically to libtabula
has legal consequences that may matter to you, due to the library's
license, the GNU LGPL.  Familiarize yourself with the license,
and consider getting legal counsel before proceeding.  Also,
see the [libtabula FAQ][5].  There is more on this topic there.

The necessary changes are all in `libtabula.bkl`:

- Change the `<dll>` tag to `<lib>`.  (Remember the closing tag!)

- Remove the `<dllname>` tag

- Remove the `<so_version>` tag

Then, re-bootstrap the library.  See [HACKERS.md](HACKERS.md) if you
need further instruction on doing that.



[1]: http://www.gnu.org/software/autoconf/
[2]: http://libtabula.org/doc/html/userman/configuration.html#max-fields
[3]: http://libtabula.org/doc/html/userman/threads.html
[4]: http://libtabula.org/doc/html/userman/
[5]: http://libtabula.org/#faq
