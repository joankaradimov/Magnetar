#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"

#include "race.h"
#include "campaign.h"
#include "video.h"
#include "glu_campaign.h"

CampaignSet* create_campaign_set()
{
	return new CampaignSet{
		{
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
					cinematic(0xC, ExpandedMapData::EMD_terran03, C_WASTELAND_PATROL_INTRO, SVID_AUTOCUTSCENE, 0),
					cinematic(0, ExpandedMapData::EMD_terran03, C_WASTELAND_PATROL, SVID_AUTOCUTSCENE, 1),
					mission(4, ExpandedMapData::EMD_terran03, RACE_Terran, 0, "rez\\EstT03.txt"),
					mission(5, ExpandedMapData::EMD_terran04, RACE_Terran, 0, "rez\\EstT04.txt"),
					mission(6, ExpandedMapData::EMD_terran05, RACE_Terran, 0, "rez\\EstT05.txt"),
					cinematic(0xD, ExpandedMapData::EMD_terran06, C_THE_DOWNING_OF_NORAD_II_INTRO, SVID_AUTOCUTSCENE, 0),
					cinematic(0, ExpandedMapData::EMD_terran06, C_THE_DOWNING_OF_NORAD_II, SVID_AUTOCUTSCENE, 1),
					mission(7, ExpandedMapData::EMD_terran06, RACE_Terran, 0, "rez\\EstT06.txt"),
					mission(8, ExpandedMapData::EMD_terran08, RACE_Terran, 0, "rez\\EstT08.txt"),
					mission(9, ExpandedMapData::EMD_terran09, RACE_Terran, 0, "rez\\EstT09.txt"),
					cinematic(0xE, ExpandedMapData::EMD_terran11, C_OPEN_REBELION_INTRO, SVID_AUTOCUTSCENE, 0),
					cinematic(0, ExpandedMapData::EMD_terran11, C_OPEN_REBELION, SVID_AUTOCUTSCENE, 1),
					mission(0xA, ExpandedMapData::EMD_terran11, RACE_Terran, 0, "rez\\EstT11.txt"),
					mission(0xB, ExpandedMapData::EMD_terran12, RACE_Terran, 0, "rez\\EstT12.txt"),
					cinematic(0xF, ExpandedMapData::EMD_zerg01, C_THE_INAUGURATION_INTRO, SVID_AUTOCUTSCENE, 0),
					cinematic(0, ExpandedMapData::EMD_zerg01, C_THE_INAUGURATION, SVID_AUTOCUTSCENE, 1),
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
					cinematic(0x2A, ExpandedMapData::EMD_zerg04, C_THE_DREAM, SVID_AUTOCUTSCENE, 0),
					mission(0x23, ExpandedMapData::EMD_zerg04, RACE_Zerg, 0, "rez\\EstZ04.txt"),
					mission(0x24, ExpandedMapData::EMD_zerg05, RACE_Zerg, 0, "rez\\EstZ05.txt"),
					cinematic(0x2B, ExpandedMapData::EMD_zerg06, C_BATTLE_ON_THE_AMERIGO_INTRO, SVID_AUTOCUTSCENE, 0),
					cinematic(0, ExpandedMapData::EMD_zerg06, C_BATTLE_ON_THE_AMERIGO, SVID_AUTOCUTSCENE, 1),
					mission(0x25, ExpandedMapData::EMD_zerg06, RACE_Zerg, 0, "rez\\EstZ06.txt"),
					mission(0x26, ExpandedMapData::EMD_zerg07, RACE_Zerg, 0, "rez\\EstZ07.txt"),
					mission(0x27, ExpandedMapData::EMD_zerg08, RACE_Zerg, 0, "rez\\EstZ08.txt"),
					cinematic(0x2C, ExpandedMapData::EMD_zerg09, C_THE_WARP_INTRO, SVID_AUTOCUTSCENE, 0),
					cinematic(0, ExpandedMapData::EMD_zerg09, C_THE_WARP, SVID_AUTOCUTSCENE, 1),
					mission(0x28, ExpandedMapData::EMD_zerg09, RACE_Zerg, 0, "rez\\EstZ09.txt"),
					mission(0x29, ExpandedMapData::EMD_zerg10, RACE_Zerg, 0, "rez\\EstZ10.txt"),
					cinematic(0x2D, ExpandedMapData::EMD_protoss01, C_THE_INVASION_OF_AIUR_INTRO, SVID_AUTOCUTSCENE, 0),
					cinematic(0, ExpandedMapData::EMD_protoss01, C_THE_INVASION_OF_AIUR, SVID_AUTOCUTSCENE, 1),
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
					cinematic(0x1B, ExpandedMapData::EMD_protoss04, C_THE_FALL_OF_FENIX_INTRO, SVID_AUTOCUTSCENE, 0),
					cinematic(0, ExpandedMapData::EMD_protoss04, C_THE_FALL_OF_FENIX, SVID_AUTOCUTSCENE, 1),
					mission(0x14, ExpandedMapData::EMD_protoss04, RACE_Protoss, 0, "rez\\EstP04.txt"),
					mission(0x15, ExpandedMapData::EMD_protoss05, RACE_Protoss, 0, "rez\\EstP05.txt"),
					cinematic(0x1C, ExpandedMapData::EMD_protoss06, C_THE_AMBUSH_INTRO, SVID_AUTOCUTSCENE, 0),
					cinematic(0, ExpandedMapData::EMD_protoss06, C_THE_AMBUSH, SVID_AUTOCUTSCENE, 1),
					mission(0x16, ExpandedMapData::EMD_protoss06, RACE_Protoss, 0, "rez\\EstP06.txt"),
					mission(0x17, ExpandedMapData::EMD_protoss07, RACE_Protoss, 0, "rez\\EstP07.txt"),
					cinematic(0x1D, ExpandedMapData::EMD_protoss08, C_THE_RETURN_TO_AIUR_INTRO, SVID_AUTOCUTSCENE, 0),
					cinematic(0, ExpandedMapData::EMD_protoss08, C_THE_RETURN_TO_AIUR, SVID_AUTOCUTSCENE, 1),
					mission(0x18, ExpandedMapData::EMD_protoss08, RACE_Protoss, 0, "rez\\EstP08.txt"),
					mission(0x19, ExpandedMapData::EMD_protoss09, RACE_Protoss, 0, "rez\\EstP09.txt"),
					mission(0x1A, ExpandedMapData::EMD_protoss10, RACE_Protoss, 0, "rez\\EstP10.txt"),
					cinematic(0x1E, ExpandedMapData::EMD_xprotoss01, C_THE_DEATH_OF_THE_OVERMIND, SVID_AUTOCUTSCENE, 0),
					epilog("rez\\epilog.txt", &Race::races()[RaceId::RACE_Protoss].ingame_music[2]),
					epilog("rez\\crdt_lst.txt", &Race::races()[RaceId::RACE_Protoss].ingame_music[2]),
				},
				MenuPosition::GLUE_MAIN_MENU,
			},
		}
	};
};

