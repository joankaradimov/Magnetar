#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"

void InitNetProviders_(dialog* dlg)
{
	dlg->lFlags |= DialogFlags::CTRL_LBOX_NORECALC;
	for (TPROVIDER* provider = (int)dword_51A21C > 0 ? dword_51A21C : 0; (int)provider > 0; provider = provider->next)
	{
		ListBox_AddEntry(provider->name, dlg, 0);
	}

	if (dlg->lFlags & DialogFlags::CTRL_LBOX_NORECALC)
	{
		dlg->lFlags &= ~DialogFlags::CTRL_LBOX_NORECALC;
		List_Update(dlg);
	}

	dlgEvent event;
	*(_DWORD*)&event.wSelection = (unsigned __int8)byte_5999B8;
	event.wNo = EVN_USER;
	event.dwUser = USER_SELECT;
	dlg->pfcnInteract(dlg, &event);
	selConn_connectionList_setSelection(dlg);
}

FAIL_STUB_PATCH(InitNetProviders, "starcraft");

void selConn_connectionList_Create_(dialog* a1)
{
	dialog* ok_button = getControlFromIndex_(gluConn_Dlg, 9);
	ok_button->lFlags |= DialogFlags::CTRL_DISABLED;
	InitNetProviders_(a1);
	if ((ok_button->lFlags & DialogFlags::CTRL_UPDATE) == 0)
	{
		ok_button->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog(ok_button);
	}
}

FAIL_STUB_PATCH(selConn_connectionList_Create, "starcraft");

int __fastcall selConn_ConnectionList_Interact_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			selConn_connectionList_Create_(dlg);
			break;
		case EventUser::USER_INIT:
			dlg->lFlags |= DialogFlags::CTRL_PLAIN;
			return genericListboxInteract(dlg, evt);
		case EventUser::USER_SELECT:
			genericListboxInteract(dlg, evt);
			selConn_connectionList_setSelection(dlg);
			return 1;
		}
	}
	return genericListboxInteract(dlg, evt);
}

FAIL_STUB_PATCH(selConn_ConnectionList_Interact, "starcraft");

int __fastcall GatewayListProc_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			dlg->fields.list.pDrawItemFcn = sub_4BA3C0;
			ListBNGateways(dlg); // TODO: Move the gateways to the YML configuration
			break;
		case USER_INIT:
			dlg->lFlags |= DialogFlags::CTRL_PLAIN | DialogFlags::CTRL_FONT_SMALL;
			break;
		case USER_DESTROY:
			sub_4BA240(dlg->fields.list.bStrs ? dlg->fields.list.bCurrStr : -1);
			[[fallthrough]];
		case USER_SELECT:
			genericListboxInteract(dlg, evt);
			return 1;
		}
	}

	return genericListboxInteract(dlg, evt);
}

FAIL_STUB_PATCH(GatewayListProc, "starcraft");

void ConnSel_InitChildren_(dialog* a1)
{
	static FnInteract v2[14] = {
		0,
		0,
		0,
		0,
		selConn_ConnectionList_Interact_,
		0,
		0,
		0,
		Menu_Generic_Button,
		Menu_Generic_Button,
		0,
		GatewayListProc_,
		0,
		0,
	};

	static swishTimer timers[] =
	{
		{1, 0},
		{2, 2},
		{3, 3},
		{4, 2},
		{11, 0},
	};

	DlgSwooshin_(a1, timers, 0);
	registerMenuFunctions_(v2, a1, sizeof(v2));
}

FAIL_STUB_PATCH(ConnSel_InitChildren, "starcraft");

int getGameList_(dialog* dlg)
{
	sub_4D35A0_();

	getControlFromIndex_(dlg, 6)->pszText = (char*)SMemAlloc(128, "Starcraft\\SWAR\\lang\\gluConn.cpp", 511, 0);
	getControlFromIndex_(dlg, 7)->pszText = (char*)SMemAlloc(128, "Starcraft\\SWAR\\lang\\gluConn.cpp", 511, 0);
	getControlFromIndex_(dlg, 13)->pszText = (char*)SMemAlloc(255, "Starcraft\\SWAR\\lang\\gluConn.cpp", 511, 0);

	const char* v11 = get_GluAll_String_((GluAllTblEntry)0xB9);
	dialog* v14 = getControlFromIndex_(gluConn_Dlg, 13);
	SStrCopy(v14->pszText, v11, 0xFFu);
	if ((v14->lFlags & DialogFlags::CTRL_UPDATE) == 0)
	{
		v14->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog(v14);
	}
	SNetEnumProviders(0, Provider_Constructor);
	if (!byte_6D5BBC)
	{
		DLGMusicFade_(&title_music);
	}
	Template_Destructor(&templates_list);
	return LoadGameTemplates_(Template_Constructor);
}

FAIL_STUB_PATCH(getGameList, "starcraft");

int __fastcall ConnSel_Interact_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			getGameList_(dlg);
			DLG_SwishIn_(dlg);
			break;
		case USER_DESTROY:
			DestroyProviderList(dlg);
			break;
		case USER_ACTIVATE:
			if (LastControlID == 9)
			{
				if (!BeginBNET())
				{
					return 1;
				}
				LastControlID = 9;
			}
			return DLG_SwishOut_(dlg);
		case USER_INIT:
			ConnSel_InitChildren_(dlg);
			break;
		case 0x405:
			showDialog_(getControlFromIndex_(gluConn_Dlg, 12));
			showDialog_(getControlFromIndex_(gluConn_Dlg, 13));
			showDialog_(getControlFromIndex_(gluConn_Dlg, 14));
			break;
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(ConnSel_Interact, "starcraft");

int Begin_BNET_(Char4 network_provider_id)
{
	if (network_provider_id.as_number != 'BNET')
	{
		return InitializeNetworkProvider_(network_provider_id);
	}
	sub_4DCEE0();
	dword_50E064 = -1;
	sub_4AD140_();
	if (InitializeNetworkProvider_(network_provider_id))
	{
		return 1;
	}
	sub_4ACF20();
	return 0;
}

FAIL_STUB_PATCH(Begin_BNET, "starcraft");

void loadMenu_gluConn_()
{
	gluConn_Dlg = LoadDialog("rez\\gluConn.bin");

	if (gluLoadBINDlg_(gluConn_Dlg, ConnSel_Interact_) != 9)
	{
		glGluesMode = MenuPosition::GLUE_MAIN_MENU;
	}
	else if (network_provider_id.as_number == 'BNET')
	{
		stopMusic_();
		glGluesMode = Begin_BNET_(network_provider_id) ? glGluesRelated_maybe : GLUE_CONNECT;
	}
	else
	{
		glGluesMode = glGluesRelated_maybe;
	}

	changeMenu_();
}

FAIL_STUB_PATCH(loadMenu_gluConn, "starcraft");
