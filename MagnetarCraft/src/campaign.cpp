#include "campaign.h"
#include "race.h"
#include "patching.h"

std::vector<Campaign> campaigns = {
	{
		"swterran",
		0,
		false,
		RaceId::RACE_Terran,
		{
			{0x33, ExpandedMapData::EMD_swtutorial, C_BLIZZARD_LOGO, RACE_Terran, 0, "Estt0tsw"},
			{0x34, ExpandedMapData::EMD_swterran01, C_BLIZZARD_LOGO, RACE_Terran, 0, "Estt01sw"},
			{0x35, ExpandedMapData::EMD_swterran02, C_BLIZZARD_LOGO, RACE_Terran, 0, "Estt02sw"},
			{0x36, ExpandedMapData::EMD_swterran03, C_BLIZZARD_LOGO, RACE_Terran, 0, "Estt03sw"},
			{0x37, ExpandedMapData::EMD_swterran04, C_BLIZZARD_LOGO, RACE_Terran, 0, "Estt04sw"},
			{0x38, ExpandedMapData::EMD_swterran05, C_BLIZZARD_LOGO, RACE_Terran, 0, "Estt05sw"},
			{0},
		},
		{"epilogsw"},
		&Race::races()[RaceId::RACE_Terran].ingame_music[1],
		MenuPosition::GLUE_CAMPAIGN,
	},
	{
		"terran",
		0,
		false,
		RaceId::RACE_Terran,
		{
			{1, ExpandedMapData::EMD_tutorial, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT0t"},
			{2, ExpandedMapData::EMD_terran01, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT01"},
			{3, ExpandedMapData::EMD_terran02, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT02"},
			{0xC, ExpandedMapData::EMD_terran03, C_WASTELAND_PATROL_INTRO, RACE_Terran, 0},
			{0, ExpandedMapData::EMD_terran03, C_WASTELAND_PATROL, RACE_Terran, 1},
			{4, ExpandedMapData::EMD_terran03, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT03"},
			{5, ExpandedMapData::EMD_terran04, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT04"},
			{6, ExpandedMapData::EMD_terran05, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT05"},
			{0xD, ExpandedMapData::EMD_terran06, C_THE_DOWNING_OF_NORAD_II_INTRO, RACE_Terran, 0},
			{0, ExpandedMapData::EMD_terran06, C_THE_DOWNING_OF_NORAD_II, RACE_Terran, 1},
			{7, ExpandedMapData::EMD_terran06, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT06"},
			{8, ExpandedMapData::EMD_terran08, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT08"},
			{9, ExpandedMapData::EMD_terran09, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT09"},
			{0xE, ExpandedMapData::EMD_terran11, C_OPEN_REBELION_INTRO, RACE_Terran, 0},
			{0, ExpandedMapData::EMD_terran11, C_OPEN_REBELION, RACE_Terran, 1},
			{0xA, ExpandedMapData::EMD_terran11, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT11"},
			{0xB, ExpandedMapData::EMD_terran12, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT12"},
			{0xF, ExpandedMapData::EMD_zerg01, C_THE_INAUGURATION_INTRO, RACE_Terran, 0},
			{0, ExpandedMapData::EMD_zerg01, C_THE_INAUGURATION, RACE_Terran, 1},
			{0},
		},
		{},
		nullptr,
		MenuPosition::GLUE_CAMPAIGN,
	},
	{
		"zerg",
		1,
		false,
		RaceId::RACE_Zerg,
		{
			{0x20, ExpandedMapData::EMD_zerg01, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ01"},
			{0x21, ExpandedMapData::EMD_zerg02, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ02"},
			{0x22, ExpandedMapData::EMD_zerg03, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ03"},
			{0x2A, ExpandedMapData::EMD_zerg04, C_THE_DREAM, RACE_Zerg, 0},
			{0x23, ExpandedMapData::EMD_zerg04, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ04"},
			{0x24, ExpandedMapData::EMD_zerg05, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ05"},
			{0x2B, ExpandedMapData::EMD_zerg06, C_BATTLE_ON_THE_AMERIGO_INTRO, RACE_Zerg, 0},
			{0, ExpandedMapData::EMD_zerg06, C_BATTLE_ON_THE_AMERIGO, RACE_Zerg, 1},
			{0x25, ExpandedMapData::EMD_zerg06, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ06"},
			{0x26, ExpandedMapData::EMD_zerg07, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ07"},
			{0x27, ExpandedMapData::EMD_zerg08, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ08"},
			{0x2C, ExpandedMapData::EMD_zerg09, C_THE_WARP_INTRO, RACE_Zerg, 0},
			{0, ExpandedMapData::EMD_zerg09, C_THE_WARP, RACE_Zerg, 1},
			{0x28, ExpandedMapData::EMD_zerg09, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ09"},
			{0x29, ExpandedMapData::EMD_zerg10, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ10"},
			{0x2D, ExpandedMapData::EMD_protoss01, C_THE_INVASION_OF_AIUR_INTRO, RACE_Zerg, 0},
			{0, ExpandedMapData::EMD_protoss01, C_THE_INVASION_OF_AIUR, RACE_Zerg, 1},
			{0},
		},
		{},
		nullptr,
		MenuPosition::GLUE_CAMPAIGN,
	},
	{
		"protoss",
		1,
		false,
		RaceId::RACE_Protoss,
		{
			{0x11, ExpandedMapData::EMD_protoss01, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP01"},
			{0x12, ExpandedMapData::EMD_protoss02, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP02"},
			{0x13, ExpandedMapData::EMD_protoss03, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP03"},
			{0x1B, ExpandedMapData::EMD_protoss04, C_THE_FALL_OF_FENIX_INTRO, RACE_Protoss, 0},
			{0, ExpandedMapData::EMD_protoss04, C_THE_FALL_OF_FENIX, RACE_Protoss, 1},
			{0x14, ExpandedMapData::EMD_protoss04, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP04"},
			{0x15, ExpandedMapData::EMD_protoss05, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP05"},
			{0x1C, ExpandedMapData::EMD_protoss06, C_THE_AMBUSH_INTRO, RACE_Protoss, 0},
			{0, ExpandedMapData::EMD_protoss06, C_THE_AMBUSH, RACE_Protoss, 1},
			{0x16, ExpandedMapData::EMD_protoss06, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP06"},
			{0x17, ExpandedMapData::EMD_protoss07, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP07"},
			{0x1D, ExpandedMapData::EMD_protoss08, C_THE_RETURN_TO_AIUR_INTRO, RACE_Protoss, 0},
			{0, ExpandedMapData::EMD_protoss08, C_THE_RETURN_TO_AIUR, RACE_Protoss, 1},
			{0x18, ExpandedMapData::EMD_protoss08, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP08"},
			{0x19, ExpandedMapData::EMD_protoss09, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP09"},
			{0x1A, ExpandedMapData::EMD_protoss10, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP10"},
			{0x1E, ExpandedMapData::EMD_xprotoss01, C_THE_DEATH_OF_THE_OVERMIND, RACE_Protoss, 0},
			{0},
		},
		{"epilog", "crdt_lst"},
		&Race::races()[RaceId::RACE_Protoss].ingame_music[2],
		MenuPosition::GLUE_MAIN_MENU,
	},
	{
		"xprotoss",
		1,
		true,
		RaceId::RACE_Protoss,
		{
			{0x39, ExpandedMapData::EMD_xprotoss01, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP01x"},
			{0x3A, ExpandedMapData::EMD_xprotoss02, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP02x"},
			{0x3B, ExpandedMapData::EMD_xprotoss03, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP03x"},
			{0x3C, ExpandedMapData::EMD_xprotoss04, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP04x"},
			{0x3D, ExpandedMapData::EMD_xprotoss05, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP05x"},
			{0x3E, ExpandedMapData::EMD_xprotoss06, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP06x"},
			{0x3F, ExpandedMapData::EMD_xprotoss07, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP07x"},
			{0x40, ExpandedMapData::EMD_xprotoss08, C_BLIZZARD_LOGO, RACE_Protoss, 0, "EstP08x"},
			{0x56, ExpandedMapData::EMD_xterran01, C_FURY_OF_THE_XEL_NAGA, RACE_Protoss, 0},
			{0},
		},
		{},
		nullptr,
		MenuPosition::GLUE_EX_CAMPAIGN,
	},
	{
		"xterran",
		1,
		true,
		RaceId::RACE_Terran,
		{
			{0x41, ExpandedMapData::EMD_xterran01, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT01x"},
			{0x42, ExpandedMapData::EMD_xterran02, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT02x"},
			{0x43, ExpandedMapData::EMD_xterran03, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT03x"},
			{0x44, ExpandedMapData::EMD_xterran04, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT04x"},
			{0x45, ExpandedMapData::EMD_xterran05a, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT05ax"},
			{0x46, ExpandedMapData::EMD_xterran05b, C_BLIZZARD_LOGO, RACE_Terran, 1, "EstT05bx"},
			{0x47, ExpandedMapData::EMD_xterran06, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT06x"},
			{0x48, ExpandedMapData::EMD_xterran07, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT07x"},
			{0x49, ExpandedMapData::EMD_xterran08, C_BLIZZARD_LOGO, RACE_Terran, 0, "EstT08x"},
			{0x57, ExpandedMapData::EMD_xzerg01, C_UED_VICTORY_REPORT, RACE_Terran, 0},
			{0},
		},
		{},
		nullptr,
		MenuPosition::GLUE_EX_CAMPAIGN,
	},
	{
		"xzerg",
		1,
		true,
		RaceId::RACE_Zerg,
		{
			{0x4A, ExpandedMapData::EMD_xzerg01, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ01x"},
			{0x4B, ExpandedMapData::EMD_xzerg02, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ02x"},
			{0x4C, ExpandedMapData::EMD_xzerg03, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ03x"},
			{0x4D, ExpandedMapData::EMD_xzerg04a, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ04x"},
			{0, ExpandedMapData::EMD_xzerg04b, C_BLIZZARD_LOGO, RACE_Zerg, 1, "EstZ04x"},
			{0, ExpandedMapData::EMD_xzerg04c, C_BLIZZARD_LOGO, RACE_Zerg, 1, "EstZ04x"},
			{0, ExpandedMapData::EMD_xzerg04d, C_BLIZZARD_LOGO, RACE_Zerg, 1, "EstZ04x"},
			{0, ExpandedMapData::EMD_xzerg04e, C_BLIZZARD_LOGO, RACE_Zerg, 1, "EstZ04x"},
			{0, ExpandedMapData::EMD_xzerg04f, C_BLIZZARD_LOGO, RACE_Zerg, 1, "EstZ04x"},
			{0x4E, ExpandedMapData::EMD_xzerg05, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ05x"},
			{0x4F, ExpandedMapData::EMD_xzerg06, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ06x"},
			{0x50, ExpandedMapData::EMD_xzerg07, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ07x"},
			{0x51, ExpandedMapData::EMD_xzerg08, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ08x"},
			{0x52, ExpandedMapData::EMD_xzerg09, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ09x"},
			{0, ExpandedMapData::EMD_xbonus, C_BLIZZARD_LOGO, RACE_Zerg, 1, "EstZ09bx", "FinZ09bx"},
			{0x54, ExpandedMapData::EMD_xzerg10, C_BLIZZARD_LOGO, RACE_Zerg, 0, "EstZ10x"},
			{0x55, ExpandedMapData::EMD_Unknown, C_THE_ASCENTION, RACE_Zerg, 0},
			{0},
		},
		{"epilogX", "crdt_exp"},
		&Race::races()[RaceId::RACE_Protoss].ingame_music[2],
		MenuPosition::GLUE_MAIN_MENU,
	},
};

std::vector<Campaign*> campaigns_by_race = { &campaigns[2], &campaigns[1], &campaigns[3] };
std::vector<Campaign*> expcampaigns_by_race = { &campaigns[6], &campaigns[5], &campaigns[4] };

MEMORY_PATCH(0x4B69CA, (BYTE)sizeof(ExpandedCampaignMenuEntry));

// Switch between building portraits in campaigns (Overmind/Daggoth and Aldaris/Fenix):
MEMORY_PATCH(0x45E350, (BYTE)EMD_protoss10);
MEMORY_PATCH(0x45F02A, (BYTE)EMD_protoss10);
MEMORY_PATCH(0x45E33F, (BYTE)EMD_protoss07);
MEMORY_PATCH(0x45F019, (BYTE)EMD_protoss07);
