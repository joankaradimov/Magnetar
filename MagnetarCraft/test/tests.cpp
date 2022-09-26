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

	PreInitData_();
	CreateMainWindow_();
	audioVideoInit_();
	CpuThrottle = 0;

	LoadInitIscriptBIN_();
	AppAddExit_(CleanupIscriptBINHandle_);
	for (int i = 0; i < _countof(byte_50CDC1); ++i)
	{
		byte_50CDC1[i] = i;
	}

	gwGameMode = GAME_RUN;
	strcpy_s(playerName, "Tester");

	char* replays[] = {
		// TODO: read the replay test fixtures from a folder
		"Maps\\replays\\LastReplay.rep",
	};

	for (char* replay : replays)
	{
		InReplay = 1;
		IsExpansion = 1;
		LoadReplayFile(replay, 0);

		MapChunks chunks;
		ReadMapData_(replay, &chunks, 0);

		strcpy_s(Players[g_LocalNationID].szName, "Tester");
		IsExpansion = replay_header.is_expansion;

		CreateGame_(&replay_header.game_data);
		GameRun_();

		// TODO: persist the replay game speed between games [when rendering frames]
		// TODO: save the final game state and compare it with a predefined state

		printf(".");
	}
	printf("\nDone!\n");

	return 0;
}
