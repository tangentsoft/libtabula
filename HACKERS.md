Hacking on libtabula
====

If you are going to make any changes to libtabula, here are some rules
and hints to keep in mind while you work.


Code Repository Access
----
libtabula 4.0 and newer uses the [Fossil][1] [distributed version
control system][2].

To clone the code repository, say:

    $ fossil clone http://libtabula.org/code libtabula.fossil

That will get you a file called `libtabula.fossil` containing the
full history of libtabula from just after the MySQL++ 3.2.1 fork.
(If you need to dig into libtabula's history prior to the fork,
see the instructions in [the MySQL++ equivalent to this file][3].)
You can call that clone file anything you like.  Even the `.fossil`
extension is just a convention, not a requirement.

Then to "open" the repo clone so you can hack on it, say:

    $ mkdir libtabula
    $ cd libtabula
    $ fossil open ../libtabula.fossil

As with libtabula.fossil, you can call the working directory
anythihg you like.  I actually prefer a tree like this:

    src/
        libtabula.fossil
        libtabula/
            head/              # Fossil libtabula trunk checkout
            4.0.0/             # release branch checkout
        mysql++/
            head/              # Gna! MySQL++ svn trunk checkout
            3.2.1/             # release tarball unpacked

Fossil will let you make any modifications you like to your local
repository copy.  For those with privileges on the upstream
copy, checkins get automatically synced with it by default.
(If you prefer Git or Mercurial style two-phase commits, you
can say `fossil settings autosync off`.)  If you don't have such
permissions, you just modify your local copy, then have to merge
in upstream changes when updating your local clone.


Bootstrapping the Library
----
When you open a clone of the Fossil repository, there are a lot
of things "missing" as compared to a distributed tarball, because
the repository contains only source files, no generated files.
The process that turns a fresh libtabula repository checkout into
something you can build and hack on is called bootstrapping.

Boostrapping is best done on a modern Unix type platform: Linux, OS
X, BSD, Solaris...any version released in the last 4 years or so.
It's possible to do it on Windows, but harder; enough so that we
cover the options below in a separate section.

Two of the tools you need to do this are commonly available on
Unixy systems, at least as an option: Perl 5 and Autoconf 1.59
or higher.  If they're not installed, you can probably run your
system's package manager to install suitable versions.

There's a third tool you'll need to bootstrap libtabula called
[Bakefile][4]. You will need Bakefile 0.2.5 or higher, which in turn
requires Python 2.3 or higher to run.  To build Bakefile from source,
you will also need SWIG, so if you don't have that, you'll want to
use one of the binary builds of Bakefile.

Once you have all the tools in place, you can bootstrap libtabula
with a Bourne shell script called bootstrap, which you get as part
of the repo checkout.  It's fairly powerful, with many options.
For most cases, it suffices to just run it without any arguments:

    $ ./bootstrap

For more unusual situations, here's the complete usage:

    $ ./bootstrap [no{doc,ex,lib,opt}] [pedantic] [bat] [configure flags]

**Bootstrap Script Arguments:**

* *nodoc*: The documentation won't be considered a prerequisite
for building the distribution tarball.  This is useful on systems
where the documentation doesn't build correctly, and you only
need to make a binary RPM.  That process requires a tarball, but
doesn't need the documentation.  Don't distribute the tarball or
SRPM that results, as they are no good for any other purpose.

* *noex*: The generated makefiles and project files won't try to
build any of the examples.

* *nolib*: The generated makefiles and project files won't try
to build the libtabula library.

* *nomaint*: Turn off "maintainer mode" stuff in the build.
These are features used only by those building libtabula from the
code repository.  The 'dist' build target uses this when creating
the tarball.

* *noopt*: Compiler optimization will be turned off.  (This
currently has no effect on MinGW or Visual C++.)

* *pedantic*: Turns on all of GCC's warnings and portability
checks.  Good for checking changes before making a public release.

