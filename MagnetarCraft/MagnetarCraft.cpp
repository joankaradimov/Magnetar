#include <Windows.h>
#include <ShlObj.h>
#include <mbstring.h>
#include <stdio.h>
#include "../MemoryModule/MemoryModule.h"
#include "starcraft.h"
#include "magnetorm.h"
#include <exception>
#include <sstream>
#include <filesystem>
#include <fstream>
#include <yaml-cpp/yaml.h>
#include "patching/patching.h"

void* const STARCRAFT_IMAGE_BASE = (void*)0x400000;
const int STARCRAFT_IMAGE_SIZE = 0x2ec000;
void* const STARCRAFT_IMAGE_END = (char*)STARCRAFT_IMAGE_BASE + STARCRAFT_IMAGE_SIZE;
WORD EXPECTED_MAJOR_VERSION = 1;
WORD EXPECTED_MINOR_VERSION = 16;
WORD EXPECTED_PATCH_VERSION = 1;

#pragma section(".scimg", read, write)
__declspec(allocate(".scimg")) char scimg[STARCRAFT_IMAGE_SIZE * 2];

class FileInfo
{
public:
	FileInfo(const char* path): version_data(nullptr)
	{
		DWORD _unused;
		int version_info_size = GetFileVersionInfoSizeA(path, &_unused);
		if (version_info_size)
		{
			version_data = malloc(version_info_size);
			if (GetFileVersionInfoA(path, 0, version_info_size, version_data))
			{
				VS_FIXEDFILEINFO* file_info_buffer;
				unsigned int file_info_buffer_length;
				if (VerQueryValueA(version_data, "\\", (LPVOID*)&file_info_buffer, &file_info_buffer_length))
				{
					file_info = file_info_buffer;
				}
			}
		}
	}

	~FileInfo()
	{
		if (version_data)
		{
			free(version_data);
		}
	}

	bool is_valid()
	{
		return version_data != nullptr;
	}

	VS_FIXEDFILEINFO* operator ->()
	{
		return file_info;
	}

private:
	void* version_data;
	VS_FIXEDFILEINFO* file_info;
};

class StarCraftExecutable
{
public:
	StarCraftExecutable(const char* path) : module(nullptr), file_info(path), executable_path(path)
	{
		if (scimg > STARCRAFT_IMAGE_BASE || scimg + sizeof(scimg) < STARCRAFT_IMAGE_END)
		{
			std::ostringstream error_message;
			error_message << "Could not reserve memory at base address 0x" << std::hex << STARCRAFT_IMAGE_BASE << " for " << executable_path.filename();
			throw std::exception(error_message.str().c_str());
		}

		module = LoadLibraryA(executable_path.generic_string().c_str());

		if (module == nullptr)
		{
			std::ostringstream error_message;
			error_message << "Could not load " << executable_path.filename();
			throw std::exception(error_message.str().c_str());
		}
	}

	~StarCraftExecutable()
	{
		if (module != nullptr)
		{
			FreeLibrary(module);
		}
	}

	HMODULE GetModule()
	{
		return module;
	}

	void check()
	{
		if (!file_info.is_valid())
		{
			std::ostringstream error_message;
			error_message << "Could not retrieve version info for " << executable_path.filename();
			throw std::exception(error_message.str().c_str());
		}

		if (HIWORD(file_info->dwProductVersionMS) != EXPECTED_MAJOR_VERSION ||
			LOWORD(file_info->dwProductVersionMS) != EXPECTED_MINOR_VERSION ||
			HIWORD(file_info->dwProductVersionLS) != EXPECTED_PATCH_VERSION)
		{
			std::ostringstream error_message;
			error_message << "Expected " << executable_path.filename() << ' ' << EXPECTED_MAJOR_VERSION << '.' << EXPECTED_MINOR_VERSION << '.' << EXPECTED_PATCH_VERSION;
			error_message << "; found " << HIWORD(file_info->dwProductVersionMS) << '.' << LOWORD(file_info->dwProductVersionMS) << '.' << HIWORD(file_info->dwProductVersionLS);
			throw std::exception(error_message.str().c_str());
		}

		HMEMORYMODULE starcraftModule = MemoryLoadLibraryEx(module, STARCRAFT_IMAGE_SIZE, VirtualAlloc, VirtualFree, LoadLibrary, MemoryDefaultGetProcAddress, MemoryDefaultFreeLibrary, NULL);
		if (starcraftModule == NULL)
		{
			std::ostringstream error_message;
			error_message << "Could not initialize " << executable_path.filename() << " as a library";
			throw std::exception(error_message.str().c_str());
		}
	}

private:

	static LPVOID VirtualAlloc(LPVOID address, SIZE_T size, DWORD allocationType, DWORD protect, void* userdata) {
		if (address < STARCRAFT_IMAGE_BASE || address >= STARCRAFT_IMAGE_END)
		{
			return nullptr; // TODO: alloc, maybe
		}
		DWORD old_protect;
		BOOL success = VirtualProtect(address, size, protect, &old_protect);
		if (success)
		{
			return address;
		}
		else
		{
			return nullptr;
		}
	}

	static BOOL VirtualFree(LPVOID address, SIZE_T size, DWORD freeType, void* userdata) {
		if (address < STARCRAFT_IMAGE_BASE || address >= STARCRAFT_IMAGE_END)
		{
			return ::VirtualFree(address, size, freeType);
		}
		DWORD old_protect;
		return VirtualProtect(address, size, MEM_RESET, &old_protect);
	}

	static HCUSTOMMODULE LoadLibrary(LPCSTR filename, void* userdata)
	{
		UNREFERENCED_PARAMETER(userdata);
		if (!strcmp(filename, "storm.dll"))
		{
			filename = "Magnetorm.dll";
		}
		return LoadLibraryA(filename);
	}

	std::filesystem::path executable_path; // e.g. "C:\\Program Files\\StarCraft.exe"
	HMODULE module;
	FileInfo file_info;
};

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

const char* CONFIG_FILE = "MagnetarCraft.yml";

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) try
{
	bool starcraft_root_manually_selected = false;
	YAML::Node config;
	try
	{
		config = YAML::LoadFile(CONFIG_FILE);
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
	std::ofstream(CONFIG_FILE) << config;

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
