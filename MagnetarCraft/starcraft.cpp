#include <ddraw.h>
#include <time.h>
#include "starcraft.h"
#include "tbl_file.h"
#include "patching/FailStubPatch.h"
#include "patching/FunctionPatch.h"
#include "patching/CallSitePatch.h"
#include "patching/MemoryPatch.h"
#include "patching/NopPatch.h"

signed int AppAddExit_(AppExitHandle a1)
{
	AppExitHandle* app_exit_handles_ = app_exit_handles;
	if (!app_exit_handles)
	{
		app_exit_handles_ = (AppExitHandle *)SMemAlloc(
			128,
			"Starcraft\\SWAR\\lang\\gds\\appaddex.cpp",
			42,
			(int)app_exit_handles);
		memset(app_exit_handles_, 0, 128u);
		app_exit_handles = app_exit_handles_;
	}
	int v2 = -1;
	int v3 = 0;
	do
	{
		AppExitHandle exit_handle = app_exit_handles_[v3];
		if (exit_handle == a1)
			return 0;
		if (v2 == -1 && !exit_handle)
			v2 = v3;
		++v3;
	} while (v3 < 32);
	if (v2 == -1)
	{
		FatalError("APPADDEX:1");
		app_exit_handles_ = app_exit_handles;
	}
	app_exit_handles_[v2] = a1;
	return 1;
}

HANDLE LoadInstallArchiveHD(const char *a1, CHAR *a2, HANDLE hMpq, HANDLE phFile)
{
	if (!GetModuleFileNameA(hInst, a2, MAX_PATH))
		*a2 = 0;
	char* v4 = strrchr(a2, '\\');
	if (v4)
		*v4 = 0;
	SStrNCat(a2, (char *)hMpq, MAX_PATH);
	if (!SFileOpenArchive(a2, (DWORD)phFile, 2u, &hMpq))
		goto LABEL_15;
	if (a1)
	{
		if (!SFileOpenFileEx(hMpq, a1, 0, &phFile))
		{
			SFileCloseArchive(hMpq);
			return 0;
		}
		SFileCloseFile(phFile);
	}
	HANDLE result = hMpq;
	if (!hMpq)
		LABEL_15:
	result = 0;
	return result;
}

signed int InitializeCDArchives_(const char *filename, int a2)
{
	if (hMpq)
		return 1;

	char path_buffer[MAX_PATH];
	hMpq = LoadInstallArchiveHD(filename, path_buffer, "\\BroodWar.mpq", (HANDLE)1000);
	if (hMpq || (hMpq = LoadInstallArchiveHD(filename, path_buffer, "\\StarCraft.mpq", (HANDLE)1000)) != 0)
		return 1;


	hMpq = LoadInstallArchiveCD(1000u, "\\Install.exe", (char *)filename);
	if (hMpq)
		return 1;

	while (a2)
	{
		INT_PTR v4 = DialogBoxParamA(hModule, (LPCSTR)107, hWndParent, DialogFunc, 0);
		if (v4 == -1)
		{
			FatalError("GdsDialogBoxParam: %d", 107);
		LABEL_13:
			AppExit(0);
			ProcError(1);
			exit(0);
		}
		if (v4 != 1)
			goto LABEL_13;
		hMpq = LoadInstallArchiveCD(1000u, "\\Install.exe", (char *)filename);
		if (hMpq)
			return 1;
	}
	return 0;
}

int(*signal)(int a1, int a2) = (int(*)(int a1, int a2)) 0x0040C8D5;

void* fastFileRead_(int *bytes_read, int searchScope, char *filename, int defaultValue, int bytes_to_read, char *logfilename, int logline)
{
	HANDLE phFile;

	if (SFileOpenFileEx(0, filename, searchScope, &phFile))
	{
		LONG filesize = SFileGetFileSize(phFile, 0);
		if (filesize == -1)
		{
			FileFatal(phFile, GetLastError());
		}
		if (bytes_read)
			*bytes_read = filesize;
		if (!filesize)
		{
			if (bytes_to_read)
			{
				SFileCloseFile(phFile);
				return 0;
			}
			SysWarn_FileNotFound(filename, 24);
		}
		void* buffer = (void *)defaultValue;
		if (!defaultValue)
			buffer = SMemAlloc(filesize, logfilename, logline, defaultValue);
		if (!SFileReadFile(phFile, buffer, filesize, &bytes_to_read, 0))
		{
			if (GetLastError() == 38)
				FileFatal(phFile, 24);
			FileFatal(phFile, GetLastError());
		}
		if (bytes_to_read != filesize)
			FileFatal(phFile, 24);
		SFileCloseFile(phFile);
		return buffer;
	}
	else
	{
		int lastError = SErrGetLastError();
		if (!bytes_to_read || lastError != 2 && lastError != 1006)
		{
			SysWarn_FileNotFound(filename, lastError);
			//JUMPOUT(*(_DWORD *)byte_4D2E37);
		}
		if (bytes_read)
			*bytes_read = 0;
		return 0;
	}
}

void InitializeFontKey_(void)
{
	char buff[MAX_PATH];
	_snprintf(buff, MAX_PATH, "%s\\%s.gid", "font", "font");
	void* v0 = fastFileRead_(&cdkey_encrypted_len, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (v0)
	{
		if (cdkey_encrypted_len == 0)
		{
			SMemFree(v0, "Starcraft\\SWAR\\lang\\grid.cpp", 118, 0);
		}
		else
		{
			goto LABEL_5;
		}
	}
	v0 = 0;
LABEL_5:
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

FailStubPatch InitializeFontKey_patch(InitializeFontKey);

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

FailStubPatch LoadMainModuleStringInfo_patch(LoadMainModuleStringInfo);

int InitializeArchiveHandles_()
{
	char *v0; // eax@3
	HANDLE v1; // eax@6
	char *v2; // eax@12
	HANDLE v3; // eax@15
	char *v4; // eax@22
	HANDLE v5; // eax@25
	BOOL v6; // eax@29
	CHAR Filename[MAX_PATH]; // [sp+8h] [bp-20Ch]@1
	CHAR archivename_[MAX_PATH]; // [sp+10Ch] [bp-108h]@20
	HANDLE phFile; // [sp+210h] [bp-4h]@5

	dword_51CD44 = 20;
	dword_51CD48 = aInternalVersio;
	dword_51CD4C = tstrFilename;
	dword_51CD50 = broodat_mpq_path;
	dword_51CD54 = archivename;
	LoadMainModuleStringInfo_();
	if (!GetModuleFileNameA(hInst, Filename, MAX_PATH))
		Filename[0] = 0;
	v0 = strrchr(Filename, '\\');
	if (v0)
		*v0 = 0;
	SStrNCat(Filename, "\\Stardat.mpq", MAX_PATH);
	if (!SFileOpenArchive(Filename, 2000u, 2u, &phFile) || (v1 = phFile) == 0)
		v1 = 0;
	dword_51CC38 = v1;
	if (!v1)
	{
		int last_error = GetLastError();
		SysWarn_FileNotFound("Stardat.mpq", last_error);
	}
	if (!GetModuleFileNameA(hInst, archivename, MAX_PATH))
		archivename[0] = 0;
	v2 = strrchr(archivename, '\\');
	if (v2)
		*v2 = 0;
	SStrNCat(archivename, "\\patch_rt.mpq", MAX_PATH);
	if (!SFileOpenArchive(archivename, 7000u, 2u, &phFile) || (v3 = phFile) == 0)
		v3 = 0;
	dword_51CC28 = v3;

    char magnetarDatFilename[MAX_PATH] = { 0 };
    if (!GetModuleFileNameA(hInst, magnetarDatFilename, MAX_PATH))
		*magnetarDatFilename = 0;
    auto separator = strrchr(magnetarDatFilename, '\\');
    if (separator)
		*separator = 0;
    SStrNCat(magnetarDatFilename, "\\MagnetarDat.mpq", MAX_PATH);
    !SFileOpenArchive(magnetarDatFilename, 8000u, 2u, &phFile);

	InitializeFontKey_();
	AppAddExit_(DestroyFontKey);
	if (!is_spawn)
		InitializeCDArchives_(0, 1);
	broodat_mpq_path[0] = 0;
	if (!is_spawn)
	{
		if (!GetModuleFileNameA(hInst, archivename_, MAX_PATH))
			*archivename_ = 0;
		v4 = strrchr(archivename_, '\\');
		if (v4)
			*v4 = 0;
		SStrNCat(archivename_, "\\Broodat.mpq", MAX_PATH);
		if (!SFileOpenArchive(archivename_, 2500u, 2u, &phFile) || (v5 = phFile) == 0)
			v5 = 0;
		dword_51CC2C = v5;
		if (v5)
		{
			SStrCopy(broodat_mpq_path, archivename_, 0x208u);
			SStrNCat(broodat_mpq_path, ";", 520);
		}
	}
	v6 = SFileOpenFileEx(0, "rez\\epilogX.txt", 0, &phFile);
	if (v6)
	{
		SFileCloseFile(phFile);
		v6 = 1;
	}
	dword_6D11E4 = v6;
	return SStrNCat(broodat_mpq_path, Filename, 520);
}

FailStubPatch InitializeArchiveHandles_patch(InitializeArchiveHandles);

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

void * loadTBL_(int a1, int a2, char *a3, char *filename, char **a5)
{
	char **v6; // edi@1
	void *result; // eax@1
	unsigned __int16 v9; // cx@3
	char *v10; // ecx@4

	v6 = a5;
	for (result = _fastFileRead(filename, 0, 0, a3, a1); a2; --a2)
	{
		if (*v6)
		{
			v9 = *(_WORD *)v6 - 1;
			if (*(_WORD *)v6)
			{
				if (v9 < *(_WORD *)result)
					v10 = (char *)result + *((_WORD *)result + v9 + 1);
				else
					v10 = "";
			}
			else
			{
				v10 = 0;
			}
			*v6 = v10;
		}
		++v6;
	}
	return result;
}

void LoadGameData_(DatLoad* a1, char* a2)
{
	unsigned int offset;

	int bytes_read = 0;
	BYTE* v3 = (BYTE*) fastFileRead(&bytes_read, 0, a2, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 356);
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

void LoadSfx_()
{
	LoadGameData_(sfxdataDat, "arr\\sfxdata.dat");
	dword_5999B0 = loadTBL_(1711, 1144, "Starcraft\\SWAR\\lang\\snd.cpp", "arr\\sfxdata.tbl", SFXData_SoundFile);
}

FailStubPatch LoadSfx_patch(LoadSfx);

char* MapdataFilenames_[73];

MemoryPatch MapdataFilenames_patch1(0x4280A2, MapdataFilenames_, 4);
MemoryPatch MapdataFilenames_patch2(0x4A7DC9, MapdataFilenames_, 4);
MemoryPatch MapdataFilenames_patch3(0x512BA0, MapdataFilenames_, 4);
MemoryPatch MapdataFilenames_patch4(0x512BA8, (void*) _countof(MapdataFilenames_), 4);

void CommandLineCheck_()
{
	const char* command_line = GetCommandLineA();
	if (command_line)
	{
		strTokenize(command_line);
		CheatFlags game_cheats = GameCheats;
		for (char* argument = strTokenize(0); argument; argument = strTokenize(0))
		{
			if (CommandLineCheatCompare(&game_cheats, argument))
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

FailStubPatch CommandLineCheck_patch(CommandLineCheck);

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
	if (hMpq)
	{
		DataVersionCheck("rez\\CDversion.txt");
	}
	dword_6CDFEC |= 7u;
	LoadRegOptions();
	AppAddExit_(saveRegOptions);
	LoadNetworkTBL();
	LoadAccelerators_();
	AppAddExit_(DestroyAccelerators);
	LoadMenuFonts();
	AppAddExit_(DestroyFonts);
	InitializeImage();
	AppAddExit_(DestroyImage);
	LoadCursors();
	InitializeScreenLayer();
	dword_6D5E20 = &GameScreenBuffer;
	CreateHelpContext();
	AppAddExit_(DestroyHelpContext);
	LoadGameData_(mapdataDat, "arr\\mapdata.dat"); // TODO: is this call needed?
	dword_51CC30 = loadTBL_(1577, _countof(MapdataFilenames_), "Starcraft\\SWAR\\lang\\init.cpp", "arr\\mapdata.tbl", MapdataFilenames_);
	AppAddExit_(FreeMapdataTable);
	LoadGameTemplates(Template_Constructor);
	AppAddExit_(DestroyGameTemplates);
}

FailStubPatch PreInitData_patch(PreInitData);

char *GetErrorString_(LPSTR lpBuffer, DWORD a2, unsigned int a3)
{
	unsigned int v4; // eax@1
	char *v5; // edi@1
	unsigned int v6; // eax@10
	char *i; // ecx@10
	char v8; // dl@11

	v4 = (a3 >> 16) & 0x1FFF;
	v5 = lpBuffer;
	switch (v4)
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
		if (!SErrGetErrorStr(a3, lpBuffer, a2) && !FormatMessageA(0x1000u, 0, a3, 0x400u, v5, a2, 0))
			_snprintf(v5, a2, "unknown error 0x%08x", a3);
		break;
	}
	v6 = strlen(v5);
	for (i = &v5[v6 - 1]; (signed int)v6 > 0; *i = 0)
	{
		v8 = *(i - 1);
		--v6;
		--i;
		if (v8 != '\r' && v8 != '\n')
			break;
	}
	return v5;
}

void ErrorDDrawInit_(char *source_file, char *function_name, unsigned int last_error, WORD resource, int source_line)
{
	char dwInitParam[512];
	CHAR Buffer[256];

	char* v5 = source_file;
	char* v6 = function_name;
	char* v7 = strrchr(source_file, '\\');
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
	if (DialogBoxParamA(hModule, (LPCSTR)resource, hWndParent, DialogFunc, (LPARAM)dwInitParam) == -1)
		FatalError("GdsDialogBoxParam: %d", resource);
	DLGErrFatal();
}

FailStubPatch ErrorDDrawInit_patch(ErrorDDrawInit);

BOOL BWFXN_DDrawInitialize_()
{
	int v7; // edi@16
	int v8; // eax@16
	unsigned int v9; // ebx@16
	unsigned int v10; // ebx@18
	unsigned int v11; // ebx@20
	unsigned int v12; // ebx@22
	unsigned int v13; // ebx@25
	PALETTEENTRY palette_entries[256]; // [sp+Ch] [bp-46Ch]@18
	DDSURFACEDESC surface_desc; // [sp+40Ch] [bp-6Ch]@20

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
	if (DDInterface->SetDisplayMode(640, 480, 8))
	{
		v7 = GetSystemMetrics(SM_CXSCREEN);
		v8 = GetSystemMetrics(SM_CYSCREEN);
		v9 = DDInterface->SetDisplayMode(v7, v8, 8);
		if (v9)
			ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "SetDisplayMode", v9, 0x67u, 160);
	}
#endif
	memset(palette_entries, 0, sizeof(palette_entries));
	v10 = DDInterface->CreatePalette(DDPCAPS_ALLOW256 | DDPCAPS_8BIT, palette_entries, &PrimaryPalette, 0);
	if (v10)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "CreatePalette", v10, 0x66u, 182);
	memset(&surface_desc, 0, sizeof(surface_desc));
	surface_desc.dwSize = 108;
	surface_desc.dwFlags = 1;
	surface_desc.ddsCaps.dwCaps = 512;
	v11 = DDInterface->CreateSurface(&surface_desc, &PrimarySurface, 0);
	if (v11)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "CreatePrimarySurface", v11, 0x66u, 193);
#ifndef BYPASS_DDRAW_STUFF
	v12 = PrimarySurface->SetPalette(PrimaryPalette);
	if (v12)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "SetPalette", v12, 0x66u, 203);
#endif
	surface_desc.dwSize = 108;
	if (PrimarySurface->Lock(0, &surface_desc, 1, 0))
	{
		memset(&surface_desc, 0, sizeof(surface_desc));
		surface_desc.dwSize = 108;
		surface_desc.dwFlags = 7;
		surface_desc.ddsCaps.dwCaps = 2112;
		surface_desc.dwWidth = 640;
		surface_desc.dwHeight = 480;
		v13 = DDInterface->CreateSurface(&surface_desc, &BackSurface, 0);
		if (v13)
			ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "CreateBackSurface", v13, 0x66u, 220);
	}
	else
	{
		PrimarySurface->Unlock(surface_desc.lpSurface);
	}
	return SDrawManualInitialize(hWndParent, DDInterface, PrimarySurface, 0, 0, BackSurface, PrimaryPalette, 0);
}

