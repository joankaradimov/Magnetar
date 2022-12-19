#include "race.h"

const std::vector<Race> Race::races =
{
	Race(
		'z',
		UnitType::Zerg_Hatchery,
		UnitType::Zerg_Drone,
		MenuPosition::GLUE_READY_Z,
		MenuPosition::GLUE_SCORE_Z_VICTORY,
		MenuPosition::GLUE_SCORE_Z_DEFEAT
	),

	Race(
		't',
		UnitType::Terran_Command_Center,
		UnitType::Terran_SCV,
		MenuPosition::GLUE_READY_T,
		MenuPosition::GLUE_SCORE_T_VICTORY,
		MenuPosition::GLUE_SCORE_T_DEFEAT
	),

	Race(
		'p',
		UnitType::Protoss_Nexus,
		UnitType::Protoss_Probe,
		MenuPosition::GLUE_READY_P,
		MenuPosition::GLUE_SCORE_P_VICTORY,
		MenuPosition::GLUE_SCORE_P_DEFEAT
	),
};
