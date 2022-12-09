#pragma once

#include <functional>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include "starcraft_exe/offsets.h"

#define MAX_MAP_DIMENTION 256

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

DEFINE_ENUM_FLAG_OPERATORS(DialogFlags);
DEFINE_ENUM_FLAG_OPERATORS(CheatFlags);
DEFINE_ENUM_FLAG_OPERATORS(MapDirEntryFlags);
DEFINE_ENUM_FLAG_OPERATORS(MegatileFlags);
DEFINE_ENUM_FLAG_OPERATORS(ImageFlags);
DEFINE_ENUM_FLAG_OPERATORS(SaiAccessabilityFlags);
DEFINE_ENUM_FLAG_OPERATORS(PrintFlags);
DEFINE_ENUM_FLAG_OPERATORS(StatusFlags);

extern std::function<void()> on_end_game;
extern bool frame_capping;
extern bool has_viewport;
extern bool has_hud;
extern bool end_mission_prompt;
extern bool keep_app_active_in_background;

void localDll_Init_(HINSTANCE a1);
void GameMainLoop_();
int AppAddExit_(AppExitHandle handle);
void AppExit_(bool exit_code);
void PreInitData_();
void CreateMainWindow_();
void audioVideoInit_();
void LoadInitIscriptBIN_();
void __fastcall CleanupIscriptBINHandle_(bool exit_code);
int LoadReplayFile_(const char* a1, int* a3);
int __stdcall ReadMapData_(const char* source, MapChunks* a4, int is_campaign);
int CreateGame_(GameData* data);
void GameRun_();
