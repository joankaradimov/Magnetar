#include "starcraft_executable.h"

#include <sstream>

void* const STARCRAFT_IMAGE_BASE = (void*)0x400000;
const int STARCRAFT_IMAGE_SIZE = 0x2ec000;
void* const STARCRAFT_IMAGE_END = (char*)STARCRAFT_IMAGE_BASE + STARCRAFT_IMAGE_SIZE;

WORD EXPECTED_MAJOR_VERSION = 1;
WORD EXPECTED_MINOR_VERSION = 16;
WORD EXPECTED_PATCH_VERSION = 1;

#pragma section(".scimg", read, write)
__declspec(allocate(".scimg")) char scimg[STARCRAFT_IMAGE_SIZE * 2];

StarCraftExecutable::StarCraftExecutable(const char* path) : module(nullptr), file_info(path), executable_path(path)
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

StarCraftExecutable::~StarCraftExecutable()
{
	if (module != nullptr)
	{
		FreeLibrary(module);
	}
}

HMODULE StarCraftExecutable::GetModule()
{
	return module;
}

void StarCraftExecutable::check()
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

LPVOID StarCraftExecutable::VirtualAlloc(LPVOID address, SIZE_T size, DWORD allocationType, DWORD protect, void* userdata) {
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

BOOL StarCraftExecutable::VirtualFree(LPVOID address, SIZE_T size, DWORD freeType, void* userdata) {
	if (address < STARCRAFT_IMAGE_BASE || address >= STARCRAFT_IMAGE_END)
	{
		return ::VirtualFree(address, size, freeType);
	}
	DWORD old_protect;
	return VirtualProtect(address, size, MEM_RESET, &old_protect);
}

HCUSTOMMODULE StarCraftExecutable::LoadLibrary(LPCSTR filename, void* userdata)
{
	UNREFERENCED_PARAMETER(userdata);
	if (!strcmp(filename, "storm.dll"))
	{
		filename = "Magnetorm.dll";
	}
	return LoadLibraryA(filename);
}
