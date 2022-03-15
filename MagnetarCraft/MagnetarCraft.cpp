#include <Windows.h>
#include <stdio.h>
#include "../MemoryModule/MemoryModule.h"
#include "starcraft.h"
#include <exception>
#include "patching/BasePatch.h"
#include "patching/FailStubPatch.h"

void* const STARCRAFT_IMAGE_BASE = (void*)0x400000;
const int STARCRAFT_IMAGE_SIZE = 0x2ec000;
void* const STARCRAFT_IMAGE_END = (char*)STARCRAFT_IMAGE_BASE + STARCRAFT_IMAGE_SIZE;

#pragma section(".scimg", read, write)
__declspec(allocate(".scimg")) char scimg[STARCRAFT_IMAGE_SIZE * 3];

static LPVOID _VirtualAlloc(LPVOID address, SIZE_T size, DWORD allocationType, DWORD protect, void* userdata) {
	if (address < STARCRAFT_IMAGE_BASE || address >= STARCRAFT_IMAGE_END) {
		return nullptr; // TODO: alloc, maybe
	}
	DWORD old_protect;
	BOOL success = VirtualProtect(address, size, protect, &old_protect);
	if (success) {
		return address;
	}
	else {
		return nullptr;
	}
}

static HCUSTOMMODULE _LoadLibrary(LPCSTR filename, void *userdata)
{
	UNREFERENCED_PARAMETER(userdata);
	if (!strcmp(filename, "storm.dll")) {
		filename = "Magnetorm.dll";
	}
	return LoadLibraryA(filename);
}

static BOOL _VirtualFree(LPVOID address, SIZE_T size, DWORD freeType, void* userdata) {
	if (address < STARCRAFT_IMAGE_BASE || address >= STARCRAFT_IMAGE_END) {
		return VirtualFree(address, size, freeType);
	}
	DWORD old_protect;
	return VirtualProtect(address, size, MEM_RESET, &old_protect);
}

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
		AppExit(1);
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
			AppExit(1);
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

FailStubPatch VerifySystemMemory_patch(VerifySystemMemory);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd) try
{
	if (scimg > STARCRAFT_IMAGE_BASE || scimg + sizeof(scimg) < STARCRAFT_IMAGE_END) {
		throw std::exception("Could not reserve memory at base address 0x400000 for Starcraft.exe");
	}

	HINSTANCE starcraftExeData = LoadLibrary(L"Starcraft.exe");

	if (starcraftExeData == NULL) {
		throw std::exception("Could not load starcraft.exe to memory");
	}

	HMEMORYMODULE starcraftModule = MemoryLoadLibraryEx(starcraftExeData, STARCRAFT_IMAGE_SIZE, _VirtualAlloc, _VirtualFree, _LoadLibrary, MemoryDefaultGetProcAddress, MemoryDefaultFreeLibrary, NULL);
	if (starcraftModule == NULL) {
		throw std::exception("Could not initialize starcraft.exe as a library");
	}

	init_stacraftexe_clib();
	BasePatch::apply_pending_patches();

	hInst = starcraftExeData;
	main_thread_id = GetCurrentThreadId();
	CheckForOtherInstances("SWarClass");
	localDll_Init_(hInst);
	VerifySystemMemory_();
	FastIndexInit();
	BWSetSecurityInfo();
	GameMainLoop_();

	return 0;
}
catch (const std::exception& e) {
	report_error(e.what());
	return 1;
}
