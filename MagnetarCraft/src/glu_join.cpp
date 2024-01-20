#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"

void gluJoin_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		gluJoin_Listbox,
		genericLabelInteract,
		gluJoin_MapStatsLabel,
		gluJoin_MapStatsLabel,
		gluJoin_MapStatsLabel,
		gluJoin_MapStatsLabel,
		gluJoin_MapStatsLabel,
		gluJoin_MapStatsLabel,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
	};

	static swishTimer timers[] = {
		{1, 0},
		{2, 2},
		{3, 3},
		{4, 2},
	};

	DlgSwooshin_(dlg, timers, 0);
	registerMenuFunctions_(functions, dlg, sizeof(functions));
}

FAIL_STUB_PATCH(gluJoin_CustomCtrlID, "starcraft");

int __fastcall gluJoin_Main_(dialog* dlg, struct dlgEvent* evt)
{
	dialog* v5 = getControlFromIndex_(dlg, 13);

	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			sub_4B7E10(dlg);
			DLG_SwishIn_(dlg);
			break;
		case USER_DESTROY:
			DestroyGameNodes();
			break;
		case USER_ACTIVATE:
			if (LastControlID == 13)
			{
				DisableControl(v5);
				if (JoinNetworkGame(&stru_5999F0))
				{
					EnableControl(v5);
					return DLG_SwishOut_(dlg);
				}
			}
			else if (LastControlID != 15 || !is_spawn)
			{
				return DLG_SwishOut_(dlg);
			}
			else
			{
				const char* str = GetNetworkTblString_(104);
				BWFXN_gluPOK_MBox_(str);
			}
			return 1;
		case USER_INIT:
			gluJoin_CustomCtrlID_(dlg);
			break;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluJoin_Main, "starcraft");

void loadMenu_gluJoin_()
{
	InReplay = 0;
	freeChkFileMem_();

	gluJoin_Dlg = LoadDialog("rez\\gluJoin.bin");

	switch (gluLoadBINDlg_(gluJoin_Dlg, gluJoin_Main_))
	{
	case 13:
		glGluesMode = MenuPosition::GLUE_CHAT;
		break;
	case 14:
		glGluesMode = NetMode.as_number != 'SCBL' ? MenuPosition::GLUE_LOGIN : MenuPosition::GLUE_CONNECT;
		break;
	case 15:
		glGluesMode = MenuPosition::GLUE_CREATE;
		break;
	default:
		glGluesMode = MenuPosition::GLUE_MAIN_MENU;
		break;
	}

	changeMenu_();
}

FAIL_STUB_PATCH(loadMenu_gluJoin, "starcraft");
