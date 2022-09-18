#include <filesystem>
#include <Windows.h>

#include "../MemoryModule/MemoryModule.h"
#include "file_info.h"

class StarCraftExecutable
{
public:
	StarCraftExecutable(const char* path);
	~StarCraftExecutable();

	std::filesystem::path GetParentDirectory();
	HMODULE GetModule();
	void check();

private:
	static LPVOID VirtualAlloc(LPVOID address, SIZE_T size, DWORD allocationType, DWORD protect, void* userdata);
	static BOOL VirtualFree(LPVOID address, SIZE_T size, DWORD freeType, void* userdata);
	static HCUSTOMMODULE LoadLibrary(LPCSTR filename, void* userdata);

	std::filesystem::path executable_path; // e.g. "C:\\Program Files\\StarCraft.exe"
	HMODULE module;
	FileInfo file_info;
};
