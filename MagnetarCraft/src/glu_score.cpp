#include <time.h>

#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"
#include "race.h"
#include "exception.h"

int endgameData_(char* a1, size_t a2, char* buff, size_t a4)
{
	if (!a1 || !buff || !dword_59B618)
	{
		return 0;
	}

	__int16 v4;
	if (consoleIndex == RaceId::RACE_Zerg)
	{
		v4 = 7;
	}
	else if (consoleIndex == RaceId::RACE_Terran)
	{
		v4 = 8;
	}
	else if (consoleIndex == RaceId::RACE_Protoss)
	{
		v4 = 9;
	}
	else
	{
		v4 = 8;
	}

	time_t v19 = time(0);
	char* v5 = ctime(&v19);
	char dest[1024];
	SStrVPrintf(dest, sizeof(dest), "%s%s%s", playerName, CurrentMapFileName, v5);
	int v16[5];
	ShaState v17;
	sha1_init(&v17);
	MD5_0(&v17, dest, strlen(dest));
	MD5_1(&v17, (BYTE*)v16);
	game_id_hash = v16[0] ^ v16[1] ^ v16[2] ^ v16[3] ^ v16[4];
	int leagueID = 0;
	SNetGetLeagueName(&leagueID);
	char v14[256] = { 0 };
	unsigned elapsed_time = getElapsedGameTimeSeconds();
	sub_4B2FC0(0xFFu, v14, elapsed_time);
	const char* v7 = GetNetworkTblString_(v4);
	_snprintf(buff, a4, "<leagueid>%d</leagueid>\n<gameid>0x%08x</gameid>\n<race>%s</race>\n<time>%u</time>\n", leagueID, game_id_hash, v7, elapsed_time);

	static ScoreFormatRelated v18[] =
	{
		{overallScoreCalc, "<score overall=\"%u\" units=\"%u\" structures=\"%u\" resources=\"%u\"/>\n", 4, 5, 6},
		{unitScoreCalc, "<units score=\"%u\" produced=\"%u\" killed=\"%u\" lost=\"%u\"/>\n", 7, 8, 9},
		{structureScoreCalc, "<structures score=\"%u\" constructed=\"%u\" razed=\"%u\" lost=\"%u\"/>\n", 10, 11, 12},
		{resourceScoreCalc, "<resources score=\"%u\" gas=\"%u\" minerals=\"%u\" spent=\"%u\"/>\n", 13, 14, 15},
	};

	for (int i = 0; i < _countof(v18); i++)
	{
		char new_[1024];
		sub_4B31C0(stru_59A0F0, byte_59B3D8, 8, v18[i].score_calc);
		sprintf_s(new_, v18[i].format_string,
			stru_59A0F0[g_LocalNationID].total_score_field,
			stru_59A0F0[g_LocalNationID].score_field_0,
			stru_59A0F0[g_LocalNationID].score_field_1,
			stru_59A0F0[g_LocalNationID].score_field_2);
		SStrNCat(buff, "\n  ", a4);
		SStrNCat(buff, new_, a4);
	}
	_snprintf(a1, a2, "<map>%s</map>\n", CurrentMapName);
	return 1;
}

FAIL_STUB_PATCH(endgameData, "starcraft");

int __fastcall gluScore_Tab_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_DESTROY:
			if (dword_6D63BC)
			{
				if (stru_6DC2A4.data)
				{
					SMemFree(stru_6DC2A4.data, "Starcraft\\SWAR\\lang\\gluScore.cpp", 574, 0);
					stru_6DC2A4.data = 0;
				}
			}
			dlg->srcBits.data = 0;
			dword_6D63BC = 0;
			break;
		case USER_ACTIVATE:
			sub_4B4520(dlg);
			return 1;
		case USER_INIT:
			if (!dword_6D63BC)
			{
				char v13[MAX_PATH];
				strcpy(v13, byte_59B628);
				strcat(v13, "untab.pcx");
				AllocBackgroundImage(v13, &stru_6DC2A4, 0, "Starcraft\\SWAR\\lang\\gluScore.cpp", 548);
				dword_6D63BC = 1;
			}
			dlg->pfcnUpdate = (FnUpdate)sub_4B3820;
			dlg->srcBits.wid = stru_6DC2A4.wid;
			dlg->srcBits.ht = stru_6DC2A4.ht;
			dlg->lFlags |= CTRL_UNKOWN1;
			dlg->srcBits.data = stru_6DC2A4.data;
			UpdateDlgOnFlag(dlg);
			break;
		case USER_SELECT:
			if (*(_DWORD*)&evt->wSelection)
			{
				sub_4B4520(dlg);
			}
			return 1;
		case USER_SHOW:
			if (dlg == getControlFromIndex_(dlg, 3))
			{
				sub_4B4520(dlg);
			}
			break;
		}
	}

	return genericOptionInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluScore_Tab, "starcraft");

