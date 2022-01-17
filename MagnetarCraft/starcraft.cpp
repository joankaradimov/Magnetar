#include <ddraw.h>
#include "starcraft.h"
#include "tbl_file.h"
#include "patching/AddressPatch.h"
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
	signed int result; // eax@2
	INT_PTR v4; // eax@8
	char path_buffer[MAX_PATH]; // [sp+4h] [bp-104h]@3

	if (hMpq)
	{
		result = 1;
	}
	else
	{
		hMpq = LoadInstallArchiveHD(filename, path_buffer, "\\BroodWar.mpq", (HANDLE)1000);
		if (hMpq || (hMpq = LoadInstallArchiveHD(filename, path_buffer, "\\StarCraft.mpq", (HANDLE)1000)) != 0)
		{
			result = 1;
		}
		else
		{
			hMpq = LoadInstallArchiveCD(1000u, "\\Install.exe", (char *)filename);
			if (hMpq)
			{
			LABEL_11:
				result = 1;
			}
			else
			{
				while (a2)
				{
					v4 = DialogBoxParamA(hInstance, (LPCSTR)107, hWndParent, DialogFunc, 0);
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
						goto LABEL_11;
				}
				result = 0;
			}
		}
	}
	return result;
}

int(*signal)(int a1, int a2) = (int(*)(int a1, int a2)) 0x0040C8D5;

void* fastFileRead_(int *bytes_read, int searchScope, char *filename, int defaultValue, int bytes_to_read, char *logfilename, int logline)
{
	DWORD lastError; // eax@2
	void *result; // eax@7
	LONG filesize; // eax@8 MAPDST
	void *buffer; // ebx@17
	HANDLE phFile; // [sp+8h] [bp-4h]@1 MAPDST

	if (SFileOpenFileEx(0, filename, searchScope, &phFile))
	{
		filesize = SFileGetFileSize(phFile, 0);
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
		buffer = (void *)defaultValue;
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
		result = buffer;
	}
	else
	{
		lastError = SErrGetLastError();
		if (!bytes_to_read || lastError != 2 && lastError != 1006)
		{
			SysWarn_FileNotFound(filename, lastError);
			//JUMPOUT(*(_DWORD *)byte_4D2E37);
		}
		if (bytes_read)
			*bytes_read = 0;
		result = 0;
	}
	return result;
}

