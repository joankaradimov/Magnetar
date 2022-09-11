# MagnetarCraft

This is the executable file for Magnetar. It contains some code that
loads starcraft.exe into memory (via a brilliant little library called
[MemoryModule](https://github.com/fancycode/MemoryModule)). Afterwards it
mixes its own code with that of Starcraft.

## Overview

MagnetarCraft loads `Starcraft.exe` within its own address space and treats it
like a DLL file. This is possible in part because both DLL and EXE files are
in the PE format. The obvious problem is that `Starcraft.exe` exposes no
symbols except its entry point. This is addressed (pun intended) by faking
symbols via C++ references to memory locations in `Starcraft.exe`.

## Symbols

Those can be found under `set/starcraft_exe/offsets.cpp`/
`include/starcraft_exe/offsets.h`/`include/starcraft_exe/types.h`. All files
are auto-generated and should not be edited manually. For more details see
[idb-export](https://github.com/joankaradimov/MagnetarCraft/tree/master/idb-export).

## Address 0x400000

StarCraft.exe (like most 32 bit x86 code) is not relocatable. This means that
the executable needs to be loaded at address 0x400000 for its code to function
properly. Becuase of ASLR (Address-Space-Layout-Randomization) newer Windows
versions only reliably load the first module of the process at its requested
base address. Since the first module is always `MagnetarCraft.exe`,
`StarCraft.exe` will end up being loaded somewhere else.

To fix this, `MagnetarCraft.exe` does this:
  - it uses 0x300000 as its base address
  - it allocates a lot of static memory in a separate segment, hoping that
    0x400000 will end up there
  - it fails if 0x400000 is not in that segment or if there is insufficient
    space to fit `StarCraft.exe`
  - it calls `LoadLibrary` with `StarCraft.exe`, if all looks good
  - TODO: tell a bit about the role of the `MemoryModule` library

## EXE patching

TODO: tell a bit about these:
  - CALL_SITE_PATCH
  - NOT_PATCH
  - MEMORY_PATCH
  - FAIL_STUB_PATCH
  - FUNCTION_PATCH
