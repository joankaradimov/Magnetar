#pragma once

#include <functional>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include "starcraft_exe/types.h"
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
extern MusicTrackDescription title_music;
extern const MusicTrackDescription* current_music_track;
extern std::vector<RaceId> SELECTABLE_RACES;

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
int load_gluGameMode_BINDLG_();
void registerMenuFunctions_(FnInteract* functions, dialog* a2, int functions_size);
void stopMusic_();
void DLGMusicFade_(const MusicTrackDescription* music_track);
const char* GetNetworkTblString_(__int16 network_tbl_entry);
int BWFXN_gluPOKCancel_MBox_(const char* a1);
void DLG_SwishIn_(dialog* a1);
char __stdcall DLG_SwishOut_(dialog* dlg);
void DlgSwooshin_(dialog* dlg, swishTimer* timers, size_t timers_count, __int16 a4);
void BWFXN_gluPOK_MBox_(const char* a1);
void changeMenu_();
void* fastFileRead_(int* bytes_read, int searchScope, const char* filename, int defaultValue, int bytes_to_read, const char* logfilename, int logline);
dialog* getControlFromIndex_(dialog* dlg, __int16 index);
int gluLoadBINDlg_(dialog* a1, FnInteract fn_interact);
GotFileValues* InitUseMapSettingsTemplate_();
char* __stdcall get_GluAll_String_(GluAllTblEntry tbl_entry);
int ContinueCampaign_(int a1);
void sub_4BCA80_(SfxData a1);
void sub_4DBF80_();
int LoadSaveGameBIN_Main_(int a1, RaceId a2);
dialog* LoadDialog(const char* bin_path);
MenuPosition BWFXN_NetSelectReturnMenu_();
dialog* loadAndInitFullMenuDLG_(const char* filename);
void __cdecl freeChkFileMem_();
signed int sub_4D4130_();
void sub_4D3860_();
int sub_4EE210_();
int sub_4EEFD0_();
int killTimerFunc_();
void HideDialog_(dialog* dlg);
int LobbyLoopCnt_();
void load_MinimapPreview_();
void DisplayEstablishingShot_();
void sub_46D1F0_();
void sub_46D200_(const MusicTrackDescription* music_track);
void sub_46D220_(dialog* a1);
void sub_46D3C0_(dialog* dlg);
int __fastcall gluRdyZ_Secret_(dialog* dlg, dlgEvent* evt);
int getMapListEntryCount_(int(__stdcall* callback)(MapDirEntry*, char*, MapDirEntryFlags), MapDirEntryFlags flags, char* directory, char* filename);
unsigned sub_4A8050_(MapDirEntry* a1, char* source, int a3, unsigned int a4, unsigned __int8 a5, char* dest);
const char* __stdcall getRaceString(RaceId race);
void InitializeInputProcs_();
void __stdcall BWFXN_videoLoop_(int flag);
void BWFXN_RedrawTarget_();
void InitializeDialog_(dialog* a1, FnInteract a2);
void TitlePaletteUpdate_(int a1);

template <size_t TIMERS_COUNT>
void DlgSwooshin_(dialog* dlg, swishTimer(&timers)[TIMERS_COUNT], __int16 a4)
{
	DlgSwooshin_(dlg, timers, TIMERS_COUNT, a4);
}
