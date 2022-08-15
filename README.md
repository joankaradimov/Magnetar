# Magnetar

A type of neutron star with an extremely powerful magnetic field.

## Overview

Magnetar is a StarCraft mod/reimplementation. It loads Starcraft.exe to memory and
combines its code and assets with Magnetar's own.

## Installation

1. You'll need to have StarCraft 1.16.1 installed.
1. You'll find the lastest release\
   [here](https://github.com/joankaradimov/Magnetar/releases).
   Download it and extract it to a directory of your choice.
1. TODO: add more instructions once version 0.2 is released

## Hacking

The repo uses submodules for 3rd party dependencies. So when cloning, you might
want to recursively clone the submodules:

```bash
git clone --recurse-submodules git@github.com:joankaradimov/Magnetar.git
```

### Hacking on Windows

Install Visual Studio with support for `C++ CMake tools for Windows`. At the
time of writing both VS2019 and VS2022 were able to compile the project.

- To open the project in VS:\
  In visual Studio hit `File -> Open -> Folder` and select the root folder.
- To run/debug the project:\
  Expand the `MagnetarCraft` directory, right click on any `.cpp` file under
  it and select `Set as Startup Item`.

### Hacking on Linux/Unix

The project **DOES NOT** build or run on non-Windows platforms. A mid-term goal
is to have support them via [winelib](https://wiki.winehq.org/Winelib) (without
using the `wine` command). A long-term goal is for the project to be truly
multi-platform via a set of libraries (e.g. SDL).

Other limitations, that will prevent the project from building:
  - There's a ton of auto-generated MASM assembly that will not compile on clang/gcc.
  - There might be small amounts of hand-written MASM assembly too.
  - The `cmake` build scripts only work with the MSVC toolchain.
  - The `MemoryModule` library will need some work to function on Linux.

### Directory organization

Below is an overview of the components of the project. Some of the
subdirectories have a README of their own. See those for more details.

- [MagnetarCraft](https://github.com/joankaradimov/Magnetar/tree/master/MagnetarCraft)\
  The executable file and entry point for the project.
- [MagnetarDat](https://github.com/joankaradimov/Magnetar/tree/master/Magnetorm)\
  Files that are included in the MPQ used by the project.
- [Magnetorm](https://github.com/joankaradimov/Magnetar/tree/master/Magnetorm)\
  A wrapper (and maybe one day a reimplementation) of `storm.dll`.
- [idb-export](https://github.com/joankaradimov/Magnetar/tree/master/idb-export)\
  Anything related to IDA Pro goes here.
- [MpqBuilder](https://github.com/joankaradimov/Magnetar/tree/master/MpqBuilder)\
  A tool that uses StormLib and is used by the project's cmake scripts to
  generate MPQ files
- [yaml-cpp](https://github.com/jbeder/yaml-cpp)\
  A C++ library for parsing and emitting YAML.
- [StormLib](https://github.com/ladislav-zezula/StormLib)\
  [Ladislav Zezula](https://github.com/ladislav-zezula)'s library for working
  with Blizzard MPQ archives.
- [MemoryModule](https://github.com/fancycode/MemoryModule)\
  Library for loading DLL files from memory.
- [LuaJIT](https://github.com/WohlSoft/LuaJIT)
  Just-In-Time Compiler for the Lua programming language (fork with cmake support)
- [sol2](https://github.com/ThePhD/sol2)
  A Lua API wrapper