int __fastcall gluScore_PlayerRaceIcon_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_DESTROY:
			if (dword_6D63C0 && stru_6DB284.data)
			{
				SMemFree(stru_6DB284.data, "Starcraft\\SWAR\\lang\\gluScore.cpp", 473, 0);
				stru_6DB284.data = 0;
				dlg->srcBits.data = 0;
			}
			dword_6D63C0 = 0;
			break;
		case EventUser::USER_INIT:
			if (!dword_6D63C0)
			{
				char v12[260];
				strcpy(v12, byte_59B628);
				strcat(v12, "pInset.pcx");
				AllocBackgroundImage(v12, &stru_6DB284, 0, "Starcraft\\SWAR\\lang\\gluScore.cpp", 455);
				dword_6D63C0 = 1;
			}
			dlg->srcBits = stru_6DB284;
			dlg->lFlags |= DialogFlags::CTRL_UNKOWN1;
			if ((dlg->lFlags & DialogFlags::CTRL_UPDATE) == 0)
			{
				dlg->lFlags |= DialogFlags::CTRL_UPDATE;
				updateDialog(dlg);
			}
			return genericCommonInteract(evt, dlg);
		case EventUser::USER_SHOW:
			if (dlg->lUser)
			{
				int v4 = dlg->lUser;
				if (*(_DWORD*)(v4 + 20))
				{
					return genericCommonInteract(evt, dlg);
				}
			}

			return 1;
		case EventUser::USER_CREATE:
		case EventUser::USER_NEXT:
		case EventUser::USER_UNK_8:
		case EventUser::USER_HIDE:
			return genericCommonInteract(evt, dlg);
		}
	}

	return 0;
}

FAIL_STUB_PATCH(gluScore_PlayerRaceIcon, "starcraft");

int sub_4B4600_(dialog* a1)
{
	const char* v3;
	switch (dword_59B3D0)
	{
	case 0:
	case 3:
		v3 = get_GluAll_String(DISCONNECTED);
		break;
	case 1:
		v3 = get_GluAll_String((GluAllTblEntry)18);
		break;
	case 2:
		v3 = get_GluAll_String((GluAllTblEntry)17);
		break;
	case 4:
		v3 = get_GluAll_String(UNDECIDED);
		break;
	default:
		v3 = NULL;
		break;
	}

	if (dword_6D5A60)
	{
		v3 = get_GluAll_String((GluAllTblEntry)0xB2);
	}
	else if (v3)
	{
		dialog* v2 = getControlFromIndex_(a1, 2);
		v2->pszText = (char*)v3;
		if ((v2->lFlags & DialogFlags::CTRL_UPDATE) == 0)
		{
			v2->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog(v2);
		}
		v2->lFlags |= DialogFlags::CTRL_DLG_ACTIVE;
	}

	getControlFromIndex_(a1, 8)->lFlags |= DialogFlags::CTRL_DLG_ACTIVE;

	char fileName[260];
	strcpy(fileName, byte_59B628);
	strcat(fileName, "pMain.pcx");

	int height;
	int width;
	void* buffer;
	while (!SBmpAllocLoadImage(fileName, 0, &buffer, &width, &height, 0, 0, allocFunction))
	{
		throw FileNotFoundException(fileName, SErrGetLastError());
	}

	dialog* v13 = getControlFromIndex_(a1, 1);
	v13->srcBits.ht = height;
	v13->srcBits.wid = width;
	v13->srcBits.data = (u8*)buffer;
	if ((v13->lFlags & DialogFlags::CTRL_UPDATE) == 0)
	{
		v13->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog(v13);
	}
	return sub_4B42D0(a1);
}

FAIL_STUB_PATCH(sub_4B4600, "starcraft");

int __fastcall gluScore_SaveReplay_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			if (dword_6D5A60 || saveLoadSuccess || active_campaign || is_spawn)
			{
				DisableControl(dlg);
			}
			break;
		case EventUser::USER_ACTIVATE:
			if (LoadSaveGameBIN_Main_(dword_59B75C, Players[g_LocalNationID].nRace))
			{
				dlg->pszText = (char*) get_GluAll_String_((GluAllTblEntry)177); // TODO: fix this cast
				if ((dlg->lFlags & DialogFlags::CTRL_UPDATE) == 0)
				{
					dlg->lFlags = dlg->lFlags | DialogFlags::CTRL_UPDATE;
					updateDialog(dlg);
				}
				DisableControl(dlg);
			}
			return 1;
		}
	}

	return Menu_Generic_Button(dlg, evt);
}

