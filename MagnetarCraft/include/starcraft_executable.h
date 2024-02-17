#include <filesystem>
#include <Windows.h>

#include "MemoryModule.h"
#include "file_info.h"

class StarCraftExecutable
{
public:
	StarCraftExecutable(const std::filesystem::path& path);
	~StarCraftExecutable();

	std::filesystem::path GetParentDirectory();
	HMODULE GetModule();
	virtual void check();

protected:
	static LPVOID VirtualAlloc(LPVOID address, SIZE_T size, DWORD allocationType, DWORD protect, void* userdata);
	static BOOL VirtualFree(LPVOID address, SIZE_T size, DWORD freeType, void* userdata);
	static HCUSTOMMODULE LoadLibrary(LPCSTR filename, void* userdata);

	std::filesystem::path executable_path; // e.g. "C:\\Program Files\\StarCraft.exe"
	HMODULE module;
	FileInfo file_info;
};

// TODO: fix this horrbile hack
// There should be a base class for StarCraft/WarCraft2 executables
// And all these StarCraft-specific global constants should be class-level
class WarCraft2Executable : public StarCraftExecutable
{
public:
	WarCraft2Executable(const std::filesystem::path& path);
	virtual void check();
};
