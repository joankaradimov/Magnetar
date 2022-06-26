#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include "starcraft_exe/offsets.h"

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int INTERFACE_HEIGHT = 96;

const int TILE_SIZE = 32;
const int TILE_WIDTH = TILE_SIZE;
const int TILE_HEIGHT = TILE_SIZE;

const int GAME_AREA_WIDTH = SCREEN_WIDTH;
const int GAME_AREA_HEIGHT = SCREEN_HEIGHT - 80;

const int GAME_AREA_TILE_COLUMNS = (GAME_AREA_WIDTH - 1) / TILE_WIDTH + 1;
const int GAME_AREA_TILE_ROWS = (GAME_AREA_HEIGHT - 1) / TILE_HEIGHT + 1;

const int RENDER_AREA_TILE_COLUMNS = GAME_AREA_TILE_COLUMNS + 1;
const int RENDER_AREA_TILE_ROWS = GAME_AREA_TILE_ROWS + 1;

const int RENDER_AREA_WIDTH = RENDER_AREA_TILE_COLUMNS * TILE_WIDTH;
const int RENDER_AREA_HEIGHT = RENDER_AREA_TILE_ROWS * TILE_HEIGHT;

// when screen is 640x480, this is 301056
const int TILE_CACHE_SIZE = RENDER_AREA_WIDTH * RENDER_AREA_HEIGHT;

void localDll_Init_(HINSTANCE a1);
void GameMainLoop_();
int AppAddExit_(AppExitHandle handle);
void AppExit_(bool exit_code);
