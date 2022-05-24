#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include "starcraft_exe/offsets.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int INTERFACE_HEIGHT = 96;

void localDll_Init_(HINSTANCE a1);
void GameMainLoop_();
