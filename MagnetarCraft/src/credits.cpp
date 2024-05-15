#include "campaign.h"
#include "magnetorm.h"
#include "starcraft.h"
#include "patching.h"
#include "race.h"

struct EstablishingShotPositionEx
{
	int index;
	const char* tag;
	PrintFlags alignment;
};

EstablishingShotPositionEx establishingShotPositions_[] = {
	{1, "</SCREENLEFT>", PF_VALIGN_CENTER | PF_HALIGN_LEFT},
	{2, "</SCREENRIGHT>", PF_HALIGN_RIGHT | PF_VALIGN_CENTER},
	{3, "</SCREENTOP>", PF_HALIGN_CENTER | PF_VALIGN_CENTER},
	{4, "</SCREENBOTTOM>", PF_HALIGN_CENTER | PF_VALIGN_CENTER},
	{5, "</SCREENCENTER>", PF_HALIGN_CENTER | PF_VALIGN_CENTER},
	{6, "</SCREENLOWERLEFT>", PF_HALIGN_LEFT | PF_VALIGN_BOTTOM},
	{7, "</SCREENUPPERLEFT>", PF_HALIGN_LEFT | PF_VALIGN_TOP},
	{8, "</SCREENUPPERRIGHT>", PF_HALIGN_RIGHT | PF_VALIGN_TOP},
	{9, "</SCREENLOWERRIGHT>", PF_HALIGN_RIGHT | PF_VALIGN_BOTTOM},
};

void creditsDlgDestroy_(dialog* dlg)
{
	for (auto& position : establishingShotPositions_)
	{
		dialog* position_dlg = getControlFromIndex_(dlg, position.index);
		if (position_dlg->pszText)
		{
			SMemFree(position_dlg->pszText, "Starcraft\\SWAR\\lang\\credits.cpp", 566, 0);
		}
	}

	if (!byte_51A0E9)
	{
		memset(stru_6CEB40, 0, sizeof(stru_6CEB40));
		byte_51A0E9 = 1;
		memcpy(stru_6CE720, GamePalette, sizeof(stru_6CE720));
		gluDlgFadePalette(byte_51CEC8);
		BWFXN_RedrawTarget_();
	}
}

FAIL_STUB_PATCH(creditsDlgDestroy, "starcraft");

void sub_4D8840_(int element_length, char* element_start)
{
	if (dword_51CEB0)
	{
		if (dword_51CEB0->pszText)
		{
			SMemFree(dword_51CEB0->pszText, "Starcraft\\SWAR\\lang\\credits.cpp", 321, 0);
			dword_51CEB0->pszText = 0;
			HideDialog_(dword_51CEB0);
		}
		if (element_length)
		{
			dword_51CEB0->pszText = (char*)SMemAlloc(element_length + 1, "Starcraft\\SWAR\\lang\\credits.cpp", 327, 0);
			SStrCopy(dword_51CEB0->pszText, element_start, element_length + 1);
			if ((dword_51CEB0->lFlags & CTRL_UPDATE) == 0)
			{
				dword_51CEB0->lFlags |= CTRL_UPDATE;
				updateDialog_(dword_51CEB0);
			}
			showDialog_(dword_51CEB0);
		}
	}
}

FAIL_STUB_PATCH(sub_4D8840, "starcraft");

void creditsEndPage_(dialog* a1)
{
	if (byte_51CEAC)
	{
		byte_51CEAC = 0;
		sub_41E9E0_(byte_51CEC8);
		memcpy(stru_6CEB40, palette, sizeof(stru_6CEB40));
		if ((a1->lFlags & DialogFlags::CTRL_UPDATE) == 0)
		{
			a1->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog_(a1);
		}
		TitlePaletteUpdate_(byte_51CEC8);
	}
}

FAIL_STUB_PATCH(creditsEndPage, "starcraft");

void __fastcall sub_4D8930_(dialog* dlg, int x, int y, rect* dst)
{
	if (dlg->wIndex - 1 < _countof(establishingShotPositions_))
	{
		PrintXY_flags = establishingShotPositions_[dlg->wIndex - 1].alignment;
	}
	else
	{
		PrintXY_flags = PF_VALIGN_TOP | PF_HALIGN_LEFT;
	}
	dlg->lFlags |= DialogFlags::CTRL_LBOX_NORECALC;
	DlgDrawText(0, dlg, 0, 0);
}

FAIL_STUB_PATCH(sub_4D8930, "starcraft");