void InitializeFontKey_(void)
{
	void *v0; // eax@1
	void *v1; // eax@5
	char buff[MAX_PATH]; // [sp+8h] [bp-104h]@1

	_snprintf(buff, MAX_PATH, "%s\\%s.gid", "font", "font");
	v0 = fastFileRead_(&cdkey_encrypted_len, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (v0)
	{
		if (cdkey_encrypted_len)
			goto LABEL_5;
		SMemFree(v0, "Starcraft\\SWAR\\lang\\grid.cpp", 118, 0);
	}
	v0 = 0;
LABEL_5:
	cdkey_encrypted = v0;
	_snprintf(buff, MAX_PATH, "%s\\%s.clh", "font", "font");
	v1 = fastFileRead_(&cdkeyowner_encrypted_len, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (!v1)
	{
	LABEL_8:
		v1 = 0;
		goto LABEL_9;
	}
	if (!cdkeyowner_encrypted_len)
	{
		SMemFree(v1, "Starcraft\\SWAR\\lang\\grid.cpp", 118, 0);
		goto LABEL_8;
	}
LABEL_9:
	cdkeyowner_encrypted = v1;
	_snprintf(buff, MAX_PATH, "%s\\%s.ccd", "font", "font");
	is_spawn = KeyVerification(buff, "sgubon") == 0;
}

int LoadMainModuleStringInfo_()
{
	int result; // eax@1
	DWORD v1; // esi@3
	const void *v2; // ebx@4
	unsigned int puLen; // [sp+0h] [bp-Ch]@5
	DWORD dwHandle; // [sp+4h] [bp-8h]@3
	LPVOID lpBuffer; // [sp+8h] [bp-4h]@5

	result = GetModuleFileNameA(hInst, tstrFilename, MAX_PATH);
	if (result)
	{
		result = GetFileVersionInfoSizeA(tstrFilename, &dwHandle);
		v1 = result;
		if (result)
		{
			v2 = SMemAlloc(result, "Starcraft\\SWAR\\lang\\init.cpp", 345, 0);
			if (GetFileVersionInfoA(tstrFilename, 0, v1, (LPVOID)v2))
			{
				if (VerQueryValueA(v2, "\\", &lpBuffer, &puLen))
					_snprintf(
						aInternalVersio,
						MAX_PATH,
						"Version %d.%d.%d",
						*((_DWORD *)lpBuffer + 4) >> 16,
						(unsigned __int16)*((_DWORD *)lpBuffer + 4),
						*((_WORD *)lpBuffer + 11));
			}
			result = SMemFree((void *)v2, "Starcraft\\SWAR\\lang\\init.cpp", 408, 0);
		}
	}
	else
	{
		tstrFilename[0] = 0;
	}
	return result;
}

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

AddressPatch InitializeArchiveHandles_patch(InitializeArchiveHandles, InitializeArchiveHandles_);

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

void PreInitData_()
{
	SFileSetIoErrorMode(1, FileIOErrProc_);
	AppAddExit_(leaveOnQuit);
	if (signal(2, 1) != -1)
	{
		signal(21, 1);
	}
	CommandLineCheck();
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
	LoadGameData(mapdataDat, "arr\\mapdata.dat");
	dword_51CC30 = loadTBL_(1577, 65, "Starcraft\\SWAR\\lang\\init.cpp", "arr\\mapdata.tbl", MapdataFilenames);
	AppAddExit_(FreeMapdataTable);
	LoadGameTemplates(Template_Constructor);
	AppAddExit_(DestroyGameTemplates);
}

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
	if (DialogBoxParamA(hInstance, (LPCSTR)resource, hWndParent, DialogFunc, (LPARAM)dwInitParam) == -1)
		FatalError("GdsDialogBoxParam: %d", resource);
	DLGErrFatal();
}

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
	if (v5 == 0x887600DE)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "DirectDrawCreate", 0x887600DE, 0x6Eu, 124);
	if (v5)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "DirectDrawCreate", v5, 0x66u, 125);
	HRESULT v6 = DDInterface->SetCooperativeLevel(hWndParent, 16 | DSSCL_PRIORITY | DSSCL_NORMAL);
	if (v6 != 0x88760245 && v6)
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
	if (CurrentTileSet == Platform)
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

AddressPatch DrawGameProc_patch(DrawGameProc, DrawGameProc_);

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
	LoadGameData(sfxdataDat, "arr\\sfxdata.dat");
	dword_5999B0 = loadTBL_(1711, 1144, "Starcraft\\SWAR\\lang\\snd.cpp", "arr\\sfxdata.tbl", SFXData_SoundFile);
	AppAddExit_(sfxdata_cleanup);
	if (!byte_6D11D0)
	{
		AudioVideoInitializationError error;
		if (DSoundInit(&error, hWndParent))
			AppAddExit_(j_BWFXN_DSoundDestroy);
	}
}