* *bat*: Asks `cmd.exe` to run `bootstrap.bat` for you.  This is
useful when using Cygwin just as a command shell in preference to
cmd.exe, as opposed to using Cygwin to build libtabula using its
native tools.  Passing `bat` stops all command line processing
in the bootstrap script, so if you also pass some of the other
options, make `bat` last.  The only options that affect the built
project files and makefiles work are the `no*` ones.

* *configure options*: As soon as the bootstrap script sees an
option that it doesn't understand, it stops processing the command
line.  Any subsequent options are passed to the configure script.
See [the Unix README][5] for more on configure script options.


Bootstrapping the Library Using only Windows
----
The thing that makes bootstrapping on Windows difficult is that
one of the required steps uses a Unix-centric tool, Autoconf.
This section is about working out a way to get that working on
Windows, or avoiding the need for it, so you can get on with
hacking on libtabula on Windows.

The thing autoconf does that's relevant to Windows builds of libtabula
is that it substitutes the current libtabula version number into
several source files.  This allows us to change the version number in
just one place &mdash; `configure.ac` &mdash; and have it applied to
all these other places.  Until you do this step, a repo checkout of
libtabula won't build, because these files with the version numbers
in them won't be generated.

**Option 1: Copy the generated files over from a released version**

Only one of these generated files is absolutely critical to allowing
libtabula to build: `src/libtabula.h`.  So, the simplest option
you have to bootstrap libtabula entirely on Windows is to copy
`src/libtabula.h` over from a released version of libtabula.  While
you're doing that, you might copy over the other such generated files:

    install.hta
    libtabula.spec
    doc/userman/userman.dbx
    src/Doxyfile
    src/ssx/Doxyfile

Having done that, you can complete the bootstrapping process by
running `bootstrap.bat`.  It has the same purpose as the Bourne
shell script described above, but much simpler.  It has none
of the command line options described above, for one thing.

The main downside of doing it this way is that your changed
version will have the same version number as the release of
libtabula you copied the files from, unless you go into each
file and change the version numbers.

**Option 2: Cygwin**

If you'd like to hack on libtabula entirely on Windows and
have all the build freedoms enjoyed by those working on Unixy
platforms, the simplest solution is probably to install Cygwin.

First, get the Cygwin installer. There are [32-bit][6] and [64-bit][7]
versions.

When you run it, it will walk you through the steps to install Cygwin.
Autoconf and Perl 5 aren't installed in Cygwin by default, so when
you get to the packages list, be sure to select them.  Autoconf is
in the Devel category, and Perl 5 in the Interpreters category.

You will also need to install the native Windows binary version of
[Bakefile][4].  Don't get the source version and try to build Bakefile
under Cygwin; it won't work.  The Windows binary version of Bakefile
includes an embedded version of Python, so you won't need to install
Cygwin's Python.

Having done all this, you can follow the Unix bootstrapping
instructions in the previous section.

**Option 3: "[Here's a nickel, kid, get yourself a better computer.][8]"**

Finally, you might have access to a Unixy system, or the
ability to set one up.  You don't even need a separate physical
computer, now that virtual machine techology is free.

For example, you could [download a Linux "appliance"][9] and the free
[VMware Player][10] to run it on your Windows machine.  You'd do the
repo checkout of libtabula on that machine and bootstrap it there
using the instructions in the previous section. That done, just copy
the result over to the Windows machine to continue hacking on it.


On Manipulating the Build System Source Files
----
One of the things the bootstrapping system described above
does is produces various types of project and make files from a
small number of source files.  This system lets us support many
platforms without having to maintain separate build system files
for each platform.

[Bakefile][4] produces most of these project and make files from a
single source file called `libtabula.bkl`.

Except for small local changes, it's best to change `libtabula.bkl`
and "re-bake" the project and make files rather than change those
files directly.  You can do this with the bootstrapping scripts
covered above.  On Windows, if all you've changed is `libtabula.bkl`,
you can use `rebake.bat` instead, which doesn't try to do as much
as `bootstrap.bat`.

