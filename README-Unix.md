Building libtabula on Unix
====

Platform Variations
----

This file only covers details common to all Unix variants supported by
libtabula  For platform-specific details, see the file appropriate to
your OS:

*   [Cygwin](README-Cygwin.md)
*   [Linux](README-Linux.md)
*   [macOS](README-macOS.md)
*   [Solaris](README-Solaris.md)

There are no special instructions for any other Unix flavors.


Building the Library and Example Programs
----

libtabula uses the [CMake][1] build system.  The standard way to build
and install a CMake based program on a Unix type OS is:

    $ mkdir build                   # in libtabula source root
    $ cd build
    $ cmake ..
    $ make
    $ sudo make install             # OR: su -c 'make install'


Building a Static Library
----

As shipped, libtabula only builds a shared library.  To build a static
library instead, simply change the `SHARED` tag on the `add_library()`
command in `src/CMakeLists.txt` to `STATIC`, then `make clean && make`.

Changing `CMakeLists.txt` automatically triggers a reconfiguration, but
if you don't "clean" the library between builds, it may not link
properly.

Beware that liking statically to libtabula has legal consequences that
may matter to you, due to the library's license, the GNU LGPL.
Familiarize yourself with the license, and consider getting legal
counsel before proceeding.  Also, see the [libtabula FAQ][2].  There is
more on this topic there.



[1]: http://cmake.org/
[2]: http://libtabula.org/#faq
