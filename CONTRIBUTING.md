Contributing work to the libtabula development effort
====

If you are going to make any changes to libtabula, here are some rules
and hints to keep in mind while you work.


Code Repository Access
----
libtabula 4.0 and newer uses the [Fossil][1] [distributed version
control system][2].

To clone the code repository anonymously, say:

    $ fossil clone https://libtabula.org/code libtabula.fossil

If you have a developer account on libtabula.org's Fossil instance, just
add your username to the URL like so:

    $ fossil clone https://username@libtabula.org/code libtabula.fossil

That will get you a file called `libtabula.fossil` containing the
full history of libtabula from just after the MySQL++ 3.2.1 fork.

You can call that clone file anything you like.  Even the `.fossil`
extension is just a convention, not a requirement.

Then to "open" the repo clone so you can hack on it, say:

    $ mkdir libtabula
    $ cd libtabula
    $ fossil open ../libtabula.fossil

As with `libtabula.fossil`, you can call the working directory
anythihg you like.  I actually prefer a tree like this:

    museum/                    # Where fossils are kept
        libtabula.fossil
    src/                       # Working tree for software projects
        libtabula/
            skull/             # Fossil head, get it?   I crack me up.
            trunk -> skull/    # Alias to match Fossil branch naming
                build/         # Build directory for the skull/trunk
            some-branch/       # Separately-opened working branch
                build/         # Build directory for the working branch
            4.0.0/             # Release branch checkout
        mysqlpp/
            head/              # MySQL++ Fossil trunk checkout
            3.2.3/             # Release tarball unpacked

Fossil will let you make any modifications you like to your local
repository copy.  For those with privileges on the upstream
copy, checkins get automatically synced with it by default.
(If you prefer Git or Mercurial style two-phase commits, you
can say `fossil settings autosync off`.)  If you don't have such
permissions, you just modify your local copy, then have to merge
in upstream changes when updating your local clone.

Developers are expected to make all nontrivial changes on a branch,
rather than check such changes directly into the trunk.  Once we have
discussed the change on the mailing list and resolved any isssues with
the experimental branch, it will be merged into the trunk.

Creating a branch in Fossil is scary-simple, to the point that those
coming from other version control systems may ask, "Is that really all
there is to it?"  Yes, really, this is it:

    $ fossil checkin --branch new-branch-name

That is to say, you make your changes as you normally would; then when
you go to check them in, you give the `--branch` option to the
`ci/checkin` command to put the changes on a new branch, rather than add
them to the same branch the changes were made against.

At some point, the trunk version becomes the next major version.  Stable
versions become either tags or branches.  (The only difference between
tags and branches in Fossil is that branches may have subsequent changes
made to them.)


Bootstrapping the Library
----

If you are coming to libtabula from MySQL++ and remember having to
bootstrap the library to get it to build from a Subversion checkout,
you can forget all that.  CMake takes care of that for us now.  Just
use the normal build procedures documented in `README-*.md`.


CMake Build Directories
----

Although the Bakefile + Autotools based build system allowed separate
source and object file trees, it was not the standard way of working on
MySQL++. If you say something like `./bootsrap && make && make install`,
you get what is called an in-tree build.

CMake works best with out-of-tree builds.  Therefore, the standard
commands after you have "opened" the libtabula Fossil repository are:

    $ mkdir build
    $ cd build
    $ cmake ..

Once CMake has generated the `Makefiles`, you can run `make` as normal.

You may prefer parallel build and source trees to the above nested
scheme.  This works as you would expect:

    $ mkdir ../build
    $ cd ../build
    $ cmake ../trunk


Debug Builds
----

By default, CMake creates a release build on Unixy systems, which means
you don't get debugging symbols in the resulting binaries.  To enable a
debug build, change the "cmake .." command above to:

     $ cmake -DCMAKE_BUILD_TYPE=Debug ..


On Manipulating the Build System Source Files
----

The [CMake build system][4] files are called `CMakeLists.txt`, of
which there are several in the libtabula tree.  These files in turn
depend on files in the `modules` directory off the top level of the
libtabula source tree.

Everything that is customizable about libtabula's build system either
lives in one of these files already, or should be added to one of
them.  Changes go in the file "nearest" to the affected subsystem.
For instance, to customize the way the examples get built, put the
change in `examples/CMakeLists.txt`, not the top-level `CMakeLists.txt`
file.


Submitting Patches
----

The simplest way to send a change to libtabula is to say this after
developing your change against the trunk of libtabula:

    $ fossil diff > my-changes.patch

Then attach that file to a new [mailing list][11] message.

If your change is more than a small patch, `fossil diff` might not
incorporate all of the changes you have made. The old unified `diff`
format can't encode branch names, file renamings, file deletions, tags,
checkin comments, and other Fossil-specific information. For such
changes, it is better to send a Fossil bundle:

    $ fossil checkin --branch my-changes
    $ fossil bundle export --branch my-changes my-changes.bundle

