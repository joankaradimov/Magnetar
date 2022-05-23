#include <algorithm>
#include <ddraw.h>
#include <process.h>
#include <time.h>
#include "starcraft.h"
#include "magnetorm.h"
#include "tbl_file.h"
#include "patching/patching.h"

signed int AppAddExit_(AppExitHandle a1)
{
	if (!app_exit_handles)
	{
		app_exit_handles = (AppExitHandle *)SMemAlloc(128, "Starcraft\\SWAR\\lang\\gds\\appaddex.cpp", 42, (int)app_exit_handles);
		memset(app_exit_handles, 0, 128u);
	}
	int v2 = -1;
	int v3 = 0;
	do
	{
		AppExitHandle exit_handle = app_exit_handles[v3];
		if (exit_handle == a1)
			return 0;
		if (v2 == -1 && !exit_handle)
			v2 = v3;
		++v3;
	} while (v3 < 32);
	if (v2 == -1)
	{
		FatalError("APPADDEX:1");
	}
	app_exit_handles[v2] = a1;
	return 1;
}

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
		DirtyArrayHandling();
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
	HANDLE phFile;
	if (!SFileOpenFileEx(0, bin_path, 0, &phFile))
	{
		SysWarn_FileNotFound(bin_path, SErrGetLastError());
	}
	LONG file_size = SFileGetFileSize(phFile, 0);
	if (file_size == -1)
	{
		FileFatal(phFile, GetLastError());
	}
	if (file_size == 0)
	{
		SysWarn_FileNotFound(bin_path, 24);
	}
	dialog* bin_dialog = (dialog*)SMemAlloc(file_size, __FILE__, __LINE__, 0);

	int read;
	if (!SFileReadFile(phFile, bin_dialog, file_size, &read, 0))
	{
		DWORD last_error = GetLastError();
		FileFatal(phFile, last_error == 38 ? 24 : last_error);
	}

	if (read != file_size)
	{
		FileFatal(phFile, 24);
	}
	SFileCloseFile(phFile);
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
	dword_51CD44 = 20;
	dword_51CD48 = aInternalVersio;
	dword_51CD4C = tstrFilename;
	dword_51CD50 = broodat_mpq_path;
	dword_51CD54 = archivename;
	LoadMainModuleStringInfo_();

	CHAR Filename[MAX_PATH];
	if (!GetModuleFileNameA(hInst, Filename, MAX_PATH))
		Filename[0] = 0;
	char* v0 = strrchr(Filename, '\\');
	if (v0)
		*v0 = 0;
	SStrNCat(Filename, "\\Stardat.mpq", MAX_PATH);
	if (!SFileOpenArchive(Filename, 2000u, 2u, &stardat_mpq))
	{
		SysWarn_FileNotFound("Stardat.mpq", GetLastError());
	}

	if (!GetModuleFileNameA(hInst, archivename, MAX_PATH))
		archivename[0] = 0;
	char* v2 = strrchr(archivename, '\\');
	if (v2)
		*v2 = 0;
	SStrNCat(archivename, "\\patch_rt.mpq", MAX_PATH);
	if (!SFileOpenArchive(archivename, 7000u, 2u, &patch_rt_mpq))
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
	!SFileOpenArchive(magnetarDatFilename, 8000u, 2u, &magnetar_mpq);

	InitializeFontKey_();
	AppAddExit_(DestroyFontKey);
	if (!is_spawn)
		InitializeCDArchives_(0, 1);
	broodat_mpq_path[0] = 0;
	if (!is_spawn)
	{
		CHAR archivename_[MAX_PATH];
		if (!GetModuleFileNameA(hInst, archivename_, MAX_PATH))
			*archivename_ = 0;
		char* v4 = strrchr(archivename_, '\\');
		if (v4)
			*v4 = 0;
		SStrNCat(archivename_, "\\Broodat.mpq", MAX_PATH);
		if (SFileOpenArchive(archivename_, 2500u, 2u, &broodat_mpq))
		{
			SStrCopy(broodat_mpq_path, archivename_, 0x208u);
			SStrNCat(broodat_mpq_path, ";", 520);
		}
		else
		{
			broodat_mpq = 0;
		}
	}

	DetectExpansionInstallation_();
	return SStrNCat(broodat_mpq_path, Filename, 520);
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

void LoadSfx_()
{
	LoadGameData_(sfxdataDat, "arr\\sfxdata.dat");
	dword_5999B0 = loadTBL_(1711, 1144, "Starcraft\\SWAR\\lang\\snd.cpp", "arr\\sfxdata.tbl", SFXData_SoundFile);
}

FAIL_STUB_PATCH(LoadSfx);

char* MapdataFilenames_[73];

MEMORY_PATCH(0x4280A2, MapdataFilenames_);
MEMORY_PATCH(0x4A7DC9, MapdataFilenames_);
MEMORY_PATCH(0x512BA0, MapdataFilenames_);
MEMORY_PATCH(0x512BA8, _countof(MapdataFilenames_));

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

FAIL_STUB_PATCH(CommandLineCheck);

