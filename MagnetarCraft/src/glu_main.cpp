#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"

void gluMainDestroy_(dialog* dlg)
{
	dialog* v1 = getControlFromIndex_(dlg, 10);
	SMemFree(v1->pszText, "Starcraft\\SWAR\\lang\\gluMain.cpp", 452, 0);
}

FAIL_STUB_PATCH(gluMainDestroy, "starcraft");

BOOL cmpgn_WaitForCDRom_(GluAllTblEntry a2, char* filename)
{
	HANDLE phFile;
	if (cd_archive_mpq && SFileOpenFileEx(cd_archive_mpq, filename, 0, &phFile))
	{
		SFileCloseFile(phFile);
		return 1;
	}
	Streamed_SFX_FullDestructor(&soundFXList);
	stopMusic_();
	if (cd_archive_mpq)
	{
		SFileCloseArchive(cd_archive_mpq);
		cd_archive_mpq = 0;
	}
	if (InitializeCDArchives(filename, 0))
	{
		return 1;
	}
	if ((unsigned __int8)BWFXN_gluPOKCancel_MBox_(get_GluAll_String(a2)))
	{
		while (!InitializeCDArchives(filename, 0))
		{
			if (!(unsigned __int8)BWFXN_gluPOKCancel_MBox_(get_GluAll_String(a2)))
			{
				goto LABEL_11;
			}
		}
		return 1;
	}

LABEL_11:
	while (!InitializeCDArchives(0, 0))
	{
		BWFXN_gluPOK_MBox_(get_GluAll_String((GluAllTblEntry)0xA9));
	}
	return cd_archive_mpq && SFileExists(filename, cd_archive_mpq);
}

FAIL_STUB_PATCH(cmpgn_WaitForCDRom, "starcraft");

int SelGameMode_(int a2)
{
	int v4 = LastControlID;
	switch (load_gluGameMode_BINDLG_())
	{
	case 6:
		if (a2 || cmpgn_WaitForCDRom_((GluAllTblEntry)167, "rez\\glucmpgn.bin"))
		{
			IsExpansion = 0;
			LastControlID = v4;
			return 1;
		}
		else
		{
			LastControlID = v4;
			return 0;
		}
	case 7:
		if (cmpgn_WaitForCDRom_((GluAllTblEntry)168, "rez\\gluexpcmpgn.bin"))
		{
			IsExpansion = 1;
			LastControlID = v4;
			return 1;
		}
		else
		{
			LastControlID = v4;
			return 0;
		}
	default:
		LastControlID = v4;
		return 0;
	}
}

FAIL_STUB_PATCH(SelGameMode, "starcraft");
FAIL_STUB_PATCH(sub_4DB6A0, "starcraft");
FAIL_STUB_PATCH(sub_4DB6C0, "starcraft");

int gluMain_DisplayCDRomErrorBinDlg_()
{
	int v0 = LastControlID;
	int result = cmpgn_WaitForCDRom_((GluAllTblEntry)167, "rez\\glucmpgn.bin");
	LastControlID = v0;
	return result;
}

FAIL_STUB_PATCH(gluMain_DisplayCDRomErrorBinDlg, "starcraft");

signed int loadStareditProcess_(dialog* a1)
{
	CHAR CommandLine[MAX_PATH];
	CHAR Filename[MAX_PATH];

	if (!GetModuleFileNameA(hInst, Filename, sizeof(Filename)))
	{
		Filename[0] = 0;
	}
	char* v2 = strrchr(Filename, '\\');
	if (v2)
	{
		v2[1] = 0;
	}
	strcpy_s(CommandLine, Filename);
	strcat_s(CommandLine, "StarEdit.exe");

	struct _PROCESS_INFORMATION ProcessInformation;
	struct _STARTUPINFOA StartupInfo;
	memset(&StartupInfo, 0, sizeof(StartupInfo));
	StartupInfo.cb = 68;

	if (CreateProcessA(0, CommandLine, 0, 0, 0, 0x20u, 0, Filename, &StartupInfo, &ProcessInformation))
	{
		HWND v3 = GetDesktopWindow();
		SetForegroundWindow(v3);
		dialog* v5 = getControlFromIndex_(a1, 5);

		DisableControl(v5);
		WaitForInputIdle(ProcessInformation.hProcess, -1u);
		EnableControl(v5);
		CloseHandle(ProcessInformation.hThread);
		CloseHandle(ProcessInformation.hProcess);
		return 1;
	}
	else
	{
		const char* v8 = get_GluAll_String_(SPAWNED_BY);
		BWFXN_gluPOK_MBox_(v8);
		return 0;
	}
}

