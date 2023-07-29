#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"

#include "tbl_file.h"
#include "race.h"
#include "campaign.h"
#include "video.h"
#include "glu_campaign_exp.h"

CampaignSet* create_exp_campaign_set()
{
	TblFile mission_names("rez\\gluHist.tbl");

	return new CampaignSet {
		{
			{
				"xprotoss",
				1,
				true,
				RaceId::RACE_Protoss,
				{
					mission(mission_names[0x38], ExpandedMapData::EMD_xprotoss01, RACE_Protoss, 0, "rez\\EstP01x.txt"),
					mission(mission_names[0x39], ExpandedMapData::EMD_xprotoss02, RACE_Protoss, 0, "rez\\EstP02x.txt"),
					mission(mission_names[0x3A], ExpandedMapData::EMD_xprotoss03, RACE_Protoss, 0, "rez\\EstP03x.txt"),
					mission(mission_names[0x3B], ExpandedMapData::EMD_xprotoss04, RACE_Protoss, 0, "rez\\EstP04x.txt"),
					mission(mission_names[0x3C], ExpandedMapData::EMD_xprotoss05, RACE_Protoss, 0, "rez\\EstP05x.txt"),
					mission(mission_names[0x3D], ExpandedMapData::EMD_xprotoss06, RACE_Protoss, 0, "rez\\EstP06x.txt"),
					mission(mission_names[0x3E], ExpandedMapData::EMD_xprotoss07, RACE_Protoss, 0, "rez\\EstP07x.txt"),
					mission(mission_names[0x3F], ExpandedMapData::EMD_xprotoss08, RACE_Protoss, 0, "rez\\EstP08x.txt"),
					cinematic(mission_names[0x55], ExpandedMapData::EMD_xterran01, C_FURY_OF_THE_XEL_NAGA, SVID_AUTOCUTSCENE | StormVideoFlags::SVID_FLAG_UNK, 0),
				},
				MenuPosition::GLUE_EX_CAMPAIGN,
			},
			{
				"xterran",
				1,
				true,
				RaceId::RACE_Terran,
				{
					mission(mission_names[0x40], ExpandedMapData::EMD_xterran01, RACE_Terran, 0, "rez\\EstT01x.txt"),
					mission(mission_names[0x41], ExpandedMapData::EMD_xterran02, RACE_Terran, 0, "rez\\EstT02x.txt"),
					mission(mission_names[0x42], ExpandedMapData::EMD_xterran03, RACE_Terran, 0, "rez\\EstT03x.txt"),
					mission(mission_names[0x43], ExpandedMapData::EMD_xterran04, RACE_Terran, 0, "rez\\EstT04x.txt"),
					mission(mission_names[0x44], ExpandedMapData::EMD_xterran05a, RACE_Terran, 0, "rez\\EstT05ax.txt"),
					mission(mission_names[0x45], ExpandedMapData::EMD_xterran05b, RACE_Terran, 1, "rez\\EstT05bx.txt"),
					mission(mission_names[0x46], ExpandedMapData::EMD_xterran06, RACE_Terran, 0, "rez\\EstT06x.txt"),
					mission(mission_names[0x47], ExpandedMapData::EMD_xterran07, RACE_Terran, 0, "rez\\EstT07x.txt"),
					mission(mission_names[0x48], ExpandedMapData::EMD_xterran08, RACE_Terran, 0, "rez\\EstT08x.txt"),
					cinematic(mission_names[0x56], ExpandedMapData::EMD_xzerg01, C_UED_VICTORY_REPORT, SVID_AUTOCUTSCENE | StormVideoFlags::SVID_FLAG_UNK, 0),
				},
				MenuPosition::GLUE_EX_CAMPAIGN,
			},
			{
				"xzerg",
				1,
				true,
				RaceId::RACE_Zerg,
				{
					mission(mission_names[0x49], ExpandedMapData::EMD_xzerg01, RACE_Zerg, 0, "rez\\EstZ01x.txt"),
					mission(mission_names[0x4A], ExpandedMapData::EMD_xzerg02, RACE_Zerg, 0, "rez\\EstZ02x.txt"),
					mission(mission_names[0x4B], ExpandedMapData::EMD_xzerg03, RACE_Zerg, 0, "rez\\EstZ03x.txt"),
					mission(mission_names[0x4C], ExpandedMapData::EMD_xzerg04a, RACE_Zerg, 0, "rez\\EstZ04x.txt"),
					mission(nullptr, ExpandedMapData::EMD_xzerg04b, RACE_Zerg, 1, "rez\\EstZ04x.txt"),
					mission(nullptr, ExpandedMapData::EMD_xzerg04c, RACE_Zerg, 1, "rez\\EstZ04x.txt"),
					mission(nullptr, ExpandedMapData::EMD_xzerg04d, RACE_Zerg, 1, "rez\\EstZ04x.txt"),
					mission(nullptr, ExpandedMapData::EMD_xzerg04e, RACE_Zerg, 1, "rez\\EstZ04x.txt"),
					mission(nullptr, ExpandedMapData::EMD_xzerg04f, RACE_Zerg, 1, "rez\\EstZ04x.txt"),
					mission(mission_names[0x4D], ExpandedMapData::EMD_xzerg05, RACE_Zerg, 0, "rez\\EstZ05x.txt"),
					mission(mission_names[0x4E], ExpandedMapData::EMD_xzerg06, RACE_Zerg, 0, "rez\\EstZ06x.txt"),
					mission(mission_names[0x4F], ExpandedMapData::EMD_xzerg07, RACE_Zerg, 0, "rez\\EstZ07x.txt"),
					mission(mission_names[0x50], ExpandedMapData::EMD_xzerg08, RACE_Zerg, 0, "rez\\EstZ08x.txt"),
					mission(mission_names[0x51], ExpandedMapData::EMD_xzerg09, RACE_Zerg, 0, "rez\\EstZ09x.txt"),
					mission(mission_names[0x52], ExpandedMapData::EMD_xbonus, RACE_Zerg, 1, "rez\\EstZ09bx.txt"),
					epilog("rez\\FinZ09bx.txt", &Race::races()[RaceId::RACE_Zerg].briefing_music),
					mission(mission_names[0x53], ExpandedMapData::EMD_xzerg10, RACE_Zerg, 0, "rez\\EstZ10x.txt"),
					cinematic(mission_names[0x54], ExpandedMapData::EMD_Unknown, C_THE_ASCENTION, SVID_AUTOCUTSCENE | StormVideoFlags::SVID_FLAG_UNK, 0),
					epilog("rez\\epilogX.txt", &Race::races()[RaceId::RACE_Protoss].ingame_music[2]),
					epilog("rez\\crdt_exp.txt", &Race::races()[RaceId::RACE_Protoss].ingame_music[2]),
				},
				MenuPosition::GLUE_MAIN_MENU,
			},
		}
	};
}

