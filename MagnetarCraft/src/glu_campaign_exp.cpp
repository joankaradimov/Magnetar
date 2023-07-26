#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"

#include "glu_campaign_exp.h"

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
		if (!sub_4B5110_(campaigns[4]))
		{
			return true;
		}
		LastControlID = 6;
		break;
	case 7:
		if (!sub_4B5110_(campaigns[5]))
		{
			return true;
		}
		LastControlID = 7;
		break;
	case 8:
		if (!sub_4B5110_(campaigns[6]))
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