FunctionPatch BWFXN_DDrawInitialize_patch(BWFXN_DDrawInitialize, BWFXN_DDrawInitialize_);

void __stdcall DrawGameProc_(Bitmap* a1, bounds* a2)
{
	int v2 = ScreenLayers[5].bits & 1;
	if ((ScreenLayers[5].bits & 1) != 0)
	{
		maskSomething0();
		memcpy(dword_6D5C10, dword_6D5C0C, 0x198u);
		BWFXN_updateImageData();
		maskSomething2();
		BWFXN_drawMapTiles();
		BWFXN_blitMapTiles();
	}
	else
	{
		maskSomething1();
		refreshImageRange(result, dword_5993C0);
		maskSomething2();
		BWFXN_drawMapTiles();
		blitTileCacheOnRefresh();
	}
	BWFXN_drawAllSprites();
	if (CurrentTileSet == Tileset::Platform)
	{
		if (v2)
			drawStars();
		else
			refreshStars();
	}
	updateAllFog();
	BWFXN_DrawHighTarget();
	BWFXN_drawDragSelBox();
	BWFXN_drawAllThingys();
}

FunctionPatch DrawGameProc_patch(DrawGameProc, DrawGameProc_);

void audioVideoInit_()
{
	loadColorSettings();
	GameScreenBuffer.wid = 640;
	GameScreenBuffer.ht = 480;
	GameScreenBuffer.data = 0;
	GameScreenBuffer.data = (u8 *)SMemAlloc(640 * 480, "Starcraft\\SWAR\\lang\\gds\\vidinimo.cpp", 55, 0);
	BWFXN_DDrawInitialize_();
	dword_6D5DF8 = 1;
	AppAddExit_(vidinimoDestroy);
	memcpy(stru_6CEB40, &palette, sizeof(PALETTEENTRY[256]));
	BWFXN_RedrawTarget();
	LoadSfx_();
	AppAddExit_(sfxdata_cleanup);
	if (!byte_6D11D0)
	{
		AudioVideoInitializationError error;
		if (DSoundInit(&error, hWndParent))
			AppAddExit_(j_BWFXN_DSoundDestroy);
	}
}

FailStubPatch audioVideoInit_patch(audioVideoInit);

void InitializeDialog_(dialog *a1, FnInteract a2)
{
	dlgEvent event;

	a1->lFlags |= 0x44000000u;
	a1->fields[0].dlg.pModalFcn = 0;
	if (a2)
		a1->pfcnInteract = a2;
	size_t size = a1->fields[0].dlg.dstBits_wid * a1->fields[0].dlg.dstBits_ht;
	a1->fields[0].dlg.dstBits_data = (u8*)SMemAlloc(size, "Starcraft\\SWAR\\lang\\dlg\\dlg.cpp", 481, 0);
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
	a1->lFlags &= ~0x4000000u;
}

bool __fastcall TitleDlgProc_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		if (evt->dwUser == EventUser::USER_CREATE)
		{
			titleInit(dlg);
		}
		else if (evt->dwUser == EventUser::USER_DESTROY)
		{
			DrawBINDialog(dlg);
			return genericDlgInteract(dlg, evt);
		}
	}
	return genericDlgInteract(dlg, evt);
}

FailStubPatch TitleDlgProc_patch(TitleDlgProc);

void LoadTitle_()
{
	HANDLE phFile;

	if (!SFileOpenFileEx(0, "rez\\titledlg.bin", 0, &phFile))
	{
		int v8 = SErrGetLastError();
		SysWarn_FileNotFound("rez\\titledlg.bin", v8);
	}
	LONG v1 = SFileGetFileSize(phFile, 0);
	if (v1 == -1)
	{
		int v3 = GetLastError();
		FileFatal(phFile, v3);
	}
	else
	{
		if (!v1)
			SysWarn_FileNotFound("rez\\titledlg.bin", 24);
		dialog *v4 = (dialog *)SMemAlloc(v1, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210, 0);
		int read;
		if (SFileReadFile(phFile, v4, v1, &read, 0))
		{
			if (read == v1)
			{
				SFileCloseFile(phFile);
				if (v4)
				{
					v4->lFlags |= 4u;
					AllocInitDialogData(v4, v4, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\title.cpp", 190);
					load_screen = v4;
				}
				else
				{
					load_screen = 0;
				}
				InitializeDialog_(load_screen, TitleDlgProc_);
			}
			else
			{
				FileFatal(phFile, 24);
			}
		}
		else if (GetLastError() == 38)
		{
			FileFatal(phFile, 24);
		}
		else
		{
			int v6 = GetLastError();
			FileFatal(phFile, v6);
		}
	}
}

FailStubPatch LoadTitle_patch(LoadTitle);

void LoadInitIscriptBIN_()
{
	int iscript_bin_size;
	iscript_data = fastFileRead_(&iscript_bin_size, 0, "scripts\\iscript.bin", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	LoadGameData_(imagesDat, "arr\\images.dat");
	if (!SBmpLoadImage("game\\tselect.pcx", 0, dword_5240B8, 24, 0, 0, 0))
	{
		SysWarn_FileNotFound("game\\tselect.pcx", SErrGetLastError());
	}
	LoadImageData();
	InitDamageOverlayCounts();
}

FailStubPatch LoadInitIscriptBIN_patch(LoadInitIscriptBIN);

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

// FailStubPatch CreateInitialMeleeWorker_patch(CreateInitialMeleeWorker);

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
				CreateInitialMeleeBuildings(player->nRace, player_index);
				if (player->nRace == Race::RACE_Zerg)
					CreateInitialOverlord(player_index);
				[[fallthrough]];
			case StartingUnits::SU_WORKER_ONLY:
				CreateInitialMeleeWorker(player->nRace, player_index);
				CreateInitialMeleeWorker(player->nRace, player_index);
				CreateInitialMeleeWorker(player->nRace, player_index);
				CreateInitialMeleeWorker(player->nRace, player_index);
				[[fallthrough]];
			case StartingUnits::SU_MAP_DEFAULT:
				break;
			}
		}
	}
}

FailStubPatch CreateInitialMeleeUnits_patch(CreateInitialMeleeUnits);

bool __stdcall ChkLoader_TYPE_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_VER_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_DIM_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_VCOD_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_ERA_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_STR_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_MTXM_(SectionData* a1, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_THG2_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_UNIT_(SectionData* section_data, int section_size, MapChunks* a3);

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
	CreateChkSectionLoader("SPRP", ChkLoader_SPRP, 1),
	CreateChkSectionLoader("FORC", ChkLoader_FORC, 1),
	CreateChkSectionLoader("VCOD", ChkLoader_VCOD_, 1),
};

ChkSectionLoader chk_loaders_briefing_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MBRF", ChkLoader_MBRF, 1),
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
	CreateChkSectionLoader("COLR", ChkLoader_COLR, 1),
};

ChkSectionLoader chk_loaders_ums_1_00_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("MASK", ChkLoader_MASK, 1),
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
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN, 1),
	CreateChkSectionLoader("TRIG", ChkLoader_TRIG, 1),
};

ChkSectionLoader chk_loaders_ums_1_04_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("MASK", ChkLoader_MASK, 1),
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
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN_, 1),
	CreateChkSectionLoader("TRIG", ChkLoader_TRIG, 1),
};

ChkSectionLoader chk_loaders_ums_broodwar_1_04_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("MASK", ChkLoader_MASK, 1),
	CreateChkSectionLoader("UNIx", ChkLoader_UNIx, 1),
	CreateChkSectionLoader("UPGx", ChkLoader_UPGx, 1),
	CreateChkSectionLoader("TECx", ChkLoader_TECx, 1),
	CreateChkSectionLoader("PUNI", ChkLoader_PUNI, 1),
	CreateChkSectionLoader("PUPx", ChkLoader_PUPx, 1),
	CreateChkSectionLoader("PTEx", ChkLoader_PTEx, 1),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT_, 1),
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN_, 1),
	CreateChkSectionLoader("TRIG", ChkLoader_TRIG, 1),
	CreateChkSectionLoader("COLR", ChkLoader_COLR, 1),
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