The checkin command will fail on the "autosync" step if you did an
anonymous checkout of the libtabula Fossil repo, but your changes will
get stored in a new branch.  The "bundle" feature of Fossil takes that
branch and packs just your changes up into a file that one of the
developers can temporarily attach to their local repository, then apply
if they approve the changes.

Because you are working on a branch on your private copy of the
libtabula Fossil repository, you are free to make as many checkins as
you like on the new branch before giving the `bundle export` command.

Contributors with a history of providing quality patches/bundles can
apply to get a developer login on [the repository][14].

Please make your patches or experimental branch bundles against the tip
of the current trunk.  libtabula often drifts enough during development
that a patch against a stable release may not apply to the trunk cleanly
otherwise.


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

File types: `dbx`, `hta`

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

* Unix line endings.  Notepad can't cope with LF line endings, and
  it is the default handler of `*.txt` on Windows, but then, such
  files are all build and test system files, so you really want to be
  editing them in a competent programmer's text editor to begin with,
  and such things understand LF line endings.

  As for `*.md`, there is no default association for that on Windows,
  so you might as well associate it with either a competent plain
  text editor or a Markdown editor/viewer.

When in doubt, mimic what you see in the current code.  When still
in doubt, ask on the mailing list.


Testing Your Proposed Change
----

libtabula includes a self-test mechanism called `dtest`.  It's a
Bourne shell script, run much like `exrun`:

    $ ./dtest [-s server_addr] [-u user] [-p password]

This automatically runs most of the examples, capturing the program
outputs to a file, and then compares that to a known-good run's
outputs, stored in `bmark.txt`.  So, before you submit a patch, run
`dtest` to see if anything has changed.

The easiest way to run `dtest` is as `make dtest`, which assumes you
have a "test" user with password "test" on your system's default DBMS.

If `dtest` turns up a change and you can't account for it, it
represents a problem that you'll have to fix before submitting
the patch.  If it gives an expected change, remove `bmark.txt`,
re-run `dtest`, and include the `bmark.txt` diffs in your patch.
This communicates to us the fact that you know there are differences
and want the patch evaluated anyway.  Otherwise, we are likely to
view the change as a regression.

`dtest` also runs all of the unit tests in `test/*` which don't
purposely fail.

(One of the unit tests won't even build, on purpose, because it tests
whether a compile-time check within the library stops compilation as
it should.)

The purpose of `test/*` is different from that of `examples/*`:

*   `test/*` are unit tests: each tests only one libtabula class,
    independent of everything else.  Because DB access requires
    several libtabula classes to cooperate, a unit test never accesses
    a database; hence, no unit test needs DB connection parameters.
    We will never get 100% code coverage from `test/*` alone.

*   `examples/*` can be thought of as integration tests: they
    test many pieces of libtabula working together, accessing a real
    database server.  In addition to ensuring that all the pieces work
    together and give consistent results from platform to platform
    and run to run, it also fills in gaps in the code coverage where
    no suitable `test/*` module could be created.

*   `test/*` programs always run silently on success, writing
    output only to indicate test failures.  This is because they're
    usually only run via `dtest`.

*   `examples/*` are always "noisy," regardless of whether they
    succeed or fail, because they're also run interactively by people
    learning to use libtabula.

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


Building libtabula with Unsupported C++ Compilers/IDEs
----

As described above, libtabula uses the CMake build system, which
actually supports more IDE and compiler types than libtabula is
known to work with.  Before you go trying to hack up a one-off
set of project files, or whatever, run `cmake --help` and see if
there is already a generator for your favorite tool.  If there is,
it might work without any help.

If there is a CMake generator for your tool and it doesn't work, we're
going to want to fix it, either by changing the libtabula CMake input
files or by working to get CMake itself fixed.  We don't want your
hand-built IDE project files.  They're unmaintainable; we know from
experience that they won't get updated every time something in the
library changes that would require a change to all supported platform
build systems.  It's why we use a cross-platform build system like
CMake in the first place.

If you do get libtabula building on a platform that isn't already
known to work, please test it thoroughly.  Run its test suite, and get
it linking to your own program and doing real work before reporting
success.  At that point, a success report on [the mailing list][11]
would be welcome.  Even more welcome would be a `README-*.md` file
for your platform.


[1]:  http://fossil-scm.org/
[2]:  http://en.wikipedia.org/wiki/Distributed_revision_control
[3]:  http://svn.gna.org/viewcvs/*checkout*/mysqlpp/trunk/HACKERS.txt
[4]:  http://cmake.org/
[5]:  README-Unix.md
[6]:  http://cygwin.com/setup-x86.exe
[7]:  http://cygwin.com/setup-x86_64.exe
[8]:  http://tomayko.com/writings/that-dilbert-cartoon
[9]:  http://www.vmware.com/appliances/
[10]: http://www.vmware.com/products/player
[11]: http://libtabula.org/ml/
[12]: http://libtabula.org/code/tktnew
[13]: http://linux.die.net/man/1/indent
[14]: http://libtabula.org/code/
