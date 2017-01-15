#include "starcraft.h"


signed int AppAddExit_(AppExitHandle a1)
{
	AppExitHandle *app_exit_handles_; // edx@1
	signed int v2; // esi@3
	signed int v3; // eax@3
	AppExitHandle v4; // ecx@4

	app_exit_handles_ = app_exit_handles;
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
	v2 = -1;
	v3 = 0;
	do
	{
		v4 = app_exit_handles_[v3];
		if (v4 == a1)
			return 0;
		if (v2 == -1 && !v4)
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
	char *v4; // eax@3
	HANDLE result; // eax@9

	if (!GetModuleFileNameA(hInst, a2, 0x104u))
		*a2 = 0;
	v4 = strrchr(a2, '\\');
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
	result = hMpq;
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
	char *v5; // esi@1
	char *v6; // edi@1
	char *v7; // eax@1
	char *v8; // eax@5
	char dwInitParam[512]; // [sp+8h] [bp-300h]@5
	CHAR Buffer[256]; // [sp+208h] [bp-100h]@5

	v5 = source_file;
	v6 = function_name;
	v7 = strrchr(source_file, '\\');
	if (v7)
		v5 = v7 + 1;
	if (!v6)
		v6 = "";
	v8 = GetErrorString_(Buffer, sizeof(Buffer), last_error);
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
	unsigned int v1; // eax@3
	unsigned int v3; // eax@5
	signed int v4; // ecx@6
	unsigned int v5; // eax@8
	unsigned int v6; // eax@12
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
	if (!ddraw_dll)
	{
		ddraw_dll = LoadLibraryA("ddraw.dll");
		if (!ddraw_dll)
		{
			v1 = GetLastError();
			ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "LoadLibrary", v1, 0x65u, 113);
		}
	}
	int(__stdcall *DirectDrawCreate)(signed int, IDirectDraw **, _DWORD) = (decltype(DirectDrawCreate))GetProcAddress(ddraw_dll, "DirectDrawCreate");
	if (!DirectDrawCreate)
	{
		v3 = GetLastError();
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "GetProcAddress", v3, 0x66u, 118);
	}
	v4 = 0;
	if (byte_6D5DFC)
		v4 = 2;
	v5 = DirectDrawCreate(v4, &DDInterface, 0);
	if (v5 == 0x887600DE)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "DirectDrawCreate", 0x887600DE, 0x6Eu, 124);
	if (v5)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "DirectDrawCreate", v5, 0x66u, 125);
	v6 = DDInterface->SetCooperativeLevel(hWndParent, 16 | DSSCL_PRIORITY | DSSCL_NORMAL);
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
	memcpy(&stru_6CEB40, &palette, sizeof(stru_6CEB40));
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
	u8 *v2; // eax@3
	u16 v3; // dx@3
	u16 v4; // cx@3
	dlgEvent v5; // [sp+4h] [bp-14h]@3

	a1->lFlags |= 0x44000000u;
	a1->fields[0].dlg.pModalFcn = 0;
	if (a2)
		a1->pfcnInteract = a2;
	v2 = (u8 *)SMemAlloc(
		a1->fields[0].dlg.dstBits_wid * a1->fields[0].dlg.dstBits_ht,
		"Starcraft\\SWAR\\lang\\dlg\\dlg.cpp",
		481,
		0);
	v3 = Mouse.x;
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
	v4 = LOWORD(Mouse.y);
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
	LONG v1; // eax@2 MAPDST
	int v3; // eax@3
	dialog *v4; // ebx@6
	int v6; // eax@9
	dialog *v7; // esi@13
	int v8; // eax@16
	int read; // [sp+8h] [bp-8h]@6
	HANDLE phFile; // [sp+Ch] [bp-4h]@1 MAPDST

	if (!SFileOpenFileEx(0, "rez\\titledlg.bin", 0, &phFile))
	{
		v8 = SErrGetLastError();
		SysWarn_FileNotFound("rez\\titledlg.bin", v8);
	}
	v1 = SFileGetFileSize(phFile, 0);
	if (v1 == -1)
	{
		v3 = GetLastError();
		FileFatal(phFile, v3);
	}
	else
	{
		if (!v1)
			SysWarn_FileNotFound("rez\\titledlg.bin", 24);
		v4 = (dialog *)SMemAlloc(v1, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210, 0);
		if (SFileReadFile(phFile, v4, v1, &read, 0))
		{
			if (read == v1)
			{
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
			v6 = GetLastError();
			FileFatal(phFile, v6);
		}
	}
}

