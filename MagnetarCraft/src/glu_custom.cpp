#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"
#include "race.h"

TypeDropdownSelect singleTypeSelect_[]
{
	{PlayerType::PT_NotUsed, (GluAllTblEntry)0x86 },
	{PlayerType::PT_Computer, (GluAllTblEntry)0x84 },
};

void gluCustm_initSwish_(dialog* dlg)
{
	static swishTimer timers[] =
	{
		{1, 0},
		{2, 2},
		{3, 3},
		{4, 2},
		{19, 0},
	};

	DlgSwooshin_(dlg, timers, 0);
	getControlFromIndex_(dlg, 6)->pfcnUpdate = gluCustm_UpdateCB;
}

FAIL_STUB_PATCH(gluCustm_initSwish, "starcraft");

void InitGlueMapListBox_()
{
	map_listbox->lFlags |= DialogFlags::CTRL_LBOX_NORECALC;
	ClearListBox(map_listbox);
	int v1 = getMapListEntryCount_(AddMapToList_CB, (MapDirEntryFlags)40, CurrentMapFolder, byte_59BA68[0] != 0 ? byte_59BA68 : 0);
	if (map_listbox->lFlags & DialogFlags::CTRL_LBOX_NORECALC)
	{
		map_listbox->lFlags &= ~DialogFlags::CTRL_LBOX_NORECALC;
		List_Update(map_listbox);
	}
	if (v1 != 255)
	{
		sub_4A7FC0((MapDirEntry*)map_listbox->fields.list.pdwData[v1]);
	}

	if ((unsigned __int8)v1 < map_listbox->fields.scroll.bSliderSkip || (_BYTE)v1 == 0xFF)
	{
		dlgEvent v5;
		*(_DWORD*)&v5.wSelection = (unsigned __int8)v1;
		v5.wNo = EVN_USER;
		v5.dwUser = USER_SELECT;
		map_listbox->pfcnInteract(map_listbox, &v5);
		DlgSetSelected_UpdateScrollbar(v1, map_listbox);
	}
}

FAIL_STUB_PATCH(InitGlueMapListBox, "starcraft");

int gluCustmLoadMapFromList_()
{
	if (map_listbox->fields.list.bStrs)
	{
		u8 v0 = map_listbox->fields.list.bCurrStr;
		if (v0 != 0xFF)
		{
			GluAllTblEntry error_tbl_entry;
			MapDirEntry* directory_entry = (MapDirEntry*)map_listbox->fields.list.pdwData[v0];
			auto flags = (unsigned __int8)selectedGameType | ((((unsigned __int16)selectedGameTypeParam << 8) | (unsigned __int8)byte_59BA65) << 8);
			unsigned error_code = sub_4A8050_(directory_entry, playerName, 0, flags, byte_59BB6C, CurrentMapFolder);

			switch (error_code)
			{
			case 0:
				if (!dword_59B844)
				{
					char v16[MAX_PATH];
					sub_4DCB00(v16, "");
					strcpy_s(menuMapRelativePath, CurrentMapFolder + strlen(v16));
					strcpy_s(menuMapFileName, map_listbox->fields.list.ppStrs[v0]);
				}
				return multiPlayerMode ? 1 : gluCustmSinglePlayerInit();
			case 0x80000001:
				error_tbl_entry = INVALID_SCENARIO;
				break;
			case 0x80000002:
				error_tbl_entry = EXPANSION_ONLY;
				break;
			case 0x80000005:
				error_tbl_entry = GAME_NETWORK_INITIALIZATION_ERROR;
				break;
			case 0x80000007:
				error_tbl_entry = ERROR_CREATING_GAME;
				break;
			case 0x80000008:
				error_tbl_entry = TOO_MANY_GAMES_ON_NETWORK;
				break;
			case 0x8000000B:
				gluCustm_UpdateMapFolderDisplay(CurrentMapFolder);
				byte_59BA68[0] = 0;
				InitGlueMapListBox_();
				return 0;
			case 0x8000000C:
				error_tbl_entry = UMS_ONLY;
				break;
			case 0x8000000D:
				error_tbl_entry = HUMAN_PLAYER_REQUIRED;
				break;
			default:
				if (SErrGetLastError() != 1222)
				{
					error_tbl_entry = NETWORK_GAME_EXISTS;
				}
				else if (NetMode.as_number == 'MODM')
				{
					error_tbl_entry = (GluAllTblEntry)74;
				}
				else
				{
					error_tbl_entry = OPPONENT_PLAYER_REQUIRED;
				}
			}

			BWFXN_gluPOK_MBox_(get_GluAll_String(error_tbl_entry));
		}
	}

	return 0;
}

FAIL_STUB_PATCH(gluCustmLoadMapFromList, "starcraft");

