#pragma once

#include <vector>

#include "starcraft.h"

enum ExpandedMapData : u16
{
	EMD_none = 0x0,
	EMD_swtutorial = 0x1,
	EMD_swterran01 = 0x2,
	EMD_swterran02 = 0x3,
	EMD_swterran03 = 0x4,
	EMD_swterran04 = 0x5,
	EMD_swterran05 = 0x6,
	EMD_tutorial = 0x7,
	EMD_terran01 = 0x8,
	EMD_terran02 = 0x9,
	EMD_terran03 = 0xA,
	EMD_terran04 = 0xB,
	EMD_terran05 = 0xC,
	EMD_terran06 = 0xD,
	EMD_terran07 = 0xE,
	EMD_terran08 = 0xF,
	EMD_terran09 = 0x10,
	EMD_terran10 = 0x11,
	EMD_terran11 = 0x12,
	EMD_terran12 = 0x13,
	EMD_zerg01 = 0x14,
	EMD_zerg02 = 0x15,
	EMD_zerg03 = 0x16,
	EMD_zerg04 = 0x17,
	EMD_zerg05 = 0x18,
	EMD_zerg06 = 0x19,
	EMD_zerg07 = 0x1A,
	EMD_zerg08 = 0x1B,
	EMD_zerg09 = 0x1C,
	EMD_zerg10 = 0x1D,
	EMD_protoss01 = 0x1E,
	EMD_protoss02 = 0x1F,
	EMD_protoss03 = 0x20,
	EMD_protoss04 = 0x21,
	EMD_protoss05 = 0x22,
	EMD_protoss06 = 0x23,
	EMD_protoss07 = 0x24,
	EMD_protoss08 = 0x25,
	EMD_protoss09 = 0x26,
	EMD_protoss10 = 0x27,
	EMD_xprotoss01 = 0x28,
	EMD_xprotoss02 = 0x29,
	EMD_xprotoss03 = 0x2A,
	EMD_xprotoss04 = 0x2B,
	EMD_xprotoss05 = 0x2C,
	EMD_xprotoss06 = 0x2D,
	EMD_xprotoss07 = 0x2E,
	EMD_xprotoss08 = 0x2F,
	EMD_xterran01 = 0x30,
	EMD_xterran02 = 0x31,
	EMD_xterran03 = 0x32,
	EMD_xterran04 = 0x33,
	EMD_xterran05a = 0x34,
	EMD_xterran05b = 0x35,
	EMD_xterran06 = 0x36,
	EMD_xterran07 = 0x37,
	EMD_xterran08 = 0x38,
	EMD_xzerg01 = 0x39,
	EMD_xzerg02 = 0x3A,
	EMD_xzerg03 = 0x3B,
	EMD_xzerg04a = 0x3C,
	EMD_xzerg04b = 0x3D,
	EMD_xzerg04c = 0x3E,
	EMD_xzerg04d = 0x3F,
	EMD_xzerg04e = 0x40,
	EMD_xzerg04f = 0x41,
	EMD_xzerg05 = 0x42,
	EMD_xzerg06 = 0x43,
	EMD_xzerg07 = 0x44,
	EMD_xzerg08 = 0x45,
	EMD_xzerg09 = 0x46,
	EMD_xbonus = 0x47,
	EMD_xzerg10 = 0x48,
	EMD_Unknown = 0x49,
};

enum CampaignMenuEntryType
{
	CINEMATIC,
	MISSION,
	EPILOG,
};

struct CampaignMenuEntryEx
{
	CampaignMenuEntryType entry_type;
	unsigned __int16 glu_hist_tbl_index;
	ExpandedMapData next_mission;
	Cinematic cinematic;
	RaceId race;
	bool hide;
	const char* establishing_shot;
	const char* epilog;
	const MusicTrackDescription* epilog_music_track;
	StormVideoFlags video_flags;
};

struct Campaign
{
	const char* campaign_id;
	int first_mission_index;
	bool is_expansion;
	RaceId race;
	std::vector<CampaignMenuEntryEx> entries;
	MenuPosition post_epilog_menu;
};

CampaignMenuEntryEx mission(unsigned __int16 glu_hist_tbl_index, ExpandedMapData next_mission, RaceId race, bool hide, const char* establishing_shot, const char* epilog = nullptr);
CampaignMenuEntryEx cinematic(unsigned __int16 glu_hist_tbl_index, ExpandedMapData next_mission, Cinematic cinematic, StormVideoFlags flags, bool hide);
CampaignMenuEntryEx epilog(const char* epilog, const MusicTrackDescription* epilog_music_track);

class CampaignSet
{
public:
	std::vector<Campaign> campaigns;
};

extern CampaignSet* active_campaign_set;