Bakefile produces finished project files for Visual C++ and Xcode and
finished makefiles for MinGW.  It also produces `Makefile.in`, which
is input to GNU Autoconf along with `configure.ac` and `config/*`.
You may need to change these latter files in addition to or instead
of `libtabula.bkl` to get the effect you want.  Running `bootstrap`
incorporates changes to all of these files in the GNU Autoconf output.

While Bakefile's documentation isn't as comprehensive as it
ought to be, you can at least count on it to list all of the
available features.  So, if you can't see a way to make Bakefile
do something, it's likely it just can't do it.  Bakefile is a
high-level abstraction of build systems in general, so it'll never
support all the particulars of every odd build system out there.


Submitting Patches
----
If you wish to submit a patch to the library, please send it to [the
libtabula mailing list][11], or [create a ticket][12] and attach the patch
to the ticket.  We want patches in unified diff format.

The easiest way to get a unified diff is to check out a copy of the
current libtabula tree as described above.  Then make your change,
and ask Fossil to generate the diff for you:

    $ fossil diff > mychange.patch

If your patch adds new files to the distribution, you can say
`fossil add newfile` before you do the diff, which will include the
contents of that file in the patch.  (You can do this even when
you've checked out the tree anonymously.)  Then say `fossil revert
newfile` to make Fossil forget about the new file.

Please don't submit patches against branches of the repository or
against released versions. libtabula often drifts enough during
development that a patch against anything other than the tip of the
trunk won't apply cleanly.


The libtabula Code Style
----
Every code base should have a common code style.  Love it or
hate it, here are libtabula's current code style rules:

**Source Code**

File types: `ac`, `cpp`, `h`, `in`, `m4`, `pl`

* Tabs for indents, size 4

* Unix line endings.  Any decent programmer's editor can
  cope with this, even on Windows.

* C/C++ rules:

    - Base whitespace style is AT&Tish: K&R/Stroustrup, plus a little
      local spice.  If you have the [`indent(1)`][13] program, the
      command is:

            indent -kr -nce -cli4 -ss -di1 -psl -ts4 FILES...

          That is, start with K&R, don't cuddle else, indent case
          statement labels, space before semicolon with empty loop
          body, no extra space between a variable type and name, return
          value of function on separate line from rest of definition,
          and use 4-column tabs.

    - Class names are in `CamelCase`, uppercased first letter

    - Method names are in `all_lower_case_with_underscores()`;
      ditto most other global symbols.

    - Macro names are in `ALL_UPPERCASE_WITH_UNDERSCORES`

    - All public declarations require a Doxygen comment unless
      there is a very good reason to keep the thing undocumented.

* Perl and shell script rules are more or less the same
  as for C/C++, to the extent this makes sense.


**XML/HTML Dialects**

File types: `bkl`, `dbx`, `hta`

* Spaces for indents, size 2.  Shallow indents due to the
  high level of nesting occurring in such files, and spaces
  because they're not as annoying at shallow indent levels
  in editors that don't treat space indents like tabs.

* Unix line endings.  Again, these are intended to be viewed
  in a programmer's text editor, which should work with Unix
  line endings no matter the platform.


**Plain Text Files**

File types: `md`, `txt`

* Spaces for indents, size 4.  Spaces because such files
  are often viewed in Notepad and similarly crippled text
  editors which use a default indent level of 8.

* DOS line endings, again for the Notepad reason.  And on
  modern Unixy platforms, the tools cope with DOS line endings
  reasonably well.  Better than the converse, anyway.

When in doubt, mimic what you see in the current code.  When still
in doubt, ask on the mailing list.


Testing Your Proposed Change
----
libtabula includes a self-test mechanism called `dtest`.  It's a
Bourne shell script, run much like `exrun`:

    $ ./dtest [-s server_addr] [-u user] [-p password]

This automatically runs most of the examples, captures the outputs
to a file, and then compares that to a known-good run's outputs,
stored in `bmark.txt`.  So, before you submit a patch, run `dtest`
to see if anything has changed.  If something has and you can't
account for it, it represents a problem that you'll have to fix
before submitting the patch.  If it gives an expected change,
remove `bmark.txt`, re-run `dtest`, and include the `bmark.txt` diffs in
your patch.  This communicates to us the fact that you know there
are differences and want the patch evaluated anyway.  Otherwise,
we are likely to view the change as a bug.

`dtest` also runs all of the unit tests in `test/*`.  The purpose of
`test/*` is different from that of `examples/*`:

* `test/*` are unit tests: each tests only one libtabula class,
  independent of everything else.  Because DB access requires
  several libtabula classes to cooperate, a unit test never
  accesses a database; hence, no unit test needs DB connection
  parameters.  We will never get 100% code coverage from
  `test/*` alone.

* `examples/*` can be thought of as integration tests: they
  test many pieces of libtabula working together, accessing
  a real database server.  In addition to ensuring that all
  the pieces work together and give consistent results from
  platform to platform and run to run, it also fills in gaps
  in the code coverage where no suitable `test/*` module could
  be created.

* `test/*` programs always run silently on success, writing
  output only to indicate test failures.  This is because
  they're usually only run via dtest.

* `examples/*` are always "noisy," regardless of whether they
  succeed or fail, because they're also run interactively by
  people learning to use libtabula.

Patches should include tests if they introduce new functionality or fix
a bug that the existing test coverage failed to catch.  If the test
is noisy, needs DB access, or tests multiple parts of the library at
once, it goes in `examples/*`.  If your change affects only one class
in libtabula and testing it can be done without instantiating other
libtabula classes &mdash; other than by composition, of course &mdash;
it should go in `test/*`.

In general, prefer modifying an existing `examples/*` or `test/*`
program.  Add a new one only if you're introducing brand new
functionality or when a given feature currently has no test at all.

Beware that the primary role the examples is to illustrate points
in the user manual.  If an existing example does something similar
to what a proper test would need to do and the test doesn't change
the nature of the example, don't worry about changing the example
code.  If your test would change the nature of the example, you
either need to do the test another way, or also submit a change
to `doc/userman/*.dbx` that incorporates the difference.


Adding Support for a Different Compiler
----
As described above, libtabula uses the Bakefile system for
creating project files and makefiles.  This allows us to make
changes to a single set of files, and have the proper changes be
made to all generated project files and makefiles.  In the past,
we used more ad-hoc systems, and we'd frequently forget to update
individual project files and makefiles, so at any given time,
at least one target was likely to be broken.

If libtabula doesn't currently ship with project files or makefiles
tuned for your compiler of choice, you need to work through the
Bakefile mechanism to add support.  We're not willing to do ad-hoc
platform support any more, so please don't ask if you can send
us project files instead; we don't want them.

If you want to port libtabula to another platform, we need to be
confident that the entire library works on your platform before
we'll accept patches.  In the past, we've had broken ports that
were missing important library features, or that crashed when built
in certain ways.  Few people will knowingly use a crippled version
of libtabula, since there are usually acceptable alternatives.
Therefore, such ports become maintenance baggage with little
compensating value.


[1]:  http://fossil-scm.org/
[2]:  http://en.wikipedia.org/wiki/Distributed_revision_control
[3]:  http://svn.gna.org/viewcvs/*checkout*/mysqlpp/trunk/HACKERS.txt
[4]:  http://bakefile.org/
[5]:  README-Unix.md
[6]:  http://cygwin.com/setup-x86.exe
[7]:  http://cygwin.com/setup-x86_64.exe
[8]:  http://tomayko.com/writings/that-dilbert-cartoon
[9]:  http://www.vmware.com/appliances/
[10]: http://www.vmware.com/products/player
[11]: http://libtabula.org/ml/
[12]: http://libtabula.org/code/tktnew
[13]: http://linux.die.net/man/1/indent