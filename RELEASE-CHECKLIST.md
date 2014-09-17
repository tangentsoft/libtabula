-   Change version number in the top-level `CMakeLists.txt`.

    All other places the version number occurs are files generated
    by CMake from a corresponding `*.in` file.

-   Run `make abicheck`.  There should be no changes.

    You may have to run the following command in the current "stable"
    directory before this will succeed, since it depends on there being
    an ACC dump file in place already.

        $ abi-compliance-checker -lib tabula -dump abi.xml

    This assumes a working tree that looks something like this:

        projects/
           libtabula/
              4.0.0/                # unpacked stable tarball
              head/                 # Fossil repo working checkout

    The working checkout directory's name doesn't matter, it just needs
    to be next to the stable tree so that ../4.0.0 finds it.

    This dependence on an existing ABI dump file is deemed reasonable
    since the ABI of the stable version had better not be changing!
    Plus, it saves some processing time, since ACC can load the stable
    ABI info without re-parsing its headers and library file.

-   Reconfigure the build system in pedantic mode, then do a clean
    rebuild.  Fix any new errors and warnings.

    Known bogus warnings:

    -   `Query`'s `std::basic_ios<>` base class is not being
        initialized.  Yes, we know.  We don't care.

    -   The `==` float comparisons in `lib/stadapter.cpp` are harmless.
        They're comparisons against special `NaN` and `infinity``
        constants.  Those are safe.
