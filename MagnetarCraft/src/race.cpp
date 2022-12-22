#include "race.h"

const std::vector<Race>& Race::races() {
	static std::vector<Race> races =
	{
		Race(
			'z',
			UnitType::Zerg_Hatchery,
			UnitType::Zerg_Drone,
			"dlgs\\zerg.grp",
			MenuPosition::GLUE_READY_Z,
			MenuPosition::GLUE_SCORE_Z_VICTORY,
			MenuPosition::GLUE_SCORE_Z_DEFEAT,
			{ "music\\zrdyroom.wav", MusicTrackType::MENU_MUSIC, 1, 0 },
			{ "music\\zvict.wav", MusicTrackType::MENU_MUSIC, 1, 0 },
			{ "music\\zdefeat.wav", MusicTrackType::MENU_MUSIC, 1, 0 },
			{
				{"music\\zerg1.wav", MusicTrackType::IN_GAME_MUSIC, 0, 1},
				{"music\\zerg2.wav", MusicTrackType::IN_GAME_MUSIC, 0, 2},
				{"music\\zerg3.wav", MusicTrackType::IN_GAME_MUSIC, 0, 0},
			}
		),

		Race(
			't',
			UnitType::Terran_Command_Center,
			UnitType::Terran_SCV,
			"dlgs\\terran.grp",
			MenuPosition::GLUE_READY_T,
			MenuPosition::GLUE_SCORE_T_VICTORY,
			MenuPosition::GLUE_SCORE_T_DEFEAT,
			{ "music\\trdyroom.wav", MusicTrackType::MENU_MUSIC, 1, 0 },
			{ "music\\tvict.wav", MusicTrackType::MENU_MUSIC, 0, 0 },
			{ "music\\tdefeat.wav", MusicTrackType::MENU_MUSIC, 0, 0 },
			{
				{"music\\terran1.wav", MusicTrackType::IN_GAME_MUSIC, 0, 1},
				{"music\\terran2.wav", MusicTrackType::IN_GAME_MUSIC, 0, 2},
				{"music\\terran3.wav", MusicTrackType::IN_GAME_MUSIC, 0, 0},
			}
		),

		Race(
			'p',
			UnitType::Protoss_Nexus,
			UnitType::Protoss_Probe,
			"dlgs\\protoss.grp",
			MenuPosition::GLUE_READY_P,
			MenuPosition::GLUE_SCORE_P_VICTORY,
			MenuPosition::GLUE_SCORE_P_DEFEAT,
			{ "music\\prdyroom.wav", MusicTrackType::MENU_MUSIC, 1, 0 },
			{ "music\\pvict.wav", MusicTrackType::MENU_MUSIC, 0, 0 },
			{ "music\\pdefeat.wav", MusicTrackType::MENU_MUSIC, 0, 0 },
			{
				{"music\\protoss1.wav", MusicTrackType::IN_GAME_MUSIC, 0, 1},
				{"music\\protoss2.wav", MusicTrackType::IN_GAME_MUSIC, 0, 2},
				{"music\\protoss3.wav", MusicTrackType::IN_GAME_MUSIC, 0, 0},
			}
		),
	};

	return races;
}

