#include "targetver.h"
#include "magnetorm.h"

void ReinitializeStormFileIO()
{
	HINSTANCE storm_dll = LoadLibrary(L"storm.dll");
	void** storm_1505E5E4 = (void**)((char*)storm_dll + 0x5E5E4);
	*storm_1505E5E4 = 0;
}
