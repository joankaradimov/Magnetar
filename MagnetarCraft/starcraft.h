#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include "starcraft_exe.h"

void localDll_Init_(HINSTANCE a1);
void GameMainLoop_();