int __fastcall gluCmpgn_CampaignButton_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_LBUTTONDOWN:
	case EventNo::EVN_LBUTTONDBLCLK:
		if (dlg->lFlags & CTRL_DISABLED)
		{
			return 0;
		}
		break;
	case EventNo::EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			SetCallbackTimer(CTRL_DISABLED, dlg, 200, gluCmpgnBtn_UpdateTimer);
			break;
		case EventUser::USER_DESTROY:
			waitLoopCntd(CTRL_DISABLED, dlg);
			break;
		case EventUser::USER_MOUSEMOVE:
			return sub_4B24B0(dlg, evt);
		case EventUser::USER_INIT:
			genericLightupBtnInteract(dlg, evt);
			dlg->pfcnUpdate = gluCmpgnBtn_BtnLightupUpdate;
			SetCallbackTimer(72, dlg, 30, gluCmpgnBtn_InitTimer);
			return 1;
		}
		break;
	}

	return genericLightupBtnInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluCmpgn_CampaignButton);

void gluCmpgn_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		gluCmpgn_CampaignButton_,
		genericLightupBtnInteract,
		gluCmpgn_CampaignButton_,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
	};

	static swishTimer timers[] = {
		{1, 0},
		{3, 2},
	};

	registerMenuFunctions_(functions, dlg, sizeof(functions));
	DlgSwooshin_(dlg, timers, 0);
}

FAIL_STUB_PATCH(gluCmpgn_CustomCtrlID);

bool sub_4B27A0_(Campaign& campaign)
{
	if (dword_59B760[campaign.race])
	{
		const char* v3 = GetNetworkTblString_(campaign.race == RaceId::RACE_Protoss ? 142 : 141);

		if (!sub_4B5B20(v3))
		{
			return 0;
		}
	}

	return LoadCampaignWithCharacter_(campaign);
}

FAIL_STUB_PATCH(sub_4B27A0);

bool sub_4B2810_(dialog* a1)
{
	switch (LastControlID)
	{
	case 6:
		if (!sub_4B27A0_(active_campaign_set->campaigns[3]))
		{
			return true;
		}
		LastControlID = 6;
		break;
	case 7:
		if (!sub_4B27A0_(active_campaign_set->campaigns[1]))
		{
			return true;
		}
		LastControlID = 7;
		break;
	case 8:
		if (!sub_4B27A0_(active_campaign_set->campaigns[2]))
		{
			return true;
		}
		LastControlID = 8;
		break;
	case 30:
		if (!LoadPrecursorCampaign())
		{
			return true;
		}
		LastControlID = 30;
		break;
	}
	return DLG_SwishOut_(a1);
}

FAIL_STUB_PATCH(sub_4B2810);

int __fastcall gluCmpgn_Main_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			DLG_SwishIn_(dlg);
			if (!byte_6D5BBC)
			{
				DLGMusicFade_(&title_music);
			}
			break;
		case EventUser::USER_ACTIVATE:
			return sub_4B2810_(dlg);
		case EventUser::USER_INIT:
			gluCmpgn_CustomCtrlID_(dlg);
			break;
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluCmpgn_Main);

void loadMenu_gluCmpgn_()
{
	active_campaign_set.reset(create_campaign_set());
	OpheliaEnabled = 0;
	multiPlayerMode = 0;
	sub_4B26E0();
	dialog* campaign_dialog = loadAndInitFullMenuDLG_("rez\\gluCmpgn.bin");

	switch (gluLoadBINDlg_(campaign_dialog, gluCmpgn_Main_))
	{
	case 5:
		glGluesMode = GLUE_LOAD;
		break;
	case 6:
		glGluesMode = GLUE_READY_P;
		break;
	case 30:
	case 7:
		glGluesMode = GLUE_READY_T;
		break;
	case 8:
		glGluesMode = GLUE_READY_Z;
		break;
	case 9:
		glGluesMode = GLUE_LOGIN;
		break;
	case 10:
		glGluesMode = GLUE_CREATE;
		break;
	case 11:
		glGluesMode = GLUE_CREATE_MULTI;
		break;
	default:
		glGluesMode = GLUE_MAIN_MENU;
		break;
	}

	changeMenu_();
}

FAIL_STUB_PATCH(loadMenu_gluCmpgn);
