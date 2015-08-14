Wishlist
====

This is a list of all the ideas for features and fixes that we have
seriously contemplated for libtabula.

Patches for any of these thoughtfully considered!  See the [HACKERS.md
file][1] for instructions on sending patches.

Any Version
-----------

Items in this section can be done at any time because they do not break
either the API or the ABI.

*   test/manip doesn't test the escape manipulator!

*   Move unit tests out from under dtest umbrella into CTest.

*   The `DateTime` family's `time_t` conversion operators should detect
    DATETIME("0") type values and return `time_t(0)` rather than
    attempt to stuff an out-of-range value into a smaller box.

*   Any time you must hand-roll some SQL code in your program,
    consider whether it could be generalized to a widely-useful
    API feature.

*   Suppress DOS line-ending related diffs from `examples/cgi_jpeg`
    output when running dtest on Windows.  Check for -D?

*   Need to link statically to connect to MySQL Embedded?
  ([Report](http://stackoverflow.com/questions/672451/))

*   Query::storein(slist<T>&) is unusable.  As a template method, it
    must be defined in a header file; we cannot #include config.h from
    a header, thus the proper HAVE macro that would let us define this
    template method is never defined.

    One solution is to create lib/slist.h.in, parameterized by the
    detected slist type name and the header file defining it.  This
    will #include the proper header file, define SList<T>::Type (a
    template typdedef: http://www.gotw.ca/gotw/079.htm), and #define
    LIBTABULA_HAVE_SLIST.  Then Query::storein() can be wrapped by an
    ifdef checking for LIBTABULA_HAVE_SLIST, only defined when the
    configure script found a suitable slist type.

    Also create a default lib/slist.h file, checked into svn as
    lib/slistdef.h and copied to slist.h on tarball creation.  Remove
    this file early in configure script run, so we're forced to
    overwrite its contents with detected values.  Default version
    contains ifdefs for non-autoconf platforms where we know what
    slist definition is available on that platform.  Xcode, for
    instance, will let us use <ext/slist>.

*   Add ctor and assignment operator that takes libtabula::null.  You
    now have to call a 4-argument version of either the ctor or
    assign() to get a NULL SQL string.

*   The `escape_q()` functions aren't returning the right value when
    dealing with `Null<T>` wrapped types such as `sql_blob_null`.

*   The current method SSQLS uses to compare floating point numbers
    is highly dubious.  It just subtracts them, and checks that
    the absolute difference is under some threshold.  The manual
    warns that this is fine for "human scale" applications, but even
    that's not actually true.  It means that if Larry Ellison loses
    a hundredth of a penny in his couch, it is somehow significant.
    I have no idea how much money Larry Ellison is comfortable losing
    to his couch cushions, but it's probably closer to 1 ppm than
    the current threshold, which is 100 parts per quadrillion on
    the scale of $1 bn.

    For backards compatibility, we should keep this method, but we
    should add these two more mathematically sound methods:

    -   Percentage: Divide the smaller number into the larger, then
        compare against a threshold.  The default should be
        something like 1.000001 (1 ppm), which lets us make much
        finer distinctions without running out of precision, even
        with single-precision numbers counting Bill Gates' losses to
        his couch cushions.

    -   Logarithmic, or "Bels": Same as percentage, but on a
        log<sub>10</sub> scale so it works better for FP numbers,
        which are based on powers of 10.  Logarithms are more costly
        than division, and we still need a division for this, so it
        shouldn't be the new default.

        1 ppm is ~4.3&times;10<sup>-7</sup>, which is below what
        single-precision FP can distinguish.  Increasing the threshold
        to a value you *can* distinghish with a 32-bit IEEE float
        makes it ignore significant amounts of money in Carlos Slim's
        couch cusions.  (Hundreds of dollars.)  Therefore, we should
        use something like 1e-7 or 1e-8 anyway, and make it clear
        that the default threshold is only suitable for doubles.

        Someone using single precision FP should increase the threshold
        to 1e-5 or so.  Such a person would be assumed to know what
        they're doing.

        It's probably more efficient to change the algorithm from:

            double diff = log10(a > b ? a / b : b / a);

        to:

            double diff = fabs(log10(a / b));

        Logarithms give the same magnitude result for a/b as b/a,
        differing only in sign. `fabs()` is probably implemented as
        an intrinsic that just clears a single bit, which should be
        cheaper than a floating point comparison followed by a jump.

        With suitable tuning, this method would allow you to
        distinguish the change lost by a single Pentagon contractor's
        lobbyist to a single couch, on a single occasion, as compared
        to the combined net worth of all Pentagon contractors and
        their employees, assigns, mistresses, and, ah, hired help.
        If you use doubles, anyway.


Must Be Done in v4.0
-------------

Items in this section are big or break the library's ABI, so they have
to be done in 4.0, else they must wait for the next allowed ABI breakage
point, 5.0.

*   Reimplement the ostringstream interface in SQLStream, drop the
    inheritance, and include an ostreamstring instance as a member
    instead of deriving from it.  There is some evidence that VC++ has
    problems with dynamic linkage to libraries that export classes
    derived from Standard C++ classes.

    Derive Query from SQLStream instead of std::ostream?

    That might be enough.  All other derivations from Standard C++
    Library classes derive from vector and exception, which are much
    less likely to be a problem.

*   Convert from Bakefile to CMake.  See the TODO list in the top-level
    `CMakeLists.txt` file.

*   Database independence:

    -   `Row` remains implemented in terms of `MYSQL_ROW`.  Need to
	    create a `[MySQL]RowImpl` class hierarchy and pass instances
	    of them via the pimpl idiom to `Row` instead of `MYSQL_ROW`.
	    An easy case is `MySQLDriver::fetch_row()`, part of the
	    low-level implementation of "use" queries, but the real
	    trick is doing it for "store" queries.

    -   Field is entirely MySQL-specific.  Rather than pimpl it,
        consider just ripping it out, along with the only other
        users of it, Result::fetch_field*().  All it does is let the
        library user iterate through a thinly-wrapped version of the
        C API's view of the table schema.  That's basically a form
        of reflection, which isn't really a key use case for libtabula.

        If we want to expose schema exploration, we can do so via
        DBDriver.

        Having done this, maybe rename String to Field, since that
        makes Row a vector-of-Field, which makes more sense than
        vector-of-String.  It also solves the old quandary about the
        difference between libtabula::String vs std::string.

    -   Pass a single string parameter to Connection::connect(), being
        a URL scheme which we pass to DBDriver::create(), a virtual
        ctor which looks at the bit before the colon to figure out which
        subclass to create.  Examples:

        -   `mysql://[user][:password][@][host][:port][/db]` = MySQL
            driver with optional parameters.  If you give only
            "mysql://", it uses default user name, no password,
            localhost, default port, and selects no DB to start.
            You can give any combination of other optional parts,
            within reason; you must give @ if you give either user
            or password, but can leave it off if you are using the
            defaults, for example.

        -   `db://...` = Generic form of `mysql://` URL scheme.  We
            infer the DB type based on the URL parameters.  If nothing
            in the string gives us reason to prefer a driver, it
            tries the first compatible driver.  For instance, if
            you pass a port, it won't try to use the SQLite driver,
            but if you have two network DBMS drivers compiled in and
            the port is 3306, it infers MySQLDriver.

        -   `odbc://[user:password@]DSN` = ODBC

        -   `sqlite3:///path/to/foo.db` = Most specific form of SQLite
            URL, specifying the SQLite v3 API, with the assumption
            that we will one day have a sqlite4 URL scheme, allowing
            libtabula to support both APIs if they differ enough to
            matter to libtabula.

        -   `sqlite:///path/to/foo.db` = Generic "any SQLite" form;
            dunno whether it should imply sqlite3 or sqlite4 if
            both are compiled in.  On the one hand, sqlite3 would be
            forward-compatible, whereas by not specifying, the code
            could arguably be electing not to care.

        -   `file:///path/to/foo.db` = Most generic form for local
            file-based DBMSes, where we have to do either process
            of elimination or magic byte checks to figure out what
            driver to use.  If only SQLite is built in, it must be
            a SQLite DB.

    -   Map C API error numbers to libtabula constants in
        DBDriver::errnum().  Rename to error_code()?

    -   Must create at least two other DBDriver subclasses to
        ensure base class is reusable before releasing v4.0.
        PostgresDriver and SQLiteDriver?

    -   Templatize all classes that use DBDriver interface with the
        DB driver type.  This lets you specify the driver type to use
        with a Connection and all its children without modifying the
        existing method parameter lists.  This also lets us worry less
        about C API types, as they can be hidden away behind typedefs:

            class MysqlDriver : public DBDriver { ...
                typedef MYSQL_ROW row_type;
                ...
            }

            template <class DBD = MysqlDriver>
            class Connection ... { ...
                Query<DBD> query();
                ...
            }

            template <class DBD = MysqlDriver>
            class UseQueryResult { ...
                DBD::row_type fetch_raw_row();
            }

    -   Will CMake let us figure out which DB engines are available
        on non-autoconf systems, or at least pass in options that
        let the user select them?

*   If `pkg-config` is available, register ourselves with it using
    information discovered by `configure`.  Also, write out a
    `libtabula-config` script, which either wraps `pkg-config` or
    reinvents it, poorly, for systems that don't have it.

*   Add `String::operator==(const libtabula::null_type&)`.  Needed to
    allow comparison of `row[x]` returns to SQL `null`.  Change one of
    the examples to show it?

*   Memory "leak" and C API library init fixes:

    -   Add `DBDriver::library_begin()` and `library_end()`, wrapping
        similarly named functions in the C API.

    -   Create `Process` class, which you create at the top of `main()`
        on the stack, purely to call these automatically.

    -   Update userman to recommend creating `Process` object in
        `ConnectionPool` derivatives, instead.

    -   Create `Thread` class to call existing
        `DBDriver::thread_start()` and `thread_end()`, similar to
        Process, created on the stack of the thread entry function.

    -   Move memory leak FAQ into userman, rewriting it to cover
        all this.

*   `libtabula::execute` manipulator.  Immediately executes built
    query string.  Works best with exceptions, as that's the only
    way to detect failures.

*   Chris Frey's `packarray` class

*   Create adaptors for `std::bitset`, for storing binary data in a
    table.  Make two options available, one for storing the return
    from `bitset::to_ulong()` in an `UNSIGNED INTEGER` column, and
    another for storing a larger set of bits in a more flexible way,
    perhaps as a `BLOB`.

*   Has experience with new thread awareness changed our mind on
    atomic inc/dec of reference counts in `RefCounted*`?

*   Optional checked conversions in `String` for numerics.

    Throw `BadConversion` on range overflow?

*   Add `Query::storein_if()`, mirroring `store_if()`

*   Add a method to `libtabula::String` to return a widened version of the
    string.  Probably make return type templatized so we can return
    `wstring`, C++/CLI native strings, etc.  Then convert examples that
    do this conversion to use this new mechanism.

*   Try to add operator `std::string` to `String`.  If it doesn't work,
    explain why not in the userman and in `Row::operator[]` refman.
  
*   Subclass `libtabula::Blob` from `libtabula::String`, differing
    only in that it knows that it should be automatically quoted and
    escaped when inserted into a SQL query.

*   If SSQLSv2 does use a common base class, change `Query` template
    methods taking SSQLS into concrete methods taking `SsqlsBase&`.

*   Make `Query::insert()`, `replace()` and `update()` execute their
    queries immediately.  Requires an ABI break, because they'll
    have to return `SimpleResult`.

    Ensure that calling `execute()` on the resulting auto-reset `Query`
    object is a no-op.

*   Switch Query's safe `bool` to overload `basic_ios<>::operator
    void*()` instead.  We create an ambiguous conversion in `bool`
    context with some C++ standard libraries otherwise.

*   Templatize `libtabula::String` on `value_type` so it can be used to
    hold wide characters.  Then the method that converts UTF-8 to the
    platform's best wide character type can just return a different
    variant of `libtabula::String`.

*   `Transaction` class should check an "in transaction" flag on
    `Connection` (or `DBDriver`) before sending `BEGIN`, defaulting to
    false.  If set, the `Transaction` object does nothing.  If not
    set, set it and send the query.  This prevents it from trying
    to set up nested queries, which MySQL doesn't support.

*   Remove throw-spec for `std::out_of_range` from
    `SQLTypeAdapter::at()`.  It no longer throws this, and throw-specs
    are passe&eacute; anyway.

*   Store failed query string in `BadQuery` exception object, to make
    logging and debugging easier.  One could have a try block wrapping
    many queries, and be able to recover the failed query string from
    the exception object, instead of somehow keeping track yourself.
    ([Patch](http://lists.mysql.com/plusplus/8374))

*   `Query` and `SQLStream` could have a common base class that would
    allow the stream manipulator functions to catch and modify
    strings based on only one `dynamic_cast` instead of requiring
    two as it does since the addition of the `SQLStream` class.

*   Make internal call chain steps like `Query::execute(SQLQueryParms&)`
    protected?  No good reason for end users to call it, and making
    it part of the public API causes people to try calling it, and
    discovering that it's not a very elegant interface, compared to
    the ones taking `SQLString`.

*   SQL time type allows &plusmn; 839 hours of range.  We currently
    can't cope with negative times, and if we change it to use signed
    integers, we'll still only get &plusmn; 127 hours instead of +255.
    Need to switch the hour field to a `signed short` to get the
    full range.

*   Get rid of two-step create in `DBDriver`, requiring a connection to
    be established in ctor for object to be valid?  RAII.  The
    DB-specific functions that don't require a connection can be
    static methods.  Tricky bit: a failed `Connection::connect()` call
    will likely be followed by an indirect call to `DBDriver::err*()`.
    Does `Connection` cache the error value and message?  If we can pull
    this off, we can drop the `DBDriver::is_connected_` flag and change
    `Connection::connected()` to `return driver_ != 0`.

*   Audit library for non-virtual methods that could be profitably
    made virtual.  Many are non-overridable now purely because
    changing them would break the ABI, so now's the time to fix that.

*   Connection has some query-building functions (e.g. ping()) that do
    explicit checks on the connection state before running, while the
    main execution paths (exec(), use(), etc.) do not.  We should remove
    all the explicit checks.  The underlying DBDriver level will detect
    the error, thus ensuring that we handle this problem in a consistent
    way.


v4.1 or Later
----

Items in this section could go in v4.0, as they do not break the ABI,
but they will probably have to wait for round-tuits to appear after
v4.0 ships.

*   Finish [SSQLS v2](http://lists.mysql.com/plusplus/6929):

    -   C++ code generator, from walking DSL parse tree.
        `examples/stock.ssqls` gives `ssqls_stock.h` containing:

            class SQLStock : public libtabula::SsqlsBase
            {
            public:
                SQLStock(Connection* conn = 0); // default ctor
                SQLStock(const SQLStock& other);
                SQLStock(const libtabula::Row& row,
                        Connection* conn = 0); // full init from query
                SQLStock(libtabula::sql_bigint key1); // exemplar creation
                SQLStock(Connection* conn,
                        libtabula::sql_bigint key1) // calls load();
                SQLStock(
                    libtabula::sql_bigint f1,
                    libtabula::sql_bigint f2,
                    libtabula::sql_double f3,
                    libtabula::sql_double f4,
                    const libtabula::sql_date& f5,
                    const libtabula::sql_mediumtext& f6); // full init
                SQLStock(Connection* conn,
                    libtabula::sql_bigint f2,
                    libtabula::sql_double f3,
                    libtabula::sql_double f4,
                    const libtabula::sql_date& f5,
                    const libtabula::sql_mediumtext& f6); // calls create()
                SQLStock(Connection* conn,
                    libtabula::sql_bigint f1,
                    libtabula::sql_bigint f2,
                    libtabula::sql_double f3,
                    libtabula::sql_double f4,
                    const libtabula::sql_date& f5,
                    const libtabula::sql_mediumtext& f6); // calls save()

                bool createTable(Connection* conn = 0) const;
                const char* getTableName() const
                        { return class_table_name_ || table(); }
                void setTableName(const char* name)
                        { instance_table(name); }
                static void setTableName(const char* name)
                        { class_table_name_ = name; }

                std::ostream& equal_list(std::ostream& os) const;
                std::ostream& json(std::ostream& os) const;
                std::ostream& name_list(std::ostream& os) const;
                std::ostream& value_list(std::ostream& os) const;
                std::ostream& xml(std::ostream& os) const;

                libtabula::sql_bigint getId() const;
                libtabula::sql_bigint getNum() const;
                libtabula::sql_double getWeight() const;
                libtabula::sql_double getPrice() const;
                const libtabula::sql_date& getSdate() const;
                const libtabula::sql_mediumtext& getDescription() const;

                void setId(libtabula::sql_bigint value);
                void setNum(libtabula::sql_bigint value);
                void setWeight(libtabula::sql_double value);
                void setPrice(libtabula::sql_double value);
                void setSdate(const libtabula::sql_date& value);
                void setDescripion(const libtabula::sql_mediumtext& value);

                bool operator<(const SQLStock& rhs) const;
                SQLStock& operator=(const SQLStock& rhs);

            protected:
                libtabula::sql_bigint id_;
                libtabula::sql_bigint num_;
                libtabula::sql_double weight_;
                libtabula::sql_double price_;
                libtabula::sql_date sdate_;
                libtabula::sql_mediumtext description_;

                std::bitset<6> set_fields_;

            private:
                static const char* class_table_name_;
            };

        ...and `ssqls_stock.cc`, containing implementation for same.

    -   Ensure we're using case-insensitive SQL column to C++
        field name matching.  Column names aren't case-sensitive
        in SQL.

    -   SQL table updater/builder, -T option, taking .ssqls and
        creating or updating the DB table to match.

    -   If using accessors, generate `std::bitset<num_fields> is_set_`,
        and set the appropriate bit when calling each `setFoo()` so we
        can intuit which fields were set.  Probably also need an enum:

            enum FieldIndices {
                id_field_index_,
                num_field_index_,
                ...
            };

        This will allow `Query::select()`, for instance, to figure
        out that we want it to select by a non-key field, returning
        all matches.

    -   Define `operator<<` for SSQLS and a set of manipulators which
        govern whether the operator calls `equal_list()`, `json()`,
        `name_list()`, `value_list()`, or `xml()` to do the actual
        insertion.

    -   Define `operator<<` for `sequence_container<SsqlsBase>`
        and similar for associative containers.  Give `mysql(1)`-like
        ASCII grid or some other table format.

    -   Define `operator>>` for SSQLS, taking XML as input, in the form
        emitted via `xml()`.  expat uses the new BSD license, so
        maybe we can just drop it in the tree, with an option to
        use the platform expat on Autoconf systems.

    -   MySQL table metadata to SSQLSv2 data structure translator.
        (`-s`, `-u`, `-p` and `-t` flag support.)  Add this to `dtest`,
        extracting `stock.ssqls` definition from DB.

    -   Replace `Query`'s template methods taking SSQLSes with concrete
        methods taking `const SsqlsBase&`.

    -   Create `Query::remove(const SsqlsBase&)`

    -   Remove `explicit` from `Date`, `DateTime` and `Time` ctors taking
        stringish types.

    -   Auto-`create()` ctor: if there is an `auto_increment` field,
        populate it on query success.

    -   Detect ctor conflicts corresponding to `sql_create_N(N, N...)`
        in SSQLSv1.  `ssqlsxlat` can be smart enough to just not emit
        duplicate ctors.

    -   Special case of ctor suppression: if the number of key fields
        equals the total number of fields, you get an auto-`load()`
        ctor, not auto-`update()`.

    -   Replace `CREATE TABLE` SQL in `resetdb` with `create_table()`
        calls.

    -   Option to use integer indices into Row when populating?  Only
        bother if it gives a speed advantage we can actually see,
        because it removes all the dynamic typing advantages we
        got with the change to field name indices in MySQL++ v3.0.

    -   Try to add `Query::storein(container, ssqls)` which generates
        `SELECT * FROM {ssqls.table()}` and stores the result.
        May not be possible due to existing overloads, but try.
        If it works, use this form in the userman Overview section,
        saving one LOC.

    -   Convert SSQLS v1 examples to v2.  SSQLS v2 isn't done until
        there's a straightformward conversion path for all examples.

    -   Add `#warning` to generated `ssqls.h` saying that you should
        now use SSQLS v2.  Wrap it in a check for
        `LIBTABULA_ALLOW_SSQLS_V1`, so people can disable the warning.

    -   Add `Query::storein<Container, T>(container)`, getting table
        name from `container::value_type.table()` instead.

*   Define `operator<<` for `Fields`, `Row`, `StoreQueryResult`, etc.

    Make output format selectable: CSV, XML and JSON to start?  Could
    add YAML, BSON, etc. later.

    This and SSQLS v2 will allow us to get rid of `libexcommon`, since
    we won't need hand-rolled data dumping code.

*   Bring back mandatory quoting for manipulators?  If someone says
    `os << libtabula::escape << foo` do they not really really
    mean `escape foo`?  Automatic quoting and escaping is
    [different](http://lists.mysql.com/plusplus/7999)

*   Configure script should try to get MySQL C API directories
    from `mysql_config`.

*   Create a backtick manipulator for use by `field_list()` in `row.h`
    and `ssqls.h`.  These currently use `do_nothing0`, but that prevents
    use of SQL reserved words as identifiers.

*   Add wrapper functions to `Null<>` like `length()` that call the
    corresponding function on data member, if present, to make it
    more transparent.  At minimum, mirror the `std::string` API.

*   Create a thread-safe message queue for separating DB access and
    data use into multiple threads.  Something like `ConnectionPool`,
    optional and with no ties to the library internals.  There could
    be an adapter between one end of the queue and a `Connection`
    object, which creates `Queries` to handle standardized messages,
    delivering the results back to the queue.

*   Create a fixed-point data type for use with SQL's `DECIMAL` and
    related types.  Right now, `sql_decimal` is a typedef for `double`,
    so you lose accuracy in the fractional part.  Don't forget to
    include an `is_null` flag to cope with conversion from infinite
    or `NaN` float values; that's how MySQL stores these.

*   Create libdbi or SOCI DBDriver subclasses?  Will give a fast path
    to other DBMSes, at the expense of doubled abstraction layers.


Not Until v5.0 At Earliest
----

We could do these items in v4.0 if we had infinite resources, but
because they are not Our itches and they break the ABI, they probably
won't get done until we start v5.0 at earliest.

*   Add STL-like custom `Allocator` template parameters to memory-hungry
    classes like `Row`?  Useful in apps that process lots of data over
    long periods, causing heap fragmentation with the default C++
    allocator.

*   Some sort of support for prepared statements.  Can we hijack
    the template query mechanism?
  
*   Wrap `LOAD DATA INFILE`:

        bool Query::load_file(
            const char* path,
            bool local_path,
            const char* table,
            const char* field_terminator = 0,   // default \t
            const char* field_encloser = 0,     // default none
            const char* field_escape = 0,       // default \
            const char* line_terminator = 0,    // \n on *ix, \r\n on Windows
            const char* line_starter = 0,       // default none
            const char* character_set = 0,      // default UTF-8
            const char* comment_prefix = 0,     // ignore no lines
            bool replace_existing = false,
            bool ignore_duplicates = false,
            bool low_priority = false,
            int skip_lines = 0);

*   Wrappers for above: `load_local_tab_file()`, `load_local_csv_file()`,
    `load_remote_*()`...

*   `Query::save_file()` interfaces, wrapping `SELECT ... INTO FILE`,
    modeled on above.


[1]: HACKERS.md
