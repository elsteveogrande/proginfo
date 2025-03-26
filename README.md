
# libRuntimeDiagnostics, `librtd` for short

Simple, clean, portable library for runtime diagnostics.
Requires no dependencies other than a C++ compiler supporting C++17.

This aims to be fully C++17-compliant with little to no little `#if`-checking
for doing different things on different platforms.  However this is only
tested with Clang and LLVM at the moment (but `g++` is expected to also work).

* Supports OSX and Linux runtimes
* Zero heap allocation, and otherwise minimal memory usage
* Signal-safe operations
* Optional caching to speed up repeated operations

In short, this aims to replicate some of GNU's [`libbacktrace`](https://github.com/ianlancetaylor/libbacktrace) and intends to provide support for runtime stack traces.  The hope is that this can be used for `libcxx`'s C++23 stacktrace support.  In the future this might support more, but the first milestone is to
get stacktraces working for C++ in LLVM's C++ library.

`librtd` includes these components:

## Binary file support

* `<librtd/binfile/ELF.h>`: an ELF parser and "navigator"
* `<librtd/binfile/MachO.h>`: for Mach-O files

## Debug info

* `<librtd/debug/DWARF.h>`: for [DWARF version 5](https://dwarfstd.org/doc/DWARF5.pdf) (currently latest) access

## Inspecting the current process

* `<librtd/procinfo/ProcInfo.h>`: for information about modules
(program images) in the current process, such as shared libraries
and the current executable
  - `<librtd/procinfo/OSX.h>` for OSX-specific impl
  - `<librtd/procinfo/Linux.h>` for Linux-specific impl
  - (you don't need to include these specifically;
    `ProcInfo.h` does the right thing automatically for you)

This project does not support Windows (or PE, DLL, PDB, etc.),
because Windows already has support for all the above things
in its `dbghelp.dll` library.

# Prerequisites

* `clang++` supporting C++17 (`g++` probably works)
* `make`, only if you want to run tests
* `docker`, only if you want to run tests under different platforms

# Installation

There's no installation per se.
Just include this project's headers, e.g.:

```
clang++ -I/path/to/librtd/include
```

# Testing

Just `make test`.  Note that only those tests enabled for the current platform will be performed.  Consider trying a docker image (e.g. see `images/linux` if you're
running OSX or something else).

# License

Released under the MIT license; see `LICENSE` file.

# Authors

* `steve@obrien.cc`

# Links

Projects worth mentioning because they are awesome and have been
great resources in learning all this stuff:

* https://github.com/ianlancetaylor/libbacktrace
* https://github.com/bombela/backward-cpp/blob/master/backward.hpp
* https://github.com/aidansteele/osx-abi-macho-file-format-reference

## Assorted references

* https://github.com/torvalds/linux/blob/master/include/uapi/linux/elf.h
* https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
* https://dwarfstd.org/doc/DWARF5.pdf

