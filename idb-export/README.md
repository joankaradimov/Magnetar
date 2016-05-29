# IDB Export

## Overview

The purpose of this python script is to dump the types, functions and data
inside an .idb file (a database for Hex-Rays' Interactive Disassembler) to
C++ source files and headers. These expose pointers and references to the inner
workings of Starcraft.exe.

Scalar data is exposed as refences, arrays as pointers, functions as function
pointers. This preserves the syntax of the original function/data usage.

## Details

The script generates 3 files:
    - starcraft_exe_types.h
      A header file containing types from the database. This also includes some
      DirectX and Win32 headers and defines some hard-coded macros.

    - starcraft_exe.h
      A header file containing the declarations of pointers and references that
      use addresses in Starcraft.exe.

    - starcraft_exe.cpp
      A source file that initializes the data in starcraft_exe.h

## Usage

TODO
