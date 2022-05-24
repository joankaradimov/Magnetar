# MagnetarCraft

A magnetar is a type of neutron star with an extremely powerful magnetic field.

## Overview

MagnetarCraft is a StarCraft engine mod. It loads Starcraft.exe to memory and
combines its code and data with MagnetarCraft's own code.

## Installation

1. You'll need to have StarCraft 1.16.1 installed.

1. You'll find the lastest release
   [here](https://github.com/joankaradimov/MagnetarCraft/releases).
   Download it and extract it to a directory of your choice.

1. TODO: add more instructions once version 0.2 is released

## Hacking

The repo uses submodules for 3rd party dependencies. So when cloning, you might
want to recursively clone the submodules:

```bash
git clone --recurse-submodules git@github.com:joankaradimov/MagnetarCraft.git
```

### Hacking on Windows

Install Visual Studio with support for `C++ CMake tools for Windows`. At the
time of writing both VS2019 and VS2022 were able to compile the project.

- To open the project in VS:
  In visual Studio hit `File -> Open -> Folder` and select the root folder.

- To run/debug the project:
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
  - The `MemoryModule` library will need some work to function on Windows.
