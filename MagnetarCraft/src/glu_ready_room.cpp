#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"
#include "race.h"

void rdyPFrame_()
{
	const char* v1[] =
	{
		"glue\\ReadyP\\PFrameH1.pcx",
		"glue\\ReadyP\\PFrameH2.pcx",
		"glue\\ReadyP\\PFrameH3.pcx",
		"glue\\ReadyP\\PFrameH4.pcx",
	};
	const char* v2[] =
	{
		"glue\\ReadyP\\PFrame1.pcx",
		"glue\\ReadyP\\PFrame2.pcx",
		"glue\\ReadyP\\PFrame3.pcx",
		"glue\\ReadyP\\PFrame4.pcx",
	};
	LoadBriefingFrameImages(v1, v2);
}

FAIL_STUB_PATCH(rdyPFrame);

void RdyTFrame_()
{
	const char* v1[] =
	{
		"glue\\ReadyT\\TFrameH1.pcx",
		"glue\\ReadyT\\TFrameH2.pcx",
		"glue\\ReadyT\\TFrameH3.pcx",
		"glue\\ReadyT\\TFrameH4.pcx",
	};
	const char* v2[] =
	{
		"glue\\ReadyT\\TFrame1.pcx",
		"glue\\ReadyT\\TFrame2.pcx",
		"glue\\ReadyT\\TFrame3.pcx",
		"glue\\ReadyT\\TFrame4.pcx",
	};
	LoadBriefingFrameImages(v1, v2);
}

FAIL_STUB_PATCH(RdyTFrame);

void RdyZFrame_()
{
	const char* v1[] =
	{
		"glue\\ReadyZ\\ZFrameH1.pcx",
		"glue\\ReadyZ\\ZFrameH2.pcx",
		"glue\\ReadyZ\\ZFrameH3.pcx",
		"glue\\ReadyZ\\ZFrameH4.pcx",
	};
	const char* v2[] =
	{
		"glue\\ReadyZ\\ZFrame1.pcx",
		"glue\\ReadyZ\\ZFrame2.pcx",
		"glue\\ReadyZ\\ZFrame3.pcx",
		"glue\\ReadyZ\\ZFrame4.pcx",
	};
	LoadBriefingFrameImages(v1, v2);
}

FAIL_STUB_PATCH(RdyZFrame);

void gluRdy_CustomCtrlID_(dialog* dlg)
{
	static swishTimer timers[] =
	{
		{5, 3},
		{6, 0},
		{7, 3},
		{9, 0},
		{10, 0},
		{11, 2},
		{12, 2},
	};

	static FnInteract functions[] = {
		gluRdyZ_Secret_,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		genericLightupBtnInteract,
		Menu_Generic_Button,
		gluRdy_Portrait,
		gluRdy_Portrait,
		gluRdy_Portrait,
		gluRdy_Portrait,
		Menu_Generic_Button,
		Menu_Generic_Button,
	};

	DlgSwooshin_(dlg, timers, 80);
	registerMenuFunctions_(functions, dlg, sizeof(functions));
}

FAIL_STUB_PATCH(gluRdyP_CustomCtrlID);
FAIL_STUB_PATCH(gluRdyT_CustomCtrlID);
FAIL_STUB_PATCH(gluRdyZ_CustomCtrlID);

int sub_46D160_(dialog* dlg)
{
	switch (LastControlID)
	{
	case 13:
		BriefingStart(dlg, 0);
		return 1;
	case 14:
		BriefingStart(dlg, 1);
		break;
	case 20:
		sub_46CBC0(dlg);
		return 1;
	}

	sub_46CA90(dlg);
	return (unsigned __int8) DLG_SwishOut(dlg);
}

FAIL_STUB_PATCH(sub_46D160);

int __fastcall gluRdy_BINDLG_Loop(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			DLG_SwishIn_(dlg);
			break;
		case EventUser::USER_DESTROY:
			briefingFramesCleanup(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			return sub_46D160_(dlg);
		case EventUser::USER_INIT:
			sub_46D3C0_(dlg);
			gluRdy_CustomCtrlID_(dlg);
			break;
		case 0x405:
			sub_46D220_(dlg);
			switch (glGluesMode)
			{
			case MenuPosition::GLUE_READY_P:
				rdyPFrame_();
				break;
			case MenuPosition::GLUE_READY_T:
				RdyTFrame_();
				break;
			case MenuPosition::GLUE_READY_Z:
				RdyZFrame_();
				break;
			}
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluRdyT_BINDLG_Loop);
FAIL_STUB_PATCH(gluRdyZ_BINDLG_Loop);
FAIL_STUB_PATCH(gluRdyP_BINDLG_Loop);

void loadMenu_gluRdy(RaceId race)
{
	auto briefing_music = &Race::races()[race].briefing_music;
	if (current_music_track != briefing_music)
	{
		sub_46D200_(briefing_music);
	}
	DisplayEstablishingShot_();
	if (gwGameMode == GAME_GLUES)
	{
		dword_50E064 = -1;
		dialog* bin_dialog = LoadDialog(Race::races()[race].get_ready_room_bin());

		switch (gluLoadBINDlg_(bin_dialog, gluRdy_BINDLG_Loop))
		{
		case 14:
			if (multiPlayerMode)
			{
				glGluesMode = BWFXN_NetSelectReturnMenu_();
			}
			else
			{
				glGluesMode = IsExpansion ? GLUE_EX_CAMPAIGN : GLUE_CAMPAIGN;
			}
			sub_46D1F0_();
			break;
		case 19:
			ContinueCampaign_(1);
			break;
		case 100:
			sub_46D1F0_();
			break;
		case 101:
			gwGameMode = GAME_RUNINIT;
			break;
		default:
			glGluesMode = GLUE_MAIN_MENU;
			sub_46D1F0_();
			break;
		}
		changeMenu_();
	}
}

FAIL_STUB_PATCH(loadMenu_gluRdyT);
FAIL_STUB_PATCH(loadMenu_gluRdyZ);
FAIL_STUB_PATCH(loadMenu_gluRdyP);