MemoryPatch chk_loader_patch1(0x4BF65A, &chk_loaders_->lobby_loader_count, 4);
MemoryPatch chk_loader_patch2(0x4BF660, &chk_loaders_->lobby_loaders, 4);
MemoryPatch chk_loader_patch3(0x4CC0CD, chk_loaders_, 4);
MemoryPatch chk_loader_patch4(0x4CC0E1, &chk_loaders_->requires_expansion, 4);
MemoryPatch chk_loader_patch5(0x4CCA8A, &chk_loaders_->briefing_loader_count, 4);
MemoryPatch chk_loader_patch6(0x4CCA90, &chk_loaders_->briefing_loaders, 4);
MemoryPatch chk_loader_patch7(0x4CCBFC, &chk_loaders_->lobby_loader_count, 4);
MemoryPatch chk_loader_patch8(0x4CCC02, &chk_loaders_->lobby_loaders, 4);

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
	if (chk_data)
	{
		int loader_index = 0;
		if (ReadMapChunks_(0, chk_data, &loader_index, chk_size))
		{
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
			int result = ReadChunkNodes_(loader_count, chk_size, loaders, chk_data, 0);
			SMemFree(chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2159, 0);
			if (mapArchiveHandle)
			{
				SFileCloseArchive(mapArchiveHandle);
				mapArchiveHandle = 0;
			}
			return result;
		}
		else
		{
			SMemFree(chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2149, 0);
			if (mapArchiveHandle)
			{
				SFileCloseArchive(mapArchiveHandle);
				mapArchiveHandle = 0;
			}
			return 0;
		}
	}
	else
	{
		if (mapArchiveHandle)
		{
			SFileCloseArchive(mapArchiveHandle);
			mapArchiveHandle = 0;
		}
		return 0;
	}
}

FailStubPatch sub_4CC7F0_patch(sub_4CC7F0);

int LoadMap_()
{
	if (InReplay)
	{
		int loader_index = 0;
		int result = ReadMapChunks_(0, scenarioChk, &loader_index, scenarioChkSize);
		if (result)
		{
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
			return ReadChunkNodes_(loader_count, scenarioChkSize, loaders, scenarioChk, 0);
		}
	}
	else if (CurrentMapFileName[0])
	{
		return sub_4CC7F0_(CurrentMapFileName);
	}

	return 0;
}

FunctionPatch LoadMap_patch(LoadMap, LoadMap_);

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
	initMapData();
	InitializePresetImageArrays();
	InitializeSpriteArray();
	InitializeThingyArray();
	LoadGameData_(flingyDat, "arr\\flingy.dat");
	memset(dword_63FEE0, 0, 76u);
	dword_63FF3C = (CUnit *)dword_63FEE0;
	dword_63FF38 = (CUnit *)dword_63FEE0;
	dword_63FEC8 = 0;
	dword_63FF34 = 0;
	InitializeBulletArray();
	InitializeOrderArray();
	sub_4CB5B0(0, off_5127DC[CurrentTileSet]->y);
	sub_41E450(sub_4BDB30, palette);
	sub_4C99C0();
	if (!loadGameFileHandle)
		InitializeUnitCounts();
	initializePsiFieldData();
	ResetDATFiles();
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
				CreateInitialTeamMeleeUnits();
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
				ReplayFrames = ElapsedTimeFrames;
			}
		}
		nextLeaveGameMenu();
	}
	return 0;
}

FunctionPatch GameInit_patch(GameInit, GameInit_);
FailStubPatch sub_4CD770_patch(sub_4CD770);
FailStubPatch sub_4A13B0_patch(sub_4A13B0);

GotFileValues* readTemplate_(char* template_name, char* got_template_name, char* got_template_label)
{
	char buff[260];
	int got_file_size;

	_snprintf(buff, 0x104u, "%s%s%s", "Templates\\", template_name, ".got");
	GotFile* got_file_data = (GotFile*)fastFileRead(&got_file_size, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
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

GotFileValues* InitUseMapSettingsTemplate_()
{
	char ununsed[32];

	return readTemplate_("Use Map Settings(1)", ununsed, ununsed);
}

FunctionPatch InitUseMapSettingsTemplate_patch(InitUseMapSettingsTemplate, InitUseMapSettingsTemplate_);

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
		int loader_index = 0;
		if (ReadMapChunks_(a2, chk_data, &loader_index, chk_size))
		{
			int v7 = ReadChunkNodes_(chk_loaders_[loader_index].lobby_loader_count, chk_size, chk_loaders_[loader_index].lobby_loaders, chk_data, a2);
			SMemFree((void*)chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2077, 0);
			mapHandleDestroy();
			return v7;
		}
		SMemFree(chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2068, 0);
		mapHandleDestroy();
		return 0;
	}
	if (mapArchiveHandle)
	{
		SFileCloseArchive(mapArchiveHandle);
		mapArchiveHandle = 0;
	}
	return 0;
}

FailStubPatch sub_4CCAC0_patch(sub_4CCAC0);

FailStubPatch sub_4CC2A0_patch(sub_4CC2A0);

int __stdcall ReadMapData_(char* source, MapChunks* a4, int is_campaign)
{
	char* v8;
	__int16 v12;
	char* v13;

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
		int loader_index = 0;
		if (!ReadMapChunks_(a4, scenarioChk, &loader_index, scenarioChkSize)
			|| !ReadChunkNodes_(chk_loaders_[loader_index].lobby_loader_count, scenarioChkSize, chk_loaders_[loader_index].lobby_loaders, scenarioChk, a4))
			return 0;
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
		CampaignIndex = MD_none;
	v12 = LOWORD(a4->data0);
	dword_5994DC = 1;
	if (v12 == 0)
		goto LABEL_25;
	if (MapStringTbl.buffer)
	{
		if (v12 - 1 < *MapStringTbl.buffer)
		{
			v13 = (char*)MapStringTbl.buffer + MapStringTbl.buffer[v12];
			goto LABEL_26;
		}
	LABEL_25:
		v13 = "";
		goto LABEL_26;
	}
	v13 = 0;
LABEL_26:
	SStrCopy(CurrentMapName, v13, 32u);
	return 1;
}

FunctionPatch ReadMapData_patch(ReadMapData, ReadMapData_);

bool ReadCampaignMapData_(MapChunks* map_chunks)
{
	return ReadMapData_(MapdataFilenames_[CampaignIndex], map_chunks, 1) != 0;
}

FailStubPatch ReadCampaignMapData_patch(ReadCampaignMapData);

int LevelCheatInitGame__()
{
	if (!OpheliaEnabled)
	{
		return 1;
	}

	MapChunks map_chunks;

	OpheliaEnabled = 0;
	if (!playerName[0])
	{
		const char* v1;
		if (*networkTable > 0x47u)
		{
			v1 = (char*)networkTable + networkTable[72];
		}
		else
		{
			v1 = empty_string;
		}
		SStrCopy(playerName, v1, 25);
	}
	if (CampaignIndex == MD_none)
	{
		char dest[260];
		SStrCopy(dest, CurrentMapFileName, 260u);
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
						ReplayFrames = ElapsedTimeFrames;
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
		SStrCopy(v6.player_name, playerName, 24u);
		SStrCopy(v6.map_name, CurrentMapName, 32u);
		v6.active_human_players = 1;
		v6.max_players = 1;
		v6.game_speed = GameSpeed;
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
							ReplayFrames = ElapsedTimeFrames;
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
						ReplayFrames = ElapsedTimeFrames;
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
				ReplayFrames = ElapsedTimeFrames;
			}
		}
		nextLeaveGameMenu();
	}
	return 0;
}

FailStubPatch LevelCheatInitGame_patch(LevelCheatInitGame);

signed int LoadGameInit_()
{
	stopMusic();
	if (InReplay)
	{
		if (!scenarioChk)
			LoadReplayFile(CurrentMapFileName, 0);
		if (InReplay)
		{
			*(int*)playerForce = playerForceSomethingReplay;
			*((int*)playerForce + 1) = dword_6D11A5;
		}
	}
	if (!loadGameFileHandle)
		ElapsedTimeFrames = 0;
	if (!LOBYTE(multiPlayerMode))
	{
		if (!LevelCheatInitGame__() || !LoadGameCreate() || !sub_4EE3D0() || !SinglePlayerMeleeInitGame())
			return 0;
		if (InReplay)
		{
			initialSeed = *(int *)((char *)&replaySeed + 1);
		}
		else
		{
			initialSeed = time(0);
			*(int *)((char *)&replaySeed + 1) = initialSeed;
		}
	}
	InitializeScreenLayer();
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
	LoadRaceUI();
	hotkeyRemapping();
	if (!GameInit_())
		return 0;
	if (InReplay)
		getReplPlayerColors((int)factionsColorsOrdering);
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
	if (!LoadGameCore())
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
	SetGameSpeed_maybe(GameSpeed, 0, 1u);
	if (InReplay)
	{
		copyPlayerStructsToReplayPlayerStructs(Players, &gameData);
		int* v4 = (int*)replayData;
		dword_6D5BF0 = 0;
		int v5 = *((_DWORD *)replayData + 2);
		*(_DWORD *)replayData = 0;
		v4[1] = 1;
		v4[7] = v5;
		ReplayVision = 255;
		playerVisions = 255;
		replayShowEntireMap = 0;
		nextReplayCommandFrame = -1;
		playerExploredVisions = 65280;
	}
	else
	{
		createNewGameActionDataBlock();
	}
	return 1;
}

FailStubPatch LoadGameInit_patch(LoadGameInit);

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
	if (gwGameMode == GAME_GLUES && glGluesMode == GLUE_MAIN_MENU)
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
		if (gwGameMode == GAME_RUN)
		{
			hAccTable = DlgAccelerator;
			input_procedures[16] = AcceleratorTables;
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
	destroyGameHUD();
	DestroyMapData();
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
		SMemFree(SAIPathing, "Starcraft\\SWAR\\lang\\sai_PathCreate.cpp", 226, 0);
		SAIPathing = 0;
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

	stopSounds();
	stopMusic();
	stopSounds();
	LoadRaceSFX(0);
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
		if (scenarioChk != 0)
		{
			SMemFree(scenarioChk, "Starcraft\\SWAR\\lang\\replay.cpp", 1106, 0);
			scenarioChk = 0;
		}
		InReplay = 0;
		game_id_hash = 0;
	}
	else
	{
		SaveReplay("LastReplay", 1);
		if (league_maybe)
		{
			char a1[260];
			createLeagueFile(a1);
			SNetSendReplayPath(a1, game_id_hash, validation_replay_path[0] != 0 ? validation_replay_path : NULL);
		}
		game_id_hash = 0;
	}
}

FailStubPatch DestroyGame_patch(DestroyGame);

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
		GamePosition next_game_position = BeginGame(a1);
		DestroyGame_();
		gwGameMode = next_game_position;
	}
	else
	{
		DestroyGame_();
		gwGameMode = GAME_GLUES;
	}
}

FailStubPatch GameRun_patch(GameRun);