FAIL_STUB_PATCH(gluScore_SaveReplay, "starcraft");

void gluScore_CustomCtrlID_(dialog* dlg)
{
	static swishTimer timers[] = { {1, 0} };

	static FnInteract gluScore_menu_functions[] = {
		genericImageInteract,
		genericLabelInteract,
		gluScore_Tab_,
		gluScore_Tab_,
		gluScore_Tab_,
		gluScore_Tab_,
		Menu_Generic_Button,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		genericLabelInteract,
		gluScore_PlayerRaceIcon_,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon_,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon_,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon_,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon_,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon_,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon_,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_PlayerRaceIcon_,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		statRes_Text_Interact,
		gluScore_PlayerScoreTotal,
		gluScore_SaveReplay_,
	};

	DlgSwooshin_(dlg, timers, 500);
	registerMenuFunctions_(gluScore_menu_functions, dlg, sizeof(gluScore_menu_functions));
}

FAIL_STUB_PATCH(gluScore_CustomCtrlID, "starcraft");

int __fastcall gluScore_Main_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			sub_4B4600_(dlg);
			DLG_SwishIn_(dlg);
			break;
		case USER_DESTROY:
			Sleep(1000);
			sub_4B30A0(dlg);
			break;
		case USER_ACTIVATE:
			stopSounds();
			return DLG_SwishOut_(dlg);
		case USER_INIT:
			gluScore_CustomCtrlID_(dlg);
			break;
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluScore_Main, "starcraft");

const char* victory_screens[] = {
	"glue\\scoreZv\\",
	"glue\\scoreTv\\",
	"glue\\scorePv\\",
};

const char* defeat_screens[] = {
	"glue\\scoreZd\\",
	"glue\\scoreTd\\",
	"glue\\scorePd\\",
};

void loadMenu_gluScore_()
{
	ApplyGameVictoryStatus(dword_59B73C, &dword_59B3D0);

	dword_59B75C = dword_59B3D0 == 1;

	const char* score_screen;
	const MusicTrackDescription* score_scren_music_track;
	if (dword_6D5A60)
	{
		score_screen = victory_screens[RaceId::RACE_Zerg];
		score_scren_music_track = &Race::races()[RaceId::RACE_Zerg].victory_music;
		glGluesMode = Race::races()[RaceId::RACE_Zerg].victory_menu;
	}
	else if (dword_59B75C)
	{
		score_screen = victory_screens[Players[g_LocalNationID].nRace];
		score_scren_music_track = &Race::races()[Players[g_LocalNationID].nRace].victory_music;
		glGluesMode = Race::races()[Players[g_LocalNationID].nRace].victory_menu;
	}
	else
	{
		score_screen = defeat_screens[Players[g_LocalNationID].nRace];
		score_scren_music_track = &Race::races()[Players[g_LocalNationID].nRace].defeat_music;
		glGluesMode = Race::races()[Players[g_LocalNationID].nRace].defeat_menu;
	}

	strcpy(byte_59B628, score_screen);
	DLGMusicFade_(score_scren_music_track);

	char v25[MAX_PATH];
	strcpy(v25, score_screen);
	strcat(v25, "iScore.grp");
	iscore_grp = (void*)LoadGraphic(v25, 0, "Starcraft\\SWAR\\lang\\gluScore.cpp", 1376);

	strcpy(v25, score_screen);
	strcat(v25, "tminimap.pcx");
	if (!SBmpLoadImage(v25, 0, byte_59B730, 12, 0, 0, 0))
	{
		throw FileNotFoundException(v25, SErrGetLastError());
	}
	strcpy(byte_59B628, score_screen);
	sub_4BCA80_(SFX_glue_scorefill);

	gluScore_Dlg = LoadDialog("rez\\gluScore.bin");

	if (gluLoadBINDlg_(gluScore_Dlg, gluScore_Main_) == 7)
	{
		if (multiPlayerMode)
		{
			glGluesMode = BWFXN_NetSelectReturnMenu_();
		}
		else if (!ContinueCampaign_(dword_59B75C))
		{
			glGluesMode = MenuPosition::GLUE_MAIN_MENU;
		}
	}
	else
	{
		sub_4DBF80_();
		glGluesMode = MenuPosition::GLUE_MAIN_MENU;
	}

	changeMenu_();
	stopMusic_();
	if (iscore_grp)
	{
		SMemFree(iscore_grp, "Starcraft\\SWAR\\lang\\gluScore.cpp", 1409, 0);
	}
	dword_59B618 = 0;
}

FAIL_STUB_PATCH(loadMenu_gluScore, "starcraft");