void InitializeDialog_(dialog *a1, FnInteract a2)
{
	dlgEvent v5;

	a1->lFlags |= 0x44000000u;
	a1->fields[0].dlg.pModalFcn = 0;
	if (a2)
		a1->pfcnInteract = a2;
	u8 *v2 = (u8 *)SMemAlloc(
		a1->fields[0].dlg.dstBits_wid * a1->fields[0].dlg.dstBits_ht,
		"Starcraft\\SWAR\\lang\\dlg\\dlg.cpp",
		481,
		0);
	u16 v3 = Mouse.x;
	a1->fields[0].dlg.dstBits_data = v2;
	LOWORD(v2) = LOWORD(Mouse.y);
	v5.cursor.x = v3;
	a1->wIndex = 0;
	v5.wNo = 14;
	v5.dwUser = 7;
	*(_DWORD *)&v5.wSelection = 0;
	v5.cursor.y = (unsigned __int16)v2;
	a1->pfcnInteract(a1, &v5);
	a1->rct.right += a1->rct.left;
	u16 v4 = LOWORD(Mouse.y);
	a1->rct.bottom += a1->rct.top;
	v5.cursor.y = v4;
	v5.wNo = 14;
	v5.dwUser = 10;
	*(_DWORD *)&v5.wSelection = 0;
	v5.cursor.x = Mouse.x;
	a1->pfcnInteract(a1, &v5);
	v5.cursor.x = Mouse.x;
	v5.wNo = 14;
	v5.dwUser = 0;
	*(_DWORD *)&v5.wSelection = 0;
	v5.cursor.y = LOWORD(Mouse.y);
	a1->pfcnInteract(a1, &v5);
	SetCallbackTimer(12, a1, 100, 0);
	a1->lFlags &= ~0x4000000u;
}

void LoadTitle_()
{
	HANDLE phFile; // [sp+Ch] [bp-4h]@1 MAPDST

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
				dialog *v7;
				SFileCloseFile(phFile);
				if (v4)
				{
					v4->lFlags |= 4u;
					AllocInitDialogData(v4, v4, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\title.cpp", 190);
					v7 = v4;
				}
				else
				{
					v7 = 0;
				}
				load_screen = v7;
				InitializeDialog_(v7, TitleDlgProc);
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

void CreateInitialMeleeUnits_()
{
	for (int player_index = 0; player_index < 8; player_index++)
	{
		PlayerInfo* player = Players + player_index;
		if (player->nType == PT_Human || player->nType == PT_Computer)
		{
			u8 starting_units = gameData.got_file_values.starting_units;
			if (!gameData.got_file_values.victory_conditions
				&& !gameData.got_file_values.starting_units
				&& !gameData.got_file_values.tournament_mode
				&& player_index < 8
				&& playerForce[player_index])
			{
				starting_units = 2;
			}
			if (starting_units != 1)
			{
				if (starting_units != 2)
				{
					continue;
				}
				CreateInitialMeleeBuildings(player->nRace, player_index);
				if (player->nRace == Race::RACE_Zerg)
					CreateInitialOverlord(player_index);
			}

			CreateInitialMeleeWorker_(player->nRace, player_index);
			CreateInitialMeleeWorker_(player->nRace, player_index);
			CreateInitialMeleeWorker_(player->nRace, player_index);
			CreateInitialMeleeWorker_(player->nRace, player_index);
		}
	}
}

//AddressPatch CreateInitialMeleeUnits_patch(CreateInitialMeleeUnits, CreateInitialMeleeUnits_);

ChkSectionLoader CreateChkSectionLoader(const char(&section_name)[5], bool(__stdcall* func)(SectionData*, int, MapChunks*), int flags)
{
	return {
		{section_name[0], section_name[1], section_name[2], section_name[3]},
		func,
		flags,
	};
}

ChkSectionLoader chk_loaders_version_[] = {
	CreateChkSectionLoader("TYPE", ChkLoader_TYPE, 1),
	CreateChkSectionLoader("VER ", ChkLoader_VER, 1),
};

ChkSectionLoader chk_loaders_lobby_[] = {
	CreateChkSectionLoader("VER ", ChkLoader_VER, 1),
	CreateChkSectionLoader("DIM ", ChkLoader_DIM, 1),
	CreateChkSectionLoader("ERA ", ChkLoader_ERA, 1),
	CreateChkSectionLoader("OWNR", ChkLoader_OWNR, 1),
	CreateChkSectionLoader("SIDE", ChkLoader_SIDE, 1),
	CreateChkSectionLoader("STR ", ChkLoader_STR, 1),
	CreateChkSectionLoader("SPRP", ChkLoader_SPRP, 1),
	CreateChkSectionLoader("FORC", ChkLoader_FORC, 1),
	CreateChkSectionLoader("VCOD", ChkLoader_VCOD, 1),
};

ChkSectionLoader chk_loaders_briefing_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR, 1),
	CreateChkSectionLoader("MBRF", ChkLoader_MBRF, 1),
};