void setup_HUD_()
{
	LoadConsoleImage();
	if (!SBmpLoadImage("game\\thpbar.pcx", 0, byte_66FBE4, 19, 0, 0, 0))
	{
		int v0 = SErrGetLastError();
		SysWarn_FileNotFound("game\\thpbar.pcx", v0);
	}
	load_statfluf_BIN();
	loadPortdata_BINDLG();
	load_statlb();
	load_StatRes_BIN();
	load_Statdata_BIN();
	load_WireframeGRP();
	load_statbtn_BIN();
	load_Statf10_BIN();
	ctextbox_BIN();
	load_gluMinimap();
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

FunctionPatch setup_HUD_patch(setup_HUD, setup_HUD_);

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
void __cdecl minimapVisionUpdate_()
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

FailStubPatch minimapVisionUpdate_64_patch(minimapVisionUpdate_64);
FailStubPatch minimapVisionUpdate_96_128_patch(minimapVisionUpdate_96_128);
FailStubPatch minimapVisionUpdate_192_256_patch(minimapVisionUpdate_192_256);

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
void __cdecl minimapSurfaceUpdate_()
{
	for (int i = 0; i < map_size.height / word_59CC68; i++)
	{
		for (int j = 0; j < map_size.width / word_59CC68; j++)
		{
			minimapSurfaceUpdateMegatile<PIXEL_STRIDE>(i, j);
		}
	}
}

FailStubPatch minimapSurfaceUpdate_64_patch(minimapSurfaceUpdate_64);
FailStubPatch minimapSurfaceUpdate_96_128_patch(minimapSurfaceUpdate_96_128);
FailStubPatch minimapSurfaceUpdate_192_256_patch(minimapSurfaceUpdate_192_256);

void __cdecl setMapSizeConstants_()
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

FunctionPatch setMapSizeConstants_patch(setMapSizeConstants, setMapSizeConstants_);

char* TILESET_NAMES[] = {
	"badlands",
	"platform",
	"install",
	"AshWorld",
	"Jungle",
	"Desert",
	"Ice",
	"Twilight",
};

bool __stdcall ChkLoader_TYPE_(SectionData* section_data, int section_size, MapChunks* a3)
{
	return 1;
}

FailStubPatch ChkLoader_TYPE_patch(ChkLoader_TYPE);

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

FailStubPatch ChkLoader_VER_patch(ChkLoader_VER);

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

FailStubPatch ChkLoader_DIM_patch(ChkLoader_DIM);

bool __stdcall ChkLoader_ERA_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 2)
		return 0;
	if (section_data->start_address + section_data->size > section_data->next_section)
		return 0;

	CurrentTileSet = *(Tileset*)section_data->start_address;
	if (CurrentTileSet > Tileset::Jungle && !IsExpansion)
		return 0;

	int tileset_count = sizeof(TILESET_NAMES) / sizeof(*TILESET_NAMES);
	CurrentTileSet = Tileset(CurrentTileSet % tileset_count);
	return 1;
}

FailStubPatch ChkLoader_ERA_patch(ChkLoader_ERA);

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
	AppAddExit(freeCHKStringHandle);
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return false;
	}

	memcpy(MapStringTbl.buffer, section_data->start_address, section_data->size);
	return true;
}

FailStubPatch ChkLoader_STR_patch(ChkLoader_STR);

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

FailStubPatch ChkLoader_VCOD_patch(ChkLoader_VCOD);
FailStubPatch CopySectionData_patch(CopySectionData);

#define MAX_MAP_DIMENTION 256

DEFINE_ENUM_FLAG_OPERATORS(MegatileFlags);

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
	return SAI_PathCreate(active_tiles) != 0;
}

FailStubPatch ChkLoader_MTXM_patch(ChkLoader_MTXM);

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

FailStubPatch ChkLoader_THG2_patch(ChkLoader_THG2);

const int SCREEN_WDITH = 640;
const int SCREEN_HEIGHT = 480;
const int INTERFACE_HEIGHT = 96;

int CHK_UNIT_StartLocationSub_(Position* a1, ChunkUnitEntry* a2)
{
	if (a2->unit_type != Special_Start_Location)
	{
		return 0;
	}
	a1[a2->player] = a2->position;

	bool v3 = InReplay ? MoveToTile.x == 0xFFFF : a2->player == g_LocalNationID;
	if (v3)
	{
		MoveToTile.x = max(a2->position.x - SCREEN_WDITH / 2, 0) / 32;
		MoveToTile.y = max(a2->position.y - (SCREEN_HEIGHT - INTERFACE_HEIGHT) / 2, 0) / 32;
	}
	return 1;
}

FailStubPatch CHK_UNIT_StartLocationSub_patch(CHK_UNIT_StartLocationSub);

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
		return unit_entry->player == 11 && (IsResource(unit_entry->unit_type) || IsCritter(unit_entry->unit_type));
	}
}

FailStubPatch unitNotNeutral_patch(unitNotNeutral);

bool __stdcall ChkLoader_UNIT_(SectionData* a1, int section_size, MapChunks* a3)
{
	if (section_size % sizeof(ChunkUnitEntry))
	{
		return 0;
	}
	if (a1->start_address + a1->size > a1->next_section)
	{
		return 0;
	}

	memset(startPositions, 0, 8 * sizeof(Position));

	UnitRelated20* v18 = NULL;
	UnitRelated20* v19 = NULL;
	ChunkUnitEntry* unit_entries = (ChunkUnitEntry*)a1->start_address;
	int unit_count = section_size / sizeof(ChunkUnitEntry);
	for (int i = 0; i < unit_count; i++)
	{
		ChunkUnitEntry* unit_entry = unit_entries + i;

		if (!CHK_UNIT_StartLocationSub_(startPositions, unit_entry)
			&& (unit_entry->player >= 8u || Players[unit_entry->player].nType != PT_NotUsed && (Players[unit_entry->player].nType <= PT_Unknown0 || Players[unit_entry->player].nType == PT_Neutral))
			&& unitIsNeutral(unit_entry)
			&& (gameData.got_file_values.victory_conditions
				|| gameData.got_file_values.starting_units
				|| gameData.got_file_values.tournament_mode
				|| !getPlayerForce(unit_entry->player)
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

FailStubPatch ChkLoader_UNIT_patch(ChkLoader_UNIT);

void InitTerrainGraphicsAndCreep_(struct_a1* a1, TileID* a2, int a3, int a4, void* a5)
{
	dword_6D0E84 = a2;
	dword_6D0C74 = a1->isCreepCovered;
	dword_6D0C70 = a1->pfunc0;
	dword_6D0F08 = a3;
	dword_6D0C7C = a1->pfuncC;
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
		AppAddExit_(FreeCreepData);
	}
}

FailStubPatch InitTerrainGraphicsAndCreep_patch(InitTerrainGraphicsAndCreep);

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

FailStubPatch DoCycle_patch(DoCycle);

void __cdecl colorCycleInterval_()
{
	unsigned int v0 = DoCycle_(cycle_colors, 0, 0x80u);
	if (v0 < 8)
		DoCycle_(&cycle_colors[v0], v0, 0x100u);
}


void __cdecl updateHUDInformation_()
{
	if (byte_59723C)
	{
		updateSelectedUnitData();
		byte_59723C = 0;
	}
	updateSelectedUnitPortrait();
	updateCurrentButtonset();
	sub_458120();
	refreshScreen();
	if (dword_51BFA8)
	{
		if (ColorCycle)
		{
			if (!IS_GAME_PAUSED)
			{
				DWORD tickCount = GetTickCount();
				if (tickCount - dword_6D6374 >= 0xA)
				{
					dword_6D6374 = tickCount;
					colorCycleInterval_();
				}
			}
		}
	}
	refreshGameTextIfCounterActive();
}

FunctionPatch updateHUDInformation_patch(updateHUDInformation, updateHUDInformation_);

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
		if (GetLastError() == 38)
		{
			FileFatal(hFile, 24);
		}
		else
		{
			FileFatal(hFile, GetLastError());
		}
	}
}

void initMapData_()
{
	char filename[MAX_PATH];
	struct_a1 a1;
	int read;
	int bytes_read;

	word_6556FC = 0;
	byte_66FF5C = 0;
	MapTileArray = (TileID *)SMemAlloc(0x80000, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 603, 0);
	CellMap = (__int16*)SMemAlloc(0x20000, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 604, 0);
	GameTerrainCache = (byte *)SMemAlloc(0x49800, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 605, 0);
	active_tiles = (MegatileFlags*)SMemAlloc(0x100000, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 606, 0);
	memset(active_tiles, 0, 0x40000u);
	dword_6D5CD8 = SMemAlloc(29241, "Starcraft\\SWAR\\lang\\repulse.cpp", 323, 8);
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[(unsigned __int16)CurrentTileSet], ".wpe");
	fastFileRead_(0, 0, filename, (int)palette, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[(unsigned __int16)CurrentTileSet], ".vf4");
	MiniTileFlags = (MiniTileMaps_type *)fastFileRead_(&bytes_read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	megatileCount = (unsigned int)bytes_read >> 5;
	GenerateMegatileDefaultFlags();
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[(unsigned __int16)CurrentTileSet], ".cv5");
	TileSetMap = (TileType *)fastFileRead_(&bytes_read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	TileSetMapSize = bytes_read / 52u;
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[(unsigned __int16)CurrentTileSet], ".grp");
	a1.pfunc0 = (int(__stdcall *)(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD))sub_47E2D0;
	a1.isCreepCovered = isCreepCovered;
	a1.isTileVisible = isTileVisible;
	a1.pfuncC = 0;
	InitTerrainGraphicsAndCreep_(&a1, MapTileArray, map_size.width, map_size.height, filename);
	ZergCreepArray = location;
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".vx4");
	VX4Data = (vx4entry *)fastFileRead_(&read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".vr4");
	HANDLE v4;
	if (!SFileOpenFileEx(0, filename, 0, &v4))
	{
		int v11 = SErrGetLastError();
		SysWarn_FileNotFound(filename, v11);
		throw "Could not load tileset"; // TODO: better error reporting
	}
	LONG v5 = SFileGetFileSize(v4, 0);
	if (v5 == -1)
	{
		FileFatal(v4, GetLastError());
	}
	else
	{
		int v0 = 0;
		if (!v5)
		{
			v0 = 24;
			SysWarn_FileNotFound(filename, 24);
		}
		vr4entry *v8 = (vr4entry *)SMemAlloc(v5, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210, v0);
		HANDLE v9 = v4;
		if (SFileReadFile(v4, v8, v5, &read, 0))
		{
			if (read == v5)
			{
				SFileCloseFile(v4);
				VR4Data = v8;
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
					InitializeGameLayer();
				}
				byte_658AC0 = 0;
				dword_658AA4 = 0;
				loadParallaxStarGfx_("star");
				sub_47D660();
			}
			else
			{
				FileFatal(v9, 24);
			}
		}
		else if (GetLastError() == 38)
		{
			FileFatal(v9, 24);
		}
		else
		{
			int v10 = GetLastError();
			FileFatal(v9, v10);
		}
	}
}

FunctionPatch initMapData_patch(initMapData, initMapData_);

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
			SMemFree(chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2113, 0);
		}
		else
		{
			SMemFree(chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2104, 0);
		}
	}
}

FunctionPatch sub_4CC990_patch(sub_4CC990, sub_4CC990_);

int CreateCampaignGame__(MapData mapData)
{
	MapChunks mapChunks;
	GameData v4;

	CampaignIndex = mapData;
	int readSuccess = ReadCampaignMapData_(&mapChunks);
	if (!readSuccess) {
		return 0;
	}

	memset(&v4, 0, 140u);
	v4.got_file_values.unused3[4] = 0;
	SStrCopy(v4.player_name, playerName, 24u);
	SStrCopy(v4.map_name, CurrentMapName, 32u);
	v4.game_speed = GameSpeed;
	v4.active_human_players = 1;
	v4.max_players = 1;
	GotFileValues* v2 = InitUseMapSettingsTemplate_();
	if (v2 && (memcpy(&v4.got_file_values, v2, sizeof(v4.got_file_values)), SMemFree(v2, "Starcraft\\SWAR\\lang\\uiSingle.cpp", 270, 0), sub_4DBE50()))
	{
		isHost = 0;
		return CreateGame(&v4) != 0;
	}
	else
	{
		return 0;
	}
}

FailStubPatch CreateCampaignGame__patch(CreateCampaignGame);

bool __fastcall sub_4B6E10_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		if (evt->dwUser == EventUser::USER_CREATE)
		{
			sub_4B6930(dlg);
		}
		else if(evt->dwUser == EventUser::USER_ACTIVATE)
		{
			sub_4B6570(dlg);
			return sub_4B6D60(dlg, evt);
		}
	}
	return sub_4B6D60(dlg, evt);
}

FailStubPatch sub_4B6E10_patch(sub_4B6E10);

