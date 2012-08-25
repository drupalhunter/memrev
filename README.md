memrev
======
memrev is a library for efficiently reversing blocks of memory. It's
implemented in C++ and can easily be called from C or C++ code.

memrev's speed comes largely from its SIMD optimization on capable platforms.
To get the most out of it, you'll need a CPU with 16-byte vector instructions, including some kind of vector shuffle instruction (e.g. Intel
[SSSE3](http://en.wikipedia.org/wiki/SSSE3)).

Compiling
---------
To build memrev, you need the following on your system.

*   [CMake](http://www.cmake.org)
*   A recent version of [Clang](http://clang.llvm.org) (tested with 3.1). A
    GCC port is incomplete but in the works.

Invoke `cmake -DCMAKE_CXX_COMPILER=clang++ [...] <srcdir>` to generate build
files for your system.

**Note**: Vector acceleration is disabled by default. To enable it, you must do
two things:

1.  Pass `-DMEMREV_USE_VECTOR=ON` to CMake.
2.  Explicitly tell the compiler that the target CPU supports vector
    instructions, e.g. via passing `-DCMAKE_CXX_FLAGS='-mssse3'` (or
    `-DCMAKE_CXX_FLAGS='-march=native'` if you are compiling for your
    own vector-enabled CPU).

Doing the first without doing the second will actually result in *slower* code
than you'd get by disabling vector instructions altogether, so be careful!

If you want to build the tests, you need to do the following:

*   Extract the [gtest](http://code.google.com/p/googletest/) source tree into
    a subdirectory of the memrev source tree named `gtest`.
*   Pass `-DMEMREV_BUILD_TESTS=ON` to CMake.

Using
-----
The top-level project in this repository produces a binary, `rev`, that mimics
the behavior of the `rev` utility in util-linux.

The library itself is located in the `libmemrev` subfolder and can be compiled
separately.
