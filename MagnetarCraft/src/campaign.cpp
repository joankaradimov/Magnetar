#include "campaign.h"
#include "race.h"
#include "patching.h"

CampaignMenuEntryEx mission(unsigned __int16 glu_hist_tbl_index, ExpandedMapData next_mission, RaceId race, bool hide, const char* establishing_shot, const char* epilog)
{
	return
	{
		CampaignMenuEntryType::MISSION,
		glu_hist_tbl_index,
		next_mission,
		C_BLIZZARD_LOGO,
		race,
		hide,
		establishing_shot
	};
}

CampaignMenuEntryEx cinematic(unsigned __int16 glu_hist_tbl_index, ExpandedMapData next_mission, Cinematic cinematic, bool hide)
{
	return
	{
		CampaignMenuEntryType::CINEMATIC,
		glu_hist_tbl_index,
		next_mission,
		cinematic,
		RaceId::RACE_None,
		hide,
		nullptr,
		nullptr
	};
}

CampaignMenuEntryEx epilog(const char* epilog, const MusicTrackDescription* epilog_music_track)
{
	return
	{
		CampaignMenuEntryType::EPILOG,
		0,
		ExpandedMapData::EMD_none,
		C_BLIZZARD_LOGO,
		RaceId::RACE_None,
		true,
		nullptr,
		epilog,
		epilog_music_track,
	};
}