GotFileValues* readTemplate_(const char* template_name, char* got_template_name, char* got_template_label)
{
	char buff[260];
	int got_file_size;

	_snprintf(buff, 0x104u, "%s%s%s", "Templates\\", template_name, ".got");
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
	char buff[260];
	char v7[260];
	int v10;

	_snprintf(buff, 0x104u, "%s%s", "Templates\\", "templates.lst");
	BYTE* v1 = (BYTE*)fastFileRead(&v10, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	if (v1 == nullptr)
	{
		FatalError("Unable to read game templates.");
	}
	BYTE* v11 = v1;
	while (sub_4AAE20(v7, (unsigned int*)&v10, (_BYTE**) &v11, 0x104u))
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

FUNCTION_PATCH(LoadGameTemplates, LoadGameTemplates_);
FAIL_STUB_PATCH(sub_4AB970);

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
	registry_options.field_18 |= 7u;
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
	if (DialogBoxParamA(hModule, (LPCSTR)resource, hWndParent, DialogFunc, (LPARAM)dwInitParam) == -1)
		FatalError("GdsDialogBoxParam: %d", resource);
	DLGErrFatal();
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
		surface_desc.dwFlags = 7;
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

FUNCTION_PATCH(BWFXN_DDrawInitialize, BWFXN_DDrawInitialize_);

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
		refreshImageRange(dword_5993A4, dword_5993C0);
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

FAIL_STUB_PATCH(DrawGameProc);

void InitializeGameLayer_()
{
	SetRect(&game_screen_pos, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 80 - 1);
	ScreenLayers[5].left = 0;
	ScreenLayers[5].top = 0;
	ScreenLayers[5].pSurface = 0;
	ScreenLayers[5].bits = 0;
	ScreenLayers[5].width = SCREEN_WIDTH;
	ScreenLayers[5].height = SCREEN_HEIGHT - 80;
	ScreenLayers[5].pUpdate = DrawGameProc_;
	memset(RefreshRegions, 1u, sizeof(RefreshRegions));
	for (int i = 3; i <= 5; ++i)
	{
		ScreenLayers[i].bits |= 1u;
		int left = ScreenLayers[i].left;
		int top = ScreenLayers[i].top;
		int bottom = top + ScreenLayers[i].height - 1;
		int right = left + ScreenLayers[i].width - 1;
		BWFXN_RefreshTarget(left, bottom, top, right);
	}
}

FAIL_STUB_PATCH(InitializeGameLayer);

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
	muteBgm(&registry_options);
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

void loadColorSettings_()
{
	AppAddExit(saveColorSettings);

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
	AppAddExit_(sfxdata_cleanup);
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

FAIL_STUB_PATCH(LoadInitIscriptBIN);

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
				CreateInitialMeleeBuildings(player->nRace, player_index);
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
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN, 1),
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
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN_, 1),
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
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN_, 1),
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

FUNCTION_PATCH(LoadMap, LoadMap_);

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

void initMapData_();
void setMapSizeConstants_();

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
		drawAllMinimapBoxes();
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
		InitializeDialog_(minimap_Dlg, MiniMapPreviewInteract);
	}
}

FAIL_STUB_PATCH(load_gluMinimap);

void __cdecl InitializeSpriteArray_()
{
	LoadGameData_(spritesDat, "arr\\sprites.dat");
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

FUNCTION_PATCH(InitializeSpriteArray, InitializeSpriteArray_);

int sub_4EEFD0_()
{
	memcpy(stru_59C6C0, palette, sizeof(stru_59C6C0));
	sub_49BB90();
	initMapData_();
	InitializePresetImageArrays();
	InitializeSpriteArray();
	InitializeThingyArray();
	LoadGameData_(flingyDat, "arr\\flingy.dat");
	memset(dword_63FEE0, 0, 76u);
	dword_63FF3C = (CUnit*)dword_63FEE0;
	dword_63FF38 = (CUnit*)dword_63FEE0;
	dword_63FEC8 = 0;
	dword_63FF34 = 0;
	InitializeBulletArray();
	InitializeOrderArray();
	if (!loadGameFileHandle)
	{
		InitializeUnitCounts();
	}
	initializePsiFieldData();
	ResetDATFiles();
	resetOrdersUnitsDAT();
	createUnitBuildingSpriteValidityArray();
	if (loadGameFileHandle || LoadMap())
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
	sub_4CB5B0(0, TILESET_PALETTE_RELATED[CurrentTileSet]->y);
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
				replay_header.ReplayFrames = ElapsedTimeFrames;
			}
		}
		nextLeaveGameMenu();
	}
	return 0;
}

FUNCTION_PATCH(GameInit, GameInit_);
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
	__int16 v12;
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
		SStrCopy(v6.player_name, playerName, 24u);
		SStrCopy(v6.map_name, CurrentMapName, 32u);
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
		if (!LevelCheatInitGame_() || !LoadGameCreate_() || !RestartGame_() || !SinglePlayerMeleeInitGame())
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

FAIL_STUB_PATCH(LoadGameInit);

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
		freeChkFileMem_();
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

FAIL_STUB_PATCH(DestroyGame);