int __fastcall gluCustm_FileListbox_Main_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			SingleMakeCreateGameDialog(dlg);
			break;
		case USER_DESTROY:
			ClearListBox(map_listbox);
			cleanupBNListboxData();
			break;
		case USER_INIT:
			map_listbox = dlg;
			dlg->lFlags |= DialogFlags::CTRL_PLAIN | DialogFlags::CTRL_FONT_SMALL;
			break;
		case USER_SELECT:
			genericListboxInteract(dlg, evt);
			if (dlg->fields.list.bStrs)
			{
				sub_4A7FC0((MapDirEntry*)dlg->fields.list.pdwData[dlg->fields.list.bCurrStr]);
			}

			dword_59B848 = 1;
			gluCustmInitPlayerTypes(dlg->fields.ctrl.pDlg, 5);
			return 1;
		}
	}

	return genericListboxInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluCustm_FileListbox_Main, "starcraft");

void gluCustm_raceDropdown_(dialog* a1)
{
	a1->lFlags |= DialogFlags::CTRL_LBOX_NORECALC;

	for (RaceId race: SELECTABLE_RACES)
	{
		const char* race_name = getRaceString(race);
		u8 v5 = ListBox_AddEntry(race_name, a1, 0);
		if (v5 == 0xFF)
		{
			break;
		}
		a1->fields.list.pdwData[v5] = race;
	}

	if (a1->lFlags & DialogFlags::CTRL_LBOX_NORECALC)
	{
		a1->lFlags &= ~DialogFlags::CTRL_LBOX_NORECALC;
		List_Update(a1);
	}

	if (a1->fields.scroll.bSliderSkip > 3u)
	{
		dlgEvent v8;
		v8.wNo = EVN_USER;
		v8.dwUser = USER_SELECT;
		*(_DWORD*)&v8.wSelection = 3;
		a1->pfcnInteract(a1, &v8);
		DlgSetSelected_UpdateScrollbar(3u, a1);
	}
}

FAIL_STUB_PATCH(gluCustm_raceDropdown, "starcraft");

void gluCustm_typeDropdown_(dialog* dlg)
{
	unsigned __int8 v2 = 0;
	dlg->lFlags |= DialogFlags::CTRL_LBOX_NORECALC;
	for (const auto& player_type : singleTypeSelect_)
	{
		const char* v4 = get_GluAll_String_(player_type.tbl_entry);
		u8 v5 = ListBox_AddEntry(v4, dlg, 0);
		if (v5 == 0xFF)
		{
			break;
		}
		dlg->fields.list.pdwData[v5] = player_type.player_type;
		v2 = player_type.player_type == PlayerType::PT_Computer ? v5 : 0;
	}

	if (dlg->lFlags & DialogFlags::CTRL_LBOX_NORECALC)
	{
		dlg->lFlags &= ~DialogFlags::CTRL_LBOX_NORECALC;
		List_Update(dlg);
	}
	if (v2 < dlg->fields.list.bStrs || v2 == 0xFF)
	{
		dlgEvent v7;
		v7.wNo = EVN_USER;
		v7.dwUser = USER_SELECT;
		v7.wSelection = v2;
		v7.wUnk_0x06 = 0;
		dlg->pfcnInteract(dlg, &v7);
		DlgSetSelected_UpdateScrollbar(v2, dlg);
	}
}

FAIL_STUB_PATCH(gluCustm_typeDropdown, "starcraft");

int __fastcall gluCustm_PlayerSlot_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gluCustm_typeDropdown_(dlg);
			break;
		case EventUser::USER_INIT:
			dlg->lFlags |= DialogFlags::CTRL_PLAIN;
			break;
		case EventUser::USER_SELECT:
			genericListboxInteract(dlg, evt);
			sub_4ADD90(1, dlg);
			return 1;
		}
	}
	return genericComboboxInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluCustm_PlayerSlot, "starcraft");

int __fastcall gluCustm_RaceSlot_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gluCustm_raceDropdown_(dlg);
			break;
		case EventUser::USER_INIT:
			dlg->lFlags |= DialogFlags::CTRL_PLAIN;
			break;
		}
	}

	return genericComboboxInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluCustm_RaceSlot, "starcraft");

void gluCustm_CustomCtrl_InitializeChildren_(dialog* dlg)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		gluCustm_FileListbox_Main_,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		Menu_Generic_Button,
		Menu_Generic_Button,
		genericLabelInteract,
		genericLabelInteract,
		gluCreate_GameSpeed_Slider,
		gluCustm_GameType,
		gluCustm_SubType,
		NULL,
		gluCustm_Player1,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_PlayerSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		gluCustm_RaceSlot_,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
	};

	registerMenuFunctions_(functions, dlg, sizeof(functions));

	custom_game_mode = getControlFromIndex_(dlg, 17);
	custom_game_submode = getControlFromIndex_(dlg, 18);
	custom_game_slots = getControlFromIndex_(dlg, 19);

	if (!multiPlayerMode)
	{
		byte_59BB6C = registry_options.GameSpeed;
		registry_options.GameSpeed;
	}
}