FAIL_STUB_PATCH(loadStareditProcess, "starcraft");

void gluMain_CustomCtrlID_(dialog* a1)
{
	static FnInteract functions[] = {
		NULL,
		flc_ctrl_interact,
		flc_ctrl_interact,
		flc_ctrl_interact,
		flc_ctrl_interact,
		NULL,
		NULL,
		Menu_Generic_Button,
		Menu_Generic_Button,
		genericLabelInteract,
		flc_ctrl_interact,
	};

	registerMenuFunctions_(functions, a1, sizeof(functions));
}

FAIL_STUB_PATCH(gluMain_CustomCtrlID, "starcraft");

int __fastcall gluMain_Dlg_Interact_(dialog* dlg, struct dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			gluMainCreate(dlg);
			DLG_SwishIn_(dlg);
			genericDlgInteract(dlg, evt);
			if (!byte_6D5BBC)
			{
				DLGMusicFade_(&title_music);
			}
			return true;
		case USER_DESTROY:
			gluMainDestroy_(dlg);
			break;
		case USER_ACTIVATE:
			switch (LastControlID)
			{
			case 3:
				if (is_spawn)
				{
					const char* s = GetNetworkTblString_(103);
					BWFXN_gluPOK_MBox_(s);
					return true;
				}
				else if (is_expansion_installed)
				{
					if (!SelGameMode_(0))
					{
						return true;
					}
				}
				else
				{
					if (!gluMain_DisplayCDRomErrorBinDlg_())
					{
						return true;
					}
					IsExpansion = 0;
				}
				break;
			case 4:
				if (is_expansion_installed)
				{
					if (!SelGameMode_(1))
					{
						return true;
					}
				}
				else if (is_spawn)
				{
					IsExpansion = 0;
				}
				else
				{
					if (!gluMain_DisplayCDRomErrorBinDlg_())
					{
						return true;
					}
					IsExpansion = 0;
				}
				break;
			case 5:
				loadStareditProcess_(dlg);
				return true;
			}
			return DLG_SwishOut_(dlg);
		case USER_INIT:
			gluMain_CustomCtrlID_(dlg);
			break;
		}
		break;
	case EventNo::EVN_CHAR:
		if (evt->wVirtKey == VK_SPACE)
		{
			return true;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluMain_Dlg_Interact, "starcraft");

void loadMenu_gluMain_()
{
	multiPlayerMode = 0;

	dialog* bin_dialog = (dialog*)fastFileRead_(NULL, 0, "rez\\gluMain.bin", 0, 1, "Starcraft\\SWAR\\lang\\gluMain.cpp", 573);

	if (bin_dialog)
	{
		bin_dialog->lFlags |= DialogFlags::CTRL_ACTIVE;
		AllocInitDialogData(bin_dialog, bin_dialog, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\gluMain.cpp", 624);
	}

	if (bin_dialog)
	{
		GotFileValues* ums_game_template;

		switch (gluLoadBINDlg_(bin_dialog, gluMain_Dlg_Interact_))
		{
		case 2:
			gwGameMode = GamePosition::GAME_EXIT;
			break;
		case 3:
			multiPlayerMode = 0;
			glGluesMode = MenuPosition::GLUE_LOGIN;
			ums_game_template = InitUseMapSettingsTemplate_();
			memcpy(&gameData.got_file_values, ums_game_template, sizeof(gameData.got_file_values));
			SMemFree(ums_game_template, "Starcraft\\SWAR\\lang\\gluMain.cpp", 646, 0);
			break;
		case 4:
			multiPlayerMode = 1;
			glGluesMode = MenuPosition::GLUE_CONNECT;
			ums_game_template = InitUseMapSettingsTemplate_();
			memcpy(&gameData.got_file_values, ums_game_template, sizeof(gameData.got_file_values));
			SMemFree(ums_game_template, "Starcraft\\SWAR\\lang\\gluMain.cpp", 635, 0);
			break;
		case 5:
			break;
		case 8:
			gwGameMode = GamePosition::GAME_INTRO;
			break;
		case 9:
			gwGameMode = GamePosition::GAME_CREDITS;
			break;
		case 65520:
			break;
		default:
			glGluesMode = MenuPosition::GLUE_MAIN_MENU;
			break;
		}
		changeMenu_();
		return;
	}
}

FAIL_STUB_PATCH(loadMenu_gluMain, "starcraft");
