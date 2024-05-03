#pragma once

#include <functional>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <windowsx.h>

#include "starcraft_exe/types.h"
#include "starcraft_exe/offsets.h"

using namespace game::starcraft;

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
DEFINE_ENUM_FLAG_OPERATORS(RegistryOptionsFlags);

extern std::function<void()> on_end_game;
extern bool frame_capping;
extern bool has_viewport;
extern bool has_hud;
extern bool end_mission_prompt;
extern bool keep_app_active_in_background;
extern MusicTrackDescription title_music;
extern const MusicTrackDescription* current_music_track;
extern std::vector<RaceId> SELECTABLE_RACES;

// TODO: move to campaign.h, maybe?
extern struct Campaign* active_campaign;
extern int active_campaign_entry_index;

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
int ReadMapData_(const char* source, MapChunks* a4, int is_campaign);
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
const char* __stdcall get_GluAll_String_(GluAllTblEntry tbl_entry);
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
void DisplayEstablishingShot_();
void sub_46D1F0_();
void sub_46D200_(const MusicTrackDescription* music_track);
void sub_46D220_(dialog* a1);
void sub_46D3C0_(dialog* dlg);
int __fastcall flc_only_mouseover_(dialog* dlg, dlgEvent* evt);
int getMapListEntryCount_(int(__stdcall* callback)(MapDirEntry*, char*, MapDirEntryFlags), MapDirEntryFlags flags, char* directory, char* filename);
unsigned sub_4A8050_(MapDirEntry* a1, char* source, int a3, unsigned int a4, unsigned __int8 a5, char* dest);
const char* __stdcall getRaceString(RaceId race);
void InitializeInputProcs_();
void __stdcall BWFXN_videoLoop_(int flag);
void BWFXN_RedrawTarget_();
void InitializeDialog_(dialog* a1, FnInteract a2);
void TitlePaletteUpdate_(int a1);
void BriefingStart_(dialog* a1, int buffer);
void sub_4D35A0_();
void showDialog_(dialog* dlg);
int LoadGameTemplates_(TemplateConstructor template_constructor);
int InitializeNetworkProvider_(Char4 provider_id);
void SetCursorClipBounds_();
void __cdecl refreshSelectionScreen_();
void initMapData_();
void BINDLG_BlitSurface_(dialog* dlg);
void GroundAttackInit_(__int16 x, __int16 y);
void BWFXN_OpenGameDialog_(char* a1, FnInteract a2);
void __fastcall BWFXN_QueueCommand_(const void* buffer, unsigned int buffer_size);
void minimapPreviewUpdateState_();
bool LoadPrecursorCampaign();
bool LoadCampaignWithCharacter_(Campaign& campaign);
void PlaySoundAtPos_(SfxData sfx, points a2, int a3, int a4);
unsigned int getTextDisplayTime_(const char* text);
void __fastcall game_menu_handler_(dialog* dlg);
int __fastcall gamemenu_Dlg_Interact_(dialog* dlg, dlgEvent* evt);
void __fastcall gameMenu_BINDLG_(dialog* dlg);
void open_game_menu_();
void open_exit_game_menu_();
void open_abort_menu_();
void open_quit_mission_menu_();
void open_options_menu_();
void open_tips_dialog_(int a1);
void open_help_menu_();
void options_OK_(dialog* dlg);
void __fastcall options_menu_handler_(dialog* dlg);
void __fastcall quit_mission_menu_handler_(dialog* dlg);
int __fastcall tips_dialog_main_(dialog* dlg, dlgEvent* evt);
void ReportGameResult_();
void PauseGame_maybe_();
DWORD sub_4A2B60_();
void SetInGameInputProcs_();
int sub_4BDB30_(PALETTEENTRY* palette, PALETTEENTRY a2);
void sub_4AD140_();
void sub_41E9E0_(int a1);
void sub_4DCEE0_();

template <size_t TIMERS_COUNT>
void DlgSwooshin_(dialog* dlg, swishTimer(&timers)[TIMERS_COUNT], __int16 a4)
{
	DlgSwooshin_(dlg, timers, TIMERS_COUNT, a4);
}

template <typename T>
void BWFXN_QueueCommand__(const T& buffer)
{
	BWFXN_QueueCommand_(&buffer, sizeof(T));
}
