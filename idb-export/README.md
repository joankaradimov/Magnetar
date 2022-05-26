# IDB Export

## Overview

The purpose of this python script is to dump the types, functions and data
inside an .idb file (a database for Hex-Rays' Interactive Disassembler) to
C++ source files and headers. These expose pointers and references to the inner
workings of StarCraft.exe.

Scalar data is exposed as refences, arrays as pointers, functions as function
pointers. This preserves the syntax of the original function/data usage.

## Details

The script generates 3 files:

  - starcraft_exe/types.h\
    A header file containing types from the database. This also includes some
    DirectX and Win32 headers and defines some hard-coded macros.
  - starcraft_exe/offsets.h\
    A header file containing the declarations of references that use addresses
    in StarCraft.exe.
  - starcraft_exe/offsets.cpp\
    A source file that defines the symbols within StarCraft.exe

## Usage

TODO