ChkSectionLoader chk_loaders_melee_vanilla_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2, 1),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT, 1),
};

ChkSectionLoader chk_loaders_ums_1_00_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2, 1),
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
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT, 1),
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN, 1),
	CreateChkSectionLoader("TRIG", ChkLoader_TRIG, 1),
};

ChkSectionLoader chk_loaders_ums_1_04_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2, 1),
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
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT, 1),
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN_, 1),
	CreateChkSectionLoader("TRIG", ChkLoader_TRIG, 1),
};

ChkSectionLoader chk_loaders_melee_broodwar_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2, 1),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT, 1),
	CreateChkSectionLoader("COLR", ChkLoader_COLR, 1),
};

ChkSectionLoader chk_loaders_ums_broodwar_1_04_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2, 1),
	CreateChkSectionLoader("MASK", ChkLoader_MASK, 1),
	CreateChkSectionLoader("UNIx", ChkLoader_UNIx, 1),
	CreateChkSectionLoader("UPGx", ChkLoader_UPGx, 1),
	CreateChkSectionLoader("TECx", ChkLoader_TECx, 1),
	CreateChkSectionLoader("PUNI", ChkLoader_PUNI, 1),
	CreateChkSectionLoader("PUPx", ChkLoader_PUPx, 1),
	CreateChkSectionLoader("PTEx", ChkLoader_PTEx, 1),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT, 1),
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
			v6 = a2->field2.previous;
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
			if (v6->section_data.chunk_name == *(DWORD*)loader[v4].name)
			{
				if (loader[v4].func)
				{
					if (!loader[v4].func(&v6->section_data, v6->section_data.size, a4))
						break;
				}
			}
			v6 = v6->field1.previous;
			if ((signed int)v6 <= 0)
				goto LABEL_8;
		}
		return 0;
	}
}

signed int ReadChunkNodes_(int a1, int a2, ChkSectionLoader* chk_section_loader, void* chk_data, MapChunks* a4)
{
	ChunkNode v8;

	v8.field2.next = (ChunkData*)&v8.field2.next;
	v8.count = 0;
	v8.field2.previous = (ChunkData*)~(unsigned int)&v8.field2;
	sub_413670((int)chk_data, &v8, a2, ChunkNode_Constructor);
	if (sub_4135C0(chk_section_loader, &v8, a1))
	{
		if (sub_413550_(chk_section_loader, &v8, a1, a4))
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
		_list_unlink((int)&v8);
		if (v8.field2.next)
		{
			if ((signed int)v8.field2.previous <= 0)
			{
				*(_DWORD*)~(unsigned int)v8.field2.previous = (DWORD)v8.field2.next;
				v8.field2.next->field1.previous = v8.field2.previous;
				return 0;
			}
			*(ChunkData**)((char*)&v8.field2.previous->field1.next
				+ (int)&v8.field2
				- (int)v8.field2.next->field1.previous) = v8.field2.next;
			v8.field2.next->field1.previous = v8.field2.previous;
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
				if (chk_loaders_[i].version == a1->data6)
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
		return sub_4CC7F0(CurrentMapFileName);
	}

	return 0;
}

AddressPatch LoadMap_patch(LoadMap, LoadMap_);

void GameRun_(MenuPosition a1)
{
	IsInGameLoop = 1;
	int v1 = LoadGameInit();
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
		DestroyGame();
		gwGameMode = next_game_position;
	}
	else
	{
		DestroyGame();
		gwGameMode = GAME_GLUES;
	}
}

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