FAIL_STUB_PATCH(gluCustm_CustomCtrl_InitializeChildren, "starcraft");

int __fastcall gluCustm_Interact_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gluCustm_initSwish_(dlg);
			DLG_SwishIn_(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			if (LastControlID == 12 && !gluCustmLoadMapFromList_())
			{
				return 1;
			}
			waitLoopCntd(5, gluCreateOrCustm_bin);
			return DLG_SwishOut_(dlg);
		case EventUser::USER_INIT:
			gluCustm_CustomCtrl_InitializeChildren_(dlg);
			break;
		case 1029:
			InitGlueMapListBox_();
			break;
		}
	}
	else if (evt->wNo == EventNo::EVN_WHEELUP)
	{
		dlgEvent event;
		event.dwUser = USER_SCROLLUP;
		event.wSelection = 0;
		event.wUnk_0x06 = 0;
		event.wNo = EVN_USER;
		event.cursor.x = Mouse.x;
		event.cursor.y = Mouse.y;

		dialog* v4 = map_listbox->fields.list.pScrlBar;
		v4->pfcnInteract(v4, &event);
		return 1;
	}
	else if (evt->wNo == EventNo::EVN_WHEELDWN)
	{
		dlgEvent event;
		event.dwUser = USER_SCROLLDOWN;
		event.wSelection = 0;
		event.wUnk_0x06 = 0;
		event.wNo = EVN_USER;
		event.cursor.x = Mouse.x;
		event.cursor.y = Mouse.y;

		dialog* v4 = map_listbox->fields.list.pScrlBar;
		v4->pfcnInteract(v4, &event);
		return 1;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluCustm_Interact, "starcraft");

void loadMenu_gluCustm_(int is_multiplayer)
{
	dword_59B844 = is_multiplayer;
	const char* v2 = multiPlayerMode ? "rez\\gluCreat.bin" : "rez\\gluCustm.bin";
	RaceId race;

	gluCreateOrCustm_bin = loadAndInitFullMenuDLG_(v2);
	dword_59BA60 = (void*)LoadGraphic("glue\\create\\iCreate.grp", 0, "Starcraft\\SWAR\\lang\\gluCreat.cpp", 1427);
	dword_6D5A74 = GAME_RUNINIT;
	switch (gluLoadBINDlg_(gluCreateOrCustm_bin, gluCustm_Interact_))
	{
	case 12:
		if (multiPlayerMode)
		{
			glGluesMode = MenuPosition::GLUE_CHAT;
		}
		else if (gameData.got_file_values.victory_conditions
			|| gameData.got_file_values.starting_units
			|| gameData.got_file_values.tournament_mode
			|| InReplay)
		{
			gwGameMode = GAME_RUNINIT;
		}
		else
		{
			race = Players[g_LocalNationID].nRace;
			if (race == RaceId::RACE_Zerg || race == RaceId::RACE_Terran || race == RaceId::RACE_Protoss)
			{
				glGluesMode = Race::races()[race].ready_room_menu;
			}
		}
		break;
	case -3:
	case 13:
		InReplay = 0;
		freeChkFileMem_();

		if (!multiPlayerMode)
		{
			glGluesMode = IsExpansion ? MenuPosition::GLUE_EX_CAMPAIGN : MenuPosition::GLUE_CAMPAIGN;
		}
		else if (NetMode.as_number == 'SCBL')
		{
			glGluesMode = MenuPosition::GLUE_DIRECT;
		}
		else if (NetMode.as_number == 'MODM')
		{
			glGluesMode = MenuPosition::GLUE_MODEM;
		}
		else if (NetMode.as_number == 'BNET')
		{
			glGluesMode = MenuPosition::GLUE_BATTLE;
		}
		else if (NetMode.as_number == 'MDMX')
		{
			glGluesMode = MenuPosition::GLUE_MODEM;
		}
		else
		{
			glGluesMode = MenuPosition::GLUE_GAME_SELECT;
		}
		break;
	default:
		glGluesMode = MenuPosition::GLUE_MAIN_MENU;
	}

	changeMenu_();
	if (dword_59BA60)
		SMemFree(dword_59BA60, "Starcraft\\SWAR\\lang\\gluCreat.cpp", 1484, 0);
	dword_6D5A74 = 0;
}

FAIL_STUB_PATCH(loadMenu_gluCustm, "starcraft");