void GameLoop_(MenuPosition a1)
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
		updateActiveTileInfo();
		RemoveFoWCheat();
	}
	UpdateUnits();
	ImageDrawingBulletDrawing();
	UpdateImages(a1);
	updateThingys(a1);
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

void __cdecl colorCycleInterval_()
{
	unsigned int v0 = DoCycle_(cycle_colors, 0, 0x80u);
	if (v0 < 8)
		DoCycle_(&cycle_colors[v0], v0, 0x100u);
}

FAIL_STUB_PATCH(colorCycleInterval);

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

void sub_4D93B0_()
{
	if (dword_51BFA8 && ColorCycle && !IS_GAME_PAUSED)
	{
		DWORD tick_count = GetTickCount();
		if (tick_count - dword_6D6374 >= 0xA)
		{
			dword_6D6374 = tick_count;
			colorCycleInterval_();
		}
	}
}

FAIL_STUB_PATCH(sub_4D93B0);

void updateHUDInformation_()
{
	if (byte_59723C)
	{
		updateSelectedUnitData();
		byte_59723C = 0;
	}
	updateSelectedUnitPortrait();
	updateCurrentButtonset_();
	sub_458120();
	refreshScreen();
	sub_4D93B0_();
	refreshGameTextIfCounterActive();
}

FAIL_STUB_PATCH(updateHUDInformation);

void DoGameLoop_(MenuPosition a1)
{
	GameLoop_(a1);
	updateHUDInformation_();
	GameLoop_(a1);
	updateHUDInformation_();
}

FAIL_STUB_PATCH(DoGameLoop);

