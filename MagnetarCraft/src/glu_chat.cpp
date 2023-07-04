#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"
#include "minimap.h"

void sub_4B8D90_(dialog* dlg)
{
	if (dword_5993AC)
	{
		if (dword_5999D0 == 1)
		{
			updateMinimapPreviewDisplayOffOn(0, dlg, 1);
			dword_5999D0 = 0;
		}
		sub_4EE210_();
		dword_5993AC = 0;
		dword_5994DC = 0;
	}
	else if (minimap_Dlg)
	{
		DestroyDialog(minimap_Dlg);
		minimap_Dlg = NULL;
		if (blink_grp)
		{
			SMemFree(blink_grp, "Starcraft\\SWAR\\lang\\minimap.cpp", 2065, 0);
			blink_grp = NULL;
		}
	}
}

FAIL_STUB_PATCH(sub_4B8D90);

void gluChat_init_(dialog* dlg)
{
	static swishTimer timers[] = {
		{1, 3},
		{2, 2},
		{3, 0},
		{4, 3},
		{5, 2},
	};

	dword_5999D8 = isHost;

	if (!isHost)
	{
		HideDialog_(getControlFromIndex_(dlg, 5));
		HideDialog_(getControlFromIndex_(dlg, 7));
	}
	sub_4B9480(dlg);
	DlgSwooshin_(dlg, timers, 0);
}

FAIL_STUB_PATCH(gluChat_init);

void gluChat_CustomCtrlID_(dialog* dlg)
{
	FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		gluChat_Button,
		Menu_Generic_Button,
		gluChat_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		gluChat_Textbox_Interact,
		gluChat_Listbox_Interact,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		gluChat_GameStatsLabel,
		genericLabelInteract,
		genericLabelInteract,
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
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
	};

	registerMenuFunctions_(functions, dlg, sizeof(functions));
	dword_6D5A38 = 0;
}

FAIL_STUB_PATCH(gluChat_CustomCtrlID);

void SendLobbyMessage_()
{
	if (*dword_5999E4->pszText)
	{
		BWFXN_SendLobbyCallTarget(dword_5999E4->pszText);
	}
	*dword_5999E4->pszText = 0;
	dword_5999E4->fields.edit.bCursorPos = 0;

	if ((dword_5999E4->lFlags & DialogFlags::CTRL_UPDATE) == 0)
	{
		dword_5999E4->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog(dword_5999E4);
	}
}

FAIL_STUB_PATCH(SendLobbyMessage);

int gluChat_controlActivation_(signed int last_control_id, dialog* dlg)
{
	switch (last_control_id)
	{
	case 7:
		if (!InReplay && sub_44F7B0() < 2)
		{
			BWFXN_gluPOK_MBox_(get_GluAll_String_((GluAllTblEntry)0x72));
		}
		else if (!InReplay && isGameTypeSpecial() && getHumansOnTeam(1) < 2)
		{
			BWFXN_gluPOK_MBox_(get_GluAll_String_((GluAllTblEntry)0x0BA));
		}
		else if (map_download && !IsDownloadComplete(map_download))
		{
			BWFXN_gluPOK_MBox_(get_GluAll_String_((GluAllTblEntry)0x73));
		}
		else
		{
			_startGame();
			DisableControl(getControlFromIndex_(dlg, 7));
			updateMinimapPreviewDisplayOffOn(0, dlg, 0);
			dword_5999D0 = 0;
		}
		return 1;
	case 9:
		SendLobbyMessage_();
		return 1;
	case 8:
		DisableControl(getControlFromIndex_(dlg, 8));
		sub_4D3860_();
		[[fallthrough]];
	case 557:
		gameState = 1;
		killTimerFunc_();
		updateMinimapPreviewDisplayOffOn(0, dlg, 1);
		dword_5999D0 = 0;
	}
	lobby_dlg->fields.dlg.pModalFcn = 0;
	return DLG_SwishOut_(dlg);
}

FAIL_STUB_PATCH(gluChat_controlActivation);

void __fastcall MinimapPreviewProc_(dialog* a1, __int16 _timer_id)
{
	if (dword_5993AC != 1 && dword_5994DC && g_LocalNationID < 8u && (!map_download || IsDownloadComplete(map_download)))
	{
		int OriginalIsInGameLoop = IsInGameLoop;
		dword_5993AC = 1;
		IsInGameLoop = 1;
		sub_4EEFD0_();
		IsInGameLoop = OriginalIsInGameLoop;
		waitLoopCntd(1, a1);
	}
}

FAIL_STUB_PATCH(MinimapPreviewProc);

void gluChat_HoverMinimapPreview_(dialog* dlg)
{
	dword_5999DC = 0;
	dword_5999D0 = 1;
	dword_5993AC = 1;
	load_MinimapPreview_();
	dword_5993AC = 0;
	SetCallbackTimer(1, dlg, 1000, MinimapPreviewProc_);
}

FAIL_STUB_PATCH(gluChat_HoverMinimapPreview);