int runCreditsScriptCommands_(char* tag, unsigned int tag_length, dialog* dlg)
{
	if (tag_length > 0xE && !_strnicmp(tag, "</BACKGROUND ", 13u))
	{
		creditsSetBackgroundImageFromFile(tag + 13, dlg);
		return 0;
	}
	if (!_strnicmp(tag, "</PLAYSOUND ", 12u))
	{
		char* attribute_begin = tag + 12;
		char* attribute_end = strchr(attribute_begin, '>');
		std::string sound_attribute(attribute_begin, attribute_end);

		PlayWavByFilename_maybe(sound_attribute.c_str());

		return 0;
	}
	if (!_strnicmp(tag, "</FONTSIZE ", 11u))
	{
		char* attribute_begin = tag + 11;
		char* attribute_end = strchr(attribute_begin, '>');
		std::string font_size_attribute(attribute_begin, attribute_end);

		DialogFlags font_size;
		if (font_size_attribute == "10")
		{
			font_size = DialogFlags::CTRL_FONT_SMALLEST;
		}
		else if (font_size_attribute == "14")
		{
			font_size = DialogFlags::CTRL_FONT_SMALL;
		}
		else if (font_size_attribute == "16")
		{
			font_size = DialogFlags::CTRL_FONT_LARGE;
		}
		else // font_size_attribute == "16x"
		{
			font_size = DialogFlags::CTRL_FONT_LARGEST;
		}

		for (int i = 0; i < _countof(establishingShotPositions_); i++)
		{
			auto label = getControlFromIndex_(dlg, i + 1);
			label->lFlags &= ~DialogFlags::CTRL_FONT_LARGEST;
			label->lFlags &= ~DialogFlags::CTRL_FONT_LARGE;
			label->lFlags &= ~DialogFlags::CTRL_FONT_SMALL;
			label->lFlags &= ~DialogFlags::CTRL_FONT_SMALLEST;
			label->lFlags |= font_size;
		}
		return 0;
	}
	if (tag_length > 0xD && !_strnicmp(tag, "</FONTCOLOR ", 12u))
	{
		creditsSetFontColorFromFile(tag + 12);
		return 0;
	}
	if (tag_length > 0xD && !_strnicmp(tag, "</FADESPEED ", 12u))
	{
		creditsSetFadeSpeed(tag + 12);
		return 0;
	}
	if (tag_length > 0xF && !_strnicmp(tag, "</DISPLAYTIME ", 14u))
	{
		creditsSetDisplayTime(tag + 14);
		return 0;
	}
	else if (!_strnicmp(tag, "</PAGE>", tag_length))
	{
		creditsEndPage_(dlg);
		return 1;
	}
	else
	{
		for (auto& position : establishingShotPositions_)
		{
			if (!_strnicmp(tag, position.tag, tag_length))
			{
				if (dword_51CEB0)
				{
					HideDialog_(dword_51CEB0);
				}
				dword_51CEB0 = getControlFromIndex_(dlg, position.index);
				break;
			}
		}
		return 0;
	}
}

FAIL_STUB_PATCH(runCreditsScriptCommands, "starcraft");

int runCredits_(dialog* a1)
{
	if (dword_51CEB8)
	{
		while (true)
		{
			int is_tag;
			char* element_start;

			int element_length = sub_4D86A0(&element_start, &is_tag);
			if (!element_length)
			{
				break;
			}
			if (!is_tag)
			{
				sub_4D8840_(element_length, element_start);
			}
			else if (runCreditsScriptCommands_(element_start, element_length, a1))
			{
				dword_51CEC4 = dword_51CEB4 + GetTickCount();
				return 1;
			}
		}
	}

	DestroyDialog(a1);
	return 0;
}

FAIL_STUB_PATCH(runCredits, "starcraft");

int creditsDlgInit_(dialog* dlg)
{
	for (auto& position : establishingShotPositions_)
	{
		dialog* position_dlg = getControlFromIndex_(dlg, position.index);
		if (position_dlg)
		{
			position_dlg->pszText = 0;
			position_dlg->pfcnUpdate = sub_4D8930_;
		}
	}

	byte_51CEC8 = 1;
	dword_51CEB4 = 5000;
	return runCredits_(dlg);
}

FAIL_STUB_PATCH(creditsDlgInit, "starcraft");

int credits_idle_(dialog* dlg)
{
	if (gwGameMode == GAME_EXIT)
	{
		credits_interrupted = 1;
		DestroyDialog(dlg);
		return 0;
	}
	else if (is_keycode_used[VK_BACK] || GetTickCount() <= dword_51CEC4)
	{
		return 1;
	}
	else
	{
		return runCredits_(dlg);
	}
}

FAIL_STUB_PATCH(credits_idle, "starcraft");

int credits_keyDwn_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wVirtKey)
	{
	case VK_SPACE:
	case VK_RETURN:
		return runCredits_(dlg);
	case VK_ESCAPE:
		credits_interrupted = 1;
		DestroyDialog(dlg);
		return 0;
	default:
		return 1;
	}
}

FAIL_STUB_PATCH(credits_keyDwn, "starcraft");