void GameLoop_State_(MenuPosition a2)
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
		if (!GameLoopWaitSendTurn(&v5))
		{
			dword_6D11F0 = 1;
			break;
		}
		if (InReplay && is_replay_paused)
		{
			replayFrameComputation();
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
			ScreenLayers[5].bits |= 2u;
			if (BWFXN_IsPaused())
			{
				RefreshAllUnits();
			}
			else
			{
				++ElapsedTimeFrames;
				++v9;
				GameLoop_(a2);
			}
			SetInGameLoop(1);
			BWFXN_ExecuteGameTriggers(GameSpeedModifiers.gameSpeedModifiers[registry_options.GameSpeed]);
			SetInGameLoop(0);
			if (InReplay)
			{
				replayFrameComputation();
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

void GameLoop_Top_(MenuPosition a1)
{
	FramesUntilNextTurn = 1;
	byte_51CE9D = 0;
	dword_51CE94 = GetTickCount();
	bool v2 = false;
	while (GameState)
	{
		BWFXN_videoLoop(3);
		DWORD v1 = GetTickCount();
		if (!byte_51CE9D && abs(int(dword_51CE94 - v1)) > GameSpeedModifiers.altSpeedModifiers[registry_options.GameSpeed])
		{
			dword_51CE94 = v1;
		}
		BWFXN_NextFrameHelperFunctionTarget();
		if (v1 + dword_51CE8C[0] > 0xA)
		{
			dword_51CE8C[0] = -v1;
			PollInput();
			v2 = true;
		}
		RecvMessage();
		LeagueChatFilter();
		if (v1 >= dword_51CE94)
		{
			GameLoop_State_(a1);
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

GamePosition BeginGame_(MenuPosition a1)
{
	visionUpdateCount = 1;
	DLGMusicFade((MusicTrack) currentMusicId);
	SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	GameState = 1;
	TickCountSomething(0);
	DoGameLoop_(a1);
	RefreshLayer5();
	int v1 = getCursorType();
	_drawCursor(v1);
	cursorRefresh();
	if (!multiPlayerMode && !getMapStartStatus() && !InReplay && (registry_options.field_18 & 0x100) != 0)
	{
		loadTips_BINDLG(1);
	}
	SetMapStartStatus();
	SetCurrentPaletteInfo(palette, 0x100u, 0);
	get_tFontGam_PCX();
	TitlePaletteUpdate(3);
	if (multiPlayerMode)
	{
		registry_options.GameSpeed = (unsigned __int8)gameData.game_speed;
	}
	else if (CampaignIndex == MD_none)
	{
		registry_options.GameSpeed = 4;
	}
	newGame(1);
	BWFXN_videoLoop(0);
	loseSightSelection();
	turn_counter = 0;
	GameKeepAlive();
	while (GameState && !gameLoopTurns())
	{
		BWFXN_RedrawTarget_();
	}
	GameLoop_Top_(a1);
	newGame(0);
	stopAllSound();
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
	order->action(order->f4, is_keycode_used[VK_SHIFT]);
}

FAIL_STUB_PATCH(sub_458BB0);

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
	InitializeDialog(current_dialog, statbtn_DLG_Interact_);
}

FAIL_STUB_PATCH(load_statbtn_BIN);

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
	load_statbtn_BIN_();
	load_Statf10_BIN();
	ctextbox_BIN();
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

FUNCTION_PATCH(setup_HUD, setup_HUD_);

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

FUNCTION_PATCH(setMapSizeConstants, setMapSizeConstants_);

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
	AppAddExit(freeCHKStringHandle);
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

DEFINE_ENUM_FLAG_OPERATORS(MegatileFlags);

// TODO: reimplement sub_422A90, sub_422FA0, SAI_PathCreate_Sub3_4 (0x483260) for pathfinding on map sizes > 256x256

u16 SAI_GetRegionIdFromPx_(__int16 x, __int16 y)
{
	u16 region_id = SAIPathing->mapTileRegionId[x / 32][y / 32];
	if (region_id >= 0x2000u)
	{
		return SAIPathing->splitTiles[region_id - 0x2000].rgn1;
	}
	return region_id;
}

u16 SAI_GetRegionIdFromPx__(__int16 y)
{
	__int16 x;

	__asm mov x, ax

	return SAI_GetRegionIdFromPx_(x, y);
}

FUNCTION_PATCH((void*)0x49C9A0, SAI_GetRegionIdFromPx__);

u16 GetRegionIdAtPosEx_(int x, int y)
{
	u16 region_id = SAIPathing->mapTileRegionId[x / 32][y / 32];
	if (region_id >= 0x2000u)
	{
		if ((1 << (((y / 8) & 3) + 4 * ((x / 8) & 3))) & SAIPathing->splitTiles[region_id - 0x2000].minitileMask)
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
		mov x, ecx
		mov y, edi
	}

	return GetRegionIdAtPosEx_(x, y);
}

FUNCTION_PATCH((void*)0x49C9F0, GetRegionIdAtPosEx__);

int SAI_PathCreate_Sub3_(PathCreateRelated* a1, SAI_Paths* a2)
{
	int old_region_count = a2->regionCount;

	if (!SAI_PathCreate_Sub3_0(a2, a1->position, a1->map_size))
	{
		return 0;
	}

	SAI_PathCreate_Sub3_1(old_region_count, a2);
	SAI_PathCreate_Sub3_2(a2);
	SAI_PathCreate_Sub3_3(a2);
	SAI_PathCreate_Sub3_1(old_region_count, a2);
	a2->splitTiles_end = a2->splitTiles;
	SAI_PathCreate_Sub3_4();
	memset(&a2->regions[a2->regionCount], 0, (5000 - a2->regionCount) << 6);
	return 1;
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3);

MEMORY_PATCH((void*)0x46EAA0, sizeof(SAI_Paths));

bool SAI_PathCreate_(MegatileFlags* a1)
{
	AllocateSAI_Paths_();

	SAI_PathCreate_Sub1(a1);

	PathCreateRelated v5;
	v5.position.x = 0;
	v5.position.y = 0;
	v5.map_size = map_size;
	CreateUIUnreachableRegion(SAIPathing);

	if (!SAI_PathCreate_Sub3_(&v5, SAIPathing))
	{
		return false;
	}

	SAI_PathCreate_Sub4(SAIPathing);
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
	a1[a2->player] = a2->position;

	bool v3 = InReplay ? MoveToTile.x == 0xFFFF : a2->player == g_LocalNationID;
	if (v3)
	{
		MoveToTile.x = max(a2->position.x - SCREEN_WIDTH / 2, 0) / 32;
		MoveToTile.y = max(a2->position.y - (SCREEN_HEIGHT - INTERFACE_HEIGHT) / 2, 0) / 32;
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
		return unit_entry->player == 11 && (IsResource(unit_entry->unit_type) || IsCritter(unit_entry->unit_type));
	}
}

FAIL_STUB_PATCH(unitNotNeutral);

int __fastcall getVisibilityMaskFromPositionAndSize_(int a1, __int16 a2, __int16 a3, __int16 a4, __int16 a5)
{
	MegatileFlags result = (MegatileFlags) 0;

	int v5 = a2 + a4 - 1;
	if (v5 >= a4)
	{
		int v7 = a5 + a3 - 1;
		do
		{
			if (v7 >= a3)
			{
				MegatileFlags* v8 = &active_tiles[v7 + v5 * map_size.width];
				for (int i = 0; i < a5; i++)
				{
					result |= ~(v8[i] & (MegatileFlags) 0xFF);
				}
			}
			--v5;
		} while (v5 >= a4);
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

FAIL_STUB_PATCH(ChkLoader_UNIT);

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
	CellMap = (__int16*)SMemAlloc(0x20000, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 604, 0);
	GameTerrainCache = (byte *)SMemAlloc(0x49800, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 605, 0);
	active_tiles = (MegatileFlags*)SMemAlloc(0x100000, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 606, 0);
	memset(active_tiles, 0, 0x40000u);
	dword_6D5CD8 = SMemAlloc(29241, "Starcraft\\SWAR\\lang\\repulse.cpp", 323, 8);
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".wpe");
	fastFileRead_(0, 0, filename, (int)palette, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".vf4");
	MiniTileFlags = (MiniTileMaps_type *)fastFileRead_(&bytes_read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	megatileCount = (unsigned int)bytes_read >> 5;
	GenerateMegatileDefaultFlags_();
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".cv5");
	TileSetMap = (TileType *)fastFileRead_(&bytes_read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	TileSetMapSize = bytes_read / 52u;
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".grp");

	struct_a1 a1;
	a1.pfunc0 = (int(__stdcall *)(_DWORD, _DWORD, _DWORD, _DWORD, _DWORD))sub_47E2D0;
	a1.isCreepCovered = isCreepCovered;
	a1.isTileVisible = isTileVisible;
	a1.pfuncC = 0;
	InitTerrainGraphicsAndCreep_(&a1, MapTileArray, map_size.width, map_size.height, filename);
	ZergCreepArray = location;
	_snprintf(filename, MAX_PATH, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".vx4");
	VX4Data = (vx4entry *)fastFileRead_(&bytes_read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
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
		if (!SFileReadFile(v4, v8, v5, &bytes_read, 0))
		{
			DWORD last_error = GetLastError();
			FileFatal(v4, last_error == 38 ? 24 : last_error);
		}
		else
		{
			if (bytes_read == v5)
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
					InitializeGameLayer_();
				}
				byte_658AC0 = 0;
				dword_658AA4 = 0;
				loadParallaxStarGfx_("star");
				sub_47D660();
			}
			else
			{
				FileFatal(v4, 24);
			}
		}
	}
}

FAIL_STUB_PATCH(initMapData);

unsigned int GetGroundHeightAtPos_(int x, int y)
{
	int megatile_index = x / 32 + y / 32 * map_size.width;
	TileID megatile = ZergCreepArray[megatile_index] ? ZergCreepArray[megatile_index] : MapTileArray[megatile_index];

	u16 v1 = TileSetMap[(megatile >> 4) & 0x7FF].megaTileRef[megatile & 0xF];
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
		if (x / 32 - v11 / 2 < 0 || x / 32 + v11 / 2 >= map_size.width ||
			y / 32 - v13 / 2 < 0 || y / 32 + v13 / 2 >= map_size.height)
		{
			v14 += 2;
		}
		unsigned v7 = ~vision_mask & ~(vision_mask << 8);
		sight_range = (*v14)(x / 32, y / 32, v6, &active_tiles[x / 32 + (y / 32) * map_size.width], v15, v7);
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
		BWFXN_videoLoop(3);
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

void registerMenuFunctions_(FnInteract* functions, dialog* a2, int functions_size)
{
	AnimateVideos(a2);
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
		int v12 = dword_50E170[326 * v10];
		if (dword_597394 != v12)
		{
			dword_597394 = v12;
			drawCursor();
		}
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

// TODO: patch and always use this function

int CreateCampaignGame_(MapData mapData)
{
	MapChunks mapChunks;

	CampaignIndex = mapData;
	if (ReadCampaignMapData_(&mapChunks))
	{
		GameData v4;
		memset(&v4, 0, 140u);
		v4.got_file_values.unused3[4] = 0;
		SStrCopy(v4.player_name, playerName, 24u);
		SStrCopy(v4.map_name, CurrentMapName, 32u);
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

void gluHist_Activate_(dialog* dlg)
{
	if (LastControlID == 1)
	{
		if (dlg->wCtrlType)
		{
			dlg = dlg->fields.ctrl.pDlg;
		}
		dialog* v1 = dlg->fields.dlg.pFirstChild;
		if (v1)
		{
			while (v1->wIndex != 6)
			{
				v1 = v1->pNext;
				if (!v1)
				{
					return;
				}
			}
			if (v1->fields.list.bStrs)
			{
				u8 v2 = v1->fields.list.bCurrStr;
				if (v2 != 0xFF)
				{
					dword_6D5A48 = (CampaignMenuEntry*) ((ExpandedCampaignMenuEntry*) dword_6D5A4C + v1->fields.list.pdwData[v2]);
				}
			}
		}
	}
}

FAIL_STUB_PATCH(gluHist_Activate);

bool __fastcall gluHist_Interact_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		if (evt->dwUser == EventUser::USER_CREATE)
		{
			gluHist_Create(dlg);
		}
		else if(evt->dwUser == EventUser::USER_ACTIVATE)
		{
			gluHist_Activate_(dlg);
		}
	}
	return sub_4B6D60(dlg, evt);
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
	BYTE* v9 = (BYTE*) SMemAlloc(glu_hist_file_size, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210, 0);
	int read;
	if (!SFileReadFile(glu_hist_file, v9, glu_hist_file_size, &read, 0))
	{
		DWORD last_error = GetLastError();
		FileFatal(glu_hist_file, last_error == 38 ? 24 : last_error);
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

	dword_6D5A3C = LoadDialog("rez\\gluHist.bin");

	gluLoadBINDlg_(dword_6D5A3C, gluHist_Interact_);
	if (dword_6D5A44)
	{
		SMemFree(dword_6D5A44, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 609, 0);
	}
	dword_6D5A44 = 0;
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

FUNCTION_PATCH(updateActiveCampaignMission, updateActiveCampaignMission_);

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

FAIL_STUB_PATCH(sub_4B5110);

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
	}
	return DLG_SwishOut(a1);
}

FAIL_STUB_PATCH(sub_4B2810);

void gluCmpgn_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		gluCmpgn_CampaignButton,
		genericLightupBtnInteract,
		gluCmpgn_CampaignButton,
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
			DLG_SwishIn(dlg);
			if (!byte_6D5BBC)
			{
				DLGMusicFade(MT_TITLE);
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

void gluExpCmpgn_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		genericLightupBtnInteract,
		gluExpCmpgn_CampaignButton,
		gluExpCmpgn_CampaignButton,
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
			DLG_SwishIn(dlg);
			if (!byte_6D5BBC)
				DLGMusicFade(MT_TITLE);
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

void loadMenu_gluLogin_()
{
	gluLogin_Dlg = LoadDialog("rez\\gluLogin.bin");

	switch (gluLoadBINDlg_(gluLogin_Dlg, gluLogin_Main))
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

void loadMenu_gluJoin_()
{
	InReplay = 0;
	freeChkFileMem_();

	gluJoin_Dlg = LoadDialog("rez\\gluJoin.bin");

	switch (gluLoadBINDlg_(gluJoin_Dlg, gluJoin_Main))
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

bool __fastcall gluCustm_Interact_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gluCustm_initSwish(dlg);
			DLG_SwishIn(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			if (LastControlID == 12 && !gluCustmLoadMapFromList())
			{
				return 1;
			}
			waitLoopCntd(5, gluCreateOrCustm_bin);
			return DLG_SwishOut(dlg);
		case EventUser::USER_INIT:
			gluCustm_CustomCtrl_InitializeChildren(dlg);
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

int SelGameMode_(int a2)
{
	int v4 = LastControlID;
	switch (load_gluGameMode_BINDLG())
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
		const char* v8 = get_GluAll_String(SPAWNED_BY);
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
			DLG_SwishIn(dlg);
			genericDlgInteract(dlg, evt);
			if (byte_6D5BBC)
			{
				return true;
			}
			DLGMusicFade(MT_TITLE);
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
					const char* s = *networkTable > 0x66u ? (const char*)networkTable + networkTable[103] : "";
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

void gluRdyP_CustomCtrlID_(dialog* dlg)
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

void gluRdyT_CustomCtrlID_(dialog* dlg)
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

FAIL_STUB_PATCH(gluRdyT_CustomCtrlID);

void gluRdyZ_CustomCtrlID_(dialog* dlg)
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
		gluRdyZ_Secret,
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

FAIL_STUB_PATCH(gluRdyZ_CustomCtrlID);

bool __fastcall gluRdy_BINDLG_Loop(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			DLG_SwishIn(dlg);
			break;
		case EventUser::USER_DESTROY:
			briefingFramesCleanup(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			return sub_46D160(dlg);
		case EventUser::USER_INIT:
			sub_46D3C0(dlg);
			switch (glGluesMode)
			{
			case MenuPosition::GLUE_READY_P:
				gluRdyP_CustomCtrlID_(dlg);
				break;
			case MenuPosition::GLUE_READY_T:
				gluRdyT_CustomCtrlID_(dlg);
				break;
			case MenuPosition::GLUE_READY_Z:
				gluRdyZ_CustomCtrlID_(dlg);
				break;
			}
			break;
		case 0x405:
			RdyTFrame(dlg);
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluRdyT_BINDLG_Loop);
FAIL_STUB_PATCH(gluRdyZ_BINDLG_Loop);
FAIL_STUB_PATCH(gluRdyP_BINDLG_Loop);

void DisplayEstablishingShot_();
int ContinueCampaign_(int a1);

void loadMenu_gluRdy(MusicTrack music_track, const char* bin_path, bool __fastcall BINDLG_Loop(dialog* dlg, struct dlgEvent* evt))
{
	stopMusic();
	DLGMusicFade(music_track);
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
			stopMusic();
			DLGMusicFade(MT_TITLE);
			break;
		case 19:
			ContinueCampaign_(1);
			break;
		case 100:
			stopMusic();
			DLGMusicFade(MT_TITLE);
			break;
		case 101:
			gwGameMode = GAME_RUNINIT;
			break;
		default:
			glGluesMode = GLUE_MAIN_MENU;
			stopMusic();
			DLGMusicFade(MT_TITLE);
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

bool __fastcall ConnSel_Interact_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			getGameList(dlg);
			DLG_SwishIn(dlg);
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

void sub_4B9BF0_(dialog* dlg)
{
	if (sub_4D4130())
	{
		dlg->fields.dlg.pModalFcn = (bool(__fastcall*)(dialog*))sub_4B9B10;
	}
	else
	{
		getErrorStringPair(STAR_EDIT_NOT_FOUND, 557);
	}
}

FAIL_STUB_PATCH(sub_4B9BF0);

bool IsCursorWithin(pt cursor, rect rectangle)
{
	return rectangle.left <= cursor.x && cursor.x <= rectangle.right && rectangle.top <= cursor.y && cursor.y <= rectangle.bottom;
}

void gluChat_HoverMinimapPreview_(dialog* dlg)
{
	dword_5999DC = 0;
	dword_5999D0 = 1;
	dword_5993AC = 1;
	load_MinimapPreview();
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
			gluChat_init(dlg);
			DLG_SwishIn(dlg);
			genericDlgInteract(dlg, evt);
			sub_4B9BF0_(dlg);
			return 1;
		case EventUser::USER_DESTROY:
			sub_4B8D70(dlg);
			sub_4B8D90(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			return gluChat_controlActivation(LastControlID, dlg);
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

void loadMenu_gluLoad_()
{
	glu_load_Dlg = loadAndInitFullMenuDLG_("rez\\gluLoad.bin");

	switch (gluLoadBINDlg_(glu_load_Dlg, gluLoad_Main))
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
			if (LoadSaveGameBIN_Main(dword_59B75C, Players[g_LocalNationID].nRace))
			{
				dlg->pszText = (char*)get_GluAll_String((GluAllTblEntry)177);
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
		gluScore_Tab,
		gluScore_Tab,
		gluScore_Tab,
		gluScore_Tab,
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
			sub_4B4600(dlg);
			DLG_SwishIn(dlg);
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
		const char* v1 = (const char*)(*networkTable > 0x47u ? ((char*)networkTable + networkTable[72]) : "");
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
		updateActiveCampaignMission();
		if (active_campaign_menu_entry && active_campaign_menu_entry->next_mission)
		{
			sub_4DBEE0_((ExpandedCampaignMenuEntry*) active_campaign_menu_entry + 1);
		}
	}
}

FAIL_STUB_PATCH(sub_4DBF80);

void loadMenu_gluScore_()
{
	char v25[260];

	ApplyGameVictoryStatus(dword_59B73C, &dword_59B3D0);
	int v0 = 1;
	dword_59B75C = dword_59B3D0 == 1;
	if (!dword_6D5A60)
	{
		v0 = (dword_59B3D0 == 1) + 2 * Players[g_LocalNationID].nRace;
	}
	char* v1 = off_512A98[v0];
	char** v2 = &off_512A98[v0];
	glGluesMode = dword_512AB0[v0];
	int v3 = byte_59B628 - v1;

	char v4;
	do
	{
		v4 = *v1;
		v1[v3] = *v1;
		++v1;
	} while (v4);

	DLGMusicFade(dword_512AC8[v0]);
	int v5 = 0;
	char v6;
	do
	{
		v6 = byte_59B628[v5];
		v25[v5++] = v6;
	} while (v6);

	char* v7 = v25 - 1;
	while (*++v7);
	strcpy(v7, "iScore.grp");
	dword_59B72C = (void*)LoadGraphic(v25, 0, "Starcraft\\SWAR\\lang\\gluScore.cpp", 1376);

	int v9 = 0;
	char v10;
	do
	{
		v10 = byte_59B628[v9];
		v25[v9++] = v10;
	} while (v10);
	char* v11 = v25 - 1;
	while (*++v11);
	strcpy(v11, "tminimap.pcx");
	if (!SBmpLoadImage(v25, 0, byte_59B730, 12, 0, 0, 0))
	{
		SysWarn_FileNotFound(v25, SErrGetLastError());
	}
	char* v13 = *v2;
	int v14 = byte_59B628 - *v2;
	char v15;
	do
	{
		v15 = *v13;
		v13[v14] = *v13;
		++v13;
	} while (v15);
	sub_4BCA80(SFX_glue_scorefill);

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
			const char* v3;
			if (*networkTable > 71u)
				v3 = (char *)networkTable + networkTable[72];
			else
				v3 = "";
			SStrCopy(playerName, v3, 25u);
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
			loadMenu_gluModem();
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
		char* v0 = (*networkTable > 0x6Du) ? (char*)networkTable + networkTable[110] : "";
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

		if (!sendInputToAllDialogs(&event) && input_procedures[a4])
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

		if (!sendInputToAllDialogs(&event) && input_procedures[a4])
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

		if (!sendInputToAllDialogs(&event) && input_procedures[a4])
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
	if (!sendInputToAllDialogs(&v3) && input_procedures[wNo])
	{
		input_procedures[wNo](&v3);
	}
}

FAIL_STUB_PATCH(Game_MouseWheel);

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
			if (!sendInputToAllDialogs(&v16))
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
			if (!sendInputToAllDialogs(&v16) && input_procedures[EventNo::EVN_KEYUP])
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
				HIBYTE(v16.wUnk_0x0A) |= 1u;
			}
			if (!sendInputToAllDialogs(&v16) && input_procedures[EventNo::EVN_CHAR])
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
				if (!sendInputToAllDialogs(&v16) && input_procedures[EventNo::EVN_CHAR])
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
				if (!sendInputToAllDialogs(&v16) && input_procedures[EventNo::EVN_CHAR])
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
				if (!sendInputToAllDialogs(&v16) && input_procedures[EventNo::EVN_CHAR])
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
		Game_Capturechanged();
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

	const char* v0 = is_expansion_installed ? "Brood War" : "Starcraft";
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

void loadInitCreditsBIN_(const char* a1)
{
	char buff[MAX_PATH];
	_snprintf(buff, MAX_PATH, "rez\\%s.txt", a1);

	dword_51CEA8 = (char*)fastFileRead_(&bytes_read, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	dword_51CEBC = dword_51CEA8;
	dword_51CEB8 = bytes_read;
	credits_interrupted = 0;

	dialog* credits_bin = LoadDialog("rez\\credits.bin");
	gluLoadBINDlg_(credits_bin, creditsDlgInteract);
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
		DLGMusicFade(active_campaign->epilog_music_track);
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

	DLGMusicFade(MT_TERRAN2);
	credits_interrupted = 0;
	if (is_expansion_installed)
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
			BWFXN_videoLoop(3);
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
					AppExit(0);
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

unsigned int LocalGetLang_()
{
	CHAR Buffer[16];
	char *v2;

	if (hModule && LoadStringA(hModule, 3u, Buffer, 16) || LoadStringA(hInst, 3u, Buffer, 16))
		return strtoul(Buffer, &v2, 16);
	else
		return 1033;
}

FUNCTION_PATCH(LocalGetLang, LocalGetLang_);

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
	dword_6509AC->container.dwExecutionFlags |= 4u;
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
		a1->flags |= 1u;
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
		if (a1->number2 == 7)
		{
			v7 = a1->number;
		}
		else if (a1->number2 == 8)
		{
			v7 = a1->time + a1->number;
		}
		else if (a1->number2 == 9)
		{
			if (a1->time >= a1->number)
			{
				v7 = a1->time - a1->number;
			}
			else
			{
				v7 = 0;
			}
		}
		else
		{
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
		if (MapStringTbl.buffer && a1->string)
		{
			if (a1->string - 1 < *MapStringTbl.buffer)
			{
				text_message = (char*)MapStringTbl.buffer + MapStringTbl.buffer[a1->string];
			}
			else
			{
				text_message = "";
			}
		}
		else
		{
			text_message = 0;
		}

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
				(LocationTable[a1->location - 1].dimensions.left + LocationTable[a1->location - 1].dimensions.right - SCREEN_WIDTH) / 2,
				(LocationTable[a1->location - 1].dimensions.top + LocationTable[a1->location - 1].dimensions.bottom - (SCREEN_HEIGHT - 80)) / 2);
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
			int v11 = (LocationTable[a1->location - 1].dimensions.left + LocationTable[a1->location - 1].dimensions.right - SCREEN_WIDTH) / 2;
			int v12 = (LocationTable[a1->location - 1].dimensions.top + LocationTable[a1->location - 1].dimensions.bottom - (SCREEN_HEIGHT - 80)) / 2;
			if (v11 >= 0)
			{
				if (v11 + 640 >= (unsigned __int16)map_width_pixels)
				{
					v11 = (unsigned __int16)map_width_pixels - 641;
				}
			}
			else
			{
				v11 = 0;
			}
			if (v12 >= 0)
			{
				if (v12 + 400 >= (unsigned __int16)map_height_pixels)
				{
					v12 = (unsigned __int16)map_height_pixels - 401;
				}
			}
			else
			{
				v12 = 0;
			}
			assignCenterViewProc(v12, v11, defCenterViewProc);
		}
		a1->flags |= 1u;
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
	if (a1->number2 == 4)
	{
		dword_58D708 = 1;
	}
	else if (a1->number2 == 5)
	{
		dword_58D708 = 0;
	}
	else if (a1->number2 == 6)
	{
		dword_58D708 ^= 1;
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
		if (MapStringTbl.buffer == 0 || a1->string == 0)
		{
			SStrCopy(next_scenario, 0, 32u);
		}
		else if (a1->string - 1 >= *MapStringTbl.buffer)
		{
			SStrCopy(next_scenario, empty_string, 32u);
		}
		else
		{
			SStrCopy(next_scenario, (char*)MapStringTbl.buffer + MapStringTbl.buffer[a1->string], 32u);
		}
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
	dword_6509AC->container.dwExecutionFlags |= 0x40u;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_DisableDebugMode);

int __fastcall TriggerAction_EnableDebugMode_(Action* a1)
{
	dword_6509AC->container.dwExecutionFlags &= ~0x40u;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_EnableDebugMode);

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
	TriggerAction_LeaderBoard_,
	TriggerAction_LeaderBoard_,
	TriggerAction_LeaderBoard_,
	TriggerAction_LeaderBoard_,
	TriggerAction_LeaderBoard_,
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
	TriggerAction_LeaderBoard_,
	TriggerAction_LeaderBoard_,
	TriggerAction_LeaderBoard_,
	TriggerAction_LeaderBoard_,
	TriggerAction_LeaderBoard_,
	TriggerAction_MoveLocation,
	TriggerAction_MoveUnit,
	TriggerAction_LeaderBoard_,
	TriggerAction_SetNextScenario_,
	TriggerAction_SetDoodadState_,
	TriggerAction_SetInvincibility,
	TriggerAction_CreateUnitWithProperties,
	TriggerAction_SetDeaths_,
	TriggerAction_Order,
	TriggerAction_NoAction_,
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

FUNCTION_PATCH(BWFXN_ExecuteGameTriggers, BWFXN_ExecuteGameTriggers_);