AddressPatch setup_HUD_patch(setup_HUD, setup_HUD_);

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

AddressPatch setMapSizeConstants_patch(setMapSizeConstants, setMapSizeConstants_);

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

bool __stdcall ChkLoader_ERA_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 2)
		return 0;
	if ((unsigned int)section_data->field1 + section_data->size > section_data->field0)
		return 0;

	memcpy_s(&CurrentTileSet, sizeof(CurrentTileSet), section_data->field1, section_size);
	if (CurrentTileSet > Tileset::Jungle && !IsExpansion)
		return 0;

	int tileset_count = sizeof(TILESET_NAMES) / sizeof(*TILESET_NAMES);
	CurrentTileSet = Tileset(CurrentTileSet % tileset_count);
	return 1;
}

AddressPatch ChkLoader_ERA_patch(ChkLoader_ERA, ChkLoader_ERA_);

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

	if (!CopySectionData(section_data, hCHKData))
	{
		SMemFree(hCHKData, "Starcraft\\SWAR\\lang\\maphdr.cpp", 1174, 0);
		return 0;
	}
	int Hash_Chk = CHK_PerformVCODcheck(LobbyPlayers, 0x1B0u, (int) hCHKData, section_size);
	int Hash_VCOD = CHK_PerformVCODcheck(LobbyPlayers, 0x1B0u, (int) VCOD_Locked, section_size);

	SMemFree(hCHKData, "Starcraft\\SWAR\\lang\\maphdr.cpp", 1182, 0);
	return Hash_VCOD == Hash_Chk;
}

AddressPatch ChkLoader_VCOD_patch(ChkLoader_VCOD, ChkLoader_VCOD_);

#define MAX_MAP_DIMENTION 256

DEFINE_ENUM_FLAG_OPERATORS(MegatileFlags);