std::vector<Campaign> campaigns = {
	{
		"swterran",
		0,
		false,
		RaceId::RACE_Terran,
		{
			mission(0x33, ExpandedMapData::EMD_swtutorial, RACE_Terran, 0, "rez\\Estt0tsw.txt"),
			mission(0x34, ExpandedMapData::EMD_swterran01, RACE_Terran, 0, "rez\\Estt01sw.txt"),
			mission(0x35, ExpandedMapData::EMD_swterran02, RACE_Terran, 0, "rez\\Estt02sw.txt"),
			mission(0x36, ExpandedMapData::EMD_swterran03, RACE_Terran, 0, "rez\\Estt03sw.txt"),
			mission(0x37, ExpandedMapData::EMD_swterran04, RACE_Terran, 0, "rez\\Estt04sw.txt"),
			mission(0x38, ExpandedMapData::EMD_swterran05, RACE_Terran, 0, "rez\\Estt05sw.txt"),
			epilog("rez\\epilogsw.txt", &Race::races()[RaceId::RACE_Terran].ingame_music[1]),
		},
		MenuPosition::GLUE_CAMPAIGN,
	},
	{
		"terran",
		0,
		false,
		RaceId::RACE_Terran,
		{
			mission(1, ExpandedMapData::EMD_tutorial, RACE_Terran, 0, "rez\\EstT0t.txt"),
			mission(2, ExpandedMapData::EMD_terran01, RACE_Terran, 0, "rez\\EstT01.txt"),
			mission(3, ExpandedMapData::EMD_terran02, RACE_Terran, 0, "rez\\EstT02.txt"),
			cinematic(0xC, ExpandedMapData::EMD_terran03, C_WASTELAND_PATROL_INTRO, 0),
			cinematic(0, ExpandedMapData::EMD_terran03, C_WASTELAND_PATROL, 1),
			mission(4, ExpandedMapData::EMD_terran03, RACE_Terran, 0, "rez\\EstT03.txt"),
			mission(5, ExpandedMapData::EMD_terran04, RACE_Terran, 0, "rez\\EstT04.txt"),
			mission(6, ExpandedMapData::EMD_terran05, RACE_Terran, 0, "rez\\EstT05.txt"),
			cinematic(0xD, ExpandedMapData::EMD_terran06, C_THE_DOWNING_OF_NORAD_II_INTRO, 0),
			cinematic(0, ExpandedMapData::EMD_terran06, C_THE_DOWNING_OF_NORAD_II, 1),
			mission(7, ExpandedMapData::EMD_terran06, RACE_Terran, 0, "rez\\EstT06.txt"),
			mission(8, ExpandedMapData::EMD_terran08, RACE_Terran, 0, "rez\\EstT08.txt"),
			mission(9, ExpandedMapData::EMD_terran09, RACE_Terran, 0, "rez\\EstT09.txt"),
			cinematic(0xE, ExpandedMapData::EMD_terran11, C_OPEN_REBELION_INTRO, 0),
			cinematic(0, ExpandedMapData::EMD_terran11, C_OPEN_REBELION, 1),
			mission(0xA, ExpandedMapData::EMD_terran11, RACE_Terran, 0, "rez\\EstT11.txt"),
			mission(0xB, ExpandedMapData::EMD_terran12, RACE_Terran, 0, "rez\\EstT12.txt"),
			cinematic(0xF, ExpandedMapData::EMD_zerg01, C_THE_INAUGURATION_INTRO, 0),
			cinematic(0, ExpandedMapData::EMD_zerg01, C_THE_INAUGURATION, 1),
		},
		MenuPosition::GLUE_CAMPAIGN,
	},
	{
		"zerg",
		1,
		false,
		RaceId::RACE_Zerg,
		{
			mission(0x20, ExpandedMapData::EMD_zerg01, RACE_Zerg, 0, "rez\\EstZ01.txt"),
			mission(0x21, ExpandedMapData::EMD_zerg02, RACE_Zerg, 0, "rez\\EstZ02.txt"),
			mission(0x22, ExpandedMapData::EMD_zerg03, RACE_Zerg, 0, "rez\\EstZ03.txt"),
			cinematic(0x2A, ExpandedMapData::EMD_zerg04, C_THE_DREAM, 0),
			mission(0x23, ExpandedMapData::EMD_zerg04, RACE_Zerg, 0, "rez\\EstZ04.txt"),
			mission(0x24, ExpandedMapData::EMD_zerg05, RACE_Zerg, 0, "rez\\EstZ05.txt"),
			cinematic(0x2B, ExpandedMapData::EMD_zerg06, C_BATTLE_ON_THE_AMERIGO_INTRO, 0),
			cinematic(0, ExpandedMapData::EMD_zerg06, C_BATTLE_ON_THE_AMERIGO, 1),
			mission(0x25, ExpandedMapData::EMD_zerg06, RACE_Zerg, 0, "rez\\EstZ06.txt"),
			mission(0x26, ExpandedMapData::EMD_zerg07, RACE_Zerg, 0, "rez\\EstZ07.txt"),
			mission(0x27, ExpandedMapData::EMD_zerg08, RACE_Zerg, 0, "rez\\EstZ08.txt"),
			cinematic(0x2C, ExpandedMapData::EMD_zerg09, C_THE_WARP_INTRO, 0),
			cinematic(0, ExpandedMapData::EMD_zerg09, C_THE_WARP, 1),
			mission(0x28, ExpandedMapData::EMD_zerg09, RACE_Zerg, 0, "rez\\EstZ09.txt"),
			mission(0x29, ExpandedMapData::EMD_zerg10, RACE_Zerg, 0, "rez\\EstZ10.txt"),
			cinematic(0x2D, ExpandedMapData::EMD_protoss01, C_THE_INVASION_OF_AIUR_INTRO, 0),
			cinematic(0, ExpandedMapData::EMD_protoss01, C_THE_INVASION_OF_AIUR, 1),
		},
		MenuPosition::GLUE_CAMPAIGN,
	},
	{
		"protoss",
		1,
		false,
		RaceId::RACE_Protoss,
		{
			mission(0x11, ExpandedMapData::EMD_protoss01, RACE_Protoss, 0, "rez\\EstP01.txt"),
			mission(0x12, ExpandedMapData::EMD_protoss02, RACE_Protoss, 0, "rez\\EstP02.txt"),
			mission(0x13, ExpandedMapData::EMD_protoss03, RACE_Protoss, 0, "rez\\EstP03.txt"),
			cinematic(0x1B, ExpandedMapData::EMD_protoss04, C_THE_FALL_OF_FENIX_INTRO, 0),
			cinematic(0, ExpandedMapData::EMD_protoss04, C_THE_FALL_OF_FENIX, 1),
			mission(0x14, ExpandedMapData::EMD_protoss04, RACE_Protoss, 0, "rez\\EstP04.txt"),
			mission(0x15, ExpandedMapData::EMD_protoss05, RACE_Protoss, 0, "rez\\EstP05.txt"),
			cinematic(0x1C, ExpandedMapData::EMD_protoss06, C_THE_AMBUSH_INTRO, 0),
			cinematic(0, ExpandedMapData::EMD_protoss06, C_THE_AMBUSH, 1),
			mission(0x16, ExpandedMapData::EMD_protoss06, RACE_Protoss, 0, "rez\\EstP06.txt"),
			mission(0x17, ExpandedMapData::EMD_protoss07, RACE_Protoss, 0, "rez\\EstP07.txt"),
			cinematic(0x1D, ExpandedMapData::EMD_protoss08, C_THE_RETURN_TO_AIUR_INTRO, 0),
			cinematic(0, ExpandedMapData::EMD_protoss08, C_THE_RETURN_TO_AIUR, 1),
			mission(0x18, ExpandedMapData::EMD_protoss08, RACE_Protoss, 0, "rez\\EstP08.txt"),
			mission(0x19, ExpandedMapData::EMD_protoss09, RACE_Protoss, 0, "rez\\EstP09.txt"),
			mission(0x1A, ExpandedMapData::EMD_protoss10, RACE_Protoss, 0, "rez\\EstP10.txt"),
			cinematic(0x1E, ExpandedMapData::EMD_xprotoss01, C_THE_DEATH_OF_THE_OVERMIND, 0),
			epilog("rez\\epilog.txt", &Race::races()[RaceId::RACE_Protoss].ingame_music[2]),
			epilog("rez\\crdt_lst.txt", &Race::races()[RaceId::RACE_Protoss].ingame_music[2]),
		},
		MenuPosition::GLUE_MAIN_MENU,
	},
	{
		"xprotoss",
		1,
		true,
		RaceId::RACE_Protoss,
		{
			mission(0x39, ExpandedMapData::EMD_xprotoss01, RACE_Protoss, 0, "rez\\EstP01x.txt"),
			mission(0x3A, ExpandedMapData::EMD_xprotoss02, RACE_Protoss, 0, "rez\\EstP02x.txt"),
			mission(0x3B, ExpandedMapData::EMD_xprotoss03, RACE_Protoss, 0, "rez\\EstP03x.txt"),
			mission(0x3C, ExpandedMapData::EMD_xprotoss04, RACE_Protoss, 0, "rez\\EstP04x.txt"),
			mission(0x3D, ExpandedMapData::EMD_xprotoss05, RACE_Protoss, 0, "rez\\EstP05x.txt"),
			mission(0x3E, ExpandedMapData::EMD_xprotoss06, RACE_Protoss, 0, "rez\\EstP06x.txt"),
			mission(0x3F, ExpandedMapData::EMD_xprotoss07, RACE_Protoss, 0, "rez\\EstP07x.txt"),
			mission(0x40, ExpandedMapData::EMD_xprotoss08, RACE_Protoss, 0, "rez\\EstP08x.txt"),
			cinematic(0x56, ExpandedMapData::EMD_xterran01, C_FURY_OF_THE_XEL_NAGA, 0),
		},
		MenuPosition::GLUE_EX_CAMPAIGN,
	},
	{
		"xterran",
		1,
		true,
		RaceId::RACE_Terran,
		{
			mission(0x41, ExpandedMapData::EMD_xterran01, RACE_Terran, 0, "rez\\EstT01x.txt"),
			mission(0x42, ExpandedMapData::EMD_xterran02, RACE_Terran, 0, "rez\\EstT02x.txt"),
			mission(0x43, ExpandedMapData::EMD_xterran03, RACE_Terran, 0, "rez\\EstT03x.txt"),
			mission(0x44, ExpandedMapData::EMD_xterran04, RACE_Terran, 0, "rez\\EstT04x.txt"),
			mission(0x45, ExpandedMapData::EMD_xterran05a, RACE_Terran, 0, "rez\\EstT05ax.txt"),
			mission(0x46, ExpandedMapData::EMD_xterran05b, RACE_Terran, 1, "rez\\EstT05bx.txt"),
			mission(0x47, ExpandedMapData::EMD_xterran06, RACE_Terran, 0, "rez\\EstT06x.txt"),
			mission(0x48, ExpandedMapData::EMD_xterran07, RACE_Terran, 0, "rez\\EstT07x.txt"),
			mission(0x49, ExpandedMapData::EMD_xterran08, RACE_Terran, 0, "rez\\EstT08x.txt"),
			cinematic(0x57, ExpandedMapData::EMD_xzerg01, C_UED_VICTORY_REPORT, 0),
		},
		MenuPosition::GLUE_EX_CAMPAIGN,
	},
	{
		"xzerg",
		1,
		true,
		RaceId::RACE_Zerg,
		{
			mission(0x4A, ExpandedMapData::EMD_xzerg01, RACE_Zerg, 0, "rez\\EstZ01x.txt"),
			mission(0x4B, ExpandedMapData::EMD_xzerg02, RACE_Zerg, 0, "rez\\EstZ02x.txt"),
			mission(0x4C, ExpandedMapData::EMD_xzerg03, RACE_Zerg, 0, "rez\\EstZ03x.txt"),
			mission(0x4D, ExpandedMapData::EMD_xzerg04a, RACE_Zerg, 0, "rez\\EstZ04x.txt"),
			mission(0, ExpandedMapData::EMD_xzerg04b, RACE_Zerg, 1, "rez\\EstZ04x.txt"),
			mission(0, ExpandedMapData::EMD_xzerg04c, RACE_Zerg, 1, "rez\\EstZ04x.txt"),
			mission(0, ExpandedMapData::EMD_xzerg04d, RACE_Zerg, 1, "rez\\EstZ04x.txt"),
			mission(0, ExpandedMapData::EMD_xzerg04e, RACE_Zerg, 1, "rez\\EstZ04x.txt"),
			mission(0, ExpandedMapData::EMD_xzerg04f, RACE_Zerg, 1, "rez\\EstZ04x.txt"),
			mission(0x4E, ExpandedMapData::EMD_xzerg05, RACE_Zerg, 0, "rez\\EstZ05x.txt"),
			mission(0x4F, ExpandedMapData::EMD_xzerg06, RACE_Zerg, 0, "rez\\EstZ06x.txt"),
			mission(0x50, ExpandedMapData::EMD_xzerg07, RACE_Zerg, 0, "rez\\EstZ07x.txt"),
			mission(0x51, ExpandedMapData::EMD_xzerg08, RACE_Zerg, 0, "rez\\EstZ08x.txt"),
			mission(0x52, ExpandedMapData::EMD_xzerg09, RACE_Zerg, 0, "rez\\EstZ09x.txt"),
			mission(0, ExpandedMapData::EMD_xbonus, RACE_Zerg, 1, "rez\\EstZ09bx.txt"),
			epilog("rez\\FinZ09bx.txt", &Race::races()[RaceId::RACE_Zerg].briefing_music),
			mission(0x54, ExpandedMapData::EMD_xzerg10, RACE_Zerg, 0, "rez\\EstZ10x.txt"),
			cinematic(0x55, ExpandedMapData::EMD_Unknown, C_THE_ASCENTION, 0),
			epilog("rez\\epilogX.txt", &Race::races()[RaceId::RACE_Protoss].ingame_music[2]),
			epilog("rez\\crdt_exp.txt", &Race::races()[RaceId::RACE_Protoss].ingame_music[2]),
		},
		MenuPosition::GLUE_MAIN_MENU,
	},
};

std::vector<Campaign*> campaigns_by_race = { &campaigns[2], &campaigns[1], &campaigns[3] };
std::vector<Campaign*> expcampaigns_by_race = { &campaigns[6], &campaigns[5], &campaigns[4] };

MEMORY_PATCH(0x4B69CA, (BYTE)sizeof(CampaignMenuEntryEx));

// Switch between building portraits in campaigns (Overmind/Daggoth and Aldaris/Fenix):
MEMORY_PATCH(0x45E350, (BYTE)EMD_protoss10);
MEMORY_PATCH(0x45F02A, (BYTE)EMD_protoss10);
MEMORY_PATCH(0x45E33F, (BYTE)EMD_protoss07);
MEMORY_PATCH(0x45F019, (BYTE)EMD_protoss07);
