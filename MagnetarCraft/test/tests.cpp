#include <Windows.h>

#include "starcraft_executable.h"
#include "magnetorm.h"
#include "MagnetarCraft.h"
#include "patching.h"
#include "starcraft.h"

int main()
{
	// TODO: remove hard-coded paths
	SetDllDirectoryA("C:\\games\\Starcraft-1.16");
	StarCraftExecutable* starcraft_exe = new StarCraftExecutable("C:\\games\\Starcraft-1.16\\StarCraft.exe");
	starcraft_exe->check();
	SFileDestroy();

	init_stacraftexe_clib();
	BasePatch::apply_pending_patches();

	hInst = starcraft_exe->GetModule();
	main_thread_id = GetCurrentThreadId();
	localDll_Init_(hInst);
	FastIndexInit_();
	BWSetSecurityInfo();
	GameMainLoop_();

	return 0;
}