bool __stdcall ChkLoader_MTXM_(SectionData *a1, int a2, MapChunks *a3)
{
	if (a2 > MAX_MAP_DIMENTION * MAX_MAP_DIMENTION * sizeof(TileID) || (int)(a1->field1 + a1->size) > a1->field0)
	{
		return 0;
	}

	memcpy(MapTileArray, a1->field1, a1->size);
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

AddressPatch ChkLoader_MTXM_patch(ChkLoader_MTXM, ChkLoader_MTXM_);

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

AddressPatch updateHUDInformation_patch(updateHUDInformation, updateHUDInformation_);

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

AddressPatch initMapData_patch(initMapData, initMapData_);

int sub_4CCAC0_(char *a1, MapChunks *a2)
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
			SMemFree((void *)chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2077, 0);
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

int __stdcall ReadMapData_(char *source, MapChunks *a4, int is_campaign)
{
	char *v8;
	__int16 v12;
	char *v13;

	CurrentMapFileName[0] = 0;
	if (!is_campaign)
		CampaignIndex = MD_none;
	memset(LobbyPlayers, 0, sizeof(PlayerInfo[12]));
	memset(playerForce, 0, 8);
	a4->data0 = 0;
	a4->data1 = 0;
	a4->data2 = 0;
	a4->data3 = 0;
	a4->data4 = 0;
	a4->data5 = 0;
	a4->data6 = 0;
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
		if (!*source || !sub_4CCAC0(source, a4))
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
	sub_45AC10(&a4->data1);
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
			v13 = (char *)MapStringTbl.buffer + MapStringTbl.buffer[v12];
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

AddressPatch ReadMapData_patch(ReadMapData, ReadMapData_);

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

AddressPatch sub_4CC990_patch(sub_4CC990, sub_4CC990_);

int CreateCampaignGame_(MapData mapData)
{
	MapChunks mapChunks;
	GameData v4;
	char v5[32];

	CampaignIndex = mapData;
	int readSuccess = ReadMapData_(MapdataFilenames[mapData], &mapChunks, 1);
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
	GotFileValues* v2 = readTemplate("Use Map Settings(1)", v5, v5);
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

AddressPatch CreateCampaignGame_patch(CreateCampaignGame_, CreateCampaignGame);

int LoadCampaignWithCharacter_(int a1)
{
	const char *v1; // eax@3
	int v2; // ecx@9
	char *v3; // esi@9
	WORD *result; // eax@11
	unsigned int v5; // ecx@12
	WORD v6; // cx@14
	MapData v7; // dx@15
	CharacterData v8; // [sp+8h] [bp-70h]@1

	customSingleplayer = 0;
	dword_51CA1C = 0;
	if (!LoadCharacterData(&v8, playerName))
	{
		v1 = (const char *)(*networkTable > 71u ? ((char *)networkTable + networkTable[72]) : "");
		if ((_stricmp(playerName, v1) || !verifyCharacterFile(&v8, playerName)) && !outOfGame)
			doNetTBLError(0, 0, 0, 88);
	}
	v2 = off_5122A0[a1];
	v3 = (char *)&v8.gap[a1];
	if (IsExpansion)
	{
		v2 = off_5122AC[a1];
		v3 = &v8.more_data[4 * a1];
	}
	result = (WORD *)loadmenu_GluHist(*(_DWORD *)v3, v2);
	dword_6D11CC = (int)result;
	if (result)
	{
		v5 = result[1];
		if (*(_DWORD *)v3 < v5)
		{
			*(_DWORD *)v3 = v5;
			CreateCharacterFile(&v8);
			result = (WORD *)dword_6D11CC;
		}
		v6 = result[2];
		if (v6)
		{
			v7 = (MapData) result[1];
			dword_5122B8 = v6;
			CampaignIndex = v7;
			byte_57F246[0] = 0;
			gwGameMode = GAME_CINEMATIC;
			return 1;
		}
		else
		{
			return CreateCampaignGame_((MapData)result[1]);
		}
	}
	else {
		return 0;
	}
}

int sub_4B5110_(int a1)
{
	int result = 0;

	if (!dword_59A0D4[a1])
	{
		result = LoadCampaignWithCharacter_(a1) != 0;
	}
	else {
		WORD v2;
		char *v3;

		if ((v2 = (a1 != 1) + 142, (a1 != 1) != ~142)
			? (v2 < *networkTable ? (v3 = (char *)networkTable + networkTable[v2 + 1]) : (v3 = ""))
			: (v3 = 0),
			sub_4B5B20(v3)) {
			result = LoadCampaignWithCharacter_(a1) != 0;
		}
	}
	return result;
}

signed int __stdcall sub_4B5180_(dialog *a1)
{
	if (LastControlID == 6)
	{
		if (!sub_4B5110_(2))
			return 1;
		LastControlID = 6;
	}
	else
	{
		if (LastControlID == 7)
		{
			if (sub_4B5110_(1))
			{
				LastControlID = 7;
				return (unsigned __int8)DLG_SwishOut(a1);
			}
			return 1;
		}
		if (LastControlID == 8)
		{
			if (sub_4B5110_(0))
			{
				LastControlID = 8;
				return (unsigned __int8)DLG_SwishOut(a1);
			}
			return 1;
		}
	}
	return (unsigned __int8)DLG_SwishOut(a1);
}

bool __fastcall gluExpCmpgn_CustomCtrlID_(dialog *dlg, struct dlgEvent *evt)
{
	if (evt->wNo == 14)
	{
		if (evt->dwUser)
		{
			if (evt->dwUser == 2)
				return sub_4B5180_(dlg);
			if (evt->dwUser == 10)
			{
				registerMenuFunctions(off_51A818, dlg, 44, 0);
				DlgSwooshin(2, &stru_512A8C, dlg, 0);
				return genericDlgInteract(dlg, evt);
			}
		}
		else
		{
			DLG_SwishIn(dlg);
			if (!byte_6D5BBC)
				DLGMusicFade(22);
		}
	}
	return genericDlgInteract(dlg, evt);
}

int loadMenu_gluExpCmpgn_()
{
	dialog *v0; // eax@1 MAPDST
	dialog *v2; // eax@2
	int result; // eax@5

	OpheliaEnabled = GLUE_MAIN_MENU;
	LOBYTE(multiPlayerMode) = GLUE_MAIN_MENU;
	sub_4B5050();
	v0 = (dialog *)loadFullMenuDLG("rez\\gluExpCmpgn.bin", 0, GLUE_MAIN_MENU, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
	if (v0)
	{
		v0->lFlags |= 4u;
		AllocInitDialogData(v0, v0, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
		v2 = v0;
	}
	else
	{
		v2 = 0;
	}
	switch (gluLoadBINDlg(v2, gluExpCmpgn_CustomCtrlID_))
	{
	case 8:
		glGluesMode = GLUE_READY_Z;
		result = changeMenu();
		break;
	case 7:
		glGluesMode = GLUE_READY_T;
		result = changeMenu();
		break;
	case 6:
		glGluesMode = GLUE_READY_P;
		result = changeMenu();
		break;
	case 5:
		glGluesMode = GLUE_LOAD;
		result = changeMenu();
		break;
	case 10:
		glGluesMode = GLUE_CREATE;
		result = changeMenu();
		break;
	case 11:
		glGluesMode = GLUE_CREATE_MULTI;
		result = changeMenu();
		break;
	case 9:
		glGluesMode = GLUE_LOGIN;
		result = changeMenu();
		break;
	default:
		glGluesMode = GLUE_MAIN_MENU;
		result = changeMenu();
		break;
	}
	return result;
}

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
				else if (Players[g_LocalNationID].nRace)
				{
					if (Players[g_LocalNationID].nRace == 1)
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
					glGluesMode = GLUE_READY_Z;
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
	if ((unsigned int)NetMode <= 0x4D4F444D)
	{
		if (NetMode == 0x4D4F444D)
		{
		LABEL_32:
			glGluesMode = MenuPosition::GLUE_MODEM;
			goto LABEL_37;
		}
		if (NetMode == 1112425812)
		{
			glGluesMode = MenuPosition::GLUE_BATTLE;
			goto LABEL_37;
		}
		if (NetMode == 1296321880)
			goto LABEL_32;
		goto LABEL_35;
	}
	if (NetMode != 1396916812)
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
	unsigned int v1; // eax@5
	void *v2; // ecx@7
	char *v3; // eax@15

	if (!GetModuleFileNameA(0u, main_directory, MAX_PATH))
		main_directory[0] = 0;

	char* lastDirectorySeparator = strrchr(main_directory, '\\');
	if (lastDirectorySeparator)
		*lastDirectorySeparator = 0;

	v1 = 0;
	do
	{
		byte_50CDC1[v1] = v1;
		++v1;
	} while (v1 < 256);
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
	if (Ophelia && !LOBYTE(multiPlayerMode))
	{
		Ophelia = 0;
		if (!playerName[0])
		{
			if (*networkTable > 71u)
				v3 = (char *)networkTable + networkTable[72];
			else
				v3 = "";
			SStrCopy(playerName, v3, 25u);
		}
		customSingleplayer = 0;
		IsExpansion = level_cheat_is_bw != 0;
		if (level_cheat_is_bw && !dword_6D11E4)
			goto LABEL_38;
		LOBYTE(v2) = level_cheat_is_bw != 0;
		if (!loadCampaignBIN(v2) || !CreateCampaignGame_((MapData)level_cheat_mission))
			goto LABEL_38;
		if (level_cheat_race)
		{
			if (level_cheat_race != 1)
			{
				if (level_cheat_race == 2)
				{
					glGluesMode = GLUE_READY_P;
					goto LABEL_26;
				}
			LABEL_38:
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
		if (GameCheats & CHEAT_NoGlues)
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
			loadMenu_gluCmpgn();
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

void GameMainLoop_()
{
	bool v2;
	HANDLE phFile;

	gwGameMode = GAME_GLUES;
	PreInitData_();
	InitializeInputProcs();
	CreateMainWindow();
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
		if (!(BYTE1(dword_6CDFEC) & 8))
		{
		LABEL_8:
			LoadTitle();
			LoadInitIscriptBIN();
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
					PlayMovieWithIntro(dword_5122B8);
					dword_5122B8 = 28;
					if (gwGameMode == GAME_CINEMATIC)
						ContinueCampaign(1);
					continue;
				case GAME_RESTART:
					next_campaign_mission = 1;
					goto LABEL_23;
				case GAME_RUNINIT:
				LABEL_23:
					gwGameMode = GAME_RUN;
					continue;
				case GAME_INTRO:
					PlayMovie(0);
					if (gwGameMode == GAME_INTRO)
					{
						if (hMpq && SFileOpenFileEx(hMpq, "rez\\gluexpcmpgn.bin", GLUE_MAIN_MENU, &phFile))
						{
							SFileCloseFile(phFile);
							PlayMovie(24);
						}
						else
						{
							PlayMovie(1);
						}
						if (gwGameMode == GAME_INTRO)
							gwGameMode = GAME_GLUES;
					}
					continue;
				case GAME_GLUES:
				case GAME_WIN:
				case GAME_LOSE:
					SwitchMenu(); // TODO: use SwitchMenu_
					continue;
				case GAME_CREDITS:
					BeginCredits();
					v2 = gwGameMode == GAME_CREDITS;
					break;
				case GAME_EPILOG:
					BeginEpilog();
					v2 = gwGameMode == GAME_EPILOG;
					break;
				default:
					AppExit(0);
					ProcError(1);
					exit(0);
					return;
				}
				if (v2)
				{
					gwGameMode = GAME_GLUES;
					glGluesMode = GLUE_MAIN_MENU;
				}
			}
		}
		dword_6CDFEC &= ~0x800u;
		PlayMovie(0);
		PlayMovie(24);
	}
	else
	{
		if (!(BYTE1(dword_6CDFEC) & 2))
			goto LABEL_8;
		dword_6CDFEC &= ~0x200u;
		PlayMovie(0);
		PlayMovie(1);
	}
	goto LABEL_8;
}

unsigned __int32 LocalGetLang_()
{
	CHAR Buffer[16];
	char *v2;

	if (hInstance && LoadStringA(hInstance, 3u, Buffer, 16) || LoadStringA(hInst, 3u, Buffer, 16))
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
	hInstance = LoadLibraryA(Filename);
	if (!hInstance)
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
	int v4; // eax
	dialog* v5; // ebx
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
	v5 = (dialog*)SMemAlloc(v3, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210, 0);
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
	if (!*multiPlayerMode && !(GameCheats & CHEAT_NoGlues) && CampaignIndex)
	{
		for (int i = 0; i < 64; i++)
		{
			if (campaign_missions[i].mission_index == CampaignIndex)
				loadInitCreditsBIN_(campaign_missions[i].mission_name);
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

AddressPatch sub_4D9200_patch(sub_4D9200, sub_4D9200_);

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

void main(HINSTANCE starcraft_exe) {
	hInst = starcraft_exe;
	main_thread_id = GetCurrentThreadId();
	CheckForOtherInstances("SWarClass");
	localDll_Init_(hInst);
	VerifySystemMemory();
	FastIndexInit();
	BWSetSecurityInfo();
	GameMainLoop_();
}