int __fastcall gluExpCmpgn_CampaignButton_(dialog* dlg, dlgEvent* evt)
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
			SetCallbackTimer(2, dlg, 200, sub_4B4E70);
			break;
		case EventUser::USER_DESTROY:
			waitLoopCntd(2, dlg);
			break;
		case EventUser::USER_MOUSEMOVE:
			return sub_4B4E20(dlg, evt);
		case EventUser::USER_INIT:
			genericLightupBtnInteract(dlg, evt);
			dlg->pfcnUpdate = sub_4B4F10;
			SetCallbackTimer(72, dlg, 30, sub_4B4EE0);
			return 1;
		}
		break;
	}

	return genericLightupBtnInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluExpCmpgn_CampaignButton);

void gluExpCmpgn_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		genericLightupBtnInteract,
		gluExpCmpgn_CampaignButton_,
		gluExpCmpgn_CampaignButton_,
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

FAIL_STUB_PATCH(gluExpCmpgn_CustomCtrlID);

int sub_4B5110_(Campaign& campaign)
{
	if (dword_59A0D4[campaign.race])
	{
		const char* v3 = GetNetworkTblString_(campaign.race != RaceId::RACE_Terran ? 144 : 143);

		if (!sub_4B5B20(v3))
		{
			return 0;
		}
	}

	return LoadCampaignWithCharacter_(campaign);
}

FAIL_STUB_PATCH(sub_4B5110);

bool sub_4B5180_(dialog* a1)
{
	switch (LastControlID)
	{
	case 6:
		if (!sub_4B5110_(active_campaign_set->campaigns[0]))
		{
			return true;
		}
		LastControlID = 6;
		break;
	case 7:
		if (!sub_4B5110_(active_campaign_set->campaigns[1]))
		{
			return true;
		}
		LastControlID = 7;
		break;
	case 8:
		if (!sub_4B5110_(active_campaign_set->campaigns[2]))
		{
			return true;
		}
		LastControlID = 8;
		break;
	}
	return DLG_SwishOut_(a1);
}

FAIL_STUB_PATCH(sub_4B5180);

int __fastcall gluExpCmpgn_Main_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			DLG_SwishIn_(dlg);
			if (!byte_6D5BBC)
				DLGMusicFade_(&title_music);
			break;
		case EventUser::USER_ACTIVATE:
			return sub_4B5180_(dlg);
		case EventUser::USER_INIT:
			gluExpCmpgn_CustomCtrlID_(dlg);
			break;
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluExpCmpgn_Main);

void loadMenu_gluExpCmpgn_()
{
	active_campaign_set.reset(create_exp_campaign_set());
	OpheliaEnabled = 0;
	multiPlayerMode = 0;
	sub_4B5050();
	dialog* campaign_dialog = loadAndInitFullMenuDLG_("rez\\gluExpCmpgn.bin");

	switch (gluLoadBINDlg_(campaign_dialog, gluExpCmpgn_Main_))
	{
	case 8:
		glGluesMode = GLUE_READY_Z;
		break;
	case 7:
		glGluesMode = GLUE_READY_T;
		break;
	case 6:
		glGluesMode = GLUE_READY_P;
		break;
	case 5:
		glGluesMode = GLUE_LOAD;
		break;
	case 10:
		glGluesMode = GLUE_CREATE;
		break;
	case 11:
		glGluesMode = GLUE_CREATE_MULTI;
		break;
	case 9:
		glGluesMode = GLUE_LOGIN;
		break;
	default:
		glGluesMode = GLUE_MAIN_MENU;
		break;
	}

	changeMenu_();
}

FAIL_STUB_PATCH(loadMenu_gluExpCmpgn);
