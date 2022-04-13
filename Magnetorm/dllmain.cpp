#include "targetver.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

extern "C" BOOL StormLibFileDestroy();

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		// An initial call to SFileDestroy is needed to avoid corruptions in SFile* functions
		// see: https://github.com/bwapi/bwapi/issues/375#issuecomment-233162808
		StormLibFileDestroy();
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}
