#include <fstream>
#include <filesystem>
#include <gtest/gtest.h>
#include <Windows.h>

#include "starcraft_executable.h"
#include "magnetorm.h"
#include "MagnetarCraft.h"
#include "patching.h"
#include "starcraft.h"

bool files_match(const std::filesystem::path& path1, const std::filesystem::path& path2) {
	std::ifstream file1(path1, std::ifstream::binary | std::ifstream::ate);
	std::ifstream file2(path2, std::ifstream::binary | std::ifstream::ate);

	if (file1.fail() || file2.fail() || file1.tellg() != file2.tellg())
	{
		return false;
	}

	file1.seekg(0, std::ifstream::beg);
	file2.seekg(0, std::ifstream::beg);

	return std::equal(std::istreambuf_iterator<char>(file1.rdbuf()), std::istreambuf_iterator<char>(), std::istreambuf_iterator<char>(file2.rdbuf()));
}

class ReplayTest : public testing::TestWithParam<std::filesystem::path> {
public:
	ReplayTest()
	{
		std::filesystem::path entry = GetParam();

		InReplay = 1;
		IsExpansion = 1;
		LoadReplayFile_(entry.generic_string().c_str(), 0);

		MapChunks chunks;
		ReadMapData_(entry.generic_string().c_str(), &chunks, 0);

		strcpy_s(Players[g_LocalNationID].szName, "Tester");
		IsExpansion = replay_header.is_expansion;

		CreateGame_(&replay_header.game_data);
	}

	static void SetUpTestSuite()
	{
		frame_capping = false;
		has_viewport = false;
		has_hud = false;
		end_mission_prompt = false;
		keep_app_active_in_background = true;

		// TODO: remove hard-coded paths
		SetDllDirectoryA("D:\\games\\Starcraft-1.16");
		StarCraftExecutable* starcraft_exe = new StarCraftExecutable("D:\\games\\Starcraft-1.16\\StarCraft.exe");
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
		if (has_viewport)
		{
			audioVideoInit_();
		}
		CpuThrottle = 0;

		LoadInitIscriptBIN_();
		AppAddExit_(CleanupIscriptBINHandle_);
		for (int i = 0; i < _countof(byte_50CDC1); ++i)
		{
			byte_50CDC1[i] = i;
		}

		gwGameMode = GAME_RUN;
		strcpy_s(playerName, "Tester");
	}
};

TEST_P(ReplayTest, Replays) {
	std::filesystem::path replay_path = GetParam();

	on_end_game = [&replay_path]()
	{
		std::filesystem::path actual_state_path = replay_path.replace_extension("replay-actual");
		std::filesystem::path expected_state_path = replay_path.replace_extension("replay-expected");

		FILE* savegame_file = fopen(actual_state_path.generic_string().c_str(), "wb");
		// TODO: writeImages(savegame_file);
		writeSprites(savegame_file);
		// TODO: write thingys
		WriteFlingys(savegame_file);
		WriteUnits(savegame_file);
		WriteBullets(savegame_file);
		WriteOrders(savegame_file);
		fclose(savegame_file);

		if (!std::filesystem::exists(expected_state_path))
		{
			std::filesystem::rename(actual_state_path, expected_state_path);
		}
		else
		{
			ASSERT_TRUE(files_match(actual_state_path, expected_state_path));
		}

		if (files_match(actual_state_path, expected_state_path))
		{
			std::filesystem::remove(actual_state_path);
		}
	};

	GameRun_();
}

std::vector<std::filesystem::path> ReadTestCasesFromDisk() {
	std::string path = "..\\..\\..\\..\\MagnetarCraft\\test\\fixtures";
	std::vector<std::filesystem::path> result;

	for (const auto& entry : std::filesystem::directory_iterator(path))
	{
		if (entry.is_regular_file() && entry.path().extension() == ".rep")
		{
			result.push_back(entry);
		}
	}

	return result;
}

INSTANTIATE_TEST_SUITE_P(Replays, ReplayTest, testing::ValuesIn(ReadTestCasesFromDisk()), [](auto& info) {
	return info.param.filename().replace_extension("").generic_string();
});

int main(int argc, char** argv)
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