int __fastcall creditsDlgInteract_(dialog* dlg, struct dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_KEYFIRST:
		if (!credits_keyDwn_(dlg, evt))
		{
			return false;
		}
		break;
	case EventNo::EVN_LBUTTONDOWN:
	case EventNo::EVN_RBUTTONDOWN:
		if (!runCredits_(dlg))
		{
			return false;
		}
		break;
	case EventNo::EVN_IDLE:
		if (!credits_idle_(dlg))
		{
			return false;
		}
		break;
	case EventNo::EVN_USER:
		if (evt->dwUser == 0)
		{
			if (!creditsDlgInit_(dlg))
			{
				return false;
			}
			break;
		}
		else if (evt->dwUser == EventUser::USER_DESTROY)
		{
			creditsDlgDestroy_(dlg);
		}
		break;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(creditsDlgInteract, "starcraft");

void loadInitCreditsBIN_(const char* a1)
{
	dword_51CEA8 = (char*)fastFileRead_(&bytes_read, 0, a1, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	dword_51CEBC = dword_51CEA8;
	dword_51CEB8 = bytes_read;
	credits_interrupted = 0;

	dialog* credits_bin = LoadDialog("rez\\credits.bin");
	gluLoadBINDlg_(credits_bin, creditsDlgInteract_);
	if (dword_51CEA8)
		SMemFree(dword_51CEA8, "Starcraft\\SWAR\\lang\\credits.cpp", 623, 0);
	dword_51CEA8 = 0;
	dword_51CEBC = 0;
	dword_51CEB0 = 0;
}

FAIL_STUB_PATCH(loadInitCreditsBIN, "starcraft");

void BeginCredits_()
{
	int v0 = registry_options.music;
	if (!registry_options.music && registry_options.sfx)
	{
		if (directsound)
		{
			SFileDdaEnd(directsound);
			SFileCloseFile(directsound);
			directsound = 0;
		}
		byte_6D5BBC = 0;
		byte_6D5BBD = 0;
		bigvolume = 0;
		registry_options.music = 50;
	}

	DLGMusicFade_(&Race::races()[RaceId::RACE_Terran].ingame_music[1]);
	credits_interrupted = 0;
	loadInitCreditsBIN_("rez\\crdt_mag.txt");
	if (credits_interrupted == 0 && is_expansion_installed)
	{
		loadInitCreditsBIN_("rez\\crdt_exp.txt");
	}
	if (credits_interrupted == 0)
	{
		loadInitCreditsBIN_("rez\\crdt_lst.txt");
	}
	stopMusic_();
	registry_options.music = v0;
}

FAIL_STUB_PATCH(BeginCredits, "starcraft");
FAIL_STUB_PATCH(sub_4A60D0, "starcraft");
FAIL_STUB_PATCH(sub_48EB90, "starcraft");

void BeginEpilog_()
{
	int v0 = registry_options.music;
	if (!registry_options.music && registry_options.sfx)
	{
		if (directsound)
		{
			SFileDdaEnd(directsound);
			SFileCloseFile(directsound);
			directsound = 0;
		}
		byte_6D5BBC = 0;
		byte_6D5BBD = 0;
		bigvolume = 0;
		registry_options.music = 50;
	}

	while (active_campaign_entry_index < active_campaign->entries.size() && active_campaign->entries[active_campaign_entry_index].entry_type == CampaignMenuEntryType::EPILOG)
	{
		DLGMusicFade_(active_campaign->entries[active_campaign_entry_index].epilog_music_track);
		loadInitCreditsBIN_(active_campaign->entries[active_campaign_entry_index].epilog);

		if (active_campaign_entry_index + 1 == active_campaign->entries.size())
		{
			stopMusic_();
			break;
		}
		else if (active_campaign->entries[active_campaign_entry_index + 1].entry_type == CampaignMenuEntryType::EPILOG)
		{
			active_campaign_entry_index++;
		}
		else if (active_campaign->entries[active_campaign_entry_index + 1].entry_type == CampaignMenuEntryType::CINEMATIC)
		{
			stopMusic_();
			active_campaign_entry_index++;
			break;
		}
		else if (active_campaign->entries[active_campaign_entry_index + 1].entry_type == CampaignMenuEntryType::MISSION)
		{
			break;
		}
	}

	registry_options.music = v0;
}

FAIL_STUB_PATCH(BeginEpilog, "starcraft");

void DisplayEstablishingShot_()
{
	if (!active_campaign && CurrentMapFileName)
	{
		SFileOpenArchive(CurrentMapFileName, 0, 0, &mapArchiveHandle);
		HANDLE handle;
		bool establishingShotExists = SFileOpenFileEx(mapArchiveHandle, "rez\\est.txt", 0, &handle);
		if (handle)
			SFileCloseFile(handle);

		if (establishingShotExists)
			loadInitCreditsBIN_("rez\\est.txt");

		if (mapArchiveHandle)
			SFileCloseArchive(mapArchiveHandle);
	}
	else if (!multiPlayerMode && !(GameCheats & CHEAT_NoGlues) && active_campaign_entry_index != -1)
	{
		if (const char* establishing_shot = active_campaign->entries[active_campaign_entry_index].establishing_shot)
		{
			loadInitCreditsBIN_(establishing_shot);
		}
	}
}

FAIL_STUB_PATCH(DisplayEstablishingShot, "starcraft");