void GameRun_(MenuPosition a1)
{
	signed int v1; // esi@1
	GamePosition next_game_position; // si@18

	IsInGameLoop = 1;
	v1 = LoadGameInit();
	IsInGameLoop = 0;
	if (!InReplay)
	{
		if (!word_596818)
			word_596818 = map_size.width;
		if (!word_59681A)
			word_59681A = map_size.height;
		if (!word_596828)
			word_596828 = CurrentTileSet;
		if (!gameType)
			gameType = got_template.template_id;
		if (!byte_596821)
			byte_596821 = got_template.unused1;
		if (!word_596822)
			word_596822 = got_template.variation_id;
		SetReplayData(&dword_5967F8, Players, dword_57F21C);
	}
	if (dword_6D1218)
	{
		fclose(dword_6D1218);
		dword_6D1218 = 0;
	}
	if (v1)
	{
		next_game_position = BeginGame(a1);
		DestroyGame();
		gwGameMode = next_game_position;
	}
	else
	{
		DestroyGame();
		gwGameMode = GAME_GLUES;
	}
}

int sub_4B5110_(int a1)
{
	char *v3;
	int result = 0;

	if (!dword_59A0D4[a1])
	{
		result = LoadCampaignWithCharacter(a1) != 0;
	}
	else
	{
		WORD v2 = (a1 != 1) + 142;
		if ((a1 != 1) != -143)
		{
			if (v2 < *dword_6D1220) {
				v3 = (char *)dword_6D1220 + dword_6D1220[v2 + 1];
			}
			else
			{
				v3 = "";
			}
		}
		else {
			v3 = NULL;
		}
		if (sub_4B5B20(v3)) {
			result = LoadCampaignWithCharacter(a1) != 0;
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
	dword_59BA60 = (void *)LoadGraphic("Starcraft\\SWAR\\lang\\gluCreat.cpp", 1427);
	dword_6D5A74 = GAME_RUNINIT;
	v5 = gluLoadBINDlg(dword_6D5A70, gluCustm_Interact);
	if (v5 != -3)
	{
		if (v5 == 12)
		{
			if (LOBYTE(OpheliaCheat2[0]) == v1)
			{
				if (got_template.victory_conditions != v1
					|| got_template.starting_units != v1
					|| got_template.tournament_mode != v1
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
		if (!loadCampaignBIN(v2) || !CreateCampaignGame((MapData)NextCampaign))
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
		memset(stru_6CEB40, 0, sizeof(stru_6CEB40));
		byte_51A0E9 = 1;
		memcpy(stru_6CE720, GamePalette, sizeof(stru_6CE720));
		gluDlgFadePalette(3u);
		BWFXN_RedrawTarget();
	}
	RefreshCursor_0();
	dword_6D5E38 = (int(__thiscall *)(_DWORD))jmpNoMenu;
	if (stru_51C000[0].data)
		SMemFree(stru_51C000[0].data, "Starcraft\\SWAR\\lang\\glues.cpp", 442, 0);
	memset(stru_51C000, 0, sizeof(stru_51C000));
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
	memset(stru_51C000, 0, sizeof(stru_51C000));
	stopSounds();
	stopMusic();
	dword_6D5E20 = &GameScreenBuffer;
	if (!byte_51A0E9)
	{
		memset(stru_6CEB40, 0, sizeof(stru_6CEB40));
		byte_51A0E9 = 1;
		memcpy(stru_6CE720, GamePalette, sizeof(stru_6CE720));
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
