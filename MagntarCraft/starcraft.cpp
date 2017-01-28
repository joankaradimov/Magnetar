#include <ddraw.h>
#include "starcraft.h"
#include "AddressPatch.h"

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
	if (!GetModuleFileNameA(hInst, a2, 0x104u))
		*a2 = 0;
	char* v4 = strrchr(a2, '\\');
	if (v4)
		*v4 = 0;
	SStrNCat(a2, (char *)hMpq, 260);
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
	char path_buffer[260]; // [sp+4h] [bp-104h]@3

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
	HANDLE v14; // edi@19
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
		v14 = phFile;
		if (!SFileReadFile(phFile, buffer, filesize, &bytes_to_read, 0))
		{
			if (GetLastError() == 38)
				FileFatal(v14, 24);
			FileFatal(v14, GetLastError());
		}
		if (bytes_to_read != filesize)
			FileFatal(v14, 24);
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
	char buff[260]; // [sp+8h] [bp-104h]@1

	_snprintf(buff, 0x104u, "%s\\%s.gid", "font", "font");
	v0 = fastFileRead_(&dword_51CE5C, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (v0)
	{
		if (dword_51CE5C)
			goto LABEL_5;
		SMemFree(v0, "Starcraft\\SWAR\\lang\\grid.cpp", 118, 0);
	}
	v0 = 0;
LABEL_5:
	dword_51CE60 = v0;
	_snprintf(buff, 0x104u, "%s\\%s.clh", "font", "font");
	v1 = fastFileRead_(&dword_51CE64, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (!v1)
	{
	LABEL_8:
		v1 = 0;
		goto LABEL_9;
	}
	if (!dword_51CE64)
	{
		SMemFree(v1, "Starcraft\\SWAR\\lang\\grid.cpp", 118, 0);
		goto LABEL_8;
	}
LABEL_9:
	dword_51CE68 = v1;
	_snprintf(buff, 0x104u, "%s\\%s.ccd", "font", "font");
	dword_5124D0 = KeyVerification(buff, "sgubon") == 0;
}

int LoadMainModuleStringInfo_()
{
	int result; // eax@1
	DWORD v1; // esi@3
	const void *v2; // ebx@4
	unsigned int puLen; // [sp+0h] [bp-Ch]@5
	DWORD dwHandle; // [sp+4h] [bp-8h]@3
	LPVOID lpBuffer; // [sp+8h] [bp-4h]@5

	result = GetModuleFileNameA(hInst, tstrFilename, 0x104u);
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
						0x104u,
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
	CHAR Filename[260]; // [sp+8h] [bp-20Ch]@1
	CHAR archivename_[260]; // [sp+10Ch] [bp-108h]@20
	HANDLE phFile; // [sp+210h] [bp-4h]@5

	dword_51CD44 = 20;
	dword_51CD48 = aInternalVersio;
	dword_51CD4C = tstrFilename;
	dword_51CD50 = byte_51CA20;
	dword_51CD54 = archivename;
	LoadMainModuleStringInfo_();
	if (!GetModuleFileNameA(hInst, Filename, 0x104u))
		Filename[0] = 0;
	v0 = strrchr(Filename, '\\');
	if (v0)
		*v0 = 0;
	SStrNCat(Filename, "\\Stardat.mpq", 260);
	if (!SFileOpenArchive(Filename, 0x7D0u, 2u, &phFile) || (v1 = phFile) == 0)
		v1 = 0;
	dword_51CC38 = v1;
	if (!v1)
	{
		int last_error = GetLastError();
		SysWarn_FileNotFound("Stardat.mpq", last_error);
	}
	if (!GetModuleFileNameA(hInst, archivename, 0x104u))
		archivename[0] = 0;
	v2 = strrchr(archivename, '\\');
	if (v2)
		*v2 = 0;
	SStrNCat(archivename, "\\patch_rt.mpq", 260);
	if (!SFileOpenArchive(archivename, 0x1B58u, 2u, &phFile) || (v3 = phFile) == 0)
		v3 = 0;
	dword_51CC28 = v3;
	InitializeFontKey_();
	AppAddExit_(DestroyFontKey);
	if (!dword_5124D0)
		InitializeCDArchives_(0, 1);
	byte_51CA20[0] = 0;
	if (!dword_5124D0)
	{
		if (!GetModuleFileNameA(hInst, archivename_, 0x104u))
			*archivename_ = 0;
		v4 = strrchr(archivename_, '\\');
		if (v4)
			*v4 = 0;
		SStrNCat(archivename_, "\\Broodat.mpq", 260);
		if (!SFileOpenArchive(archivename_, 0x9C4u, 2u, &phFile) || (v5 = phFile) == 0)
			v5 = 0;
		dword_51CC2C = v5;
		if (v5)
		{
			SStrCopy(byte_51CA20, archivename_, 0x208u);
			SStrNCat(byte_51CA20, ";", 520);
		}
	}
	v6 = SFileOpenFileEx(0, "rez\\epilogX.txt", 0, &phFile);
	if (v6)
	{
		SFileCloseFile(phFile);
		v6 = 1;
	}
	dword_6D11E4 = v6;
	return SStrNCat(byte_51CA20, Filename, 520);
}

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
		SStrCopy(byte_596910, source, 0x104u);
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
		signal(21, 1);
	CommandLineCheck();
	AppAddExit_(CloseAllArchives);
	InitializeArchiveHandles_();
	DataVersionCheck("rez\\DataVersion.txt");
	if (hMpq)
		DataVersionCheck("rez\\CDversion.txt");
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
	dword_51CC30 = loadTBL_(
		1577,
		65,
		"Starcraft\\SWAR\\lang\\init.cpp",
		"arr\\mapdata.tbl",
		MapdataFilenames);
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
	if (GetCurrentThreadId() == main_thread_id_maybe)
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

void __cdecl audioVideoInit_()
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
	dword_5999B0 = loadTBL_(
		1711,
		1144,
		"Starcraft\\SWAR\\lang\\snd.cpp",
		"arr\\sfxdata.tbl",
		SFXData_SoundFile);
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
				dword_57F0DC = v7;
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

void GameRun_(MenuPosition a1)
{
	IsInGameLoop = 1;
	int v1 = LoadGameInit();
	IsInGameLoop = 0;
	if (!InReplay)
	{
		if (!stru_5967F8.width)
			stru_5967F8.width = map_size.width;
		if (!stru_5967F8.height)
			stru_5967F8.height = map_size.height;
		if (!stru_5967F8.tileset)
			stru_5967F8.tileset = CurrentTileSet;
		if (!stru_5967F8.game_type)
			stru_5967F8.game_type = stru_5967F8.got_file_values.template_id;
		if (!stru_5967F8.data242)
			stru_5967F8.data242 = stru_5967F8.got_file_values.unused1;
		if (!stru_5967F8.data243)
			stru_5967F8.data243 = stru_5967F8.got_file_values.variation_id;
		SetReplayData(&stru_5967F8, Players, dword_57F21C);
	}
	if (dword_6D1218)
	{
		fclose(dword_6D1218);
		dword_6D1218 = 0;
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

PatchAddress ChkLoader_VCOD_patch(ChkLoader_VCOD, ChkLoader_VCOD_);

bool __stdcall ChkLoader_MTXM_(SectionData *a1, int a2, MapChunks *a3)
{
	if ((unsigned)a2 <= 0x10000 && (unsigned int)((char *)a1->field1 + a1->size) <= a1->field0)
	{
		memcpy(MapTileArray, a1->field1, a1->size);
		sub_4BCEA0();

		ActiveTile *v7 = &ActiveTileArray[map_size.width * (map_size.height - 2)];
		for (int i = 0; i <= 2; ++i)
		{
			for (int j = 0; j <= 5; ++j)
			{
				*(_DWORD *)v7 &= ~0x20410000u;
				++v7;
			}
			v7 = v7 + map_size.width - 5;
		}

		ActiveTile *v11 = &ActiveTileArray[map_size.width * (map_size.height - 2)];
		for (int i = 0; i <= 2; ++i)
		{
			for (int j = 0; j <= 5; ++j)
			{
				*(_DWORD *)v11 |= 0x800000u;
				++v11;
			}
			v11 = v11 + map_size.width - 5;
		}

		ActiveTile *v15 = ActiveTileArray + map_size.width * (map_size.height - 1) - 5;
		for (int i = 0; i <= 2; ++i)
		{
			for (int j = 0; j <= 5; ++j)
			{
				*(_DWORD *)v15 &= ~0x20410000u;
				++v15;
			}
			v15 = (v15 + map_size.width - 5);
		}

		ActiveTile *v19 = ActiveTileArray + map_size.width * (map_size.height - 1) - 5;
		for (int i = 0; i <= 2; ++i)
		{
			for (int j = 0; j <= 5; ++j)
			{
				*(_DWORD *)v19 |= 0x800000u;
				++v19;
			}
			v19 = v19 + map_size.width - 5;
		}

		SetFogMask(0x20410000, 1, map_size.width, 0, map_size.height - 1);
		AddFogMask(1, map_size.width, 0x800000, 0, map_size.height - 1);
		return SAI_PathCreate(ActiveTileArray) != 0;
	}
	else
	{
		return 0;
	}
}

PatchAddress ChkLoader_MTXM_patch(ChkLoader_MTXM, ChkLoader_MTXM_);

int sub_413550_(ChkSectionLoader *loader, ChunkNode *a2, int a3, MapChunks *a4)
{
	ChunkData *v6;

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

signed int ReadChunkNodes_(int a1, int a2, ChkSectionLoader *chk_section_loader, int a3, MapChunks *a4)
{
	ChunkNode v8;

	v8.field2.next = (ChunkData *)&v8.field2.next;
	v8.count = 0;
	v8.field2.previous = (ChunkData *)~(unsigned int)&v8.field2;
	sub_413670(a3, &v8, a2, ChunkNode_Constructor);
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
				*(_DWORD *)~(unsigned int)v8.field2.previous = (DWORD) v8.field2.next;
				v8.field2.next->field1.previous = v8.field2.previous;
				return 0;
			}
			*(ChunkData **)((char *)&v8.field2.previous->field1.next
				+ (int)&v8.field2
				- (int)v8.field2.next->field1.previous) = v8.field2.next;
			v8.field2.next->field1.previous = v8.field2.previous;
		}
		return 0;
	}
}

signed int sub_4CCAC0_(char *a1, MapChunks *a2)
{
	int v3;
	void* v5;
	signed int v7;
	char buff[260];
	char v9[260];
	int v10;
	int v11;

	v3 = a2 != 0 ? (a1 != 0 ? -(SStrLen(a1) != 0) : 0) : 0;
	SStrLen(a1);
	if (!v3)
	{
		SErrSetLastError(0x57u);
		return 0;
	}
	if (!sub_4CC350(v9, a1, (int)&a2->data7, 0x104u))
		return 0;
	v10 = 0;
	if (v9[0])
		_snprintf(buff, 0x104u, "%s\\%s", v9, "staredit\\scenario.chk");
	else
		SStrCopy(buff, "staredit\\scenario.chk", 0x104u);
	v5 = fastFileRead(&v10, 0, buff, 0, 1, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2060);
	if (v5)
	{
		v11 = 0;
		if (ReadMapChunks(a2, (int) v5, &v11, v10))
		{
			v7 = ReadChunkNodes_(chk_loaders[v11].i1, v10, chk_loaders[v11].ptr1, (int) v5, a2);
			SMemFree((void *)v5, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2077, 0);
			mapHandleDestroy();
			return v7;
		}
		SMemFree((void *)v5, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2068, 0);
		mapHandleDestroy();
		return 0;
	}
	if (hArchive)
	{
		SFileCloseArchive(hArchive);
		hArchive = 0;
	}
	return 0;
}

int ReadMapData_(char *source, MapChunks *a4, int a5)
{
	int v5; // edi@4
	int v6; // esi@4
	char *v8; // esi@7
	int v9; // ecx@11
	bool v11; // zf@12
	__int16 v12; // ax@20
	char *v13; // eax@22
	u16 v14; // ax@23
	int v15; // [sp+0h] [bp-4h]@1

	CurrentMapFileName[0] = 0;
	if (!a5)
		CampaignIndex = MD_none;
	memset(LobbyPlayers, 0, sizeof(PlayerInfo[12]));
	dword_59BDA8 = 0;
	dword_59BDAC = 0;
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
		v5 = dword_6D0F20;
		v6 = (int)dword_6D0F24;
		v15 = MD_none;
		if (!ReadMapChunks(a4, (int)dword_6D0F24, &v15, dword_6D0F20)
			|| !ReadChunkNodes_(chk_loaders[v15].i1, v5, chk_loaders[v15].ptr1, v6, a4))
			return 0;
		v8 = source;
	}
	else
	{
		v8 = source;
		if (!*source || !sub_4CCAC0_(source, a4))
			return 0;
	}
	v9 = 12;
	PlayerInfo* v10_ = LobbyPlayers + 12;
	do
	{
		--v10_;
		--v9;
		v11 = v10_->nRace == Race::RACE_Select;
		v10_->dwPlayerID = v9;
		v10_->dwStormId = -1;
		if (v11)
		{
			v10_->nRace = Race::RACE_Random;
			if (v9 < 8u)
				*((_BYTE *)&dword_59BDA8 + (unsigned __int8)v9) = 1;
		}
		if (v10_ >= LobbyPlayers + 8)
		{
			v10_->nType = PlayerType::PT_NotUsed;
			v10_->nRace = Race::RACE_Zerg;
			v10_->nTeam = 0;
		}
	} while (v10_ != LobbyPlayers);
	sub_4A91E0();
	sub_45AC10(&a4->data1);
	updatePlayerForce();
	strrchr(v8, '\\');
	SStrCopy(CurrentMapFileName, v8, 0x104u);
	if (!a5)
		CampaignIndex = MD_none;
	v12 = LOWORD(a4->data0);
	v11 = LOWORD(a4->data0) == 0;
	dword_5994DC = 1;
	if (v11)
		goto LABEL_25;
	if (MapStringTbl.buffer)
	{
		v14 = v12 - 1;
		if (v14 < *MapStringTbl.buffer)
		{
			v13 = (char *)MapStringTbl.buffer + MapStringTbl.buffer[v14 + 1];
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

int CreateCampaignGame_(MapData a1)
{
	int result; // eax@1
	GotFileValues *v2; // eax@2
	MapChunks a4; // [sp+0h] [bp-D0h]@1
	struct_game_140 v4; // [sp+20h] [bp-B0h]@2
	char v5[32]; // [sp+B0h] [bp-20h]@2

	CampaignIndex = a1;
	result = ReadMapData_(MapdataFilenames[a1], &a4, 1);
	if (result)
	{
		memset(&v4, 0, 140u);
		v4.got_file_values.unused3[4] = 0;
		SStrCopy(v4.player_name, CurrentPlayer, 24u);
		SStrCopy(v4.map_name, CurrentMapName, 32u);
		v4.game_speed = GameSpeed;
		v4.data231 = 1;
		v4.number_of_open_slots = 1;
		v2 = readTemplate("Use Map Settings(1)", v5, v5);
		if (v2)
		{
			memcpy(&v4.got_file_values, v2, sizeof(v4.got_file_values));
			SMemFree(v2, "Starcraft\\SWAR\\lang\\uiSingle.cpp", 270, 0);
			if (sub_4DBE50())
			{
				dword_596888 = 0;
				return CreateGame(&v4) != 0;
			}
		}
	}
	return 0;
}

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

	byte_57F1E3 = 0;
	dword_51CA1C = 0;
	if (!LoadCharacterData(&v8, CurrentPlayer))
	{
		v1 = (const char *)(*dword_6D1220 > 71u ? ((char *)dword_6D1220 + dword_6D1220[72]) : "");
		if ((_stricmp(CurrentPlayer, v1) || !verifyCharacterFile(&v8, CurrentPlayer)) && !byte_6D5A10)
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
			? (v2 < *dword_6D1220 ? (v3 = (char *)dword_6D1220 + dword_6D1220[v2 + 1]) : (v3 = ""))
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
	LOBYTE(OpheliaCheat2[0]) = GLUE_MAIN_MENU;
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
	if (LOBYTE(OpheliaCheat2[0]))
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
			if (LOBYTE(OpheliaCheat2[0]) == v1)
			{
				if (stru_5967F8.got_file_values.victory_conditions != v1
					|| stru_5967F8.got_file_values.starting_units != v1
					|| stru_5967F8.got_file_values.tournament_mode != v1
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
	if (dword_6D0F24)
	{
		SMemFree(dword_6D0F24, "Starcraft\\SWAR\\lang\\replay.cpp", 1106, 0);
		dword_6D0F24 = 0;
	}
	if (!LOBYTE(OpheliaCheat2[0]))
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
	char *v0; // eax@3
	unsigned int v1; // eax@5
	void *v2; // ecx@7
	char *v3; // eax@15

	if (!GetModuleFileNameA(0u, byte_51C4C0, 260u))
		byte_51C4C0[0] = 0;
	v0 = strrchr(byte_51C4C0, '\\');
	if (v0)
		*v0 = 0;
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
	if (Ophelia && !LOBYTE(OpheliaCheat2[0]))
	{
		Ophelia = 0;
		if (!CurrentPlayer[0])
		{
			if (*dword_6D1220 > 71u)
				v3 = (char *)dword_6D1220 + dword_6D1220[72];
			else
				v3 = "";
			SStrCopy(CurrentPlayer, v3, 25u);
		}
		byte_57F1E3 = 0;
		IsExpansion = dword_51C4BC != 0;
		if (dword_51C4BC && !dword_6D11E4)
			goto LABEL_38;
		LOBYTE(v2) = dword_51C4BC != 0;
		if (!loadCampaignBIN(v2) || !CreateCampaignGame_((MapData)NextCampaign))
			goto LABEL_38;
		if (dword_51C608)
		{
			if (dword_51C608 != 1)
			{
				if (dword_51C608 == 2)
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
	if (dword_57F0DC)
	{
		DestroyDialog(dword_57F0DC);
		dword_57F0DC = 0;
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
	if (stru_51C000[0].data)
		SMemFree(stru_51C000[0].data, "Starcraft\\SWAR\\lang\\glues.cpp", 442, 0);
	memset(stru_51C000, 0, sizeof(Bitmap[129]));
	while (gwGameMode == GAME_GLUES)
	{
		switch (glGluesMode)
		{
		case GLUE_GENERIC:
			loadMenu_None();
			break;
		case GLUE_MAIN_MENU:
			byte_6D5A10 = 0;
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
	if (stru_51C000[0].data)
		SMemFree(stru_51C000[0].data, "Starcraft\\SWAR\\lang\\glues.cpp", 442, 0);
	memset(stru_51C000, 0, sizeof(Bitmap[129]));
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
	unsigned int v0; // eax@6
	unsigned int v1; // eax@27
	bool v2; // zf@32
	HANDLE phFile; // [sp+Ch] [bp-4h]@1

	gwGameMode = GAME_GLUES;
	PreInitData_();
	InitializeInputProcs();
	CreateMainWindow();
	audioVideoInit_();
	AppAddExit_(SaveCPUThrottleOption);
	if (SRegLoadValue("Starcraft", "CPUThrottle", 0, (int *)&phFile))
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
			if (gwGameMode != GAME_GLUES && dword_57F0DC)
			{
				DestroyDialog(dword_57F0DC);
				dword_57F0DC = (dialog *)GLUE_MAIN_MENU;
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
					dword_6D0F0C = 1;
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
							v1 = 24;
						}
						else
						{
							v1 = 1;
						}
						PlayMovie(v1);
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
		v0 = 24;
	}
	else
	{
		if (!(BYTE1(dword_6CDFEC) & 2))
			goto LABEL_8;
		dword_6CDFEC &= ~0x200u;
		PlayMovie(0);
		v0 = 1;
	}
	PlayMovie(v0);
	goto LABEL_8;
}

unsigned __int32 LocalGetLang_()
{
	CHAR Buffer[16]; // [sp+4h] [bp-14h]@2
	char *v2; // [sp+14h] [bp-4h]@5

	if (hInstance && LoadStringA(hInstance, 3u, Buffer, 16) || LoadStringA(hInst, 3u, Buffer, 16))
		return strtoul(Buffer, &v2, 16);
	else
		return 1033;
}

void localDll_Init_(HINSTANCE a1)
{
	char Filename[260];

	if (!GetModuleFileNameA(a1, Filename, 260u))
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

void main(HINSTANCE starcraft_exe) {
	hInst = starcraft_exe;
	main_thread_id_maybe = GetCurrentThreadId();
	CheckForOtherInstances("SWarClass");
	localDll_Init_(hInst);
	VerifySystemMemory();
	FastIndexInit();
	BWSetSecurityInfo();
	GameMainLoop_();
}
