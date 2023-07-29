#include "campaign.h"
#include "patching.h"

CampaignMenuEntryEx mission(const char* description, ExpandedMapData next_mission, RaceId race, bool hide, const char* establishing_shot, const char* epilog)
{
	return
	{
		CampaignMenuEntryType::MISSION,
		description ? description : "",
		next_mission,
		C_BLIZZARD_LOGO,
		race,
		hide,
		establishing_shot,
		nullptr,
		nullptr,
		(StormVideoFlags)0,
	};
}

CampaignMenuEntryEx cinematic(const char* description, ExpandedMapData next_mission, Cinematic cinematic, StormVideoFlags flags, bool hide)
{
	return
	{
		CampaignMenuEntryType::CINEMATIC,
		description ? description : "",
		next_mission,
		cinematic,
		RaceId::RACE_None,
		hide,
		nullptr,
		nullptr,
		nullptr,
		flags,
	};
}

CampaignMenuEntryEx epilog(const char* epilog, const MusicTrackDescription* epilog_music_track)
{
	return
	{
		CampaignMenuEntryType::EPILOG,
		"",
		ExpandedMapData::EMD_none,
		C_BLIZZARD_LOGO,
		RaceId::RACE_None,
		true,
		nullptr,
		epilog,
		epilog_music_track,
		(StormVideoFlags)0,
	};
}

std::unique_ptr<CampaignSet> active_campaign_set;

MEMORY_PATCH(0x4B69CA, (BYTE)sizeof(CampaignMenuEntryEx));

// Switch between building portraits in campaigns (Overmind/Daggoth and Aldaris/Fenix):
MEMORY_PATCH(0x45E350, (BYTE)EMD_protoss10);
MEMORY_PATCH(0x45F02A, (BYTE)EMD_protoss10);
MEMORY_PATCH(0x45E33F, (BYTE)EMD_protoss07);
MEMORY_PATCH(0x45F019, (BYTE)EMD_protoss07);
