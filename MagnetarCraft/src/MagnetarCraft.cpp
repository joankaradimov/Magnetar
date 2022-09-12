#include <Windows.h>
#include <ShlObj.h>
#include <mbstring.h>
#include <stdio.h>
#include "starcraft.h"
#include "magnetorm.h"
#include <exception>
#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "patching.h"
#include "starcraft_executable.h"

void init_stacraftexe_clib()
{
	int(*_ioinit)() = (int(*)())0x405725;
	int(*_mtinit)() = (int(*)())0x405BCB;
	int(*_heap_init)(int) = (int(*)(int))0x405CD4;
	int(*_cinit)(int) = (int(*)(int))0x404E50;
	void(*_RTC_Initialize)() = (void(*)())0x00405923;

	if (!_heap_init(1))
		exit(0x1Cu);
	if (!_mtinit())
		exit(0x10u);
	_RTC_Initialize();
	if (_ioinit() < 0)
		exit(0x1Bu);
	int v6 = _cinit(1);
	if (v6)
		exit(v6);
}

void report_error(const char* error_message)
{
	va_list myargs;
	va_start(myargs, error_message);
	MessageBoxA(NULL, error_message, NULL, MB_ICONSTOP);
	va_end(myargs);
}

int VerifySystemMemory_()
{
	double v0;
	struct _SYSTEM_INFO SystemInfo;
	struct _MEMORYSTATUS Buffer;
	DWORD TotalNumberOfClusters;
	DWORD NumberOfFreeClusters;
	DWORD BytesPerSector;
	DWORD SectorsPerCluster;

	GetSystemInfo(&SystemInfo);
	if (!SystemInfo.wProcessorArchitecture && SystemInfo.dwProcessorType < 486 && !SystemWarning_PentiumProcessor()
		|| GetDiskFreeSpaceA(RootPathName, &SectorsPerCluster, &BytesPerSector, &NumberOfFreeClusters, &TotalNumberOfClusters)
		&& (v0 = (double)SectorsPerCluster * 0.0009765625 * ((double)BytesPerSector * 0.0009765625) * (double)NumberOfFreeClusters, v0 < 20.0)
		&& !SystemWarning_20MBDiskSpace((unsigned __int64)v0))
	{
		SErrSuppressErrors(1);
		AppExit_(1);
		ProcError(1);
		exit(1);
	}

	Buffer.dwLength = 32;
	GlobalMemoryStatus(&Buffer);
	if (Buffer.dwTotalPhys < 0xD00000 || Buffer.dwTotalPageFile < 0x1E00000)
	{
		int v1;
		if (Buffer.dwTotalPhys >= 0xD00000)
		{
			v1 = SystemWarning_Configuration();
		}
		else
		{
			v1 = SystemWarning_PhysicalMemory();
		}
		if (!v1)
		{
			SErrSuppressErrors(1);
			AppExit_(1);
			ProcError(1);
			exit(1);
		}
	}

	int value;
	if (SRegLoadValue("Starcraft", "ForceLowMem", 0, &value) && value != 0)
	{
		low_memory = 1;
		return value;
	}
	else
	{
		int result = SRegLoadValue("Starcraft", "ForceHighMem", 0, &value);
		if (result && (result = value) != 0)
		{
			low_memory = 0;
		}
		else
		{
			low_memory = Buffer.dwTotalPhys < 0x1600000;
		}
		return result;
	}
}

FAIL_STUB_PATCH(VerifySystemMemory);

HWND GetClassWindow_(const char* a1)
{
	char ClassName[512];

	HWND v1 = GetForegroundWindow();
	if (!v1)
	{
		return 0;
	}
	do
	{
		if (GetClassNameA(v1, ClassName, 512) && !_mbsicmp((const unsigned __int8*)ClassName, (const unsigned __int8*)a1))
		{
			break;
		}
		v1 = GetWindow(v1, 2u);
	} while (v1);
	return v1;
}

FAIL_STUB_PATCH(GetClassWindow);

void FastIndexInit_()
{
	AppAddExit_(destroyFileFindIndexer);

	HWND v0 = GetClassWindow_("MOM Parent");
	if (v0)
	{
		PostMessageA(v0, 0x10u, 0, 0);
		PostMessageA(v0, 0x12u, 0, 0);
	}
	dword_6D11D8 = v0 != 0;

	HWND v2 = GetClassWindow_("Find Fast Indexer");
	if (v2)
	{
		PostMessageA(v2, 0x10u, 0, 0);
		PostMessageA(v2, 0x12u, 0, 0);
	}
	dword_6D11DC = v2 != 0;
}

FAIL_STUB_PATCH(FastIndexInit);

