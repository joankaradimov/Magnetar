#include <algorithm>
#include <filesystem>
#include <ddraw.h>
#include <process.h>
#include <time.h>
#include "starcraft_exe/offsets.h"
#include "magnetorm.h"
#include "tbl_file.h"
#include "patching/patching.h"

void SysWarn_FileNotFound_(const char* a1, int last_error)
{
	char dwInitParam[512];
	char Buffer[256];

	char* v2 = GetErrorString(Buffer, 0x100u, last_error);
	_snprintf(dwInitParam, 0x200u, "%s\n%s", a1, v2);
	SErrSuppressErrors(1);
	SNetLeaveGame(3);
	SNetDestroy();
	if (GetCurrentThreadId() == main_thread_id)
	{
		BWFXN_DDrawDestroy();
		BWFXN_DSoundDestroy();
	}
	if (DialogBoxParamA(local_dll_library, (LPCSTR)0x6A, hWndParent, DialogFunc, (LPARAM)dwInitParam) == -1)
	{
		FatalError("GdsDialogBoxParam: %d", 106);
	}
	DLGErrFatal();
}

void __stdcall SysWarn_FileNotFound__(const char* a1)
{
	int last_error;

	__asm mov last_error, ebx

	SysWarn_FileNotFound_(a1, last_error);
}

FUNCTION_PATCH((void*)0x4212C0, SysWarn_FileNotFound__);

void FileFatal_(HANDLE a1, int a2)
{
	char buffer[260];

	if (!SFileGetFileName(a1, buffer, 260))
	{
		SStrCopy(buffer, "*unknown*", 0x104u);
	}
	SysWarn_FileNotFound(buffer, a2);
}

void FileFatal__()
{
	HANDLE a1;
	int a2;

	__asm mov a1, ecx
	__asm mov a2, ebx

	FileFatal_(a1, a2);
}

FUNCTION_PATCH((void*)0x4D2880, FileFatal__);

const int app_exit_handles_count = 32;

void AppExit_(bool exit_code)
{
	if (app_exit_handles && !byte_6D63F0)
	{
		for (byte_6D63F0 = 1; word_519F64 != -1; --word_519F64)
		{
			AppExitHandle exit_handle = app_exit_handles[word_519F64];
			if (exit_handle)
			{
				app_exit_handles[word_519F64] = NULL;
				exit_handle(exit_code);
			}
		}
		SMemFree(app_exit_handles, "Starcraft\\SWAR\\lang\\gds\\appexi.cpp", 50, 0);
		app_exit_handles = NULL;
	}
}

FAIL_STUB_PATCH(AppExit);

int AppAddExit_(AppExitHandle handle)
{
	if (!app_exit_handles)
	{
		app_exit_handles = (AppExitHandle *)SMemAlloc(app_exit_handles_count * sizeof(AppExitHandle), "Starcraft\\SWAR\\lang\\gds\\appaddex.cpp", 42, (int)app_exit_handles);
		memset(app_exit_handles, 0, app_exit_handles_count * sizeof(AppExitHandle));
	}
	int v2 = -1;
	for (int i = 0; i < app_exit_handles_count; i++)
	{
		AppExitHandle exit_handle = app_exit_handles[i];
		if (exit_handle == handle)
			return 0;
		if (v2 == -1 && !exit_handle)
			v2 = i;
	}

	if (v2 == -1)
	{
		FatalError("APPADDEX:1");
	}
	app_exit_handles[v2] = handle;
	return 1;
}

FAIL_STUB_PATCH(AppAddExit);

void __fastcall sub_4C9120_(bool exit_code)
{
	if (dword_596CD0)
	{
		SMemFree(dword_596CD0, "Starcraft\\SWAR\\lang\\opt_dlgs.cpp", 58, 0);
		dword_596CD0 = 0;
		byte_596BC8[0] = 0;
	}
}

FAIL_STUB_PATCH(sub_4C9120);

void sub_4C9C40_(const char* a1)
{
	if (dword_596CD0)
	{
		if (!_stricmp(byte_596BC8, a1))
		{
			return;
		}
		sub_4C9120_(0);
	}
	else
	{
		AppAddExit_(sub_4C9120_);
	}
	dword_596CD0 = LoadGraphic(a1, 0, "Starcraft\\SWAR\\lang\\opt_dlgs.cpp", 80);
	strcpy(byte_596BC8, a1);
}

void sub_4C9C40__()
{
	char* a1;

	__asm mov a1, esi

	sub_4C9C40_(a1);
}

FUNCTION_PATCH((void*)0x4C9C40, sub_4C9C40__);

void __cdecl DLGErrFatal_()
{
	if (GetCurrentThreadId() == main_thread_id)
	{
		SErrSuppressErrors(1);
		AppExit_(1);
		ProcError(1);
		exit(1);
	}
	ExitProcess(1u);
}

FUNCTION_PATCH(DLGErrFatal, DLGErrFatal_);

bool sendInputToAllDialogs_(dlgEvent* evt)
{
	sub_419F80();
	dialog* event_dialog = EventDialogs[evt->wNo];

	if (event_dialog)
	{
		bool result = event_dialog->pfcnInteract(event_dialog, evt);
		if (result)
		{
			pressGlobalDlgHotkey();
			return result;
		}
	}

	for (dialog* dlg = DialogList; dlg; dlg = dlg->pNext)
	{
		int result = dlg->pfcnInteract(dlg, evt);
		if (result)
		{
			pressGlobalDlgHotkey();
			return result;
		}
	}

	pressGlobalDlgHotkey();
	return 0;
}

FAIL_STUB_PATCH(sendInputToAllDialogs);

void RefreshCursorScreen_()
{
	if (ScreenLayers[0].buffers)
	{
		ScreenLayers[0].bits |= 1;
		BWFXN_RefreshTarget(
			(__int16)ScreenLayers[0].left,
			(__int16)ScreenLayers[0].height + (__int16)ScreenLayers[0].top - 1,
			(__int16)ScreenLayers[0].top,
			(__int16)ScreenLayers[0].width + (__int16)ScreenLayers[0].left - 1);
	}
}

FAIL_STUB_PATCH(RefreshCursorScreen);

void __cdecl drawCursor_()
{
	if (last_cursor)
	{
		RefreshCursorScreen_();
		RefreshCursorRect();
		grpFrame* cursor_frame = &last_cursor->frames[dword_597390 % last_cursor->wFrames];
		ScreenLayers[0].width = cursor_frame->wid;
		ScreenLayers[0].height = cursor_frame->hgt;
		ScreenLayers[0].left = cursor_frame->x + Mouse.x - 63;
		ScreenLayers[0].top = cursor_frame->y + Mouse.y - 63;
		RefreshCursorScreen_();
		RefreshCursorRect();
	}
}

FUNCTION_PATCH(drawCursor, drawCursor_);

void UpdateDlgMousePosition_(void)
{
	drawCursor_();
	LOBYTE(InputFlags) = InputFlags & 0xFE;

	dlgEvent v0;
	v0.wNo = EVN_MOUSEMOVE;
	v0.cursor.x = Mouse.x;
	v0.cursor.y = Mouse.y;
	if (!sendInputToAllDialogs_(&v0) && input_procedures[EVN_MOUSEMOVE])
	{
		input_procedures[EVN_MOUSEMOVE](&v0);
	}
	dword_5968EC = 1;
}

FAIL_STUB_PATCH(UpdateDlgMousePosition);

void __fastcall DestroyGluAllStrings_(bool exit_code)
{
	if (gluAllTblDataLoaded && gluAllTblData)
	{
		SMemFree(gluAllTblData, "Starcraft\\SWAR\\lang\\glues.cpp", 688, 0);
	}
}

FAIL_STUB_PATCH(DestroyGluAllStrings);

const char* GetNetworkTblString(__int16 network_tbl_entry)
{
	if (network_tbl_entry == 0)
	{
		return NULL;
	}
	else if (network_tbl_entry < *networkTable + 1)
	{
		return (const char*)networkTable + networkTable[network_tbl_entry];
	}
	else
	{
		return "";
	}
}

const char* GetMapTblString(__int16 map_tbl_entry)
{
	if (map_tbl_entry == 0)
	{
		return "";
	}
	else if (MapStringTbl.buffer == 0)
	{
		return NULL;
	}
	else if (map_tbl_entry < *MapStringTbl.buffer + 1)
	{
		return (char*)MapStringTbl.buffer + MapStringTbl.buffer[map_tbl_entry];
	}
	else
	{
		return "";
	}
}

void* fastFileRead_(int* bytes_read, int searchScope, const char* filename, int defaultValue, int bytes_to_read, const char* logfilename, int logline);

char* __stdcall get_GluAll_String_(GluAllTblEntry tbl_entry)
{
	if (!gluAllTblDataLoaded)
	{
		gluAllTblData = fastFileRead_(0, 0, "rez\\gluAll.tbl", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
		AppAddExit_(DestroyGluAllStrings_);
		gluAllTblDataLoaded = 1;
	}

	if (tbl_entry == STAR_EDIT_NOT_FOUND)
	{
		return 0;
	}

	return tbl_entry - 1 < *(_WORD*)gluAllTblData ? (char*)gluAllTblData + *((unsigned __int16*)gluAllTblData + tbl_entry) : "";
}

FUNCTION_PATCH(get_GluAll_String, get_GluAll_String_);

void PlayMusic_(MusicTrack a1)
{
	if (directsound == NULL || a1 != current_music)
	{
		current_music = a1;
		if (directsound)
		{
			SFileDdaEnd(directsound);
			SFileCloseFile(directsound);
			directsound = 0;
			byte_6D5BBC = 0;
		}
		if (a1 == NULL)
		{
			return;
		}

		SFileOpenFile(music_tracks[a1].wav_filename, &directsound);
	}

	if (directsound)
	{
		if (getArchiveName(directsound))
		{
			if (gwGameMode == GAME_RUN)
			{
				BWFXN_PrintText(8, (char*) GetNetworkTblString(140), 0);
			}
		}
		else
		{
			byte_6D5BBD = 0;
			if (registry_options.Music)
			{
				if (SFileDdaBeginEx(directsound, 0x40000u, music_tracks[a1].track_type != MENU_MUSIC ? 0 : 0x40000, 0, getMusicVolume(), 0, 0))
				{
					byte_6D5BBC = 1;
				}
				else
				{
					if (directsound)
					{
						SFileDdaEnd(directsound);
						SFileCloseFile(directsound);
						directsound = 0;
					}
					byte_6D5BBC = 0;
				}
			}
		}
	}
}

FAIL_STUB_PATCH(PlayMusic);

void playNextMusic_()
{
	unsigned a2;
	unsigned a3;

	if (byte_6D5BBC && music_tracks[current_music].track_type == IN_GAME_MUSIC)
	{
		SFileDdaGetPos(directsound, (int)&a2, (int)&a3);
		if (a2 >= a3)
		{
			PlayMusic_((MusicTrack)music_tracks[current_music].in_game_music_index);
		}
	}
}

FAIL_STUB_PATCH(playNextMusic);

void __stdcall BWFXN_videoLoop_(int flag)
{
	checkLastFileError();
	if (flag)
	{
		MSG Msg;
		while (hasMessagesWaiting(&Msg, flag & 2))
		{
			if (!hAccTable || !hWndParent || !TranslateAcceleratorA(hWndParent, hAccTable, &Msg))
			{
				DispatchMessageA(&Msg);
				if (!dword_6D0530)
				{
					TranslateMessage(&Msg);
				}
			}
		}
		if (CpuThrottle && flag & 2)
		{
			Sleep(1u);
		}
		if (InputFlags & 1)
		{
			UpdateDlgMousePosition_();
		}
		DWORD ticks = GetTickCount();
		if (ticks + message_handling_tick >= 100)
		{
			dlgEvent v3;
			v3.wNo = EventNo::EVN_IDLE;
			v3.cursor.x = Mouse.x;
			v3.cursor.y = Mouse.y;
			if (!sendInputToAllDialogs_(&v3) && input_procedures[EventNo::EVN_IDLE])
			{
				input_procedures[EventNo::EVN_IDLE](&v3);
			}
			message_handling_tick = -ticks;
			if (ticks + dword_6D637C >= 1000)
			{
				PlayBriefingWAVBegin();
				playNextMusic_();
				dword_6D637C = message_handling_tick;
			}
		}
		if (flag & 1)
		{
			iterateTimers();
		}
	}
	else
	{
		memset(is_keycode_used, 0, sizeof(is_keycode_used));
	}
}

FUNCTION_PATCH(BWFXN_videoLoop, BWFXN_videoLoop_);

void sub_4BD3A0_()
{
	if (GameState)
	{
		dword_5993A4 = max(MoveToTile.y - 4, 0);
		dword_5993C0 = min(MoveToTile.y + 404, map_size.height - 1);
		DoVisibilityUpdate(dword_5993A4, dword_5993C0);
	}
}

FAIL_STUB_PATCH(sub_4BD3A0);

DEFINE_ENUM_FLAG_OPERATORS(DialogFlags);

void updateAllDlgs_()
{
	for (dialog* dlg = DialogList; dlg; dlg = dlg->pNext)
	{
		if ((dlg->lFlags & CTRL_UPDATE) == 0)
		{
			dlg->lFlags |= CTRL_UPDATE;
			updateDialog(dlg);
		}
	}
}

FAIL_STUB_PATCH(updateAllDlgs);

bool realizePalette_()
{
	if (!PrimarySurface || !dword_51BFA8 || !hWndParent || IsIconic(hWndParent))
	{
		return false;
	}
	if (dword_6D5E1C)
	{
		SDrawRealizePalette();
		memset(RefreshRegions, 1u, sizeof(RefreshRegions));
		updateAllDlgs_();
		dword_6D5E1C = 0;
	}
	return true;
}

FAIL_STUB_PATCH(realizePalette);

void DoBltUsingMask_()
{
	int Pitch;
	void* lpSurface;

	if (SDrawLockSurface(0, 0, &lpSurface, &Pitch, 0))
	{
		STransBltUsingMask((int)lpSurface, (int)GameScreenBuffer.data, Pitch, SCREEN_WIDTH, dword_6D5E18);
		SDrawUnlockSurface(0, lpSurface, 0, 0);
	}
}

FAIL_STUB_PATCH(DoBltUsingMask);

void sub_41E000_()
{
	if (handle && dword_6D5E18)
	{
		STransDelete(dword_6D5E18);
		STransIntersectDirtyArray(handle, (int)RefreshRegions, 3u, (int)&dword_6D5E18);
		DoBltUsingMask_();
		memset(RefreshRegions, 0, sizeof(RefreshRegions));
	}
}

FAIL_STUB_PATCH(sub_41E000);

void DirtyArrayHandling_()
{
	u8* v0 = GameScreenBuffer.data;
	if (!GameScreenBuffer.data)
	{
		return;
	}
	dword_6CF4A8 = &GameScreenBuffer;
	if (!byte_51A0E9)
	{
		for (layer* v2 = ScreenLayers + 7; v2 >= ScreenLayers; v2--)
		{
			if (v2->buffers)
			{
				bounds b;
				b.left = -v2->left;
				b.top = -v2->top;
				b.right = -v2->left + SCREEN_WIDTH - 1;
				b.bottom = -v2->top + SCREEN_HEIGHT - 1;
				b.width = SCREEN_WIDTH;
				b.height = SCREEN_HEIGHT;
				if (v2 == ScreenLayers)
				{
					BlitCursorSurface(0, 0, &GameScreenBuffer, 0);
				}
				if (v2->bits & 0x21)
				{
					goto LABEL_12;
				}
				if (refreshRect(
					(__int16)v2->left,
					(__int16)v2->top,
					(__int16)v2->left + (__int16)v2->width,
					(__int16)v2->top + (__int16)v2->height))
				{
					v2->bits |= 4;
				LABEL_12:
					v2->pUpdate(0, 0, v2->pSurface, &b);
					v2->bits &= 0xF8u;
					continue;
				}
				if (v2->bits & 2)
				{
					goto LABEL_12;
				}
			}
		}
	}
	else
	{
		unsigned v1 = GameScreenBuffer.wid * GameScreenBuffer.ht;
		memset(GameScreenBuffer.data, 0, 4 * (v1 >> 2));
		memset(&v0[4 * (v1 >> 2)], 0, v1 & 3);

		RECT screen_rect;
		screen_rect.left = 0;
		screen_rect.top = 0;
		screen_rect.right = SCREEN_WIDTH;
		screen_rect.bottom = SCREEN_HEIGHT;
		BlitDirtyArray(&screen_rect);
	}

	sub_41E000_();

	if (!byte_51A0E9)
	{
		if (ScreenLayers[0].buffers)
		{
			BlitBitmap(&GameScreenBuffer);
		}
	}
	dword_6CF4A8 = 0;
}

FAIL_STUB_PATCH(DirtyArrayHandling);

void BWFXN_RedrawTarget_()
{
	sub_4BD3A0_();
	if (realizePalette_())
	{
		if (ScreenLayers[1].buffers)
		{
			if (ScreenLayers[1].width)
			{
				RECT rc;
				SetRect(&rc, ScreenLayers[1].left, ScreenLayers[1].top, ScreenLayers[1].width + ScreenLayers[1].left - 1, ScreenLayers[1].height + ScreenLayers[1].top - 1);
				isRectBoundsInside_Assign_32(&rc, &ScrLimit);
				sub_41C2A0(&rc);
				sub_41C2A0(&stru_6D63F4);
				stru_6D63F4 = rc;
			}
			else
			{
				if (stru_6D63F4.left || stru_6D63F4.right)
				{
					sub_41C2A0(&stru_6D63F4);
				}
				if (stru_6D63F4.left || stru_6D63F4.right || stru_6D63F4.top || stru_6D63F4.bottom)
				{
					stru_6D63F4.top = stru_6D63F4.bottom + 1;
				}
			}
		}
		DirtyArrayHandling_();
	}

	if (dword_6D5E2C)
	{
		SRgnDelete(dword_6D5E2C);
		dword_6D5E2C = 0;
	}
}

FUNCTION_PATCH(BWFXN_RedrawTarget, BWFXN_RedrawTarget_);

HANDLE LoadInstallArchiveHD_(const char* a1, char* a2, const char* mpq_filename, DWORD dwFlags)
{
	if (!GetModuleFileNameA(hInst, a2, 0x104u))
	{
		*a2 = 0;
	}
	char* v4 = strrchr(a2, '\\');
	if (v4)
	{
		*v4 = 0;
	}
	SStrNCat(a2, mpq_filename, 260);
	HANDLE hMpq;
	if (!SFileOpenArchive(a2, dwFlags, 2u, &hMpq))
	{
		return 0;
	}
	if (a1)
	{
		HANDLE phFile;
		if (!SFileOpenFileEx(hMpq, a1, 0, &phFile))
		{
			SFileCloseArchive(hMpq);
			return 0;
		}
		SFileCloseFile(phFile);
	}
	return hMpq;
}

FAIL_STUB_PATCH(LoadInstallArchiveHD);

signed int InitializeCDArchives_(const char *filename, int a2)
{
	if (cd_archive_mpq)
		return 1;

	char path_buffer[MAX_PATH];
	cd_archive_mpq = LoadInstallArchiveHD_(filename, path_buffer, "\\BroodWar.mpq", 1000);
	if (cd_archive_mpq || (cd_archive_mpq = LoadInstallArchiveHD_(filename, path_buffer, "\\StarCraft.mpq", 1000)) != 0)
		return 1;

	cd_archive_mpq = LoadInstallArchiveCD(1000u, "\\Install.exe", filename);
	if (cd_archive_mpq)
		return 1;

	while (a2)
	{
		INT_PTR v4 = DialogBoxParamA(local_dll_library, (LPCSTR)107, hWndParent, DialogFunc, 0);
		if (v4 == -1)
		{
			FatalError("GdsDialogBoxParam: %d", 107);
		LABEL_13:
			AppExit_(0);
			ProcError(1);
			exit(0);
		}
		if (v4 != 1)
			goto LABEL_13;
		cd_archive_mpq = LoadInstallArchiveCD(1000u, "\\Install.exe", (char *)filename);
		if (cd_archive_mpq)
			return 1;
	}
	return 0;
}

int(*signal)(int a1, int a2) = (int(*)(int a1, int a2)) 0x0040C8D5;

void* fastFileRead_(int *bytes_read, int searchScope, const char *filename, int defaultValue, int bytes_to_read, const char *logfilename, int logline)
{
	HANDLE phFile;

	if (!SFileOpenFileEx(0, filename, searchScope, &phFile))
	{
		int last_error = SErrGetLastError();
		if (!bytes_to_read || last_error != 2 && last_error != 1006)
		{
			SysWarn_FileNotFound(filename, last_error);
		}
		if (bytes_read)
			*bytes_read = 0;
		return 0;
	}

	LONG file_size = SFileGetFileSize(phFile, 0);
	if (file_size == -1)
	{
		FileFatal(phFile, GetLastError());
	}
	if (bytes_read)
		*bytes_read = file_size;
	if (!file_size)
	{
		if (bytes_to_read)
		{
			SFileCloseFile(phFile);
			return 0;
		}
		SysWarn_FileNotFound(filename, 24);
	}
	void* buffer = (void*)defaultValue;
	if (!defaultValue)
		buffer = SMemAlloc(file_size, logfilename, logline, defaultValue);

	int read;
	if (!SFileReadFile(phFile, buffer, file_size, &read, 0))
	{
		DWORD last_error = GetLastError();
		FileFatal(phFile, last_error == 38 ? 24 : last_error);
	}
	if (read != file_size)
		FileFatal(phFile, 24);
	SFileCloseFile(phFile);
	return buffer;
}

dialog* LoadDialog(const char* bin_path)
{
	dialog* bin_dialog = (dialog*)fastFileRead_(0, 0, bin_path, 0, 0, __FILE__, __LINE__);
	if (bin_dialog)
	{
		bin_dialog->lFlags |= DialogFlags::CTRL_ACTIVE;
		AllocInitDialogData(bin_dialog, bin_dialog, AllocBackgroundImage, __FILE__, __LINE__);
	}

	return bin_dialog;
}

void InitializeFontKey_(void)
{
	char buff[MAX_PATH];
	_snprintf(buff, MAX_PATH, "%s\\%s.gid", "font", "font");
	void* v0 = fastFileRead_(&cdkey_encrypted_len, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (v0 && cdkey_encrypted_len == 0)
	{
		SMemFree(v0, "Starcraft\\SWAR\\lang\\grid.cpp", 118, 0);
		v0 = NULL;
	}

	cdkey_encrypted = v0;
	_snprintf(buff, MAX_PATH, "%s\\%s.clh", "font", "font");
	void* v1 = fastFileRead_(&cdkeyowner_encrypted_len, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (v1 == NULL)
	{
		v1 = NULL;
	}
	else if (!cdkeyowner_encrypted_len)
	{
		SMemFree(v1, "Starcraft\\SWAR\\lang\\grid.cpp", 118, 0);
		v1 = NULL;
	}
	cdkeyowner_encrypted = v1;
	_snprintf(buff, MAX_PATH, "%s\\%s.ccd", "font", "font");
	is_spawn = KeyVerification(buff, "sgubon") == 0;
}

FAIL_STUB_PATCH(InitializeFontKey);

void LoadMainModuleStringInfo_()
{
	int result = GetModuleFileNameA(hInst, tstrFilename, MAX_PATH);
	if (result)
	{
		DWORD dwHandle;
		int v1 = GetFileVersionInfoSizeA(tstrFilename, &dwHandle);
		if (v1)
		{
			void* v2 = SMemAlloc(v1, "Starcraft\\SWAR\\lang\\init.cpp", 345, 0);
			if (GetFileVersionInfoA(tstrFilename, 0, v1, (LPVOID)v2))
			{
				VS_FIXEDFILEINFO* lpBuffer;
				unsigned int puLen;
				if (VerQueryValueA(v2, "\\", (LPVOID*)&lpBuffer, &puLen))
					_snprintf(
						aInternalVersio,
						MAX_PATH,
						"Version %d.%d.%d",
						HIWORD(lpBuffer->dwProductVersionMS),
						LOWORD(lpBuffer->dwProductVersionMS),
						HIWORD(lpBuffer->dwProductVersionLS));
			}
			SMemFree(v2, "Starcraft\\SWAR\\lang\\init.cpp", 408, 0);
		}
	}
	else
	{
		tstrFilename[0] = 0;
	}
}

FAIL_STUB_PATCH(LoadMainModuleStringInfo);

void DetectExpansionInstallation_()
{
	HANDLE phFile;
	if (SFileOpenFileEx(0, "rez\\epilogX.txt", 0, &phFile))
	{
		SFileCloseFile(phFile);
		is_expansion_installed = 1;
	}
	else
	{
		is_expansion_installed = 0;
	}
}

FAIL_STUB_PATCH(DetectExpansionInstallation);

HANDLE magnetar_mpq;

int InitializeArchiveHandles_()
{
	snet_version_data.size = sizeof(snet_version_data);
	snet_version_data.versionstring = aInternalVersio;
	snet_version_data.executablefile = tstrFilename;
	snet_version_data.originalarchivefile = archive_files;
	snet_version_data.patcharchivefile = patch_archive_file;
	LoadMainModuleStringInfo_();

	CHAR starcraft_exe_filename[MAX_PATH];
	if (!GetModuleFileNameA(hInst, starcraft_exe_filename, MAX_PATH))
	{
		starcraft_exe_filename[0] = 0;
	}
	std::filesystem::path starcraft_exe_path = starcraft_exe_filename;
	std::filesystem::path starcraft_path = starcraft_exe_path.parent_path();
	std::filesystem::path stardat_path = starcraft_path / "Stardat.mpq";
	std::filesystem::path broodat_path = starcraft_path / "Broodat.mpq";
	std::filesystem::path patch_rt_path = starcraft_path / "patch_rt.mpq";

	if (!SFileOpenArchive(stardat_path.generic_string().c_str(), 2000u, 2u, &stardat_mpq))
	{
		SysWarn_FileNotFound("Stardat.mpq", GetLastError());
	}

	if (SFileOpenArchive(patch_rt_path.generic_string().c_str(), 7000u, 2u, &patch_rt_mpq))
	{
		SStrCopy(patch_archive_file, patch_rt_path.generic_string().c_str(), sizeof(patch_archive_file));
	}
	else
	{
		patch_rt_mpq = 0;
	}

	char magnetarDatFilename[MAX_PATH] = { 0 };
	if (!GetModuleFileNameA(NULL, magnetarDatFilename, MAX_PATH))
		*magnetarDatFilename = 0;
	auto separator = strrchr(magnetarDatFilename, '\\');
	if (separator)
		*separator = 0;
	SStrNCat(magnetarDatFilename, "\\MagnetarDat.mpq", MAX_PATH);
	if (!SFileOpenArchive(magnetarDatFilename, 8000u, 2u, &magnetar_mpq))
	{
		SysWarn_FileNotFound("MagnetarDat.mpq", GetLastError());
	}

	InitializeFontKey_();
	AppAddExit_(DestroyFontKey);
	if (!is_spawn)
		InitializeCDArchives_(0, 1);

	archive_files[0] = 0;
	if (!is_spawn)
	{
		if (SFileOpenArchive(broodat_path.generic_string().c_str(), 2500u, 2u, &broodat_mpq))
		{
			SStrCopy(archive_files, broodat_path.generic_string().c_str(), sizeof(archive_files));
			SStrNCat(archive_files, ";", sizeof(archive_files));
		}
		else
		{
			broodat_mpq = 0;
		}
	}

	DetectExpansionInstallation_();
	return SStrNCat(archive_files, stardat_path.generic_string().c_str(), sizeof(archive_files));
}

FAIL_STUB_PATCH(InitializeArchiveHandles);

signed int __stdcall FileIOErrProc_(char *source, int a2, unsigned int a3)
{
	if (!a3)
		return 1;
	if (a3 <= 3)
	{
		Sleep(0xAu);
		return 1;
	}
	if (!byte_596910[0])
	{
		EnterCriticalSection(&stru_6D5EDC);
		SStrCopy(byte_596910, source, MAX_PATH);
		LeaveCriticalSection(&stru_6D5EDC);
	}
	return 0;
}

void * loadTBL_(int a1, int a2, const char *a3, const char *filename, char **a5)
{
	static char* empty_tbl_string = { 0 };

	void *result;
	char** v6 = a5;

	for (result = _fastFileRead(filename, 0, 0, a3, a1); a2; --a2)
	{
		if (*v6)
		{
			unsigned __int16 v9 = *(_WORD *)v6 - 1;
			if (*(_WORD *)v6 == 0)
			{
				*v6 = 0;
			}
			else if (v9 < *(_WORD*)result)
			{
				*v6 = (char*)result + *((_WORD*)result + v9 + 1);
			}
			else
			{
				*v6 = empty_tbl_string;
			}
		}
		++v6;
	}
	return result;
}

void LoadGameData_(DatLoad* a1, const char* a2)
{
	unsigned int offset;

	int bytes_read = 0;
	BYTE* v3 = (BYTE*) fastFileRead_(&bytes_read, 0, a2, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 356);
	const void* v5 = (const void*)v3;
	for (BYTE* i = v3; a1->address; i += offset)
	{
		offset = a1->length * a1->entries;
		if (offset > bytes_read)
		{
			break;
		}
		memcpy(a1->address, v5, offset);
		++a1;
		bytes_read -= offset;
		v5 = (const void*)(offset + i);
	}
	if (bytes_read)
	{
		SysWarn_FileNotFound(a2, 24);
	}
	SMemFree(v3, "Starcraft\\SWAR\\lang\\gamedata.cpp", 402, 0);
}

DatLoad sfxdataDat_[] = {
	DatLoad { SFXData_SoundFile, 4, 1144 },
	DatLoad { SFXData_Flags1, 1, 1144 },
	DatLoad { SFXData_Flags2, 1, 1144 },
	DatLoad { SFXData_Race, 2, 1144 },
	DatLoad { SFXData_MuteVolume, 1, 1144 },
};

void LoadSfx_()
{
	// TODO: dynamically allocate sfxdataDat memory
	LoadGameData_(sfxdataDat_, "arr\\sfxdata.dat");
	dword_5999B0 = loadTBL_(1711, 1144, "Starcraft\\SWAR\\lang\\snd.cpp", "arr\\sfxdata.tbl", SFXData_SoundFile);
}

FAIL_STUB_PATCH(LoadSfx);

char* MapdataFilenames_[73];

MEMORY_PATCH(0x4280A2, MapdataFilenames_);
MEMORY_PATCH(0x4A7DC9, MapdataFilenames_);
MEMORY_PATCH(0x512BA0, MapdataFilenames_);
MEMORY_PATCH(0x512BA8, _countof(MapdataFilenames_));

void playRadioFreeZerg_()
{
	MusicTrack v0;
	const char* v1;

	if (current_music == MT_RADIO_FREE_ZERG)
	{
		v0 = MT_ZERG1;
		v1 = GetNetworkTblString(66);
	}
	else
	{
		v0 = MT_RADIO_FREE_ZERG;
		v1 = GetNetworkTblString(65);
	}

	if (v1)
	{
		setUnitStatTxtErrorMsg((char*) v1);
	}
	PlayMusic_(v0);
}

FAIL_STUB_PATCH(playRadioFreeZerg);

DEFINE_ENUM_FLAG_OPERATORS(CheatFlags);

bool radioFreeZergCheat_(const void* a2, CheatFlags* cheat_flags)
{
	if (gwGameMode == GAME_RUN && gameData.got_file_values.cheats != 1)
	{
		return false;
	}

	if (!memcmp(a2, &cheat_hash_no_glues, 8))
	{
		*cheat_flags ^= CHEAT_NoGlues;
		return true;
	}

	if (!memcmp(a2, &cheat_hash_radio_free_zerg, 8))
	{
		if (gwGameMode == GAME_RUN && is_expansion_installed && consoleIndex == Race::RACE_Zerg)
		{
			playRadioFreeZerg_();
		}
		return true;
	}

	return false;
}

FAIL_STUB_PATCH(radioFreeZergCheat);

BOOL CommandLineCheatCompare_(CheatFlags* game_cheats, const char* a2)
{
	CheatHashMaybe v7;
	CheatHashMaybe a2a;

	if (multiPlayerMode)
	{
		return 0;
	}

	srand(0x75u);
	for (int i = 0; i < 4; ++i)
	{
		v7.parts[i] = rand();
	}
	srand(time(0));
	a2a = v7;
	sub_44E420((CheatHashMaybe*)stru_68F580, &a2a);
	memset(a2a.parts, 0, sizeof(a2a.parts));
	makeCheatHash(a2, (__int64*)&a2a.parts[2]);
	memset(stru_68F580, 0, sizeof(stru_68F580));

	if (verifyCheatCode(&a2a.parts[2], game_cheats) || radioFreeZergCheat_(&a2a.parts[2], game_cheats))
	{
		return 1;
	}
	else
	{
		return campaignTypeCheatStrings(a2) != 0;
	}
}

FAIL_STUB_PATCH(CommandLineCheatCompare);

void CommandLineCheck_()
{
	const char* command_line = GetCommandLineA();
	if (command_line)
	{
		strTokenize(command_line);
		CheatFlags game_cheats = GameCheats;
		for (char* argument = strTokenize(0); argument; argument = strTokenize(0))
		{
			if (CommandLineCheatCompare_(&game_cheats, argument))
			{
				cheatActivation(game_cheats, 0);
			}
			else
			{
				size_t argument_length = strlen(argument);
				if (!_strnicmp(argument, "nosound", argument_length))
				{
					byte_6D11D0 = 1;
				}
				else if (!_strnicmp(argument, "ddemulate", argument_length))
				{
					byte_6D5DFC = 1;
				}
			}
		}
	}
}

FAIL_STUB_PATCH(CommandLineCheck);

void __fastcall FreeNetworkTBLHandle_(bool exit_code)
{
	if (networkTable)
	{
		SMemFree(networkTable, "Starcraft\\SWAR\\lang\\error.cpp", 21, 0);
	}
	networkTable = NULL;
}

FAIL_STUB_PATCH(FreeNetworkTBLHandle);

void LoadNetworkTBL_()
{
	AppAddExit_(FreeNetworkTBLHandle_);
	networkTable = (WORD*) fastFileRead_(0, 0, "rez\\network.tbl", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
}

FAIL_STUB_PATCH(LoadNetworkTBL);

HACCEL InitLocaleAccelerators_(int a1)
{
	HACCEL result = 0;

	if (local_dll_library != NULL)
	{
		result = LoadAcceleratorsA(local_dll_library, MAKEINTRESOURCEA(a1));
	}
	if (result == 0)
	{
		result = LoadAcceleratorsA(hInst, MAKEINTRESOURCEA(a1));
	}
	return result;
}

FAIL_STUB_PATCH(InitLocaleAccelerators);

void InitAccelerators_()
{
	hAccel = 0;
	HACCEL v1 = InitLocaleAccelerators_(0x65);
	HACCEL v2 = InitLocaleAccelerators_(0x71);
	if (v1)
	{
		if (v2)
		{
			ACCEL AccelDst[256];

			int v3 = CopyAcceleratorTableA(v1, 0, 0);
			int cAccelEntries = CopyAcceleratorTableA(v2, 0, 0);
			int cAccel = v3 + cAccelEntries;
			if (cAccel <= 0x100 && v3 == CopyAcceleratorTableA(v1, AccelDst, v3))
			{
				if (cAccelEntries == CopyAcceleratorTableA(v2, &AccelDst[v3], cAccelEntries))
				{
					hAccel = CreateAcceleratorTableA(AccelDst, cAccel);
				}
			}
		}
		DestroyAcceleratorTable(v1);
	}
	if (v2)
	{
		DestroyAcceleratorTable(v2);
	}
	if (!hAccel)
	{
		hAccel = InitLocaleAccelerators_(0x65);
	}
	dword_5968F4 = InitLocaleAccelerators_(0x66);
	hAccTable = InitLocaleAccelerators_(0x67);
	dword_5968F8 = hAccTable;
	DlgAccelerator = 0;
	AcceleratorTables = 0;
	input_procedures[EventNo::EVN_SYSCHAR] = input_standardSysHotkeys;
}

FAIL_STUB_PATCH(InitAccelerators);

GotFileValues* readTemplate_(const char* template_name, char* got_template_name, char* got_template_label)
{
	char buff[MAX_PATH];
	int got_file_size;

	_snprintf(buff, sizeof(buff), "%s%s%s", "Templates\\", template_name, ".got");
	GotFile* got_file_data = (GotFile*)fastFileRead_(&got_file_size, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (!got_file_data)
	{
		return 0;
	}
	if (got_file_size != 97 || got_file_data->version != 3)
	{
		SMemFree(got_file_data, "Starcraft\\SWAR\\lang\\gametype.cpp", 97, 0);
		return 0;
	}
	if (got_file_data->values.template_id >= 129u || got_file_data->values.variation_id >= 8u)
	{
		SMemFree(got_file_data, "Starcraft\\SWAR\\lang\\gametype.cpp", 98, 0);
		return 0;
	}
	memcpy(got_template_name, got_file_data->name, 32u);
	memcpy(got_template_label, got_file_data->label, 32u);
	GotFileValues* result = (GotFileValues*)SMemAlloc(sizeof(GotFileValues), "Starcraft\\SWAR\\lang\\gametype.cpp", 74, 0);
	memcpy(result, &got_file_data->values, sizeof(GotFileValues));
	SMemFree(got_file_data, "Starcraft\\SWAR\\lang\\gametype.cpp", 78, 0);
	return result;
}

FAIL_STUB_PATCH(readTemplate);

int __stdcall LoadGameTemplates_(TemplateConstructor template_constructor)
{
	char buff[MAX_PATH];
	char v7[MAX_PATH];
	int v10;

	_snprintf(buff, sizeof(buff), "%s%s", "Templates\\", "templates.lst");
	BYTE* v1 = (BYTE*)fastFileRead(&v10, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	if (v1 == nullptr)
	{
		FatalError("Unable to read game templates.");
	}
	BYTE* v11 = v1;
	while (sub_4AAE20(v7, (unsigned int*)&v10, (_BYTE**) &v11, sizeof(v7)))
	{
		char* v2 = strrchr(v7, '.');
		if (v2)
		{
			*v2 = 0;
		}

		char got_template_name[32];
		char got_template_label[32];
		GotFileValues* got_template_values = readTemplate_(v7, got_template_name, got_template_label);
		if (got_template_values)
		{
			template_constructor(got_template_name, got_template_label, got_template_values, 0);
			SMemFree(got_template_values, "Starcraft\\SWAR\\lang\\gametype.cpp", 231, 0);
		}
	}
	if (v1)
	{
		SMemFree(v1, "Starcraft\\SWAR\\lang\\gametype.cpp", 233, 0);
	}
	return 1;
}

FAIL_STUB_PATCH(LoadGameTemplates);
FAIL_STUB_PATCH(sub_4AB970);

void InitializeImage_()
{
	memset(ScreenLayers, 0, sizeof(layer) * 8);
	memset(RefreshRegions, 0, sizeof(RefreshRegions));
	GameScreenBuffer.wid = SCREEN_WIDTH;
	GameScreenBuffer.ht = SCREEN_HEIGHT;
	GameScreenBuffer.data = 0;
	stru_6D5DF0.data = (u8*)SMemAlloc(2304, "Starcraft\\SWAR\\lang\\gds\\image.cpp", 141, 0);
}

FAIL_STUB_PATCH(InitializeImage);

void InitializeDialogScreenLayer_()
{
	AppAddExit_(DestroyScreenLayer);
	ScreenLayers[2].left = 0;
	ScreenLayers[2].top = 0;
	ScreenLayers[2].width = SCREEN_WIDTH;
	ScreenLayers[2].height = SCREEN_HEIGHT;
	ScreenLayers[2].pSurface = 0;
	ScreenLayers[2].pUpdate = DialogUpdateProc;
	ScreenLayers[2].buffers = 1;
	ScreenLayers[2].bits = 32;
}

FAIL_STUB_PATCH(InitializeDialogScreenLayer);

void setCursor_(grpHead* cursor)
{
	if (last_cursor != cursor)
	{
		last_cursor = cursor;
		drawCursor_();
	}
}

FAIL_STUB_PATCH(setCursor);

void setCursorType_(CursorType cursor_type)
{
	if (last_cursor_type != cursor_type)
	{
		last_cursor_type = cursor_type;
		setCursor_(cursor_graphics[cursor_type]);
	}
}

FAIL_STUB_PATCH(setCursorType);

void __fastcall DestroyCursors_(bool exit_code)
{
	ScreenLayers[0].buffers = 0;
	last_cursor = NULL;

	for (int i = 0; i < _countof(cursor_graphics); i++)
	{
		if (cursor_graphics[i])
		{
			SMemFree(cursor_graphics[i], "Starcraft\\SWAR\\lang\\cur.cpp", 205, 0);
			cursor_graphics[i] = 0;
		}
	}
}

FAIL_STUB_PATCH(DestroyCursors);

CursorType operator++(CursorType& cursor_type)
{
	return CursorType(++reinterpret_cast<int&>(cursor_type));
}

void LoadCursors_()
{
	AppAddExit_(DestroyCursors_);
	for (CursorType i = CursorType::CUR_ARROW; i < CursorType::CUR_MAX; ++i)
	{
		char dest[260];
		SStrCopy(dest, "cursor\\", 0x104u);
		SStrNCat(dest, cursor_filenames[i], 260);
		grpHead* v1 = LoadGraphic(dest, 0, "Starcraft\\SWAR\\lang\\cur.cpp", 212);

		for (int frame_index = (v1->wFrames & 0x7FFF) - 1; frame_index >= 0; frame_index--)
		{
			grpFrame* frame = &v1->frames[frame_index];
			frame->dataOffset = (u32)v1 + (frame->dataOffset & 0x7FFFFFFF);
		}
		cursor_graphics[i] = v1;
	}
	setCursorType_(CUR_ARROW);
	ScreenLayers[0].buffers = 0;
	ScreenLayers[0].pUpdate = cursorUpdateProc;
	dword_597398 = GetTickCount();
}

FAIL_STUB_PATCH(LoadCursors);

void __fastcall FreeMapdataTable_(bool exit_code)
{
	if (dword_51CC30)
	{
		SMemFree(dword_51CC30, "Starcraft\\SWAR\\lang\\init.cpp", 1302, 0);
		dword_51CC30 = NULL;
	}

	// TODO: dynamically clean mapdataDat memory
	// TODO: dynamically clean upgradesDat memory
	// TODO: dynamically clean weaponsDat memory
	// TODO: dynamically clean techdataDat memory
	// TODO: dynamically clean portdataDat memory
}

FAIL_STUB_PATCH(FreeMapdataTable);

void PreInitData_()
{
	SFileSetIoErrorMode(1, FileIOErrProc_);
	AppAddExit_(leaveOnQuit);
	if (signal(2, 1) != -1)
	{
		signal(21, 1);
	}
	CommandLineCheck_();
	AppAddExit_(CloseAllArchives);
	InitializeArchiveHandles_();
	DataVersionCheck("rez\\DataVersion.txt");
	if (cd_archive_mpq)
	{
		DataVersionCheck("rez\\CDversion.txt");
	}
	registry_options.field_18 |= 7;
	LoadRegOptions();
	AppAddExit_(saveRegOptions);
	LoadNetworkTBL_();
	InitAccelerators_();
	AppAddExit_(DestroyAccelerators);
	LoadMenuFonts();
	AppAddExit_(DestroyFonts);
	InitializeImage_();
	AppAddExit_(DestroyImage);
	LoadCursors_();
	InitializeDialogScreenLayer_();
	dword_6D5E20 = &GameScreenBuffer;
	CreateHelpContext();
	AppAddExit_(DestroyHelpContext);
	LoadGameData_(mapdataDat, "arr\\mapdata.dat"); // TODO: is this call needed?
	dword_51CC30 = loadTBL_(1577, _countof(MapdataFilenames_), "Starcraft\\SWAR\\lang\\init.cpp", "arr\\mapdata.tbl", MapdataFilenames_);
	AppAddExit_(FreeMapdataTable_);
	LoadGameTemplates_(Template_Constructor);
	AppAddExit_(DestroyGameTemplates);
}

FAIL_STUB_PATCH(PreInitData);

char *GetErrorString_(LPSTR lpBuffer, DWORD a2, unsigned int a3)
{
	switch ((a3 >> 16) & 0x1FFF)
	{
	case 0x878u:
		DSERR_GetString(a2, lpBuffer);
		break;
	case 0x876u:
		DDERR_GetString(a2, lpBuffer);
		break;
	case 0x617u:
		_snprintf(lpBuffer, a2, "MMSYS error 0x%x", a3);
		break;
	default:
		if (!SErrGetErrorStr(a3, lpBuffer, a2) && !FormatMessageA(0x1000u, 0, a3, 0x400u, lpBuffer, a2, 0))
			_snprintf(lpBuffer, a2, "unknown error 0x%08x", a3);
		break;
	}
	unsigned v6 = strlen(lpBuffer);
	for (char* i = &lpBuffer[v6 - 1]; (signed int)v6 > 0; *i = 0)
	{
		--v6;
		--i;
		char v8 = *i;
		if (v8 != '\r' && v8 != '\n')
			break;
	}
	return lpBuffer;
}

void ErrorDDrawInit_(const char *source_file, const char *function_name, unsigned int last_error, WORD resource, int source_line)
{
	char dwInitParam[512];
	CHAR Buffer[256];

	const char* v5 = source_file;
	const char* v6 = function_name;
	const char* v7 = strrchr(source_file, '\\');
	if (v7)
		v5 = v7 + 1;
	if (!v6)
		v6 = "";
	char* v8 = GetErrorString_(Buffer, sizeof(Buffer), last_error);
	_snprintf(dwInitParam, 512u, "%s\n%s line %d\n%s", v8, v5, source_line, v6);
	SErrSuppressErrors(1);
	SNetLeaveGame(3);
	SNetDestroy();
	if (GetCurrentThreadId() == main_thread_id)
	{
		BWFXN_DDrawDestroy();
		BWFXN_DSoundDestroy();
	}
	if (DialogBoxParamA(local_dll_library, (LPCSTR)resource, hWndParent, DialogFunc, (LPARAM)dwInitParam) == -1)
		FatalError("GdsDialogBoxParam: %d", resource);
	DLGErrFatal_();
}

FAIL_STUB_PATCH(ErrorDDrawInit);

BOOL BWFXN_DDrawInitialize_()
{
	PALETTEENTRY palette_entries[256];
	DDSURFACEDESC surface_desc;

	ShowWindow(hWndParent, 1);
	GUID* v4 = 0;
	if (byte_6D5DFC)
		v4 = (GUID *) 2;
	HRESULT v5 = DirectDrawCreate(v4, &DDInterface, 0);
	if (v5 == DDERR_NODIRECTDRAWSUPPORT)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "DirectDrawCreate", 0x887600DE, 0x6Eu, 124);
	if (v5)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "DirectDrawCreate", v5, 0x66u, 125);
	HRESULT v6 = DDInterface->SetCooperativeLevel(hWndParent, 16 | DSSCL_PRIORITY | DSSCL_NORMAL);
	if (v6 != DDERR_EXCLUSIVEMODEALREADYSET && v6 != NULL)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "SetCooperativeLevel", v6, 0x66u, 148);
#ifndef BYPASS_DDRAW_STUFF
	if (DDInterface->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, 8))
	{
		int v7 = GetSystemMetrics(SM_CXSCREEN);
		int v8 = GetSystemMetrics(SM_CYSCREEN);
		unsigned v9 = DDInterface->SetDisplayMode(v7, v8, 8);
		if (v9)
			ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "SetDisplayMode", v9, 0x67u, 160);
	}
#endif
	memset(palette_entries, 0, sizeof(palette_entries));
	unsigned v10 = DDInterface->CreatePalette(DDPCAPS_ALLOW256 | DDPCAPS_8BIT, palette_entries, &PrimaryPalette, 0);
	if (v10)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "CreatePalette", v10, 0x66u, 182);
	memset(&surface_desc, 0, sizeof(surface_desc));
	surface_desc.dwSize = sizeof(DDSURFACEDESC);
	surface_desc.dwFlags = 1;
	surface_desc.ddsCaps.dwCaps = 512;
	unsigned v11 = DDInterface->CreateSurface(&surface_desc, &PrimarySurface, 0);
	if (v11)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "CreatePrimarySurface", v11, 0x66u, 193);
#ifndef BYPASS_DDRAW_STUFF
	unsigned v12 = PrimarySurface->SetPalette(PrimaryPalette);
	if (v12)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "SetPalette", v12, 0x66u, 203);
#endif
	surface_desc.dwSize = sizeof(DDSURFACEDESC);
	if (PrimarySurface->Lock(0, &surface_desc, 1, 0))
	{
		memset(&surface_desc, 0, sizeof(surface_desc));
		surface_desc.dwSize = sizeof(DDSURFACEDESC);
		surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		surface_desc.ddsCaps.dwCaps = 2112;
		surface_desc.dwWidth = SCREEN_WIDTH;
		surface_desc.dwHeight = SCREEN_HEIGHT;
		unsigned v13 = DDInterface->CreateSurface(&surface_desc, &BackSurface, 0);
		if (v13)
			ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "CreateBackSurface", v13, 0x66u, 220);
	}
	else
	{
		PrimarySurface->Unlock(surface_desc.lpSurface);
	}
	return SDrawManualInitialize(hWndParent, DDInterface, PrimarySurface, 0, 0, BackSurface, PrimaryPalette, 0);
}

FAIL_STUB_PATCH(sub_41DDD0);
FAIL_STUB_PATCH(BWFXN_DDrawInitialize);

void BWFXN_updateImageData_()
{
	for (int i = 0; i < map_size.height; ++i)
	{
		for (CSprite* sprites = SpritesOnTileRow.heads[i]; sprites; sprites = sprites->next)
		{
			for (CImage* image = sprites->pImageHead; image; image = CImage__updateGraphicData(image)->next);
		}
	}
}

FAIL_STUB_PATCH(BWFXN_updateImageData);

void drawMinitileImageData_(int framebuf_pos, int minitile)
{
	char* v4 = (char*)VR4Data + 32 * (minitile & 0xFFFE);
	for (int i = 0; i < 8; i++)
	{
		if (framebuf_pos >= TILE_CACHE_SIZE)
		{
			framebuf_pos -= TILE_CACHE_SIZE;
		}
		for (int j = 0; j < 8; j++)
		{
			GameTerrainCache[framebuf_pos + j] = (minitile & 1) ? v4[8 * i + 7 - j] : v4[8 * i + j];
		}
		framebuf_pos += RENDER_AREA_WIDTH;
	}
}

FAIL_STUB_PATCH(drawMinitileImageData);

void drawMegatileImageData_(int megatile, int framebuf_position, int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			drawMinitileImageData_(framebuf_position + 8 * RENDER_AREA_WIDTH * i + 8 * j, VX4Data[megatile & 0x7FFF].wImageRef[i][j]);
		}
	}

	if (megatile & 0x8000)
	{
		u8 v20 = CreepEdgeData[y * dword_6D0F08 + x];
		if (v20)
		{
			renderTerrainGRPToCache((grpFrame*)((char*)TerrainGraphics + 8 * v20 - 2), framebuf_position);
		}
	}
}

void __stdcall drawMegatileImageData__(int x, int y)
{
	int megatile;
	int framebuf_position;

	__asm
	{
		mov megatile, ebx
		mov framebuf_position, edi
	}

	drawMegatileImageData_(megatile, framebuf_position, x, y);
}

FUNCTION_PATCH((void*)0x49B9F0, drawMegatileImageData__);

DEFINE_ENUM_FLAG_OPERATORS(MegatileFlags);

void drawScreenRowTiles_(__int16* a1, TileID* a2, MegatileFlags* a3, int a4, int a5)
{
	if (a4 < map_size.height)
	{
		int v8 = 0;
		int v6 = MoveToTile.x;

		for (int i = 0; i < min(map_size.width - MoveToTile.x, RENDER_AREA_TILE_COLUMNS); i++)
		{
			if (InReplay ? (ReplayVision & ~a3[i]) : (playerVisions & a3[i]) == 0)
			{
				__int16 megatile_ref = TileSetMap[a2[i].group].megaTileRef[a2[i].tile];
				if (megatile_ref != (a1[i] & 0x7FFF))
				{
					a1[i] = megatile_ref;
					if (a2[i].group == 1)
					{
						a3[i] |= LOCAL_CREEP;
					}
					else
					{
						a3[i] &= ~LOCAL_CREEP;
					}
					iterateDirectionalCreepData(BWFXN_CreepManagementCB, 0, v6 + i, a4);
					drawMegatileImageData_(a1[i], (a5 + i * TILE_HEIGHT) % (9408 * TILE_HEIGHT), v6 + i, a4);
					v8 = 1;
				}
			}
		}

		if (v8)
		{
			HasMegatileUpdate = 1;
		}
	}
}

FAIL_STUB_PATCH(drawScreenRowTiles);

void BWFXN_drawMapTiles_()
{
	int v2 = MoveToTile.y;
	if (v2 >= 16 * RENDER_AREA_TILE_ROWS)
	{
		v2 -= 16 * RENDER_AREA_TILE_ROWS;
	}
	if (v2 >= 8 * RENDER_AREA_TILE_ROWS)
	{
		v2 -= 8 * RENDER_AREA_TILE_ROWS;
	}
	if (v2 >= 4 * RENDER_AREA_TILE_ROWS)
	{
		v2 -= 4 * RENDER_AREA_TILE_ROWS;
	}
	if (v2 >= 2 * RENDER_AREA_TILE_ROWS)
	{
		v2 -= 2 * RENDER_AREA_TILE_ROWS;
	}
	if (v2 >= 1 * RENDER_AREA_TILE_ROWS)
	{
		v2 -= 1 * RENDER_AREA_TILE_ROWS;
	}
	for (int i = 0; i < RENDER_AREA_TILE_ROWS; i++)
	{
		drawScreenRowTiles_(
			&CellMap[MoveToTile.x + (MoveToTile.y + i) * map_size.width],
			&MapTileArray[MoveToTile.x + (MoveToTile.y + i) * map_size.width],
			&active_tiles[MoveToTile.x + (MoveToTile.y + i) * map_size.width],
			MoveToTile.y + i,
			(MoveToTile.x + ((v2 + i) % RENDER_AREA_TILE_ROWS) * RENDER_AREA_TILE_COLUMNS * TILE_WIDTH) * TILE_HEIGHT);
	}
}

FAIL_STUB_PATCH(BWFXN_drawMapTiles);

void BlitToBitmap_(int a1, int a2, byte* a3)
{
	unsigned int v7;
	unsigned int v4;
	byte* a4 = &GameTerrainCache[a1];

	for (int i = 0; i < a2; i++)
	{
		v4 = dword_50CEF0;
		if (dword_50CEF0 + a1 >= TILE_CACHE_SIZE)
		{
			if (a1 < TILE_CACHE_SIZE)
			{
				v7 = (unsigned int)(TILE_CACHE_SIZE - a1);
				memcpy(a3, a4, v7);
				a3 += v7;
				a4 += v7;
				v4 = dword_50CEF0 - (TILE_CACHE_SIZE - a1);
			}
			a1 -= TILE_CACHE_SIZE;
			a4 -= TILE_CACHE_SIZE;
		}
		memcpy(a3, a4, v4);
		a1 += RENDER_AREA_WIDTH;
		a3 += v4 - dword_50CEF0 + SCREEN_WIDTH;
		a4 += v4 - dword_50CEF0 + RENDER_AREA_WIDTH;
	}
}

FAIL_STUB_PATCH(BlitToBitmap);

void BWFXN_blitMapTiles_()
{
	dword_50CEF0 = GAME_AREA_WIDTH;
	BlitToBitmap_((MoveToX + RENDER_AREA_WIDTH * MoveToY) % TILE_CACHE_SIZE, GAME_AREA_HEIGHT, GameScreenBuffer.data);
}

FAIL_STUB_PATCH(BWFXN_blitMapTiles);

void __fastcall BlitTerrainCacheToGameBitmap_(int a0, int a1, int a2, int a3)
{
	dword_50CEF0 = 16 * a2;
	BlitToBitmap_(a3, 16, &GameScreenBuffer.data[16 * a1 + 16 * a0 * GameScreenBuffer.wid]);
}

FAIL_STUB_PATCH(BlitTerrainCacheToGameBitmap);

void blitTileCacheOnRefresh_()
{
	u8* v0 = RefreshRegions;
	int v1 = (MoveToX + RENDER_AREA_WIDTH * MoveToY) % TILE_CACHE_SIZE;

	for (int v6 = 0; v6 < GAME_AREA_HEIGHT / 16; v6++)
	{
		for (int i = 0; i < GAME_AREA_WIDTH / 16; ++i)
		{
			if (v1 >= TILE_CACHE_SIZE)
			{
				v1 -= TILE_CACHE_SIZE;
			}
			if (*v0 == 1)
			{
				int v3 = i + 1;
				int v4 = 1;
				if (i + 1 < GAME_AREA_WIDTH / 16)
				{
					do
					{
						if (*v0 == 0)
						{
							break;
						}
						++v0;
						++v4;
						++v3;
					} while (v3 < GAME_AREA_WIDTH / 16);
				}
				BlitTerrainCacheToGameBitmap_(v6, i, v4, v1);
				v1 = v1 + 16 * (v4 - 1);
				i = i + v4 - 1;
				if (v1 >= TILE_CACHE_SIZE)
				{
					v1 -= TILE_CACHE_SIZE;
				}
			}
			v1 += 16;
			++v0;
		}
		v1 += 632 * 16;
		if (v1 >= TILE_CACHE_SIZE)
		{
			v1 -= TILE_CACHE_SIZE;
		}
	}
}

FAIL_STUB_PATCH(blitTileCacheOnRefresh);

void drawImage_(CImage* a1)
{
	if ((a1->flags & 0x40) == 0 && a1->grpBounds.bottom > 0 && a1->grpBounds.right > 0 && ((a1->flags & 1) != 0 || isImageRefreshable(a1)))
	{
		RECT v8;
		v8.left = a1->grpBounds.left;
		v8.right = a1->grpBounds.right;
		v8.top = a1->grpBounds.top;
		v8.bottom = a1->grpBounds.bottom;

		a1->renderFunction(a1->screenPosition.x, a1->screenPosition.y, &a1->GRPFile->frames[a1->frameIndex], (rect*)&v8, (int)a1->coloringData);
	}
	a1->flags &= ~1;
}

FAIL_STUB_PATCH(drawImage);

void drawSprite_(CSprite* a1)
{
	dword_6D5BD4 = getColourID(a1->playerID);
	memcpy(byte_50CDC1 + 8, stru_581D76[dword_6D5BD4].colors, sizeof(stru_581D76[dword_6D5BD4].colors));

	if (a1->pImagePrimary)
	{
		unknownColorShiftSomething(a1->pImagePrimary->paletteType, a1->playerID);
	}
	for (CImage* image = a1->pImageTail; image; image = image->prev)
	{
		drawImage_(image);
	}
}

FAIL_STUB_PATCH(drawSprite);

void BWFXN_drawAllSprites_()
{
	for (CSprite* sprite = getFirstSprite(); sprite; sprite = dword_6C2318[dword_6C4A28])
	{
		drawSprite_(sprite);
		if (dword_6C4A28 == 0)
		{
			break;
		}
		--dword_6C4A28;
	}
}

FAIL_STUB_PATCH(BWFXN_drawAllSprites);

void updateAllFog_(int a1)
{
	if (a1)
	{
		updateFog(GAME_AREA_HEIGHT, 0, 0, GAME_AREA_WIDTH);
	}
	else
	{
		int region_index = 0;
		for (int i = 0; i < GAME_AREA_HEIGHT; i += 16)
		{
			for (int j = 0; j < GAME_AREA_WIDTH; j += 16)
			{
				if (RefreshRegions[region_index++])
				{
					int k;
					for (k = 16; j + k < GAME_AREA_WIDTH; k += 16)
					{
						if (RefreshRegions[region_index] == 0)
						{
							break;
						}
						++region_index;
					}
					updateFog(i + 16, i, j, k + j);
					j = k + j - 16;
				}
			}
		}
	}
}

FAIL_STUB_PATCH(updateAllFog);

void BWFXN_drawDragSelBox_()
{
	if (byte_66FF5C)
	{
		if (InputFlags)
		{
			DrawColor = byte_6CEB32;
			BWFXN_Draw(stru_66FF50.top, stru_66FF50.left, stru_66FF50.right - stru_66FF50.left + 1, stru_66FF50.bottom - stru_66FF50.top + 1);
		}
		else
		{
			refreshDragSelectBox();
			SetInGameInputProcs();
		}
	}
}

FAIL_STUB_PATCH(BWFXN_drawDragSelBox);

void BWFXN_drawAllThingys_()
{
	if (wantThingyUpdate)
	{
		CThingy* currentThingy = ThingyList_UsedFirst;
		for (CImage* image = ThingyList_UsedFirst->sprite->pImageHead; image; image = CImage__updateGraphicData(image)->next);
		drawSprite_(currentThingy->sprite);
		for (CImage* image = ThingyList_UsedFirst->sprite->pImageHead; image; image = image->next)
		{
			image->flags |= 1;
		}
	}
}

FAIL_STUB_PATCH(BWFXN_drawAllThingys);

MEMORY_PATCH((void*)0x4D5857, GAME_AREA_WIDTH);
MEMORY_PATCH((void*)0x4D5888, GAME_AREA_HEIGHT);

MEMORY_PATCH((void*)0x40B168, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B28E, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B252, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B2E6, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B400, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B3CA, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40BC64, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40BC2E, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B5A9, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B454, SCREEN_WIDTH * SCREEN_HEIGHT);

void __fastcall DrawGameProc_(int _unused1, int _unused2, Bitmap* a1, bounds* a2)
{
	int v2 = ScreenLayers[5].bits & 1;
	if (ScreenLayers[5].bits & 1)
	{
		maskSomething0();
		memcpy(dword_6D5C10, dword_6D5C0C, 0x198u);
		BWFXN_updateImageData_();
		maskSomething2();
		BWFXN_drawMapTiles_();
		BWFXN_blitMapTiles_();
	}
	else
	{
		maskSomething1();
		refreshImageRange(dword_5993A4, dword_5993C0);
		maskSomething2();
		BWFXN_drawMapTiles_();
		blitTileCacheOnRefresh_();
	}
	BWFXN_drawAllSprites_();
	if (CurrentTileSet == Tileset::Platform)
	{
		if (v2)
			drawStars();
		else
			refreshStars();
	}
	updateAllFog_(v2);
	BWFXN_DrawHighTarget();
	BWFXN_drawDragSelBox_();
	BWFXN_drawAllThingys_();
}

FAIL_STUB_PATCH(DrawGameProc);

void InitializeGameLayer_()
{
	SetRect(&game_screen_pos, 0, 0, GAME_AREA_WIDTH - 1, GAME_AREA_HEIGHT - 1);
	ScreenLayers[5].left = 0;
	ScreenLayers[5].top = 0;
	ScreenLayers[5].pSurface = 0;
	ScreenLayers[5].bits = 0;
	ScreenLayers[5].width = GAME_AREA_WIDTH;
	ScreenLayers[5].height = GAME_AREA_HEIGHT;
	ScreenLayers[5].pUpdate = DrawGameProc_;
	memset(RefreshRegions, 1u, sizeof(RefreshRegions));
	for (int i = 3; i <= 5; ++i)
	{
		ScreenLayers[i].bits |= 1;
		int left = ScreenLayers[i].left;
		int top = ScreenLayers[i].top;
		int bottom = top + ScreenLayers[i].height - 1;
		int right = left + ScreenLayers[i].width - 1;
		BWFXN_RefreshTarget(left, bottom, top, right);
	}
}

FAIL_STUB_PATCH(InitializeGameLayer);

void __cdecl refreshSelectionScreen_()
{
	BWFXN_RefreshTarget(ScreenLayers[1].left, ScreenLayers[1].height + ScreenLayers[1].top - 1, ScreenLayers[1].top, ScreenLayers[1].width + ScreenLayers[1].left - 1);
	if (ScreenLayers[1].buffers)
	{
		ScreenLayers[1].bits |= 1;
		BWFXN_RefreshTarget(ScreenLayers[1].left, ScreenLayers[1].height + ScreenLayers[1].top - 1, ScreenLayers[1].top, ScreenLayers[1].width + ScreenLayers[1].left - 1);
	}
	ScreenLayers[1].left = GAME_AREA_WIDTH;
	ScreenLayers[1].top = GAME_AREA_HEIGHT;
	dword_655C48 = 0;
}

FUNCTION_PATCH(refreshSelectionScreen, refreshSelectionScreen_);

int DSoundCreate_(AudioVideoInitializationError* a1)
{
	HRESULT v4 = DirectSoundCreate(0, &direct_sound, 0);
	a1->error_code = v4;
	if (v4 == 0x8878000A)
	{
		HRESULT v5;
		do
		{
			if (dword_51A43C <= 0)
			{
				break;
			}
			Sleep(1000u);
			--dword_51A43C;
			v5 = DirectSoundCreate(0, &direct_sound, 0);
			a1->error_code = v5;
		} while (v5 == -2005401590);
	}
	if (a1->error_code)
	{
		a1->function_name = "DirectSoundCreate";
		if (a1->error_code == -2005401590)
		{
			a1->dword4 = 135;
		}
		return 0;
	}
	else
	{
		return 1;
	}
}

FAIL_STUB_PATCH(DSoundCreate);

int SetCooperativeLevel_(HWND hwnd, AudioVideoInitializationError* a2)
{
	a2->error_code = direct_sound->SetCooperativeLevel(hwnd, 2);
	if (!a2->error_code)
	{
		return 1;
	}
	a2->error_code = direct_sound->SetCooperativeLevel(hwnd, 1);
	if (!a2->error_code)
	{
		return 1;
	}
	a2->function_name = "SetCooperativeLevel";
	return 0;
}

FAIL_STUB_PATCH(SetCooperativeLevel);

int CreateSoundBuffer_(AudioVideoInitializationError* a1)
{
	DSBUFFERDESC sound_buffer;

	sound_buffer.dwSize = sizeof(DSBUFFERDESC);
	sound_buffer.dwBufferBytes = 0;
	sound_buffer.dwReserved = 0;
	sound_buffer.lpwfxFormat = 0;
	sound_buffer.dwFlags = 1;

	a1->error_code = direct_sound->CreateSoundBuffer(&sound_buffer, &soundbuffer, 0);
	if (!a1->error_code)
	{
		return 1;
	}
	a1->function_name = "CreateSoundBuffer";
	return 0;
}

FAIL_STUB_PATCH(CreateSoundBuffer);

HRESULT SetAudioFormat_()
{
	WAVEFORMATEX wave_format;
	wave_format.cbSize = 0;
	wave_format.wFormatTag = 1;
	wave_format.nChannels = 2;
	wave_format.nSamplesPerSec = 22050;
	wave_format.wBitsPerSample = 16;
	wave_format.nBlockAlign = 4;
	wave_format.nAvgBytesPerSec = 88200;

	return soundbuffer->SetFormat(&wave_format);
}

FAIL_STUB_PATCH(SetAudioFormat);

void LoadBtnSfxFile_()
{
	char buff[256];

	if (SFXData_SoundFile[SFX_Misc_Button_1])
	{
		_snprintf(buff, 0x104u, "sound\\%s", SFXData_SoundFile[15]);
		dword_6D1268 = LoadSoundProc(buff, 0);
	}
	else
	{
		dword_6D1268 = (IDirectSoundBuffer*)SFXData_SoundFile[SFX_Misc_Button_1];
	}
}

FAIL_STUB_PATCH(LoadBtnSfxFile);

void DLGMusicFade_(MusicTrack music_track)
{
	if (!directsound || !byte_6D5BBC)
	{
		if (music_tracks[music_track].fade_in_maybe)
		{
			int old_bigvolume = bigvolume;
			bigvolume = -10000;
			PlayMusic_(music_track);
			bigvolume = old_bigvolume;
			if (registry_options.Music)
			{
				dword_6D5BB8 = -3396;
				KillTimer(hWndParent, 3u);
				SetTimer(hWndParent, 3u, 0x14u, FadeMusicProc);
			}
		}
		else
		{
			PlayMusic_(music_track);
		}
	}
}

FAIL_STUB_PATCH(DLGMusicFade);

void muteBgm_(RegistryOptions* a1)
{
	if (direct_sound)
	{
		dword_6D5E3C = volume[99 * a1->Sfx / 100] - dword_6D5A0C;
		if (a1->Music)
		{
			int v2 = dword_5008EC[a1->Music] - dword_6D5A0C;
			bigvolume = v2;
			if (directsound)
			{
				if (byte_6D5BBD)
				{
					v2 -= 750;
					if (v2 < -10000)
					{
						v2 = -10000;
					}
				}
				SFileDdaSetVolume(directsound, v2, 0);
			}
			if (!byte_6D638C)
			{
				byte_6D638C = 1;
				DLGMusicFade_(current_music);
			}
		}
		else
		{
			bigvolume = -10000;
			if (directsound)
			{
				SFileDdaSetVolume(directsound, -10000, 0);
				if (directsound)
				{
					SFileDdaEnd(directsound);
					SFileCloseFile(directsound);
					directsound = 0;
				}
			}
			byte_6D5BBC = 0;
			byte_6D638C = 0;
		}
	}
}

void muteBgm__()
{
	RegistryOptions* a1;

	__asm mov a1, esi

	muteBgm_(a1);
}

FUNCTION_PATCH((void*)0x4BC320, muteBgm__);

void initVolume_()
{
	dword_5998E8 = 50;
	if (registry_options.Sfx == -1)
	{
		registry_options.Sfx = 50;
	}
	dword_5999B4 = 25;
	if (registry_options.Music == -1)
	{
		registry_options.Music = 25;
	}
	muteBgm_(&registry_options);
}

FAIL_STUB_PATCH(initVolume);

BOOL DSoundInit_(AudioVideoInitializationError* a1, HWND a2)
{
	if (direct_sound)
	{
		return 1;
	}
	a1->error_code = 0;
	a1->function_name = 0;
	a1->dword4 = 138;
	memset(stru_5998F0, 0, sizeof(stru_5998F0));
	if (!DSoundCreate_(a1) || !SetCooperativeLevel_(a2, a1) || !CreateSoundBuffer_(a1))
	{
		BWFXN_DSoundDestroy();
		return 0;
	}
	SetAudioFormat_();
	SFileDdaInitialize(direct_sound);
	byte_6D1265 = 1;
	SVidInitialize(direct_sound);
	byte_6D1266 = 1;
	LoadBtnSfxFile_();
	initVolume_();
	if (SRegLoadValue("Starcraft", "Sound Memory Cache", 0, &value))
	{
		if ((unsigned int)value < 0x100000)
		{
			value = 0x100000;
		}
	}
	else
	{
		value = 0x400000;
	}
	dword_6D59FC = 0;
	dword_6D5A00 = CreateEventA(0, 0, 0, 0);
	if (dword_6D5A00 == NULL)
	{
		BWFXN_DSoundDestroy();
		return 0;
	}
	dword_6D5A08 = 1;
	sound_thread_handle = (HANDLE) _beginthreadex(0, 0, DSoundThread, 0, 0, &ThreadId);
	if (sound_thread_handle == NULL)
	{
		BWFXN_DSoundDestroy();
		return 0;
	}
	byte_6D1264 = 1;
	return 1;
}

FAIL_STUB_PATCH(DSoundInit);

void __fastcall saveColorSettings_(bool exit_code)
{
	if (!exit_code)
	{
		SRegSaveValue("Starcraft", "Gamma", 0, Gamma);
		SRegSaveValue("Starcraft", "ColorCycle", 0, ColorCycle);
		SRegSaveValue("Starcraft", "UnitPortraits", 0, UnitPortraits);
	}
}

FAIL_STUB_PATCH(saveColorSettings);

void loadColorSettings_()
{
	AppAddExit_(saveColorSettings_);

	if (SRegLoadValue("Starcraft", "Gamma", 0, &Gamma))
	{
		if ((unsigned int)Gamma > 0x8C || (unsigned int)Gamma < 0x3C)
		{
			Gamma = 100;
		}
	}

	if (SRegLoadValue("Starcraft", "ColorCycle", 0, &ColorCycle))
	{
		ColorCycle = (ColorCycle != 0);
	}
	else
	{
		ColorCycle = 1;
	}

	if (SRegLoadValue("Starcraft", "UnitPortraits", 0, &UnitPortraits))
	{
		UnitPortraits = min(UnitPortraits, 2);
	}
	else
	{
		UnitPortraits = 2;
	}

	if (dword_68AC98)
	{
		if (ActivePortraitUnit)
		{
			UnitType v2 = getLastQueueSlotType(ActivePortraitUnit);
			WORD v3 = setBuildingSelPortrait(v2);
			displayUpdatePortrait(v3, ActivePortraitUnit, 1);
		}
	}
}

FAIL_STUB_PATCH(loadColorSettings);

void __fastcall sfxdata_cleanup_(bool exit_code)
{
	// TODO: dynamically clean sfxdataDat memory
	if (dword_5999B0)
	{
		SMemFree(dword_5999B0, "Starcraft\\SWAR\\lang\\snd.cpp", 1719, 0);
	}
	dword_5999B0 = 0;
}

FAIL_STUB_PATCH(sfxdata_cleanup);

void audioVideoInit_()
{
	loadColorSettings_();
	GameScreenBuffer.wid = SCREEN_WIDTH;
	GameScreenBuffer.ht = SCREEN_HEIGHT;
	GameScreenBuffer.data = 0;
	GameScreenBuffer.data = (u8 *)SMemAlloc(SCREEN_WIDTH * SCREEN_HEIGHT, "Starcraft\\SWAR\\lang\\gds\\vidinimo.cpp", 55, 0);
	BWFXN_DDrawInitialize_();
	dword_6D5DF8 = 1;
	AppAddExit_(vidinimoDestroy);
	memcpy(stru_6CEB40, &palette, sizeof(PALETTEENTRY[256]));
	BWFXN_RedrawTarget_();
	LoadSfx_();
	AppAddExit_(sfxdata_cleanup_);
	if (!byte_6D11D0)
	{
		AudioVideoInitializationError error;
		if (DSoundInit_(&error, hWndParent))
			AppAddExit_(j_BWFXN_DSoundDestroy);
	}
}

FAIL_STUB_PATCH(audioVideoInit);

void InitializeDialog_(dialog *a1, FnInteract a2)
{
	dlgEvent event;

	a1->lFlags |= CTRL_DLG_ACTIVE | CTRL_VALIGN_BOTTOM;
	a1->fields.dlg.pModalFcn = 0;
	if (a2)
		a1->pfcnInteract = a2;
	size_t size = a1->fields.dlg.dstBits_wid * a1->fields.dlg.dstBits_ht;
	a1->fields.dlg.dstBits_data = (u8*)SMemAlloc(size, "Starcraft\\SWAR\\lang\\dlg\\dlg.cpp", 481, 0);
	a1->wIndex = 0;

	event.cursor.x = Mouse.x;
	event.cursor.y = Mouse.y;
	event.wNo = EventNo::EVN_USER;
	event.dwUser = EventUser::USER_UNK_7;
	event.wSelection = 0;
	event.wUnk_0x06 = 0;
	a1->pfcnInteract(a1, &event);

	a1->rct.right += a1->rct.left;
	a1->rct.bottom += a1->rct.top;

	event.cursor.x = Mouse.x;
	event.cursor.y = Mouse.y;
	event.wNo = EventNo::EVN_USER;
	event.dwUser = EventUser::USER_INIT;
	event.wSelection = 0;
	event.wUnk_0x06 = 0;
	a1->pfcnInteract(a1, &event);

	event.cursor.x = Mouse.x;
	event.cursor.y = Mouse.y;
	event.wNo = EventNo::EVN_USER;
	event.dwUser = EventUser::USER_CREATE;
	event.wSelection = 0;
	event.wUnk_0x06 = 0;
	a1->pfcnInteract(a1, &event);

	SetCallbackTimer(12, a1, 100, 0);
	a1->lFlags &= ~CTRL_VALIGN_BOTTOM;
}

FAIL_STUB_PATCH(InitializeDialog);

void __fastcall BWFXN_OpenGameDialog_(char* a1, FnInteract a2)
{
	if (!multiPlayerMode)
	{
		TickCountSomething(1);
	}
	if (GameMenuDlg)
	{
		DestroyDialog(GameMenuDlg);
		GameMenuDlg = 0;
		if (gwGameMode == GAME_RUN)
		{
			hAccTable = DlgAccelerator;
			input_procedures[EventNo::EVN_SYSCHAR] = AcceleratorTables;
		}
		byte_6D1214 = 0;
	}
	sub_4195E0();
	if (byte_66FF5C)
	{
		BWFXN_RefreshTarget(stru_66FF50.left, stru_66FF50.bottom, stru_66FF50.top, stru_66FF50.right);
		byte_66FF5C = 0;
		SetInGameInputProcs();
	}
	setCursorType_(CursorType::CUR_ARROW);
	if (!multiPlayerMode)
	{
		PauseGame_maybe();
	}
	if (gwGameMode == GAME_RUN)
	{
		if (is_placing_building)
		{
			refreshLayer3And4();
			refreshPlaceBuildingLocation();
		}
		if (byte_641694)
		{
			CancelTargetOrder();
		}
	}
	byte_6D1214 = 1;
	if (gwGameMode == GAME_RUN)
	{
		DlgAccelerator = hAccTable;
		AcceleratorTables = input_procedures[EventNo::EVN_SYSCHAR];
	}
	hAccTable = dword_5968F8;
	input_procedures[EventNo::EVN_SYSCHAR] = input_standardSysHotkeys;
	refreshSelectionScreen_();

	GameMenuDlg = (dialog*)fastFileRead(0, 0, a1, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (GameMenuDlg)
	{
		GameMenuDlg->lFlags |= CTRL_ACTIVE;
		AllocInitDialogData(GameMenuDlg, GameMenuDlg, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\modeless.cpp", 63);
	}

	InitializeDialog_(GameMenuDlg, a2);
}

FUNCTION_PATCH(BWFXN_OpenGameDialog, BWFXN_OpenGameDialog_);

void TitlePaletteUpdate_(int a1)
{
	PALETTEENTRY* v1;
	PALETTEENTRY a2[256];

	if (byte_51A0E9)
	{
		memset(GamePalette, 0, sizeof(GamePalette));
		byte_51A0E9 = 0;
		v1 = GamePalette;
		if (Gamma != 100)
		{
			sub_41DC20(GamePalette, a2, 256);
			v1 = a2;
		}
		SDrawUpdatePalette(0, 0x100u, v1, 1);
		BWFXN_RedrawTarget_();
		memset(stru_6CE720, 0, sizeof(stru_6CE720));
		gluDlgFadePalette(a1);
	}
}

FAIL_STUB_PATCH(TitlePaletteUpdate);

void titleInit_(dialog* dlg)
{
	void* buffer;
	int height;
	int width;

	if (!SBmpAllocLoadImage("glue\\title\\title.pcx", (int*)palette, &buffer, &width, &height, 0, 0, allocFunction))
	{
		SysWarn_FileNotFound("glue\\title\\title.pcx", SErrGetLastError());
	}
	dlg->srcBits.wid = width;
	dlg->srcBits.ht = height;
	dlg->srcBits.data = (u8*)buffer;
	memset(&stru_6CE000, 0, sizeof(stru_6CE000));

	if (!SBmpLoadImage("glue\\title\\tFont.pcx", 0, &stru_6CE000, 192, 0, 0, 0))
	{
		SysWarn_FileNotFound("glue\\title\\tFont.pcx", SErrGetLastError());
	}
	if (!low_memory)
	{
		DLGMusicFade_(MT_TITLE);
	}

	if ((dlg->lFlags & CTRL_UPDATE) == 0)
	{
		dlg->lFlags |= CTRL_UPDATE;
		updateDialog(dlg);
	}
	RefreshCursor_0();
	memcpy(stru_6CEB40, palette, sizeof(stru_6CEB40));
	TitlePaletteUpdate_(3);
	TitleBlitAndLoop(dlg);
}

FAIL_STUB_PATCH(titleInit);

bool __fastcall TitleDlgProc_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		if (evt->dwUser == EventUser::USER_CREATE)
		{
			titleInit_(dlg);
		}
		else if (evt->dwUser == EventUser::USER_DESTROY)
		{
			DrawBINDialog(dlg);
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(TitleDlgProc);

void LoadTitle_()
{
	load_screen = LoadDialog("rez\\titledlg.bin");
	InitializeDialog_(load_screen, TitleDlgProc_);
}

FAIL_STUB_PATCH(LoadTitle);

void LoadImageData_()
{
	OVERLAPPED a5[999];
	_DWORD a6[999];

	u16* images_tbl = (u16*) fastFileRead_(0, 0, "arr\\images.tbl", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	dword_6D1210 = low_memory;
	loadImagesData_lowMem(ImageGrpGraphics, Images_GrpFile, images_tbl, 999, a5, a6);
	dword_6D1210 = 0;

	loadImagesData(&lo_files.attackOverlays, Images_AttackOverlayLO, images_tbl, 999, a5, a6);
	loadImagesData(&lo_files.damageOverlays, Images_InjuryOverlayLO, images_tbl, 999, a5, a6);
	loadImagesData(&lo_files.specialOverlays, Images_SpecialOverlayLO, images_tbl, 999, a5, a6);
	loadImagesData(&lo_files.landingDustOverlays, Images_LandingDustLO, images_tbl, 999, a5, a6);
	loadImagesData(&lo_files.liftoffDustOverlays, Images_LiftOffDustLO, images_tbl, 999, a5, a6);
	loadImagesData(&ShieldOverlays, Images_ShieldOverlayLO, images_tbl, 999, a5, a6);
	if (images_tbl)
	{
		SMemFree(images_tbl, "Starcraft\\SWAR\\lang\\CImage.cpp", 1389, 0);
	}
}

FAIL_STUB_PATCH(LoadImageData);

DatLoad imagesDat_[] = {
	DatLoad { Images_GrpFile, 4, 999 },
	DatLoad { Images_IsTurnable, 1, 999 },
	DatLoad { Images_IsClickable, 1, 999 },
	DatLoad { Images_UseFullIscript, 1, 999 },
	DatLoad { Image_DrawIfCloaked, 1, 999 },
	DatLoad { Image_DrawFunction, 1, 999 },
	DatLoad { Images_Remapping, 1, 999 },
	DatLoad { Images_IscriptEntry, 4, 999 },
	DatLoad { Images_ShieldOverlayLO, 4, 999 },
	DatLoad { Images_AttackOverlayLO, 4, 999 },
	DatLoad { Images_InjuryOverlayLO, 4, 999 },
	DatLoad { Images_SpecialOverlayLO, 4, 999 },
	DatLoad { Images_LandingDustLO, 4, 999 },
	DatLoad { Images_LiftOffDustLO, 4, 999 },
};

void LoadInitIscriptBIN_()
{
	int iscript_bin_size;
	iscript_data = fastFileRead_(&iscript_bin_size, 0, "scripts\\iscript.bin", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	// TODO: dynamically allocate imagesDat memory
	LoadGameData_(imagesDat_, "arr\\images.dat");
	if (!SBmpLoadImage("game\\tselect.pcx", 0, dword_5240B8, 24, 0, 0, 0))
	{
		SysWarn_FileNotFound("game\\tselect.pcx", SErrGetLastError());
	}
	LoadImageData_();
	InitDamageOverlayCounts();
}

FAIL_STUB_PATCH(LoadInitIscriptBIN);

void __fastcall CleanupIscriptBINHandle_(bool exit_code)
{
	if (iscript_data)
	{
		SMemFree(iscript_data, "Starcraft\\SWAR\\lang\\CImage.cpp", 1626, 0);
		iscript_data = NULL;
	}

	sub_47AB40((LO_Overlays*)ImageGrpGraphics, 999);
	sub_47AB40(&lo_files.attackOverlays, 999);
	sub_47AB40(&lo_files.damageOverlays, 999);
	sub_47AB40(&lo_files.specialOverlays, 999);
	sub_47AB40(&lo_files.landingDustOverlays, 999);
	sub_47AB40(&lo_files.liftoffDustOverlays, 999);
	sub_47AB40(&ShieldOverlays, 999);

	// TODO: dynamically clean imagesDat memory
}

FAIL_STUB_PATCH(CleanupIscriptBINHandle);

UnitType GetBaseBuilding(Race race)
{
	switch (race)
	{
	case Race::RACE_Zerg:
		return UnitType::Zerg_Hatchery;
	case Race::RACE_Terran:
		return UnitType::Terran_Command_Center;
	case Race::RACE_Protoss:
		return UnitType::Protoss_Nexus;
	default:
		return (UnitType)228;
	}
}

void CreateInitialMeleeBuildings_(Race race, u8 player_index)
{
	UnitType base_building = GetBaseBuilding(race);

	// TODO: test this piece of code against the one in StarCraft.exe:
	int v5 = Unit_Placement[base_building].x;
	int v6 = Unit_Placement[base_building].y;
	int v7 = (v5 - HIWORD(v5)) / 2;
	int v8 = (v6 - HIWORD(v6)) / 2;
	int v9 = v7 + ((startPositions[player_index].x - v7) & 0xFFE0);
	int v10 = v8 + ((startPositions[player_index].y - v8) & 0xFFE0);

	Box16 v14;
	v14.left = v9 - v7;
	v14.top = v10 - v6 / 2;
	v14.bottom = v6 / 2 + v10 - 1;
	v14.right = v7 + v9 - 1;
	ModifyUnit_maybe(&v14, 0, (int(__fastcall*)(CUnit*, void*))SelfDestructCB);

	CUnit* v12 = CreateUnit(base_building, v9, v10, player_index);
	if (v12)
	{
		updateUnitStatsFinishBuilding(v12);
		if (sub_49EC30(v12))
		{
			updateUnitStrengthAndApplyDefaultOrders(v12);
		}
		if (spreadsCreep(v12->unitType, 1) || (Unit_PrototypeFlags[v12->unitType] & CreepBuilding) != 0)
		{
			ApplyCreepAtLocationFromUnitType(v12->unitType, v12->sprite->position.x, v12->sprite->position.y);
			sub_49D660(v12);
			sub_49D660(v12);
		}
	}
}

FAIL_STUB_PATCH(CreateInitialMeleeBuildings);

UnitType GetWorkerType(Race race)
{
	switch (race)
	{
	case Race::RACE_Zerg:
		return UnitType::Zerg_Drone;
	case Race::RACE_Terran:
		return UnitType::Terran_SCV;
	case Race::RACE_Protoss:
		return UnitType::Protoss_Probe;
	case 4:
		return UnitType::Zerg_Infested_Terran;
	default:
		return (UnitType)228;
	}
}

void CreateInitialMeleeWorker_(Race race, unsigned __int8 player_index)
{
	UnitType workerUnitType = GetWorkerType(race);

	CUnit* worker = CreateUnit(workerUnitType, startPositions[player_index].x, startPositions[player_index].y, player_index);
	if (worker)
	{
		updateUnitStatsFinishBuilding(worker);
		if (sub_49EC30(worker))
		{
			updateUnitStrengthAndApplyDefaultOrders(worker);
		}
	}
}

FAIL_STUB_PATCH(CreateInitialMeleeWorker);

void CreateInitialMeleeUnits_()
{
	for (int player_index = 0; player_index < 8; player_index++)
	{
		PlayerInfo* player = Players + player_index;
		if (player->nType == PlayerType::PT_Human || player->nType == PlayerType::PT_Computer)
		{
			StartingUnits starting_units;
			if (gameData.got_file_values.victory_conditions != VictoryConditions::VC_MAP_DEFAULT
				&& gameData.got_file_values.starting_units != StartingUnits::SU_MAP_DEFAULT
				&& !gameData.got_file_values.tournament_mode
				&& player_index < 8
				&& playerForce[player_index])
			{
				starting_units = StartingUnits::SU_WORKER_AND_CENTER;
			}
			else
			{
				starting_units = gameData.got_file_values.starting_units;
			}

			switch (starting_units)
			{
			case StartingUnits::SU_WORKER_AND_CENTER:
				CreateInitialMeleeBuildings_(player->nRace, player_index);
				if (player->nRace == Race::RACE_Zerg)
					CreateInitialOverlord(player_index);
				[[fallthrough]];
			case StartingUnits::SU_WORKER_ONLY:
				CreateInitialMeleeWorker_(player->nRace, player_index);
				CreateInitialMeleeWorker_(player->nRace, player_index);
				CreateInitialMeleeWorker_(player->nRace, player_index);
				CreateInitialMeleeWorker_(player->nRace, player_index);
				[[fallthrough]];
			case StartingUnits::SU_MAP_DEFAULT:
				break;
			}
		}
	}
}

FAIL_STUB_PATCH(CreateInitialMeleeUnits);

bool __stdcall ChkLoader_TYPE_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_VER_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_DIM_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_VCOD_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_ERA_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_STR_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_MBRF_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_SPRP_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_FORC_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_MTXM_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_THG2_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_UNIT_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_UPRP_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_COLR_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_MASK_(SectionData* section_data, int section_size, MapChunks* a3);

ChkSectionLoader CreateChkSectionLoader(const char(&section_name)[5], bool(__stdcall* func)(SectionData*, int, MapChunks*), int flags)
{
	return {
		{section_name[0], section_name[1], section_name[2], section_name[3]},
		func,
		flags,
	};
}

ChkSectionLoader chk_loaders_version_[] = {
	CreateChkSectionLoader("TYPE", ChkLoader_TYPE_, 0),
	CreateChkSectionLoader("VER ", ChkLoader_VER_, 1),
};

ChkSectionLoader chk_loaders_lobby_[] = {
	CreateChkSectionLoader("VER ", ChkLoader_VER_, 1),
	CreateChkSectionLoader("DIM ", ChkLoader_DIM_, 1),
	CreateChkSectionLoader("ERA ", ChkLoader_ERA_, 1),
	CreateChkSectionLoader("OWNR", ChkLoader_OWNR, 1),
	CreateChkSectionLoader("SIDE", ChkLoader_SIDE, 1),
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("SPRP", ChkLoader_SPRP_, 1),
	CreateChkSectionLoader("FORC", ChkLoader_FORC_, 1),
	CreateChkSectionLoader("VCOD", ChkLoader_VCOD_, 1),
};

ChkSectionLoader chk_loaders_briefing_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MBRF", ChkLoader_MBRF_, 1),
};

ChkSectionLoader chk_loaders_melee_vanilla_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT_, 1),
};

ChkSectionLoader chk_loaders_melee_broodwar_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT_, 1),
	CreateChkSectionLoader("COLR", ChkLoader_COLR_, 1),
};

ChkSectionLoader chk_loaders_ums_1_00_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("MASK", ChkLoader_MASK_, 1),
	CreateChkSectionLoader("UNIS", ChkLoader_UNIS, 1),
	CreateChkSectionLoader("UPGS", ChkLoader_UPGS, 1),
	CreateChkSectionLoader("TECS", ChkLoader_TECS, 1),
	CreateChkSectionLoader("PUNI", ChkLoader_PUNI, 1),
	CreateChkSectionLoader("UPGR", ChkLoader_UPGR, 1),
	CreateChkSectionLoader("PTEC", ChkLoader_PTEC, 1),
	CreateChkSectionLoader("UNIx", ChkLoader_UNIx, 0),
	CreateChkSectionLoader("UPGx", ChkLoader_UPGx, 0),
	CreateChkSectionLoader("TECx", ChkLoader_TECx, 0),
	CreateChkSectionLoader("PUPx", ChkLoader_PUPx, 0),
	CreateChkSectionLoader("PTEx", ChkLoader_PTEx, 0),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT_, 1),
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP_, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN_1_00, 1),
	CreateChkSectionLoader("TRIG", ChkLoader_TRIG, 1),
};

ChkSectionLoader chk_loaders_ums_1_04_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("MASK", ChkLoader_MASK_, 1),
	CreateChkSectionLoader("UNIS", ChkLoader_UNIS, 1),
	CreateChkSectionLoader("UPGS", ChkLoader_UPGS, 1),
	CreateChkSectionLoader("TECS", ChkLoader_TECS, 1),
	CreateChkSectionLoader("PUNI", ChkLoader_PUNI, 1),
	CreateChkSectionLoader("UPGR", ChkLoader_UPGR, 1),
	CreateChkSectionLoader("PTEC", ChkLoader_PTEC, 1),
	CreateChkSectionLoader("UNIx", ChkLoader_UNIx, 0),
	CreateChkSectionLoader("UPGx", ChkLoader_UPGx, 0),
	CreateChkSectionLoader("TECx", ChkLoader_TECx, 0),
	CreateChkSectionLoader("PUPx", ChkLoader_PUPx, 0),
	CreateChkSectionLoader("PTEx", ChkLoader_PTEx, 0),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT_, 1),
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP_, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN, 1),
	CreateChkSectionLoader("TRIG", ChkLoader_TRIG, 1),
};

ChkSectionLoader chk_loaders_ums_broodwar_1_04_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("MASK", ChkLoader_MASK_, 1),
	CreateChkSectionLoader("UNIx", ChkLoader_UNIx, 1),
	CreateChkSectionLoader("UPGx", ChkLoader_UPGx, 1),
	CreateChkSectionLoader("TECx", ChkLoader_TECx, 1),
	CreateChkSectionLoader("PUNI", ChkLoader_PUNI, 1),
	CreateChkSectionLoader("PUPx", ChkLoader_PUPx, 1),
	CreateChkSectionLoader("PTEx", ChkLoader_PTEx, 1),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT_, 1),
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP_, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN, 1),
	CreateChkSectionLoader("TRIG", ChkLoader_TRIG, 1),
	CreateChkSectionLoader("COLR", ChkLoader_COLR_, 1),
};

ChkLoader chk_loaders_[] = {
	{
		// Loaders for Starcraft 1.00
		59,
		chk_loaders_lobby_, _countof(chk_loaders_lobby_),
		chk_loaders_briefing_, _countof(chk_loaders_briefing_),
		chk_loaders_melee_vanilla_, _countof(chk_loaders_melee_vanilla_),
		chk_loaders_ums_1_00_, _countof(chk_loaders_ums_1_00_),
		0
	},
	{
		// Loaders for Starcraft 1.04
		63,
		chk_loaders_lobby_, _countof(chk_loaders_lobby_),
		chk_loaders_briefing_, _countof(chk_loaders_briefing_),
		chk_loaders_melee_vanilla_, _countof(chk_loaders_melee_vanilla_),
		chk_loaders_ums_1_04_, _countof(chk_loaders_ums_1_04_),
		0
	},
	{
		// Loaders for Brood War 1.04
		205,
		chk_loaders_lobby_, _countof(chk_loaders_lobby_),
		chk_loaders_briefing_, _countof(chk_loaders_briefing_),
		chk_loaders_melee_broodwar_, _countof(chk_loaders_melee_broodwar_),
		chk_loaders_ums_broodwar_1_04_, _countof(chk_loaders_ums_broodwar_1_04_),
		1
	},
};

MEMORY_PATCH(0x4BF65A, &chk_loaders_->lobby_loader_count);
MEMORY_PATCH(0x4BF660, &chk_loaders_->lobby_loaders);
MEMORY_PATCH(0x4CC0CD, chk_loaders_);
MEMORY_PATCH(0x4CC0E1, &chk_loaders_->requires_expansion);
MEMORY_PATCH(0x4CCA8A, &chk_loaders_->briefing_loader_count);
MEMORY_PATCH(0x4CCA90, &chk_loaders_->briefing_loaders);
MEMORY_PATCH(0x4CCBFC, &chk_loaders_->lobby_loader_count);
MEMORY_PATCH(0x4CCC02, &chk_loaders_->lobby_loaders);

int sub_413550_(ChkSectionLoader* loader, ChunkNode* a2, int a3, MapChunks* a4)
{
	ChunkData* v6;

	int v4 = 0;
	if (a3 <= 0)
	{
		return 1;
	}
	else
	{
		while (1)
		{
			v6 = a2->f2;
			if (v6 > NULL)
				break;
		LABEL_8:
			++v4;
			if (v4 >= a3)
			{
				return 1;
			}
		}
		while (1)
		{
			if (v6->section_data.chunk_name.as_number == *(DWORD*)loader[v4].name)
			{
				if (loader[v4].func)
				{
					if (!loader[v4].func(&v6->section_data, v6->section_data.size, a4))
						break;
				}
			}
			v6 = v6->next;
			if ((signed int)v6 <= 0)
				goto LABEL_8;
		}
		return 0;
	}
}

FAIL_STUB_PATCH(sub_413550);

signed int ReadChunkNodes_(int chk_section_loader_count, int a2, ChkSectionLoader* chk_section_loader, void* chk_data, MapChunks* a4)
{
	ChunkNode v8;

	v8.f1 = (ChunkData*)&v8.f1;
	v8.count = 0;
	v8.f2 = (ChunkData*)~(unsigned int)&v8.f1;
	sub_413670((Chunk*)chk_data, &v8, a2, ChunkNode_Constructor);
	if (sub_4135C0(chk_section_loader, &v8, chk_section_loader_count))
	{
		if (sub_413550_(chk_section_loader, &v8, chk_section_loader_count, a4))
		{
			ChunkNode_Destructor(&v8);
			sub_404B20(&v8);
			return 1;
		}
		else
		{
			ChunkNode_Destructor(&v8);
			sub_404B20(&v8);
			return 0;
		}
	}
	else
	{
		ChunkNode_Destructor(&v8);
		_list_unlink((ListNode*)&v8);
		if (v8.f1)
		{
			if ((signed int)v8.f2 <= 0)
			{
				*(_DWORD*)~(unsigned int)v8.f2 = (DWORD)v8.f1;
				v8.f1->next = v8.f2;
				return 0;
			}
			*(ChunkData**)((char*)&v8.f2->previous
				+ (int)&v8.f1
				- (int)v8.f1->next) = v8.f1;
			v8.f1->next = v8.f2;
		}
		return 0;
	}
}

FAIL_STUB_PATCH(ReadChunkNodes);

int ReadMapChunks_(MapChunks* a1, void* chk_data, int* out_version_loader_index, int chk_size)
{
	if (out_version_loader_index && chk_data)
	{
		MapChunks location;
		SMemZero(&location, sizeof(location));

		*out_version_loader_index = 0;

		if (!a1)
		{
			a1 = &location;
		}
		if (ReadChunkNodes_(_countof(chk_loaders_version_), chk_size, chk_loaders_version_, chk_data, a1))
		{
			for (int i = 0; i < _countof(chk_loaders_); i++)
			{
				if (chk_loaders_[i].version == a1->version)
				{
					if (!chk_loaders_[i].requires_expansion || IsExpansion)
					{
						*out_version_loader_index = i;
						return 1;
					}
					return 0;
				}
			}
		}
	}
	else
	{
		SErrSetLastError(0x57u);
	}
	return 0;
}

FAIL_STUB_PATCH(ReadMapChunks);

int ReadLobbyChunks(void* chk_data, int chk_size, MapChunks* a4)
{
	int loader_index = 0;
	if (!ReadMapChunks_(a4, chk_data, &loader_index, chk_size))
	{
		return 0;
	}

	return ReadChunkNodes_(chk_loaders_[loader_index].lobby_loader_count, chk_size, chk_loaders_[loader_index].lobby_loaders, chk_data, a4);
}

int ReadGameChunks(void* chk_data, int chk_size)
{
	int loader_index = 0;
	if (!ReadMapChunks_(0, chk_data, &loader_index, chk_size))
	{
		return 0;
	}

	ChkSectionLoader* loaders;
	int loader_count;
	if (gameData.got_file_values.victory_conditions || gameData.got_file_values.starting_units || gameData.got_file_values.tournament_mode)
	{
		loaders = chk_loaders_[loader_index].melee_loaders;
		loader_count = chk_loaders_[loader_index].melee_loader_count;
	}
	else
	{
		loaders = chk_loaders_[loader_index].ums_loaders;
		loader_count = chk_loaders_[loader_index].ums_loader_count;
	}
	return ReadChunkNodes_(loader_count, chk_size, loaders, chk_data, 0);
}

FAIL_STUB_PATCH(sub_4CC2A0);

BOOL sub_4CC7F0_(char* a1)
{
	char buff[260];
	char dest[260];
	int chk_size;

	if (CampaignIndex)
	{
		SStrCopy(dest, a1, 0x104u);
	}
	else
	{
		dest[0] = 0;
		int result = LoadFileArchiveToSBigBuf(a1, &chk_size, 1, &mapArchiveHandle);
		if (!result)
		{
			return result;
		}
	}
	if (dest[0])
	{
		_snprintf(buff, 260u, "%s\\%s", dest, "staredit\\scenario.chk");
	}
	else
	{
		SStrCopy(buff, "staredit\\scenario.chk", 0x104u);
	}
	void* chk_data = (void*)fastFileRead_(&chk_size, 0, buff, 0, 1, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2141);
	int result;
	if (chk_data)
	{
		result = ReadGameChunks(chk_data, chk_size);
		SMemFree(chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2159, 0);
	}
	else
	{
		result = 0;
	}

	if (mapArchiveHandle)
	{
		SFileCloseArchive(mapArchiveHandle);
		mapArchiveHandle = NULL;
	}
	return result;
}

FAIL_STUB_PATCH(sub_4CC7F0);

int LoadMap_()
{
	if (InReplay)
	{
		return ReadGameChunks(scenarioChk, scenarioChkSize);
	}
	else if (CurrentMapFileName[0])
	{
		return sub_4CC7F0_(CurrentMapFileName);
	}

	return 0;
}

FAIL_STUB_PATCH(LoadMap);

UnknownTilesetRelated1* TILESET_PALETTE_RELATED[] = {
	&stru_512778,
	&stru_6D1228,
	&stru_6D1228,
	&stru_5127B8,
	&stru_512778,
	&stru_51279C,
	&stru_51279C,
	&stru_51279C,
};

MEMORY_PATCH(0x4BDD8A, TILESET_PALETTE_RELATED);
MEMORY_PATCH(0x4C99E4, TILESET_PALETTE_RELATED);
MEMORY_PATCH(0x4CB56A, TILESET_PALETTE_RELATED);
MEMORY_PATCH(0x4CB5DF, TILESET_PALETTE_RELATED);
MEMORY_PATCH(0x4CBEDA, TILESET_PALETTE_RELATED);
MEMORY_PATCH(0x4EEEB7, TILESET_PALETTE_RELATED);

void drawAllMinimapBoxes_()
{
	Bitmap* v0 = dword_6CF4A8;
	dword_6CF4A8 = &minimap_surface;
	dword_59C2B8 = 0;
	dword_59C1A8 = 0;

	for (int player = 11; player >= 0; player--)
	{
		if (player >= 8)
		{
			drawMinimapUnitBox(player);
		}
		else if (g_LocalNationID != player || InReplay)
		{
			drawMinimapUnitBox2(player);
		}
	}

	if (!InReplay)
	{
		drawAllMinimapUnitBoxes(g_LocalNationID);
	}

	if (dword_654868)
	{
		for (CThingy* thingy = dword_654868; thingy; thingy = thingy->next)
		{
			if (thingy->hitPoints < 203 || thingy->hitPoints > 213)
			{
				if (!InReplay || !CThingyIsVisible(1, thingy))
				{
					char v8;
					CSprite* sprite = thingy->sprite;
					if (sprite->spriteID == 275 || sprite->spriteID == 279 || sprite->spriteID == 280 || sprite->spriteID == 281)
					{
						v8 = byte_6CEB39;
					}
					else if (byte_6D5BBE == 0)
					{
						v8 = PlayerColors[sprite->playerID];
					}
					else if (Alliance[g_LocalNationID].player[sprite->playerID])
					{
						v8 = byte_6CEB31;
					}
					else
					{
						v8 = byte_6CEB34;
					}

					drawUnitBox(v8, sprite->position.x, sprite->position.y, Unit_Placement[thingy->hitPoints].x, Unit_Placement[thingy->hitPoints].y, 1);
					--dword_59C2B8;
				}
			}
		}
	}

	dword_6CF4A8 = v0;
}

FAIL_STUB_PATCH(drawAllMinimapBoxes);

void minimapGameUpdate_(dialog* a1)
{
	a1->pfcnUpdate = MinimapImageUpdate;
	if (dword_5993AC == 0)
	{
		minimapSurfaceUpdate();
		sub_4A4150();
		drawAllMinimapBoxes_();
	}
	if ((a1->lFlags & CTRL_UPDATE) == 0)
	{
		a1->lFlags |= CTRL_UPDATE;
		updateDialog(a1);
	}
}

FAIL_STUB_PATCH(minimapGameUpdate);

bool __fastcall MinimapImageInteract_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_MOUSEMOVE:
		minimapGameMouseUpdate(dlg);
		return 1;
	case EVN_LBUTTONDOWN:
	case EVN_LBUTTONDBLCLK:
		if (IS_GAME_PAUSED)
		{
			return 1;
		}
		else if (byte_641694)
		{
			MinimapGameTargetOrder(evt);
			return 1;
		}
		else
		{
			MinimapGameClickEvent(evt, dlg);
			return 1;
		}
	case EVN_LBUTTONUP:
	case EVN_RBUTTONUP:
		minimapGameResetMouseInput(dlg);
		return 1;
	case EVN_RBUTTONDOWN:
	case EVN_RBUTTONDBLCLK:
		if (IS_GAME_PAUSED)
		{
			return 1;
		}
		else if (is_placing_building || byte_641694)
		{
			MinimapGameRightclickEventMoveto(dlg);
			return 1;
		}
		else
		{
			CMDACT_RightClick(evt);
			return 1;
		}
	case EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			minimapGameUpdate_(dlg);
			break;
		case USER_DESTROY:
		case USER_NEXT:
			minimapGameResetMouseInput(dlg);
			break;
		case USER_MOUSEMOVE:
			return 1;
		default:
			break;
		}
	}

	return GenericDlgInteractFxns[dlg->wCtrlType](dlg, evt);
}

FAIL_STUB_PATCH(MinimapImageInteract);

void minimap_dlg_Activate_(dialog* dlg)
{
	switch (dlg->wIndex)
	{
	case 2:
		byte_6D5BBF ^= 1;
		dword_59C1A4 = 0;
		sub_4A4150();
		drawAllMinimapBoxes_();
		sub_4A3870();
		if ((minimap_dialog->lFlags & CTRL_UPDATE) == 0)
		{
			minimap_dialog->lFlags |= CTRL_UPDATE;
			updateDialog(minimap_dialog);
		}
		drawShowHideTerrainContextHelp(dlg);
		break;
	case 3:
		refreshSelectionScreen_();
		byte_63FF70 = byte_581D60;
		BWFXN_OpenGameDialog("rez\\msgfltr.bin", msgfltr_Main);
		break;
	case 4:
		refreshSelectionScreen_();
		MinimapControl_ShowAllianceDialog();
		break;
	}
}

FAIL_STUB_PATCH(minimap_dlg_Activate);

bool __fastcall MinimapButton_EventHandler_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_MOUSEMOVE:
		minimap_dlg_MouseMove(dlg, evt);
		break;
	case EVN_USER:
		if (evt->dwUser == USER_CREATE)
		{
			Minimap_InitVisionButton(dlg);
		}
		else if (evt->dwUser == USER_ACTIVATE)
		{
			minimap_dlg_Activate_(dlg);
			if (dlg->wIndex == -3 || dlg->wIndex == -2)
			{
				LastControlID = dlg->wIndex;
				dlg->fields.ctrl.pDlg->pfcnInteract(dlg->fields.ctrl.pDlg, evt);
				return 1;
			}
		}
		break;
	}

	return GenericControlInteract(dlg, evt);
}

FAIL_STUB_PATCH(MinimapButton_EventHandler);

void setMapSizeConstants_();

void __fastcall Minimap_TimerRefresh_(dialog* dlg, __int16 timer_id)
{
	if (HasMegatileUpdate)
	{
		minimapSurfaceUpdate();
	}
	sub_4A4150();
	drawAllMinimapBoxes_();
	sub_4A3870();

	if ((minimap_dialog->lFlags & DialogFlags::CTRL_UPDATE) == 0)
	{
		minimap_dialog->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog(minimap_dialog);
	}
	HasMegatileUpdate = 0;
}

FAIL_STUB_PATCH(Minimap_TimerRefresh);

void __fastcall updateMinimapSurfaceInfoProc_(dialog* a1, __int16 a2)
{
	byte_6D5BC0 ^= 1;
	if (byte_6D5BC1)
	{
		int i = 0;
		while (stru_59C1B8[i].a0 == 0)
		{
			i += 1;
			if (i >= _countof(stru_59C1B8))
			{
				if (HasMegatileUpdate)
				{
					minimapSurfaceUpdate();
				}
				if (dword_5993AC == 1 || !byte_6D5BBF)
				{
					sub_4A3A00();
					minimapVisionUpdate();
				}
				else
				{
					memset(minimap_surface.data, 0, minimap_surface.ht * minimap_surface.wid);
				}
				drawAllMinimapBoxes_();
				sub_4A3870();
				if ((minimap_dialog->lFlags & DialogFlags::CTRL_UPDATE) == 0)
				{
					minimap_dialog->lFlags |= DialogFlags::CTRL_UPDATE;
					updateDialog(minimap_dialog);
				}
				HasMegatileUpdate = 0;
				return;
			}
		}
	}
}

FAIL_STUB_PATCH(updateMinimapSurfaceInfoProc);

void __fastcall updateMinimapSurfaceInfo2Proc_(dialog* a1, __int16 a2)
{
	int v2 = 0;
	for (int i = 0; i < _countof(stru_59C1B8); i++)
	{
		if (stru_59C1B8[i].a0)
		{
			if (stru_59C1B8[i].d == 19)
			{
				stru_59C1B8[i].a0 = 0;
			}
			else
			{
				stru_59C1B8[i].d++;
			}
			v2 = 1;
		}
	}
	if (v2)
	{
		if (HasMegatileUpdate)
		{
			minimapSurfaceUpdate();
		}
		if (dword_5993AC == 1 || !byte_6D5BBF)
		{
			sub_4A3A00();
			minimapVisionUpdate();
		}
		else
		{
			memset(minimap_surface.data, 0, minimap_surface.ht * minimap_surface.wid);
		}
		drawAllMinimapBoxes_();
		sub_4A3870();
		if ((minimap_dialog->lFlags & DialogFlags::CTRL_UPDATE) == 0)
		{
			minimap_dialog->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog(minimap_dialog);
		}
		HasMegatileUpdate = 0;
	}
}

FAIL_STUB_PATCH(updateMinimapSurfaceInfo2Proc);

void updateMinimapPreviewDlg_(dialog* dlg)
{
	static FnInteract menu_functions[] = {
		MinimapImageInteract_,
	};

	static FnInteract ingame_functions[] = {
		MinimapImageInteract_,
		MinimapButton_EventHandler_,
		MinimapButton_EventHandler_,
		MinimapButton_EventHandler_,
	};

	dlg->pfcnUpdate = MiniMapUpdate;
	dlg->lFlags = dlg->lFlags | CTRL_USELOCALGRAPHIC;
	int v1 = dword_5993AC;
	if (dword_5993AC == 1)
	{
		dlg->lFlags = dlg->lFlags | CTRL_USELOCALGRAPHIC | CTRL_TRANSPARENT;
		registerUserDialogAction(dlg, sizeof(menu_functions), menu_functions);
	}
	else if (dword_5993AC == 0)
	{
		registerUserDialogAction(dlg, sizeof(ingame_functions), ingame_functions);
		BINDLG_BlitSurface(dlg);
		v1 = dword_5993AC;
	}
	else
	{
		registerUserDialogAction(dlg, sizeof(menu_functions), menu_functions);
	}

	minimap_dialog = getControlFromIndex(minimap_Dlg, 1);
	if (v1 == 0)
	{
		setMapSizeConstants_();
		if (dword_5993AC == 0)
		{
			SetCallbackTimer(1, dlg, 200, Minimap_TimerRefresh_);
			SetCallbackTimer(2, dlg, 200, updateMinimapPositioninfoProc);
			SetCallbackTimer(3, dlg, 200, updateMinimapSurfaceInfoProc_);
			SetCallbackTimer(6, dlg, 100, updateMinimapSurfaceInfo2Proc_);
			if (multiPlayerMode && (GetActivePlayerCount() > 1 || multiPlayerMode && gameData.got_file_values.template_id == 15) || InReplay)
			{
				SetCallbackTimer(5, dlg, 500, playerInfoSomethingTvBProc);
			}
			if (dword_5993AC == 0)
			{
				showDialog(minimap_dialog);
			}
		}
	}
	if ((minimap_dialog->lFlags & CTRL_UPDATE) == 0)
	{
		minimap_dialog->lFlags |= CTRL_UPDATE;
		updateDialog(minimap_dialog);
	}
}

FAIL_STUB_PATCH(updateMinimapPreviewDlg);

void minimapPreviewUpdateState_()
{
	sub_4A4150();
	drawAllMinimapBoxes_();
	sub_4A3870();
	if ((minimap_dialog->lFlags & DialogFlags::CTRL_UPDATE) == 0)
	{
		minimap_dialog->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog(minimap_dialog);
	}
}

FAIL_STUB_PATCH(minimapPreviewUpdateState);

void initMapData_();
void setMapSizeConstants_();

bool __fastcall MiniMapPreviewInteract_(dialog* dlg, dlgEvent* evt)
{
	dialog* v6;
	dialog* v7;

	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
		return 0;
	case EVN_MOUSEMOVE:
		if (isSinglePaused())
		{
			return 0;
		}
		minimapPreviewMouseUpdate(dlg, evt);
		break;
	case EVN_LBUTTONDOWN:
	case EVN_LBUTTONDBLCLK:
	case EVN_RBUTTONDOWN:
	case EVN_RBUTTONDBLCLK:
		if (IS_GAME_PAUSED && !multiPlayerMode)
		{
			return 0;
		}
		v7 = dlgSetMouseOver(dlg, evt);
		if (!v7)
		{
			return 0;
		}
		if (v7 != dlg)
		{
			v7->pfcnInteract(v7, evt);
		}
		return 1;
	case EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			updateMinimapPreviewDlg_(dlg);
			break;
		case EventUser::USER_DESTROY:
			killMinimapPreviewDlg(dlg);
			break;
		case EventUser::USER_MOUSEMOVE:
			return 1;
		}

		break;
	case EVN_CHAR:
		if (isSinglePaused() || dword_5993AC || evt->wVirtKey != VK_TAB)
		{
			return 0;
		}
		else
		{
			if (is_keycode_used[VK_SHIFT])
			{
				++byte_6D5BBE;
				if (byte_6D5BBE > 2)
				{
					byte_6D5BBE = 0;
					dword_6D5E1C = 1;
				}
				else if (byte_6D5BBE != 1)
				{
					dword_6D5E1C = 1;
				}
			}
			else
			{
				byte_6D5BBF = !byte_6D5BBF;
			}
			minimapPreviewUpdateState_();
			v6 = getControlFromIndex(dlg, 2);
			if (SLOBYTE(v6->lFlags) < 0)
			{
				drawShowHideTerrainContextHelp(v6);
			}
			return 1;
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(MiniMapPreviewInteract);

void load_gluMinimap_()
{
	word_59C19C = MoveToTile.y;
	word_59CC70 = MoveToTile.x;
	dword_59C2B8 = 0;
	dword_59C1A8 = 0;
	dword_59C1AC = (void*)LoadGraphic("game\\blink.grp", 0, "Starcraft\\SWAR\\lang\\minimap.cpp", 2011);
	if (!SBmpLoadImage("game\\tblink.pcx", 0, byte_59CAC0, sizeof(byte_59CAC0), 0, 0, 0))
	{
		SysWarn_FileNotFound("game\\tblink.pcx", SErrGetLastError());
	}

	for (int i = 0; i < _countof(palette); i++)
	{
		if (dword_5993AC)
		{
			byte_59CB60[i] = sub_4BDB30(stru_59C6C0, palette[i]);
		}
		else if (sub_4CB560(i))
		{
			byte_59CB60[i] = sub_4BDB30(palette, palette[i]);
		}
		else
		{
			byte_59CB60[i] = i;
		}
	}

	for (int i = 0; i < _countof(stru_59C1B8); i++)
	{
		stru_59C1B8[i].a0 = 0;
	}

	if (dword_5993AC)
	{
		setMapSizeConstants_();
		minimapSurfaceUpdate();
		if (dword_5993AC == 1 || !byte_6D5BBF)
		{
			sub_4A3A00();
			minimapVisionUpdate();
		}
		else
		{
			memset(minimap_surface.data, 0, minimap_surface.ht * minimap_surface.wid);
		}
		drawAllMinimapBoxes_();
		sub_4A3870();
		if ((minimap_dialog->lFlags & DialogFlags::CTRL_UPDATE) == 0)
		{
			minimap_dialog->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog(minimap_dialog);
		}
	}
	else
	{
		minimap_Dlg = LoadDialog("rez\\minimap.bin");
		InitializeDialog_(minimap_Dlg, MiniMapPreviewInteract_);
	}
}

FAIL_STUB_PATCH(load_gluMinimap);

DatLoad spritesDat_[] = {
	DatLoad { Sprites_Image, 2, 517 },
	DatLoad { Sprites_HealthBarSize, 1, 387 },
	DatLoad { Sprites_Unknown, 1, 517 },
	DatLoad { Sprites_IsVisible, 1, 517 },
	DatLoad { Sprites_SelectionCircle, 1, 387 },
	DatLoad { Sprites_SelectionCircleVPos, 1, 387 },
};

void InitializeSpriteArray_()
{
	LoadGameData_(spritesDat_, "arr\\sprites.dat"); // TODO: dynamically allocate spritesDat memory
	memset(SpriteTable, 0, sizeof(SpriteTable));
	memset(SpritesOnTileRow.heads, 0, sizeof(SpritesOnTileRow.heads));
	memset(&SpritesOnTileRow, 0, 0x400u);

	SpriteTable[0].index = 0;
	dword_63FE34 = SpriteTable;
	UnusedSprites = SpriteTable;

	for (int index = 1; index < _countof(SpriteTable); index++)
	{
		CSprite* sprite = SpriteTable + index;

		sprite->index = index;
		if (dword_63FE34 == UnusedSprites)
		{
			dword_63FE34 = sprite;
		}
		sprite->prev = UnusedSprites;
		sprite->next = UnusedSprites->next;
		if (UnusedSprites->next)
		{
			UnusedSprites->next->prev = sprite;
		}
		UnusedSprites->next = sprite;
	}
}

FAIL_STUB_PATCH(InitializeSpriteArray);

DatLoad flingyDat_[] = {
	DatLoad { Flingy_SpriteID, 2, 209 },
	DatLoad { Flingy_TopSpeed, 4, 209 },
	DatLoad { Flingy_Acceleration, 2, 209 },
	DatLoad { Flingy_HaltDistance, 4, 209 },
	DatLoad { Flingy_TurnSpeed, 1, 209 },
	DatLoad { Flingy_Unused, 1, 209 },
	DatLoad { Flingy_MovementControl, 1, 209 },
};

void InitializeFlingyDat_()
{
	// TODO: dynamically allocate flingyDat memory
	LoadGameData_(flingyDat_, "arr\\flingy.dat");
	memset(dword_63FEE0, 0, 76u);
	dword_63FF3C = (CUnit*)dword_63FEE0;
	dword_63FF38 = (CUnit*)dword_63FEE0;
	dword_63FEC8 = 0;
	dword_63FF34 = 0;
}

FAIL_STUB_PATCH(InitializeFlingyDat);

DatLoad weaponsDat_[] = {
	DatLoad { Weapon_Label, 2, 130 },
	DatLoad { Weapon_Graphic, 4, 130 },
	DatLoad { Weapon_TechHint, 1, 130 },
	DatLoad { Weapon_TargetFlags, 2, 130 },
	DatLoad { Weapon_MinRange, 4, 130 },
	DatLoad { Weapon_MaxRange, 4, 130 },
	DatLoad { Weapon_Upgrade, 1, 130 },
	DatLoad { Weapon_DamageType, 1, 130 },
	DatLoad { Weapon_Behavior, 1, 130 },
	DatLoad { Weapon_RemoveAfter, 1, 130 },
	DatLoad { Weapon_ExplosionType, 1, 130 },
	DatLoad { Weapon_InnerSplashRadius, 2, 130 },
	DatLoad { Weapon_MedianSplashRadius, 2, 130 },
	DatLoad { Weapon_OuterSplashRadius, 2, 130 },
	DatLoad { Weapon_DamageAmount, 2, 130 },
	DatLoad { Weapon_DamageBonus, 2, 130 },
	DatLoad { Weapon_DamageCooldown, 1, 130 },
	DatLoad { Weapon_DamageFactor, 1, 130 },
	DatLoad { Weapon_AttackDirection, 1, 130 },
	DatLoad { Weapon_LaunchSpin, 1, 130 },
	DatLoad { Weapon_XOffset, 1, 130 },
	DatLoad { Weapon_YOffset, 1, 130 },
	DatLoad { Weapon_TargetErrorMessage, 2, 130 },
	DatLoad { Weapon_Icon, 2, 130 },
};

void ResetDATFiles_()
{
	LoadGameData(upgradesDat, "arr\\upgrades.dat");
	LoadGameData(weaponsDat_, "arr\\weapons.dat"); // TODO: dynamically allocate weaponsDat memory
	LoadGameData(techdataDat, "arr\\techdata.dat");
	LoadGameData(portdataDat, "arr\\portdata.dat");

	if (!loadGameFileHandle)
	{
		memset(&UnitAvailability, 1u, sizeof(UnitAvailability));
		memset(TechAvailableSC, 1u, sizeof(TechAvailableSC));
		memset(TechAvailableBW, 1u, sizeof(TechAvailableBW));
		memset(TechResearchSC, 0, sizeof(TechResearchSC));
		memset(TechResearchBW, 0, sizeof(TechResearchBW));
		memset(ResearchProgress, 0, sizeof(ResearchProgress));

		for (int i = 0; i < 12; i++)
		{
			memcpy(UpgradeMaxSC + i, Upgrade_MaxRepeats, sizeof(UpgradesSC));
			memcpy(UpgradeMaxBW[i].items, &Upgrade_MaxRepeats[46], sizeof(UpgradeMaxBW[i].items));
		}

		memset(UpgradeLevelSC, 0, sizeof(UpgradeLevelSC));
		memset(UpgradeLevelBW, 0, sizeof(UpgradeLevelBW));
		memset(UpgradeProgress, 0, sizeof(UpgradeProgress));
		memset(stru_596CD8, 0, sizeof(stru_596CD8));
		memset(next_scenario, 0, sizeof(next_scenario));

		dword_58F442 = 0;
		dword_58F446 = 0;
	}
}

FAIL_STUB_PATCH(ResetDATFiles);

int sub_4EEFD0_()
{
	memcpy(stru_59C6C0, palette, sizeof(stru_59C6C0));
	sub_49BB90();
	initMapData_();
	InitializePresetImageArrays();
	InitializeSpriteArray_();
	InitializeThingyArray();
	InitializeFlingyDat_();
	InitializeBulletArray();
	InitializeOrderArray();
	if (!loadGameFileHandle)
	{
		InitializeUnitCounts();
	}
	initializePsiFieldData();
	ResetDATFiles_();
	resetOrdersUnitsDAT();
	createUnitBuildingSpriteValidityArray();
	if (loadGameFileHandle || LoadMap_())
	{
		load_gluMinimap_();
		memcpy(palette, stru_59C6C0, sizeof(palette));
		return 1;
	}
	else
	{
		if (!dword_6D5BF8 && !outOfGame)
		{
			leaveGame(3);
			outOfGame = 1;
			doNetTBLError(0, 0, 0, 97);
			if (gwGameMode == GAME_RUN)
			{
				GameState = 0;
				gwNextGameMode = GAME_GLUES;
				if (!InReplay)
				{
					replay_header.ReplayFrames = ElapsedTimeFrames;
				}
			}
			nextLeaveGameMenu();
		}
		return 0;
	}
}

FAIL_STUB_PATCH(sub_4EEFD0);

void __fastcall MinimapPreviewProc_(dialog* a1, __int16 _timer_id)
{
	if (dword_5993AC != 1 && dword_5994DC && g_LocalNationID < 8u && (!map_download || IsDownloadComplete(map_download)))
	{
		int OriginalIsInGameLoop = IsInGameLoop;
		dword_5993AC = 1;
		IsInGameLoop = 1;
		sub_4EEFD0_();
		IsInGameLoop = OriginalIsInGameLoop;
		waitLoopCntd(1, a1);
	}
}

FAIL_STUB_PATCH(MinimapPreviewProc);

void CreateInitialTeamMeleeUnits_()
{
	int team_count = gameData.got_file_values.template_id != 15 ? gameData.got_file_values.team_mode : 2;

	for (int team_index = 0; team_index < team_count; team_index++)
	{
		for (int player_index = 7; player_index >= 0; player_index--)
		{
			if (Players[player_index].nTeam == team_index + 1 && Players[player_index].nType == PlayerType::PT_Human)
			{
				goto LABEL_15;
			}
		}
		for (int player_index = 7; player_index >= 0; player_index--)
		{
			if (Players[player_index].nTeam == team_index + 1 && (Players[player_index].nType == PlayerType::PT_Computer || Players[player_index].nType == PlayerType::PT_ComputerSlot))
			{
				goto LABEL_15;
			}
		}
		continue;

	LABEL_15:
		u8 a1 = byte_57F1CB[team_index + 1];
		u8 v6 = team_index * illegalTeamCheck();
		if (gameData.got_file_values.starting_units == StartingUnits::SU_WORKER_AND_CENTER)
		{
			CreateInitialMeleeBuildings_(byte_57F1C0[v6], a1);
			if (byte_57F1C0[v6] == RACE_Zerg)
			{
				CreateInitialOverlord(a1);
			}
		}
		for (int j = 0; j < 4; ++j)
		{
			int v9 = j % illegalTeamCheck();
			Race v10 = v6 + v9 < 8 ? byte_57F1C0[v6 + v9] : byte_57F1C0[v6 + 1];

			if (gameData.got_file_values.starting_units && gameData.got_file_values.starting_units <= StartingUnits::SU_WORKER_AND_CENTER)
			{
				CUnit* worker_unit = CreateUnit(GetWorkerType(v10), startPositions[a1].x, startPositions[a1].y, a1);
				if (worker_unit)
				{
					updateUnitStatsFinishBuilding(worker_unit);
					if (sub_49EC30(worker_unit))
					{
						updateUnitStrengthAndApplyDefaultOrders(worker_unit);
					}
				}
			}
		}
	}
}

FAIL_STUB_PATCH(CreateInitialTeamMeleeUnits);

signed int GameInit_()
{
	memset(Chat_GameText, 0, 2832u);
	*(_WORD *)&Chat_GameText[12].chars[216] = 0;
	resetTextAndLineData();
	dword_6D5BD0 = 0;
	initAIControllerData();
	initDetailFinder();
	InitRegionCaptains(0);
	load_AIScript_BIN();
	initAITownMgr();
	dword_59CC94 = 60;
	dword_59CC8C = 15;
	dword_59CC90 = 6;
	initMapData_();
	InitializePresetImageArrays();
	InitializeSpriteArray_();
	InitializeThingyArray();
	InitializeFlingyDat_();
	InitializeBulletArray();
	InitializeOrderArray();
	sub_4CB5B0(TILESET_PALETTE_RELATED[CurrentTileSet]->y);
	sub_41E450(sub_4BDB30, palette);
	sub_4C99C0();
	if (!loadGameFileHandle)
		InitializeUnitCounts();
	initializePsiFieldData();
	ResetDATFiles_();
	resetOrdersUnitsDAT();
	calculateUnitStrengths();
	createUnitBuildingSpriteValidityArray();
	if (loadGameFileHandle)
		return 1;
	if (LoadMap_())
	{
		if (!loadGameFileHandle)
		{
			if (LOBYTE(multiPlayerMode) && gameData.got_file_values.team_mode)
				CreateInitialTeamMeleeUnits_();
			else
				CreateInitialMeleeUnits_();
			InitialSetScreenToStartLocation();
			if (!loadGameFileHandle)
			{
				InitRegionCaptains(1);
				sub_446350();
			}
		}
		return 1;
	}
	if (!dword_6D5BF8 && !outOfGame)
	{
		leaveGame(3);
		outOfGame = 1;
		doNetTBLError(0, 0, 0, 97);
		if (gwGameMode == GamePosition::GAME_RUN)
		{
			GameState = 0;
			gwNextGameMode = GamePosition::GAME_GLUES;
			if (!InReplay)
			{
				replay_header.ReplayFrames = ElapsedTimeFrames;
			}
		}
		nextLeaveGameMenu();
	}
	return 0;
}

FAIL_STUB_PATCH(GameInit);
FAIL_STUB_PATCH(sub_4CD770);
FAIL_STUB_PATCH(sub_4A13B0);

GotFileValues* InitUseMapSettingsTemplate_()
{
	char ununsed[32];

	return readTemplate_("Use Map Settings(1)", ununsed, ununsed);
}

FAIL_STUB_PATCH(InitUseMapSettingsTemplate);

int sub_4CCAC0_(char* a1, MapChunks* a2)
{
	char buff[MAX_PATH];
	char v9[MAX_PATH];

	int v3 = a2 != 0 ? (a1 != 0 ? -(SStrLen(a1) != 0) : 0) : 0;
	SStrLen(a1);
	if (!v3)
	{
		SErrSetLastError(0x57u);
		return 0;
	}
	if (!sub_4CC350(v9, a1, (int)&a2->data7, MAX_PATH))
		return 0;
	int chk_size = 0;
	if (v9[0])
		_snprintf(buff, MAX_PATH, "%s\\%s", v9, "staredit\\scenario.chk");
	else
		SStrCopy(buff, "staredit\\scenario.chk", MAX_PATH);
	void* chk_data = fastFileRead_(&chk_size, 0, buff, 0, 1, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2060);
	if (chk_data)
	{
		int v7 = ReadLobbyChunks(chk_data, chk_size, a2);
		SMemFree(chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2077, 0);
		mapHandleDestroy();
		return v7;
	}
	if (mapArchiveHandle)
	{
		SFileCloseArchive(mapArchiveHandle);
		mapArchiveHandle = 0;
	}
	return 0;
}

FAIL_STUB_PATCH(sub_4CCAC0);

int __stdcall ReadMapData_(char* source, MapChunks* a4, int is_campaign)
{
	char* v8;
	const char* v13;

	CurrentMapFileName[0] = 0;
	if (!is_campaign)
		CampaignIndex = MD_none;
	memset(LobbyPlayers, 0, sizeof(PlayerInfo[12]));
	memset(playerForce, 0, 8);
	a4->data0 = 0;
	memset(a4->player_force, 0, sizeof(a4->player_force));
	memset(a4->tbl_index_force_name, 0, sizeof(a4->tbl_index_force_name));
	memset(a4->force_flags, 0, sizeof(a4->force_flags));
	a4->version = 0;
	a4->data7 = 0;
	if (InReplay)
	{
		if (!ReadLobbyChunks(scenarioChk, scenarioChkSize, a4))
		{
			return 0;
		}
		v8 = source;
	}
	else
	{
		v8 = source;
		if (!*source || !sub_4CCAC0_(source, a4))
			return 0;
	}
	int v9 = 12;
	do
	{
		--v9;
		LobbyPlayers[v9].dwPlayerID = v9;
		LobbyPlayers[v9].dwStormId = -1;
		if (LobbyPlayers[v9].nRace == Race::RACE_Select)
		{
			LobbyPlayers[v9].nRace = Race::RACE_Random;
			if (v9 < 8)
				playerForce[v9] = 1;
		}
		if (v9 >= 8)
		{
			LobbyPlayers[v9].nType = PlayerType::PT_NotUsed;
			LobbyPlayers[v9].nRace = Race::RACE_Zerg;
			LobbyPlayers[v9].nTeam = 0;
		}
	} while (v9 > 0);
	sub_4A91E0();
	sub_45AC10(a4->player_force);
	updatePlayerForce();
	strrchr(v8, '\\');
	SStrCopy(CurrentMapFileName, v8, MAX_PATH);
	if (!is_campaign)
	{
		CampaignIndex = MD_none;
	}
	__int16 v12 = LOWORD(a4->data0);
	dword_5994DC = 1;
	v13 = GetMapTblString(v12);

	SStrCopy(CurrentMapName, v13, sizeof(CurrentMapName));
	return 1;
}

FUNCTION_PATCH(ReadMapData, ReadMapData_);

bool ReadCampaignMapData_(MapChunks* map_chunks)
{
	return ReadMapData_(MapdataFilenames_[CampaignIndex], map_chunks, 1) != 0;
}

FAIL_STUB_PATCH(ReadCampaignMapData);

int LevelCheatInitGame_()
{
	if (!OpheliaEnabled)
	{
		return 1;
	}

	MapChunks map_chunks;

	OpheliaEnabled = 0;
	if (!playerName[0])
	{
		const char* v1 = GetNetworkTblString(72);
		SStrCopy(playerName, v1, sizeof(playerName));
	}
	if (CampaignIndex == MD_none)
	{
		char dest[MAX_PATH];
		SStrCopy(dest, CurrentMapFileName, sizeof(dest));
		if (!ReadMapData_(dest, &map_chunks, 0))
		{
			if (!outOfGame)
			{
				leaveGame(3);
				outOfGame = 1;
				doNetTBLError(0, 0, 0, 97);
				if (gwGameMode == GAME_RUN)
				{
					GameState = 0;
					gwNextGameMode = GAME_GLUES;
					if (!InReplay)
					{
						replay_header.ReplayFrames = ElapsedTimeFrames;
					}
				}
				nextLeaveGameMenu();
			}
			return 0;
		}

		for (int player_index = 0; player_index < 8; player_index++)
		{
			if (Players[player_index].nType == PT_OpenSlot && player_index != 0)
			{
				Players[player_index].nType = PT_ComputerSlot;
			}
		}
		goto LABEL_21;
	}
	if (ReadCampaignMapData_(&map_chunks))
	{
	LABEL_21:
		GameData v6;
		memset(&v6, 0, 140u);
		v6.got_file_values.unused3[4] = 0;
		SStrCopy(v6.player_name, playerName, sizeof(v6.player_name));
		SStrCopy(v6.map_name, CurrentMapName, sizeof(v6.map_name));
		v6.active_human_players = 1;
		v6.max_players = 1;
		v6.game_speed = registry_options.GameSpeed;
		GotFileValues* v4 = InitUseMapSettingsTemplate_();
		if (v4)
		{
			memcpy(&v6.got_file_values, v4, sizeof(v6.got_file_values));
			SMemFree(v4, "Starcraft\\SWAR\\lang\\game.cpp", 342, 0);
			if (sub_4DBE50())
			{
				isHost = 0;
				return CreateGame(&v6);
			}
			else
			{
				if (!outOfGame)
				{
					leaveGame(3);
					outOfGame = 1;
					doNetTBLError(0, 0, 0, 97);
					if (gwGameMode == GAME_RUN)
					{
						GameState = 0;
						gwNextGameMode = GAME_GLUES;
						if (!InReplay)
						{
							replay_header.ReplayFrames = ElapsedTimeFrames;
						}
					}
					nextLeaveGameMenu();
				}
				return 0;
			}
		}
		else
		{
			if (!outOfGame)
			{
				leaveGame(3);
				outOfGame = 1;
				doNetTBLError(0, 0, 0, 102);
				if (gwGameMode == GAME_RUN)
				{
					GameState = 0;
					gwNextGameMode = GAME_GLUES;
					if (!InReplay)
					{
						replay_header.ReplayFrames = ElapsedTimeFrames;
					}
				}
				nextLeaveGameMenu();
			}
			return 0;
		}
	}
	if (!outOfGame)
	{
		leaveGame(3);
		outOfGame = 1;
		doNetTBLError(0, 0, 0, 97);
		if (gwGameMode == GAME_RUN)
		{
			GameState = 0;
			gwNextGameMode = GAME_GLUES;
			if (!InReplay)
			{
				replay_header.ReplayFrames = ElapsedTimeFrames;
			}
		}
		nextLeaveGameMenu();
	}
	return 0;
}

FAIL_STUB_PATCH(LevelCheatInitGame);

signed int LoadGameCreate_()
{
	if (!loadGameFileHandle)
	{
		return 1;
	}
	if (sub_4CF5F0() && sub_4DBE50())
	{
		isHost = 0;
		return CreateGame(&gameData);
	}
	else
	{
		if (!outOfGame)
		{
			leaveGame(3);
			outOfGame = 1;
			doNetTBLError(0, 0, 0, 97);
			if (gwGameMode == GAME_RUN)
			{
				GameState = 0;
				gwNextGameMode = GAME_GLUES;
				if (!InReplay)
				{
					replay_header.ReplayFrames = ElapsedTimeFrames;
				}
			}
			nextLeaveGameMenu();
		}
		return 0;
	}
}

FAIL_STUB_PATCH(LoadGameCreate);

int RestartGame_()
{
	if (!next_campaign_mission)
	{
		return 1;
	}
	Race v1 = Players[g_LocalNationID].nRace;
	next_campaign_mission = 0;
	char dest[260];
	SStrCopy(dest, CurrentMapFileName, 0x104u);
	MapChunks a4;
	if (ReadMapData_(dest, &a4, 1))
	{
		if (sub_4DBE50())
		{
			Players[g_LocalNationID].nRace = v1;
			isHost = 0;
			return CreateGame(&gameData);
		}
		else
		{
			if (!outOfGame)
			{
				leaveGame(3);
				outOfGame = 1;
				doNetTBLError(0, 0, 0, 97);
				if (gwGameMode == GAME_RUN)
				{
					GameState = 0;
					gwNextGameMode = GAME_GLUES;
					if (!InReplay)
					{
						replay_header.ReplayFrames = ElapsedTimeFrames;
					}
				}
				nextLeaveGameMenu();
			}
			return 0;
		}
	}
	else
	{
		if (!outOfGame)
		{
			leaveGame(3);
			outOfGame = 1;
			doNetTBLError(0, 0, 0, 97);
			if (gwGameMode == GAME_RUN)
			{
				GameState = 0;
				gwNextGameMode = GAME_GLUES;
				if (!InReplay)
				{
					replay_header.ReplayFrames = ElapsedTimeFrames;
				}
			}
			nextLeaveGameMenu();
		}
		return 0;
	}
}

FAIL_STUB_PATCH(RestartGame);

int SinglePlayerMeleeInitGame_()
{
	if (multiPlayerMode)
	{
		return 0;
	}
	if (customSingleplayer[0] && (gameData.got_file_values.victory_conditions || gameData.got_file_values.starting_units || gameData.got_file_values.tournament_mode))
	{
		ClearMeleeCompSlots();
		Players[g_LocalNationID].nRace = selectedSingleplayerRace;
		for (int i = 0; i < 8; ++i)
		{
			if (single_player_opponent_races[i] != RACE_None)
			{
				append_melee_computer(single_player_opponent_races[i]);
			}
		}
	}
	return 1;
}

FAIL_STUB_PATCH(SinglePlayerMeleeInitGame);

void sub_4BCA80_(SfxData a1)
{
	if (direct_sound && registry_options.Sfx && a1 < SFX_MAX)
	{
		struct_5* v1 = &stru_6D1270[a1];
		v1->anonymous_1 = 81 - GetTickCount();
		if (v1->sound_buffer)
		{
			SFXData_Flags2[a1] |= 4;
		}
		else
		{
			v1->sound_buffer = sub_4BCA30(a1, &stru_6D1270[a1]);
			if (v1->sound_buffer)
			{
				v1->anonymous_2 = sub_4BB890(v1);

				if ((SFXData_Flags2[a1] & 1) == 0)
				{
					dword_6D59FC += v1->sound_buffer_bytes;
				}
				SFXData_Flags2[a1] |= 4;
				if (dword_6D59FC > value)
				{
					sub_4BBC00(dword_6D59FC - value);
				}
			}
		}
	}
}

FAIL_STUB_PATCH(sub_4BCA80);

void playsound_init_UI_(u16* a1)
{
	if (direct_sound == NULL)
	{
		return;
	}

	if (a1)
	{
		for (int i = 0; a1[i]; ++i)
		{
			SfxData v2 = (SfxData)a1[i];

			if (v2 < SFX_MAX && registry_options.Sfx && (SFXData_Flags2[v2] & 1))
			{
				sub_4BCA80_(v2);
			}
		}
	}
	else if (dword_6D6388)
	{
		for (int i = 0; dword_6D6388[i]; ++i)
		{
			SfxData v2 = (SfxData)dword_6D6388[i];

			if (v2 < SFX_MAX)
			{
				sub_4BB8E0(v2);
			}
		}
	}

	dword_6D6388 = a1;
}

FAIL_STUB_PATCH(playsound_init_UI);

int COMMON_SFX_COUNT = 113;
int TERRAN_SFX_COUNT = 299;
int PROTOSS_SFX_COUNT = 282;
int ZERG_SFX_COUNT = 213;

u16* off_513628_[] = {
	zerg_sfx_related,
	terran_sfx_related,
	protoss_sfx_related,
};

void LoadRaceSFX_(char a1)
{
	dword_64086C = 0;
	dword_640870 = 0;
	dword_640874 = 0;
	dword_640878 = 0;
	dword_64087C = 0;
	selectionSoundCounter = 0;

	if (dword_6D5BDC == 0)
	{
		for (int i = 0; i < COMMON_SFX_COUNT; ++i)
		{
			protoss_sfx_related[i] = i + 1;
			terran_sfx_related[i] = i + 1;
			zerg_sfx_related[i] = i + 1;
		}

		for (int i = 0; i < TERRAN_SFX_COUNT; ++i)
		{
			terran_sfx_related[COMMON_SFX_COUNT + i] = i + 174;
		}
		terran_sfx_related[COMMON_SFX_COUNT + TERRAN_SFX_COUNT] = 0;

		for (int i = 0; i < PROTOSS_SFX_COUNT; ++i)
		{
			protoss_sfx_related[COMMON_SFX_COUNT + i] = i + 473;
		}
		protoss_sfx_related[COMMON_SFX_COUNT + PROTOSS_SFX_COUNT] = 0;

		for (int i = 0; i < ZERG_SFX_COUNT; ++i)
		{
			zerg_sfx_related[COMMON_SFX_COUNT + i] = i + 755;
		}
		zerg_sfx_related[COMMON_SFX_COUNT + ZERG_SFX_COUNT] = 0;

		dword_6D5BDC = 1;
	}

	playsound_init_UI_(a1 ? off_513628_[consoleIndex] : 0);
}

FAIL_STUB_PATCH(LoadRaceSFX);

enum ExpandedMapData : u16
{
	EMD_none = 0x0,
	EMD_swtutorial = 0x1,
	EMD_swterran01 = 0x2,
	EMD_swterran02 = 0x3,
	EMD_swterran03 = 0x4,
	EMD_swterran04 = 0x5,
	EMD_swterran05 = 0x6,
	EMD_tutorial = 0x7,
	EMD_terran01 = 0x8,
	EMD_terran02 = 0x9,
	EMD_terran03 = 0xA,
	EMD_terran04 = 0xB,
	EMD_terran05 = 0xC,
	EMD_terran06 = 0xD,
	EMD_terran07 = 0xE,
	EMD_terran08 = 0xF,
	EMD_terran09 = 0x10,
	EMD_terran10 = 0x11,
	EMD_terran11 = 0x12,
	EMD_terran12 = 0x13,
	EMD_zerg01 = 0x14,
	EMD_zerg02 = 0x15,
	EMD_zerg03 = 0x16,
	EMD_zerg04 = 0x17,
	EMD_zerg05 = 0x18,
	EMD_zerg06 = 0x19,
	EMD_zerg07 = 0x1A,
	EMD_zerg08 = 0x1B,
	EMD_zerg09 = 0x1C,
	EMD_zerg10 = 0x1D,
	EMD_protoss01 = 0x1E,
	EMD_protoss02 = 0x1F,
	EMD_protoss03 = 0x20,
	EMD_protoss04 = 0x21,
	EMD_protoss05 = 0x22,
	EMD_protoss06 = 0x23,
	EMD_protoss07 = 0x24,
	EMD_protoss08 = 0x25,
	EMD_protoss09 = 0x26,
	EMD_protoss10 = 0x27,
	EMD_xprotoss01 = 0x28,
	EMD_xprotoss02 = 0x29,
	EMD_xprotoss03 = 0x2A,
	EMD_xprotoss04 = 0x2B,
	EMD_xprotoss05 = 0x2C,
	EMD_xprotoss06 = 0x2D,
	EMD_xprotoss07 = 0x2E,
	EMD_xprotoss08 = 0x2F,
	EMD_xterran01 = 0x30,
	EMD_xterran02 = 0x31,
	EMD_xterran03 = 0x32,
	EMD_xterran04 = 0x33,
	EMD_xterran05a = 0x34,
	EMD_xterran05b = 0x35,
	EMD_xterran06 = 0x36,
	EMD_xterran07 = 0x37,
	EMD_xterran08 = 0x38,
	EMD_xzerg01 = 0x39,
	EMD_xzerg02 = 0x3A,
	EMD_xzerg03 = 0x3B,
	EMD_xzerg04a = 0x3C,
	EMD_xzerg04b = 0x3D,
	EMD_xzerg04c = 0x3E,
	EMD_xzerg04d = 0x3F,
	EMD_xzerg04e = 0x40,
	EMD_xzerg04f = 0x41,
	EMD_xzerg05 = 0x42,
	EMD_xzerg06 = 0x43,
	EMD_xzerg07 = 0x44,
	EMD_xzerg08 = 0x45,
	EMD_xzerg09 = 0x46,
	EMD_xbonus = 0x47,
	EMD_xzerg10 = 0x48,
	EMD_Unknown = 0x49,
};

void LoadRaceUI_()
{
	LoadRaceSFX_(1);
	if (consoleIndex == Race::RACE_Zerg)
	{
		DlgGrp_Constructor(173, "Starcraft\\SWAR\\lang\\game.cpp", "dlgs\\zerg.grp", LoadGraphic);
		currentMusicId = MT_ZERG1;
	}
	else if (consoleIndex == Race::RACE_Terran)
	{
		DlgGrp_Constructor(178, "Starcraft\\SWAR\\lang\\game.cpp", "dlgs\\terran.grp", LoadGraphic);
		currentMusicId = MT_TERRAN1;
	}
	else if (consoleIndex == Race::RACE_Protoss)
	{
		DlgGrp_Constructor(183, "Starcraft\\SWAR\\lang\\game.cpp", "dlgs\\protoss.grp", LoadGraphic);
		currentMusicId = MT_PROTOSS1;
	}

	if (CampaignIndex)
	{
		int v2 = CampaignIndex > ExpandedMapData::EMD_protoss10 ? xCampaignFirstMission[consoleIndex] : campaignFirstMission[consoleIndex];
		int v1 = ((unsigned __int16)CampaignIndex - v2) % 3;
		currentMusicId += v1;
	}
	else
	{
		int v0 = consoleRaceSpecific == -1 ? rand() % 3 : consoleRaceSpecific;
		consoleRaceSpecific = v0 + 1;
		if (consoleRaceSpecific >= 3)
		{
			consoleRaceSpecific = 0;
		}
		else
		{
			currentMusicId += consoleRaceSpecific;
		}
	}
}

FAIL_STUB_PATCH(LoadRaceUI);

void setup_HUD_();

int LoadGameCore_()
{
	memset(PlayerSelection, 0, sizeof(PlayerSelection));
	memset(playersSelections, 0, sizeof(playersSelections));
	dword_59724C[0] = 0;
	dword_597250[0] = 0;
	dword_597254[0] = 0;
	setAlliance();
	if (!loadGameFileHandle || loadGameFull())
	{
		setup_HUD_();
		dword_51CE8C[0] = -GetTickCount();
		resetLastInputFrameCounts();
		memcpy(dword_596B7C, playersSelections[g_LocalHumanID], sizeof(dword_596B7C));
		memset(&playersSelections, 0, sizeof(playersSelections));
		return 1;
	}
	else
	{
		if (!outOfGame)
		{
			leaveGame(3);
			outOfGame = 1;
			doNetTBLError(0, 0, 0, 98);
			if (gwGameMode == GAME_RUN)
			{
				GameState = 0;
				gwNextGameMode = GAME_GLUES;
				if (!InReplay)
				{
					replay_header.ReplayFrames = ElapsedTimeFrames;
				}
			}
			nextLeaveGameMenu();
		}
		return 0;
	}
}

FAIL_STUB_PATCH(LoadGameCore);

GameActionDataBlock* allocateRepGameActionMemory_(GameActionDataBlock* a1)
{
	a1->net_record_size = 50000;
	a1->net_record_buffer = SMemAlloc(a1->net_record_size, "Starcraft\\SWAR\\lang\\net_record.cpp", 334, 0);
	a1->field4 = 0;
	a1->field6 = 0;
	a1->field7 = -1;
	a1->field1 = 1;
	a1->field8 = 0;
	return a1;
}

FAIL_STUB_PATCH(allocateRepGameActionMemory);

void __fastcall FreeGameActionData_(bool exit_code)
{
	if (replayData)
	{
		SMemFree(replayData->net_record_buffer, "Starcraft\\SWAR\\lang\\net_record.cpp", 348, 0);
		SMemFree(replayData, "delete", -1, 0);
		replayData = NULL;
	}
}

FAIL_STUB_PATCH(FreeGameActionData);

void __cdecl createNewGameActionDataBlock_()
{
	FreeGameActionData_(false);
	GameActionDataBlock* v1 = (GameActionDataBlock*)SMemAlloc(32, "new", -1, 0);
	replayData = v1 ? allocateRepGameActionMemory_(v1) : NULL;
	AppAddExit_(FreeGameActionData_);
}

FUNCTION_PATCH(createNewGameActionDataBlock, createNewGameActionDataBlock_);

signed int LoadGameInit_()
{
	stopMusic();
	if (InReplay)
	{
		if (!scenarioChk)
			LoadReplayFile(CurrentMapFileName, NULL);
		if (InReplay)
		{
			for (int i = 0; i < 8; i++)
			{
				playerForce[i] = replay_header.playerForce[i];
			}
		}
	}
	if (!loadGameFileHandle)
		ElapsedTimeFrames = 0;
	if (!LOBYTE(multiPlayerMode))
	{
		if (!LevelCheatInitGame_() || !LoadGameCreate_() || !RestartGame_() || !SinglePlayerMeleeInitGame_())
			return 0;
		if (InReplay)
		{
			initialSeed = replay_header.initial_seed;
		}
		else
		{
			initialSeed = time(0);
			replay_header.initial_seed = initialSeed;
		}
	}
	InitializeDialogScreenLayer_();
	ButtonPressSound = mouseOver_Loading_CB;
	LoadGameFonts();
	memset(randomCounts, 0, 0x400u);
	randomCountsTotal = 0;
	LastRandomNumber = initialSeed;
	srand(initialSeed);
	AllocAIPathPool();
	AppAddExit_(nullsub_1);
	if (!loadGameFileHandle)
		initializePlayerColours();
	BWFXN_RandomizePlayerRaces();
	if (InReplay)
		getReplPlayerStructs(Players);
	savePlayerSlotTypesAndRace();
	if (!InReplay)
	{
		if (loadGameFileHandle)
			InitializeLoadedGameSlots();
		else
			RandomizeSlotsForcesColors();
	}
	BWFXN_InitializePlayerConsole();
	initializeDefaultPlayerNames();
	memset(randomCounts, 0, 0x400u);
	randomCountsTotal = 0;
	LastRandomNumber = initialSeed;
	srand(initialSeed);
	LoadRaceUI_();
	hotkeyRemapping();
	if (!GameInit_())
		return 0;
	if (InReplay)
		getReplPlayerColors(factionsColorsOrdering);
	if (!loadGameFileHandle)
		sub_49B060();

	int v6;
	if (!InReplay
		&& CampaignIndex == MD_none
		&& !LoadFileArchiveToSBigBuf(CurrentMapFileName, &v6, 1, &mapArchiveHandle)
		&& !gameData.got_file_values.victory_conditions
		&& !gameData.got_file_values.starting_units
		&& !gameData.got_file_values.tournament_mode)
	{
		if (loadGameFileHandle)
		{
			BigPacketError(99, CurrentMapFileName, 0, 0, 1);
			return 0;
		}
		BigPacketError(97, 0, 0, 0, 1);
		return 0;
	}
	GameCheats = (CheatFlags)((int)GameCheats & (CHEAT_NoGlues | CHEAT_Ophelia | 0x8000000)); // TODO: fix the mess with the flags
	if (!LoadGameCore_())
		return 0;
	if (!mapStarted)
	{
		if (!chooseTRGTemplate())
		{
			BigPacketError(93, 0, 0, 0, 1);
			return 0;
		}
		if (!mapStarted)
		{
			*(_DWORD *)playerHasLeft = 0;
			lossType = 0;
			*(_DWORD *)&unkVictoryVariable = 0;
		}
	}
	sub_4B2DF0();
	if (!LOBYTE(multiPlayerMode))
		TickCounterInit();
	saveLoadSuccess = (unsigned __int8)mapStarted;
	elapstedTimeModifier = mapStarted != 0 ? savedElapsedSeconds : 0;
	SetGameSpeed_maybe(registry_options.GameSpeed, 0, 1u);
	if (InReplay)
	{
		copyPlayerStructsToReplayPlayerStructs(Players, &gameData);
		dword_6D5BF0 = 0;
		replayData->field1 = 0;
		replayData->field2 = 1;
		replayData->field8 = replayData->net_record_buffer;
		ReplayVision = 255;
		playerVisions = 255;
		replayShowEntireMap = 0;
		nextReplayCommandFrame = -1;
		playerExploredVisions = 65280;
	}
	else
	{
		createNewGameActionDataBlock_();
	}
	return 1;
}

FAIL_STUB_PATCH(sub_49CC10);
FAIL_STUB_PATCH(LoadGameInit);

void registerMenuFunctions_(FnInteract* functions, dialog* a2, int functions_size);
int gluLoadBINDlg_(dialog* a1, FnInteract fn_interact);

int loadOKBIN_(int a1, const char* message, HANDLE a3)
{
	dword_6D1244 = a1;
	dword_5128F0 = a1;

	size_t message_size = strlen(message) + 1;
	okcancel_message = (char*) SMemAlloc(message_size, "Starcraft\\SWAR\\lang\\okcancel.cpp", 163, 0);
	strcpy_s(okcancel_message, message_size, message);

	dword_6D1248 = a3;

	dialog* okcancel_bin = LoadDialog("ok.bin");
	registerMenuFunctions_(0, okcancel_bin, 0);
	return gluLoadBINDlg_(okcancel_bin, okcancel_Interact);
}

FAIL_STUB_PATCH(loadOKBIN);

int SaveReplay_(const char* a1, int a3)
{
	CHAR FileName[260];
	if (!getDirectoryPath(FileName, 0x104u, a1))
	{
		return 0;
	}

	if (a3 && !DeleteFileA(FileName) && GetFileAttributesA(FileName) != -1)
	{
		char buff[256];
		const char* var = GetNetworkTblString(2);
		_snprintf(buff, 0x100u, var, &byte_51BFB8);
		loadOKBIN_(1, buff, dword_6D0F2C);
		return -1;
	}

	FILE* v4 = fopen(FileName, "wb+");
	if (v4 == NULL)
	{
		return 0;
	}

	int v5;
	ReplayHeader a1a;

	replay_header.campaign_index = CampaignIndex;
	replay_header.field_46 = 1;
	replay_header.is_expansion = IsExpansion;
	memcpy(&a1a, &replay_header, sizeof(ReplayHeader));
	a3 = 'SRer';
	v5 = CompressWrite(&a3, 4, v4);
	if (v5)
	{
		v5 = CompressWrite(&a1a, 633, v4);
		if (v5)
		{
			v5 = WriteGameActions(v4, replayData);
			if (v5)
			{
				void* v6 = (void*)getFullMapChunk(CurrentMapFileName, &a3);
				if (v6)
				{
					v5 = CompressWrite(&a3, 4, v4);
					if (v5)
					{
						v5 = CompressWrite(v6, a3, v4);
					}
					SMemFree(v6, "Starcraft\\SWAR\\lang\\replay.cpp", 940, 0);
				}
				else
				{
					v5 = 0;
				}
			}
		}
	}
	fclose(v4);
	if (!v5)
	{
		DeleteFileA(FileName);
	}
	return v5 != 0;
}

FAIL_STUB_PATCH(SaveReplay);

void __cdecl freeChkFileMem_()
{
	if (scenarioChk)
	{
		SMemFree(scenarioChk, "Starcraft\\SWAR\\lang\\replay.cpp", 1106, 0);
		scenarioChk = nullptr;
	}
}

FUNCTION_PATCH(freeChkFileMem, freeChkFileMem_);

void AllocateSAI_Paths_()
{
	SAIPathing = (SAI_Paths*)SMemAlloc(sizeof(SAI_Paths), "Starcraft\\SWAR\\lang\\sai_PathCreate.cpp", 210, 0);
	memset(SAIPathing, 0, sizeof(SAI_Paths));
}

FAIL_STUB_PATCH(AllocateSAI_Paths);

void FreeSAI_Paths_()
{
	SMemFree(SAIPathing, "Starcraft\\SWAR\\lang\\sai_PathCreate.cpp", 226, 0);
	SAIPathing = NULL;
}

FAIL_STUB_PATCH(FreeSAI_Paths);

void destroyStatdata_()
{
	if (stardata_Dlg)
	{
		DestroyDialog(stardata_Dlg);
		stardata_Dlg = NULL;
	}

	if (dword_68C1FC)
	{
		SMemFree(dword_68C1FC, "Starcraft\\SWAR\\lang\\statdata.cpp", 1360, 0);
		dword_68C1FC = NULL;
	}

	if (dword_68C1F4)
	{
		SMemFree(dword_68C1F4, "Starcraft\\SWAR\\lang\\statdata.cpp", 1362, 0);
		dword_68C1F4 = NULL;
	}
}

FAIL_STUB_PATCH(destroyStatdata);

void free_cmdIcons_()
{
	if (current_dialog)
	{
		DestroyDialog(current_dialog);
		current_dialog = NULL;
	}

	if (dword_68C1C0)
	{
		SMemFree(dword_68C1C0, "Starcraft\\SWAR\\lang\\statcmd.cpp", 1107, 0);
		dword_68C1C0 = NULL;
	}

	if (cmdicons_grp)
	{
		SMemFree(cmdicons_grp, "Starcraft\\SWAR\\lang\\statcmd.cpp", 1109, 0);
		cmdicons_grp = NULL;
	}
}

FAIL_STUB_PATCH(free_cmdIcons);

void clearSelectionPortrait_()
{
	if (video)
	{
		SVidPlayEnd(video);
		video = NULL;
	}

	if (statport_Dlg)
	{
		DestroyDialog(statport_Dlg);
		statport_Dlg = NULL;
	}

	if (dword_6D5C9C)
	{
		SMemFree(dword_6D5C9C, "Starcraft\\SWAR\\lang\\statport.cpp", 882, 0);
		dword_6D5C9C = NULL;
	}
}

FAIL_STUB_PATCH(clearSelectionPortrait);

StatFlufDialog* statfluf_dialogs_[] = {
	statfluf_zerg,
	statfluf_terran,
	statfluf_protoss,
	statfluf_neutral,
};

void sub_4F4CF0_()
{
	for (StatFlufDialog* i = statfluf_dialogs_[statfluf_current_race]; i->position.left != -1; ++i)
	{
		if (i->dialog)
		{
			DestroyDialog(i->dialog);
			i->dialog = NULL;
		}
	}
}

FAIL_STUB_PATCH(sub_4F4CF0);

void destroyGameHUD_()
{
	if (minimap_Dlg)
	{
		DestroyDialog(minimap_Dlg);
		minimap_Dlg = NULL;
	}

	if (dword_59C1AC)
	{
		SMemFree(dword_59C1AC, "Starcraft\\SWAR\\lang\\minimap.cpp", 2065, 0);
		dword_59C1AC = NULL;
	}

	if (dword_68C140)
	{
		DestroyDialog(dword_68C140);
		dword_68C140 = NULL;
	}

	if (stat_f10_Dlg)
	{
		DestroyDialog(stat_f10_Dlg);
		stat_f10_Dlg = NULL;
	}

	if (dword_68C204)
	{
		SMemFree(dword_68C204, "Starcraft\\SWAR\\lang\\statwire.cpp", 367, 0);
		dword_68C204 = NULL;
	}

	destroyStatdata_();
	if (statres_Dlg)
	{
		DestroyDialog(statres_Dlg);
		statres_Dlg = NULL;
	}

	if (dword_68C238)
	{
		SMemFree(dword_68C238, "Starcraft\\SWAR\\lang\\statres.cpp", 507, 0);
		dword_68C238 = NULL;
	}

	if (statlb_Dlg)
	{
		DestroyDialog(statlb_Dlg);
		statlb_Dlg = NULL;
	}

	free_cmdIcons_();
	clearSelectionPortrait_();
	sub_4F4CF0_();
	refreshSelectionScreen_();

	if (StatTxtTbl.buffer)
	{
		SMemFree(StatTxtTbl.buffer, "Starcraft\\SWAR\\lang\\status.cpp", 236, 0);
		StatTxtTbl.buffer = NULL;
	}

	updateConsoleImage(&MainBltMask->pvoid0);
	ActivePortraitUnit = NULL;
	memset(ClientSelectionGroup, 0, sizeof(ClientSelectionGroup));
}

FAIL_STUB_PATCH(destroyGameHUD);

void DestroyMapData_()
{
	if (dword_6D5CD8)
	{
		SMemFree(dword_6D5CD8, "Starcraft\\SWAR\\lang\\repulse.cpp", 315, 0);
		dword_6D5CD8 = NULL;
	}
	if (VR4Data)
	{
		SMemFree(VR4Data, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 470, 0);
		VR4Data = NULL;
	}
	if (VX4Data)
	{
		SMemFree(VX4Data, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 471, 0);
		VX4Data = NULL;
	}
	if (TileSetMap)
	{
		SMemFree(TileSetMap, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 472, 0);
		TileSetMap = NULL;
	}
	if (MiniTileFlags)
	{
		SMemFree(MiniTileFlags, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 473, 0);
		MiniTileFlags = NULL;
	}
	if (active_tiles)
	{
		SMemFree(active_tiles, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 475, 0);
		active_tiles = NULL;
	}
	if (dword_5993A0)
	{
		SMemFree(dword_5993A0, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 476, 0);
		dword_5993A0 = NULL;
	}
	if (GameTerrainCache)
	{
		SMemFree(GameTerrainCache, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 477, 0);
		GameTerrainCache = NULL;
	}
	if (CellMap)
	{
		SMemFree(CellMap, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 478, 0);
		CellMap = NULL;
	}
	if (MapTileArray)
	{
		SMemFree(MapTileArray, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 479, 0);
		MapTileArray = NULL;
	}
	if (megatile_default_flags)
	{
		SMemFree(megatile_default_flags, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 480, 0);
		megatile_default_flags = NULL;
	}
	ZergCreepArray = NULL;

	DestroyFogSightData();
	if (spkHandle)
	{
		SMemFree(spkHandle, "Starcraft\\SWAR\\lang\\scroll.cpp", 550, 0);
		spkHandle = NULL;
	}
	TransDestroy();
	if (!dword_5993AC && dword_6D125C)
	{
		SMemFree(dword_6D125C, "Starcraft\\SWAR\\lang\\light.cpp", 121, 0);
		dword_6D125C = NULL;
	}

	for (int i = 1; i < _countof(colorShift); i++)
	{
		if (colorShift[i].data)
		{
			SMemFree(colorShift[i].data, "Starcraft\\SWAR\\lang\\light.cpp", 167, 0);
			colorShift[i].data = NULL;
		}
	}
}

FAIL_STUB_PATCH(DestroyMapData);

void CleanupFlingyDat_()
{
	// TODO: dynamically clean flingyDat memory
}

FAIL_STUB_PATCH(CleanupFlingyDat);

void CleanupSpritesDat_()
{
	// TODO: dynamically clean spritesDat memory
}

FAIL_STUB_PATCH(CleanupSpritesDat);

void DestroyGame_()
{
	if (isInGame)
	{
		countdownTimeTickCount_0 = GetTickCount();
		isInGame = 0;
	}
	if (multiPlayerMode && NetMode.as_number == 'BNET')
	{
		ReportGameResult();
	}
	leaveGame(0x40000001);
	if (gwGameMode == GamePosition::GAME_GLUES && glGluesMode == MenuPosition::GLUE_MAIN_MENU)
	{
		initializePlayerValues();
	}
	memset(cycle_colors, 0, sizeof(cycle_colors));
	if (!multiPlayerMode)
	{
		TickCountSomething(1);
	}
	if (GameMenuDlg)
	{
		DestroyDialog(GameMenuDlg);
		GameMenuDlg = 0;
		if (gwGameMode == GamePosition::GAME_RUN)
		{
			hAccTable = DlgAccelerator;
			input_procedures[EventNo::EVN_SYSCHAR] = AcceleratorTables;
		}
		byte_6D1214 = 0;
	}
	if (has_effects_scode_maybe)
	{
		BWFXN_GameEndTarget();
		has_effects_scode_maybe = 0;
	}
	layer* v0 = &ScreenLayers[3];
	do
	{
		v0->buffers = 0;
		++v0;
	} while ((int)v0 <= (int)&ScreenLayers[5]);
	Streamed_SFX_FullDestructor(&soundFXList);
	for (int i = 0; i < 8; i++)
	{
		TriggerNode_Destructor(stru_51A218.triggers + i);
	}
	struct_0* v2 = placement_boxes;
	do
	{
		if (v2->field_0)
		{
			SMemFree(v2->field_0, "Starcraft\\SWAR\\lang\\placebox.cpp", 578, 0);
			v2->field_0 = 0;
		}
		++v2;
	} while ((int)v2 < (int)playerReplayWatchers);
	if (is_placing_building)
	{
		refreshPlaceBuildingLocation();
	}
	if (byte_641694)
	{
		CancelTargetOrder();
	}
	SetInGameInputProcs();
	destroyGameHUD_();
	DestroyMapData_();
	if (dword_6BEE8C)
	{
		SMemFree(dword_6BEE8C, "Starcraft\\SWAR\\lang\\Sai_path.cpp", 792, 0);
		dword_6BEE8C = 0;
	}
	if (SAIPathing)
	{
		SaiContourHub* v4 = SAIPathing->contours;
		if (v4)
		{
			sai_contoursCreate_Cleanup(v4->contours);
			SMemFree(v4, "Starcraft\\SWAR\\lang\\sai_PathCreate.cpp", 333, 0);
			SAIPathing->contours = 0;
		}
		FreeSAI_Paths_();
	}
	if (aiscript_bin_data)
	{
		SMemFree(aiscript_bin_data, "Starcraft\\SWAR\\lang\\SAI_Scripts.cpp", 1546, 0);
		aiscript_bin_data = 0;
	}
	if (dword_68C108)
	{
		SMemFree(dword_68C108, "Starcraft\\SWAR\\lang\\SAI_Scripts.cpp", 1551, 0);
		dword_68C108 = 0;
	}
	freeAICaptains();

	// TODO: dynamically clean unitsDat memory
	// TODO: dynamically clean ordersDat memory
	CleanupFlingyDat_();
	CleanupSpritesDat_();

	stopSounds();
	stopMusic();
	stopSounds();
	LoadRaceSFX_(0);
	InitializeInputProcs();
	if (pylon_power_mask)
	{
		SMemFree(pylon_power_mask, "Starcraft\\SWAR\\lang\\CUnitProtoss.cpp", 102, 0);
	}
	pylon_power_mask = 0;
	if (MapStringTbl.buffer)
	{
		SMemFree(MapStringTbl.buffer, "Starcraft\\SWAR\\lang\\maphdr.cpp", 267, 0);
	}
	MapStringTbl.buffer = 0;
	if (mapArchiveHandle)
	{
		SFileCloseArchive(mapArchiveHandle);
		mapArchiveHandle = 0;
	}
	dword_6D5A60 = (int)InReplay;
	if (InReplay)
	{
		replayData->field2 = 0;
		freeChkFileMem_();
		InReplay = 0;
		game_id_hash = 0;
	}
	else
	{
		SaveReplay_("LastReplay", 1);
		if (league_maybe)
		{
			char a1[260];
			createLeagueFile(a1);
			SNetSendReplayPath(a1, game_id_hash, validation_replay_path[0] != 0 ? validation_replay_path : NULL);
		}
		game_id_hash = 0;
	}
}

FAIL_STUB_PATCH(DestroyGame);

void updateActiveTileInfo_()
{
	int v7 = 0;

	for (int y = 0; y < map_size.height; y++)
	{
		for (int x = 0; x < map_size.width; x++)
		{
			int i = y * map_size.width + x;

			if (InReplay ? (ReplayVision & ~active_tiles[i]) : (playerVisions & active_tiles[i]) == 0)
			{
				if (x - (unsigned int)MoveToTile.x >= 0x15 || y - (unsigned int)MoveToTile.y >= 0xE)
				{
					u16 v4 = TileSetMap[MapTileArray[i].group].megaTileRef[MapTileArray[i].tile];
					if (v4 != (CellMap[i] & 0x7FFF))
					{
						CellMap[i] = v4;
						if (MapTileArray[i].group == 1)
						{
							active_tiles[i] |= LOCAL_CREEP;
						}
						else
						{
							active_tiles[i] &= ~LOCAL_CREEP;
						}
						iterateDirectionalCreepData(BWFXN_CreepManagementCB, 0, x, y);
						v7 = 1;
					}
				}
			}
		}
	}

	if (v7)
	{
		HasMegatileUpdate = 1;
	}
}

FAIL_STUB_PATCH(updateActiveTileInfo);

void RemoveFoWCheat_()
{
	if (multiPlayerMode)
	{
		GameCheats &= ~(CHEAT_BlackSheepWall | CHEAT_WarAintWhatItUsedToBe);
	}
	if ((ScreenLayers[5].bits & 1) == 0)
	{
		ScreenLayers[5].bits |= 2;
	}
	if ((GameCheats & (CHEAT_BlackSheepWall | CHEAT_WarAintWhatItUsedToBe)) == 0)
	{
		RefreshLayer5();
		for (int i = 0; i < map_size.width * map_size.height; i++)
		{
			active_tiles[i] |= VISIBLE_PLAYER_8 | VISIBLE_PLAYER_7 | VISIBLE_PLAYER_6 | VISIBLE_PLAYER_5 | VISIBLE_PLAYER_4 | VISIBLE_PLAYER_3 | VISIBLE_PLAYER_2 | VISIBLE_PLAYER_1;
		}
	}
}

FAIL_STUB_PATCH(RemoveFoWCheat);

void UpdateImages_()
{
	CUnit* next_unit;
	for (CUnit* unit = dword_63FF34; unit; unit = next_unit)
	{
		next_unit = unit->next;
		UpdateImage(unit);
	}
}

FAIL_STUB_PATCH(UpdateImages);

void updateThingys_()
{
	CThingy* next_thingy;

	for (CThingy* thingy = first_lone_sprite; thingy; thingy = next_thingy)
	{
		next_thingy = thingy->next;
		sub_488020(thingy);
	}

	for (CThingy* thingy = dword_654868; thingy; thingy = next_thingy)
	{
		next_thingy = thingy->next;
		sub_488350(thingy);
	}
}

FAIL_STUB_PATCH(updateThingys);

void GameLoop_()
{
	SetInGameLoop(1);
	InitializeRandomizerInfo();
	AI_Loop();
	if (visionUpdateCount == 0)
	{
		visionUpdateCount = 100;
	}
	visionUpdated = visionUpdateCount-- == 1;
	if (visionUpdated)
	{
		updateActiveTileInfo_();
		RemoveFoWCheat_();
	}
	UpdateUnits();
	ImageDrawingBulletDrawing();
	UpdateImages_();
	updateThingys_();
	SetInGameLoop(0);
}

FAIL_STUB_PATCH(GameLoop);

unsigned int DoCycle_(CycleStruct* cycle_struct, unsigned int cycle_struct_index, unsigned int a3)
{
	int v4 = 0, v5 = 255;

	while (cycle_struct->speed)
	{
		if (cycle_struct_index >= 8 || cycle_struct->palette_entry_low > a3)
		{
			if (v4 >= v5)
			{
				int v10 = v4 - v5 + 1;
				PALETTEENTRY* v11 = &GamePalette[v5];
				if (Gamma != 100)
				{
					PALETTEENTRY a1[256];
					sub_41DC20(v11, &a1[v5], v10);
					v11 = &a1[v5];
				}
				SDrawUpdatePalette(v5, v10, v11, 1);
			}

			break;
		}

		if (cycle_struct->active && cycle_struct->wait-- == 1)
		{
			cycle_struct->wait = cycle_struct->speed;
			if (cycle_struct->advanced_cycle_data)
			{
				CyclePaletteAdvanced(cycle_struct_index);
			}
			CyclePalette(cycle_struct_index);
			v4 = max(v4, cycle_struct->palette_entry_high);
			v5 = min(cycle_struct->palette_entry_low, v5);
		}
		++cycle_struct;
		++cycle_struct_index;
	}

	return cycle_struct_index;
}

FAIL_STUB_PATCH(DoCycle);

void colorCycleInterval_()
{
	unsigned int v0 = DoCycle_(cycle_colors, 0, 0x80u);
	if (v0 < 8)
		DoCycle_(&cycle_colors[v0], v0, 0x100u);
}

FAIL_STUB_PATCH(colorCycleInterval);

void updateSelectedUnitData_()
{
	memcpy(ClientSelectionGroup, PlayerSelection, sizeof(ClientSelectionGroup));
	ClientSelectionCount = 0;
	ActivePortraitUnit = NULL;

	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (ClientSelectionGroup[i])
		{
			if (compareUnitRank(ClientSelectionGroup[i], ActivePortraitUnit))
			{
				ActivePortraitUnit = ClientSelectionGroup[i];
			}
			++ClientSelectionCount;
		}
	}

	if (ClientSelectionCount == 1)
	{
		ClientSelectionGroup[0] = ActivePortraitUnit;
		memset(&ClientSelectionGroup[1], 0, 0x2Cu);
	}
	updateButtonSet();
}

FAIL_STUB_PATCH(updateSelectedUnitData);

void updateCurrentButtonset_()
{
	u16 v0 = word_68C1C8;
	int v1 = CanUpdateCurrentButtonSet;
	if (CanUpdateCurrentButtonSet && (word_68C1C8 == 228 || InReplay))
	{
		updateButtonSet();
	}
	if (word_68C1C8 == 228)
	{
		v0 = word_68C1C4;
		if (word_68C1C4 == 228)
		{
			if (ActivePortraitUnit)
			{
				v0 = ActivePortraitUnit->currentButtonSet;
			}
			else
			{
				goto LABEL_14;
			}
		}
	}

	if (word_68C14C != v0)
	{
		word_68C14C = v0;
		CanUpdateCurrentButtonSet = 1;
	}

LABEL_14:
	if (v1 || CanUpdateCurrentButtonSet)
	{
		sub_4591D0();
		sub_459770();
		if (ActivePortraitUnit)
		{
			Order v6 = ActivePortraitUnit->orderID;
			Order v3 = ActivePortraitUnit->subUnit ? ActivePortraitUnit->subUnit->orderID : (Order) -1;
			sub_458D50();
			byte_68C1E4 = v6;
			byte_68C1B8 = v3;
			sub_458D50();
		}
		if (InReplay)
		{
			replayStatBtns(current_dialog);
		}
		CanUpdateCurrentButtonSet = 0;
	}
	else if (ActivePortraitUnit)
	{
		Order v7 = ActivePortraitUnit->orderID;
		Order v5 = ActivePortraitUnit->subUnit ? ActivePortraitUnit->subUnit->orderID : (Order) -1;
		if (byte_68C1E4 != ActivePortraitUnit->orderID || byte_68C1B8 != v5)
		{
			sub_458D50();
			byte_68C1E4 = v7;
			byte_68C1B8 = v5;
			sub_458D50();
		}
	}
}

FUNCTION_PATCH(updateCurrentButtonset, updateCurrentButtonset_);

void __fastcall UnitStatAct_Dropship_(dialog* dlg)
{
	if ((InReplay || ActivePortraitUnit->playerID == g_LocalNationID)
		&& (ActivePortraitUnit->statusFlags & IsHallucination) == 0
		&& (sub_424FC0(), isUnitLoaded(ActivePortraitUnit))
		&& (InReplay || ActivePortraitUnit->playerID == g_LocalNationID))
	{
		if (statusScreenFunc != 11)
		{
			sub_457310(dlg);
			statusScreenFunc = 11;
		}
		setSpellSpecialBtnGraphic(dlg);
		setTextStr(dlg);
		getActivePortraitUnitName(dlg);
	}
	else if (ActivePortraitUnit->statusFlags & 2)
	{
		UnitStatAct_Building(dlg);
	}
	else
	{
		UnitStatAct_Standard(dlg);
	}
}

FAIL_STUB_PATCH(UnitStatAct_Dropship);

void __fastcall UnitStatAct_Overlord_(dialog* dlg)
{
	if ((InReplay || ActivePortraitUnit->playerID == g_LocalNationID) && (ActivePortraitUnit->statusFlags & IsHallucination) == 0)
	{
		if (isUnitLoaded(ActivePortraitUnit))
		{
			UnitStatAct_Dropship_(dlg);
		}
		else
		{
			sub_427540(dlg);
			setTextStr(dlg);
			SetUnitStatusStrText(dlg);
			AddTextToDialog(dlg, -5, UnitGetName(ActivePortraitUnit));
		}
	}
	else
	{
		UnitStatAct_Standard(dlg);
	}
}

FAIL_STUB_PATCH(UnitStatAct_Overlord);

UnitStat unit_stats_[] =
{
	{ 0, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 1, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 2, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 3, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 4, UnitStatCond_Never, UnitStatAct_Default },
	{ 5, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 6, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 7, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 8, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 9, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x0A, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x0B, UnitStatCond_Dropship, UnitStatAct_Dropship_ },
	{ 0x0C, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x0D, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{ 0x0E, UnitStatCond_Never, UnitStatAct_Default },
	{ 0x0F, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x10, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x11, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x12, UnitStatCond_Never, UnitStatAct_Default },
	{ 0x13, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x14, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x15, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x16, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x17, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x18, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x19, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1A, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1B, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1C, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1D, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1E, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1F, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x20, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x21, UnitStatCond_Never,UnitStatAct_Default },
	{ 0x22, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x23, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x24, UnitStatCond_Egg, UnitStatAct_Egg },
	{ 0x25, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x26, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x27, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x28, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x29, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x2A, UnitStatCond_overlord, UnitStatAct_Overlord_ },
	{ 0x2B, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x2C, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x2D, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x2E, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x2F, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x30, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x31, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x32, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x33, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x34, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x35, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x36, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x37, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x38, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x39, UnitStatCond_overlord, UnitStatAct_Overlord_ },
	{ 0x3A, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x3B, UnitStatCond_Egg, UnitStatAct_Egg },
	{ 0x3C, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x3D, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x3E, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x3F, UnitStatCond_Archon, UnitStatAct_Archon },
	{ 0x40, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x41, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x42, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x43, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x44, UnitStatCond_Archon, UnitStatAct_Archon },
	{ 0x45, UnitStatCond_Dropship, UnitStatAct_Dropship_ },
	{ 0x46, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x47, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x48, UnitStatCond_CarrierReaver, UnitStatAct_CarrierReaver },
	{ 0x49, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4A, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4B, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4C, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4D, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4E, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4F, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x50, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x51, UnitStatCond_CarrierReaver, UnitStatAct_CarrierReaver },
	{ 0x52, UnitStatCond_CarrierReaver, UnitStatAct_CarrierReaver },
	{ 0x53, UnitStatCond_CarrierReaver, UnitStatAct_CarrierReaver },
	{ 0x54, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x55, UnitStatCond_Standard, UnitStatAct_Default },
	{ 0x56, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x57, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x58, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x59, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x5A, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0, UnitStatCond_Never, UnitStatAct_Default },
	{ 0, UnitStatCond_Never, UnitStatAct_Default },
	{ 0x5D, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x5E, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x5F, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x60, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x61, UnitStatCond_Egg, UnitStatAct_Egg },
	{ 0x62, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x63, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x64, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x65, UnitStatCond_Standard, UnitStatAct_Default },
	{ 0x66, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x67, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x68, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x69, UnitStatCond_Never, UnitStatAct_Default },
	{ 0x6A, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x6B, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x6C, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x6D, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x6E, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x6F, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x70, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x71, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x72, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x73, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x74, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x75, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x76, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x77, UnitStatCond_Standard, UnitStatAct_Default },
	{ 0x78, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x79, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x7A, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x7B, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x7C, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x7D, UnitStatCond_Dropship, UnitStatAct_Dropship_ },
	{ 0x7E, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x7F, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x80, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{ 0x81, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{ 0x82, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x83, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x84, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x85, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x86, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x87, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x88, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x89, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8A, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8B, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8C, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8D, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8E, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8F, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x90, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x91, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x92, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x93, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x94, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x95, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x96, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x97, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x98, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x99, UnitStatCond_Standard, UnitStatAct_Default },
	{ 0x9A, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x9B, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x9C, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x9D, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x9E, UnitStatCond_Building, UnitStatAct_Default },
	{ 0x9F, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A0, UnitStatCond_Building, UnitStatAct_Building },
	{ 0, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A2, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A3, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A4, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A5, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A6, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A7, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A8, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A9, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AA, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AB, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AC, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AD, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AE, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AF, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B0, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B1, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B2, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B3, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B4, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B5, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B6, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B7, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B8, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B9, UnitStatCond_Building, UnitStatAct_Building },
	{0x0BA, UnitStatCond_Building, UnitStatAct_Building },
	{0x0BB, UnitStatCond_Building, UnitStatAct_Building },
	{0x0BC, UnitStatCond_Building, UnitStatAct_Building },
	{0x0BD, UnitStatCond_Building, UnitStatAct_Building },
	{0x0BE, UnitStatCond_Building, UnitStatAct_Building },
	{ 0, UnitStatCond_Never, UnitStatAct_Default },
	{ 0, UnitStatCond_Never, UnitStatAct_Default },
	{ 0, UnitStatCond_Never, UnitStatAct_Default },
	{0x0C2, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C3, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C4, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C5, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C6, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C7, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C8, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C9, UnitStatCond_Building, UnitStatAct_Building },
	{ 0, UnitStatCond_Building, UnitStatAct_Building },
	{0x0CB, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0CC, UnitStatCond_Never, UnitStatAct_Default },
	{0x0CD, UnitStatCond_Never, UnitStatAct_Default },
	{0x0CE, UnitStatCond_Never, UnitStatAct_Default },
	{0x0CF, UnitStatCond_Never, UnitStatAct_Default },
	{0x0D0, UnitStatCond_Never, UnitStatAct_Default },
	{0x0D1, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D2, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D3, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D4, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D5, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{ 0, UnitStatCond_Building, UnitStatAct_Building },
	{0x0D7, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D8, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D9, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DA, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DB, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DC, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DD, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DE, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DF, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0E0, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0E1, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0E2, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0E3, UnitStatCond_Powerup, UnitStatAct_Powerup },
};

void sub_458120_()
{
	if (ActivePortraitUnit)
	{
		if (ClientSelectionCount != 1)
		{
			if (CanUpdateStatDataDialog || isSelGroupUpdated())
			{
				sub_425960(stardata_Dlg);
				sub_457FE0();
			}
		}
		else if (CanUpdateStatDataDialog || unit_stats_[ActivePortraitUnit->unitType].condition())
		{
			unit_stats_[ActivePortraitUnit->unitType].action(stardata_Dlg);
			sub_4568B0();
			sub_457FE0();
		}
	}
	else if (CanUpdateStatDataDialog)
	{
		for (dialog* v1 = stardata_Dlg->wCtrlType != DialogType::cDLG ? stardata_Dlg : stardata_Dlg->fields.dlg.pFirstChild; v1; v1 = v1->pNext)
		{
			HideDialog(v1);
		}
		statusScreenFunc = 0;
	}

	CanUpdateStatDataDialog = 0;
}

FAIL_STUB_PATCH(sub_458120);

void sub_4C3B10_()
{
	if (byte_59723C)
	{
		updateSelectedUnitData_();
		byte_59723C = 0;
	}
	updateSelectedUnitPortrait();
	updateCurrentButtonset_();
	sub_458120_();
}

FAIL_STUB_PATCH(sub_4C3B10);

void sub_4D93B0_()
{
	if (dword_51BFA8 && ColorCycle && !IS_GAME_PAUSED)
	{
		DWORD tick_count = GetTickCount();
		if (tick_count >= dword_6D6374 + 10)
		{
			dword_6D6374 = tick_count;
			colorCycleInterval_();
		}
	}
}

FAIL_STUB_PATCH(sub_4D93B0);

void updateHUDInformation_()
{
	sub_4C3B10_();
	refreshScreen();
	sub_4D93B0_();
	refreshGameTextIfCounterActive();
}

FAIL_STUB_PATCH(updateHUDInformation);

void DoGameLoop_()
{
	GameLoop_();
	updateHUDInformation_();
	GameLoop_();
	updateHUDInformation_();
}

FAIL_STUB_PATCH(DoGameLoop);

void PollInput_()
{
	if (!dword_51BFA8 || IS_GAME_PAUSED && !multiPlayerMode)
	{
		return;
	}
	if (byte_658AC0)
	{
		DisableDragSelect();
	}
	else if (byte_66FF5C || byte_6D1214)
	{
		return;
	}

	int scroll_speed;
	int a1;
	int a2;
	getScreenMoveState(&a1, &a2);
	if (a2 || a1)
	{
		dword_6D5C1C = 1;
		scroll_speed = multiPlayerMode ? registry_options.MKeyScrollSpeed : registry_options.KeyScrollSpeed;
	}
	else
	{
		getScrollCursorType(&a2, &a1);
		scroll_speed = multiPlayerMode ? registry_options.MMouseScrollSpeed : registry_options.MouseScrollSpeed;
	}

	if (a2 == 0 && a1 == 0)
	{
		dword_6D63DC = 0;
	}
	else if ((unsigned int)dword_6D63DC < 6)
	{
		dword_6D63DC += 1;
	}

	if (byte_658AC0)
	{
		scroll_speed = GetScrollSpeed(&dword_6D63DC);
	}
	if (a2 || a1)
	{
		dword_6D7574 = a2;
		dword_6D7570 = a1;
	}

	if (dword_6D63DC)
	{
		int v5 = 0;
		int v6 = 0;

		if (dword_6D7570 < 0)
		{
			v5 = moveToYScrDecrease(scrollSpeeds.scroll[7 * scroll_speed + dword_6D63DC]);
		}
		else if (dword_6D7570 > 0)
		{
			v5 = moveToYScrIncrease(scrollSpeeds.scroll[7 * scroll_speed + dword_6D63DC]);
		}

		if (dword_6D7574 < 0)
		{
			v6 = moveToXScrDecrease(scrollSpeeds.scroll[7 * scroll_speed + dword_6D63DC]);
		}
		else if (dword_6D7574 > 0)
		{
			v6 = moveToXScrIncrease(scrollSpeeds.scroll[7 * scroll_speed + dword_6D63DC]);
		}

		if (v6 || v5)
		{
			RefreshLayer5();
		}
	}

	if (byte_658AC0 && MoveToX == dword_658ABC && MoveToY == dword_658AA0 && dword_658AA4 != NULL)
	{
		dword_658AA4();
		dword_658AA4 = 0;
	}
}

FAIL_STUB_PATCH(PollInput);

int BWFXN_IsPaused_()
{
	return IS_GAME_PAUSED;
}

FAIL_STUB_PATCH(BWFXN_IsPaused);

void RefreshUnit_(CUnit* unit)
{
	if ((Unit_PrototypeFlags[unit->unitType] & Subunit) == 0 && (unit->sprite->flags & 0x20) == 0)
	{
		RefreshSprite(unit->sprite, byte_581D6A[unit->playerID]);
	}
}

FAIL_STUB_PATCH(RefreshUnit);

void RefreshAllUnits_()
{
	for (CUnit* unit = UnitNodeList_VisibleUnit_First; unit; unit = unit->next)
	{
		RefreshUnit_(unit);
	}
}

FAIL_STUB_PATCH(RefreshAllUnits);

int GameLoopWaitSendTurn_(int* a1)
{
	*a1 = 0;
	if (--FramesUntilNextTurn)
	{
		return 1;
	}
	else if (gameLoopTurns())
	{
		if (byte_51CE9D)
		{
			dword_6D6370 = GetTickCount() - dword_6D6370;
			if ((unsigned int)dword_6D6370 >= 0x64)
			{
				dword_51CE94 = ((LatencyFrames[registry_options.GameSpeed] * GameSpeedModifiers.gameSpeedModifiers[registry_options.GameSpeed]) >> 1) + GetTickCount();
			}
			else
			{
				dword_51CE94 += GameSpeedModifiers.gameSpeedModifiers[registry_options.GameSpeed] >> 1;
			}
		}
		byte_51CE9D = 0;
		FramesUntilNextTurn = LatencyFrames[registry_options.GameSpeed];
		if (multiPlayerMode)
		{
			*a1 = 1;
			EnableVisibilityHashUpdate();
			minimapPreviewUpdateState_();
			saveMinimapCounts();
		}
		return 1;
	}
	else
	{
		FramesUntilNextTurn = 1;
		if (!byte_51CE9D)
		{
			byte_51CE9D = 1;
			dword_6D6370 = GetTickCount();
		}
		return 0;
	}
}

FAIL_STUB_PATCH(GameLoopWaitSendTurn);

void replayFrameComputation_()
{
	int v0 = ElapsedTimeFrames * dword_4FF90C[replay_header.field_3A];
	int v1 = 0;
	if (v0 / 1000 != dword_50E05C)
	{
		dword_50E05C = v0 / 1000;
		v1 = 1;
	}
	int v2 = 0;
	if (is_replay_paused)
	{
		DWORD v3 = GetTickCount();
		if (v3 - dword_6D11B8 > 600)
		{
			dword_6D11B8 = v3;
			v2 = 1;
			dword_6D11B4 = dword_6D11B4 == 0;
		}
	}
	if (v1 || v2)
	{
		CanUpdateCurrentButtonSet = 1;
		CanUpdateSelectedUnitPortrait = 1;
		CanUpdateStatDataDialog = 1;
		ctrl_under_mouse = 0;
		ctrl_under_mouse_val = 0;
	}
}

FAIL_STUB_PATCH(replayFrameComputation);

void BWFXN_ExecuteGameTriggers_(signed int dwMillisecondsPerFrame);

void GameLoop_State_()
{
	DWORD v10 = GetTickCount() + 2000;
	int v9 = 0;
	dword_6D11F0 = 0;
	for (int i = 0; i < FrameSkip; i++)
	{
		if (InReplay)
		{
			replayLoop();
		}

		int v5;
		if (!GameLoopWaitSendTurn_(&v5))
		{
			dword_6D11F0 = 1;
			break;
		}
		if (InReplay && is_replay_paused)
		{
			replayFrameComputation_();
			dword_6D11F0 = 2;
			break;
		}
		if (!GameState)
		{
			dword_6D11F0 = 4;
			break;
		}
		if (dword_51BFA8 || multiPlayerMode)
		{
			ScreenLayers[5].bits |= 2;
			if (BWFXN_IsPaused_())
			{
				RefreshAllUnits_();
			}
			else
			{
				++ElapsedTimeFrames;
				++v9;
				GameLoop_();
			}
			SetInGameLoop(1);
			BWFXN_ExecuteGameTriggers_(GameSpeedModifiers.gameSpeedModifiers[registry_options.GameSpeed]);
			SetInGameLoop(0);
			if (InReplay)
			{
				replayFrameComputation_();
			}
		}
		dword_51CE94 += GameSpeedModifiers.gameSpeedModifiers[registry_options.GameSpeed];
		DWORD v6 = GetTickCount();
		if (v6 < dword_51CE94)
		{
			dword_6D11F0 = 5;
			break;
		}
		if (v10 < v6)
		{
			dword_6D11F0 = 6;
			break;
		}
		if (v5 && (!InReplay || !dword_6D11E8))
		{
			dword_6D11F0 = 7;
			break;
		}
	}
	IsRunning = v9;
}

FAIL_STUB_PATCH(GameLoop_State);

void GameLoop_Top_(MenuPosition a1)
{
	FramesUntilNextTurn = 1;
	byte_51CE9D = 0;
	dword_51CE94 = GetTickCount();
	bool v2 = false;
	while (GameState)
	{
		BWFXN_videoLoop_(3);
		DWORD v1 = GetTickCount();
		if (!byte_51CE9D && abs(int(dword_51CE94 - v1)) > GameSpeedModifiers.altSpeedModifiers[registry_options.GameSpeed])
		{
			dword_51CE94 = v1;
		}
		BWFXN_NextFrameHelperFunctionTarget();
		if (v1 + dword_51CE8C[0] > 0xA)
		{
			dword_51CE8C[0] = -v1;
			PollInput_();
			v2 = true;
		}
		RecvMessage();
		LeagueChatFilter();
		if (v1 >= dword_51CE94)
		{
			GameLoop_State_();
			v2 = true;
		}
		updateHUDInformation_();
		if (dword_5968EC || v2)
		{
			dword_5968EC = 0;
			v2 = false;
			BWFXN_RedrawTarget_();
		}
	}
}

FAIL_STUB_PATCH(GameLoop_Top);

void stopAllSound_(void)
{
	stopSounds();
	stopMusic();
}

FAIL_STUB_PATCH(stopAllSound);

GamePosition BeginGame_(MenuPosition a1)
{
	visionUpdateCount = 1;
	DLGMusicFade_((MusicTrack) currentMusicId);
	SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	GameState = 1;
	TickCountSomething(0);
	DoGameLoop_();
	RefreshLayer5();
	setCursorType_(getCursorType());
	cursorRefresh();
	if (!multiPlayerMode && !getMapStartStatus() && !InReplay && (registry_options.field_18 & 0x100) != 0)
	{
		loadTips_BINDLG(1);
	}
	SetMapStartStatus();
	SetCurrentPaletteInfo(palette, 0x100u, 0);
	get_tFontGam_PCX();
	TitlePaletteUpdate_(3);
	if (multiPlayerMode)
	{
		registry_options.GameSpeed = (unsigned __int8)gameData.game_speed;
	}
	else if (CampaignIndex == MD_none)
	{
		registry_options.GameSpeed = 4;
	}
	newGame(1);
	BWFXN_videoLoop_(0);
	loseSightSelection();
	turn_counter = 0;
	GameKeepAlive();
	while (GameState && !gameLoopTurns())
	{
		BWFXN_RedrawTarget_();
	}
	GameLoop_Top_(a1);
	newGame(0);
	stopAllSound_();
	sub_41E9E0(3);
	get_tFontGam_PCX_0();
	RefreshCursor_0();
	return gwNextGameMode;
}

FAIL_STUB_PATCH(BeginGame);

void GameRun_(MenuPosition a1)
{
	IsInGameLoop = 1;
	int v1 = LoadGameInit_();
	IsInGameLoop = 0;
	if (!InReplay)
	{
		if (!gameData.width)
			gameData.width = map_size.width;
		if (!gameData.height)
			gameData.height = map_size.height;
		if (!gameData.tileset)
			gameData.tileset = CurrentTileSet;
		if (!gameData.game_type)
			gameData.game_type = (GameType) gameData.got_file_values.template_id;
		if (!gameData.game_type_unk)
			gameData.game_type_unk = gameData.got_file_values.unused1;
		if (!gameData.game_type_param)
			gameData.game_type_param = gameData.got_file_values.variation_id;
		SetReplayData(&gameData, Players, factionsColorsOrdering);
	}
	if (loadGameFileHandle)
	{
		fclose(loadGameFileHandle);
		loadGameFileHandle = 0;
	}
	if (v1)
	{
		GamePosition next_game_position = BeginGame_(a1);
		DestroyGame_();
		gwGameMode = next_game_position;
	}
	else
	{
		DestroyGame_();
		gwGameMode = GAME_GLUES;
	}
}

FAIL_STUB_PATCH(GameRun);

bool statBtn_dlg_CharPress_(dlgEvent* evt)
{
	if (evt->wUnk_0x0A & 0x100)
	{
		return 0;
	}
	char v3 = tolower(evt->wVirtKey);
	if (!DLG_IterateChildren(current_dialog->fields.dlg.pFirstChild, (int(__fastcall*)(_DWORD, _DWORD))sub_4588C0, (int)&v3))
	{
		return 0;
	}

	dlgEvent v2;
	v2.wNo = EVN_USER;
	v2.dwUser = USER_HOTKEY;
	v2.wSelection = 0;
	v2.wUnk_0x06 = 0;
	v2.cursor.x = Mouse.x;
	v2.cursor.y = Mouse.y;
	return interrupting_child->pfcnInteract(interrupting_child, &v2);
}

FAIL_STUB_PATCH(statBtn_dlg_CharPress);

void sub_458E70_(dialog* a1)
{
	a1->wUser = -1;
	a1->pfcnUpdate = statbtn_Btn_Update;
	HideDialog(a1);
}

FAIL_STUB_PATCH(sub_458E70);

void sub_458BB0_(dialog* dlg)
{
	ButtonOrder* order = (ButtonOrder*)dlg->lUser;
	order->action(order->action_variable, is_keycode_used[VK_SHIFT]);
}

FAIL_STUB_PATCH(sub_458BB0);

void statflufDlgUpdate_(dialog* dlg)
{
	dlg->lFlags |= CTRL_TRANSPARENT;
	BINDLG_BlitSurface(dlg);
	if ((dlg->lFlags & CTRL_UPDATE) == 0)
	{
		dlg->lFlags |= CTRL_UPDATE;
		updateDialog(dlg);
	}
}

FAIL_STUB_PATCH(statflufDlgUpdate);

bool __fastcall statfluf_DLG_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
	case EVN_MOUSEMOVE:
	case EVN_LBUTTONDOWN:
	case EVN_LBUTTONDBLCLK:
	case EVN_RBUTTONDOWN:
	case EVN_RBUTTONDBLCLK:
	case EVN_CHAR:
		return 0;
	case EVN_USER:
		if (evt->dwUser == EventUser::USER_CREATE)
		{
			statflufDlgUpdate_(dlg);
		}
		else if (evt->dwUser == EventUser::USER_MOUSEMOVE)
		{
			return 0;
		}
		break;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(statfluf_DLG_Interact);

void load_statfluf_BIN_()
{
	statfluf_current_race = InReplay ? Race::RACE_Other : consoleIndex;
	StatFlufDialog* dlgs = statfluf_dialogs_[statfluf_current_race];
	for (int i = 0; dlgs[i].position.left != -1; i++)
	{
		dlgs[i].dialog = LoadDialog("rez\\statfluf.bin"); // The top decoratios of the main (bottom-of-the-screen) in-game UI
		dlgs[i].dialog->rct.left = dlgs[i].position.left;
		dlgs[i].dialog->rct.top = dlgs[i].position.top;
		dlgs[i].dialog->rct.right = dlgs[i].position.right - 1;
		dlgs[i].dialog->rct.bottom = dlgs[i].position.bottom - 1;
		dlgs[i].dialog->fields.dlg.dstBits_wid = dlgs[i].position.right;
		dlgs[i].dialog->fields.dlg.dstBits_ht = dlgs[i].position.bottom;
		InitializeDialog_(dlgs[i].dialog, statfluf_DLG_Interact_);
	}
}

FAIL_STUB_PATCH(load_statfluf_BIN);

bool __fastcall statport_Dlg_Interact_(dialog* dlg, struct dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
	case EVN_MOUSEMOVE:
		return 0;
	case EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			statport_Buttonpress(dlg);
			break;
		case USER_DESTROY:
			statport_alloc();
			break;
		case USER_MOUSEMOVE:
			return 1;
		}
		break;
	case EVN_CHAR:
		return sub_45E9F0(evt);
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(statport_Dlg_Interact);

void loadPortdata_BINDLG_()
{
	unsigned __int16* v4 = (unsigned short*) fastFileRead_(0, 0, "arr\\portdata.tbl", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	dword_6D5C9C = v4;
	for (int i = 0; i < 110; i++)
	{
		const char* v9;
		if (LOWORD(dword_655C58[i]))
		{
			v9 = LOWORD(dword_655C58[i]) - 1 < *v4 ? (char*)v4 + v4[LOWORD(dword_655C58[i])] : "";
		}
		else
		{
			v9 = 0;
		}
		dword_655C58[i] = (int)v9;

		if (LOWORD(dword_655E80[i]))
		{
			v9 = LOWORD(dword_655E80[i]) - 1 < *v4 ? (char*)v4 + v4[LOWORD(dword_655E80[i])] : "";
		}
		else
		{
			v9 = 0;
		}
		dword_655E80[i] = (int)v9;
	}

	DatLoad* v28;
	for (v28 = portdataDat; v28->address != byte_655E10; ++v28);

	dword_57FD34 = MoveToX;
	dword_57FD38 = MoveToY;
	dword_68AC50[0] = (BYTE*)v28[0].address;
	dword_68AC50[1] = (BYTE*)v28[1].address;
	dword_68AC50[2] = (BYTE*)v28[2].address;
	dword_68AC50[3] = (BYTE*)v28[3].address;
	dword_68AC60 = 0;
	dword_68AC4C = 0;
	CanUpdateSelectedUnitPortrait = 1;

	statport_Dlg = LoadDialog("rez\\statport.bin");
	InitializeDialog_(statport_Dlg, statport_Dlg_Interact_);
}

FAIL_STUB_PATCH(loadPortdata_BINDLG);

void load_statlb_()
{
	if (!multiPlayerMode
		|| gameData.got_file_values.victory_conditions == VC_RESOURCES
		|| gameData.got_file_values.victory_conditions == VC_SLAUGHTER
		|| gameData.got_file_values.victory_conditions == VC_MAP_DEFAULT && gameData.got_file_values.starting_units == SU_MAP_DEFAULT && !gameData.got_file_values.tournament_mode
		|| InReplay)
	{
		byte_59728C = 0;
		dword_597288 = 1;

		statlb_Dlg = LoadDialog("rez\\statlb.bin");
		InitializeDialog_(statlb_Dlg, statlb_Dlg_Interact);
	}
}

FAIL_STUB_PATCH(load_statlb);

void __stdcall hideLeftmostResource_(int a1)
{
	if (statres_Dlg)
	{
		dialog* v3 = getControlFromIndex(statres_Dlg, 6);
		if (a1)
		{
			showDialog(v3);
		}
		else
		{
			HideDialog(v3);
		}
		v3->lUser = 0;
	}
}

FAIL_STUB_PATCH(hideLeftmostResource);

bool __fastcall StatRes_CustomCtrl_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			dlg->lUser = 0;
			dlg->pfcnUpdate = statRes_Text_Update;
			dlg->pfcnInteract = statRes_Text_Interact;
			dlg->wUser = dlg->wIndex == 6 ? 10 : 0;
			[[fallthrough]];
		case USER_NEXT:
		case USER_UNK_8:
		case USER_INIT:
		case USER_SHOW:
		case USER_HIDE:
			return genericCommonInteract(evt, dlg);
		}
	}

	return 0;
}

FAIL_STUB_PATCH(StatRes_CustomCtrl);

void StatRes_RegisterCustomProcs_(dialog* dlg)
{
	static FnInteract functions[] = {
		StatRes_CustomCtrl_,
		StatRes_CustomCtrl_,
		StatRes_CustomCtrl_,
		StatRes_CustomCtrl_,
		StatRes_CustomCtrl_,
		StatRes_CustomCtrl_,
	};

	dlg->lFlags |= CTRL_USELOCALGRAPHIC | CTRL_REVERSE | CTRL_TRANSPARENT;

	registerUserDialogAction(dlg, sizeof(functions), functions);
	SetCallbackTimer(1, dlg, 20, StatRes_MainProc);
	hideLeftmostResource_(0);
}

FAIL_STUB_PATCH(StatRes_RegisterCustomProcs);

bool __fastcall StatRes_DialogInteract_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_KEYFIRST:
	case EventNo::EVN_KEYRPT:
	case EventNo::EVN_MOUSEMOVE:
	case EventNo::EVN_LBUTTONDOWN:
	case EventNo::EVN_LBUTTONDBLCLK:
	case EventNo::EVN_RBUTTONDOWN:
	case EventNo::EVN_RBUTTONDBLCLK:
	case EventNo::EVN_CHAR:
		return 0;
	case EventNo::EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			StatRes_RegisterCustomProcs_(dlg);
			break;
		case EventUser::USER_DESTROY:
			waitLoopCntd(1, dlg);
			break;
		case EventUser::USER_MOUSEMOVE:
			return 0;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(StatRes_DialogInteract);

void load_StatRes_BIN_()
{
	dword_68C238 = (void*)LoadGraphic("game\\icons.grp", 0, "Starcraft\\SWAR\\lang\\statres.cpp", 490);
	dword_68C230 = 0;
	dword_68C22C = 0;

	statres_Dlg = LoadDialog("rez\\statres.bin"); // The topmost row of in-game UI
	InitializeDialog_(statres_Dlg, StatRes_DialogInteract_);
}

FAIL_STUB_PATCH(load_StatRes_BIN);

void ProgressBar_Create_(dialog* a1)
{
	char buff[MAX_PATH];

	a1->pfcnUpdate = statdata_ProgressBarUpdate;
	a1->wUser = 0;
	if (!progress_bar_empty_pcx.data)
	{
		_snprintf(buff, MAX_PATH, "%s%c%s", "game\\", race_lowercase_char_id[consoleIndex], "pbrempt.pcx");
		AllocBackgroundImage(buff, &progress_bar_empty_pcx, 0, "Starcraft\\SWAR\\lang\\statdata.cpp", 222);
	}
	if (!progress_bar_full_pcx.data)
	{
		_snprintf(buff, MAX_PATH, "%s%c%s", "game\\", race_lowercase_char_id[consoleIndex], "pbrfull.pcx");
		AllocBackgroundImage(buff, &progress_bar_full_pcx, 0, "Starcraft\\SWAR\\lang\\statdata.cpp", 226);
	}
}

FAIL_STUB_PATCH(ProgressBar_Create);

bool __fastcall statdata_ProgressBarInteract_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			ProgressBar_Create_(dlg);
			break;
		case EventUser::USER_DESTROY:
			ProgressBar_Destroy();
			break;
		}
	}
	return GenericDlgInteractFxns[dlg->wCtrlType](dlg, evt);
}

FAIL_STUB_PATCH(statdata_ProgressBarInteract);

bool __fastcall statdata_UnitWireframeTransit_(dialog* dlg, dlgEvent* evn)
{
	if (evn->wNo == EventNo::EVN_MOUSEMOVE)
	{
		sub_457DE0(dlg, evn);
	}
	else if (evn->wNo == EventNo::EVN_USER)
	{
		switch (evn->dwUser)
		{
		case EventUser::USER_CREATE:
			dlg->pfcnUpdate = statdata_UnitWireframeTransUpdate;
			dlg->wUser = 0;
			dlg->lUser = 0;
			break;
		case EventUser::USER_ACTIVATE:
			CMDACT_Unload((CUnit*) dlg->lUser);
			return 1;
		case EventUser::USER_MOUSEMOVE:
			if (dlg->lFlags & DialogFlags::CTRL_VISIBLE)
			{
				return 1;
			}
			break;
		case EventUser::USER_NEXT:
			if (dlg == ctrl_under_mouse)
			{
				refreshSelectionScreen_();
				ctrl_under_mouse = 0;
			}
			break;
		}
	}

	return GenericDlgInteractFxns[dlg->wCtrlType](dlg, evn);
}

FAIL_STUB_PATCH(statdata_UnitWireframeTransit);

bool __fastcall statdata_UnitWireframeSelection_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_MOUSEMOVE)
	{
		sub_457DE0(dlg, evt);
	}
	else if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			dlg->pfcnUpdate = statdata_UnitWireframeSelectUpdate;
			dlg->wUser = 0;
			dlg->lUser = (int)SMemAlloc(8, "Starcraft\\SWAR\\lang\\statdata.cpp", 775, 0);
			break;
		case EventUser::USER_DESTROY:
			if (dlg->lUser)
			{
				SMemFree((void*) dlg->lUser, "Starcraft\\SWAR\\lang\\statdata.cpp", 780, 0);
			}
			break;
		case EventUser::USER_ACTIVATE:
			StatusScreenButton(dlg, dlg);
			return 1;
		case EventUser::USER_MOUSEMOVE:
			if (dlg->lFlags & DialogFlags::CTRL_VISIBLE)
			{
				return 1;
			}
			break;
		case EventUser::USER_NEXT:
			if (dlg == ctrl_under_mouse)
			{
				refreshSelectionScreen_();
				ctrl_under_mouse = 0;
			}
			break;
		}
	}

	return GenericDlgInteractFxns[dlg->wCtrlType](dlg, evt);
}

FAIL_STUB_PATCH(statdata_UnitWireframeSelection);

void statdata_extendedCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		statdata_UnitWireframeInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_ProgressBarInteract_,
		statdata_Unknown,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_ProgressBarInteract_,
		statdata_ProgressBarInteract_,
		statdata_buttonInteract,
		statdata_ProgressBarInteract_,
		statdata_buttonInteract,
		statdata_UnitWireframeTransit_, // large unit (leftmost)
		statdata_UnitWireframeTransit_, // large unit (rightmost)
		statdata_UnitWireframeTransit_, // large unit (middle)
		statdata_UnitWireframeTransit_, // medium unit (1st left to right)
		statdata_UnitWireframeTransit_, // medium unit (2nd left to right)
		statdata_UnitWireframeTransit_, // medium unit (3rd left to right)
		statdata_UnitWireframeTransit_, // medium unit (4th left to right)
		statdata_UnitWireframeTransit_, // small unit (top row, 1st left to right)
		statdata_UnitWireframeTransit_, // small unit (bottom row, 1st left to right)
		statdata_UnitWireframeTransit_, // small unit (top row, 2nd left to right)
		statdata_UnitWireframeTransit_, // small unit (bottom row, 2nd left to right)
		statdata_UnitWireframeTransit_, // small unit (top row, 3rd left to right)
		statdata_UnitWireframeTransit_, // small unit (bottom row, 3rd left to right)
		statdata_UnitWireframeTransit_, // small unit (top row, 4th left to right)
		statdata_UnitWireframeTransit_, // small unit (bottom row, 4th left to right)
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
	};

	registerUserDialogAction(dlg, sizeof(functions), functions);
	BINDLG_BlitSurface(dlg);
	CanUpdateStatDataDialog = 1;
}

FAIL_STUB_PATCH(statdata_extendedCtrlID);

bool __fastcall statdata_dlg_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
	case EVN_CHAR:
		return 0;
		break;
	case EVN_MOUSEMOVE:
		statdata_Destroy(dlg, evt);
		break;
	case EVN_USER:
		if (evt->dwUser == USER_CREATE)
		{
			statdata_extendedCtrlID_(dlg);
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(statdata_dlg_Interact);

void load_Statdata_BIN_()
{
	dword_68C1F4 = LoadGraphic("unit\\wirefram\\tranwire.grp", 0, "Starcraft\\SWAR\\lang\\statdata.cpp", 1335);
	dword_68C1FC = LoadGraphic("unit\\wirefram\\grpwire.grp", 0, "Starcraft\\SWAR\\lang\\statdata.cpp", 1337);
	CanUpdateStatDataDialog = 1;
	statusScreenFunc = 0;
	ctrl_under_mouse = 0;
	ctrl_under_mouse_val = -1;

	stardata_Dlg = LoadDialog("rez\\statdata.bin"); // The middle area of the main (bottom-of-the-screen) in-game UI
	InitializeDialog_(stardata_Dlg, statdata_dlg_Interact_);
}

FAIL_STUB_PATCH(load_Statdata_BIN);

bool __fastcall statbtn_Btn_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_MOUSEMOVE:
		sub_4597C0(dlg, evt);
		break;
	case EVN_LBUTTONDOWN:
	case EVN_LBUTTONDBLCLK:
		if (dlg->lFlags & CTRL_DISABLED)
		{
			return 0;
		}
		break;
	case EVN_LBUTTONUP:
		GenericDlgInteractFxns[dlg->wCtrlType](dlg, evt);
		statBtn_dlg_MouseMove(current_dialog, evt);
		return 1;
	case EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			sub_458E70_(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			sub_458BB0_(dlg);
			return 1;
		case EventUser::USER_MOUSEMOVE:
			if (dlg->lFlags & CTRL_DISABLED)
			{
				return 1;
			}
			break;
		}
	}

	return GenericDlgInteractFxns[dlg->wCtrlType](dlg, evt);
}

FAIL_STUB_PATCH(statbtn_Btn_Interact);

void statbtn_BIN_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_BIN_ReplayProgressbar,
	};

	registerUserDialogAction(dlg, sizeof(functions), functions);
	BINDLG_BlitSurface(dlg);
}

FAIL_STUB_PATCH(statbtn_BIN_CustomCtrlID);

bool __fastcall statbtn_DLG_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
		return 0;
	case EVN_MOUSEMOVE:
		statBtn_dlg_MouseMove(dlg, evt);
		break;
	case EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			statbtn_BIN_CustomCtrlID_(dlg);
			break;
		case USER_MOUSEMOVE:
			return 1;
		}
		break;
	case EVN_CHAR:
		return statBtn_dlg_CharPress_(evt);
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(statbtn_DLG_Interact);

void load_statbtn_BIN_()
{
	char buff[MAX_PATH];

	word_68C14C = 228;
	word_68C1C8 = 228;
	word_68C1C4 = 228;
	word_68C1BC = -1;
	_snprintf(buff, MAX_PATH, "unit\\cmdbtns\\%ccmdbtns.grp", race_lowercase_char_id[consoleIndex]);
	dword_68C1C0 = (void*)LoadGraphic(buff, 0, "Starcraft\\SWAR\\lang\\statcmd.cpp", 1086);
	cmdicons_grp = (void*)LoadGraphic("unit\\cmdbtns\\cmdicons.grp", 0, "Starcraft\\SWAR\\lang\\statcmd.cpp", 1089);
	if (!SBmpLoadImage("unit\\cmdbtns\\ticon.pcx", 0, byte_68C150, 96, 0, 0, 0))
	{
		SysWarn_FileNotFound("unit\\cmdbtns\\ticon.pcx", SErrGetLastError());
	}

	_snprintf(buff, MAX_PATH, "rez\\statbtn%c.bin", InReplay ? 'n' : race_lowercase_char_id[consoleIndex]);
	current_dialog = LoadDialog(buff);
	InitializeDialog_(current_dialog, statbtn_DLG_Interact_);
}

FAIL_STUB_PATCH(load_statbtn_BIN);

void load_Statf10_BIN_()
{
	stat_f10_Dlg = LoadDialog("rez\\stat_f10.bin"); // The menu button in the in-game UI
	InitializeDialog_(stat_f10_Dlg, Statf10_DialogProc);
}

FAIL_STUB_PATCH(load_Statf10_BIN);

void onSendText_(dialog* a1, dlgEvent* a2, CheatFlags a3)
{
	if (byte_68C144)
	{
		// this is an inlined sub_4D16F0(0)
		if (GetUserDefaultLangID() == 1042)
		{
			if (!dword_6D6438)
			{
				dword_6D6438 = ImmGetContext(hWndParent);
			}
			dword_6D6438 = ImmAssociateContext(hWndParent, 0);
		}

		dialog* v4 = getControlFromIndex(a1, 6);
		if ((_BYTE)a3)
		{
			char dest[256];
			SStrCopy(dest, v4->pszText, 0x7FFFFFFFu);
			a3 = GameCheats;
			if (CommandLineCheatCompare_(&a3, dest))
			{
				if (a3 != GameCheats)
				{
					CMDACT_UseCheat(a3);
				}
			}
			else if ((!InReplay || !replayCommand(dest)) && multiPlayerMode)
			{
				sendChatMessage();
			}
		}
		SStrCopy(v4->pszText, empty_string, 0x100u);
		v4->fields.edit.bCursorPos = 0;
		v4->fields.optn.bEnabled |= CTRL_UPDATE;
		if ((v4->lFlags & CTRL_VISIBLE) != 0 && (v4->lFlags & CTRL_UPDATE) == 0)
		{
			v4->lFlags |= CTRL_UPDATE;
			updateDialog(v4);
		}
		hAccTable = hAccel;
		input_procedures[16] = CMDACT_Hotkey;
		byte_68C144 = 0;
	}
	else
	{
		// this is an inlined sub_4D16F0(1)
		if (GetUserDefaultLangID() == 1042)
		{
			HIMC v8 = dword_6D6438;
			if (!dword_6D6438)
			{
				v8 = ImmGetContext(hWndParent);
				dword_6D6438 = v8;
			}
			ImmAssociateContext(hWndParent, v8);
		}

		SNetGetLeaguePlayerName((int*)curPlayerID, 0x19u);
		hAccTable = dword_5968F4;
		input_procedures[16] = CMDACT_Hotkey;

		if (!multiPlayerMode)
		{
			byte_68C144 = 1;
		}
		else if (a2->wUnk_0x0A & 3)
		{
			byte_68C144 = 2;
		}
		else if (sub_4AABB0() && (a2->wUnk_0x0A & 4))
		{
			byte_68C144 = 3;
		}
		else if (byte_581D60 == 8)
		{
			byte_68C144 = 2;
		}
		else if (!sub_4AABB0())
		{
			byte_68C144 = 4;
		}
		else if (a2->wUnk_0x0A != 9)
		{
			byte_68C144 = 4;
		}
		else
		{
			byte_68C144 = 3;
		}
	}
	sub_4F3040(a1);
	dialog* v13 = getControlFromIndex(a1, 5);
	if (byte_68C144 || v13->pszText == NULL)
	{
		HideDialog(v13);
	}
	else
	{
		showDialog(v13);
	}
}

FAIL_STUB_PATCH(onSendText);

bool __fastcall textbox_CtrlInteract_(dialog* dlg, struct dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_KEYFIRST:
	case EventNo::EVN_KEYRPT:
		if (VK_PRIOR <= evt->wVirtKey && evt->wVirtKey <= VK_DOWN)
		{
			return 0;
		}
		break;
	case EventNo::EVN_USER:
		if (evt->dwUser == USER_NEXT && byte_68C144)
		{
			onSendText_(dlg->fields.ctrl.pDlg, evt, (CheatFlags) 0);
		}
		break;
	case EventNo::EVN_CHAR:
		textBox_ctrl_charEvent(dlg, evt);
		return 1;
	}

	return genericEditInteract(dlg, evt);
}

FAIL_STUB_PATCH(textbox_CtrlInteract);

void textbox_DLG_Init_Evt_(dialog* dlg)
{
	Font* v2;

	int v1 = (LOWORD(dlg->lFlags) | CTRL_TRANSPARENT) & 0x4C00;
	dlg->lFlags |= CTRL_USELOCALGRAPHIC | CTRL_REVERSE | CTRL_TRANSPARENT;
	byte_68C144 = 0;
	switch (v1)
	{
	case 0x400:
		v2 = FontBase[0];
		break;
	case 0x800:
		v2 = FontBase[2];
		break;
	case 0x4000:
		v2 = FontBase[3];
		break;
	default:
		v2 = FontBase[1];
		break;
	}
	BWFXN_SetFont(v2);

	dialog* v3 = dlg->wCtrlType == DialogType::cDLG ? dlg : dlg->fields.ctrl.pDlg;
	dialog* v4 = getControlFromIndex(dlg, 1);
	if (v4)
	{
		do
		{
			if (v4->wIndex > 3)
			{
				break;
			}
			v4->rct.right = getMessageWidth(v4->pszText) + v4->rct.left;
			v4 = v4->pNext;
		} while (v4);
	}

	BWFXN_SetFont(0);
	dialog* v6 = getControlFromIndex(dlg, 6);
	v6->pfcnInteract = textbox_CtrlInteract_;
	if (InReplay)
	{
		dlg->rct.top -= 24;
		dlg->rct.bottom -= 24;
	}
}

FAIL_STUB_PATCH(textbox_DLG_Init_Evt);

bool __fastcall textbox_DLG_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
		return isSinglePaused();
	case EVN_MOUSEMOVE:
	case EVN_LBUTTONDOWN:
	case EVN_LBUTTONDBLCLK:
	case EVN_RBUTTONDOWN:
	case EVN_RBUTTONDBLCLK:
		return IS_GAME_PAUSED && !multiPlayerMode;
	case EVN_USER:
		if (evt->dwUser == USER_CREATE)
		{
			textbox_DLG_Init_Evt_(dlg);
		}
		else if (evt->dwUser == USER_MOUSEMOVE)
		{
			return 0;
		}
		break;
	case EVN_CHAR:
		if (isSinglePaused())
		{
			if (evt->wVirtKey == VK_ESCAPE)
			{
				keyPress_Escape();
			}
			return 1;
		}
		if (evt->wVirtKey == 10 || evt->wVirtKey == VK_RETURN)
		{
			onSendText_(dlg, evt, (CheatFlags) 1);
		}
		else if (evt->wVirtKey == VK_ESCAPE && byte_68C144)
		{
			onSendText_(dlg, evt, (CheatFlags) 0);
		}
		else
		{
			break;
		}

		return 1;
	}

	bool result = genericDlgInteract(dlg, evt);
	if (IS_GAME_PAUSED && !multiPlayerMode)
	{
		return 1;
	}
	return result;
}

FAIL_STUB_PATCH(textbox_DLG_Interact);

void ctextbox_BIN_()
{
	char buff[80];
	_snprintf(buff, sizeof(buff), "rez\\%ctextbox.bin", race_lowercase_char_id[consoleIndex]);
	dword_68C140 = LoadDialog(buff);
	InitializeDialog_(dword_68C140, textbox_DLG_Interact_);
}

FAIL_STUB_PATCH(ctextbox_BIN);

void LoadConsoleImage_()
{
	char buff[260];
	char v0 = InReplay ? 'n' : race_lowercase_char_id[consoleIndex];
	_snprintf(buff, 0x104u, "game\\%c%s", v0, "console.pcx");

	void* buffer;
	int width;
	int height;
	if (!SBmpAllocLoadImage(buff, 0, &buffer, &width, &height, 0, 0, allocFunction))
	{
		SysWarn_FileNotFound(buff, SErrGetLastError());
	}

	GameScreenConsole.ht = height;
	GameScreenConsole.wid = width;
	GameScreenConsole.data = (u8*)buffer;
	setHudBeginY(&GameScreenConsole);
	MainBltMask = BltMask_Constructor(&GameScreenConsole, 0, 0);
}

FAIL_STUB_PATCH(LoadConsoleImage);

void setup_HUD_()
{
	LoadConsoleImage_();
	if (!SBmpLoadImage("game\\thpbar.pcx", 0, byte_66FBE4, 19, 0, 0, 0))
	{
		SysWarn_FileNotFound("game\\thpbar.pcx", SErrGetLastError());
	}
	load_statfluf_BIN_();
	loadPortdata_BINDLG_();
	load_statlb_();
	load_StatRes_BIN_();
	load_Statdata_BIN_();
	load_WireframeGRP();
	load_statbtn_BIN_();
	load_Statf10_BIN_();
	ctextbox_BIN_();
	load_gluMinimap_();
	if (GameScreenConsole.data != NULL)
	{
		SMemFree(GameScreenConsole.data, "Starcraft\\SWAR\\lang\\status.cpp", 217, NULL);
		GameScreenConsole.data = NULL;
	}
	memset(ClientSelectionGroup, 0, sizeof(ClientSelectionGroup));
	ClientSelectionCount = NULL;
	ActivePortraitUnit = NULL;
	ctrl_under_mouse = NULL;
	ctrl_under_mouse_val = NULL;
	CanUpdateCurrentButtonSet = 1;
	CanUpdateSelectedUnitPortrait = 1;
	CanUpdateStatDataDialog = 1;
}

FAIL_STUB_PATCH(setup_HUD);

template <int PIXEL_STRIDE>
void minimapVisionUpdateMegatile(int flags, int x, int y)
{
	for (int i = 0; i < PIXEL_STRIDE; i++)
	{
		for (int j = 0; j < PIXEL_STRIDE; j++)
		{
			int index = (PIXEL_STRIDE * x + i) * minimap_surface_width + (PIXEL_STRIDE * y + j);
			minimap_surface.data[index] = byte_59C2C0[256 * flags + minimap_surface_no_fog.data[index]];
		}
	}
}

template <int PIXEL_STRIDE>
void minimapVisionUpdate_()
{
	for (int i = 0; i < map_size.height / word_59CC68; i++)
	{
		for (int j = 0; j < map_size.width / word_59CC68; j++)
		{
			MegatileFlags active_tile = active_tiles[(i * map_size.width + j) * word_59CC68];
			BOOL v2 = (playerVisions & active_tile) == 0;
			int v3 = (playerExploredVisions & active_tile) == 0;
			if (InReplay)
			{
				if (replayShowEntireMap)
				{
					v2 = 1;
					v3 = 0;
				}
				else
				{
					v3 = 0;
					BYTE1(v3) = ReplayVision;
					int v4 = ~active_tile;
					if ((v3 & v4) != 0)
					{
						if (((unsigned __int8)v4 & (unsigned __int8)ReplayVision) != 0)
						{
							v2 = 1;
							v3 = 0;
						}
						else
						{
							v2 = 0;
							v3 = 1;
						}
					}
					else
					{
						v2 = 0;
						v3 = 0;
					}
				}
			}

			minimapVisionUpdateMegatile<PIXEL_STRIDE>(v3 | (2 * v2), i, j);
		}
	}
}

FAIL_STUB_PATCH(minimapVisionUpdate_64);
FAIL_STUB_PATCH(minimapVisionUpdate_96_128);
FAIL_STUB_PATCH(minimapVisionUpdate_192_256);

template <int PIXEL_STRIDE>
void minimapSurfaceUpdateMegatile(int x, int y)
{
	int cellMapIndex = (x * map_size.width + y) * word_59CC68;
	__int16 cell = CellMap[cellMapIndex] & 0x7FFF;
	for (int i = 0; i < PIXEL_STRIDE; i++)
	{
		for (int j = 0; j < PIXEL_STRIDE; j++)
		{
			minimap_surface_no_fog.data[(PIXEL_STRIDE * x + i) * minimap_surface_width + PIXEL_STRIDE * y + j] = byte_59CB60[VR4Data->cdata[4 * (VX4Data[cell].wImageRef[i][j] & 0xFFFE) + 6][7]];
		}
	}
}

template <int PIXEL_STRIDE>
void minimapSurfaceUpdate_()
{
	for (int i = 0; i < map_size.height / word_59CC68; i++)
	{
		for (int j = 0; j < map_size.width / word_59CC68; j++)
		{
			minimapSurfaceUpdateMegatile<PIXEL_STRIDE>(i, j);
		}
	}
}

FAIL_STUB_PATCH(minimapSurfaceUpdate_64);
FAIL_STUB_PATCH(minimapSurfaceUpdate_96_128);
FAIL_STUB_PATCH(minimapSurfaceUpdate_192_256);
FAIL_STUB_PATCH(sub_4A3D40);

void setMapSizeConstants_()
{
	int larger_dimension = map_size.width <= map_size.height ? map_size.height : map_size.width;

	if (larger_dimension <= 32)
	{
		word_59CC68 = 1;
		word_59CC6C = 8;
		minimapSurfaceUpdate = minimapSurfaceUpdate_<4>;
		minimapVisionUpdate = minimapVisionUpdate_<4>;
		minimap_surface_height = 4 * map_size.height;
		minimap_surface_width = 4 * map_size.width;
		word_59C184 = 0;
		word_59C1B0 = 2;
	}
	else if (larger_dimension <= 64)
	{
		word_59CC68 = 1;
		word_59CC6C = 16;
		minimapSurfaceUpdate = minimapSurfaceUpdate_<2>;
		minimapVisionUpdate = minimapVisionUpdate_<2>;
		minimap_surface_height = 2 * map_size.height;
		minimap_surface_width = 2 * map_size.width;
		word_59C184 = 0;
		word_59C1B0 = 1;
	}
	else if (larger_dimension <= 128)
	{
		word_59CC68 = 1;
		word_59CC6C = 32;
		minimapSurfaceUpdate = minimapSurfaceUpdate_<1>;
		minimapVisionUpdate = minimapVisionUpdate_<1>;
		minimap_surface_height = map_size.height;
		minimap_surface_width = map_size.width;
		word_59C184 = 0;
		word_59C1B0 = 0;
	}
	else if (larger_dimension <= 256)
	{
		word_59CC68 = 2;
		word_59CC6C = 64;
		minimapSurfaceUpdate = minimapSurfaceUpdate_<1>;
		minimapVisionUpdate = minimapVisionUpdate_<1>;
		minimap_surface_height = map_size.height >> 1;
		minimap_surface_width = map_size.width >> 1;
		word_59C184 = 0;
		word_59C1B0 = 0;
	}

	int v5 = (128 - minimap_surface_width) / 2;
	int v7 = (128 - minimap_surface_height) / 2;
	minimap_dialog->rct.left += v5;
	minimap_dialog->rct.right -= v5;
	minimap_dialog->rct.top += v7;
	minimap_dialog->rct.bottom -= v7;

	CreateMinimapSurface();

	stru_512D00.left = minimap_dialog->rct.left;
	stru_512D00.top = minimap_dialog->rct.top + 315;
	stru_512D00.right = minimap_dialog->rct.left + minimap_surface_width - 1;
	stru_512D00.bottom = minimap_dialog->rct.top + minimap_surface_height + 314;
}

FAIL_STUB_PATCH(setMapSizeConstants);

const char* TILESET_NAMES[] = {
	"badlands",
	"platform",
	"install",
	"AshWorld",
	"Jungle",
	"Desert",
	"Ice",
	"Twilight",
};

MEMORY_PATCH(0x4D6D41, TILESET_NAMES);

bool __stdcall ChkLoader_TYPE_(SectionData* section_data, int section_size, MapChunks* a3)
{
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_TYPE);

bool __stdcall ChkLoader_VER_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 2)
		return 0;

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}
	memcpy(&a3->version, section_data->start_address, section_data->size);
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_VER);

bool __stdcall ChkLoader_DIM_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 4)
	{
		return 0;
	}
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}
	memcpy(&map_size, section_data->start_address, section_data->size);
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_DIM);

bool __stdcall ChkLoader_ERA_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 2)
		return 0;
	if (section_data->start_address + section_data->size > section_data->next_section)
		return 0;

	CurrentTileSet = *(Tileset*)section_data->start_address;
	if (CurrentTileSet > Tileset::Jungle && !IsExpansion)
		return 0;

	if (CurrentTileSet >= _countof(TILESET_NAMES))
	{
		CurrentTileSet = (Tileset) int(CurrentTileSet % 8);
	}

	return 1;
}

FAIL_STUB_PATCH(ChkLoader_ERA);

bool __stdcall ChkLoader_STR_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (MapStringTbl.buffer)
	{
		SMemFree(MapStringTbl.buffer, "Starcraft\\SWAR\\lang\\maphdr.cpp", 356, 0);
	}
	MapStringTbl.buffer = (u16*)SMemAlloc(section_size, "Starcraft\\SWAR\\lang\\maphdr.cpp", 357, 0);
	if (MapStringTbl.buffer == NULL)
	{
		return false;
	}

	chk_string_section_size = section_size;
	AppAddExit_(freeCHKStringHandle);
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return false;
	}

	memcpy(MapStringTbl.buffer, section_data->start_address, section_data->size);
	return true;
}

FAIL_STUB_PATCH(ChkLoader_STR);

bool __stdcall ChkLoader_MBRF_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size % sizeof(BriefingEntry) != 0)
	{
		return false;
	}
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return false;
	}

	for (int i = 0; i < section_size / sizeof(BriefingEntry); i++)
	{
		if (!AddBriefingTrigger((BriefingEntry*) section_data->start_address + i * sizeof(BriefingEntry)))
		{
			break;
		}
	}
	return true;
}

FAIL_STUB_PATCH(ChkLoader_MBRF);

bool __stdcall ChkLoader_SPRP_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 4)
	{
		return false;
	}

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return false;
	}
	memcpy(a3, section_data->start_address, section_data->size);
	return true;
}

FAIL_STUB_PATCH(ChkLoader_SPRP);

bool __stdcall ChkLoader_FORC_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size > 20)
	{
		return false;
	}

	for (int i = 0; i < 8; i++)
	{
		a3->player_force[i] = 0;
	}

	for (int i = 0; i < 4; i++)
	{
		a3->tbl_index_force_name[i] = 0;
		a3->force_flags[i] = (ForceFlags) 0;
	}

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return false;
	}
	memcpy(a3->player_force, section_data->start_address, section_data->size);
	return true;
}

FAIL_STUB_PATCH(ChkLoader_FORC);

bool __stdcall ChkLoader_VCOD_(SectionData *section_data, int section_size, MapChunks* a3)
{
	HRSRC hVCOD = FindResourceA(hInst, (LPCSTR)0xCA, "VCOD");
	if (!hVCOD)
		return 0;

	DWORD VCOD_Size = SizeofResource(hInst, hVCOD);
	if (section_size != VCOD_Size)
		return 0;

	HGLOBAL v6 = LoadResource(hInst, hVCOD);
	if (!v6)
		return 0;

	void *VCOD_Locked = LockResource(v6);
	if (!VCOD_Locked)
		return 0;

	void *hCHKData = SMemAlloc(section_size, "Starcraft\\SWAR\\lang\\maphdr.cpp", 1171, 0);
	if (!hCHKData)
		return 0;

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		SMemFree(hCHKData, "Starcraft\\SWAR\\lang\\maphdr.cpp", 1174, 0);
		return 0;
	}
	memcpy(hCHKData, section_data->start_address, section_data->size);

	int Hash_Chk = CHK_PerformVCODcheck(LobbyPlayers, 0x1B0u, (int) hCHKData, section_size);
	int Hash_VCOD = CHK_PerformVCODcheck(LobbyPlayers, 0x1B0u, (int) VCOD_Locked, section_size);

	SMemFree(hCHKData, "Starcraft\\SWAR\\lang\\maphdr.cpp", 1182, 0);
	return Hash_VCOD == Hash_Chk;
}

FAIL_STUB_PATCH(ChkLoader_VCOD);
FAIL_STUB_PATCH(CopySectionData);

#define MAX_MAP_DIMENTION 256

// TODO: reimplement SAI_PathCreate_Sub3_0 (0x0483EBA) and SAI_PathCreate_Sub3_4 (0x483260) for pathfinding on map sizes > 256x256

u16 SAI_GetRegionIdFromPx_(__int16 y, __int16 x)
{
	u16 region_id = SAIPathing->mapTileRegionId[y / TILE_HEIGHT][x / TILE_WIDTH];
	if (region_id >= 0x2000u)
	{
		return SAIPathing->splitTiles[region_id - 0x2000].rgn1;
	}
	return region_id;
}

u16 SAI_GetRegionIdFromPx__(__int16 x)
{
	__int16 y;

	__asm mov y, ax

	return SAI_GetRegionIdFromPx_(y, x);
}

FUNCTION_PATCH((void*)0x49C9A0, SAI_GetRegionIdFromPx__);

int __stdcall sub_422FA0_(struct_a1_1* a1, int a2)
{
	u16 x = std::clamp(a1->unk_posintion3.x, SAIPathing->regions[a2].rgnBox.left, SAIPathing->regions[a2].rgnBox.right) / 32;
	u16 y = std::clamp(a1->unk_posintion3.y, SAIPathing->regions[a2].rgnBox.top, SAIPathing->regions[a2].rgnBox.bottom) / 32;

	for (int v19 = 1; v19 < 16; v19 += 2)
	{
		int v12 = 0;
		do
		{
			for (int v20 = 0; v20 < v19; v20++)
			{
				if (x < map_size.width && y < map_size.height && SAI_GetRegionIdFromPx_(32 * y, 32 * x) == (_WORD)a2)
				{
					a1->unk_posintion3.x = 32 * x + 16;
					a1->unk_posintion3.y = 32 * y + 16;
					a1->byte1F = 1;
					return 1;
				}
				x += word_519F54[v12];
				y += word_519F5C[v12];
			}

			x -= word_519F54[v12];
			y -= word_519F5C[v12];
			v12++;
		} while (v12 < 4);
		--x;
		--y;
	}

	a1->unk_posintion3.x = SAIPathing->regions[a2].rgnCenterX >> 8;
	a1->unk_posintion3.y = SAIPathing->regions[a2].rgnCenterY >> 8;
	a1->byte1F = 1;

	return 1;
}

FUNCTION_PATCH(sub_422FA0, sub_422FA0_);

u16 GetRegionIdAtPosEx_(int y, int x)
{
	u16 region_id = SAIPathing->mapTileRegionId[y / TILE_HEIGHT][x / TILE_WIDTH];
	if (region_id >= 0x2000u)
	{
		if ((1 << (((x / 8) & 3) + 4 * ((y / 8) & 3))) & SAIPathing->splitTiles[region_id - 0x2000].minitileMask)
		{
			return SAIPathing->splitTiles[region_id - 0x2000].rgn2;
		}
		else
		{
			return SAIPathing->splitTiles[region_id - 0x2000].rgn1;
		}
	}
	return region_id;
}

u16 GetRegionIdAtPosEx__()
{
	int x, y;

	__asm
	{
		mov x, edi
		mov y, ecx
	}

	return GetRegionIdAtPosEx_(y, x);
}

FUNCTION_PATCH((void*)0x49C9F0, GetRegionIdAtPosEx__);

int sub_422A90_(struct_a1_1* a1, Position* a2)
{
	int v18 = 1;
	while (2)
	{
		int v19 = 0;
		do
		{
			int v16 = 0;
			while (1)
			{
				u16 v5 = a2->x + a1->word158.x;
				u16 v6 = a2->y + a1->word158.y;
				u16 v7 = a2->y + a1->word15C.y;
				u16 v8 = a2->x + a1->word15C.x;
				if (v5 < map_width_pixels && v6 < map_height_pixels && v8 < map_width_pixels && v7 < map_height_pixels)
				{
					int v9 = 1;
					int v10 = v7 / 32 - v6 / 32;
					int i = 0;
					while (v10 >= 0)
					{
						for (int v11 = v8 / 32 - v5 / 32; v11 >= 0; --v11)
						{
							u16 region_id = SAIPathing->mapTileRegionId[v6 / 32 + i][v5 / 32 + v11];
							if (region_id < 0x2000u && SAIPathing->regions[region_id].accessabilityFlags == SAF_Inaccessible)
							{
								v9 = 0;
								break;
							}
						}
						--v10;
						i++;
						if (v9 == 0)
						{
							goto LABEL_19;
						}
					}
					return 1;
				}
			LABEL_19:
				++v16;
				a2->x += 8 * word_519F54[v19];
				a2->y += 8 * word_519F5C[v19];
				if (v16 >= v18)
				{
					break;
				}
			}

			a2->x += -8 * word_519F54[v19];
			a2->y += -8 * word_519F5C[v19];
			v19++;
		} while (v19 < 4);

		a2->x -= 8;
		a2->y -= 8;
		v18 += 2;
		if (v18 >= 10)
		{
			return 0;
		}
	}
}

int __stdcall sub_422A90__(Position* a2)
{
	struct_a1_1* a1;

	__asm mov a1, eax

	return sub_422A90_(a1, a2);
}

FUNCTION_PATCH((void*)0x422A90, sub_422A90__);

int SAI_PathCreate_Sub3_0_(SAI_Paths* a1, Position a2, MapSize size)
{
	int x;
	int y;
	int v23;
	int v24;
	int v28;
	int v29;
	int v30;

	x = a2.x;
	y = a2.y;
	v23 = a1->regionCount;
	dword_6D5BF8 = 0;
	v24 = 0;
	SaiAccessabilityFlags a5 = (SaiAccessabilityFlags) 0;
	v28 = 56;
	while (2)
	{
		rect v19 = { 0, 0, 0, 0 };
		v29 = 0;
		v30 = 0;

		if (x >= (__int16)size.width)
		{
			x = (__int16)a2.x;
			y += 1;
			if (y >= (__int16)size.height)
			{
				y = (__int16)a2.y;
			}
		}

		u16* v20 = &a1->mapTileRegionId[y][x];
		while (1)
		{
			if (a1->mapTileRegionId[y][x] >= 5000u)
			{
				rect a4;
				SAI_PathCreate_Sub3_0_0(y, x, a1, &a4, a1->mapTileRegionId[y][x]);
				++v29;
				if (v30 < (a4.right - a4.left) * (a4.bottom - a4.top))
				{
					v30 = (a4.right - a4.left) * (a4.bottom - a4.top);
					v19 = a4;
					a5 = (SaiAccessabilityFlags)a1->mapTileRegionId[y][x];
				}
				if (v30 >= v28 || v29 > 25)
				{
					x = v19.right;
					y = v19.top;
					break;
				}
				x = a4.right;
			}
			else
			{
				x += 1;
			}

			if (x >= size.width)
			{
				x = (__int16)a2.x;
				++y;
				if (y >= (__int16)size.height)
				{
					y = (__int16)a2.y;
				}
			}
			if (&a1->mapTileRegionId[y][x] == v20)
			{
				break;
			}
		}

		if (a1->regionCount >= 5000)
		{
			dword_6D5BF8 = 1;
			if (dword_6CA54C)
			{
				SMemFree(dword_6CA54C, "Starcraft\\SWAR\\lang\\sai_PathCreate.cpp", 882, 0);
				dword_6CA54C = 0;
			}
			if (!outOfGame)
			{
				leaveGame(3);
				outOfGame = 1;
				doNetTBLError(
					0,
					"The map could not be loaded because it had too many obstructions. "
					"Try widening corridors and reducing the number of small nooks and crannies to correct the problem.\n\n",
					0,
					97);
				if (gwGameMode == GAME_RUN)
				{
					GameState = 0;
					gwNextGameMode = GAME_GLUES;
					if (!InReplay)
					{
						replay_header.ReplayFrames = ElapsedTimeFrames;
					}
				}
				if (glGluesMode == GAME_GLUES)
				{
					GetWindowThreadProcessId(hWndParent, (LPDWORD)&size);
					EnumWindows(EnumFunc, *(_DWORD*)&size);
				}
				glGluesMode = GLUE_GENERIC;
			}
			return 0;
		}

		v28 = (v19.right - v19.left) * (v19.bottom - v19.top);
		if (v28 == 0)
		{
			return 1;
		}

		SAI_PathCreate_Sub3_0_1(a1->regionCount, &v19, a1);
		a1->regions[a1->regionCount].rgnCenterX = (v19.left + v19.right) / 2;
		a1->regions[a1->regionCount].rgnCenterY = (v19.top + v19.bottom) / 2;
		if (v19.left > 0)
		{
			--v19.left;
		}
		if (v19.right < (int)map_size.width)
		{
			++v19.right;
		}
		if (v19.top > 0)
		{
			--v19.top;
		}
		if (v19.bottom < (int)map_size.height)
		{
			++v19.bottom;
		}

		SAI_PathCreate_Sub3_0_2(a1->regionCount, a1, &v19, a5);
		a1->regions[a1->regionCount].rgnBox.left = v19.left;
		a1->regions[a1->regionCount].rgnBox.top = v19.top;
		a1->regions[a1->regionCount].rgnBox.right = v19.right;
		a1->regions[a1->regionCount].rgnBox.bottom = v19.bottom;
		a1->regions[a1->regionCount].accessabilityFlags = a5;
		a1->regionCount++;
		if (v28 <= 6 && v24 == 0)
		{
			v24 = 1;
			for (int v12 = v23; v12 < a1->regionCount; v12++)
			{
				u16* v14 = &a1->regions[v12].rgnBox.top;
				if ((__int16)a1->regions[v12].rgnBox.left > 0)
				{
					a1->regions[v12].rgnBox.left -= 1;
				}
				if ((__int16)a1->regions[v12].rgnBox.right < map_size.width)
				{
					a1->regions[v12].rgnBox.right += 1;
				}
				if ((__int16)a1->regions[v12].rgnBox.top > 0)
				{
					a1->regions[v12].rgnBox.top -= 1;
				}
				if ((__int16)a1->regions[v12].rgnBox.bottom < map_size.height)
				{
					a1->regions[v12].rgnBox.bottom += 1;
				}
				SAI_PathCreate_Sub3_0_2(v12, a1, (rect*)&a1->regions[v12].rgnBox - 1, a1->regions[v12].accessabilityFlags);
			}
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_0);

DEFINE_ENUM_FLAG_OPERATORS(SaiAccessabilityFlags);

void SAI_PathCreate_Sub3_1_0_(SAI_Paths* a1)
{
	for (int i = 0; i < a1->regionCount; i++)
	{
		a1->regions[i].tileCount = 0;
		a1->regions[i].rgnBox.left = 0x7FFF;
		a1->regions[i].rgnBox.top = 0x7FFF;
		a1->regions[i].rgnBox.bottom = -1;
		a1->regions[i].rgnBox.right = -1;
	}

	for (int v15 = 0; v15 < map_size.height; v15++)
	{
		for (int v5 = 0; v5 < map_size.width; v5++)
		{
			WORD v6 = a1->mapTileRegionId[v15][v5];
			if (v6 < 5000u)
			{
				a1->regions[v6].tileCount += 1;

				Box16* box = &a1->regions[v6].rgnBox;
				if ((__int16)box->top > 32 * v15)
				{
					box->top = 32 * v15;
				}
				if ((__int16)box->left > 32 * v5)
				{
					box->left = 32 * v5;
				}
				if ((__int16)box->bottom < 32 * v15 + 32)
				{
					box->bottom = 32 * (v15 + 1);
				}
				if ((__int16)box->right < 32 * v5 + 32)
				{
					box->right = 32 * (v5 + 1);
				}
			}
		}
	}

	for (int i = 0; i < a1->regionCount; ++i)
	{
		if (a1->regions[i].tileCount == 0 && a1->regions[i].accessabilityFlags != (SAF_Inaccessible | SAF_UNK))
		{
			a1->regions[i].accessabilityFlags = SAF_Inaccessible | SAF_UNK;
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_1_0);

void SAI_PathCreate_Sub3_1_1_(SAI_Paths* a1)
{
	a1->globalBuffer_ptr = a1->globalBuffer;

	for (int i = 0; i < a1->regionCount; i++)
	{
		SaiRegion* sai_region = &a1->regions[i];
		if (sai_region->tileCount)
		{
			SAI_PathCreate_Sub3_1_1_0(a1, i, sai_region);
			SAI_PathCreate_Sub3_1_1_1(a1, sai_region);
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_1_1);

void SAI_PathCreate_Sub3_1_(int a1, SAI_Paths* a2)
{
	a2->globalBuffer_ptr = a2->globalBuffer;
	SAI_PathCreate_Sub3_1_0_(a2);
	SAI_PathCreate_Sub3_1_1_(a2);

	for (int i = a1; i < a2->regionCount; i++)
	{
		if (a2->regions[i].rgnCenterX < 256)
		{
			a2->regions[i].rgnCenterX = (a2->regions[i].rgnCenterX << 13) + 4096;
		}
		if (a2->regions[i].rgnCenterY < 256)
		{
			a2->regions[i].rgnCenterY = (a2->regions[i].rgnCenterY << 13) + 4096;
		}
	}

	SAI_CreateRegionGroupings(a2);
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_1);

void SAI_PathCreate_Sub3_3_(SAI_Paths* a1)
{
	__int16 v16[5000];
	int v18 = 0;
	for (int v3 = 0; v3 < a1->regionCount; v3++)
	{
		v16[v3] = v18;
		if (a1->regions[v3].tileCount != 0)
		{
			++v18;
		}
	}

	SaiRegion* a1a = a1->regions;
	for (int region_index = 0; region_index < a1->regionCount; region_index++)
	{
		SaiRegion* region = &a1->regions[region_index];
		if (region->tileCount)
		{
			for (int v8 = 0; v8 < region->neighborCount; v8++)
			{
				u16* v9 = region->neighbors + v8;
				*v9 = v16[*v9];
			}
			memcpy(a1a++, region, sizeof(SaiRegion));
		}
	}

	a1->regionCount = v18;

	for (int v11 = 0; v11 < map_size.height; v11++)
	{
		for (int v15 = 0; v15 < map_size.width; v15++)
		{
			u16* v12 = &a1->mapTileRegionId[v11][v15];
			*v12 = v16[*v12];
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_3);

int SAI_PathCreate_Sub3_(PathCreateRelated* a1, SAI_Paths* a2)
{
	int old_region_count = a2->regionCount;

	if (!SAI_PathCreate_Sub3_0_(a2, a1->position, a1->map_size))
	{
		return 0;
	}

	SAI_PathCreate_Sub3_1_(old_region_count, a2);
	SAI_PathCreate_Sub3_2(a2);
	SAI_PathCreate_Sub3_3_(a2);
	SAI_PathCreate_Sub3_1_(old_region_count, a2);
	a2->splitTiles_end = a2->splitTiles;
	SAI_PathCreate_Sub3_4();
	memset(a2->regions + a2->regionCount, 0, (_countof(a2->regions) - a2->regionCount) * sizeof(SaiRegion));
	return 1;
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3);

MEMORY_PATCH((void*)0x46EAA0, sizeof(SAI_Paths));

void SAI_PathCreate_Sub1_(MegatileFlags* megatile_flags)
{
	SAI_PathCreate_Sub1_0();

	for (int i = 0; i < map_size.height; i++)
	{
		for (int j = 0; j < map_size.width; j++)
		{
			MegatileFlags flags = megatile_flags[i * map_size.width + j];
			if ((flags & (REAL_CREEP | MORE_THAN_12_WALKABLE)) == 0)
			{
				SAIPathing->mapTileRegionId[i][j] = SAF_Inaccessible;
			}
			else if ((flags & (MORE_THAN_12_HIGH_HEIGHT | MORE_THAN_12_MEDIUM_HEIGHT | MORE_THAN_12_WALKABLE)) == 0)
			{
				SAIPathing->mapTileRegionId[i][j] = SAF_Inaccessible;
			}
			else if ((flags & (MORE_THAN_12_HIGH_HEIGHT | MORE_THAN_12_MEDIUM_HEIGHT | MORE_THAN_12_WALKABLE)) == (MORE_THAN_12_MEDIUM_HEIGHT | MORE_THAN_12_WALKABLE))
			{
				SAIPathing->mapTileRegionId[i][j] = SAF_HighGround;
			}
			else if ((flags & (MORE_THAN_12_HIGH_HEIGHT | MORE_THAN_12_MEDIUM_HEIGHT | MORE_THAN_12_WALKABLE)) == (MORE_THAN_12_HIGH_HEIGHT | MORE_THAN_12_WALKABLE))
			{
				SAIPathing->mapTileRegionId[i][j] = 8186;
			}
			else
			{
				SAIPathing->mapTileRegionId[i][j] = SAF_LowGround;
			}
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub1);

void SAI_PathCreate_Sub4_(SAI_Paths* a1)
{
	a1->contours = (SaiContourHub*)SMemAlloc(56, "Starcraft\\SWAR\\lang\\sai_ContoursCreate.cpp", 129, 0);
	memset(a1->contours, 0, sizeof(SaiContourHub));
	a1->contours->a = 1;
	a1->contours->b = -1;
	a1->contours->c = -1;
	a1->contours->d = 1;
	SAI_ContoursCreate(a1->contours);
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub4);

bool SAI_PathCreate_(MegatileFlags* a1)
{
	AllocateSAI_Paths_();

	SAI_PathCreate_Sub1_(a1);

	PathCreateRelated v5;
	v5.position.x = 0;
	v5.position.y = 0;
	v5.map_size = map_size;
	CreateUIUnreachableRegion(SAIPathing);

	if (!SAI_PathCreate_Sub3_(&v5, SAIPathing))
	{
		return false;
	}

	SAI_PathCreate_Sub4_(SAIPathing);
	for (int i = 0; i < SAIPathing->regionCount; i++)
	{
		SaiRegion* sai_region = SAIPathing->regions + i;
		sai_region->defencePriority = SAI_PathCreate_Sub5(sai_region);
	}
	return true;
}

FAIL_STUB_PATCH(SAI_PathCreate);

bool __stdcall ChkLoader_MTXM_(SectionData *section_data, int a2, MapChunks *a3)
{
	if (a2 > MAX_MAP_DIMENTION * MAX_MAP_DIMENTION * sizeof(TileID) || section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}

	memcpy(MapTileArray, section_data->start_address, section_data->size);
	sub_4BCEA0();

	MegatileFlags* lowerLeftCournerTiles = &active_tiles[map_size.width * (map_size.height - 2)];
	for (int i = 0; i <= 2; ++i)
	{
		for (int j = 0; j <= 5; ++j)
		{
			lowerLeftCournerTiles[i * map_size.width + j] &= ~(CLIFF_EDGE | REAL_CREEP | MORE_THAN_12_WALKABLE);
			lowerLeftCournerTiles[i * map_size.width + j] |= ALWAYS_UNBUILDABLE;
		}
	}

	MegatileFlags* lowerRightCournerTiles = &active_tiles[map_size.width * (map_size.height - 1) - 5];
	for (int i = 0; i <= 2; ++i)
	{
		for (int j = 0; j <= 5; ++j)
		{
			lowerRightCournerTiles[i * map_size.width + j] &= ~(CLIFF_EDGE | REAL_CREEP | MORE_THAN_12_WALKABLE);
			lowerRightCournerTiles[i * map_size.width + j] |= ALWAYS_UNBUILDABLE;
		}
	}

	SetFogMask(0x20410000, 1, map_size.width, 0, map_size.height - 1);
	AddFogMask(1, map_size.width, 0x800000, 0, map_size.height - 1);
	return SAI_PathCreate_(active_tiles) != 0;
}

FAIL_STUB_PATCH(ChkLoader_MTXM);
FAIL_STUB_PATCH(sub_49CC30);

struct Thingy2Entry
{
	union {
		UnitType unit_type;
		u16 sprite_type;
	};
	Position position;
	u8 player_id;
	u8 _unused1;
	u16 _unused2 : 12;
	u16 draw_as_sprite : 1;
	u16 unused3 : 2;
	u16 disabled : 1;
};

bool __stdcall ChkLoader_THG2_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size % 10u)
	{
		return 0;
	}

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}

	Thingy2Entry* entries = (Thingy2Entry*)section_data->start_address;
	for (int i = 0; i < section_size / 10; i++)
	{
		if (entries[i].draw_as_sprite)
		{
			CreateThingy(entries[i].sprite_type, entries[i].position.x, entries[i].position.y, entries[i].player_id);
		}
		else
		{
			UnitType unit_type = entries[i].unit_type;
			if (unit_type == Special_Upper_Level_Door || unit_type == Special_Right_Upper_Level_Door || unit_type == Special_Pit_Door || unit_type == Special_Right_Pit_Door)
			{
				entries[i].player_id = 11;
			}
			if (gameData.got_file_values.victory_conditions == VC_MAP_DEFAULT && gameData.got_file_values.starting_units == SU_MAP_DEFAULT && !gameData.got_file_values.tournament_mode || entries[i].player_id == 11)
			{
				CUnit* unit = CreateUnitAtPos(entries[i].player_id, entries[i].unit_type, entries[i].position.x, entries[i].position.y);
				if (unit && entries[i].disabled)
				{
					Thg2SpecialDIsableUnit(unit);
				}
			}
		}
	}

	return 1;
}

FAIL_STUB_PATCH(ChkLoader_THG2);

bool __stdcall ChkLoader_MASK_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}

	u8* v4 = section_data->start_address;
	for (int i = 0; i < section_size; i++)
	{
		active_tiles[i] &= (MegatileFlags)~0xFFFF;
		active_tiles[i] |= (MegatileFlags)v4[i];
		active_tiles[i] |= (MegatileFlags)(v4[i] << 8);
	}
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_MASK);

int CHK_UNIT_StartLocationSub_(Position* a1, ChunkUnitEntry* a2)
{
	if (a2->unit_type != Special_Start_Location)
	{
		return 0;
	}
	a1[a2->properties.player] = a2->position;

	bool v3 = InReplay ? MoveToTile.x == 0xFFFF : a2->properties.player == g_LocalNationID;
	if (v3)
	{
		MoveToTile.x = max(a2->position.x - SCREEN_WIDTH / 2, 0) / TILE_WIDTH;
		MoveToTile.y = max(a2->position.y - (SCREEN_HEIGHT - INTERFACE_HEIGHT) / 2, 0) / TILE_HEIGHT;
	}
	return 1;
}

FAIL_STUB_PATCH(CHK_UNIT_StartLocationSub);

bool IsCritter(UnitType unit_type)
{
	return unit_type == Critter_Rhynadon
		|| unit_type == Critter_Bengalaas
		|| unit_type == Critter_Ragnasaur
		|| unit_type == Critter_Scantid
		|| unit_type == Critter_Kakaru
		|| unit_type == Critter_Ursadon;
}

bool IsResource(UnitType unit_type)
{
	return unit_type == Resource_Mineral_Field || unit_type == Resource_Mineral_Field_Type_2 || unit_type == Resource_Mineral_Field_Type_3 || unit_type == Resource_Vespene_Geyser;
}

bool unitIsNeutral(ChunkUnitEntry* unit_entry)
{
	if (gameData.got_file_values.starting_units == StartingUnits::SU_MAP_DEFAULT)
	{
		return true;
	}
	else
	{
		return unit_entry->properties.player == 11 && (IsResource(unit_entry->unit_type) || IsCritter(unit_entry->unit_type));
	}
}

FAIL_STUB_PATCH(unitNotNeutral);

int __fastcall getVisibilityMaskFromPositionAndSize_(int a1, __int16 a2, __int16 a3, __int16 a4, __int16 a5)
{
	MegatileFlags result = (MegatileFlags) 0;

	for (int i = 0; i < a2; i++)
	{
		for (int j = 0; j < a5; j++)
		{
			result |= ~(active_tiles[(a4 + i) * map_size.width + a3 + j] & (MegatileFlags)0xFF);
		}
	}
	return result;
}

FUNCTION_PATCH(getVisibilityMaskFromPositionAndSize, getVisibilityMaskFromPositionAndSize_);

bool __stdcall ChkLoader_UNIT_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size % sizeof(ChunkUnitEntry))
	{
		return 0;
	}
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}

	memset(startPositions, 0, 8 * sizeof(Position));

	UnitRelated20* v18 = NULL;
	UnitRelated20* v19 = NULL;
	ChunkUnitEntry* unit_entries = (ChunkUnitEntry*)section_data->start_address;
	int unit_count = section_size / sizeof(ChunkUnitEntry);
	for (int i = 0; i < unit_count; i++)
	{
		ChunkUnitEntry* unit_entry = unit_entries + i;

		if (!CHK_UNIT_StartLocationSub_(startPositions, unit_entry)
			&& (unit_entry->properties.player >= 8u || Players[unit_entry->properties.player].nType != PT_NotUsed && (Players[unit_entry->properties.player].nType <= PT_Unknown0 || Players[unit_entry->properties.player].nType == PT_Neutral))
			&& unitIsNeutral(unit_entry)
			&& (gameData.got_file_values.victory_conditions
				|| gameData.got_file_values.starting_units
				|| gameData.got_file_values.tournament_mode
				|| !getPlayerForce(unit_entry->properties.player)
				|| (Unit_GroupFlags[unit_entry->unit_type] & 0x80u) != 0))
		{
			CUnit* v11 = sub_4CD740(unit_entry);
			if (v11)
			{
				v19 = CHK_UNIT_Nydus(unit_entry->linked_unit_id, v11, v19, unit_entry->id);
				v18 = CHK_UNIT_Addon(v11, v18, unit_entry->linked_unit_id, unit_entry->id);
			}
		}
	}

	for (; v19; v19 = sub_4CBD30(v19));
	for (; v18; v18 = sub_4CCF90(v18));
	for (CUnit* i = UnitNodeList_VisibleUnit_First; i; i = i->next)
	{
		UpdateUnitSpriteInfo(i);
	}
	CanUpdatePoweredStatus = 1;

	return 1;
}

FAIL_STUB_PATCH(ChkLoader_UNIT);

bool __stdcall ChkLoader_UPRP_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 1280)
	{
		return 0;
	}
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}
	memcpy(stru_596CD8, section_data->start_address, section_data->size);
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_UPRP);

bool __stdcall ChkLoader_COLR_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 8)
	{
		return 0;
	}

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}

	setPlayerColours(8, (char*) section_data->start_address);
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_COLR);

void __fastcall FreeCreepData_(bool exit_code)
{
	if (location)
	{
		SMemFree(location, "Starcraft\\SWAR\\MapComn\\creep.cpp", 368, 0);
	}
	if (CreepEdgeData)
	{
		SMemFree(CreepEdgeData, "Starcraft\\SWAR\\MapComn\\creep.cpp", 369, 0);
	}
	if (TerrainGraphics)
	{
		SMemFree(TerrainGraphics, "Starcraft\\SWAR\\MapComn\\creep.cpp", 370, 0);
	}
}

FAIL_STUB_PATCH(FreeCreepData);

void InitTerrainGraphicsAndCreep_(struct_a1* a1, TileID* a2, int a3, int a4, void* a5)
{
	dword_6D0C74 = a1->isCreepCovered;
	dword_6D0C70 = a1->pfunc0;
	dword_6D0C7C = a1->pfuncC;
	dword_6D0E84 = a2;
	dword_6D0F08 = a3;

	int v8 = a4;
	dword_6D0C78 = a1->isTileVisible;
	dword_6D0C6C = a4;
	if (location)
	{
		SMemFree(location, "Starcraft\\SWAR\\MapComn\\creep.cpp", 419, 0);
		v8 = dword_6D0C6C;
	}
	location = (TileID*)SMemAlloc(2 * dword_6D0F08 * v8, "Starcraft\\SWAR\\MapComn\\creep.cpp", 420, 8);
	if (CreepEdgeData)
	{
		SMemFree(CreepEdgeData, "Starcraft\\SWAR\\MapComn\\creep.cpp", 423, 0);
	}
	CreepEdgeData = (u8*)SMemAlloc(dword_6D0F08 * dword_6D0C6C, "Starcraft\\SWAR\\MapComn\\creep.cpp", 424, 8);
	if (TerrainGraphics)
	{
		SMemFree(TerrainGraphics, "Starcraft\\SWAR\\MapComn\\creep.cpp", 427, 0);
	}
	TerrainGraphics = readCreepFile((char*)a5);
	if (!dword_6D6414)
	{
		dword_6D6414 = 1;
		sub_413C50();
		sub_413C00();
		AppAddExit_(FreeCreepData_);
	}
}

FAIL_STUB_PATCH(InitTerrainGraphicsAndCreep);

void loadParallaxStarGfx_(const char* parallaxFile)
{
	char parallaxFilePath[MAX_PATH];
	snprintf(parallaxFilePath, MAX_PATH, "parallax\\%s.spk", parallaxFile);

	parallaxSomethingWidth = 165888;
	parallaxSomethingHeight = 124928;

	HANDLE hFile;
	if (!SFileOpenFileEx(0, parallaxFilePath, 0, &hFile))
	{
		SysWarn_FileNotFound(parallaxFilePath, SErrGetLastError());
		throw std::exception("Could not find SPK file");
	}
	LONG fileSize = SFileGetFileSize(hFile, 0);
	if (fileSize == -1)
	{
		FileFatal(hFile, GetLastError());
		return;
	}
	if (!fileSize)
	{
		fileSize = 24;
		SysWarn_FileNotFound(parallaxFilePath, 24);
	}
	void* spkData = SMemAlloc(fileSize, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210, 0);
	int read;
	if (SFileReadFile(hFile, spkData, fileSize, &read, 0))
	{
		if (read != fileSize)
		{
			FileFatal(hFile, 24);
			return;
		}
		SFileCloseFile(hFile);
		int numberOfLayers = *(unsigned __int16*)spkData;
		int v8 = 0;
		spkHandle = spkData;
		if (numberOfLayers)
		{
			u16* v10 = (u16*)spkData;
			for (int layerIndex = 0; layerIndex < numberOfLayers; ++layerIndex) {
				++v10;
				spkLayer[layerIndex] = *v10;
				v8 += *v10;
			}
		}
		int v12 = (int)spkData + 2 * numberOfLayers + 2;
		if (v8 > 0)
		{
			_DWORD* v13 = (_DWORD*)(v12 + 4);
			do
			{
				*v13 += (int)spkData;
				v13 += 2;
				--v8;
			} while (v8);
		}
		dword_658AAC = v12 + 8 * spkLayer[0];
		dword_658AA8 = v12;
		dword_658AB0 = dword_658AAC + 8 * spkLayer[1];
		dword_658AB4 = dword_658AB0 + 8 * spkLayer[2];
		dword_658AB8 = dword_658AB0 + 8 * spkLayer[2] + 8 * spkLayer[3];
	}
	else
	{
		DWORD last_error = GetLastError();
		FileFatal(hFile, last_error == 38 ? 24 : last_error);
	}
}

FAIL_STUB_PATCH(loadParallaxStarGfx);

void GenerateMegatileDefaultFlags_()
{
	megatile_default_flags = (MegatileFlags*)SMemAlloc(4 * megatileCount, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 195, 8);

	for (int megatile_index = 0; megatile_index < megatileCount; megatile_index++)
	{
		int v1 = 0;
		unsigned v2 = 0;
		int mid_elevation_minitiles = 0;
		int high_elevation_minitiles = 0;
		MiniTileFlagArray minitile_flags = MiniTileFlags->tile[megatile_index];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				u16 v9 = minitile_flags.miniTile[4 * i + j];
				if (v9 & 1)
				{
					++v2;
				}
				if (v9 & 4)
				{
					++high_elevation_minitiles;
				}
				if (v9 & 2)
				{
					++mid_elevation_minitiles;
				}
				if (v9 & 8)
				{
					++v1;
				}
			}
		}

		MegatileFlags v18 = (MegatileFlags)0;
		if (v2 > 0xC)
		{
			v18 |= MORE_THAN_12_WALKABLE;
		}
		else
		{
			v18 |= LESS_THAN_13_WALKABLE;
		}
		if (v2 && v2 < 0x10)
		{
			v18 |= CLIFF_EDGE;
		}
		if (high_elevation_minitiles >= 12)
		{
			v18 |= MORE_THAN_12_HIGH_HEIGHT;
		}
		else if (mid_elevation_minitiles + high_elevation_minitiles >= 12)
		{
			v18 |= MORE_THAN_12_MEDIUM_HEIGHT;
		}
		if (v1)
		{
			v18 |= HAS_RAMP;
		}
		megatile_default_flags[megatile_index] = v18;
	}
}

FAIL_STUB_PATCH(GenerateMegatileDefaultFlags);

void initMapData_()
{
	char filename[MAX_PATH];
	int bytes_read;

	word_6556FC = 0;
	byte_66FF5C = 0;
	MapTileArray = (TileID *)SMemAlloc(MAX_MAP_DIMENTION * MAX_MAP_DIMENTION * sizeof(TileID), "Starcraft\\SWAR\\lang\\Gamemap.cpp", 603, 0);
	CellMap = (__int16*)SMemAlloc(MAX_MAP_DIMENTION * MAX_MAP_DIMENTION * sizeof(*CellMap), "Starcraft\\SWAR\\lang\\Gamemap.cpp", 604, 0);
	GameTerrainCache = (byte *)SMemAlloc(TILE_CACHE_SIZE, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 605, 0);
	active_tiles = (MegatileFlags*)SMemAlloc(MAX_MAP_DIMENTION * MAX_MAP_DIMENTION * sizeof(*active_tiles), "Starcraft\\SWAR\\lang\\Gamemap.cpp", 606, 0);
	memset(active_tiles, 0, MAX_MAP_DIMENTION * MAX_MAP_DIMENTION * sizeof(*active_tiles));
	dword_6D5CD8 = SMemAlloc(29241, "Starcraft\\SWAR\\lang\\repulse.cpp", 323, 8);

	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".wpe");
	fastFileRead_(0, 0, filename, (int)palette, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".vf4");
	MiniTileFlags = (MiniTileMaps_type *)fastFileRead_(&bytes_read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	megatileCount = bytes_read / sizeof(MiniTileFlagArray);

	GenerateMegatileDefaultFlags_();
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".cv5");
	TileSetMap = (TileType *)fastFileRead_(&bytes_read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	TileSetMapSize = bytes_read / sizeof(TileType);

	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".grp");

	struct_a1 a1;
	a1.pfunc0 = sub_47E2D0;
	a1.isCreepCovered = isCreepCovered;
	a1.isTileVisible = isTileVisible;
	a1.pfuncC = 0;
	InitTerrainGraphicsAndCreep_(&a1, MapTileArray, map_size.width, map_size.height, filename);
	ZergCreepArray = location;

	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".vx4");
	VX4Data = (vx4entry *)fastFileRead_(&bytes_read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".vr4");
	VR4Data = (vr4entry*) fastFileRead_(0, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (!dword_5993AC)
	{
		memcpy(stru_6CEB40, palette, sizeof(PALETTEENTRY[256]));
		sub_4BCD70(palette);
		sub_4BDD60();
	}
	loadColorShiftTilesetImages(TILESET_NAMES[CurrentTileSet]);
	sub_4BDDD0(TILESET_NAMES[CurrentTileSet]);
	if (!dword_5993AC)
	{
		ScreenLayers[5].buffers = 1;
		sub_480960();
		InitializeGameLayer_();
	}
	byte_658AC0 = 0;
	dword_658AA4 = 0;
	loadParallaxStarGfx_("star");
	sub_47D660();
}

FAIL_STUB_PATCH(initMapData);

unsigned int GetGroundHeightAtPos_(int x, int y)
{
	int megatile_index = x / TILE_WIDTH + y / TILE_HEIGHT * map_size.width;
	TileID megatile = *(u16*)&ZergCreepArray[megatile_index] ? ZergCreepArray[megatile_index] : MapTileArray[megatile_index];

	u16 v1 = TileSetMap[megatile.group].megaTileRef[megatile.tile];
	u16 v2 = MiniTileFlags->tile[v1].miniTile[4 * ((y >> 3) & 3) + ((x >> 3) & 3)];

	int ground_height = (v2 & 6) >> 1;
	return min(ground_height, 2); // TODO: allow a fourth ground height level
}

int GetGroundHeightAtPos__()
{
	int x, y;

	__asm {
		mov x, ecx
		mov y, eax
	}

	return GetGroundHeightAtPos_(x, y);
}

FUNCTION_PATCH((void*)0x4BD0F0, GetGroundHeightAtPos__);

int isUnitAtHeight_(CUnit* unit, char location_flags)
{
	Position position = unit->sprite->position;
	unsigned ground_height = GetGroundHeightAtPos_(position.x, position.y);

	if (unit->statusFlags & StatusFlags::InAir)
	{
		switch (ground_height)
		{
		case 0: return location_flags & 8;
		case 1: return location_flags & 0x10;
		case 2: return location_flags & 0x20;
		}
	}
	else
	{
		switch (ground_height)
		{
		case 0: return location_flags & 1;
		case 1: return location_flags & 2;
		case 2: return location_flags & 4;
		}
	}
	return 0;
}

int __cdecl isUnitAtHeight__()
{
	CUnit* unit;
	int location_flags;

	__asm {
		mov unit, eax
		mov location_flags, esi
	}

	return isUnitAtHeight_(unit, location_flags);
}

FUNCTION_PATCH((void*)0x45F8D0, isUnitAtHeight__);

int revealSightAtLocation_(int sight_range, MegatileFlags vision_mask, signed int x, signed int y, int reveal_from_air)
{
	if (vision_mask <= 0xFF)
	{
		int(__fastcall * *v14)(int, int, SightStruct*, MegatileFlags*, unsigned int, unsigned int);

		v14 = &off_504524;
		MegatileFlags v15 = vision_mask;
		if (reveal_from_air)
		{
			v14 = &off_504528;
		}
		else
		{
			switch (GetGroundHeightAtPos_(x, y))
			{
			case 0:
				v15 |= MORE_THAN_12_MEDIUM_HEIGHT;
				[[fallthrough]];
			case 1:
				v15 |= MORE_THAN_12_HIGH_HEIGHT;
				[[fallthrough]];
			default:
				v15 |= HAS_RAMP;
			}
		}

		SightStruct* v6 = &line_of_sight[sight_range];
		int v11 = v6->tileSightWidth;
		int v13 = v6->tileSightHeight;
		if (x / TILE_WIDTH - v11 / 2 < 0 || x / TILE_WIDTH + v11 / 2 >= map_size.width ||
			y / TILE_HEIGHT - v13 / 2 < 0 || y / TILE_HEIGHT + v13 / 2 >= map_size.height)
		{
			v14 += 2;
		}
		unsigned v7 = ~vision_mask & ~(vision_mask << 8);
		sight_range = (*v14)(x / TILE_WIDTH, y / TILE_HEIGHT, v6, &active_tiles[x / TILE_WIDTH + (y / TILE_HEIGHT) * map_size.width], v15, v7);
	}
	return sight_range;
}

int __stdcall revealSightAtLocation__(signed int x, signed int y, int reveal_from_air)
{
	int sight_range;
	MegatileFlags vision_mask;

	__asm {
		mov sight_range, eax
		mov vision_mask, ecx
	}

	return revealSightAtLocation_(sight_range, vision_mask, x, y, reveal_from_air);
}

FUNCTION_PATCH((void*)0x4806F0, revealSightAtLocation__);

void sub_4CC990_()
{
	char buff[MAX_PATH];
	char dest[MAX_PATH];
	int loader_index;

	if (CampaignIndex)
	{
		SStrCopy(dest, CurrentMapFileName, MAX_PATH);
	}
	else
	{
		dest[0] = 0;
		if (!LoadFileArchiveToSBigBuf(CurrentMapFileName, &loader_index, 1, &mapArchiveHandle))
			return;
	}
	int chk_size = 0;
	if (dest[0])
		_snprintf(buff, MAX_PATH, "%s\\%s", dest, "staredit\\scenario.chk");
	else
		SStrCopy(buff, "staredit\\scenario.chk", MAX_PATH);
	void* chk_data = fastFileRead_(&chk_size, 0, buff, 0, 1, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2095);
	if (chk_data)
	{
		loader_index = 0;
		if (ReadMapChunks_(0, chk_data, &loader_index, chk_size))
		{
			ReadChunkNodes_(chk_loaders_[loader_index].briefing_loader_count, chk_size, chk_loaders_[loader_index].briefing_loaders, chk_data, 0);
		}
		SMemFree(chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2113, 0);
	}
}

FUNCTION_PATCH(sub_4CC990, sub_4CC990_);

void init_gluesounds_()
{
	if (!dword_6D6364)
	{
		word_6DC2AC[0] = 165;
		word_6DC2AC[1] = 166;
		word_6DC2AC[2] = 167;
		word_6DC2AC[3] = 168;
		word_6DC2AC[4] = 169;
		word_6DC2AC[5] = 170;
		word_6DC2AC[6] = 171;
		word_6DC2AC[7] = 172;
		word_6DC2AC[8] = 173;
		dword_6D6364 = 1;
	}
	playsound_init_UI_(word_6DC2AC);
	ButtonPressSound = buttonMouseOver;
}

FAIL_STUB_PATCH(init_gluesounds);

int gluLoadBINDlg_(dialog* a1, FnInteract fn_interact)
{
	sub_4195E0();
	a1->lFlags |= DialogFlags::CTRL_LBOX_NORECALC;
	LastControlID = -3;
	InitializeDialog_(a1, fn_interact);
	while (true)
	{
		if (dword_6D5E38)
		{
			dword_6D5E38();
		}
		for (dialog* v3 = DialogList; v3; v3 = v3->pNext)
		{
			if (v3->fields.dlg.pModalFcn)
			{
				v3->fields.dlg.pModalFcn(v3);
			}
		}
		BWFXN_videoLoop_(3);
		BWFXN_RedrawTarget_();

		if (a1 == NULL)
		{
			return LastControlID;
		}

		for (dialog* v6 = DialogList; v6 != a1; v6 = v6->pNext)
		{
			if (v6 == NULL)
			{
				return LastControlID;
			}
		}
	}
}

void AnimateVideos_(dialog* result)
{
	for (dialog* i = result->fields.dlg.pFirstChild; i; i = i->pNext)
	{
		if (i->wCtrlType == DialogType::cFLCBTN)
		{
			SetCallbackTimer(72, i, 30, PlayVideoFrame);
		}
	}
}

FAIL_STUB_PATCH(AnimateVideos);

void registerMenuFunctions_(FnInteract* functions, dialog* a2, int functions_size)
{
	AnimateVideos_(a2);
	a2->lFlags |= CTRL_USELOCALGRAPHIC;
	if (functions)
	{
		registerUserDialogAction(a2, functions_size, functions);
	}
	if (dialog_count++ == 0)
	{
		if (!glue_background_palette[0].data || a2 == (dialog*)-12)
		{
			char dest[260];
			SStrCopy(dest, &aGluePalmm[1304 * stru_4FFAD0[glGluesMode].menu_position], 0x104u);
			SStrNCat(dest, "\\BackGnd.pcx", 260);
			AllocBackgroundImage(dest, &a2->srcBits, palette, "Starcraft\\SWAR\\lang\\glues.cpp", 1052);
			glue_background_palette[0].wid = a2->srcBits.wid;
			glue_background_palette[0].ht = a2->srcBits.ht;
			glue_background_palette[0].data = a2->srcBits.data;
			memcpy(glue_background_palette + 1, palette, sizeof(palette));
		}
		else
		{
			a2->srcBits.wid = glue_background_palette[0].wid;
			a2->srcBits.ht = glue_background_palette[0].ht;
			a2->srcBits.data = glue_background_palette[0].data;
			memcpy(palette, glue_background_palette + 1, sizeof(palette));
		}
		a2->lFlags |= CTRL_UNKOWN1;
		sub_41E450(sub_4BDB30, palette);
		sub_4178B0(palette, NULL);
		MenuPosition v9 = glGluesMode;
		getBGPalInfo(glGluesMode);
		MenuPosition v10 = stru_4FFAD0[v9].menu_position;
		DlgGrp* v11 = dword_51C5C8[v10];
		memcpy(&stru_6CE000, &dword_51C40C[v10], sizeof(stru_6CE000));
		sub_419290(v11);
		grpHead* v12 = (grpHead*) dword_50E170[326 * v10];
		setCursor_(v12);
		SetCallbackTimer(24, a2, 50, sub_4DCEA0);
		dword_6D5E20 = &a2->srcBits;
		dword_51C418 = a2;
		checkLastFileError();
		memset(is_keycode_used, 0, sizeof(is_keycode_used));
	}
	else
	{
		if (dword_51C418)
		{
			dlgEvent v14;
			v14.wNo = EVN_USER;
			v14.dwUser = USER_NEXT;
			v14.wSelection = 0;
			v14.wUnk_0x06 = 0;
			v14.cursor.x = Mouse.x;
			v14.cursor.y = Mouse.y;
			dword_51C418->pfcnInteract(dword_51C418, &v14);
		}
		a2->lFlags |= CTRL_UNKOWN4;
	}
}

void __stdcall registerMenuFunctions__(dialog* a2, int functions_size, int a4)
{
	FnInteract* functions;

	__asm mov functions, eax

	registerMenuFunctions_(functions, a2, functions_size);
}

// TODO: patch the remaining usages:
// gluModemList_CustomCtrlID, gluModemStatus_CustomCtrlID, gluModemEntry_CustomCtrlID, sub_4B6C70
FUNCTION_PATCH((void*) 0x4DD9E0, registerMenuFunctions__);

int CreateCampaignGame_(MapData mapData)
{
	MapChunks mapChunks;

	CampaignIndex = mapData;
	if (ReadCampaignMapData_(&mapChunks))
	{
		GameData v4;
		memset(&v4, 0, 140u);
		v4.got_file_values.unused3[4] = 0;
		SStrCopy(v4.player_name, playerName, sizeof(v4.player_name));
		SStrCopy(v4.map_name, CurrentMapName, sizeof(v4.map_name));
		v4.game_speed = registry_options.GameSpeed;
		v4.active_human_players = 1;
		v4.max_players = 1;

		GotFileValues* v2 = InitUseMapSettingsTemplate_();

		if (v2)
		{
			memcpy(&v4.got_file_values, v2, sizeof(v4.got_file_values));
			SMemFree(v2, "Starcraft\\SWAR\\lang\\uiSingle.cpp", 270, 0);
			if (sub_4DBE50())
			{
				isHost = 0;
				return CreateGame(&v4) != 0;
			}
		}
	}

	return 0;
}

FAIL_STUB_PATCH(CreateCampaignGame);

struct __declspec(align(2)) ExpandedCampaignMenuEntry
{
	unsigned __int16 glu_hist_tbl_index;
	ExpandedMapData next_mission;
	Cinematic cinematic;
	char _padding0;
	Race race;
	bool hide;
	const char* establishing_shot;
	const char* epilog;
};

MEMORY_PATCH(0x4B69CA, (BYTE) sizeof(ExpandedCampaignMenuEntry));

// Switch between building portraits in campaigns (Overmind/Daggoth and Aldaris/Fenix):
MEMORY_PATCH(0x45E350, (BYTE)EMD_protoss10);
MEMORY_PATCH(0x45F02A, (BYTE)EMD_protoss10);
MEMORY_PATCH(0x45E33F, (BYTE)EMD_protoss07);
MEMORY_PATCH(0x45F019, (BYTE)EMD_protoss07);

void gluHist_Activate_(dialog* dlg)
{
	if (LastControlID == 1)
	{
		dialog * v1 = getControlFromIndex(dlg, 6);

		if (v1 && v1->fields.list.bStrs)
		{
			u8 v2 = v1->fields.list.bCurrStr;
			if (v2 != 0xFF)
			{
				dword_6D5A48 = (CampaignMenuEntry*)((ExpandedCampaignMenuEntry*)dword_6D5A4C + v1->fields.list.pdwData[v2]);
			}
		}

	}
}

FAIL_STUB_PATCH(gluHist_Activate);

bool __fastcall Popup_Main_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			sub_4B6810(dlg);
			break;
		case USER_DESTROY:
			DestroyChildren(dlg);
			dword_6D5A3C = 0;
			dword_6D5A54 = 0;
			break;
		case USER_UNK_7:
			sub_4CD9C0(dlg);
			break;
		case USER_INIT:
			sub_4B6C70(dlg);
			break;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FUNCTION_PATCH(Popup_Main, Popup_Main_);

bool __fastcall gluHist_Interact_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gluHist_Create(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			gluHist_Activate_(dlg);
			break;
		}
	}
	else if (evt->wNo == EventNo::EVN_WHEELUP)
	{
		dlgEvent event;
		event.dwUser = USER_SCROLLUP;
		event.wSelection = 0;
		event.wUnk_0x06 = 0;
		event.wNo = EVN_USER;
		event.cursor.x = Mouse.x;
		event.cursor.y = Mouse.y;

		dialog* v4 = getControlFromIndex(dlg, 6)->fields.list.pScrlBar;
		v4->pfcnInteract(v4, &event);
		return 1;
	}
	else if (evt->wNo == EventNo::EVN_WHEELDWN)
	{
		dlgEvent event;
		event.dwUser = USER_SCROLLDOWN;
		event.wSelection = 0;
		event.wUnk_0x06 = 0;
		event.wNo = EVN_USER;
		event.cursor.x = Mouse.x;
		event.cursor.y = Mouse.y;

		dialog* v4 = getControlFromIndex(dlg, 6)->fields.list.pScrlBar;
		v4->pfcnInteract(v4, &event);
		return 1;
	}
	return Popup_Main_(dlg, evt);
}

FAIL_STUB_PATCH(gluHist_Interact);

BOOL sub_4B6530_(ExpandedCampaignMenuEntry* a1, unsigned int a2)
{
	unsigned i = 0;
	for (ExpandedMapData v2 = a1->next_mission; v2; ++a1)
	{
		if (v2 <= a2 && a1->glu_hist_tbl_index)
		{
			++i;
		}
		v2 = a1[1].next_mission;
	}
	return i > 1;
}

FAIL_STUB_PATCH(sub_4B6530);

ExpandedCampaignMenuEntry* loadmenu_GluHist_(int a1, ExpandedCampaignMenuEntry* a2)
{
	if (!sub_4B6530_(a2, a1))
	{
		return a2;
	}

	dword_6D5A48 = 0;
	dword_6D5A4C = (CampaignMenuEntry*) a2;
	dword_6D5A50 = a1;
	dword_6D5A40 = off_51A69C;
	dword_599D98 = 28;
	dword_6D5A44 = (BYTE*)fastFileRead_(0, 0, "rez\\gluHist.tbl", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	AllocBackgroundImage("glue\\campaign\\pHist.pcx", &p_hist_pcx, palette, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 602);

	dword_6D5A3C = LoadDialog("rez\\gluHist.bin");

	gluLoadBINDlg_(dword_6D5A3C, gluHist_Interact_);
	if (dword_6D5A44)
	{
		SMemFree(dword_6D5A44, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 609, 0);
		dword_6D5A44 = NULL;
	}

	return (ExpandedCampaignMenuEntry*) dword_6D5A48;
}

FAIL_STUB_PATCH(loadmenu_GluHist);

ExpandedCampaignMenuEntry terran_swcampaign_menu_entries_[] = {
	{0x33, ExpandedMapData::EMD_swtutorial, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "Estt0tsw"},
	{0x34, ExpandedMapData::EMD_swterran01, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "Estt01sw"},
	{0x35, ExpandedMapData::EMD_swterran02, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "Estt02sw"},
	{0x36, ExpandedMapData::EMD_swterran03, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "Estt03sw"},
	{0x37, ExpandedMapData::EMD_swterran04, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "Estt04sw"},
	{0x38, ExpandedMapData::EMD_swterran05, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "Estt05sw"},
	{0},
};

ExpandedCampaignMenuEntry zerg_campaign_menu_entries_[] = {
	{0x20, ExpandedMapData::EMD_zerg01, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ01"},
	{0x21, ExpandedMapData::EMD_zerg02, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ02"},
	{0x22, ExpandedMapData::EMD_zerg03, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ03"},
	{0x2A, ExpandedMapData::EMD_zerg04, C_THE_DREAM, 0, RACE_Zerg, 0},
	{0x23, ExpandedMapData::EMD_zerg04, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ04"},
	{0x24, ExpandedMapData::EMD_zerg05, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ05"},
	{0x2B, ExpandedMapData::EMD_zerg06, C_BATTLE_ON_THE_AMERIGO_INTRO, 0, RACE_Zerg, 0},
	{0, ExpandedMapData::EMD_zerg06, C_BATTLE_ON_THE_AMERIGO, 0, RACE_Zerg, 1},
	{0x25, ExpandedMapData::EMD_zerg06, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ06"},
	{0x26, ExpandedMapData::EMD_zerg07, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ07"},
	{0x27, ExpandedMapData::EMD_zerg08, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ08"},
	{0x2C, ExpandedMapData::EMD_zerg09, C_THE_WARP_INTRO, 0, RACE_Zerg, 0},
	{0, ExpandedMapData::EMD_zerg09, C_THE_WARP, 0, RACE_Zerg, 1},
	{0x28, ExpandedMapData::EMD_zerg09, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ09"},
	{0x29, ExpandedMapData::EMD_zerg10, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ10"},
	{0x2D, ExpandedMapData::EMD_protoss01, C_THE_INVASION_OF_AIUR_INTRO, 0, RACE_Zerg, 0},
	{0, ExpandedMapData::EMD_protoss01, C_THE_INVASION_OF_AIUR, 0, RACE_Zerg, 1},
	{0},
};

ExpandedCampaignMenuEntry terran_campaign_menu_entries_[] = {
	{1, ExpandedMapData::EMD_tutorial, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT0t"},
	{2, ExpandedMapData::EMD_terran01, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT01"},
	{3, ExpandedMapData::EMD_terran02, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT02"},
	{0xC, ExpandedMapData::EMD_terran03, C_WASTELAND_PATROL_INTRO, 0, RACE_Terran, 0},
	{0, ExpandedMapData::EMD_terran03, C_WASTELAND_PATROL, 0, RACE_Terran, 1},
	{4, ExpandedMapData::EMD_terran03, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT03"},
	{5, ExpandedMapData::EMD_terran04, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT04"},
	{6, ExpandedMapData::EMD_terran05, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT05"},
	{0xD, ExpandedMapData::EMD_terran06, C_THE_DOWNING_OF_NORAD_II_INTRO, 0, RACE_Terran, 0},
	{0, ExpandedMapData::EMD_terran06, C_THE_DOWNING_OF_NORAD_II, 0, RACE_Terran, 1},
	{7, ExpandedMapData::EMD_terran06, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT06"},
	{8, ExpandedMapData::EMD_terran08, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT08"},
	{9, ExpandedMapData::EMD_terran09, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT09"},
	{0xE, ExpandedMapData::EMD_terran11, C_OPEN_REBELION_INTRO, 0, RACE_Terran, 0},
	{0, ExpandedMapData::EMD_terran11, C_OPEN_REBELION, 0, RACE_Terran, 1},
	{0xA, ExpandedMapData::EMD_terran11, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT11"},
	{0xB, ExpandedMapData::EMD_terran12, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT12"},
	{0xF, ExpandedMapData::EMD_zerg01, C_THE_INAUGURATION_INTRO, 0, RACE_Terran, 0},
	{0, ExpandedMapData::EMD_zerg01, C_THE_INAUGURATION, 0, RACE_Terran, 1},
	{0},
};

ExpandedCampaignMenuEntry protoss_campaign_menu_entries_[] = {
	{0x11, ExpandedMapData::EMD_protoss01, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP01"},
	{0x12, ExpandedMapData::EMD_protoss02, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP02"},
	{0x13, ExpandedMapData::EMD_protoss03, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP03"},
	{0x1B, ExpandedMapData::EMD_protoss04, C_THE_FALL_OF_FENIX_INTRO, 0, RACE_Protoss, 0},
	{0, ExpandedMapData::EMD_protoss04, C_THE_FALL_OF_FENIX, 0, RACE_Protoss, 1},
	{0x14, ExpandedMapData::EMD_protoss04, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP04"},
	{0x15, ExpandedMapData::EMD_protoss05, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP05"},
	{0x1C, ExpandedMapData::EMD_protoss06, C_THE_AMBUSH_INTRO, 0, RACE_Protoss, 0},
	{0, ExpandedMapData::EMD_protoss06, C_THE_AMBUSH, 0, RACE_Protoss, 1},
	{0x16, ExpandedMapData::EMD_protoss06, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP06"},
	{0x17, ExpandedMapData::EMD_protoss07, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP07"},
	{0x1D, ExpandedMapData::EMD_protoss08, C_THE_RETURN_TO_AIUR_INTRO, 0, RACE_Protoss, 0},
	{0, ExpandedMapData::EMD_protoss08, C_THE_RETURN_TO_AIUR, 0, RACE_Protoss, 1},
	{0x18, ExpandedMapData::EMD_protoss08, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP08"},
	{0x19, ExpandedMapData::EMD_protoss09, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP09"},
	{0x1A, ExpandedMapData::EMD_protoss10, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP10"},
	{0x1E, ExpandedMapData::EMD_xprotoss01, C_THE_DEATH_OF_THE_OVERMIND, 0, RACE_Protoss, 0},
	{0},
};

ExpandedCampaignMenuEntry zerg_expcampaign_menu_entries_[] = {
	{0x4A, ExpandedMapData::EMD_xzerg01, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ01x"},
	{0x4B, ExpandedMapData::EMD_xzerg02, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ02x"},
	{0x4C, ExpandedMapData::EMD_xzerg03, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ03x"},
	{0x4D, ExpandedMapData::EMD_xzerg04a, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ04x"},
	{0, ExpandedMapData::EMD_xzerg04b, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1, "EstZ04x"},
	{0, ExpandedMapData::EMD_xzerg04c, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1, "EstZ04x"},
	{0, ExpandedMapData::EMD_xzerg04d, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1, "EstZ04x"},
	{0, ExpandedMapData::EMD_xzerg04e, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1, "EstZ04x"},
	{0, ExpandedMapData::EMD_xzerg04f, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1, "EstZ04x"},
	{0x4E, ExpandedMapData::EMD_xzerg05, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ05x"},
	{0x4F, ExpandedMapData::EMD_xzerg06, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ06x"},
	{0x50, ExpandedMapData::EMD_xzerg07, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ07x"},
	{0x51, ExpandedMapData::EMD_xzerg08, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ08x"},
	{0x52, ExpandedMapData::EMD_xzerg09, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ09x"},
	{0, ExpandedMapData::EMD_xbonus, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1, "EstZ09bx", "FinZ09bx"},
	{0x54, ExpandedMapData::EMD_xzerg10, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0, "EstZ10x"},
	{0x55, ExpandedMapData::EMD_Unknown, C_THE_ASCENTION, 0, RACE_Zerg, 0},
	{0},
};

ExpandedCampaignMenuEntry terran_expcampaign_menu_entries_[] = {
	{0x41, ExpandedMapData::EMD_xterran01, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT01x"},
	{0x42, ExpandedMapData::EMD_xterran02, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT02x"},
	{0x43, ExpandedMapData::EMD_xterran03, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT03x"},
	{0x44, ExpandedMapData::EMD_xterran04, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT04x"},
	{0x45, ExpandedMapData::EMD_xterran05a, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT05ax"},
	{0x46, ExpandedMapData::EMD_xterran05b, C_BLIZZARD_LOGO, 0, RACE_Terran, 1, "EstT05bx"},
	{0x47, ExpandedMapData::EMD_xterran06, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT06x"},
	{0x48, ExpandedMapData::EMD_xterran07, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT07x"},
	{0x49, ExpandedMapData::EMD_xterran08, C_BLIZZARD_LOGO, 0, RACE_Terran, 0, "EstT08x"},
	{0x57, ExpandedMapData::EMD_xzerg01, C_UED_VICTORY_REPORT, 0, RACE_Terran, 0},
	{0},
};

ExpandedCampaignMenuEntry protoss_expcampaign_menu_entries_[] = {
	{0x39, ExpandedMapData::EMD_xprotoss01, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP01x"},
	{0x3A, ExpandedMapData::EMD_xprotoss02, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP02x"},
	{0x3B, ExpandedMapData::EMD_xprotoss03, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP03x"},
	{0x3C, ExpandedMapData::EMD_xprotoss04, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP04x"},
	{0x3D, ExpandedMapData::EMD_xprotoss05, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP05x"},
	{0x3E, ExpandedMapData::EMD_xprotoss06, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP06x"},
	{0x3F, ExpandedMapData::EMD_xprotoss07, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP07x"},
	{0x40, ExpandedMapData::EMD_xprotoss08, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0, "EstP08x"},
	{0x56, ExpandedMapData::EMD_xterran01, C_FURY_OF_THE_XEL_NAGA, 0, RACE_Protoss, 0},
	{0},
};

struct Campaign
{
	const char* campaign_id;
	int first_mission_index;
	bool is_expansion;
	Race race;
	ExpandedCampaignMenuEntry* entries;
	std::vector<const char*> epilogs;
	MusicTrack epilog_music_track;
	MenuPosition post_epilog_menu;
};

std::vector<Campaign> campaigns = {
	{
		"swterran",
		0,
		false,
		Race::RACE_Terran,
		terran_swcampaign_menu_entries_,
		{"epilogsw"},
		MusicTrack::MT_TERRAN2,
		MenuPosition::GLUE_CAMPAIGN,
	},
	{
		"terran",
		0,
		false,
		Race::RACE_Terran,
		terran_campaign_menu_entries_,
		{},
		MusicTrack::MT_NONE,
		MenuPosition::GLUE_CAMPAIGN,
	},
	{
		"zerg",
		1,
		false,
		Race::RACE_Zerg,
		zerg_campaign_menu_entries_,
		{},
		MusicTrack::MT_NONE,
		MenuPosition::GLUE_CAMPAIGN,
	},
	{
		"protoss",
		1,
		false,
		Race::RACE_Protoss,
		protoss_campaign_menu_entries_,
		{"epilog", "crdt_lst"},
		MusicTrack::MT_PROTOSS3,
		MenuPosition::GLUE_MAIN_MENU,
	},
	{
		"xprotoss",
		1,
		true,
		Race::RACE_Protoss,
		protoss_expcampaign_menu_entries_,
		{},
		MusicTrack::MT_NONE,
		MenuPosition::GLUE_EX_CAMPAIGN,
	},
	{
		"xterran",
		1,
		true,
		Race::RACE_Terran,
		terran_expcampaign_menu_entries_,
		{},
		MusicTrack::MT_NONE,
		MenuPosition::GLUE_EX_CAMPAIGN,
	},
	{
		"xzerg",
		1,
		true,
		Race::RACE_Zerg,
		zerg_expcampaign_menu_entries_,
		{"epilogX", "crdt_exp"},
		MusicTrack::MT_PROTOSS3,
		MenuPosition::GLUE_MAIN_MENU,
	},
};

std::vector<Campaign*> campaigns_by_race = { &campaigns[2], &campaigns[1], &campaigns[3] };
std::vector<Campaign*> expcampaigns_by_race = { &campaigns[6], &campaigns[5], &campaigns[4] };

int parseCmpgnCheatTypeString_(Campaign* campaign, char* campaign_index, ExpandedMapData* a5)
{
	char* campaign_index_ = campaign_index;
	int v5 = strtoul(campaign_index, &campaign_index_, 10) - campaign->first_mission_index;
	if (v5 < 0)
	{
		return 0;
	}

	ExpandedCampaignMenuEntry* v8 = campaign->entries;
	if (v8->next_mission == MD_none)
	{
		return 0;
	}
	while (v8->cinematic || v8->hide || v5--)
	{
		++v8;
		if (v8->next_mission == MD_none)
		{
			return 0;
		}
	}
	if (campaign_index_ && *campaign_index_)
	{
		int v10 = tolower(*campaign_index_) - 'a';
		while (v10)
		{
			--v10;
			if (v8->next_mission == MD_none)
			{
				return 0;
			}
			++v8;
			if (!v8->hide)
			{
				return 0;
			}
		}
		if (v8->next_mission == MD_none)
		{
			return 0;
		}
	}
	*a5 = v8->next_mission;
	return 1;
}

FAIL_STUB_PATCH(parseCmpgnCheatTypeString);

void ContinueCampaignWithLevelCheat_(ExpandedMapData mission, bool is_expansion, Race race)
{
	Ophelia = 1;
	level_cheat_mission = (MapData4)mission;
	level_cheat_race = race;
	level_cheat_is_bw = is_expansion;
}

FAIL_STUB_PATCH(ContinueCampaignWithLevelCheat);

int campaignTypeCheatStrings_(char* a2)
{
	if (multiPlayerMode || (GameCheats & CheatFlags::CHEAT_Ophelia) == 0)
	{
		return 0;
	}

	Campaign* relevant_campaign = NULL;
	for (Campaign& campaign : campaigns)
	{
		int prefix_length = SStrLen(campaign.campaign_id);
		if (!SStrCmpI(a2, campaign.campaign_id, prefix_length))
		{
			relevant_campaign = &campaign;
			break;
		}
	}
	if (relevant_campaign == NULL)
	{
		return 0;
	}

	ExpandedMapData mission;
	int prefix_length = SStrLen(relevant_campaign->campaign_id);
	if (parseCmpgnCheatTypeString_(relevant_campaign, a2 + prefix_length, &mission) && mission != EMD_xbonus)
	{
		ContinueCampaignWithLevelCheat_(mission, relevant_campaign->is_expansion, relevant_campaign->race);
		if (gwGameMode == GAME_RUN)
		{
			GameState = 0;
			gwNextGameMode = GAME_GLUES;
			if (!InReplay)
			{
				replay_header.ReplayFrames = ElapsedTimeFrames;
				glGluesMode = GLUE_MAIN_MENU;
				return 1;
			}
		}
		else
		{
			gwGameMode = GAME_GLUES;
		}
		glGluesMode = GLUE_MAIN_MENU;
	}
	return 1;
}

__declspec(naked) int campaignTypeCheatStrings__()
{
	char* a2;

	__asm {
		push ebp
		mov ebp, esp
		mov a2, edi
	}

	campaignTypeCheatStrings_(a2);

	__asm {
		mov eax, eax // Put the result in the correct register
		mov esp, ebp
		pop ebp
		ret
	}
}

FUNCTION_PATCH((void*) 0x4b1dc0, campaignTypeCheatStrings__);

ExpandedCampaignMenuEntry* getCampaignIndex_(Campaign& campaign)
{
	ExpandedCampaignMenuEntry* entry = campaign.entries;

	while (entry->cinematic || entry->next_mission != CampaignIndex)
	{
		++entry;
		if (entry->next_mission == EMD_none)
		{
			return 0;
		}
	}
	return entry;
}

FAIL_STUB_PATCH(getCampaignIndex);

void updateActiveCampaignMission_()
{
	if (active_campaign_menu_entry == NULL || active_campaign_menu_entry->next_mission != CampaignIndex)
	{
		for (Campaign& campaign : campaigns)
		{
			for (ExpandedCampaignMenuEntry* entry = campaign.entries; entry->next_mission; entry++)
			{
				if (entry->next_mission == CampaignIndex)
				{
					active_campaign_menu_entry = (CampaignMenuEntry*) getCampaignIndex_(campaign);
					return;
				}
			}
		}
	}
}

FAIL_STUB_PATCH(updateActiveCampaignMission);

bool LoadCampaignWithCharacter_(Race race)
{
	customSingleplayer[0] = 0;
	dword_51CA1C = 0;
	CharacterData character_data;
	if (!LoadCharacterData(&character_data, playerName))
	{
		const char* v1 = GetNetworkTblString(72);
		if ((_stricmp(playerName, v1) || !verifyCharacterFile(&character_data, playerName)) && !outOfGame)
		{
			doNetTBLError(0, 0, 0, 88);
		}
	}

	ExpandedCampaignMenuEntry* v2;
	int* unlocked_mission;
	if (IsExpansion)
	{
		v2 = expcampaigns_by_race[race]->entries;
		unlocked_mission = &character_data.unlocked_expcampaign_mission[race];
	}
	else
	{
		v2 = campaigns_by_race[race]->entries;
		unlocked_mission = &character_data.unlocked_campaign_mission[race];
	}
	active_campaign_menu_entry = (CampaignMenuEntry*) loadmenu_GluHist_(*unlocked_mission, v2);
	if (active_campaign_menu_entry)
	{
		if (*unlocked_mission < active_campaign_menu_entry->next_mission)
		{
			*unlocked_mission = active_campaign_menu_entry->next_mission;
			CreateCharacterFile(&character_data);
		}

		if (active_campaign_menu_entry->cinematic)
		{
			active_cinematic = active_campaign_menu_entry->cinematic;
			CampaignIndex = active_campaign_menu_entry->next_mission;
			next_scenario[0] = 0;
			gwGameMode = GAME_CINEMATIC;
		}
		else
		{
			CreateCampaignGame_(active_campaign_menu_entry->next_mission);
		}
	}
	return active_campaign_menu_entry != NULL;
}

FAIL_STUB_PATCH(LoadCampaignWithCharacter);

bool LoadPrecursorCampaign()
{
	customSingleplayer[0] = 0;
	dword_51CA1C = 0;
	CharacterData character_data;
	if (!LoadCharacterData(&character_data, playerName))
	{
		const char* v1 = GetNetworkTblString(72);
		if ((_stricmp(playerName, v1) || !verifyCharacterFile(&character_data, playerName)) && !outOfGame)
		{
			doNetTBLError(0, 0, 0, 88);
		}
	}

	ExpandedCampaignMenuEntry* v2 = campaigns[0].entries;
	int unlocked_mission = 6;
	active_campaign_menu_entry = (CampaignMenuEntry*) loadmenu_GluHist_(unlocked_mission, v2);
	if (active_campaign_menu_entry)
	{
		if (active_campaign_menu_entry->cinematic)
		{
			active_cinematic = active_campaign_menu_entry->cinematic;
			CampaignIndex = active_campaign_menu_entry->next_mission;
			next_scenario[0] = 0;
			gwGameMode = GAME_CINEMATIC;
		}
		else
		{
			CreateCampaignGame_(active_campaign_menu_entry->next_mission);
		}
	}
	return active_campaign_menu_entry != NULL;
}

int sub_4B5110_(Race race)
{
	if (dword_59A0D4[race])
	{
		const char* v3 = GetNetworkTblString(race != Race::RACE_Terran ? 144 : 143);

		if (!sub_4B5B20(v3))
		{
			return 0;
		}
	}

	return LoadCampaignWithCharacter_(race);
}

FAIL_STUB_PATCH(sub_4B5110);

bool sub_4B27A0_(Race race)
{
	if (dword_59B760[race])
	{
		const char* v3 = GetNetworkTblString(race == Race::RACE_Protoss ? 142 : 141);

		if (!sub_4B5B20(v3))
		{
			return 0;
		}
	}

	return LoadCampaignWithCharacter_(race);
}

FAIL_STUB_PATCH(sub_4B27A0);

bool sub_4B5180_(dialog* a1)
{
	switch (LastControlID)
	{
	case 6:
		if (!sub_4B5110_(Race::RACE_Protoss))
		{
			return true;
		}
		LastControlID = 6;
		break;
	case 7:
		if (!sub_4B5110_(Race::RACE_Terran))
		{
			return true;
		}
		LastControlID = 7;
		break;
	case 8:
		if (!sub_4B5110_(Race::RACE_Zerg))
		{
			return true;
		}
		LastControlID = 8;
		break;
	}
	return DLG_SwishOut(a1);
}

FAIL_STUB_PATCH(sub_4B5180);

bool sub_4B2810_(dialog* a1)
{
	switch (LastControlID)
	{
	case 6:
		if (!sub_4B27A0_(Race::RACE_Protoss))
		{
			return true;
		}
		LastControlID = 6;
		break;
	case 7:
		if (!sub_4B27A0_(Race::RACE_Terran))
		{
			return true;
		}
		LastControlID = 7;
		break;
	case 8:
		if (!sub_4B27A0_(Race::RACE_Zerg))
		{
			return true;
		}
		LastControlID = 8;
		break;
	case 30:
		if (!LoadPrecursorCampaign())
		{
			return true;
		}
		LastControlID = 30;
		break;
	}
	return DLG_SwishOut(a1);
}

FAIL_STUB_PATCH(sub_4B2810);

// TODO: reimplement DlgSwooshin -- all of its calls are moved away from StarCraft.exe

void DLG_SwishIn_(dialog* a1)
{
	ScreenLayers[0].bits |= 1;
	ScreenLayers[0].buffers = 1;
	BWFXN_RefreshTarget(ScreenLayers[0].left, ScreenLayers[0].height + ScreenLayers[0].top - 1, ScreenLayers[0].top, ScreenLayers[0].width + ScreenLayers[0].left - 1);
	if (dword_50E064 != stru_4FFAD0[glGluesMode].menu_position)
	{
		memcpy(stru_6CEB40, palette, sizeof(stru_6CEB40));
		if ((a1->lFlags & DialogFlags::CTRL_UPDATE) == 0)
		{
			a1->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog(a1);
		}
		TitlePaletteUpdate_(3);
		dword_50E064 = stru_4FFAD0[glGluesMode].menu_position;
	}
	SetCallbackTimer(46, a1, 100, DLGSwishInProc);
}

FAIL_STUB_PATCH(DLG_SwishIn);

bool __fastcall gluCmpgn_CampaignButton_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_LBUTTONDOWN:
	case EventNo::EVN_LBUTTONDBLCLK:
		if (dlg->lFlags & CTRL_DISABLED)
		{
			return 0;
		}
		break;
	case EventNo::EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			SetCallbackTimer(CTRL_DISABLED, dlg, 200, gluCmpgnBtn_UpdateTimer);
			break;
		case EventUser::USER_DESTROY:
			waitLoopCntd(CTRL_DISABLED, dlg);
			break;
		case EventUser::USER_MOUSEMOVE:
			return sub_4B24B0(dlg, evt);
		case EventUser::USER_INIT:
			genericLightupBtnInteract(dlg, evt);
			dlg->pfcnUpdate = gluCmpgnBtn_BtnLightupUpdate;
			SetCallbackTimer(72, dlg, 30, gluCmpgnBtn_InitTimer);
			return 1;
		}
		break;
	}

	return genericLightupBtnInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluCmpgn_CampaignButton);

void gluCmpgn_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		gluCmpgn_CampaignButton_,
		genericLightupBtnInteract,
		gluCmpgn_CampaignButton_,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
	};

	registerMenuFunctions_(functions, dlg, sizeof(functions));
	DlgSwooshin(2, gluCmpgnSwishController, dlg, 0);
}

FAIL_STUB_PATCH(gluCmpgn_CustomCtrlID);

bool __fastcall gluCmpgn_Main_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			DLG_SwishIn_(dlg);
			if (!byte_6D5BBC)
			{
				DLGMusicFade_(MT_TITLE);
			}
			break;
		case EventUser::USER_ACTIVATE:
			return sub_4B2810_(dlg);
		case EventUser::USER_INIT:
			gluCmpgn_CustomCtrlID_(dlg);
			break;
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluCmpgn_Main);

bool __fastcall gluExpCmpgn_CampaignButton_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_LBUTTONDOWN:
	case EventNo::EVN_LBUTTONDBLCLK:
		if (dlg->lFlags & CTRL_DISABLED)
		{
			return 0;
		}
		break;
	case EventNo::EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			SetCallbackTimer(2, dlg, 200, sub_4B4E70);
			break;
		case EventUser::USER_DESTROY:
			waitLoopCntd(2, dlg);
			break;
		case EventUser::USER_MOUSEMOVE:
			return sub_4B4E20(dlg, evt);
		case EventUser::USER_INIT:
			genericLightupBtnInteract(dlg, evt);
			dlg->pfcnUpdate = sub_4B4F10;
			SetCallbackTimer(72, dlg, 30, sub_4B4EE0);
			return 1;
		}
		break;
	}

	return genericLightupBtnInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluExpCmpgn_CampaignButton);

void gluExpCmpgn_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		genericLightupBtnInteract,
		gluExpCmpgn_CampaignButton_,
		gluExpCmpgn_CampaignButton_,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
	};

	registerMenuFunctions_(functions, dlg, sizeof(functions));
	DlgSwooshin(2, &commonSwishControllers[40], dlg, 0);
}

FAIL_STUB_PATCH(gluExpCmpgn_CustomCtrlID);

bool __fastcall gluExpCmpgn_Main_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			DLG_SwishIn_(dlg);
			if (!byte_6D5BBC)
				DLGMusicFade_(MT_TITLE);
			break;
		case EventUser::USER_ACTIVATE:
			return sub_4B5180_(dlg);
		case EventUser::USER_INIT:
			gluExpCmpgn_CustomCtrlID_(dlg);
			break;
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluExpCmpgn_Main);

dialog* loadFullMenuDLG_(const char* filename, dialog* buffer, int* read, const char* logfilename, int logline)
{
	return (dialog*) fastFileRead_(read, 0, filename, (int)buffer, 1, logfilename, logline);
}

FAIL_STUB_PATCH(loadFullMenuDLG);

dialog* loadAndInitFullMenuDLG_(const char* filename)
{
	dialog* dlg = loadFullMenuDLG_(filename, 0, 0, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
	if (dlg)
	{
		dlg->lFlags |= DialogFlags::CTRL_ACTIVE;
		AllocInitDialogData(dlg, dlg, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
	}
	return dlg;
}

FAIL_STUB_PATCH(loadAndInitFullMenuDLG);

void gluLogin_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		gluLogin_Listbox,
		genericLabelInteract,
	};

	registerMenuFunctions_(functions, dlg, sizeof(functions));
	DlgSwooshin(3, &commonSwishControllers[5], dlg, 0);
}

FAIL_STUB_PATCH(gluLogin_CustomCtrlID);

bool __fastcall gluLogin_Main_(dialog* dlg, struct dlgEvent* evt)
{
	dialog* v4;
	dlgEvent v6;

	switch (evt->wNo)
	{
	case EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			gluLogin_Init(dlg);
			DLG_SwishIn_(dlg);
			break;
		case USER_DESTROY:
			_ID_Destructor(&stru_51A218.dword8);
			break;
		case USER_ACTIVATE:
			switch (LastControlID)
			{
			case 4: // OK
				if (gluLogin_Activate(dword_5999C0))
				{
					return DLG_SwishOut(dlg);
				}
				break;
			case 6: // Create character
				gluLogin_CreateCharacter(dword_5999C0);
				break;
			case 7: // Delete character
				gluLogin_DeleteCharacter(dword_5999C0);
				return 1;
			default: // Cancel
				return DLG_SwishOut(dlg);
			}
			return 1;
		case USER_UNK_8:
			if (evt->wSelection != 87)
			{
				return genericDlgInteract(dlg, evt);
			}
			waitLoopCntd(87, dlg);
			gluLogin_CreateCharacter(dword_5999C0);
			return 1;
		case USER_INIT:
			gluLogin_CustomCtrlID_(dlg);
			break;
		}
		break;
	case EVN_WHEELUP:
		v4 = dword_5999C0->fields.list.pScrlBar;
		v6.dwUser = USER_SCROLLUP;
		v6.wNo = EVN_USER;
		v6.cursor.x = Mouse.x;
		v6.wSelection = 0;
		v6.wUnk_0x06 = 0;
		v6.cursor.y = Mouse.y;
		v4->pfcnInteract(v4, &v6);
		return 1;
	case EVN_WHEELDWN:
		v4 = dword_5999C0->fields.list.pScrlBar;
		v6.dwUser = USER_SCROLLDOWN;
		v6.wNo = EVN_USER;
		v6.cursor.x = Mouse.x;
		v6.wSelection = 0;
		v6.wUnk_0x06 = 0;
		v6.cursor.y = Mouse.y;
		v4->pfcnInteract(v4, &v6);
		return 1;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluLogin_Main);

void loadMenu_gluLogin_()
{
	gluLogin_Dlg = LoadDialog("rez\\gluLogin.bin");

	switch (gluLoadBINDlg_(gluLogin_Dlg, gluLogin_Main_))
	{
	case 4:
		if (!multiPlayerMode)
		{
			glGluesMode = IsExpansion != 0 ? GLUE_EX_CAMPAIGN : GLUE_CAMPAIGN;
		}
		else if (NetMode.as_number == 'MDMX' || NetMode.as_number == 'MODM')
		{
			glGluesMode = GLUE_MODEM;
		}
		else
		{
			glGluesMode = NetMode.as_number == 'SCBL' ? GLUE_DIRECT : GLUE_GAME_SELECT;
		}
		break;
	case 5:
		leaveOnQuit(0);
		glGluesMode = multiPlayerMode != 0 ? GLUE_CONNECT : GLUE_MAIN_MENU;
		break;
	default:
		glGluesMode = GLUE_MAIN_MENU;
	}

	changeMenu();
}

FAIL_STUB_PATCH(loadMenu_gluLogin);

void loadMenu_gluCmpgn_()
{
	OpheliaEnabled = GLUE_MAIN_MENU;
	multiPlayerMode = GLUE_MAIN_MENU;
	sub_4B26E0();
	dialog* campaign_dialog = loadAndInitFullMenuDLG_("rez\\gluCmpgn.bin");

	switch (gluLoadBINDlg_(campaign_dialog, gluCmpgn_Main_))
	{
	case 5:
		glGluesMode = GLUE_LOAD;
		break;
	case 6:
		glGluesMode = GLUE_READY_P;
		break;
	case 30:
	case 7:
		glGluesMode = GLUE_READY_T;
		break;
	case 8:
		glGluesMode = GLUE_READY_Z;
		break;
	case 9:
		glGluesMode = GLUE_LOGIN;
		break;
	case 10:
		glGluesMode = GLUE_CREATE;
		break;
	case 11:
		glGluesMode = GLUE_CREATE_MULTI;
		break;
	default:
		glGluesMode = GLUE_MAIN_MENU;
		break;
	}
	changeMenu();
}

FAIL_STUB_PATCH(loadMenu_gluCmpgn);

void loadMenu_gluExpCmpgn_()
{
	OpheliaEnabled = 0;
	multiPlayerMode = 0;
	sub_4B5050();
	dialog* campaign_dialog = loadAndInitFullMenuDLG_("rez\\gluExpCmpgn.bin");

	switch (gluLoadBINDlg_(campaign_dialog, gluExpCmpgn_Main_))
	{
	case 8:
		glGluesMode = GLUE_READY_Z;
		break;
	case 7:
		glGluesMode = GLUE_READY_T;
		break;
	case 6:
		glGluesMode = GLUE_READY_P;
		break;
	case 5:
		glGluesMode = GLUE_LOAD;
		break;
	case 10:
		glGluesMode = GLUE_CREATE;
		break;
	case 11:
		glGluesMode = GLUE_CREATE_MULTI;
		break;
	case 9:
		glGluesMode = GLUE_LOGIN;
		break;
	default:
		glGluesMode = GLUE_MAIN_MENU;
		break;
	}
	changeMenu();
}

FAIL_STUB_PATCH(loadMenu_gluExpCmpgn);

void gluJoin_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		gluJoin_Listbox,
		genericLabelInteract,
		gluJoin_MapStatsLabel,
		gluJoin_MapStatsLabel,
		gluJoin_MapStatsLabel,
		gluJoin_MapStatsLabel,
		gluJoin_MapStatsLabel,
		gluJoin_MapStatsLabel,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
	};

	DlgSwooshin(4, &commonSwishControllers[18], dlg, 0);
	registerMenuFunctions_(functions, dlg, sizeof(functions));
}

FAIL_STUB_PATCH(gluJoin_CustomCtrlID);

bool __fastcall gluJoin_Main_(dialog* dlg, struct dlgEvent* evt)
{
	dialog* v5 = getControlFromIndex(dlg, 13);

	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			sub_4B7E10(dlg);
			DLG_SwishIn_(dlg);
			break;
		case USER_DESTROY:
			DestroyGameNodes();
			break;
		case USER_ACTIVATE:
			if (LastControlID == 13)
			{
				DisableControl(v5);
				if (JoinNetworkGame(&stru_5999F0))
				{
					EnableControl(v5);
					return DLG_SwishOut(dlg);
				}
			}
			else if (LastControlID != 15 || !is_spawn)
			{
				return DLG_SwishOut(dlg);
			}
			else
			{
				const char* str = GetNetworkTblString(104);
				BWFXN_gluPOK_MBox(str);
			}
			return 1;
		case USER_INIT:
			gluJoin_CustomCtrlID_(dlg);
			break;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluJoin_Main);

void loadMenu_gluJoin_()
{
	InReplay = 0;
	freeChkFileMem_();

	gluJoin_Dlg = LoadDialog("rez\\gluJoin.bin");

	switch (gluLoadBINDlg_(gluJoin_Dlg, gluJoin_Main_))
	{
	case 13:
		glGluesMode = GLUE_CHAT;
		break;
	case 14:
		glGluesMode = NetMode.as_number != 'SCBL' ? GLUE_LOGIN : GLUE_CONNECT;
		break;
	case 15:
		glGluesMode = GLUE_CREATE;
		break;
	default:
		glGluesMode = GLUE_MAIN_MENU;
		break;
	}

	changeMenu();
}

FAIL_STUB_PATCH(loadMenu_gluJoin);

void gluCustm_initSwish_(dialog* a1)
{
	DlgSwooshin(5, gluCustmSwishController, a1, 0);
	getControlFromIndex(a1, 6)->pfcnUpdate = gluCustm_UpdateCB;
}

FAIL_STUB_PATCH(gluCustm_initSwish);

TypeDropdownSelect singleTypeSelect_[]
{
	{PlayerType::PT_NotUsed, (GluAllTblEntry) 0x86 },
	{PlayerType::PT_Computer, (GluAllTblEntry) 0x84 },
};

void gluCustm_typeDropdown_(dialog* dlg)
{
	unsigned __int8 v2 = 0;
	dlg->lFlags |= CTRL_LBOX_NORECALC;
	for (const auto& player_type: singleTypeSelect_)
	{
		char* v4 = get_GluAll_String_(player_type.tbl_entry);
		u8 v5 = ListBox_AddEntry(v4, dlg, 0);
		if (v5 == 0xFF)
		{
			break;
		}
		dlg->fields.list.pdwData[v5] = player_type.player_type;
		v2 = player_type.player_type == PlayerType::PT_Computer ? v5 : 0;
	}

	if (dlg->lFlags & CTRL_LBOX_NORECALC)
	{
		dlg->lFlags &= ~CTRL_LBOX_NORECALC;
		List_Update(dlg);
	}
	if (v2 < dlg->fields.list.bStrs || v2 == 0xFF)
	{
		dlgEvent v7;
		v7.wNo = EVN_USER;
		v7.dwUser = USER_SELECT;
		v7.wSelection = v2;
		v7.wUnk_0x06 = 0;
		dlg->pfcnInteract(dlg, &v7);
		DlgSetSelected_UpdateScrollbar(v2, dlg);
	}
}

FAIL_STUB_PATCH(gluCustm_typeDropdown);

bool __fastcall gluCustm_PlayerSlot_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gluCustm_typeDropdown_(dlg);
			break;
		case EventUser::USER_INIT:
			dlg->lFlags |= CTRL_PLAIN;
			break;
		case EventUser::USER_SELECT:
			genericListboxInteract(dlg, evt);
			sub_4ADD90(1, dlg);
			return 1;
		}
	}
	return genericComboboxInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluCustm_PlayerSlot);

const char* __stdcall getRaceString(Race race)
{
	static TblFile tbl_file("rez\\races.tbl");

	return tbl_file[race];
}

Race SELECTABLE_RACES[] = {
	Race::RACE_Zerg,
	Race::RACE_Terran,
	Race::RACE_Protoss,
	Race::RACE_Random,
};

void gluCustm_raceDropdown_(dialog* a1)
{
	a1->lFlags |= CTRL_LBOX_NORECALC;

	for (int i = 0; i < _countof(SELECTABLE_RACES); i++)
	{
		const char* race_name = getRaceString(SELECTABLE_RACES[i]);
		u8 v5 = ListBox_AddEntry(race_name, a1, 0);
		if (v5 == 0xFF)
		{
			break;
		}
		a1->fields.list.pdwData[v5] = SELECTABLE_RACES[i];
	}

	if (a1->lFlags & CTRL_LBOX_NORECALC)
	{
		a1->lFlags &= ~CTRL_LBOX_NORECALC;
		List_Update(a1);
	}

	if (a1->fields.scroll.bSliderSkip > 3u)
	{
		dlgEvent v8;
		v8.wNo = EVN_USER;
		v8.dwUser = USER_SELECT;
		*(_DWORD*)&v8.wSelection = 3;
		a1->pfcnInteract(a1, &v8);
		DlgSetSelected_UpdateScrollbar(3u, a1);
	}
}

FAIL_STUB_PATCH(gluCustm_raceDropdown);

bool __fastcall gluCustm_RaceSlot_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gluCustm_raceDropdown_(dlg);
			break;
		case EventUser::USER_INIT:
			dlg->lFlags |= DialogFlags::CTRL_PLAIN;
			break;
		}
	}

	return genericComboboxInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluCustm_RaceSlot);

BYTE gluCustm_CustomCtrl_InitializeChildren_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		gluCustm_GameListboxUpdate,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		Menu_Generic_Button,
		Menu_Generic_Button,
		genericLabelInteract,
		genericLabelInteract,
		gluCreate_GameSpeed_Slider,
		gluCustm_GameType,
		gluCustm_SubType,
		NULL,
		gluCustm_Player1,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
	};

	registerMenuFunctions_(functions, dlg, sizeof(functions));

	custom_game_mode = getControlFromIndex(dlg, 17);
	custom_game_submode = getControlFromIndex(dlg, 18);
	custom_game_slots = getControlFromIndex(dlg, 19);

	if (multiPlayerMode)
	{
		return 0;
	}

	byte_59BB6C = registry_options.GameSpeed;
	return registry_options.GameSpeed;
}

FAIL_STUB_PATCH(gluCustm_CustomCtrl_InitializeChildren);

bool __fastcall gluCustm_Interact_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gluCustm_initSwish_(dlg);
			DLG_SwishIn_(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			if (LastControlID == 12 && !gluCustmLoadMapFromList())
			{
				return 1;
			}
			waitLoopCntd(5, gluCreateOrCustm_bin);
			return DLG_SwishOut(dlg);
		case EventUser::USER_INIT:
			gluCustm_CustomCtrl_InitializeChildren_(dlg);
			break;
		case 1029:
			InitGlueMapListBox();
			break;
		}
	}
	else if (evt->wNo == EventNo::EVN_WHEELUP)
	{
		dlgEvent event;
		event.dwUser = USER_SCROLLUP;
		event.wSelection = 0;
		event.wUnk_0x06 = 0;
		event.wNo = EVN_USER;
		event.cursor.x = Mouse.x;
		event.cursor.y = Mouse.y;

		dialog* v4 = map_listbox->fields.list.pScrlBar;
		v4->pfcnInteract(v4, &event);
		return 1;
	}
	else if (evt->wNo == EventNo::EVN_WHEELDWN)
	{
		dlgEvent event;
		event.dwUser = USER_SCROLLDOWN;
		event.wSelection = 0;
		event.wUnk_0x06 = 0;
		event.wNo = EVN_USER;
		event.cursor.x = Mouse.x;
		event.cursor.y = Mouse.y;

		dialog* v4 = map_listbox->fields.list.pScrlBar;
		v4->pfcnInteract(v4, &event);
		return 1;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluCustm_Interact);

void loadMenu_gluCustm_(int is_multiplayer)
{
	dword_59B844 = is_multiplayer;
	char v1 = 0;
	const char* v2 = LOBYTE(multiPlayerMode) ? "rez\\gluCreat.bin" : "rez\\gluCustm.bin";

	gluCreateOrCustm_bin = loadAndInitFullMenuDLG_(v2);
	dword_59BA60 = (void *)LoadGraphic("glue\\create\\iCreate.grp", 0, "Starcraft\\SWAR\\lang\\gluCreat.cpp", 1427);
	dword_6D5A74 = GAME_RUNINIT;
	switch(gluLoadBINDlg_(gluCreateOrCustm_bin, gluCustm_Interact_))
	{
	case 12:
		if (LOBYTE(multiPlayerMode) != v1)
		{
			glGluesMode = GLUE_CHAT;
		}
		else if (gameData.got_file_values.victory_conditions != v1
			|| gameData.got_file_values.starting_units != v1
			|| gameData.got_file_values.tournament_mode != v1
			|| InReplay)
		{
			gwGameMode = GAME_RUNINIT;
		}
		else if (Players[g_LocalNationID].nRace == Race::RACE_Zerg)
		{
			glGluesMode = GLUE_READY_Z;
		}
		else if (Players[g_LocalNationID].nRace == Race::RACE_Terran)
		{
			glGluesMode = GLUE_READY_T;
		}
		else if (Players[g_LocalNationID].nRace == Race::RACE_Protoss)
		{
			glGluesMode = GLUE_READY_P;
		}
		break;
	case -3:
	case 13:
		InReplay = 0;
		freeChkFileMem_();

		if (!LOBYTE(multiPlayerMode))
		{
			glGluesMode = IsExpansion != 0 ? GLUE_EX_CAMPAIGN : GLUE_CAMPAIGN;
		}
		else if (NetMode.as_number == 'SCBL')
		{
			glGluesMode = MenuPosition::GLUE_DIRECT;
		}
		else if (NetMode.as_number == 'MODM')
		{
			glGluesMode = MenuPosition::GLUE_MODEM;
		}
		else if (NetMode.as_number == 'BNET')
		{
			glGluesMode = MenuPosition::GLUE_BATTLE;
		}
		else if (NetMode.as_number == 'MDMX')
		{
			glGluesMode = MenuPosition::GLUE_MODEM;
		}
		else
		{
			glGluesMode = MenuPosition::GLUE_GAME_SELECT;
		}
		break;
	default:
		glGluesMode = MenuPosition::GLUE_MAIN_MENU;
	}

	changeMenu();
	if (dword_59BA60)
		SMemFree(dword_59BA60, "Starcraft\\SWAR\\lang\\gluCreat.cpp", 1484, v1);
	dword_6D5A74 = 0;
}

FAIL_STUB_PATCH(loadMenu_gluCustm);

int load_gluGameMode_BINDLG_()
{
	if (dword_6D5A3C)
	{
		LastControlID = dword_6D5A54;
		if (!dword_6D5A54)
		{
			LastControlID = 3;
		}
		DestroyDialog(dword_6D5A3C);
	}
	dword_6D5A40 = off_51A6F0;
	dword_599D98 = 28;
	SStrCopy(byte_599C98, get_GluAll_String((GluAllTblEntry)0xAA), 0xFFu);
	byte_599B98 = 0;

	char fileName[260];
	const char* v14 = &aGluePalmm[1304 * stru_4FFAD0[glGluesMode].menu_position];
	SStrCopy(fileName, v14, 0x104u);
	strcat_s(fileName, "\\retail_ex.pcx");

	int width;
	int height;
	int read;
	if (!SBmpAllocLoadImage(fileName, (int*)palette, (void**)&read, &width, &height, 0, 0, allocFunction))
	{
		SysWarn_FileNotFound(fileName, SErrGetLastError());
	}
	p_hist_pcx.wid = width;
	p_hist_pcx.ht = height;
	p_hist_pcx.data = (u8*)read;

	dword_6D5A3C = LoadDialog("rez\\gluGameMode.bin");
	return gluLoadBINDlg(dword_6D5A3C, Popup_Main_);
}

FAIL_STUB_PATCH(load_gluGameMode_BINDLG);

int SelGameMode_(int a2)
{
	int v4 = LastControlID;
	switch (load_gluGameMode_BINDLG_())
	{
	case 6:
		if (a2 || cmpgn_WaitForCDRom((GluAllTblEntry)167, "rez\\glucmpgn.bin"))
		{
			IsExpansion = 0;
			LastControlID = v4;
			return 1;
		}
		else
		{
			LastControlID = v4;
			return 0;
		}
	case 7:
		if (cmpgn_WaitForCDRom((GluAllTblEntry)168, "rez\\gluexpcmpgn.bin"))
		{
			IsExpansion = 1;
			LastControlID = v4;
			return 1;
		}
		else
		{
			LastControlID = v4;
			return 0;
		}
	default:
		LastControlID = v4;
		return 0;
	}
}

FAIL_STUB_PATCH(SelGameMode);

signed int loadStareditProcess_(dialog* a1)
{
	CHAR CommandLine[260];
	CHAR Filename[260];

	if (!GetModuleFileNameA(hInst, Filename, 0x104u))
	{
		Filename[0] = 0;
	}
	char* v2 = strrchr(Filename, '\\');
	if (v2)
	{
		v2[1] = 0;
	}
	SStrCopy(CommandLine, Filename, 0x104u);
	SStrNCat(CommandLine, "StarEdit.exe", 260);

	struct _PROCESS_INFORMATION ProcessInformation;
	struct _STARTUPINFOA StartupInfo;
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = 68;

	if (CreateProcessA(0, CommandLine, 0, 0, 0, 0x20u, 0, Filename, &StartupInfo, &ProcessInformation))
	{
		HWND v3 = GetDesktopWindow();
		SetForegroundWindow(v3);
		dialog* v4 = a1;
		if (a1->wCtrlType)
		{
			v4 = a1->fields.ctrl.pDlg;
		}
		dialog* v5 = v4->fields.dlg.pFirstChild;
		if (v5)
		{
			while (v5->wIndex != 5)
			{
				v5 = v5->pNext;
				if (v5 == NULL)
				{
					break;
				}
			}
		}

		DisableControl(v5);
		WaitForInputIdle(ProcessInformation.hProcess, -1u);
		EnableControl(v5);
		CloseHandle(ProcessInformation.hThread);
		CloseHandle(ProcessInformation.hProcess);
		return 1;
	}
	else
	{
		const char* v8 = get_GluAll_String_(SPAWNED_BY);
		BWFXN_gluPOK_MBox(v8);
		return 0;
	}
}

FAIL_STUB_PATCH(loadStareditProcess);

void gluMain_CustomCtrlID_(dialog* a1)
{
	static FnInteract functions[] = {
		NULL,
		genericLightupBtnInteract,
		genericLightupBtnInteract,
		genericLightupBtnInteract,
		genericLightupBtnInteract,
		NULL,
		NULL,
		Menu_Generic_Button,
		Menu_Generic_Button,
		genericLabelInteract,
		genericLightupBtnInteract,
	};

	registerMenuFunctions_(functions, a1, sizeof(functions));
}

FAIL_STUB_PATCH(gluMain_CustomCtrlID);

bool __fastcall gluMain_Dlg_Interact_(dialog* dlg, struct dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			gluMainCreate(dlg);
			DLG_SwishIn_(dlg);
			genericDlgInteract(dlg, evt);
			if (byte_6D5BBC)
			{
				return true;
			}
			DLGMusicFade_(MT_TITLE);
			return true;
		case USER_DESTROY:
			gluMainDestroy(dlg);
			break;
		case USER_ACTIVATE:
			switch (LastControlID)
			{
			case 3:
				if (is_spawn)
				{
					const char* s = GetNetworkTblString(103);
					BWFXN_gluPOK_MBox(s);
					return true;
				}
				if (is_expansion_installed)
				{
					if (SelGameMode_(0))
					{
						return DLG_SwishOut(dlg);
					}
					return true;
				}
				break;
			case 4:
				if (is_expansion_installed)
				{
					if (SelGameMode_(1))
					{
						return DLG_SwishOut(dlg);
					}
					return true;
				}
				if (is_spawn)
				{
					IsExpansion = 0;
					return DLG_SwishOut(dlg);
				}
				break;
			case 5:
				loadStareditProcess_(dlg);
				return true;
			default:
				return DLG_SwishOut(dlg);
			}
			if (!gluMain_DisplayCDRomErrorBinDlg())
			{
				return true;
			}
			IsExpansion = 0;
			return DLG_SwishOut(dlg);
		case USER_INIT:
			gluMain_CustomCtrlID_(dlg);
			break;
		}
		break;
	case EventNo::EVN_CHAR:
		if (evt->wVirtKey == VK_SPACE)
		{
			return true;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluMain_Dlg_Interact);

void loadMenu_gluMain_()
{
	multiPlayerMode = 0;

	dialog* bin_dialog = (dialog*) fastFileRead_(NULL, 0, "rez\\gluMain.bin", 0, 1, "Starcraft\\SWAR\\lang\\gluMain.cpp", 573);

	if (bin_dialog)
	{
		bin_dialog->lFlags |= DialogFlags::CTRL_ACTIVE;
		AllocInitDialogData(bin_dialog, bin_dialog, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\gluMain.cpp", 624);
	}

	if (bin_dialog)
	{
		GotFileValues* ums_game_template;

		switch (gluLoadBINDlg_(bin_dialog, gluMain_Dlg_Interact_))
		{
		case 2:
			gwGameMode = GAME_EXIT;
			break;
		case 3:
			multiPlayerMode = 0;
			glGluesMode = GLUE_LOGIN;
			ums_game_template = InitUseMapSettingsTemplate_();
			memcpy(&gameData.got_file_values, ums_game_template, sizeof(gameData.got_file_values));
			SMemFree(ums_game_template, "Starcraft\\SWAR\\lang\\gluMain.cpp", 646, 0);
			break;
		case 4:
			multiPlayerMode = 1;
			glGluesMode = MenuPosition::GLUE_CONNECT;
			ums_game_template = InitUseMapSettingsTemplate_();
			memcpy(&gameData.got_file_values, ums_game_template, sizeof(gameData.got_file_values));
			SMemFree(ums_game_template, "Starcraft\\SWAR\\lang\\gluMain.cpp", 635, 0);
			break;
		case 5:
			break;
		case 8:
			gwGameMode = GAME_INTRO;
			break;
		case 9:
			gwGameMode = GAME_CREDITS;
			break;
		case 65520:
			break;
		default:
			glGluesMode = GLUE_MAIN_MENU;
			break;
		}
		changeMenu();
		return;
	}
}

FAIL_STUB_PATCH(loadMenu_gluMain);

bool __fastcall gluRdyZ_Secret_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_INIT:
			ButtonVideo(dlg);
			return genericCommonInteract(evt, dlg);
		case EventUser::USER_DESTROY:
			genericLightupBtnUserDestroyEventHandler(dlg);
			return genericCommonInteract(evt, dlg);
		}
	}

	return 0;
}

FAIL_STUB_PATCH(gluRdyZ_Secret);

void gluRdy_CustomCtrlID_(dialog* dlg)
{
	static swishTimer timers[] =
	{
		{5, 3},
		{6, 0},
		{7, 3},
		{9, 0},
		{10, 0},
		{11, 2},
		{12, 2},
	};

	static FnInteract functions[] = {
		gluRdyZ_Secret_,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		genericLightupBtnInteract,
		Menu_Generic_Button,
		gluRdy_Portrait,
		gluRdy_Portrait,
		gluRdy_Portrait,
		gluRdy_Portrait,
		Menu_Generic_Button,
		Menu_Generic_Button,
	};

	DlgSwooshin(_countof(timers), timers, dlg, 80);
	registerMenuFunctions_(functions, dlg, sizeof(functions));
}

FAIL_STUB_PATCH(gluRdyP_CustomCtrlID);
FAIL_STUB_PATCH(gluRdyT_CustomCtrlID);
FAIL_STUB_PATCH(gluRdyZ_CustomCtrlID);

void rdyPFrame_()
{
	const char* v1[] =
	{
		"glue\\ReadyP\\PFrameH1.pcx",
		"glue\\ReadyP\\PFrameH2.pcx",
		"glue\\ReadyP\\PFrameH3.pcx",
		"glue\\ReadyP\\PFrameH4.pcx",
	};
	const char* v2[] =
	{
		"glue\\ReadyP\\PFrame1.pcx",
		"glue\\ReadyP\\PFrame2.pcx",
		"glue\\ReadyP\\PFrame3.pcx",
		"glue\\ReadyP\\PFrame4.pcx",
	};
	LoadBriefingFrameImages(v1, v2);
}

FAIL_STUB_PATCH(rdyPFrame);

void RdyTFrame_()
{
	const char* v1[] =
	{
		"glue\\ReadyT\\TFrameH1.pcx",
		"glue\\ReadyT\\TFrameH2.pcx",
		"glue\\ReadyT\\TFrameH3.pcx",
		"glue\\ReadyT\\TFrameH4.pcx",
	};
	const char* v2[] =
	{
		"glue\\ReadyT\\TFrame1.pcx",
		"glue\\ReadyT\\TFrame2.pcx",
		"glue\\ReadyT\\TFrame3.pcx",
		"glue\\ReadyT\\TFrame4.pcx",
	};
	LoadBriefingFrameImages(v1, v2);
}

FAIL_STUB_PATCH(RdyTFrame);

void RdyZFrame_()
{
	const char* v1[] =
	{
		"glue\\ReadyZ\\ZFrameH1.pcx",
		"glue\\ReadyZ\\ZFrameH2.pcx",
		"glue\\ReadyZ\\ZFrameH3.pcx",
		"glue\\ReadyZ\\ZFrameH4.pcx",
	};
	const char* v2[] =
	{
		"glue\\ReadyZ\\ZFrame1.pcx",
		"glue\\ReadyZ\\ZFrame2.pcx",
		"glue\\ReadyZ\\ZFrame3.pcx",
		"glue\\ReadyZ\\ZFrame4.pcx",
	};
	LoadBriefingFrameImages(v1, v2);
}

FAIL_STUB_PATCH(RdyZFrame);

bool __fastcall gluRdy_BINDLG_Loop(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			DLG_SwishIn_(dlg);
			break;
		case EventUser::USER_DESTROY:
			briefingFramesCleanup(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			return sub_46D160(dlg);
		case EventUser::USER_INIT:
			sub_46D3C0(dlg);
			gluRdy_CustomCtrlID_(dlg);
			break;
		case 0x405:
			sub_46D220(dlg);
			switch (glGluesMode)
			{
			case MenuPosition::GLUE_READY_P:
				rdyPFrame_();
				break;
			case MenuPosition::GLUE_READY_T:
				RdyTFrame_();
				break;
			case MenuPosition::GLUE_READY_Z:
				RdyZFrame_();
				break;
			}
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluRdyT_BINDLG_Loop);
FAIL_STUB_PATCH(gluRdyZ_BINDLG_Loop);
FAIL_STUB_PATCH(gluRdyP_BINDLG_Loop);

void DisplayEstablishingShot_();
int ContinueCampaign_(int a1);

void sub_46D200_(MusicTrack music_track)
{
	stopMusic();
	DLGMusicFade_(music_track);
}

FAIL_STUB_PATCH(sub_46D200);

void sub_46D1F0_()
{
	stopMusic();
	DLGMusicFade_(MT_TITLE);
}

FAIL_STUB_PATCH(sub_46D1F0);

void loadMenu_gluRdy(MusicTrack music_track, const char* bin_path, bool __fastcall BINDLG_Loop(dialog* dlg, struct dlgEvent* evt))
{
	sub_46D200_(music_track);
	DisplayEstablishingShot_();
	if (gwGameMode == GAME_GLUES)
	{
		dword_50E064 = -1;
		dialog* bin_dialog = LoadDialog(bin_path);

		switch (gluLoadBINDlg_(bin_dialog, BINDLG_Loop))
		{
		case 14:
			if (multiPlayerMode)
			{
				BWFXN_NetSelectReturnMenu();
			}
			else
			{
				glGluesMode = IsExpansion != 0 ? GLUE_EX_CAMPAIGN : GLUE_CAMPAIGN;
			}
			sub_46D1F0_();
			break;
		case 19:
			ContinueCampaign_(1);
			break;
		case 100:
			sub_46D1F0_();
			break;
		case 101:
			gwGameMode = GAME_RUNINIT;
			break;
		default:
			glGluesMode = GLUE_MAIN_MENU;
			sub_46D1F0_();
			break;
		}
		changeMenu();
	}
}

FAIL_STUB_PATCH(loadMenu_gluRdyT);
FAIL_STUB_PATCH(loadMenu_gluRdyZ);
FAIL_STUB_PATCH(loadMenu_gluRdyP);

void selConn_connectionList_Create_(dialog* a1)
{
	dialog* v1 = gluConn_Dlg;
	if (gluConn_Dlg->wCtrlType)
	{
		v1 = gluConn_Dlg->fields.ctrl.pDlg;
	}
	dialog* v2 = v1->fields.dlg.pFirstChild;
	while (v2 && v2->wIndex != 9)
	{
		v2 = v2->pNext;
	}
	v2->lFlags |= CTRL_DISABLED;
	InitNetProviders(a1);
	if ((v2->lFlags & CTRL_UPDATE) == 0)
	{
		v2->lFlags |= CTRL_UPDATE;
		updateDialog(v2);
	}
}

FAIL_STUB_PATCH(selConn_connectionList_Create);

bool __fastcall selConn_ConnectionList_Interact_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			selConn_connectionList_Create_(dlg);
			break;
		case EventUser::USER_INIT:
			dlg->lFlags |= DialogFlags::CTRL_PLAIN;
			return genericListboxInteract(dlg, evt);
		case EventUser::USER_SELECT:
			genericListboxInteract(dlg, evt);
			selConn_connectionList_setSelection(dlg);
			return 1;
		}
	}
	return genericListboxInteract(dlg, evt);
}

FAIL_STUB_PATCH(selConn_ConnectionList_Interact);

bool __fastcall GatewayListProc_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			dlg->fields.list.pDrawItemFcn = sub_4BA3C0;
			ListBNGateways(dlg); // TODO: Move the gateways to the YML configuration
			break;
		case USER_INIT:
			dlg->lFlags |= CTRL_PLAIN | CTRL_FONT_SMALL;
			break;
		case USER_DESTROY:
			sub_4BA240(dlg->fields.list.bStrs ? dlg->fields.list.bCurrStr : -1);
			[[fallthrough]];
		case USER_SELECT:
			genericListboxInteract(dlg, evt);
			return 1;
		}
	}

	return genericListboxInteract(dlg, evt);
}

FAIL_STUB_PATCH(GatewayListProc);

void ConnSel_InitChildren_(dialog* a1)
{
	static FnInteract v2[14] = {
		0,
		0,
		0,
		0,
		selConn_ConnectionList_Interact_,
		0,
		0,
		0,
		Menu_Generic_Button,
		Menu_Generic_Button,
		0,
		GatewayListProc_,
		0,
		0,
	};

	DlgSwooshin(5, commonSwishControllers, a1, 0);
	registerMenuFunctions_(v2, a1, sizeof(v2));
}

FAIL_STUB_PATCH(ConnSel_InitChildren);

int getGameList_(dialog* dlg)
{
	NetMode.as_number = -1;
	SNetDestroy();
	if (hEvent && IsBattleNet)
	{
		ResetEvent(hEvent);
		IsBattleNet = 0;
	}
	LOWORD(dword_66FF30) = 0;

	getControlFromIndex(dlg, 6)->pszText = (char*)SMemAlloc(128, "Starcraft\\SWAR\\lang\\gluConn.cpp", 511, 0);
	getControlFromIndex(dlg, 7)->pszText = (char*)SMemAlloc(128, "Starcraft\\SWAR\\lang\\gluConn.cpp", 511, 0);
	getControlFromIndex(dlg, 13)->pszText = (char*)SMemAlloc(255, "Starcraft\\SWAR\\lang\\gluConn.cpp", 511, 0);

	const char* v11 = get_GluAll_String_((GluAllTblEntry) 0xB9);
	dialog* v14 = getControlFromIndex(gluConn_Dlg, 13);
	SStrCopy(v14->pszText, v11, 0xFFu);
	if ((v14->lFlags & CTRL_UPDATE) == 0)
	{
		v14->lFlags |= CTRL_UPDATE;
		updateDialog(v14);
	}
	SNetEnumProviders(0, Provider_Constructor);
	if (!byte_6D5BBC)
	{
		DLGMusicFade_(MT_TITLE);
	}
	Template_Destructor(stru_51A218.char14 + 60);
	return LoadGameTemplates_(Template_Constructor);
}

FAIL_STUB_PATCH(getGameList);

bool __fastcall ConnSel_Interact_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			getGameList_(dlg);
			DLG_SwishIn_(dlg);
			break;
		case USER_DESTROY:
			DestroyProviderList(dlg);
			break;
		case USER_ACTIVATE:
			if (LastControlID == 9)
			{
				if (!BeginBNET())
				{
					return 1;
				}
				LastControlID = 9;
			}
			return DLG_SwishOut(dlg);
		case USER_INIT:
			ConnSel_InitChildren_(dlg);
			break;
		case 0x405:
			showDialog(getControlFromIndex(gluConn_Dlg, 12));
			showDialog(getControlFromIndex(gluConn_Dlg, 13));
			showDialog(getControlFromIndex(gluConn_Dlg, 14));
			break;
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(ConnSel_Interact);

int Begin_BNET_(Char4 network_provider_id)
{
	if (network_provider_id.as_number != 'BNET')
	{
		return InitializeNetworkProvider(network_provider_id);
	}
	sub_4DCEE0();
	dword_50E064 = -1;
	sub_4AD140();
	if (InitializeNetworkProvider(network_provider_id))
	{
		return 1;
	}
	sub_4ACF20();
	return 0;
}

FAIL_STUB_PATCH(Begin_BNET);

void loadMenu_gluConn_()
{
	gluConn_Dlg = LoadDialog("rez\\gluConn.bin");

	if (gluLoadBINDlg_(gluConn_Dlg, ConnSel_Interact_) != 9)
	{
		glGluesMode = GLUE_MAIN_MENU;
	}
	else if (network_provider_id.as_number == 'BNET')
	{
		stopMusic();
		glGluesMode = Begin_BNET_(network_provider_id) ? glGluesRelated_maybe : GLUE_CONNECT;
	}
	else
	{
		glGluesMode = glGluesRelated_maybe;
	}

	changeMenu();
}

FAIL_STUB_PATCH(loadMenu_gluConn);

void gluModem_CustomCtrlID_(dialog* a1)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		genericLabelInteract,
		genericListboxInteract,
		genericLabelInteract,
		genericLabelInteract,
	};

	registerMenuFunctions_(functions, a1, sizeof(functions));
	DlgSwooshin(4, stru_51A99C, a1, 0);
}

FAIL_STUB_PATCH(gluModem_CustomCtrlID);

bool __fastcall gluModem_Main_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			sub_4B0AE0(dlg);
			DLG_SwishIn_(dlg);
			break;
		case USER_DESTROY:
			Sleep(400);
			waitLoopCntd(56, dlg);
			break;
			return genericDlgInteract(dlg, evt);
		case USER_ACTIVATE:
			switch (LastControlID)
			{
			case 5:
				if (!load_gluModemStatus(dlg))
				{
					return 1;
				}
				LastControlID = 5;
				return DLG_SwishOut(dlg);
			case 6:
				if (dword_59B82C && !BWFXN_gluPOKCancel_MBox(get_GluAll_String((GluAllTblEntry)82)))
				{
					return 1;
				}
				LastControlID = 6;
				return DLG_SwishOut(dlg);
			case 7:
				AU_PHONENUMBER(dlg);
				return 1;
			case 8:
				DeleteGluModemEntry(dlg);
				return 1;
			case 9:
				if (is_spawn)
				{
					BWFXN_gluPOK_MBox(GetNetworkTblString(104));
					return 1;
				}
				return DLG_SwishOut(dlg);
			default:
				return DLG_SwishOut(dlg);
			}
			break;
		case USER_INIT:
			gluModem_CustomCtrlID_(dlg);
			return genericDlgInteract(dlg, evt);
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluModem_Main);

void loadMenu_gluModem_()
{
	dialog* gluModem_bin = LoadDialog("rez\\gluModem.bin");
	switch (gluLoadBINDlg_(gluModem_bin, gluModem_Main_))
	{
	case 5:
		glGluesMode = GLUE_CHAT;
		break;
	case 6:
		glGluesMode = GLUE_CONNECT;
		break;
	case 9:
		glGluesMode = GLUE_CREATE;
		break;
	default:
		glGluesMode = GLUE_MAIN_MENU;
		break;
	}

	changeMenu();
}

FAIL_STUB_PATCH(loadMenu_gluModem);

void CreateRaceDropdown_(dialog* dlg, Race race)
{
	HIBYTE(dlg->wUser) |= 1u;
	if (!dword_68F520)
	{
		showDialog(dlg);
	}
	EnableControl(dlg);
	if ((dlg->lFlags & CTRL_UPDATE) == 0)
	{
		dlg->lFlags |= CTRL_UPDATE;
		updateDialog(dlg);
	}

	dlgEvent v11;
	v11.cursor.y = Mouse.y;
	v11.wNo = EVN_USER;
	v11.dwUser = USER_NEXT;
	*(_DWORD*)&v11.wSelection = 0;
	v11.cursor.x = Mouse.x;
	dlg->pfcnInteract(dlg, &v11);
	dlg->lFlags |= CTRL_LBOX_NORECALC;
	dlg->pfcnInteract = genericComboboxInteract;
	ClearListBox(dlg);

	u8 v5 = 0;
	u8 v12 = 0;

	for (int i = 0; i < _countof(SELECTABLE_RACES); i++)
	{
		const char* race_name = getRaceString(SELECTABLE_RACES[i]);
		u8 v9 = ListBox_AddEntry(race_name, dlg, 0);
		if (v9 == 0xFF)
		{
			break;
		}
		dlg->fields.list.pdwData[v9] = SELECTABLE_RACES[i];
		if (SELECTABLE_RACES[i] == race)
		{
			v12 = v9;
		}
		v5 = v12;
	}

	if (dlg->lFlags & CTRL_LBOX_NORECALC)
	{
		dlg->lFlags &= ~CTRL_LBOX_NORECALC;
		List_Update(dlg);
	}
	if (v5 < dlg->fields.scroll.bSliderSkip || v5 == 0xFF)
	{
		*(_DWORD*)&v11.wSelection = v5;
		v11.wNo = EVN_USER;
		v11.dwUser = USER_SELECT;
		dlg->pfcnInteract(dlg, &v11);
		DlgSetSelected_UpdateScrollbar(v5, dlg);
	}
	dlg->pfcnInteract = sub_450A60;
	strcpy(byte_596BC8, a1);
}

void __stdcall CreateRaceDropdown__(Race race)
{
	dialog* dlg;

	__asm mov dlg, eax

	CreateRaceDropdown_(dlg, race);
}

FUNCTION_PATCH((void*)0x450AB0, CreateRaceDropdown__);

void gluChat_init_(dialog* dlg)
{
	dword_5999D8 = isHost;

	if (!isHost)
	{
		HideDialog(getControlFromIndex(dlg, 5));
		HideDialog(getControlFromIndex(dlg, 7));
	}
	sub_4B9480(dlg);
	DlgSwooshin(5, gluChatSwishController, dlg, 0);
}

FAIL_STUB_PATCH(gluChat_init);

void sub_4B9BF0_(dialog* dlg)
{
	if (sub_4D4130())
	{
		dlg->fields.dlg.pModalFcn = sub_4B9B10;
	}
	else
	{
		getErrorStringPair(STAR_EDIT_NOT_FOUND, 557);
	}
}

FAIL_STUB_PATCH(sub_4B9BF0);

int sub_4EE210_()
{
	DestroyMapData_();
	if (dword_6BEE8C)
	{
		SMemFree(dword_6BEE8C, "Starcraft\\SWAR\\lang\\Sai_path.cpp", 792, 0);
		dword_6BEE8C = 0;
	}
	freeSaiPaths();
	uselessIterateUnitsAndOrdersDatLoadTables();

	CleanupFlingyDat_();
	CleanupSpritesDat_();

	if (pylon_power_mask)
	{
		SMemFree(pylon_power_mask, "Starcraft\\SWAR\\lang\\CUnitProtoss.cpp", 102, 0);
		pylon_power_mask = NULL;
	}

	if (minimap_Dlg)
	{
		DestroyDialog(minimap_Dlg);
		minimap_Dlg = NULL;
	}

	if (dword_59C1AC)
	{
		SMemFree(dword_59C1AC, "Starcraft\\SWAR\\lang\\minimap.cpp", 2065, 0);
		dword_59C1AC = NULL;
	}

	return 1;
}

FAIL_STUB_PATCH(sub_4EE210);

void sub_4B8D90_(dialog* dlg)
{
	if (dword_5993AC)
	{
		if (dword_5999D0 == 1)
		{
			updateMinimapPreviewDisplayOffOn(0, dlg, 1);
			dword_5999D0 = 0;
		}
		sub_4EE210_();
		dword_5993AC = 0;
		dword_5994DC = 0;
	}
	else if (minimap_Dlg)
	{
		DestroyDialog(minimap_Dlg);
		minimap_Dlg = NULL;

		if (dword_59C1AC)
		{
			SMemFree(dword_59C1AC, "Starcraft\\SWAR\\lang\\minimap.cpp", 2065, 0);
			dword_59C1AC = NULL;
		}
	}
}

FAIL_STUB_PATCH(sub_4B8D90);

bool IsCursorWithin(pt cursor, rect rectangle)
{
	return rectangle.left <= cursor.x && cursor.x <= rectangle.right && rectangle.top <= cursor.y && cursor.y <= rectangle.bottom;
}

void load_MinimapPreview_()
{
	minimap_Dlg = LoadDialog("rez\\minimappreview.bin");
	InitializeDialog_(minimap_Dlg, MiniMapPreviewInteract_);
}

FAIL_STUB_PATCH(load_MinimapPreview);

void gluChat_HoverMinimapPreview_(dialog* dlg)
{
	dword_5999DC = 0;
	dword_5999D0 = 1;
	dword_5993AC = 1;
	load_MinimapPreview_();
	dword_5993AC = 0;
	SetCallbackTimer(1, dlg, 1000, MinimapPreviewProc_);
}

FAIL_STUB_PATCH(gluChat_HoverMinimapPreview);

void gluChat_CustomCtrlID_(dialog* dlg)
{
	FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		gluChat_Button,
		Menu_Generic_Button,
		gluChat_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		gluChat_Textbox_Interact,
		gluChat_Listbox_Interact,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		genericLabelInteract,
		genericLabelInteract,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
	};

	registerMenuFunctions_(functions, dlg, sizeof(functions));
	dword_6D5A38 = 0;
}

FAIL_STUB_PATCH(gluChat_CustomCtrlID);

int gluChat_controlActivation_(signed int last_control_id, dialog* dlg)
{
	switch (last_control_id)
	{
	case 7:
		if (!InReplay && sub_44F7B0() < 2)
		{
			BWFXN_gluPOK_MBox(get_GluAll_String_((GluAllTblEntry)0x72));
		}
		else if (!InReplay && isGameTypeSpecial() && getHumansOnTeam(1) < 2)
		{
			BWFXN_gluPOK_MBox(get_GluAll_String_((GluAllTblEntry)0x0BA));
		}
		else if (map_download && !IsDownloadComplete(map_download))
		{
			BWFXN_gluPOK_MBox(get_GluAll_String_((GluAllTblEntry)0x73));
		}
		else
		{
			_startGame();
			DisableControl(getControlFromIndex(dlg, 7));
			updateMinimapPreviewDisplayOffOn(0, dlg, 0);
			dword_5999D0 = 0;
		}
		return 1;
	case 9:
		SendLobbyMessage();
		return 1;
	case 8:
		DisableControl(getControlFromIndex(dlg, 8));
		sub_4D3860();
		[[fallthrough]];
	case 557:
		gameState = 1;
		killTimerFunc();
		updateMinimapPreviewDisplayOffOn(0, dlg, 1);
		dword_5999D0 = 0;
	}
	lobby_dlg->fields.dlg.pModalFcn = 0;
	return DLG_SwishOut(dlg);
}

FAIL_STUB_PATCH(gluChat_controlActivation);

bool __fastcall gluChat_Main_(dialog* dlg, struct dlgEvent* evt)
{
	dialog* minimap_preview_dlg = getControlFromIndex(dlg, 6);
	dialog* char_history_dlg = getControlFromIndex(dlg, 11);
	dialog* starting_in_dlg = getControlFromIndex(dlg, 23);
	dialog* countdown_dlg = getControlFromIndex(dlg, 24);

	switch (evt->wNo)
	{
	case EVN_MOUSEMOVE:
		updateMinimapPreviewDisplayOffOn(IsCursorWithin(evt->cursor, minimap_preview_dlg->rct), dlg, 1);
		[[fallthrough]];
	case EVN_CHAR:
		if (evt->wVirtKey == VK_HELP)
		{
			SNetGetLeaguePlayerName((int*)curPlayerID, 0x19u);
		}
		break;
	case EVN_WHEELUP:
		doUserEvent(EventUser::USER_SCROLLUP, 0, char_history_dlg->fields.list.pScrlBar);
		return 1;
	case EVN_WHEELDWN:
		doUserEvent(EventUser::USER_SCROLLDOWN, 0, char_history_dlg->fields.list.pScrlBar);
		return 1;
	case EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gluChat_init_(dlg);
			DLG_SwishIn_(dlg);
			genericDlgInteract(dlg, evt);
			sub_4B9BF0_(dlg);
			return 1;
		case EventUser::USER_DESTROY:
			sub_4B8D70(dlg);
			sub_4B8D90_(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			return gluChat_controlActivation_(LastControlID, dlg);
		case EventUser::USER_INIT:
			gluChat_CustomCtrlID_(dlg);
			if (!InReplay
				&& !gameData.save_timestamp
				&& (gameData.got_file_values.victory_conditions || gameData.got_file_values.starting_units || gameData.got_file_values.tournament_mode))
			{
				gluChat_HoverMinimapPreview_(dlg);
			}
			else
			{
				HideDialog(minimap_preview_dlg);
			}
			break;
		case 0x405:
			updatePasswordDisplay(dlg);
			dword_68F520 = 0;
			dword_68F4F0 = 1;
			HideDialog(starting_in_dlg);
			HideDialog(countdown_dlg);
			dword_5999DC = 1;
			return 1;
		}
		break;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluChat_Main);

void loadMenu_gluChat_()
{
	update_lobby_glue = 0;
	dword_5999E8 = 0;
	commonSwishControllers[9] = { 1, 0 };
	dword_5999CC = 0;
	lobby_dlg = LoadDialog("rez\\gluChat.bin");

	playerOwnerDropdownCreate2(lobby_dlg);
	if (GetUserDefaultLangID() == 1042)
	{
		if (dword_6D6438 == NULL)
		{
			dword_6D6438 = ImmGetContext(hWndParent);
		}
		ImmAssociateContext(hWndParent, dword_6D6438);
	}
	int v9 = gluLoadBINDlg_(lobby_dlg, gluChat_Main_);
	if (GetUserDefaultLangID() == 1042)
	{
		if (dword_6D6438 == NULL)
		{
			dword_6D6438 = ImmGetContext(hWndParent);
		}
		dword_6D6438 = ImmAssociateContext(hWndParent, 0u);
	}
	switch (v9)
	{
	case 556:
		break;
	case 8:
	case 557:
		if (NetMode.as_number == 'BNET')
		{
			glGluesMode = GLUE_BATTLE;
		}
		else if (NetMode.as_number == 'MDMX' || NetMode.as_number == 'MODM')
		{
			glGluesMode = GLUE_MODEM;
		}
		else
		{
			glGluesMode = GLUE_GAME_SELECT;
		}
		InReplay = 0;
		freeChkFileMem_();
		break;
	default:
		glGluesMode = MenuPosition::GLUE_MAIN_MENU;
		break;
	}
	changeMenu();
}

FAIL_STUB_PATCH(loadMenu_gluChat);

void gluLoad_CustomCtrlID_(dialog* a1)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		Menu_Generic_Button,
		gluLoad_List,
		Menu_Generic_Button,
	};

	DlgSwooshin(3, &commonSwishControllers[23], a1, 0);
	registerMenuFunctions_(functions, a1, sizeof(functions));
}

FAIL_STUB_PATCH(gluLoad_CustomCtrlID);

bool __fastcall gluLoad_Main_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			DLG_SwishIn_(dlg);
			break;
		case USER_ACTIVATE:
			if (LastControlID == 7)
			{
				DeleteSavedGame();
				return 1;
			}
			return DLG_SwishOut(dlg);
		case USER_INIT:
			gluLoad_CustomCtrlID_(dlg);
			break;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluLoad_Main);

void loadMenu_gluLoad_()
{
	glu_load_Dlg = loadAndInitFullMenuDLG_("rez\\gluLoad.bin");

	switch (gluLoadBINDlg_(glu_load_Dlg, gluLoad_Main_))
	{
	case 4:
		CMDRECV_LoadGame(byte_599DA4);
		break;
	case 5:
		glGluesMode = IsExpansion != 0 ? GLUE_EX_CAMPAIGN : GLUE_CAMPAIGN;
		break;
	default:
		glGluesMode = GLUE_MAIN_MENU;
		break;
	}

	changeMenu();
}

FAIL_STUB_PATCH(loadMenu_gluLoad);

int sub_4B4600_(dialog* a1)
{
	const char* v3;
	switch (dword_59B3D0)
	{
	case 0:
	case 3:
		v3 = get_GluAll_String(DISCONNECTED);
		break;
	case 1:
		v3 = get_GluAll_String((GluAllTblEntry)18);
		break;
	case 2:
		v3 = get_GluAll_String((GluAllTblEntry)17);
		break;
	case 4:
		v3 = get_GluAll_String(UNDECIDED);
		break;
	default:
		v3 = NULL;
		break;
	}

	if (dword_6D5A60)
	{
		v3 = get_GluAll_String((GluAllTblEntry)0xB2);
	}
	else if (v3)
	{
		dialog* v2 = getControlFromIndex(a1, 2);
		v2->pszText = (char*)v3;
		if ((v2->lFlags & CTRL_UPDATE) == 0)
		{
			v2->lFlags |= CTRL_UPDATE;
			updateDialog(v2);
		}
		v2->lFlags |= CTRL_DLG_ACTIVE;
	}

	getControlFromIndex(a1, 8)->lFlags |= CTRL_DLG_ACTIVE;

	char fileName[260];
	strcpy(fileName, byte_59B628);
	strcat(fileName, "pMain.pcx");

	int height;
	int width;
	void* buffer;
	while (!SBmpAllocLoadImage(fileName, 0, &buffer, &width, &height, 0, 0, allocFunction))
	{
		SysWarn_FileNotFound(fileName, SErrGetLastError());
	}

	dialog* v13 = getControlFromIndex(a1, 1);
	v13->srcBits.ht = height;
	v13->srcBits.wid = width;
	v13->srcBits.data = (u8*)buffer;
	if ((v13->lFlags & CTRL_UPDATE) == 0)
	{
		v13->lFlags |= CTRL_UPDATE;
		updateDialog(v13);
	}
	return sub_4B42D0(a1);
}

FAIL_STUB_PATCH(sub_4B4600);

void saveGame_Create_(dialog* dlg)
{
	DLG_SwishIn_(dlg);
	UpdateCancelButton(dlg, 5u, 0x20000000);
	UpdateOKButton(dlg, 3u, 0x20000000);
	dialog* v3 = getControlFromIndex(dlg, 3);
	if (v3)
	{
		HideDialog(v3);
	}

	dialog* v6 = getControlFromIndex(dlg, 1);
	if (byte_51BFD8 < v6->fields.scroll.bSliderSkip || byte_51BFD8 == -1)
	{
		dlgEvent v11;
		v11.wNo = EVN_USER;
		v11.dwUser = USER_SELECT;
		*(_DWORD*)&v11.wSelection = (unsigned __int8)byte_51BFD8;
		v6->pfcnInteract(v6, &v11);
		DlgSetSelected_UpdateScrollbar(byte_51BFD8, v6);
	}

	dialog* v9 = getControlFromIndex(dlg, 4);
	v9->pszText = (char*)get_GluAll_String((GluAllTblEntry)0xB3);
	if ((v9->lFlags & DialogFlags::CTRL_UPDATE) == 0)
	{
		v9->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog(v9);
	}
}

FAIL_STUB_PATCH(saveGame_Create);

void savegameBIN_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		savegame_Listbox,
		savegame_TextBox,
		savegame_Delete,
		NULL
	};

	registerMenuFunctions_(functions, dlg, sizeof(functions));
	saveGame_InitChildren(dlg);
}

FAIL_STUB_PATCH(savegameBIN_CustomCtrlID);

bool __fastcall savegameBIN_Main_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			saveGame_Create_(dlg);
			break;
		case USER_DESTROY:
			saveGame_Destroy(dlg);
			break;
		case USER_ACTIVATE:
			if (dialog* v5 = getControlFromIndex(dlg, 2))
			{
				SStrCopy(byte_51BFB8, v5->pszText, 0x7FFFFFFFu);
				trimTrailingSpaces(byte_51BFB8);
			}
			break;
		case USER_INIT:
			savegameBIN_CustomCtrlID_(dlg);
			break;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(savegameBIN_Main);

int loadOKCancelBIN_(int a1, char* message, HANDLE a3)
{
	dword_6D1244 = a1;
	dword_5128F0 = a1;

	size_t message_size = strlen(message) + 1;
	okcancel_message = (char*) SMemAlloc(message_size, "Starcraft\\SWAR\\lang\\okcancel.cpp", 174, 0);
	strcpy_s(okcancel_message, message_size, message);

	dword_6D1248 = a3;

	dialog* okcancel_bin = LoadDialog("rez\\okcancel.bin");
	registerMenuFunctions_(0, okcancel_bin, 0);
	return gluLoadBINDlg_(okcancel_bin, okcancel_Interact);
}

FAIL_STUB_PATCH(loadOKCancelBIN);


bool __fastcall gluScore_Tab_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_DESTROY:
			if (dword_6D63BC)
			{
				if (stru_6DC2A4.data)
				{
					SMemFree(stru_6DC2A4.data, "Starcraft\\SWAR\\lang\\gluScore.cpp", 574, 0);
					stru_6DC2A4.data = 0;
				}
			}
			dlg->srcBits.data = 0;
			dword_6D63BC = 0;
			break;
		case USER_ACTIVATE:
			sub_4B4520(dlg);
			return 1;
		case USER_INIT:
			if (!dword_6D63BC)
			{
				char v13[MAX_PATH];
				strcpy(v13, byte_59B628);
				strcat(v13, "untab.pcx");
				AllocBackgroundImage(v13, &stru_6DC2A4, 0, "Starcraft\\SWAR\\lang\\gluScore.cpp", 548);
				dword_6D63BC = 1;
			}
			dlg->pfcnUpdate = (FnUpdate)sub_4B3820;
			dlg->srcBits.wid = stru_6DC2A4.wid;
			dlg->srcBits.ht = stru_6DC2A4.ht;
			dlg->lFlags |= CTRL_UNKOWN1;
			dlg->srcBits.data = stru_6DC2A4.data;
			UpdateDlgOnFlag(dlg);
			break;
		case USER_SELECT:
			if (*(_DWORD*)&evt->wSelection)
			{
				sub_4B4520(dlg);
			}
			return 1;
		case USER_SHOW:
			if (dlg == getControlFromIndex(dlg, 3))
			{
				sub_4B4520(dlg);
			}
			break;
		}
	}

	return genericOptionInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluScore_Tab);

int ConfirmReplayOverwrite_(char* filename, __int16 a2)
{
	char* v3 = (char*)malloc(strlen(filename) + 3);
	BWFXN_SetFont(FontBase[2]);
	size_t v4 = sub_41FC20(filename, 230);
	BWFXN_SetFont(0);
	strncpy(v3, filename, v4);
	v3[v4] = 0;
	if (v4 != strlen(filename))
	{
		*(_DWORD*)&v3[strlen(v3)] = '...';
	}

	char buff[256];
	_snprintf(buff, 0x100u, GetNetworkTblString(a2), v3);

	free(v3);
	return loadOKCancelBIN_(1, buff, dword_6D0F2C) == -2;
}

FAIL_STUB_PATCH(ConfirmReplayOverwrite);

int CopyLastReplayTo_(char* a1)
{
	CHAR FileName[260];
	if (!getDirectoryPath(FileName, 0x104u, a1))
	{
		return 0;
	}

	DWORD v2 = GetFileAttributesA(FileName) != -1;
	if (v2 && !ConfirmReplayOverwrite_(a1, 3))
	{
		return -1;
	}

	return CopyLastReplay(a1);
}

FAIL_STUB_PATCH(CopyLastReplayTo);

int LoadSaveGameBIN_Main_(int a1, Race a2)
{
	dword_51BFD4 = a1;
	dword_51BFB4 = a2;
	byte_51BFD8 = 0;
	dword_6D0F2C = (&off_50E040[a2])[a1 != 0 ? 3 : 0];

	savegame_Dlg = LoadDialog("rez\\savegame.bin");
	if (GetUserDefaultLangID() == 1042)
	{
		if (!dword_6D6438)
		{
			dword_6D6438 = ImmGetContext(hWndParent);
		}
		ImmAssociateContext(hWndParent, dword_6D6438);
	}

	int v12 = gluLoadBINDlg_(savegame_Dlg, savegameBIN_Main_);
	if (GetUserDefaultLangID() == 1042)
	{
		if (!dword_6D6438)
		{
			dword_6D6438 = ImmGetContext(hWndParent);
		}
		dword_6D6438 = ImmAssociateContext(hWndParent, 0);
	}

	int v13;
	switch (v12)
	{
	case -2:
		v13 = CopyLastReplayTo_(byte_51BFB8);
		if (v13 == 1)
		{
			dword_6D0F2C = 0;
			return 1;
		}
		else if (v13 == 0)
		{
			const char* v14 = GetNetworkTblString(101);
			loadOKBIN_(1, v14, dword_6D0F2C);
		}
		break;
	case 3:
		DeleteFileIfExists();
		break;
	}
	dword_6D0F2C = 0;
	return 0;
}

FAIL_STUB_PATCH(LoadSaveGameBIN_Main);

bool __fastcall gluScore_SaveReplay_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			if (dword_6D5A60 || saveLoadSuccess || CampaignIndex || is_spawn)
			{
				DisableControl(dlg);
			}
			break;
		case EventUser::USER_ACTIVATE:
			if (LoadSaveGameBIN_Main_(dword_59B75C, Players[g_LocalNationID].nRace))
			{
				dlg->pszText = get_GluAll_String_((GluAllTblEntry)177);
				if ((dlg->lFlags & CTRL_UPDATE) == 0)
				{
					dlg->lFlags = dlg->lFlags | CTRL_UPDATE;
					updateDialog(dlg);
				}
				DisableControl(dlg);
			}
			return 1;
		}
	}

	return Menu_Generic_Button(dlg, evt);
}

FAIL_STUB_PATCH(gluScore_SaveReplay);

void gluScore_CustomCtrlID_(dialog* dlg)
{
	static swishTimer timers[] = { {1, 0} };

	static FnInteract gluScore_menu_functions[] = {
		genericImageInteract,
		genericLabelInteract,
		gluScore_Tab_,
		gluScore_Tab_,
		gluScore_Tab_,
		gluScore_Tab_,
		Menu_Generic_Button,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		gluScore_PlayerRaceIcon,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_SaveReplay_,
	};

	DlgSwooshin(_countof(timers), timers, dlg, 500);
	registerMenuFunctions_(gluScore_menu_functions, dlg, sizeof(gluScore_menu_functions));
}

FAIL_STUB_PATCH(gluScore_CustomCtrlID);

bool __fastcall gluScore_Main_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			sub_4B4600_(dlg);
			DLG_SwishIn_(dlg);
			break;
		case USER_DESTROY:
			Sleep(0x3E8u);
			sub_4B30A0(dlg);
			break;
		case USER_ACTIVATE:
			stopSounds();
			return DLG_SwishOut(dlg);
		case USER_INIT:
			gluScore_CustomCtrlID_(dlg);
			break;
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluScore_Main);

void sub_4DBEE0_(ExpandedCampaignMenuEntry* a1)
{
	CharacterData v5;

	if (!LoadCharacterData(&v5, playerName))
	{
		const char* v1 = GetNetworkTblString(72);
		if ((_stricmp(playerName, v1) || !verifyCharacterFile(&v5, playerName)) && !outOfGame)
		{
			doNetTBLError(0, 0, 0, 88);
		}
	}
	int* v3 = IsExpansion ? v5.unlocked_expcampaign_mission + a1->race : v5.unlocked_campaign_mission + a1->race;
	if (*v3 < a1->next_mission)
	{
		*v3 = a1->next_mission;
		CreateCharacterFile(&v5);
	}
}

FAIL_STUB_PATCH(sub_4DBEE0);

void sub_4DBF80_()
{
	if (!multiPlayerMode && !dword_51CA1C)
	{
		updateActiveCampaignMission_();
		if (active_campaign_menu_entry && active_campaign_menu_entry->next_mission)
		{
			sub_4DBEE0_((ExpandedCampaignMenuEntry*) active_campaign_menu_entry + 1);
		}
	}
}

FAIL_STUB_PATCH(sub_4DBF80);

char* score_screens_[] = {
	"glue\\scoreZd\\",
	"glue\\scoreZv\\",
	"glue\\scoreTd\\",
	"glue\\scoreTv\\",
	"glue\\scorePd\\",
	"glue\\scorePv\\",
};

MusicTrack score_music_track_[] = {
	MT_ZERG_DEFEAT,
	MT_ZERG_VICTORY,
	MT_TERRAN_DEFEAT,
	MT_TERRAN_VICTORY,
	MT_PROTOSS_DEFEAT,
	MT_PROTOSS_VICTORY,
};

void loadMenu_gluScore_()
{
	char v25[260];

	ApplyGameVictoryStatus(dword_59B73C, &dword_59B3D0);

	dword_59B75C = dword_59B3D0 == 1;
	int v0 = dword_6D5A60 ? 1 : (dword_59B3D0 == 1) + 2 * Players[g_LocalNationID].nRace;

	glGluesMode = dword_512AB0[v0];
	strcpy(byte_59B628, score_screens_[v0]);
	DLGMusicFade_(score_music_track_[v0]);

	strcpy(v25, score_screens_[v0]);
	strcat(v25, "iScore.grp");
	dword_59B72C = (void*)LoadGraphic(v25, 0, "Starcraft\\SWAR\\lang\\gluScore.cpp", 1376);

	strcpy(v25, score_screens_[v0]);
	strcat(v25, "tminimap.pcx");
	if (!SBmpLoadImage(v25, 0, byte_59B730, 12, 0, 0, 0))
	{
		SysWarn_FileNotFound(v25, SErrGetLastError());
	}
	strcpy(byte_59B628, score_screens_[v0]);
	sub_4BCA80_(SFX_glue_scorefill);

	gluScore_Dlg = LoadDialog("rez\\gluScore.bin");

	if (gluLoadBINDlg_(gluScore_Dlg, gluScore_Main_) == 7)
	{
		if (multiPlayerMode)
		{
			BWFXN_NetSelectReturnMenu();
		}
		else if (!ContinueCampaign_(dword_59B75C))
		{
			glGluesMode = GLUE_MAIN_MENU;
		}
	}
	else
	{
		sub_4DBF80_();
		glGluesMode = GLUE_MAIN_MENU;
	}

	changeMenu();
	stopMusic();
	if (dword_59B72C)
	{
		SMemFree(dword_59B72C, "Starcraft\\SWAR\\lang\\gluScore.cpp", 1409, 0);
	}
	dword_59B618 = 0;
}

FAIL_STUB_PATCH(loadMenu_gluScore);

int SwitchMenu_()
{
	if (!GetModuleFileNameA(NULL, main_directory, MAX_PATH))
		main_directory[0] = 0;

	char* lastDirectorySeparator = strrchr(main_directory, '\\');
	if (lastDirectorySeparator)
		*lastDirectorySeparator = 0;

	for (int i = 0; i < 256; ++i)
	{
		byte_50CDC1[i] = i;
	}
	LoadMenuFonts();
	loadtEffectPcx();
	loadCursor();
	loadTFontPcx();
	loadDlgGrp();
	init_gluesounds_();
	if (gwGameMode == GAME_WIN)
	{
		gwGameMode = GAME_GLUES;
		glGluesMode = GLUE_SCORE_T_VICTORY;
		goto LABEL_28;
	}
	if (gwGameMode == GAME_LOSE)
	{
		gwGameMode = GAME_GLUES;
		glGluesMode = GLUE_SCORE_T_VICTORY;
		goto LABEL_28;
	}
	if (Ophelia && !multiPlayerMode)
	{
		Ophelia = 0;
		if (!playerName[0])
		{
			const char* v3 = GetNetworkTblString(72);
			SStrCopy(playerName, v3, sizeof(playerName));
		}
		customSingleplayer[0] = 0;
		IsExpansion = level_cheat_is_bw != 0;
		if (level_cheat_is_bw && !is_expansion_installed || !loadCampaignBIN() || !CreateCampaignGame_((MapData)level_cheat_mission))
		{
			glGluesMode = MenuPosition::GLUE_MAIN_MENU;
			IsExpansion = 0;
			goto LABEL_28;
		}
		if (level_cheat_race == Race::RACE_Zerg)
		{
			glGluesMode = GLUE_READY_Z;
		}
		else if (level_cheat_race == Race::RACE_Terran)
		{
			glGluesMode = GLUE_READY_T;
		}
		else if (level_cheat_race == Race::RACE_Protoss)
		{
			glGluesMode = GLUE_READY_P;
		}
		else
		{
			glGluesMode = MenuPosition::GLUE_MAIN_MENU;
			IsExpansion = 0;
			goto LABEL_28;
		}

		OpheliaEnabled = 1;
		if (GameCheats & CheatFlags::CHEAT_NoGlues)
			gwGameMode = GAME_RUNINIT;
	}
LABEL_28:
	Sleep(1500u);
	if (load_screen)
	{
		DestroyDialog(load_screen);
		load_screen = 0;
	}
	dword_50E064 = -1;
	if (!byte_51A0E9)
	{
		memset(stru_6CEB40, 0, sizeof(PALETTEENTRY[256]));
		byte_51A0E9 = 1;
		memcpy(stru_6CE720, GamePalette, sizeof(PALETTEENTRY[256]));
		gluDlgFadePalette(3u);
		BWFXN_RedrawTarget_();
	}
	RefreshCursor_0();
	dword_6D5E38 = jmpNoMenu;
	if (glue_background_palette[0].data)
		SMemFree(glue_background_palette[0].data, "Starcraft\\SWAR\\lang\\glues.cpp", 442, 0);
	memset(glue_background_palette, 0, sizeof(Bitmap[129]));
	while (gwGameMode == GAME_GLUES)
	{
		switch (glGluesMode)
		{
		case GLUE_GENERIC:
			loadMenu_None();
			break;
		case GLUE_MAIN_MENU:
			outOfGame = 0;
			loadMenu_gluMain_();
			break;
		case GLUE_LOGIN:
			dword_51C414 = 0;
			loadMenu_gluLogin_();
			break;
		case GLUE_CAMPAIGN:
			loadMenu_gluCmpgn_();
			break;
		case GLUE_READY_T:
			loadMenu_gluRdy(MusicTrack::MT_TERRAN_READY, "rez\\glurdyt.bin", gluRdy_BINDLG_Loop);
			break;
		case GLUE_READY_Z:
			loadMenu_gluRdy(MusicTrack::MT_ZERG_READY, "rez\\glurdyz.bin", gluRdy_BINDLG_Loop);
			break;
		case GLUE_READY_P:
			loadMenu_gluRdy(MusicTrack::MT_PROTOSS_READY, "rez\\glurdyp.bin", gluRdy_BINDLG_Loop);
			break;
		case GLUE_EX_CAMPAIGN:
			loadMenu_gluExpCmpgn_();
			break;
		case GLUE_CONNECT:
			loadMenu_gluConn_();
			break;
		case GLUE_MODEM:
			loadMenu_gluModem_();
			break;
		case GLUE_GAME_SELECT:
		case GLUE_DIRECT:
			loadMenu_gluJoin_();
			break;
		case GLUE_CREATE:
			loadMenu_gluCustm_(0);
			break;
		case GLUE_CREATE_MULTI:
			loadMenu_gluCustm_(1);
			break;
		case GLUE_CHAT:
			loadMenu_gluChat_();
			break;
		case GLUE_LOAD:
			loadMenu_gluLoad_();
			break;
		case GLUE_SCORE_Z_DEFEAT:
		case GLUE_SCORE_Z_VICTORY:
		case GLUE_SCORE_T_DEFEAT:
		case GLUE_SCORE_T_VICTORY:
		case GLUE_SCORE_P_DEFEAT:
		case GLUE_SCORE_P_VICTORY:
			loadMenu_gluScore_();
			break;
		case GLUE_BATTLE:
			dword_51C414 = 1;
			dword_50E064 = -1;
			loadMenu_gluBNRes();
			break;
		default:
			glGluesMode = MenuPosition::GLUE_MAIN_MENU;
			break;
		}
	}
	if (glue_background_palette[0].data)
		SMemFree(glue_background_palette[0].data, "Starcraft\\SWAR\\lang\\glues.cpp", 442, 0);
	memset(glue_background_palette, 0, sizeof(Bitmap[129]));
	stopSounds();
	stopMusic();
	dword_6D5E20 = &GameScreenBuffer;
	if (!byte_51A0E9)
	{
		memset(stru_6CEB40, 0, sizeof(PALETTEENTRY[256]));
		byte_51A0E9 = 1;
		memcpy(stru_6CE720, GamePalette, sizeof(PALETTEENTRY[256]));
		gluDlgFadePalette(3u);
		BWFXN_RedrawTarget_();
	}
	RefreshCursor_0();
	playsound_init_UI_(0);
	if (dword_51C60C)
		SMemFree(dword_51C60C, "Starcraft\\SWAR\\lang\\glues.cpp", 338, 0);
	BWFXN_GameEndTarget();
	if (dword_6D125C)
		SMemFree(dword_6D125C, "Starcraft\\SWAR\\lang\\light.cpp", 121, 0);
	dword_6D125C = 0;
	sub_4DC940();
	if (dword_51C40C)
		SMemFree(dword_51C40C, "Starcraft\\SWAR\\lang\\glues.cpp", 370, 0);
	return sub_4DC870();
}

FAIL_STUB_PATCH(SwitchMenu);

void Game_Close_()
{
	dword_5967F0 = 1;
	if (glGluesMode == GLUE_BATTLE)
	{
		DWORD dwProcessId;
		GetWindowThreadProcessId(hWndParent, &dwProcessId);
		EnumWindows(EnumFunc, dwProcessId);
	}
	glGluesMode = GLUE_GENERIC;
	if (gwGameMode == GAME_RUN)
	{
		GameState = 0;
		gwNextGameMode = GAME_EXIT;
		if (!InReplay)
		{
			replay_header.ReplayFrames = ElapsedTimeFrames;
		}
	}
	else
	{
		gwGameMode = GAME_EXIT;
	}
}

FAIL_STUB_PATCH(Game_Close);

void GameShowCursor_(bool show_cursor)
{
	if (cursor == NULL)
	{
		cursor = LoadCursor(NULL, IDC_ARROW);
	}
	SetCursor(show_cursor ? cursor : NULL);

	POINT Point;
	GetCursorPos(&Point);
	SetCursorPos(Point.x, Point.y);
	if (is_cursor_shown != show_cursor)
	{
		is_cursor_shown = show_cursor;
		ShowCursor(show_cursor);
	}
}

FAIL_STUB_PATCH(GameShowCursor);

void doCursorClip_(int a1)
{
	if (a1 != dword_6D5DD0)
	{
		dword_6D5DD0 = a1;
		if (a1 && !dword_6D5DD4)
		{
			SetCursorClipBounds();
		}
		ClipCursor(dword_6D5DD0 ? &screen : NULL);
	}
}

FAIL_STUB_PATCH(doCursorClip);

void Game_NumLockInit_()
{
	is_keycode_used[VK_NUMPAD0] = 0;
	is_keycode_used[VK_NUMPAD1] = 0;
	is_keycode_used[VK_NUMPAD2] = 0;
	is_keycode_used[VK_NUMPAD3] = 0;
	is_keycode_used[VK_NUMPAD4] = 0;
	is_keycode_used[VK_NUMPAD5] = 0;
	is_keycode_used[VK_NUMPAD6] = 0;
	is_keycode_used[VK_NUMPAD7] = 0;
	is_keycode_used[VK_NUMPAD8] = 0;
	is_keycode_used[VK_NUMPAD9] = 0;
	is_keycode_used[VK_PRIOR] = 0;
	is_keycode_used[VK_NEXT] = 0;
	is_keycode_used[VK_END] = 0;
	is_keycode_used[VK_HOME] = 0;
	is_keycode_used[VK_LEFT] = 0;
	is_keycode_used[VK_UP] = 0;
	is_keycode_used[VK_RIGHT] = 0;
	is_keycode_used[VK_DOWN] = 0;
}

FAIL_STUB_PATCH(Game_NumLockInit);

void TakeScreenshot_()
{
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	char buff[MAX_PATH];
	_snprintf(
		buff,
		MAX_PATH,
		"SCScrnShot_%02d%02d%02d_%02d%02d%02d.pcx",
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wYear % 100,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond);

	if (SDrawCaptureScreen(buff) && gwGameMode == GAME_RUN)
	{
		const char* v0 = GetNetworkTblString(110);
		char text[512];
		_snprintf(text, 0x200u, v0, buff);
		InfoMessage(2000, text);
	}
}

FAIL_STUB_PATCH(TakeScreenshot);

void BWFXN_Game_ButtonDown_(int a1, EventNo a4, LPARAM lParam)
{
	if (((unsigned __int8)InputFlags & (unsigned __int8)~(_BYTE)a1 & 0x2A) == 0)
	{
		LOWORD(InputFlags) = a1 | InputFlags;
		SetCapture(hWndParent);

		dlgEvent event;
		event.wNo = a4;

		Mouse.x = event.cursor.x = min(GET_X_LPARAM(lParam), SCREEN_WIDTH - 1);
		Mouse.y = event.cursor.y = min(GET_Y_LPARAM(lParam), SCREEN_HEIGHT - 1);

		if (!sendInputToAllDialogs_(&event) && input_procedures[a4])
		{
			input_procedures[a4](&event);
		}
	}
}

FAIL_STUB_PATCH(BWFXN_Game_ButtonDown);

void BWFXN_Game_ButtonUp_(int a1, EventNo a4, LPARAM lParam)
{
	__int16 v4 = InputFlags & ~(_WORD)a1;
	if ((v4 & 0x2A) == 0)
	{
		LOWORD(InputFlags) = v4;
		ReleaseCapture();

		dlgEvent event;
		event.wNo = a4;

		Mouse.x = event.cursor.x = min(GET_X_LPARAM(lParam), SCREEN_WIDTH - 1);
		Mouse.y = event.cursor.y = min(GET_Y_LPARAM(lParam), SCREEN_HEIGHT - 1);

		if (!sendInputToAllDialogs_(&event) && input_procedures[a4])
		{
			input_procedures[a4](&event);
		}
	}
}

FAIL_STUB_PATCH(BWFXN_Game_ButtonUp);

void Game_BtnDoubleClick_(int a1, EventNo a4, LPARAM lParam)
{
	if (((unsigned __int8)InputFlags & (unsigned __int8)~(_BYTE)a1 & 0x2A) == 0)
	{
		LOWORD(InputFlags) = a1 | InputFlags;
		SetCapture(hWndParent);

		dlgEvent event;
		event.wNo = a4;

		Mouse.x = event.cursor.x = min(GET_X_LPARAM(lParam), SCREEN_WIDTH - 1);
		Mouse.y = event.cursor.y = min(GET_Y_LPARAM(lParam), SCREEN_HEIGHT - 1);

		if (!sendInputToAllDialogs_(&event) && input_procedures[a4])
		{
			input_procedures[a4](&event);
		}
	}
}

FAIL_STUB_PATCH(Game_BtnDoubleClick);

void Game_MouseWheel_(EventNo wNo, int a2)
{
	dlgEvent v3;

	v3.dwUser = USER_CREATE;
	v3.wSelection = LOWORD(a2);
	v3.wUnk_0x06 = HIWORD(a2);
	v3.wVirtKey = 0;
	v3.wUnk_0x0A = 0;
	v3.wNo = wNo;
	v3.cursor.x = 0;
	v3.cursor.y = 0;
	if (!sendInputToAllDialogs_(&v3) && input_procedures[wNo])
	{
		input_procedures[wNo](&v3);
	}
}

FAIL_STUB_PATCH(Game_MouseWheel);

void Game_Capturechanged_()
{
	if (InputFlags & 0x2A)
	{
		InputFlags &= 0xD5;
		if (gwGameMode == GAME_RUN)
		{
			SetInGameInputProcs();
		}
		else
		{
			InitializeInputProcs();
		}
	}
}

FAIL_STUB_PATCH(Game_Capturechanged);

LRESULT __stdcall MainWindowProc_(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	dlgEvent v16;

	switch (Msg)
	{
	case WM_DESTROY:
		hWndParent = NULL;
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT paint;
			BeginPaint(hWnd, &paint);
			EndPaint(hWnd, &paint);
		}
		dword_6D5E1C = 1;
		return 0;
	case WM_CLOSE:
		Game_Close_();
		return 0;
	case WM_ERASEBKGND:
	case WM_SETCURSOR:
		return 1;
	case WM_ACTIVATEAPP:
		dword_51BFA8 = wParam;
		GameShowCursor_(dword_51BFA8 == 0);
		doCursorClip_(dword_51BFA8);
		memset(is_keycode_used, 0, sizeof(is_keycode_used));
		if (dword_51BFA8)
		{
			dword_6D5E1C = 1;
			if (gwGameMode == GAME_GLUES && glGluesMode == MenuPosition::GLUE_BATTLE)
			{
				SetFocus(FindWindowA("SDlgDialog", 0));
			}
		}
		break;
	case WM_NCACTIVATE:
		if (wParam)
		{
			memset(is_keycode_used, 0, sizeof(is_keycode_used));
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		is_keycode_used[wParam] = 1;
		if (wParam == VK_NUMLOCK)
		{
			Game_NumLockInit_();
		}
		is_keycode_used[VK_MENU] = (GetKeyState(VK_MENU) & 0x8000) != 0;
		if ((InputFlags & 0x2A) == 0)
		{
			WORD key_flags = HIWORD(lParam);
			v16.wVirtKey = wParam;
			v16.wUnk_0x0A = BWFXN_Game_KeyState();
			v16.wNo = (key_flags & KF_REPEAT) ? EVN_KEYRPT : EVN_KEYFIRST;
			if (!sendInputToAllDialogs_(&v16))
			{
				InputProcedure v11 = (key_flags & KF_REPEAT) ? input_procedures[EventNo::EVN_KEYRPT] : input_procedures[EventNo::EVN_KEYDOWN];
				if (v11)
				{
					v11(&v16);
				}
			}
		}
		if (wParam == VK_SCROLL)
		{
			dword_6D5E1C = 1;
			return DefWindowProcA(hWnd, Msg, VK_SCROLL, lParam);
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		is_keycode_used[wParam] = 0;
		is_keycode_used[VK_MENU] = (GetKeyState(VK_MENU) & 0x8000) != 0;
		if (wParam == VK_SNAPSHOT)
		{
			TakeScreenshot_();
			return 1;
		}
		if ((InputFlags & 0x2A) == 0)
		{
			v16.wVirtKey = wParam;
			v16.wUnk_0x0A = BWFXN_Game_KeyState();
			v16.wNo = EventNo::EVN_KEYUP;
			if (!sendInputToAllDialogs_(&v16) && input_procedures[EventNo::EVN_KEYUP])
			{
				input_procedures[EventNo::EVN_KEYUP](&v16);
			}
		}
		break;
	case WM_CHAR:
		if ((InputFlags & 0x2A) == 0)
		{
			v16.wNo = EventNo::EVN_CHAR;
			v16.wVirtKey = wParam;
			v16.wUnk_0x0A = BWFXN_Game_KeyState();
			dlg = 0;
			if (lParam & 0x40000000)
			{
				HIBYTE(v16.wUnk_0x0A) |= 1;
			}
			if (!sendInputToAllDialogs_(&v16) && input_procedures[EventNo::EVN_CHAR])
			{
				input_procedures[EventNo::EVN_CHAR](&v16);
			}
		}
		return 1;
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_ENDCOMPOSITION:
		return 1;
	case WM_IME_COMPOSITION:
		if (GetUserDefaultLangID() != 1042)
		{
			break;
		}
		if (InputFlags & 0x2A)
		{
			return 1;
		}

		{
			HIMC v7 = ImmGetContext(hWnd);
			DWORD v12;
			if (lParam & 2048)
			{
				v12 = 2048;
			}
			else if (lParam & 8)
			{
				v12 = 8;
			}
			else
			{
				ImmReleaseContext(hWnd, v7);
				break;
			}
			DWORD v14 = ImmGetCompositionStringA(v7, v12, 0, 0);
			unsigned __int8* v13 = (unsigned __int8*)malloc(v14);
			ImmGetCompositionStringA(v7, v12, v13, v14);
			ImmReleaseContext(hWnd, v7);
			if (dword_6DC2E0 == 1)
			{
				v16.wNo = EventNo::EVN_CHAR;
				v16.wVirtKey = VK_BACK;
				v16.wUnk_0x0A = 160;
				if (!sendInputToAllDialogs_(&v16) && input_procedures[EventNo::EVN_CHAR])
				{
					input_procedures[EventNo::EVN_CHAR](&v16);
				}
				dword_6DC2E0 = 0;
			}
			if (v14 >= 2)
			{
				dlg = (dialog*)1;
				dword_596A14 = 0;
			}
			if (v14 == 0)
			{
				goto LABEL_52;
			}

			int v10 = 0;
			while (1)
			{
				v16.wNo = EventNo::EVN_CHAR;
				v16.wVirtKey = v13[v10];
				v16.wUnk_0x0A = 160;
				if (!sendInputToAllDialogs_(&v16) && input_procedures[EventNo::EVN_CHAR])
				{
					input_procedures[EventNo::EVN_CHAR](&v16);
				}
				if (dword_596A14 == 1)
				{
					break;
				}
				if (++v10 >= v14)
				{
					if ((lParam & 8) != 0 && v14)
					{
						dword_6DC2E0 = 1;
					}
					goto LABEL_52;
				}
			}
			ImmSetCompositionStringA(v7, 9u, 0, 0, 0, 0);
			if (v10 == 1)
			{
				v16.wNo = EventNo::EVN_CHAR;
				v16.wVirtKey = VK_BACK;
				v16.wUnk_0x0A = 160;
				if (!sendInputToAllDialogs_(&v16) && input_procedures[EventNo::EVN_CHAR])
				{
					input_procedures[EventNo::EVN_CHAR](&v16);
				}
				dword_6DC2E0 = 0;
			}

		LABEL_52:
			free(v13);
			ImmReleaseContext(hWnd, v7);
			return 1;
		}
	case WM_COMMAND:
		if (input_procedures[EventNo::EVN_SYSCHAR])
		{
			v16.wNo = EventNo::EVN_SYSCHAR;
			v16.wVirtKey = wParam;
			input_procedures[EventNo::EVN_SYSCHAR](&v16);
		}
		return 1;
	case WM_SYSCOMMAND:
		if (wParam != SC_CLOSE)
		{
			if (wParam != SC_KEYMENU && wParam != SC_PREVWINDOW)
			{
				break;
			}
		}
		else if (!load_screen)
		{
			if (gwGameMode != GAME_RUN)
			{
				SNetLeagueLogout(playerName);
				Game_Close_();
				return 0;
			}
			PostMessageA(hWnd, WM_COMMAND, 0xFFFF9C6B, 0);
			return DefWindowProcA(hWnd, Msg, SC_RESTORE, lParam);
		}

		return 0;
	case WM_MOUSEMOVE:
		LOBYTE(InputFlags) = InputFlags | 1;
		Mouse.x = min(GET_X_LPARAM(lParam), SCREEN_WIDTH - 1);
		Mouse.y = min(GET_Y_LPARAM(lParam), SCREEN_HEIGHT - 1);
		return 1;
	case WM_LBUTTONDOWN:
		BWFXN_Game_ButtonDown_(2, EventNo::EVN_LBUTTONDOWN, lParam);
		return 1;
	case WM_LBUTTONUP:
		BWFXN_Game_ButtonUp_(2, EventNo::EVN_LBUTTONUP, lParam);
		return 1;
	case WM_LBUTTONDBLCLK:
		Game_BtnDoubleClick_(2, EventNo::EVN_LBUTTONDBLCLK, lParam);
		return 1;
	case WM_RBUTTONDOWN:
		BWFXN_Game_ButtonDown_(8, EventNo::EVN_RBUTTONDOWN, lParam);
		return 1;
	case WM_RBUTTONUP:
		BWFXN_Game_ButtonUp_(8, EventNo::EVN_RBUTTONUP, lParam);
		return 1;
	case WM_RBUTTONDBLCLK:
		Game_BtnDoubleClick_(8, EventNo::EVN_RBUTTONDBLCLK, lParam);
		return 1;
	case WM_MBUTTONDOWN:
		BWFXN_Game_ButtonDown_(32, EventNo::EVN_MBUTTONDOWN, lParam);
		return 1;
	case WM_MBUTTONUP:
		BWFXN_Game_ButtonUp_(32, EventNo::EVN_MBUTTONUP, lParam);
		return 1;
	case WM_MBUTTONDBLCLK:
		Game_BtnDoubleClick_(32, EventNo::EVN_MBUTTONDBLCLK, lParam);
		return 1;
	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam) >= WHEEL_DELTA)
		{
			Game_MouseWheel_(EventNo::EVN_WHEELUP, lParam);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) <= -WHEEL_DELTA)
		{
			Game_MouseWheel_(EventNo::EVN_WHEELDWN, lParam);
		}
		return 1;
	case WM_CAPTURECHANGED:
		Game_Capturechanged_();
		break;
	case WM_IME_NOTIFY:
		GetUserDefaultLangID();
		return DefWindowProcA(hWnd, WM_IME_NOTIFY, wParam, lParam);
	case WM_QUERYNEWPALETTE:
		dword_6D5E1C = 1;
		return 1;
	case WM_PALETTECHANGED:
		if ((HWND)wParam != hWnd)
		{
			dword_6D5E1C = 1;
		}
		break;
	}
	return DefWindowProcA(hWnd, Msg, wParam, lParam);
}

FAIL_STUB_PATCH(MainWindowProc);

void CreateMainWindow_()
{
	WNDCLASSEXA window_class;

	memset(&window_class, 0, sizeof(window_class));
	window_class.cbSize = sizeof(window_class);
	window_class.style = 8;
	window_class.lpfnWndProc = MainWindowProc_;
	window_class.hInstance = hInst;
	window_class.hIcon = LoadIconA(hInst, (LPCSTR)0x66);
	window_class.hIconSm = (HICON)LoadImageA(hInst, (LPCSTR)0x66, 1u, 16, 16, 0x8000u);
	window_class.hCursor = LoadCursorA(0, (LPCSTR)0x7F00);
	window_class.hbrBackground = (HBRUSH)GetStockObject(5);
	window_class.lpszClassName = "SWarClass";

	if (!RegisterClassExA(&window_class))
	{
		FatalError("RegisterClass");
	}

	const char* window_name = is_expansion_installed ? "Brood War" : "Starcraft";
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	hWndParent = CreateWindowExA(0, "SWarClass", window_name, 0x90080000, 0, 0, screen_width, screen_height, 0, 0, hInst, 0);
	if (!hWndParent)
	{
		FatalError("CreateWindowEx");
	}

	UpdateWindow(hWndParent);
	SetFocus(hWndParent);
	SetCursor(0);
	if (GetUserDefaultLangID() == 1042)
	{
		if (!dword_6D6438)
		{
			dword_6D6438 = ImmGetContext(hWndParent);
		}
		dword_6D6438 = ImmAssociateContext(hWndParent, 0);
	}
}

FAIL_STUB_PATCH(CreateMainWindow);
FAIL_STUB_PATCH(PlayMovieWithIntro);
FAIL_STUB_PATCH(playActiveCinematic);

int sub_4DBD20_(const char* a1, size_t a2, int* a3)
{
	if (*a3 >= _countof(MapdataFilenames_))
	{
		return 0;
	}
	while (1)
	{
		char* v3 = SStrChrR(MapdataFilenames_[*a3], '\\');
		if (v3 && !SStrCmpI(v3 + 1, a1, a2))
		{
			return 1;
		}
		*a3 += 1;
		if (*a3 >= _countof(MapdataFilenames_))
		{
			break;
		}
	}
	return 0;
}

FAIL_STUB_PATCH(sub_4DBD20);

ExpandedCampaignMenuEntry* sub_4DBDA0_(const char* a1)
{
	ExpandedMapData v6;

	char* v2 = SStrChrR(a1, '.');
	if (!v2)
	{
		return (ExpandedCampaignMenuEntry*) active_campaign_menu_entry;
	}

	size_t v4 = v2 - a1;
	auto& campaigns = SStrCmpI(v2, ".SCM", 0x7FFFFFFFu) ? expcampaigns_by_race : campaigns_by_race;
	int v8 = 0;
	if (sub_4DBD20_(a1, v4, &v8))
	{
		while (2)
		{
			for (int i = 0; i < 3; ++i)
			{
				ExpandedCampaignMenuEntry* result = campaigns[i]->entries;
				v6 = result->next_mission;
				if (v6)
				{
					while (result->cinematic || v6 != v8)
					{
						v6 = result[1].next_mission;
						++result;
						if (v6 == MD_none)
						{
							goto LABEL_11;
						}
					}
					return result;
				}
			LABEL_11:
				;
			}
			++v8;
			if (sub_4DBD20_(a1, v4, &v8))
			{
				continue;
			}
			break;
		}
	}
	return (ExpandedCampaignMenuEntry*) active_campaign_menu_entry;
}

FAIL_STUB_PATCH(sub_4DBDA0);

struct EstablishingShotPositionEx
{
	int index;
	const char* tag;
	PrintFlags alignment;
};

DEFINE_ENUM_FLAG_OPERATORS(PrintFlags);

EstablishingShotPositionEx establishingShotPositions_[] = {
	{1, "</SCREENLEFT>", PF_VALIGN_CENTER | PF_HALIGN_LEFT},
	{2, "</SCREENRIGHT>", PF_HALIGN_RIGHT | PF_VALIGN_CENTER},
	{3, "</SCREENTOP>", PF_HALIGN_CENTER | PF_VALIGN_CENTER},
	{4, "</SCREENBOTTOM>", PF_HALIGN_CENTER | PF_VALIGN_CENTER},
	{5, "</SCREENCENTER>", PF_HALIGN_CENTER | PF_VALIGN_CENTER},
	{6, "</SCREENLOWERLEFT>", PF_HALIGN_LEFT | PF_VALIGN_BOTTOM},
	{7, "</SCREENUPPERLEFT>", PF_HALIGN_LEFT | PF_VALIGN_TOP},
	{8, "</SCREENUPPERRIGHT>", PF_HALIGN_RIGHT | PF_VALIGN_TOP},
	{9, "</SCREENLOWERRIGHT>", PF_HALIGN_RIGHT | PF_VALIGN_BOTTOM},
};

void creditsEndPage_(dialog* a1)
{
	if (byte_51CEAC)
	{
		byte_51CEAC = 0;
		sub_41E9E0(byte_51CEC8);
		memcpy(stru_6CEB40, palette, sizeof(stru_6CEB40));
		if ((a1->lFlags & DialogFlags::CTRL_UPDATE) == 0)
		{
			a1->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog(a1);
		}
		TitlePaletteUpdate_(byte_51CEC8);
	}
}

FAIL_STUB_PATCH(creditsEndPage);

void sub_4D8840_(int element_length, char* element_start)
{
	if (dword_51CEB0)
	{
		if (dword_51CEB0->pszText)
		{
			SMemFree(dword_51CEB0->pszText, "Starcraft\\SWAR\\lang\\credits.cpp", 321, 0);
			dword_51CEB0->pszText = 0;
			HideDialog(dword_51CEB0);
		}
		if (element_length)
		{
			dword_51CEB0->pszText = (char*)SMemAlloc(element_length + 1, "Starcraft\\SWAR\\lang\\credits.cpp", 327, 0);
			SStrCopy(dword_51CEB0->pszText, element_start, element_length + 1);
			if ((dword_51CEB0->lFlags & CTRL_UPDATE) == 0)
			{
				dword_51CEB0->lFlags |= CTRL_UPDATE;
				updateDialog(dword_51CEB0);
			}
			showDialog(dword_51CEB0);
		}
	}
}

FAIL_STUB_PATCH(sub_4D8840);

int runCreditsScriptCommands_(char* tag, unsigned int tag_length, dialog* dlg)
{
	if (tag_length > 0xE && !_strnicmp(tag, "</BACKGROUND ", 13u))
	{
		creditsSetBackgroundImageFromFile(tag + 13, dlg);
		return 0;
	}
	if (!_strnicmp(tag, "</PLAYSOUND ", 12u))
	{
		char* attribute_begin = tag + 12;
		char* attribute_end = strchr(attribute_begin, '>');
		std::string sound_attribute(attribute_begin, attribute_end);

		PlayWavByFilename_maybe(sound_attribute.c_str());

		return 0;
	}
	if (!_strnicmp(tag, "</FONTSIZE ", 11u))
	{
		char* attribute_begin = tag + 11;
		char* attribute_end = strchr(attribute_begin, '>');
		std::string font_size_attribute(attribute_begin, attribute_end);

		DialogFlags font_size;
		if (font_size_attribute == "10")
		{
			font_size = DialogFlags::CTRL_FONT_SMALLEST;
		}
		else if (font_size_attribute == "14")
		{
			font_size = DialogFlags::CTRL_FONT_SMALL;
		}
		else if (font_size_attribute == "16")
		{
			font_size = DialogFlags::CTRL_FONT_LARGE;
		}
		else // font_size_attribute == "16x"
		{
			font_size = DialogFlags::CTRL_FONT_LARGEST;
		}

		for (int i = 0; i < _countof(establishingShotPositions_); i++)
		{
			auto label = getControlFromIndex(dlg, i + 1);
			label->lFlags &= ~DialogFlags::CTRL_FONT_LARGEST;
			label->lFlags &= ~DialogFlags::CTRL_FONT_LARGE;
			label->lFlags &= ~DialogFlags::CTRL_FONT_SMALL;
			label->lFlags &= ~DialogFlags::CTRL_FONT_SMALLEST;
			label->lFlags |= font_size;
		}
		return 0;
	}
	if (tag_length > 0xD && !_strnicmp(tag, "</FONTCOLOR ", 12u))
	{
		creditsSetFontColorFromFile(tag + 12);
		return 0;
	}
	if (tag_length > 0xD && !_strnicmp(tag, "</FADESPEED ", 12u))
	{
		creditsSetFadeSpeed(tag + 12);
		return 0;
	}
	if (tag_length > 0xF && !_strnicmp(tag, "</DISPLAYTIME ", 14u))
	{
		creditsSetDisplayTime(tag + 14);
		return 0;
	}
	else if (!_strnicmp(tag, "</PAGE>", tag_length))
	{
		creditsEndPage_(dlg);
		return 1;
	}
	else
	{
		for (auto& position: establishingShotPositions_)
		{
			if (!_strnicmp(tag, position.tag, tag_length))
			{
				if (dword_51CEB0)
				{
					HideDialog(dword_51CEB0);
				}
				dword_51CEB0 = getControlFromIndex(dlg, position.index);
				break;
			}
		}
		return 0;
	}
}

FAIL_STUB_PATCH(runCreditsScriptCommands);

int runCredits_(dialog* a1)
{
	if (dword_51CEB8)
	{
		while (true)
		{
			int is_tag;
			char* element_start;

			int element_length = sub_4D86A0(&element_start, &is_tag);
			if (!element_length)
			{
				break;
			}
			if (!is_tag)
			{
				sub_4D8840_(element_length, element_start);
			}
			else if (runCreditsScriptCommands_(element_start, element_length, a1))
			{
				dword_51CEC4 = dword_51CEB4 + GetTickCount();
				return 1;
			}
		}
	}

	DestroyDialog(a1);
	return 0;
}

FAIL_STUB_PATCH(runCredits);

int credits_keyDwn_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wVirtKey)
	{
	case VK_SPACE:
	case VK_RETURN:
		return runCredits_(dlg);
	case VK_ESCAPE:
		credits_interrupted = 1;
		DestroyDialog(dlg);
		return 0;
	default:
		return 1;
	}
}

FAIL_STUB_PATCH(credits_keyDwn);

int credits_idle_(dialog* dlg)
{
	if (gwGameMode == GAME_EXIT)
	{
		credits_interrupted = 1;
		DestroyDialog(dlg);
		return 0;
	}
	else if (is_keycode_used[VK_BACK] || GetTickCount() <= dword_51CEC4)
	{
		return 1;
	}
	else
	{
		return runCredits_(dlg);
	}
}

FAIL_STUB_PATCH(credits_idle);

void __fastcall sub_4D8930_(dialog* dlg, int x, int y, rect* dst)
{
	if (dlg->wIndex - 1 < _countof(establishingShotPositions_))
	{
		PrintXY_flags = establishingShotPositions_[dlg->wIndex - 1].alignment;
	}
	else
	{
		PrintXY_flags = PF_VALIGN_TOP | PF_HALIGN_LEFT;
	}
	dlg->lFlags |= DialogFlags::CTRL_LBOX_NORECALC;
	DlgDrawText(0, dlg, 0, 0);
}

FAIL_STUB_PATCH(sub_4D8930);

int creditsDlgInit_(dialog* dlg)
{
	for (auto& position : establishingShotPositions_)
	{
		dialog* position_dlg = getControlFromIndex(dlg, position.index);
		if (position_dlg)
		{
			position_dlg->pszText = 0;
			position_dlg->pfcnUpdate = sub_4D8930_;
		}
	}

	byte_51CEC8 = 1;
	dword_51CEB4 = 5000;
	return runCredits_(dlg);
}

FAIL_STUB_PATCH(creditsDlgInit);

void creditsDlgDestroy_(dialog* dlg)
{
	for (auto& position : establishingShotPositions_)
	{
		dialog* position_dlg = getControlFromIndex(dlg, position.index);
		if (position_dlg->pszText)
		{
			SMemFree(position_dlg->pszText, "Starcraft\\SWAR\\lang\\credits.cpp", 566, 0);
		}
	}

	if (!byte_51A0E9)
	{
		memset(stru_6CEB40, 0, sizeof(stru_6CEB40));
		byte_51A0E9 = 1;
		memcpy(stru_6CE720, GamePalette, sizeof(stru_6CE720));
		gluDlgFadePalette(byte_51CEC8);
		BWFXN_RedrawTarget_();
	}
}

FAIL_STUB_PATCH(creditsDlgDestroy);

bool __fastcall creditsDlgInteract_(dialog* dlg, struct dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
		if (!credits_keyDwn_(dlg, evt))
		{
			return false;
		}
		break;
	case EVN_LBUTTONDOWN:
	case EVN_RBUTTONDOWN:
		if (!runCredits_(dlg))
		{
			return false;
		}
		break;
	case EVN_IDLE:
		if (!credits_idle_(dlg))
		{
			return false;
		}
		break;
	case EVN_USER:
		if (evt->dwUser == 0)
		{
			if (!creditsDlgInit_(dlg))
			{
				return false;
			}
			break;
		}
		else if (evt->dwUser == USER_DESTROY)
		{
			creditsDlgDestroy_(dlg);
		}
		break;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(creditsDlgInteract);

void loadInitCreditsBIN_(const char* a1)
{
	char buff[MAX_PATH];
	_snprintf(buff, MAX_PATH, "rez\\%s.txt", a1);

	dword_51CEA8 = (char*)fastFileRead_(&bytes_read, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	dword_51CEBC = dword_51CEA8;
	dword_51CEB8 = bytes_read;
	credits_interrupted = 0;

	dialog* credits_bin = LoadDialog("rez\\credits.bin");
	gluLoadBINDlg_(credits_bin, creditsDlgInteract_);
	if (dword_51CEA8)
		SMemFree(dword_51CEA8, "Starcraft\\SWAR\\lang\\credits.cpp", 623, 0);
	dword_51CEA8 = 0;
	dword_51CEBC = 0;
	dword_51CEB0 = 0;
}

FAIL_STUB_PATCH(loadInitCreditsBIN);

void DisplayMissionEpilog_()
{
	if (!multiPlayerMode && (GameCheats & CHEAT_NoGlues) == 0 && active_campaign_menu_entry)
	{
		const char* epilog = ((ExpandedCampaignMenuEntry*)active_campaign_menu_entry)->epilog;
		if (epilog)
		{
			loadInitCreditsBIN_(epilog);
		}
	}
}

FAIL_STUB_PATCH(DisplayMissionEpilog);
FAIL_STUB_PATCH(sub_4D8F90);

int CreateNextCampaignGame_()
{
	if (!next_scenario[0])
	{
		glGluesMode = IsExpansion != 0 ? GLUE_EX_CAMPAIGN : GLUE_CAMPAIGN;
		return 1;
	}
	GotFileValues* v1 = InitUseMapSettingsTemplate_();
	if (v1)
	{
		int v2 = v1->template_id | ((v1->unused1 | (v1->variation_id << 8)) << 8);
		SMemFree(v1, "Starcraft\\SWAR\\lang\\uiSingle.cpp", 319, 0);

		char dest[260];
		SStrCopy(dest, CurrentMapFileName, 0x104u);
		char* v3 = strrchr(dest, '\\');

		if (v3)
		{
			v3[1] = 0;
			SStrNCat(dest, next_scenario, 260);

			if (!LoadScenarioSingle(dest, v2, playerName, registry_options.GameSpeed))
			{
				switch (Players[g_LocalNationID].nRace)
				{
				case RACE_Zerg:
					glGluesMode = GLUE_READY_Z;
					return 1;
				case RACE_Terran:
					glGluesMode = GLUE_READY_T;
					return 1;
				case RACE_Protoss:
					glGluesMode = GLUE_READY_P;
					return 1;
				}
			}
		}
	}
	if (!outOfGame)
	{
		doNetTBLError(0, 0, 0, 106);
	}
	glGluesMode = IsExpansion != 0 ? GLUE_EX_CAMPAIGN : GLUE_CAMPAIGN;
	return 0;
}

FAIL_STUB_PATCH(CreateNextCampaignGame);

int ContinueCampaign_(int a1)
{
	gwGameMode = GAME_GLUES;
	if (dword_51CA1C)
	{
		CreateNextCampaignGame_();
		return 1;
	}
	if (!a1)
	{
		gwGameMode = GAME_RESTART;
		return 1;
	}
	DisplayMissionEpilog_();
	updateActiveCampaignMission_();
	if (!active_campaign_menu_entry || active_campaign_menu_entry->next_mission == MD_none)
	{
		return 0;
	}
	if (next_scenario[0])
	{
		active_campaign_menu_entry = (CampaignMenuEntry*) sub_4DBDA0_(next_scenario);
		next_scenario[0] = 0;
	}
	else
	{
		active_campaign_menu_entry = (CampaignMenuEntry*)((ExpandedCampaignMenuEntry*)active_campaign_menu_entry + 1);
	}
	sub_4DBEE0_((ExpandedCampaignMenuEntry*) active_campaign_menu_entry);
	if (active_campaign_menu_entry->next_mission)
	{
		if (active_campaign_menu_entry->cinematic)
		{
			CampaignIndex = active_campaign_menu_entry->next_mission;
			active_cinematic = active_campaign_menu_entry->cinematic;
			gwGameMode = GAME_CINEMATIC;
			return 1;
		}
		if (CreateCampaignGame_(active_campaign_menu_entry->next_mission))
		{
			switch (active_campaign_menu_entry->race)
			{
			case Race::RACE_Zerg:
				glGluesMode = MenuPosition::GLUE_READY_Z;
				break;
			case Race::RACE_Terran:
				glGluesMode = MenuPosition::GLUE_READY_T;
				break;
			case Race::RACE_Protoss:
				glGluesMode = MenuPosition::GLUE_READY_P;
				break;
			}
			return 1;
		}
		return 0;
	}

	gwGameMode = GAME_EPILOG;
	return 1;
}

FAIL_STUB_PATCH(ContinueCampaign);

Campaign* GetActiveCampaign()
{
	for (Campaign& campaign : campaigns)
	{
		ExpandedCampaignMenuEntry* last_campaign_menu_entry;
		for (last_campaign_menu_entry = campaign.entries; last_campaign_menu_entry->next_mission; last_campaign_menu_entry++);
		if (last_campaign_menu_entry == (ExpandedCampaignMenuEntry*) active_campaign_menu_entry)
		{
			return &campaign;
		}
	}
	return NULL;
}

void BeginEpilog_()
{
	int v0 = registry_options.Music;
	if (!registry_options.Music && registry_options.Sfx)
	{
		if (directsound)
		{
			SFileDdaEnd(directsound);
			SFileCloseFile(directsound);
			directsound = 0;
		}
		byte_6D5BBC = 0;
		byte_6D5BBD = 0;
		bigvolume = 0;
		registry_options.Music = 50;
	}

	Campaign* active_campaign = GetActiveCampaign();

	if (active_campaign)
	{
		DLGMusicFade_(active_campaign->epilog_music_track);
		std::for_each(active_campaign->epilogs.begin(), active_campaign->epilogs.end(), loadInitCreditsBIN_);
		glGluesMode = active_campaign->post_epilog_menu;
	}
	else
	{
		glGluesMode = MenuPosition::GLUE_MAIN_MENU;
	}
	gwGameMode = GAME_GLUES;
	active_campaign_menu_entry = NULL;

	stopMusic();
	registry_options.Music = v0;
}

FAIL_STUB_PATCH(BeginEpilog);

void BeginCredits_()
{
	int v0 = registry_options.Music;
	if (!registry_options.Music && registry_options.Sfx)
	{
		if (directsound)
		{
			SFileDdaEnd(directsound);
			SFileCloseFile(directsound);
			directsound = 0;
		}
		byte_6D5BBC = 0;
		byte_6D5BBD = 0;
		bigvolume = 0;
		registry_options.Music = 50;
	}

	DLGMusicFade_(MT_TERRAN2);
	credits_interrupted = 0;
	loadInitCreditsBIN_("crdt_mag");
	if (credits_interrupted == 0 && is_expansion_installed)
	{
		loadInitCreditsBIN_("crdt_exp");
	}
	if (credits_interrupted == 0)
	{
		loadInitCreditsBIN_("crdt_lst");
	}
	stopMusic();
	registry_options.Music = v0;
}

FAIL_STUB_PATCH(BeginCredits);
FAIL_STUB_PATCH(sub_4A60D0);
FAIL_STUB_PATCH(sub_48EB90);

void PlayMovie_(Cinematic cinematic)
{
	RefreshCursor_0();
	InitializeInputProcs();
	input_procedures[EventNo::EVN_CHAR] = endVideoProc;
	input_procedures[EventNo::EVN_LBUTTONUP] = endVideoProc;
	input_procedures[EventNo::EVN_RBUTTONUP] = endVideoProc;
	dword_5967F0 = 0;

	HANDLE video;
	SVidPlayBegin(cinematics[cinematic], 0, 0, 0, 0, cinematic < 0x19 ? 0x10280808 : 0x10A80808, &video);
	if (video)
	{
		while (!dword_5967F0)
		{
			if (dword_51BFA8 && !SVidPlayContinueSingle(video, 0, 0))
			{
				break;
			}
			BWFXN_videoLoop_(3);
			Sleep(0);
		}
		SVidPlayEnd(video);
	}
	else if (cinematic != 1 && cinematic != 24)
	{
		SysWarn_FileNotFound(cinematics[cinematic], 2);
	}
}

FAIL_STUB_PATCH(PlayMovie);

void GameMainLoop_()
{
	HANDLE phFile;

	gwGameMode = GAME_GLUES;
	PreInitData_();
	InitializeInputProcs();
	CreateMainWindow_();
	audioVideoInit_();
	AppAddExit_(SaveCPUThrottleOption);
	if (SRegLoadValue("Starcraft", "CPUThrottle", 0, (int*)&phFile))
		CpuThrottle = phFile != 0;
	SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	Mouse.x = SCREEN_WIDTH / 2;
	Mouse.y = SCREEN_HEIGHT / 2;
	if (cd_archive_mpq && SFileOpenFileEx(cd_archive_mpq, "rez\\gluexpcmpgn.bin", 0, &phFile))
	{
		SFileCloseFile(phFile);
		if ((registry_options.field_18 & 0x800) == 0)
		{
		LABEL_8:
			LoadTitle_();
			LoadInitIscriptBIN_();
			AppAddExit_(CleanupIscriptBINHandle_);
			if (gwGameMode != GAME_GLUES && load_screen)
			{
				DestroyDialog(load_screen);
				load_screen = NULL;
			}
			while (1)
			{
				setCursorType_(CursorType::CUR_TIME);
				BWFXN_RedrawTarget_();
				if (!is_expansion_installed)
					IsExpansion = 0;
				switch (gwGameMode)
				{
				case GAME_RUN:
					GameRun_(GLUE_MAIN_MENU);
					continue;
				case GAME_CINEMATIC:
					PlayMovie_(active_cinematic);
					active_cinematic = Cinematic::C_NONE;
					if (gwGameMode == GAME_CINEMATIC)
						ContinueCampaign_(1);
					continue;
				case GAME_RESTART:
					next_campaign_mission = 1;
					[[fallthrough]];
				case GAME_RUNINIT:
					gwGameMode = GAME_RUN;
					continue;
				case GAME_INTRO:
					PlayMovie_(Cinematic::C_BLIZZARD_LOGO);
					if (gwGameMode == GAME_INTRO)
					{
						if (cd_archive_mpq && SFileOpenFileEx(cd_archive_mpq, "rez\\gluexpcmpgn.bin", GLUE_MAIN_MENU, &phFile))
						{
							SFileCloseFile(phFile);
							PlayMovie_(Cinematic::C_EXPANSION_INTRO);
						}
						else
						{
							PlayMovie_(Cinematic::C_INTRO);
						}
						if (gwGameMode == GAME_INTRO)
							gwGameMode = GAME_GLUES;
					}
					continue;
				case GAME_GLUES:
				case GAME_WIN:
				case GAME_LOSE:
					SwitchMenu_();
					continue;
				case GAME_CREDITS:
					BeginCredits_();
					if (gwGameMode == GAME_CREDITS)
					{
						gwGameMode = GAME_GLUES;
						glGluesMode = GLUE_MAIN_MENU;
					}
					break;
				case GAME_EPILOG:
					BeginEpilog_();
					break;
				default:
					AppExit_(0);
					ProcError(1);
					exit(0);
					return;
				}
			}
		}
		registry_options.field_18 &= ~0x800u;
		PlayMovie_(Cinematic::C_BLIZZARD_LOGO);
		PlayMovie_(Cinematic::C_EXPANSION_INTRO);
	}
	else
	{
		if ((registry_options.field_18 & 0x200) == 0)
			goto LABEL_8;
		registry_options.field_18 &= ~0x200u;
		PlayMovie_(Cinematic::C_BLIZZARD_LOGO);
		PlayMovie_(Cinematic::C_INTRO);
	}
	goto LABEL_8;
}

FAIL_STUB_PATCH(sub_4DA790);
FAIL_STUB_PATCH(sub_481CF0);
FAIL_STUB_PATCH(GameMainLoop);

unsigned int LocalGetLang_()
{
	CHAR Buffer[16];
	char *v2;

	if (local_dll_library && LoadStringA(local_dll_library, 3u, Buffer, 16) || LoadStringA(hInst, 3u, Buffer, 16))
		return strtoul(Buffer, &v2, 16);
	else
		return 1033;
}

FUNCTION_PATCH(LocalGetLang, LocalGetLang_);

void __fastcall FreeLocalDLL_(bool exit_code)
{
	if (local_dll_library)
	{
		FreeLibrary(local_dll_library);
		local_dll_library = 0;
	}
}

FAIL_STUB_PATCH(FreeLocalDLL);

void localDll_Init_(HINSTANCE a1)
{
	char Filename[MAX_PATH];

	if (!GetModuleFileNameA(a1, Filename, MAX_PATH))
		Filename[0] = 0;
	char* v1 = strrchr(Filename, '\\');
	if (!v1)
		v1 = &Filename[-1];
	const char* v2 = "local.dll";
	int i = 0;
	while (true)
	{
		v1[i + 1] = v2[i];
		if (v2[i] == '\0')
			break;
		++i;
	};
	local_dll_library = LoadLibraryA(Filename);
	if (!local_dll_library)
	{
		DialogBoxParamA(a1, (LPCSTR)106, hWndParent, LocalErrProc, (LPARAM)Filename);
		SErrSuppressErrors(1);
		AppExit_(1);
		ProcError(1);
		exit(1);
	}
	int local_lang = LocalGetLang_();
	SFileSetLocale(local_lang);
	AppAddExit_(FreeLocalDLL_);
}

FAIL_STUB_PATCH(localDll_Init);

void DisplayEstablishingShot_()
{
	if (!multiPlayerMode && !(GameCheats & CHEAT_NoGlues) && active_campaign_menu_entry)
	{
		const char* establishing_shot = ((ExpandedCampaignMenuEntry*)active_campaign_menu_entry)->establishing_shot;
		if (establishing_shot)
		{
			loadInitCreditsBIN_(establishing_shot);
		}
	}
	else if (CampaignIndex == MapData::MD_none && CurrentMapFileName)
	{
		SFileOpenArchive(CurrentMapFileName, 0, 0, &mapArchiveHandle);
		HANDLE handle;
		bool establishingShotExists = SFileOpenFileEx(mapArchiveHandle, "rez\\est.txt", 0, &handle);
		if (handle)
			SFileCloseFile(handle);

		if (establishingShotExists)
			loadInitCreditsBIN_("est");

		if (mapArchiveHandle)
			SFileCloseArchive(mapArchiveHandle);
	}
}

FAIL_STUB_PATCH(DisplayEstablishingShot);

const char* __stdcall get_Tileset_String(Tileset tileset)
{
	static TblFile tbl_file("rez\\tilesets.tbl");

	return tbl_file[tileset];
}

NOP_PATCH((void*)0x4A7960, 3);
CALL_SITE_PATCH((void*)0x4A7964, get_Tileset_String);

signed int __fastcall packColorShifts_(int a1, void* a2)
{
	for (int i = 0; i < 8; i++)
	{
		if (colorShift[i].data == a2)
			return i;
	}
	return 0;
}

FUNCTION_PATCH(packColorShifts, packColorShifts_);

int __fastcall TriggerAction_NoAction_(Action* a1)
{
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_NoAction);

int __fastcall TriggerAction_Victory_(Action* a1)
{
	if (Players[active_trigger_player].nType == PT_Human || Players[active_trigger_player].nType == PT_Computer)
	{
		endgame_state[active_trigger_player] = VICTORY;
	}
	word_650970 = byte_6509B4 ? 1 : 45;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_Victory);

int __fastcall TriggerAction_Defeat_(Action* a1)
{
	if (endgame_state[active_trigger_player] == EndgameState::VICTORY || endgame_state[active_trigger_player] == EndgameState::UNKNOWN)
	{
		return 0;
	}
	if (Players[active_trigger_player].nType == PT_Human || Players[active_trigger_player].nType == PT_Computer)
	{
		endgame_state[active_trigger_player] = EndgameState::DEFEAT;
	}
	word_650970 = byte_6509B4 ? 1 : 45;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_Defeat);

int __fastcall TriggerAction_PreserveTrigger_(Action* a1)
{
	dword_6509AC->container.dwExecutionFlags |= 4;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_PreserveTrigger);

int __fastcall TriggerAction_Wait_(Action* a1)
{
	if (byte_6509B8[active_trigger_player])
	{
		return 0;
	}
	if (a1->flags & 1)
	{
		a1->flags = a1->flags & 0xFE;
		return 1;
	}
	if ((dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		dword_650980[active_trigger_player] = a1->time;
		byte_6509B8[active_trigger_player] = 1;
		a1->flags |= 1;
		return 0;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_Wait);

unsigned int getTextDisplayTime_(const char* text)
{
	if (text == NULL)
	{
		return 0;
	}

	return max(strlen(text) * 50, 4000);
}

FAIL_STUB_PATCH(getTextDisplayTime);

int __fastcall TriggerAction_PlayWav_(Action* a1)
{
	char buff[260];

	if (!InReplay && active_trigger_player == g_LocalNationID && a1->wavString && (dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		const char* chk_string = get_chk_String(a1->wavString);
		if (CampaignIndex == MD_none)
		{
			SStrCopy(buff, chk_string, 260u);
		}
		else
		{
			_snprintf(buff, 260u, "%s\\%s", MapdataFilenames_[CampaignIndex], chk_string);
		}
		PlayWavByFilename_maybe(buff);
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_PlayWav);

int __fastcall TriggerAction_Transmission_(Action* a1)
{
	if (a1->location == 0)
	{
		return 1;
	}

	PlayerType player_type = Players[active_trigger_player].nType;
	if (player_type == PlayerType::PT_Computer || player_type == PlayerType::PT_Rescuable || player_type == PlayerType::PT_Unknown0 || player_type == PlayerType::PT_Neutral)
	{
		return 1;
	}
	if (!byte_6509B8[active_trigger_player])
	{
		if (a1->flags & 1)
		{
			a1->flags = a1->flags & 0xFE;
			return 1;
		}
		if (dword_6509AC->container.dwExecutionFlags & 0x10)
		{
			return 1;
		}

		unsigned int v7;
		switch (a1->number2)
		{
		case 7:
			v7 = a1->number;
			break;
		case 8:
			v7 = a1->time + a1->number;
			break;
		case 9:
			v7 = max(a1->time - a1->number, 0);
			break;
		default:
			v7 = a1->time;
		}

		a1->flags = a1->flags | 1;
		dword_650980[active_trigger_player] = multiPlayerMode ? v7 : -1;
		byte_6509B8[active_trigger_player] = 1;
		if (active_trigger_player == g_LocalNationID)
		{
			TriggerAction_PlayWav_(a1);
			CUnit* unit = getUnitForDoodadState(a1->location - 1, a1->unit, 17);
			if (unit)
			{
				unit->sprite->selectionTimer = 45;
				MinimapPing_maybe(unit->sprite->position.x, unit->sprite->position.y, 17);
				DisplayTalkingPortrait_maybe(unit->sprite->position.x, v7, a1->unit, unit->sprite->position.y);
			}
			else
			{
				DisplayTalkingPortrait_maybe(-1, v7, a1->unit, -1);
			}
			if ((registry_options.field_18 & 0x400) != 0 || (a1->flags & 4) != 0)
			{
				const char* text_message = get_chk_String(a1->string);
				unsigned display_time = max(v7, getTextDisplayTime_(text_message));
				createTextMessageWithTimer(text_message, display_time);
			}
		}
	}
	return 0;
}

FAIL_STUB_PATCH(TriggerAction_Transmission);

int __fastcall TriggerAction_SetMissionObjectives_(Action* a1)
{
	MissionObjectives[active_trigger_player] = a1->string;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetMissionObjectives);

int __fastcall TriggerAction_DisplayTextMessage_(Action* a1)
{
	const char* text_message;

	if (active_trigger_player == g_LocalNationID && ((registry_options.field_18 & 0x400) || (a1->flags & 4)) && (dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		text_message = GetMapTblString(a1->string);

		if (text_message)
		{
			int display_time = getTextDisplayTime_(text_message);
			PrintText(text_message, 2u, display_time + GetTickCount(), 1);
		}
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_DisplayTextMessage);

int __fastcall TriggerAction_CenterView_(Action* a1)
{
	if (a1->location == 0)
	{
		return 1;
	}
	if (IS_GAME_PAUSED)
	{
		return 1;
	}

	PlayerType player_type = Players[active_trigger_player].nType;
	if (player_type == PT_Computer || player_type == PT_Rescuable || player_type == PT_Unknown0 || player_type == PT_Neutral)
	{
		return 1;
	}
	if (multiPlayerMode)
	{
		if (active_trigger_player == g_LocalNationID)
		{
			BWFXN_MoveScreen(
				(LocationTable[a1->location - 1].dimensions.left + LocationTable[a1->location - 1].dimensions.right - GAME_AREA_WIDTH) / 2,
				(LocationTable[a1->location - 1].dimensions.top + LocationTable[a1->location - 1].dimensions.bottom - GAME_AREA_HEIGHT) / 2);
		}
		return 1;
	}
	if (!byte_6509B8[active_trigger_player])
	{
		if (a1->flags & 1)
		{
			a1->flags = a1->flags & 0xFE;
			CenterCursorGameScreen();
			return 1;
		}
		if (dword_6509AC->container.dwExecutionFlags & 0x10)
		{
			return 1;
		}
		if (active_trigger_player == g_LocalNationID)
		{
			Box32* dimensions = &LocationTable[a1->location - 1].dimensions;

			int v11 = std::clamp((dimensions->left + dimensions->right - GAME_AREA_WIDTH) / 2, 0, map_width_pixels - GAME_AREA_WIDTH - 1);
			int v12 = std::clamp((dimensions->top + dimensions->bottom - GAME_AREA_HEIGHT) / 2, 0, map_height_pixels - GAME_AREA_HEIGHT - 1);

			assignCenterViewProc(v12, v11, defCenterViewProc);
		}
		a1->flags |= 1;
		byte_6509B8[active_trigger_player] = 1;
		dword_650980[active_trigger_player] = -1;
	}
	return 0;
}

FAIL_STUB_PATCH(TriggerAction_CenterView);

void EnableSwitch(unsigned index)
{
	TriggerSwitches[index / 32] |= 1 << (index % 32);
}

void DisableSwitch(unsigned index)
{
	TriggerSwitches[index / 32] &= ~(1 << (index % 32));
}

void ToggleSwitch(unsigned index)
{
	TriggerSwitches[index / 32] ^= 1 << (index % 32);
}

int __fastcall TriggerAction_SetSwitch_(Action* a1)
{
	unsigned int v5;

	if (a1->number >= 256)
	{
		return 1;
	}
	switch (a1->number2)
	{
	case 0xBu:
		if (IsInGameLoop)
		{
			++dword_51C6F4;
			++randomCountsTotal;
			LastRandomNumber = 22695477 * LastRandomNumber + 1;
			v5 = (unsigned)(LastRandomNumber) >> 16;
		}
		else
		{
			v5 = 0;
		}
		if ((v5 & 0x80u) == 0)
		{
			DisableSwitch(a1->number);
		}
		else
		{
			EnableSwitch(a1->number);
		}
		return 1;
	case 4u:
		EnableSwitch(a1->number);
		return 1;
	case 5u:
		DisableSwitch(a1->number);
		return 1;
	case 6u:
		ToggleSwitch(a1->number);
		return 1;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetSwitch);

int __fastcall TriggerAction_SetCountdownTimer_(Action* a1)
{
	switch (a1->number2)
	{
	case 7u:
		CountdownTimer = a1->time;
		break;
	case 8u:
		CountdownTimer = CountdownTimer + a1->time;
		break;
	case 9u:
		CountdownTimer = CountdownTimer - a1->time;
		break;
	}

	if (CountdownTimer < 0) CountdownTimer = 0;
	if (CountdownTimer > 359999) CountdownTimer = 359999;

	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetCountdownTimer);

int __fastcall TriggerAction_RunAiScript_(Action* a1)
{
	if (isAIScriptNameValid(active_trigger_player, a1->number))
	{
		int v5;
		int v3 = ParseAIScriptName(a1->number, &v5);
		if (v3)
		{
			AI_RunAIScript(0, active_trigger_player, v3, v5);
		}
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_RunAiScript);

int __fastcall TriggerAction_RunAiScriptAtLocation_(Action* a1)
{
	if (a1->location)
	{
		if (isAIScriptNameValid(active_trigger_player, a1->number))
		{
			int v5 = 0;
			int v3 = ParseAIScriptName(a1->number, &v5);
			if (v3)
			{
				AI_RunAIScript(&LocationTable[a1->location - 1], active_trigger_player, v3, v5);
			}
		}
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_RunAiScriptAtLocation);

int __fastcall TriggerAction_LeaderBoard_(Action* a1)
{
	word_65097C = 1;
	if (a1->action == 40)
	{
		byte_58D70C = a1->action;
		word_58D70E = 2;
		dword_58D710 = a1->number;
	}
	else if (a1->action != 18 && a1->action != 34)
	{
		byte_58D70C = a1->action;
		word_58D70E = a1->unit;
		dword_58D710 = a1->number;
		dword_58D714 = a1->string;
	}
	else if (a1->location)
	{
		byte_58D70D = a1->location - 1;
		byte_58D70C = a1->action;
		word_58D70E = a1->unit;
		dword_58D710 = a1->number;
		dword_58D714 = a1->string;
	}
	return 1;
}

FUNCTION_PATCH(TriggerAction_LeaderBoard, TriggerAction_LeaderBoard_);

int __fastcall TriggerAction_LeaderBoardComputerPlayers_(Action* a1)
{
	switch (a1->number2)
	{
	case 4:
		dword_58D708 = 1;
		break;
	case 5:
		dword_58D708 = 0;
		break;
	case 6:
		dword_58D708 ^= 1;
		break;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_LeaderBoardComputerPlayers);

int __fastcall TriggerAction_KillUnit_(Action* a1)
{
	dword_5971DC = 0;
	DestroyUnit_maybe(a1->player, a1->unit, 255, 999999);
	byte_685180 = 0;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_KillUnit);

int __fastcall TriggerAction_KillUnitAtLocation_(Action* a1)
{
	if (a1->location)
	{
		dword_5971DC = 0;
		DestroyUnit_maybe(a1->player, a1->unit, a1->location - 1, a1->number2 ? a1->number2 : 999999);
		byte_685180 = 0;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_KillUnitAtLocation);

int __fastcall TriggerAction_RemoveUnit_(Action* a1)
{
	dword_5971DC = 1;
	DestroyUnit_maybe(a1->player, a1->unit, 255, 999999);
	byte_685180 = 0;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_RemoveUnit);

int __fastcall TriggerAction_RemoveUnitAtLocation_(Action* a1)
{
	if (a1->location)
	{
		dword_5971DC = 1;
		DestroyUnit_maybe(a1->player, a1->unit, a1->location - 1, a1->number2 ? a1->number2 : 999999);
		byte_685180 = 0;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_RemoveUnitAtLocation);

int __fastcall TriggerAction_SetResources_(Action* a1)
{
	switch (a1->number2)
	{
	case 7u:
		SetResource(a1->player, a1->unit, a1->number);
		return 1;
	case 8u:
		AddResource(a1->player, a1->unit, a1->number);
		return 1;
	case 9u:
		SubtractResource(a1->player, a1->unit, a1->number);
		return 1;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetResources);

int __fastcall TriggerAction_SetScore_(Action* a1)
{
	switch (a1->number2)
	{
	case 7u:
		SetScore(a1->player, a1->unit, a1->number);
		return 1;
	case 8u:
		AddScore(a1->player, a1->unit, a1->number);
		return 1;
	case 9u:
		SubtractScore(a1->player, a1->unit, a1->number);
		return 1;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetScore);

int __fastcall TriggerAction_MinimapPing_(Action* a1)
{
	if (a1->location && active_trigger_player == g_LocalNationID && (dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		Box32& dimensions = LocationTable[a1->location - 1].dimensions;
		MinimapPing_maybe((dimensions.left + dimensions.right) / 2, (dimensions.top + dimensions.bottom) / 2, 17);
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_MinimapPing);

int __fastcall TriggerAction_TalkingPortrait_(Action* a1)
{
	if (active_trigger_player == g_LocalNationID && (dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		DisplayTalkingPortrait_maybe(-1, a1->time, a1->unit, -1);
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_TalkingPortrait);

int __fastcall TriggerAction_MuteUnitSpeech_(Action* a1)
{
	if (active_trigger_player == g_LocalNationID && (dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		MuteUnitSpeech_maybe();
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_MuteUnitSpeech);

int __fastcall TriggerAction_UnmuteUnitSpeech_(Action* a1)
{
	if (active_trigger_player == g_LocalNationID)
	{
		dword_64086C = 0;
		if (byte_6D5BBD)
		{
			byte_6D5BBD = 0;
			if (directsound)
			{
				SFileDdaSetVolume(directsound, bigvolume, 0);
			}
		}
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_UnmuteUnitSpeech);

int __fastcall TriggerAction_SetNextScenario_(Action* a1)
{
	if (a1->string)
	{
		SStrCopy(next_scenario, GetMapTblString(a1->string), sizeof(next_scenario));
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetNextScenario);

int __fastcall TriggerAction_SetDoodadState_(Action* a1)
{
	if (a1->location == 0)
	{
		return 1;
	}

	CUnit* unit = getUnitForDoodadState(a1->location - 1, a1->unit, a1->player);
	if (!unit)
	{
		return 1;
	}
	switch (a1->number2)
	{
	case 0u:
	case 6u:
		ToggleDoodadState(unit);
		return 1;
	case 4u:
		EnableDoodadState(unit);
		return 1;
	case 5u:
		DisableDoodadState(unit);
		return 1;
	default:
		return 1;
	}
}

FAIL_STUB_PATCH(TriggerAction_SetDoodadState);

int __fastcall TriggerAction_SetDeaths_(Action* a1)
{
	switch (a1->number2)
	{
	case 7u:
		SetDeaths(a1->player, a1->unit, a1->number);
		return 1;
	case 8u:
		AddDeaths(a1->player, a1->unit, a1->number);
		return 1;
	case 9u:
		SubtractDeaths(a1->player, a1->unit, a1->number);
		return 1;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetDeaths);

int __fastcall TriggerAction_PauseTimer_(Action* a1)
{
	TimerIsPaused = 1;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_PauseTimer);

int __fastcall TriggerAction_UnpauseTimer_(Action* a1)
{
	TimerIsPaused = 0;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_UnpauseTimer);

int __fastcall TriggerAction_Draw_(Action* a1)
{
	if (endgame_state[active_trigger_player] == EndgameState::VICTORY || endgame_state[active_trigger_player] == EndgameState::UNKNOWN)
	{
		return 0;
	}
	if (Players[active_trigger_player].nType == PT_Human || Players[active_trigger_player].nType == PT_Computer)
	{
		endgame_state[active_trigger_player] = EndgameState::DRAW;
	}
	word_650970 = byte_6509B4 ? 1 : 45;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_Draw);

int __fastcall TriggerAction_SetAllianceStatus_(Action* a1)
{
	SetAlliance_maybe(a1->player, a1->unit, 0);
	StopAttackingAllies_maybe(active_trigger_player);
	if (active_trigger_player == g_LocalNationID)
	{
		setAlliance();
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetAllianceStatus);

int __fastcall TriggerAction_DisableDebugMode_(Action* a1)
{
	dword_6509AC->container.dwExecutionFlags |= 0x40;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_DisableDebugMode);

int __fastcall TriggerAction_EnableDebugMode_(Action* a1)
{
	dword_6509AC->container.dwExecutionFlags &= ~0x40;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_EnableDebugMode);

#include <sol/sol.hpp>

void lua_print(const char* message)
{
	if (active_trigger_player == g_LocalNationID)
	{
		if (message)
		{
			int display_time = getTextDisplayTime_(message);
			PrintText(message, 2u, display_time + GetTickCount(), 1);
		}
	}
}

int __fastcall TriggerAction_ExecuteLua(Action* a1)
{
	const char* script = GetMapTblString(a1->string);

	sol::state lua;
	lua.set_function("print", lua_print);
	lua.script(script);

	return 1;
}

ActionPointer ActionTable_[] = {
	TriggerAction_NoAction_,
	TriggerAction_Victory_,
	TriggerAction_Defeat_,
	TriggerAction_PreserveTrigger_,
	TriggerAction_Wait_,
	TriggerAction_PauseGame,
	TriggerAction_UnpauseGame,
	TriggerAction_Transmission_,
	TriggerAction_PlayWav_,
	TriggerAction_DisplayTextMessage_,
	TriggerAction_CenterView_,
	TriggerAction_CreateUnitWithProperties,
	TriggerAction_SetMissionObjectives_,
	TriggerAction_SetSwitch_,
	TriggerAction_SetCountdownTimer_,
	TriggerAction_RunAiScript_,
	TriggerAction_RunAiScriptAtLocation_,
	TriggerAction_LeaderBoard_, // Control / Text, TUnit
	TriggerAction_LeaderBoard_, // Control At Location / Text, TUnit, Loc
	TriggerAction_LeaderBoard_, // Resources / Text, ResType
	TriggerAction_LeaderBoard_, // Kills / Text, TUnit
	TriggerAction_LeaderBoard_, // Points / Text, Score
	TriggerAction_KillUnit_,
	TriggerAction_KillUnitAtLocation_,
	TriggerAction_RemoveUnit_,
	TriggerAction_RemoveUnitAtLocation_,
	TriggerAction_SetResources_,
	TriggerAction_SetScore_,
	TriggerAction_MinimapPing_,
	TriggerAction_TalkingPortrait_,
	TriggerAction_MuteUnitSpeech_,
	TriggerAction_UnmuteUnitSpeech_,
	TriggerAction_LeaderBoardComputerPlayers_,
	TriggerAction_LeaderBoard_, // Control / Text, TUnit, Number
	TriggerAction_LeaderBoard_, // Control At Location / Text, TUnit, Number, Loc
	TriggerAction_LeaderBoard_, // Resources / Text, TUnit, Number, ResType
	TriggerAction_LeaderBoard_, // Kills / Text, TUnit, Number
	TriggerAction_LeaderBoard_, // Points / Text, Number, Score
	TriggerAction_MoveLocation,
	TriggerAction_MoveUnit,
	TriggerAction_LeaderBoard_,
	TriggerAction_SetNextScenario_,
	TriggerAction_SetDoodadState_,
	TriggerAction_SetInvincibility,
	TriggerAction_CreateUnitWithProperties,
	TriggerAction_SetDeaths_,
	TriggerAction_Order,
	TriggerAction_NoAction_, // Comment
	TriggerAction_GiveUnitsToPlayer,
	TriggerAction_ModifyUnitHitPoints,
	TriggerAction_ModifyUnitEnergy,
	TriggerAction_ModifyUnitShieldPoints,
	TriggerAction_ModifyUnitResourceAmount,
	TriggerAction_ModifyUnitHangarCount,
	TriggerAction_PauseTimer_,
	TriggerAction_UnpauseTimer_,
	TriggerAction_Draw_,
	TriggerAction_SetAllianceStatus_,
	TriggerAction_DisableDebugMode_,
	TriggerAction_EnableDebugMode_,
	TriggerAction_ExecuteLua,
};

void ExecuteTriggerActions_(TriggerListEntry* a1)
{
	a1->container.dwExecutionFlags = a1->container.dwExecutionFlags | 1;
	if (a1->container.dwExecutionFlags & 2)
	{
		endgame_state[active_trigger_player] = EndgameState::UNKNOWN;
	}
	while (1)
	{
		if (a1->container.bCurrentActionIndex >= 64)
		{
			break;
		}
		Action* action = a1->container.actions + a1->container.bCurrentActionIndex;
		if ((action->flags & 2) == 0)
		{
			if (action->action == 0)
			{
				a1->container.bCurrentActionIndex = 64;
				break;
			}
			if (ActionTable_[action->action](action) == 0)
			{
				break;
			}
		}
		a1->container.bCurrentActionIndex += 1;
	}
	if (a1->container.bCurrentActionIndex >= 64)
	{
		if ((a1->container.dwExecutionFlags & 0x20) != 0)
		{
			ActionTable_[6](0);
		}
		if (a1->container.dwExecutionFlags & 4)
		{
			a1->container.dwExecutionFlags = a1->container.dwExecutionFlags & ~0x51;
			a1->container.bCurrentActionIndex = 0;
		}
		else
		{
			a1->container.dwExecutionFlags = a1->container.dwExecutionFlags | 8;
		}
	}
}

FAIL_STUB_PATCH(ExecuteTriggerActions);

void executeGameTrigger_(TriggerList* a1)
{
	if (a1->begin <= 0)
	{
		return;
	}

	TriggerListEntry* entry = a1->begin;

	for (TriggerListEntry* entry = a1->begin; (int)entry > 0; entry = entry->next)
	{
		if ((entry->container.dwExecutionFlags & 8) == 0)
		{
			dword_6509AC = entry;
			if (entry->container.dwExecutionFlags & 1)
			{
				ExecuteTriggerActions_(entry);
			}
			else if (ExecuteTriggerConditions(entry))
			{
				entry->container.bCurrentActionIndex = 0;
				ExecuteTriggerActions_(entry);
			}
		}
	}
}

FAIL_STUB_PATCH(executeGameTrigger);

void BWFXN_ExecuteGameTriggers_(signed int dwMillisecondsPerFrame)
{
	if (!IS_GAME_PAUSED || byte_6509B4)
	{
		load_endmission();
		countdownTimersExecute(dwMillisecondsPerFrame);
		if (word_6509A0-- == 0)
		{
			memset(endgame_state, EndgameState::INITIAL, 8);
			word_6509A0 = 30;
			byte_685180 = 0;
			bool trigger_has_executed = false;
			unsigned player_related = -1;

			while (true)
			{
				active_trigger_player = getNextActivePlayer(&player_related);

				if (active_trigger_player >= 8)
				{
					break;
				}

				TriggerList* trigger_list = stru_51A218.triggers + active_trigger_player;
				if (trigger_list->begin > 0)
				{
					executeGameTrigger_(trigger_list);
					trigger_has_executed = true;
				}
			}

			if (trigger_has_executed)
			{
				endgameCheck();
			}
		}
	}
}

FAIL_STUB_PATCH(BWFXN_ExecuteGameTriggers);