CampaignMenuEntry* loadmenu_GluHist_(int a1, CampaignMenuEntry* a2)
{
	if (!sub_4B6530(a2, a1))
	{
		return a2;
	}

	dword_6D5A48 = 0;
	dword_6D5A4C = a2;
	dword_6D5A50 = a1;
	dword_6D5A40 = off_51A69C;
	dword_599D98 = 28;
	HANDLE glu_hist_file;
	if (!SFileOpenFileEx(0, "rez\\gluHist.tbl", 0, &glu_hist_file))
	{
		SysWarn_FileNotFound("rez\\gluHist.tbl", SErrGetLastError());
	}
	LONG glu_hist_file_size = SFileGetFileSize(glu_hist_file, 0);
	if (glu_hist_file_size == -1)
	{
		FileFatal(glu_hist_file, GetLastError());
		return NULL;
	}
	if (!glu_hist_file_size)
	{
		SysWarn_FileNotFound("rez\\gluHist.tbl", 24);
	}
	void* v9 = SMemAlloc(glu_hist_file_size, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210, 0);
	int read;
	if (!SFileReadFile(glu_hist_file, v9, glu_hist_file_size, &read, 0))
	{
		if (GetLastError() == 38)
		{
			FileFatal(glu_hist_file, 24);
		}
		else
		{
			FileFatal(glu_hist_file, GetLastError());
		}
		return NULL;
	}
	if (read != glu_hist_file_size)
	{
		FileFatal(glu_hist_file, 24);
		return NULL;
	}
	SFileCloseFile(glu_hist_file);
	dword_6D5A44 = v9;
	AllocBackgroundImage("glue\\campaign\\pHist.pcx", &p_hist_pcx, palette, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 602);
	if (!SFileOpenFileEx(0, "rez\\gluHist.bin", 0, &glu_hist_file))
	{
		SysWarn_FileNotFound("rez\\gluHist.bin", SErrGetLastError());
		return NULL;
	}
	LONG v11 = SFileGetFileSize(glu_hist_file, 0);
	if (v11 == -1)
	{
		FileFatal(glu_hist_file, GetLastError());
		return NULL;
	}
	if (!v11)
	{
		SysWarn_FileNotFound("rez\\gluHist.bin", 24);
	}
	dialog* v14 = (dialog*)SMemAlloc(v11, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210, 0);
	if (!SFileReadFile(glu_hist_file, v14, v11, &read, 0))
	{
		if (GetLastError() == 38)
		{
			FileFatal(glu_hist_file, 24);
			return NULL;
		}
		FileFatal(glu_hist_file, GetLastError());
		return NULL;
	}
	if (read != v11)
	{
		FileFatal(glu_hist_file, 24);
		return NULL;
	}
	SFileCloseFile(glu_hist_file);
	if (v14)
	{
		v14->lFlags |= 4u;
		AllocInitDialogData(v14, v14, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 604);
		dword_6D5A3C = v14;
	}
	else
	{
		dword_6D5A3C = 0;
	}
	gluLoadBINDlg(dword_6D5A3C, sub_4B6E10_);
	if (dword_6D5A44)
	{
		SMemFree(dword_6D5A44, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 609, 0);
	}
	dword_6D5A44 = 0;
	return dword_6D5A48;
}

FailStubPatch loadmenu_GluHist_patch(loadmenu_GluHist);

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

EstablishingShot establishing_shots_[] = {
	{"Estt0tsw", (MapData) EMD_swtutorial},
	{"Estt01sw", (MapData) EMD_swterran01},
	{"Estt02sw", (MapData) EMD_swterran02},
	{"Estt03sw", (MapData) EMD_swterran03},
	{"Estt04sw", (MapData) EMD_swterran04},
	{"Estt05sw", (MapData) EMD_swterran05},
	{"EstT0t", (MapData) EMD_tutorial},
	{"EstT01", (MapData) EMD_terran01},
	{"EstT02", (MapData) EMD_terran02},
	{"EstT03", (MapData) EMD_terran03},
	{"EstT04", (MapData) EMD_terran04},
	{"EstT05", (MapData) EMD_terran05},
	{"EstT06", (MapData) EMD_terran06},
	{"EstT07", (MapData) EMD_terran07},
	{"EstT08", (MapData) EMD_terran08},
	{"EstT09", (MapData) EMD_terran09},
	{"EstT10", (MapData) EMD_terran10},
	{"EstT11", (MapData) EMD_terran11},
	{"EstT12", (MapData) EMD_terran12},
	{"EstZ01", (MapData) EMD_zerg01},
	{"EstZ02", (MapData) EMD_zerg02},
	{"EstZ03", (MapData) EMD_zerg03},
	{"EstZ04", (MapData) EMD_zerg04},
	{"EstZ05", (MapData) EMD_zerg05},
	{"EstZ06", (MapData) EMD_zerg06},
	{"EstZ07", (MapData) EMD_zerg07},
	{"EstZ08", (MapData) EMD_zerg08},
	{"EstZ09", (MapData) EMD_zerg09},
	{"EstZ10", (MapData) EMD_zerg10},
	{"EstP01", (MapData) EMD_protoss01},
	{"EstP02", (MapData) EMD_protoss02},
	{"EstP03", (MapData) EMD_protoss03},
	{"EstP04", (MapData) EMD_protoss04},
	{"EstP05", (MapData) EMD_protoss05},
	{"EstP06", (MapData) EMD_protoss06},
	{"EstP07", (MapData) EMD_protoss07},
	{"EstP08", (MapData) EMD_protoss08},
	{"EstP09", (MapData) EMD_protoss09},
	{"EstP10", (MapData) EMD_protoss10},
	{"EstP01x", (MapData) EMD_xprotoss01},
	{"EstP02x", (MapData) EMD_xprotoss02},
	{"EstP03x", (MapData) EMD_xprotoss03},
	{"EstP04x", (MapData) EMD_xprotoss04},
	{"EstP05x", (MapData) EMD_xprotoss05},
	{"EstP06x", (MapData) EMD_xprotoss06},
	{"EstP07x", (MapData) EMD_xprotoss07},
	{"EstP08x", (MapData) EMD_xprotoss08},
	{"EstT01x", (MapData) EMD_xterran01},
	{"EstT02x", (MapData) EMD_xterran02},
	{"EstT03x", (MapData) EMD_xterran03},
	{"EstT04x", (MapData) EMD_xterran04},
	{"EstT05ax", (MapData) EMD_xterran05a},
	{"EstT05bx", (MapData) EMD_xterran05b},
	{"EstT06x", (MapData) EMD_xterran06},
	{"EstT07x", (MapData) EMD_xterran07},
	{"EstT08x", (MapData) EMD_xterran08},
	{"EstZ01x", (MapData) EMD_xzerg01},
	{"EstZ02x", (MapData) EMD_xzerg02},
	{"EstZ03x", (MapData) EMD_xzerg03},
	{"EstZ04x", (MapData) EMD_xzerg04a},
	{"EstZ04x", (MapData) EMD_xzerg04b},
	{"EstZ04x", (MapData) EMD_xzerg04c},
	{"EstZ04x", (MapData) EMD_xzerg04d},
	{"EstZ04x", (MapData) EMD_xzerg04e},
	{"EstZ04x", (MapData) EMD_xzerg04f},
	{"EstZ05x", (MapData) EMD_xzerg05},
	{"EstZ06x", (MapData) EMD_xzerg06},
	{"EstZ07x", (MapData) EMD_xzerg07},
	{"EstZ08x", (MapData) EMD_xzerg08},
	{"EstZ09x", (MapData) EMD_xzerg09},
	{"EstZ09bx", (MapData) EMD_xbonus},
	{"EstZ10x", (MapData) EMD_xzerg10},
	{"FinZ09bx", (MapData) EMD_xbonus},
};

