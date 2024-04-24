#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"

void gluLoad_CustomCtrlID_(dialog* a1)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		Menu_Generic_Button,
		gluLoad_List,
		Menu_Generic_Button,
	};

	static swishTimer timers[] = {
		{1, 0},
		{2, 3},
		{3, 2},
	};

	DlgSwooshin_(a1, timers, 0);
	registerMenuFunctions_(functions, a1, sizeof(functions));
}

FAIL_STUB_PATCH(gluLoad_CustomCtrlID, "starcraft");

int __fastcall gluLoad_Main_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			DLG_SwishIn_(dlg);
			break;
		case USER_ACTIVATE:
			if (LastControlID == 7)
			{
				DeleteSavedGame();
				return 1;
			}
			return DLG_SwishOut_(dlg);
		case USER_INIT:
			gluLoad_CustomCtrlID_(dlg);
			break;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluLoad_Main, "starcraft");

void loadMenu_gluLoad_()
{
	glu_load_Dlg = loadAndInitFullMenuDLG_("rez\\gluLoad.bin");

	switch (gluLoadBINDlg_(glu_load_Dlg, gluLoad_Main_))
	{
	case 4:
		CMDRECV_LoadGame(byte_599DA4);
		break;
	case 5:
		glGluesMode = IsExpansion ? MenuPosition::GLUE_EX_CAMPAIGN : MenuPosition::GLUE_CAMPAIGN;
		break;
	default:
		glGluesMode = MenuPosition::GLUE_MAIN_MENU;
		break;
	}

	changeMenu_();
}

FAIL_STUB_PATCH(loadMenu_gluLoad, "starcraft");
