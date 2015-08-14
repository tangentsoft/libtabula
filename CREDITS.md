Credits
====

MySQL++ was created by Kevin Atkinson during 1998, who passed off
maintenance to [Sinisa Milivojevic](mailto:sinisa@mysql.com), who worked
on the library from version 1.0 (June 1999) through 1.7.9 (May 2001).
The current maintainer is [Warren Young](mailto:wyoung@libtabula.org),
starting with MySQL++ 1.7.10 in August of 2004.  Warren forked libtabula
off from MySQL++ 3.2.1 sometime in 2014, which was first released as
version 4.0.0 in XXXXXX 201x.

A fuller account of the library's history &mdash; including the
transition from MySQL++ to libtabula &mdash; is [in the user manual][1].
For the nitty-gritty details, see [the libtabula change log][2] and
[the MySQL++ change log][3]. Change log items that aren't attributed to
anyone else were done by Warren Young, much of it while working for
Educational Technology Resources.


Other contributors of note since MySQL++ 1.7.10:

*   [Chris Frey](mailto:cdfrey@foursquare.net): Lots of GCC warning fixes
    for the bleeding-edge compiler versions, Gentoo ebuild support,
    and misc other fixes.

*   [Mark Meredino](mailto:Mark_Merendino@cnt.com): Several fixes and
    additions, including a lot of work on Microsoft Visual C++
    compatibility, and discoveries made while spelunking in the
    library.

*   [Evan Wies](mailto:evan@athenacr.com): Contributed several C++ code
    style cleanups.

*   [Arnon Jalon](mailto:Arnon.Jalon@247RealMedia.com): Added the
    multi-query result set handling features, and
    `examples/multiquery.cpp` to demonstrate it.

*   Korolyov Ilya has submitted several patches in many different
    areas of the library.

*   [Remi Collet](mailto:Liste@FamilleCollet.com) is maintaining
    offical RPMs for Fedora, with other systems on the way.  His work
    has improved the RPM spec file we distribute greatly.

*   [Joel Fielder](mailto:joel.fielder@switchplane.com) of Switchplane,
    Ltd. created the `ScopedConnection` class, came up with the
    original idea for the `Query::for_each()` and `store_in()` methods,
    provided the basis for `examples/for_each.cpp`, and provided a fix
    for exception flag propagation in `Query`.

*   [Jim Wallace](mailto:jwallace@kaneva.com) demonstrated the need
    for `BadQuery::errnum()`, and contributed the patches and also
    `examples/deadlock.cpp` to test that this feature does what it is
    supposed to.

*   [Jonathan Wakely](mailto:mysql@kayari.org) rebuilt my original
    versions of `ConnectionPool`, `RefCountedPointer`, and
    `RefCountedBuffer`.  They're now simpler and safer.  He also
    created the numeric conversion logic in `lib/mystring.h` introduced
    in MySQL++ v3.0.

*   [Adrian Cornish](mailto:mysql@bluedreamer.com)  Several fixes and
    additions.

*   [Rick Gutleber](mailto:rgutleber@above.net) contributed the
    `Query::insertfrom()` method and associated `InsertPolicy` object,
    as well as the `SQLStream` class.

Here are the personal credits from the old 1.7.9 documentation,
apparently written by Kevin Atkinson:

*   Chris Halverson - For helping me get it to compile under Solaris.

*   Fredric Fredricson - For a long talk about automatic conversions.

*   Michael Widenius - MySQL developer who has been very
    supportive of my efforts.

*   Paul J. Lucas - For the original idea of treating the query object
    like a stream.

*   Scott Barron - For helping me with the shared libraries.

*   Jools Enticknap - For giving me the Template Queries idea.

*   M. S. Sriram - For a detailed dission of how the Template Queries
    should be implemented, the suggestion to throw exceptions on bad
    queries, and the idea of having a back-end independent query object
    (ie SQLQuery).

*   Sinisa Milivojevic - For becoming the new offical maintainer.

*   D. Hawkins and E. Loic for their autoconf + automake contribution.

Please do not email any of these people with general questions about
libtabula. All of us who are still active in libtabula development read
[the mailing list][4], so questions sent there do get to us.

The mailing list is superior to private email because the answers are
archived for future questioners to find, and because you are likely to
get answers from more people.


[1]: http://libtabula.org/doc/html/userman/index.html#history
[2]: ChangeLog.md
[3]: http://tangentsoft.com/mysql++/ChangeLog
[4]: http://libtabula.org/ml/