CampaignMenuEntry terran_swcampaign_menu_entries_[] = {
	{0x33, (MapData)ExpandedMapData::EMD_swtutorial, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x34, (MapData)ExpandedMapData::EMD_swterran01, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x35, (MapData)ExpandedMapData::EMD_swterran02, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x36, (MapData)ExpandedMapData::EMD_swterran03, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x37, (MapData)ExpandedMapData::EMD_swterran04, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x38, (MapData)ExpandedMapData::EMD_swterran05, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0, (MapData)ExpandedMapData::EMD_none, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
};

CampaignMenuEntry zerg_campaign_menu_entries_[] = {
	{0x20, (MapData) ExpandedMapData::EMD_zerg01, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x21, (MapData) ExpandedMapData::EMD_zerg02, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x22, (MapData) ExpandedMapData::EMD_zerg03, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x2A, (MapData) ExpandedMapData::EMD_zerg04, C_THE_DREAM, 0, RACE_Zerg, 0},
	{0x23, (MapData) ExpandedMapData::EMD_zerg04, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x24, (MapData) ExpandedMapData::EMD_zerg05, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x2B, (MapData) ExpandedMapData::EMD_zerg06, C_BATTLE_ON_THE_AMERIGO, 0, RACE_Zerg, 0},
	{0x25, (MapData) ExpandedMapData::EMD_zerg06, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x26, (MapData) ExpandedMapData::EMD_zerg07, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x27, (MapData) ExpandedMapData::EMD_zerg08, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x2C, (MapData) ExpandedMapData::EMD_zerg09, C_THE_WARP, 0, RACE_Zerg, 0},
	{0x28, (MapData) ExpandedMapData::EMD_zerg09, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x29, (MapData) ExpandedMapData::EMD_zerg10, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x2D, (MapData) ExpandedMapData::EMD_protoss01, C_THE_INVASION_OF_AIUR, 0, RACE_Zerg, 0},
	{0, (MapData)ExpandedMapData::EMD_none, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
};

CampaignMenuEntry terran_campaign_menu_entries_[] = {
	{1, (MapData) ExpandedMapData::EMD_tutorial, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{2, (MapData) ExpandedMapData::EMD_terran01, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{3, (MapData) ExpandedMapData::EMD_terran02, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0xC, (MapData)ExpandedMapData::EMD_terran03, C_WASTELAND_PATROL, 0, RACE_Terran, 0},
	{4, (MapData) ExpandedMapData::EMD_terran03, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{5, (MapData) ExpandedMapData::EMD_terran04, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{6, (MapData) ExpandedMapData::EMD_terran05, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0xD, (MapData)ExpandedMapData::EMD_terran06, C_THE_DOWNING_OF_NORAD_II, 0, RACE_Terran, 0},
	{7, (MapData) ExpandedMapData::EMD_terran06, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{8, (MapData) ExpandedMapData::EMD_terran08, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{9, (MapData) ExpandedMapData::EMD_terran09, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0xE, (MapData) ExpandedMapData::EMD_terran11, C_OPEN_REBELION, 0, RACE_Terran, 0},
	{0xA, (MapData) ExpandedMapData::EMD_terran11, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0xB, (MapData) ExpandedMapData::EMD_terran12, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0xF, (MapData) ExpandedMapData::EMD_zerg01, C_THE_INAUGURATION, 0, RACE_Terran, 0},
	{0, (MapData)ExpandedMapData::EMD_none, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
};

CampaignMenuEntry protoss_campaign_menu_entries_[] = {
	{0x11, (MapData) ExpandedMapData::EMD_protoss01, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x12, (MapData) ExpandedMapData::EMD_protoss02, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x13, (MapData) ExpandedMapData::EMD_protoss03, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x1B, (MapData) ExpandedMapData::EMD_protoss04, C_THE_FALL_OF_FENIX, 0, RACE_Protoss, 0},
	{0x14, (MapData) ExpandedMapData::EMD_protoss04, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x15, (MapData) ExpandedMapData::EMD_protoss05, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x1C, (MapData) ExpandedMapData::EMD_protoss06, C_THE_AMBUSH, 0, RACE_Protoss, 0},
	{0x16, (MapData) ExpandedMapData::EMD_protoss06, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x17, (MapData) ExpandedMapData::EMD_protoss07, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x1D, (MapData) ExpandedMapData::EMD_protoss08, C_THE_RETURN_TO_AIUR, 0, RACE_Protoss, 0},
	{0x18, (MapData) ExpandedMapData::EMD_protoss08, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x19, (MapData) ExpandedMapData::EMD_protoss09, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x1A, (MapData) ExpandedMapData::EMD_protoss10, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x1E, (MapData) ExpandedMapData::EMD_xprotoss01, C_THE_DEATH_OF_THE_OVERMIND, 0, RACE_Protoss, 0},
	{0, (MapData)ExpandedMapData::EMD_none, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
};

CampaignMenuEntry* campaign_menu_entries_[] = {
	zerg_campaign_menu_entries_,
	terran_campaign_menu_entries_,
	protoss_campaign_menu_entries_,
};

CampaignMenuEntry zerg_expcampaign_menu_entries_[] = {
	{0x4A, (MapData) ExpandedMapData::EMD_xzerg01, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x4B, (MapData) ExpandedMapData::EMD_xzerg02, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x4C, (MapData) ExpandedMapData::EMD_xzerg03, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x4D, (MapData) ExpandedMapData::EMD_xzerg04a, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0, (MapData) ExpandedMapData::EMD_xzerg04b, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1},
	{0, (MapData) ExpandedMapData::EMD_xzerg04c, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1},
	{0, (MapData) ExpandedMapData::EMD_xzerg04d, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1},
	{0, (MapData) ExpandedMapData::EMD_xzerg04e, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1},
	{0, (MapData) ExpandedMapData::EMD_xzerg04f, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1},
	{0x4E, (MapData) ExpandedMapData::EMD_xzerg05, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x4F, (MapData) ExpandedMapData::EMD_xzerg06, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x50, (MapData) ExpandedMapData::EMD_xzerg07, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x51, (MapData) ExpandedMapData::EMD_xzerg08, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x52, (MapData) ExpandedMapData::EMD_xzerg09, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0, (MapData)ExpandedMapData::EMD_xbonus, C_BLIZZARD_LOGO, 0, RACE_Zerg, 1},
	{0x54, (MapData) ExpandedMapData::EMD_xzerg10, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
	{0x55, (MapData) ExpandedMapData::EMD_Unknown, C_THE_ASCENTION, 0, RACE_Zerg, 0},
	{0, (MapData)ExpandedMapData::EMD_none, C_BLIZZARD_LOGO, 0, RACE_Zerg, 0},
};

CampaignMenuEntry terran_expcampaign_menu_entries_[] = {
	{0x41, (MapData) ExpandedMapData::EMD_xterran01, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x42, (MapData) ExpandedMapData::EMD_xterran02, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x43, (MapData) ExpandedMapData::EMD_xterran03, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x44, (MapData) ExpandedMapData::EMD_xterran04, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x45, (MapData) ExpandedMapData::EMD_xterran05a, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x46, (MapData) ExpandedMapData::EMD_xterran05b, C_BLIZZARD_LOGO, 0, RACE_Terran, 1},
	{0x47, (MapData) ExpandedMapData::EMD_xterran06, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x48, (MapData) ExpandedMapData::EMD_xterran07, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x49, (MapData) ExpandedMapData::EMD_xterran08, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
	{0x57, (MapData) ExpandedMapData::EMD_xzerg01, C_UED_VICTORY_REPORT, 0, RACE_Terran, 0},
	{0, (MapData)ExpandedMapData::EMD_none, C_BLIZZARD_LOGO, 0, RACE_Terran, 0},
};

CampaignMenuEntry protoss_expcampaign_menu_entries_[] = {
	{0x39, (MapData) ExpandedMapData::EMD_xprotoss01, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x3A, (MapData) ExpandedMapData::EMD_xprotoss02, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x3B, (MapData) ExpandedMapData::EMD_xprotoss03, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x3C, (MapData) ExpandedMapData::EMD_xprotoss04, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x3D, (MapData) ExpandedMapData::EMD_xprotoss05, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x3E, (MapData) ExpandedMapData::EMD_xprotoss06, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x3F, (MapData) ExpandedMapData::EMD_xprotoss07, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x40, (MapData) ExpandedMapData::EMD_xprotoss08, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
	{0x56, (MapData) ExpandedMapData::EMD_xterran01, C_FURY_OF_THE_XEL_NAGA, 0, RACE_Protoss, 0},
	{0, (MapData)ExpandedMapData::EMD_none, C_BLIZZARD_LOGO, 0, RACE_Protoss, 0},
};

CampaignMenuEntry* expcampaign_menu_entries_[] = {
	zerg_expcampaign_menu_entries_,
	terran_expcampaign_menu_entries_,
	protoss_expcampaign_menu_entries_,
};

struct Campaign
{
	const char* campaign_id;
	int first_mission_index;
	bool is_expansion;
	Race race;
	CampaignMenuEntry* entries;
};

std::vector<Campaign> campaigns = {
	{"swterran", 0, false, Race::RACE_Terran, terran_swcampaign_menu_entries_},
	{"terran", 0, false, Race::RACE_Terran, terran_campaign_menu_entries_},
	{"zerg", 1, false, Race::RACE_Zerg, zerg_campaign_menu_entries_},
	{"protoss", 1, false, Race::RACE_Protoss, protoss_campaign_menu_entries_},
	{"xprotoss", 1, true, Race::RACE_Protoss, protoss_expcampaign_menu_entries_},
	{"xterran", 1, true, Race::RACE_Terran, terran_expcampaign_menu_entries_},
	{"xzerg", 1, true, Race::RACE_Zerg, zerg_expcampaign_menu_entries_},
};

int parseCmpgnCheatTypeString_(Campaign* campaign, char* campaign_index, MapData* a5)
{
	char* campaign_index_ = campaign_index;
	int v5 = strtoul(campaign_index, &campaign_index_, 10) - campaign->first_mission_index;
	if (v5 < 0)
	{
		return 0;
	}

	CampaignMenuEntry* v8 = campaign->entries;
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

FailStubPatch parseCmpgnCheatTypeString_patch(parseCmpgnCheatTypeString);

void ContinueCampaignWithLevelCheat_(MapData mission, bool is_expansion, Race race)
{
	Ophelia = 1;
	level_cheat_mission = (MapData4)mission;
	level_cheat_race = race;
	level_cheat_is_bw = is_expansion;
}

FailStubPatch ContinueCampaignWithLevelCheat_patch(ContinueCampaignWithLevelCheat);

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

	MapData mission;
	int prefix_length = SStrLen(relevant_campaign->campaign_id);
	if (parseCmpgnCheatTypeString_(relevant_campaign, a2 + prefix_length, &mission) && mission != MD_xbonus)
	{
		ContinueCampaignWithLevelCheat_(mission, relevant_campaign->is_expansion, relevant_campaign->race);
		if (gwGameMode == GAME_RUN)
		{
			GameState = 0;
			gwNextGameMode = GAME_GLUES;
			if (!InReplay)
			{
				ReplayFrames = ElapsedTimeFrames;
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

FunctionPatch campaignTypeCheatStrings_patch((void*) 0x4b1dc0, campaignTypeCheatStrings__);

CampaignMenuEntry* getCampaignIndex_(Campaign& campaign)
{
	CampaignMenuEntry* entry = campaign.entries;

	while (entry->cinematic || entry->next_mission != CampaignIndex)
	{
		++entry;
		if (entry->next_mission == MD_none)
		{
			return 0;
		}
	}
	return entry;
}

FailStubPatch getCampaignIndex_patch(getCampaignIndex);

void updateActiveCampaignMission_()
{
	if (active_campaign_menu_entry == NULL || active_campaign_menu_entry->next_mission != CampaignIndex)
	{
		for (Campaign& campaign : campaigns)
		{
			for (CampaignMenuEntry* entry = campaign.entries; entry->next_mission; entry++)
			{
				if (entry->next_mission == CampaignIndex)
				{
					active_campaign_menu_entry = getCampaignIndex_(campaign);
					return;
				}
			}
		}
	}
}

FunctionPatch updateActiveCampaignMission_patch(updateActiveCampaignMission, updateActiveCampaignMission_);

bool LoadCampaignWithCharacter_(Race race)
{
	customSingleplayer[0] = 0;
	dword_51CA1C = 0;
	CharacterData character_data;
	if (!LoadCharacterData(&character_data, playerName))
	{
		const char* v1 = *networkTable > 71u ? (const char*)networkTable + networkTable[72] : "";
		if ((_stricmp(playerName, v1) || !verifyCharacterFile(&character_data, playerName)) && !outOfGame)
		{
			doNetTBLError(0, 0, 0, 88);
		}
	}

	CampaignMenuEntry* v2;
	int* unlocked_mission;
	if (IsExpansion)
	{
		v2 = expcampaign_menu_entries_[race];
		unlocked_mission = &character_data.unlocked_expcampaign_mission[race];
	}
	else
	{
		v2 = campaign_menu_entries_[race];
		unlocked_mission = &character_data.unlocked_campaign_mission[race];
	}
	active_campaign_menu_entry = loadmenu_GluHist_(*unlocked_mission, v2);
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
			CreateCampaignGame__(active_campaign_menu_entry->next_mission);
		}
	}
	return active_campaign_menu_entry != NULL;
}

FailStubPatch LoadCampaignWithCharacter_patch(LoadCampaignWithCharacter);

int sub_4B5110_(Race race)
{
	if (dword_59A0D4[race])
	{
		int v2 = (race != Race::RACE_Terran) + 142;
		const char* v3 = ((race != Race::RACE_Terran) != ~142) ? (v2 < *networkTable ? (char*)networkTable + networkTable[v2 + 1] : "") : NULL;

		if (!sub_4B5B20(v3))
		{
			return 0;
		}
	}

	return LoadCampaignWithCharacter_(race);
}

FailStubPatch sub_4B5110_patch(sub_4B5110);

bool sub_4B27A0_(Race race)
{
	if (dword_59B760[race])
	{
		int v2 = (race == Race::RACE_Protoss) + 140;
		const char* v3 = ((race == Race::RACE_Protoss) != -141) ? (v2 < *networkTable ? (char*)networkTable + networkTable[v2 + 1] : "") : NULL;

		if (!sub_4B5B20(v3))
		{
			return 0;
		}
	}

	return LoadCampaignWithCharacter_(race);
}

FailStubPatch sub_4B27A0_patch(sub_4B27A0);

bool sub_4B5180_(dialog* a1)
{
	if (LastControlID == 6)
	{
		if (!sub_4B5110_(Race::RACE_Protoss))
			return 1;
		LastControlID = 6;
	}
	else if (LastControlID == 7)
	{
		if (sub_4B5110_(Race::RACE_Terran))
		{
			LastControlID = 7;
			return DLG_SwishOut(a1);
		}
		return 1;
	}
	else if (LastControlID == 8)
	{
		if (sub_4B5110_(Race::RACE_Zerg))
		{
			LastControlID = 8;
			return DLG_SwishOut(a1);
		}
		return 1;
	}

	return DLG_SwishOut(a1);
}

FailStubPatch sub_4B5180_patch(sub_4B5180);

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
		if (sub_4B27A0_(Race::RACE_Terran))
		{
			LastControlID = 7;
			return DLG_SwishOut(a1);
		}
		return true;
	case 8:
		if (sub_4B27A0_(Race::RACE_Zerg))
		{
			LastControlID = 8;
			return DLG_SwishOut(a1);
		}
		return true;
	}
	return DLG_SwishOut(a1);
}

FailStubPatch sub_4B2810_patch(sub_4B2810);

bool __fastcall gluCmpgn_Main_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		if (evt->dwUser == EventUser::USER_CREATE)
		{
			DLG_SwishIn(dlg);
			if (!byte_6D5BBC)
			{
				DLGMusicFade(22);
			}
		}
		else if (evt->dwUser == EventUser::USER_ACTIVATE)
		{
			return sub_4B2810_(dlg);
		}
		else if (evt->dwUser == EventUser::USER_INIT)
		{
			registerMenuFunctions(off_51A93C, dlg, 44, 0);
			DlgSwooshin(2, gluCmpgnSwishController, dlg, 0);
		}
	}
	return genericDlgInteract(dlg, evt);
}

FailStubPatch gluCmpgn_Main_patch(gluCmpgn_Main);

bool __fastcall gluExpCmpgn_CustomCtrlID_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		if (evt->dwUser == EventUser::USER_CREATE)
		{
			DLG_SwishIn(dlg);
			if (!byte_6D5BBC)
				DLGMusicFade(22);
		}
		else if (evt->dwUser == EventUser::USER_ACTIVATE)
		{
				return sub_4B5180_(dlg);
		}
		else if (evt->dwUser == EventUser::USER_INIT)
		{
			registerMenuFunctions(off_51A818, dlg, 44, 0);
			DlgSwooshin(2, &stru_512A8C, dlg, 0);
		}
	}
	return genericDlgInteract(dlg, evt);
}

FailStubPatch gluExpCmpgn_CustomCtrlID_patch(gluExpCmpgn_CustomCtrlID);

void loadMenu_gluCmpgn_()
{
	OpheliaEnabled = GLUE_MAIN_MENU;
	multiPlayerMode = GLUE_MAIN_MENU;
	sub_4B26E0();
	dialog* campaign_dialog = loadFullMenuDLG("rez\\gluCmpgn.bin", 0, GLUE_MAIN_MENU, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
	if (campaign_dialog)
	{
		campaign_dialog->lFlags |= 4u;
		AllocInitDialogData(campaign_dialog, campaign_dialog, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
	}
	switch (gluLoadBINDlg(campaign_dialog, gluCmpgn_Main_))
	{
	case 5:
		glGluesMode = GLUE_LOAD;
		break;
	case 6:
		glGluesMode = GLUE_READY_P;
		break;
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

FailStubPatch loadMenu_gluCmpgn_patch(loadMenu_gluCmpgn);

void loadMenu_gluExpCmpgn_()
{
	OpheliaEnabled = 0;
	multiPlayerMode = 0;
	sub_4B5050();
	dialog* campaign_dialog = loadFullMenuDLG("rez\\gluExpCmpgn.bin", 0, GLUE_MAIN_MENU, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
	if (campaign_dialog)
	{
		campaign_dialog->lFlags |= 4u;
		AllocInitDialogData(campaign_dialog, campaign_dialog, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
	}
	switch (gluLoadBINDlg(campaign_dialog, gluExpCmpgn_CustomCtrlID_))
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

FailStubPatch loadMenu_gluExpCmpgn_patch(loadMenu_gluExpCmpgn);

int loadMenu_gluCustm_(int is_multiplayer)
{
	char v1; // bl@1
	char *v2; // ecx@2
	dialog *v3; // eax@3
	dialog *v4; // esi@3
	int v5; // eax@7
	int result; // eax@37

	dword_59B844 = is_multiplayer;
	v1 = 0;
	if (LOBYTE(multiPlayerMode))
		v2 = "rez\\gluCreat.bin";
	else
		v2 = "rez\\gluCustm.bin";
	v3 = (dialog *)loadFullMenuDLG(v2, 0, 0, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
	v4 = v3;
	if (v3)
	{
		v3->lFlags |= 4u;
		AllocInitDialogData(v3, v3, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
		v1 = 0;
	}
	else
	{
		v4 = 0;
	}
	dword_6D5A70 = v4;
	dword_59BA60 = (void *)LoadGraphic("glue\\create\\iCreate.grp", 0, "Starcraft\\SWAR\\lang\\gluCreat.cpp", 1427);
	dword_6D5A74 = GAME_RUNINIT;
	v5 = gluLoadBINDlg(dword_6D5A70, gluCustm_Interact);
	if (v5 != -3)
	{
		if (v5 == 12)
		{
			if (LOBYTE(multiPlayerMode) == v1)
			{
				if (gameData.got_file_values.victory_conditions != v1
					|| gameData.got_file_values.starting_units != v1
					|| gameData.got_file_values.tournament_mode != v1
					|| InReplay)
				{
					gwGameMode = GAME_RUNINIT;
				}
				else if (Players[g_LocalNationID].nRace == 0)
				{
					glGluesMode = GLUE_READY_Z;
				}
				else if (Players[g_LocalNationID].nRace == 1)
				{
					glGluesMode = GLUE_READY_T;
				}
				else if (Players[g_LocalNationID].nRace == 2)
				{
					glGluesMode = GLUE_READY_P;
				}
			}
			else
			{
				glGluesMode = GLUE_CHAT;
			}
			goto LABEL_37;
		}
		if (v5 != 13)
		{
			glGluesMode = MenuPosition::GLUE_MAIN_MENU;
			goto LABEL_37;
		}
	}
	InReplay = 0;
	if (scenarioChk)
	{
		SMemFree(scenarioChk, "Starcraft\\SWAR\\lang\\replay.cpp", 1106, 0);
		scenarioChk = 0;
	}
	if (!LOBYTE(multiPlayerMode))
	{
		glGluesMode = IsExpansion != 0 ? GLUE_EX_CAMPAIGN : GLUE_CAMPAIGN;
		goto LABEL_37;
	}
	if ((unsigned int)NetMode.as_number <= 0x4D4F444D)
	{
		if (NetMode.as_number == 0x4D4F444D)
		{
		LABEL_32:
			glGluesMode = MenuPosition::GLUE_MODEM;
			goto LABEL_37;
		}
		if (NetMode.as_number == 1112425812)
		{
			glGluesMode = MenuPosition::GLUE_BATTLE;
			goto LABEL_37;
		}
		if (NetMode.as_number == 1296321880)
			goto LABEL_32;
		goto LABEL_35;
	}
	if (NetMode.as_number != 1396916812)
	{
	LABEL_35:
		glGluesMode = MenuPosition::GLUE_GAME_SELECT;
		goto LABEL_37;
	}
	glGluesMode = MenuPosition::GLUE_DIRECT;
LABEL_37:
	changeMenu();
	result = (int)dword_59BA60;
	if (dword_59BA60)
		result = SMemFree(dword_59BA60, "Starcraft\\SWAR\\lang\\gluCreat.cpp", 1484, v1);
	dword_6D5A74 = 0;
	return result;
}

int SwitchMenu_()
{
	if (!GetModuleFileNameA(0u, main_directory, MAX_PATH))
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
	init_gluesounds();
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
			char* v3;
			if (*networkTable > 71u)
				v3 = (char *)networkTable + networkTable[72];
			else
				v3 = "";
			SStrCopy(playerName, v3, 25u);
		}
		customSingleplayer[0] = 0;
		IsExpansion = level_cheat_is_bw != 0;
		if (level_cheat_is_bw && !dword_6D11E4 || !loadCampaignBIN() || !CreateCampaignGame__((MapData)level_cheat_mission))
		{
			glGluesMode = MenuPosition::GLUE_MAIN_MENU;
			IsExpansion = 0;
			goto LABEL_28;
		}
		if (level_cheat_race)
		{
			if (level_cheat_race != 1)
			{
				if (level_cheat_race == 2)
				{
					glGluesMode = GLUE_READY_P;
					goto LABEL_26;
				}
				glGluesMode = MenuPosition::GLUE_MAIN_MENU;
				IsExpansion = 0;
				goto LABEL_28;
			}
			glGluesMode = GLUE_READY_T;
		}
		else
		{
			glGluesMode = GLUE_READY_Z;
		}
	LABEL_26:
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
		BWFXN_RedrawTarget();
	}
	RefreshCursor_0();
	dword_6D5E38 = (int(__thiscall *)(_DWORD))jmpNoMenu;
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
			loadMenu_gluMain();
			break;
		case GLUE_LOGIN:
			dword_51C414 = 0;
			loadMenu_gluLogin();
			break;
		case GLUE_CAMPAIGN:
			loadMenu_gluCmpgn_();
			break;
		case GLUE_READY_T:
			loadMenu_gluRdyT();
			break;
		case GLUE_READY_Z:
			loadMenu_gluRdyZ();
			break;
		case GLUE_READY_P:
			loadMenu_gluRdyP();
			break;
		case GLUE_EX_CAMPAIGN:
			loadMenu_gluExpCmpgn_();
			break;
		case GLUE_CONNECT:
			loadMenu_gluConn();
			break;
		case GLUE_MODEM:
			loadMenu_gluModem();
			break;
		case GLUE_GAME_SELECT:
		case GLUE_DIRECT:
			loadMenu_gluJoin();
			break;
		case GLUE_CREATE:
			loadMenu_gluCustm_(0);
			break;
		case GLUE_CREATE_MULTI:
			loadMenu_gluCustm_(1);
			break;
		case GLUE_CHAT:
			loadMenu_gluChat();
			break;
		case GLUE_LOAD:
			loadMenu_gluLoad();
			break;
		case GLUE_SCORE_Z_DEFEAT:
		case GLUE_SCORE_Z_VICTORY:
		case GLUE_SCORE_T_DEFEAT:
		case GLUE_SCORE_T_VICTORY:
		case GLUE_SCORE_P_DEFEAT:
		case GLUE_SCORE_P_VICTORY:
			loadMenu_gluScore();
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
		BWFXN_RedrawTarget();
	}
	RefreshCursor_0();
	playsound_init_UI(0);
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

FailStubPatch SwitchMenu_patch(SwitchMenu);

void CreateMainWindow_()
{
	WNDCLASSEXA window_class;

	memset(&window_class, 0, sizeof(window_class));
	window_class.cbSize = sizeof(window_class);
	window_class.style = 8;
	window_class.lpfnWndProc = (WNDPROC)MainWindowProc;
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

	const char* v0 = dword_6D11E4 ? "Brood War" : "Starcraft";
	int v3 = GetSystemMetrics(1);
	int v1 = GetSystemMetrics(0);
	hWndParent = CreateWindowExA(0, "SWarClass", v0, 0x90080000, 0, 0, v1, v3, 0, 0, hInst, 0);
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

void PlayMovieWithIntro_(Cinematic a1)
{
	int v1 = 10;
	while (v1--)
	{
		if (cinematic_intros[v1].actual_cinematic == a1)
		{
			PlayMovie(cinematic_intros[v1].intro_cinematic);
			break;
		}
	}
	if (gwGameMode != GAME_EXIT)
	{
		PlayMovie(a1);
	}
}

FailStubPatch PlayMovieWithIntro_patch(PlayMovieWithIntro);

void playActiveCinematic_()
{
	PlayMovieWithIntro_(active_cinematic);
	active_cinematic = Cinematic::C_NONE;
}

FailStubPatch playActiveCinematic_patch(playActiveCinematic);

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

FailStubPatch sub_4DBD20_patch(sub_4DBD20);

CampaignMenuEntry* sub_4DBDA0_(const char* a1)
{
	MapData v6;

	char* v2 = SStrChrR(a1, '.');
	if (!v2)
	{
		return active_campaign_menu_entry;
	}

	size_t v4 = v2 - a1;
	CampaignMenuEntry** entries = SStrCmpI(v2, ".SCM", 0x7FFFFFFFu) ? expcampaign_menu_entries_ : campaign_menu_entries_;
	int v8 = 0;
	if (sub_4DBD20_(a1, v4, &v8))
	{
		while (2)
		{
			for (int i = 0; i < 3; ++i)
			{
				CampaignMenuEntry* result = entries[i];
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
	return active_campaign_menu_entry;
}

int __stdcall ContinueCampaign_(int a1)
{
	gwGameMode = GAME_GLUES;
	if (dword_51CA1C)
	{
		CreateNextCampaignGame();
		return 1;
	}
	if (!a1)
	{
		gwGameMode = GAME_RESTART;
		return 1;
	}
	sub_4D91B0();
	updateActiveCampaignMission_();
	if (!active_campaign_menu_entry || active_campaign_menu_entry->next_mission == MD_none)
	{
		return 0;
	}
	if (next_scenario[0])
	{
		active_campaign_menu_entry = sub_4DBDA0_(next_scenario);
		next_scenario[0] = 0;
	}
	else
	{
		active_campaign_menu_entry += 1;
	}
	sub_4DBEE0(active_campaign_menu_entry);
	if (active_campaign_menu_entry->next_mission)
	{
		if (active_campaign_menu_entry->cinematic)
		{
			CampaignIndex = active_campaign_menu_entry->next_mission;
			active_cinematic = active_campaign_menu_entry->cinematic;
			gwGameMode = GAME_CINEMATIC;
			return 1;
		}
		if (CreateCampaignGame__(active_campaign_menu_entry->next_mission))
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

	if (IsExpansion)
	{
		if (active_campaign_menu_entry->race == Race::RACE_Zerg)
		{
			gwGameMode = GAME_EPILOG;
		}
		else
		{
			glGluesMode = GLUE_EX_CAMPAIGN;
		}
	}
	else
	{
		if (active_campaign_menu_entry->race == Race::RACE_Protoss)
		{
			gwGameMode = GAME_EPILOG;
		}
		else
		{
			glGluesMode = GLUE_CAMPAIGN;
		}
	}
	active_campaign_menu_entry = 0;
	return 1;
}

FunctionPatch ContinueCampaign_patch(ContinueCampaign, ContinueCampaign_);

void BeginEpilog_()
{
	int v0 = dword_6CDFE0;
	if (!dword_6CDFE0 && dword_6CDFE4)
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
		dword_6CDFE0 = 50;
	}
	DLGMusicFade(9);
	loadInitCreditsBIN(IsExpansion ? "epilogX" : "epilog");
	if (gwGameMode == GAME_EPILOG)
	{
		loadInitCreditsBIN(IsExpansion ? "crdt_exp" : "crdt_lst");
	}
	stopMusic();
	dword_6CDFE0 = v0;
}

FailStubPatch BeginEpilog_patch(BeginEpilog);

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
	SetCursorPos(320, 240);
	Mouse.x = 320;
	Mouse.y = 240;
	if (hMpq && SFileOpenFileEx(hMpq, "rez\\gluexpcmpgn.bin", 0, &phFile))
	{
		SFileCloseFile(phFile);
		if ((dword_6CDFEC & 0x800) == 0)
		{
		LABEL_8:
			LoadTitle_();
			LoadInitIscriptBIN_();
			AppAddExit_(CleanupIscriptBINHandle);
			if (gwGameMode != GAME_GLUES && load_screen)
			{
				DestroyDialog(load_screen);
				load_screen = NULL;
			}
			while (1)
			{
				if (dword_596B70 != GAME_CINEMATIC)
				{
					dword_596B70 = GAME_CINEMATIC;
					if (dword_597394 != dword_596B48)
					{
						dword_597394 = dword_596B48;
						drawCursor();
					}
				}
				BWFXN_RedrawTarget();
				if (!dword_6D11E4)
					IsExpansion = GLUE_MAIN_MENU;
				switch (gwGameMode)
				{
				case GAME_RUN:
					GameRun_(GLUE_MAIN_MENU);
					continue;
				case GAME_CINEMATIC:
					playActiveCinematic_();
					if (gwGameMode == GAME_CINEMATIC)
						ContinueCampaign_(1);
					continue;
				case GAME_RESTART:
					next_campaign_mission = 1;
					goto LABEL_23;
				case GAME_RUNINIT:
				LABEL_23:
					gwGameMode = GAME_RUN;
					continue;
				case GAME_INTRO:
					PlayMovie(Cinematic::C_BLIZZARD_LOGO);
					if (gwGameMode == GAME_INTRO)
					{
						if (hMpq && SFileOpenFileEx(hMpq, "rez\\gluexpcmpgn.bin", GLUE_MAIN_MENU, &phFile))
						{
							SFileCloseFile(phFile);
							PlayMovie(Cinematic::C_EXPANSION_INTRO);
						}
						else
						{
							PlayMovie(Cinematic::C_INTRO);
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
					BeginCredits();
					if (gwGameMode == GAME_CREDITS)
					{
						gwGameMode = GAME_GLUES;
						glGluesMode = GLUE_MAIN_MENU;
					}
					break;
				case GAME_EPILOG:
					BeginEpilog_();
					if (gwGameMode == GAME_EPILOG)
					{
						gwGameMode = GAME_GLUES;
						glGluesMode = GLUE_MAIN_MENU;
					}
					break;
				default:
					AppExit(0);
					ProcError(1);
					exit(0);
					return;
				}
			}
		}
		dword_6CDFEC &= ~0x800u;
		PlayMovie(Cinematic::C_BLIZZARD_LOGO);
		PlayMovie(Cinematic::C_EXPANSION_INTRO);
	}
	else
	{
		if ((dword_6CDFEC & 0x200) == 0)
			goto LABEL_8;
		dword_6CDFEC &= ~0x200u;
		PlayMovie(Cinematic::C_BLIZZARD_LOGO);
		PlayMovie(Cinematic::C_INTRO);
	}
	goto LABEL_8;
}

unsigned __int32 LocalGetLang_()
{
	CHAR Buffer[16];
	char *v2;

	if (hModule && LoadStringA(hModule, 3u, Buffer, 16) || LoadStringA(hInst, 3u, Buffer, 16))
		return strtoul(Buffer, &v2, 16);
	else
		return 1033;
}

void localDll_Init_(HINSTANCE a1)
{
	char Filename[MAX_PATH];

	if (!GetModuleFileNameA(a1, Filename, MAX_PATH))
		Filename[0] = 0;
	char* v1 = strrchr(Filename, '\\');
	if (!v1)
		v1 = &Filename[-1];
	char* v2 = "local.dll";
	int i = 0;
	while (true)
	{
		v1[i + 1] = v2[i];
		if (v2[i] == '\0')
			break;
		++i;
	};
	hModule = LoadLibraryA(Filename);
	if (!hModule)
	{
		DialogBoxParamA(a1, (LPCSTR)106, hWndParent, LocalErrProc, (LPARAM)Filename);
		SErrSuppressErrors(1);
		AppExit(1);
		ProcError(1);
		exit(1);
	}
	int local_lang = LocalGetLang_();
	SFileSetLocale(local_lang);
	AppAddExit_(FreeLocalDLL);
}

void loadInitCreditsBIN_(char* a1)
{
	LONG v3; // esi MAPDST
	int v6; // eax
	char buff[MAX_PATH]; // [esp+Ch] [ebp-10Ch] BYREF
	int read; // [esp+110h] [ebp-8h] BYREF
	HANDLE phFile; // [esp+114h] [ebp-4h] MAPDST BYREF

	_snprintf(buff, MAX_PATH, "rez\\%s.txt", a1);

	dword_51CEA8 = (char*)fastFileRead_(&bytes_read, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	dword_51CEBC = dword_51CEA8;
	dword_51CEB8 = bytes_read;
	dword_51CEC0 = 0;
	if (!SFileOpenFileEx(0, "rez\\credits.bin", 0, &phFile))
	{
		v6 = SErrGetLastError();
		SysWarn_FileNotFound("rez\\credits.bin", v6);
		throw std::exception("Could not find the credits file");
	}
	v3 = SFileGetFileSize(phFile, 0);
	if (v3 == -1)
	{
		FileFatal(phFile, GetLastError());
		return;
	}
	if (!v3)
		SysWarn_FileNotFound("rez\\credits.bin", 24);
	dialog* v5 = (dialog*)SMemAlloc(v3, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210, 0);
	if (!SFileReadFile(phFile, v5, v3, &read, 0))
	{
		FileFatal(phFile, GetLastError() == 38 ? 24 : GetLastError());
		return;
	}
	if (read != v3)
	{
		FileFatal(phFile, 24);
		return;
	}
	SFileCloseFile(phFile);
	if (v5)
	{
		v5->lFlags |= 4u;
		AllocInitDialogData(v5, v5, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\credits.cpp", 618);
	}
	else
	{
		v5 = 0;
	}
	gluLoadBINDlg(v5, creditsDlgInteract);
	if (dword_51CEA8)
		SMemFree(dword_51CEA8, "Starcraft\\SWAR\\lang\\credits.cpp", 623, 0);
	dword_51CEA8 = 0;
	dword_51CEBC = 0;
	dword_51CEB0 = 0;
}

void __cdecl sub_4D9200_()
{
	if (!multiPlayerMode && !(GameCheats & CHEAT_NoGlues) && CampaignIndex)
	{
		for (int i = 0; i < 64; i++)
		{
			if (establishing_shots_[i].campaign_mission == CampaignIndex)
				loadInitCreditsBIN_(establishing_shots_[i].establishing_shot_name);
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

FunctionPatch sub_4D9200_patch(sub_4D9200, sub_4D9200_);

const char* __stdcall get_Tileset_String(Tileset tileset)
{
	static TblFile tbl_file("rez\\tilesets.tbl");

	return tbl_file[tileset];
}

NopPatch tilsetNameTblArithmetic((void*)0x4A7960, 3);
CallSitePatch tilsetNameTblCall((void*)0x4A7964, get_Tileset_String);

MemoryPatch tilesetNames_1(0x4D6D41, TILESET_NAMES, sizeof(*TILESET_NAMES));
MemoryPatch tilesetNames_2(0x4D4B26, TILESET_NAMES + 1, sizeof(TILESET_NAMES[1]));

int TILESET_PALETTE_RELATED[] = {
	0x512778,
	0x6D1228,
	0x6D1228,
	0x5127B8,
	0x512778,
	0x51279C,
	0x51279C,
	0x51279C,
};

MemoryPatch tilesetRelated_1(0x4BDD8A, TILESET_PALETTE_RELATED, sizeof(*TILESET_PALETTE_RELATED));
MemoryPatch tilesetRelated_2(0x4C99E4, TILESET_PALETTE_RELATED, sizeof(*TILESET_PALETTE_RELATED));
MemoryPatch tilesetRelated_3(0x4CB56A, TILESET_PALETTE_RELATED, sizeof(*TILESET_PALETTE_RELATED));
MemoryPatch tilesetRelated_4(0x4CB5DF, TILESET_PALETTE_RELATED, sizeof(*TILESET_PALETTE_RELATED));
MemoryPatch tilesetRelated_5(0x4CBEDA, TILESET_PALETTE_RELATED, sizeof(*TILESET_PALETTE_RELATED));
MemoryPatch tilesetRelated_6(0x4EEEB7, TILESET_PALETTE_RELATED, sizeof(*TILESET_PALETTE_RELATED));

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

FailStubPatch TriggerAction_PlayWav_patch(TriggerAction_PlayWav);

ActionPointer ActionTable_[] = {
	TriggerAction_NoAction,
	TriggerAction_Victory,
	TriggerAction_Defeat,
	TriggerAction_PreserveTrigger,
	TriggerAction_Wait,
	TriggerAction_PauseGame,
	TriggerAction_UnpauseGame,
	TriggerAction_Transmission,
	TriggerAction_PlayWav_,
	TriggerAction_DisplayTextMessage,
	TriggerAction_CenterView,
	TriggerAction_CreateUnitWithProperties,
	TriggerAction_SetMissionObjectives,
	TriggerAction_SetSwitch,
	TriggerAction_SetCountdownTimer,
	TriggerAction_RunAiScript,
	TriggerAction_RunAiScriptAtLocation,
	TriggerAction_LeaderBoard,
	TriggerAction_LeaderBoard,
	TriggerAction_LeaderBoard,
	TriggerAction_LeaderBoard,
	TriggerAction_LeaderBoard,
	TriggerAction_KillUnit,
	TriggerAction_KillUnitAtLocation,
	TriggerAction_RemoveUnit,
	TriggerAction_RemoveUnitAtLocation,
	TriggerAction_SetResources,
	TriggerAction_SetScore,
	TriggerAction_MinimapPing,
	TriggerAction_TalkingPortrait,
	TriggerAction_MuteUnitSpeech,
	TriggerAction_UnmuteUnitSpeech,
	TriggerAction_LeaderBoardComputerPlayers,
	TriggerAction_LeaderBoard,
	TriggerAction_LeaderBoard,
	TriggerAction_LeaderBoard,
	TriggerAction_LeaderBoard,
	TriggerAction_LeaderBoard,
	TriggerAction_MoveLocation,
	TriggerAction_MoveUnit,
	TriggerAction_LeaderBoard,
	TriggerAction_SetNextScenario,
	TriggerAction_SetDoodadState,
	TriggerAction_SetInvincibility,
	TriggerAction_CreateUnitWithProperties,
	TriggerAction_SetDeaths,
	TriggerAction_Order,
	TriggerAction_NoAction,
	TriggerAction_GiveUnitsToPlayer,
	TriggerAction_ModifyUnitHitPoints,
	TriggerAction_ModifyUnitEnergy,
	TriggerAction_ModifyUnitShieldPoints,
	TriggerAction_ModifyUnitResourceAmount,
	TriggerAction_ModifyUnitHangarCount,
	TriggerAction_PauseTimer,
	TriggerAction_UnpauseTimer,
	TriggerAction_Draw,
	TriggerAction_SetAllianceStatus,
	TriggerAction_DisableDebugMode,
	TriggerAction_EnableDebugMode,
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
			a1->container.dwExecutionFlags = a1->container.dwExecutionFlags & ~0x51u;
			a1->container.bCurrentActionIndex = 0;
		}
		else
		{
			a1->container.dwExecutionFlags = a1->container.dwExecutionFlags | 8;
		}
	}
}

FailStubPatch ExecuteTriggerActions_patch(ExecuteTriggerActions);

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

FailStubPatch executeGameTrigger_patch(executeGameTrigger);

void __stdcall BWFXN_ExecuteGameTriggers_(signed int dwMillisecondsPerFrame)
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

FunctionPatch BWFXN_ExecuteGameTriggers_patch(BWFXN_ExecuteGameTriggers, BWFXN_ExecuteGameTriggers_);