bool __fastcall sub_4B9B10_(dialog* lobby_dlg)
{
	int v2 = LobbyLoopCnt_();

	if (v2 == 75)
	{
		bool result = sub_452250(lobby_dlg);
		update_lobby_glue = 0;
		return result;
	}
	else if (v2 == 83)
	{
		return getErrorStringPair(STAR_EDIT_NOT_FOUND, 556);
	}
	else if (dword_5999E8)
	{
		sub_4D3860_();
		updateMinimapPreviewDisplayOffOn(0, lobby_dlg, 1);
		dword_5999D0 = 0;
		return getErrorStringPair(dword_5999E0, 557);
	}

	bool result = dword_5999E8;
	if (update_lobby_glue)
	{
		result = sub_452250(lobby_dlg);
		update_lobby_glue = 0;
	}
	if (dword_5999CC)
	{
		char game_password[24] = "";
		result = SNetGetGameInfo(SNET_INFO_GAMEPASSWORD, game_password, sizeof(game_password), 0);
		if (!game_password[0])
		{
			updatePasswordDisplay(lobby_dlg);
			dword_5999CC = 0;
		}
	}
	return result;
}

FAIL_STUB_PATCH(sub_4B9B10);

void sub_4B9BF0_(dialog* dlg)
{
	if (sub_4D4130_())
	{
		dlg->fields.dlg.pModalFcn = sub_4B9B10_;
	}
	else
	{
		getErrorStringPair(STAR_EDIT_NOT_FOUND, 557);
	}
}

FAIL_STUB_PATCH(sub_4B9BF0);

bool IsCursorWithin(pt cursor, rect rectangle)
{
	return rectangle.left <= cursor.x && cursor.x <= rectangle.right && rectangle.top <= cursor.y && cursor.y <= rectangle.bottom;
}

int __fastcall gluChat_Main_(dialog* dlg, struct dlgEvent* evt)
{
	dialog* minimap_preview_dlg = getControlFromIndex_(dlg, 6);
	dialog* char_history_dlg = getControlFromIndex_(dlg, 11);
	dialog* starting_in_dlg = getControlFromIndex_(dlg, 23);
	dialog* countdown_dlg = getControlFromIndex_(dlg, 24);

	switch (evt->wNo)
	{
	case EVN_MOUSEMOVE:
		updateMinimapPreviewDisplayOffOn(IsCursorWithin(evt->cursor, minimap_preview_dlg->rct), dlg, 1);
		[[fallthrough]];
	case EVN_CHAR:
		if (evt->wVirtKey == VK_HELP)
		{
			SNetGetLeaguePlayerName((int*)curPlayerID, 0x19u);
		}
		break;
	case EVN_WHEELUP:
		doUserEvent(EventUser::USER_SCROLLUP, 0, char_history_dlg->fields.list.pScrlBar);
		return 1;
	case EVN_WHEELDWN:
		doUserEvent(EventUser::USER_SCROLLDOWN, 0, char_history_dlg->fields.list.pScrlBar);
		return 1;
	case EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gluChat_init_(dlg);
			DLG_SwishIn_(dlg);
			genericDlgInteract(dlg, evt);
			sub_4B9BF0_(dlg);
			return 1;
		case EventUser::USER_DESTROY:
			sub_4B8D70(dlg);
			sub_4B8D90_(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			return gluChat_controlActivation_(LastControlID, dlg);
		case EventUser::USER_INIT:
			gluChat_CustomCtrlID_(dlg);
			if (!InReplay
				&& !gameData.save_timestamp
				&& (gameData.got_file_values.victory_conditions || gameData.got_file_values.starting_units || gameData.got_file_values.tournament_mode))
			{
				gluChat_HoverMinimapPreview_(dlg);
			}
			else
			{
				HideDialog_(minimap_preview_dlg);
			}
			break;
		case 0x405:
			updatePasswordDisplay(dlg);
			dword_68F520 = 0;
			dword_68F4F0 = 1;
			HideDialog_(starting_in_dlg);
			HideDialog_(countdown_dlg);
			dword_5999DC = 1;
			return 1;
		}
		break;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluChat_Main);

void loadMenu_gluChat_()
{
	update_lobby_glue = 0;
	dword_5999E8 = 0;
	commonSwishControllers[9] = { 1, 0 };
	dword_5999CC = 0;
	lobby_dlg = LoadDialog("rez\\gluChat.bin");

	playerOwnerDropdownCreate2(lobby_dlg);
	if (GetUserDefaultLangID() == 1042)
	{
		if (dword_6D6438 == NULL)
		{
			dword_6D6438 = ImmGetContext(hWndParent);
		}
		ImmAssociateContext(hWndParent, dword_6D6438);
	}
	int v9 = gluLoadBINDlg_(lobby_dlg, gluChat_Main_);
	if (GetUserDefaultLangID() == 1042)
	{
		if (dword_6D6438 == NULL)
		{
			dword_6D6438 = ImmGetContext(hWndParent);
		}
		dword_6D6438 = ImmAssociateContext(hWndParent, 0u);
	}
	switch (v9)
	{
	case 556:
		break;
	case 8:
	case 557:
		if (NetMode.as_number == 'BNET')
		{
			glGluesMode = GLUE_BATTLE;
		}
		else if (NetMode.as_number == 'MDMX' || NetMode.as_number == 'MODM')
		{
			glGluesMode = GLUE_MODEM;
		}
		else
		{
			glGluesMode = GLUE_GAME_SELECT;
		}
		InReplay = 0;
		freeChkFileMem_();
		break;
	default:
		glGluesMode = MenuPosition::GLUE_MAIN_MENU;
		break;
	}
	changeMenu_();
}

FAIL_STUB_PATCH(loadMenu_gluChat);