std::string LocateStarCraft()
{
	BROWSEINFOA browse_Info;
	browse_Info.hwndOwner = nullptr;
	browse_Info.pidlRoot = NULL;
	browse_Info.pszDisplayName = NULL;
	browse_Info.lpszTitle = "Select a directory with a StarCraft 1.16.1 installation";
	browse_Info.ulFlags = BIF_USENEWUI | BIF_NONEWFOLDERBUTTON | BIF_UAHINT;
	browse_Info.lpfn = NULL;
	browse_Info.lParam = 0;
	browse_Info.iImage = -1;

	LPITEMIDLIST item = SHBrowseForFolderA(&browse_Info);

	if (item == nullptr)
	{
		exit(1);
	}

	char path[MAX_PATH];
	SHGetPathFromIDListA(item, path);
	return path;
}

// Patch clib functions that use FILE
FUNCTION_PATCH((void*)0x4116F5, _fread_nolock);
FUNCTION_PATCH((void*)0x4117DE, fread);
FUNCTION_PATCH((void*)0x41182A, _fwrite_nolock);
FUNCTION_PATCH((void*)0x411931, fwrite);
FUNCTION_PATCH((void*)0x41197D, ftell);
FUNCTION_PATCH((void*)0x411ADF, fseek);
FUNCTION_PATCH((void*)0x411B6E, _fseek_nolock);
FUNCTION_PATCH((void*)0x411BB7, fgetc);
FUNCTION_PATCH((void*)0x40D3C8, _fsopen);
FUNCTION_PATCH((void*)0x40D424, fopen);
FUNCTION_PATCH((void*)0x40D437, fclose);
FUNCTION_PATCH((void*)0x40D483, _fclose_nolock);
FUNCTION_PATCH((void*)0x40D983, _lock_file);
FUNCTION_PATCH((void*)0x40D9D5, _unlock_file);
FUNCTION_PATCH((void*)0x411619, setvbuf);
FUNCTION_PATCH((void*)0x40DE57, fflush);
FUNCTION_PATCH((void*)0x40DF5A, flushall);
FUNCTION_PATCH((void*)0x40EBA2, fcloseall);

std::filesystem::path GetExecutablePath()
{
	TCHAR buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, sizeof(buffer));

	return buffer;
}

std::filesystem::path GetExecutableFilename()
{
	return GetExecutablePath().filename();
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) try
{
	bool starcraft_root_manually_selected = false;
	std::string config_filename = GetExecutableFilename().replace_extension("yml").generic_string();
	YAML::Node config;
	try
	{
		config = YAML::LoadFile(config_filename);
	}
	catch (const YAML::BadFile& _exception)
	{
	}

	std::string starcraft_root;
	try
	{
		starcraft_root = config["starcraft-root"].as<std::string>();
	}
	catch (const YAML::TypedBadConversion<std::string>& e)
	{
		// TODO: try to find StarCraft path in registry, maybe?
		starcraft_root = LocateStarCraft();
		starcraft_root_manually_selected = true;
	}

	StarCraftExecutable* starcraft_exe = nullptr;
	while (true)
	{
		try
		{
			std::string starcraft_exe_path = starcraft_root + "\\StarCraft.exe";
			SetDllDirectoryA(starcraft_root.c_str());
			starcraft_exe = new StarCraftExecutable(starcraft_exe_path.c_str());

			// An initial call to SFileDestroy is needed to avoid corruptions in SFile* functions
			// see: https://github.com/bwapi/bwapi/issues/375#issuecomment-233162808
			SFileDestroy();

			starcraft_exe->check();

			break;
		}
		catch (const std::exception& e)
		{
			if (starcraft_exe)
			{
				delete starcraft_exe;
				starcraft_exe = nullptr;
			}
			if (starcraft_root_manually_selected)
			{
				report_error(e.what());
			}
			starcraft_root = LocateStarCraft();
			starcraft_root_manually_selected = true;
		}
	}

	config["starcraft-root"] = starcraft_root;
	std::ofstream(config_filename) << config;

	init_stacraftexe_clib();
	BasePatch::apply_pending_patches();

	ScrSize.right = SCREEN_WIDTH;
	ScrSize.bottom = SCREEN_HEIGHT;
	ScrLimit.right = SCREEN_WIDTH - 1;
	ScrLimit.bottom = SCREEN_HEIGHT - 1;

	hInst = starcraft_exe->GetModule();
	main_thread_id = GetCurrentThreadId();
	CheckForOtherInstances("SWarClass");
	localDll_Init_(hInst);
	VerifySystemMemory_();
	FastIndexInit_();
	BWSetSecurityInfo();
	GameMainLoop_();

	return 0;
}
catch (const std::exception& e) {
	report_error(e.what());
	return 1;
}
