#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"

#include "glu_campaign.h"

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
		if (!sub_4B27A0_(campaigns[3]))
		{
			return true;
		}
		LastControlID = 6;
		break;
	case 7:
		if (!sub_4B27A0_(campaigns[1]))
		{
			return true;
		}
		LastControlID = 7;
		break;
	case 8:
		if (!sub_4B27A0_(campaigns[2]))
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
