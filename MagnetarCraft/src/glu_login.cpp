#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"

void gluLogin_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		gluLogin_Listbox,
		genericLabelInteract,
	};

	static swishTimer timers[] = {
		{1, 0},
		{2, 3},
		{3, 2},
	};

	registerMenuFunctions_(functions, dlg, sizeof(functions));
	DlgSwooshin_(dlg, timers, 0);
}

FAIL_STUB_PATCH(gluLogin_CustomCtrlID, "starcraft");

int __fastcall gluLogin_Main_(dialog* dlg, struct dlgEvent* evt)
{
	dialog* v4;
	dlgEvent v6;

	switch (evt->wNo)
	{
	case EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			gluLogin_Init(dlg);
			DLG_SwishIn_(dlg);
			break;
		case USER_DESTROY:
			_ID_Destructor(&stru_51A220);
			break;
		case USER_ACTIVATE:
			switch (LastControlID)
			{
			case 4: // OK
				if (gluLogin_Activate(dword_5999C0))
				{
					return DLG_SwishOut_(dlg);
				}
				break;
			case 6: // Create character
				gluLogin_CreateCharacter(dword_5999C0);
				break;
			case 7: // Delete character
				gluLogin_DeleteCharacter(dword_5999C0);
				return 1;
			default: // Cancel
				return DLG_SwishOut_(dlg);
			}
			return 1;
		case USER_UNK_8:
			if (evt->wSelection != 87)
			{
				return genericDlgInteract(dlg, evt);
			}
			waitLoopCntd(87, dlg);
			gluLogin_CreateCharacter(dword_5999C0);
			return 1;
		case USER_INIT:
			gluLogin_CustomCtrlID_(dlg);
			break;
		}
		break;
	case EVN_WHEELUP:
		v4 = dword_5999C0->fields.list.pScrlBar;
		v6.dwUser = USER_SCROLLUP;
		v6.wNo = EVN_USER;
		v6.cursor.x = Mouse.x;
		v6.wSelection = 0;
		v6.wUnk_0x06 = 0;
		v6.cursor.y = Mouse.y;
		v4->pfcnInteract(v4, &v6);
		return 1;
	case EVN_WHEELDWN:
		v4 = dword_5999C0->fields.list.pScrlBar;
		v6.dwUser = USER_SCROLLDOWN;
		v6.wNo = EVN_USER;
		v6.cursor.x = Mouse.x;
		v6.wSelection = 0;
		v6.wUnk_0x06 = 0;
		v6.cursor.y = Mouse.y;
		v4->pfcnInteract(v4, &v6);
		return 1;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluLogin_Main, "starcraft");

void loadMenu_gluLogin_()
{
	gluLogin_Dlg = LoadDialog("rez\\gluLogin.bin");

	switch (gluLoadBINDlg_(gluLogin_Dlg, gluLogin_Main_))
	{
	case 4:
		if (!multiPlayerMode)
		{
			glGluesMode = IsExpansion != 0 ? MenuPosition::GLUE_EX_CAMPAIGN : MenuPosition::GLUE_CAMPAIGN;
		}
		else if (NetMode.as_number == 'MDMX' || NetMode.as_number == 'MODM')
		{
			glGluesMode = MenuPosition::GLUE_MODEM;
		}
		else
		{
			glGluesMode = NetMode.as_number == 'SCBL' ? MenuPosition::GLUE_DIRECT : MenuPosition::GLUE_GAME_SELECT;
		}
		break;
	case 5:
		leaveOnQuit(0);
		glGluesMode = multiPlayerMode != 0 ? MenuPosition::GLUE_CONNECT : MenuPosition::GLUE_MAIN_MENU;
		break;
	default:
		glGluesMode = MenuPosition::GLUE_MAIN_MENU;
	}

	changeMenu_();
}

FAIL_STUB_PATCH(loadMenu_gluLogin, "starcraft");
