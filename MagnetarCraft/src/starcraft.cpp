#include <algorithm>
#include <filesystem>
#include <fstream>
#include <ddraw.h>
#include <process.h>
#include "CBullet.h"
#include "CSprite.h"
#include "iscript.h"
#include "starcraft.h"
#include "race.h"
#include "magnetorm.h"
#include "tbl_file.h"
#include "patching.h"
#include "glu_campaign.h"
#include "glu_campaign_exp.h"
#include "glu_chat.h"
#include "glu_connect.h"
#include "glu_custom.h"
#include "glu_join.h"
#include "glu_load.h"
#include "glu_login.h"
#include "glu_main.h"
#include "glu_ready_room.h"
#include "glu_score.h"
#include "glu_title.h"
#include "exception.h"
#include "minimap.h"
#include "video.h"

std::function<void()> on_end_game = nullptr;
bool frame_capping = true;
bool has_viewport = true;
bool has_hud = true;
bool end_mission_prompt = true;
bool keep_app_active_in_background = false;

void SetGameSpeed_maybe_(int game_speed, unsigned __int8 a2, unsigned speed_multiplier)
{
	registry_options.GameSpeed = game_speed;
	replay_speed_multiplier = speed_multiplier;
	is_replay_paused = a2;

	if (!frame_capping)
	{
		dword_51BFDC[0] = 0;
		dword_51BFDC[1] = 0;
		dword_51BFDC[2] = 0;
		dword_51BFDC[3] = 0;
		dword_51BFDC[4] = 0;
		dword_51BFDC[5] = 0;
		dword_51BFDC[6] = 0;
	}
	else if (game_speed)
	{
		dword_51BFDC[0] = 0xA7 / speed_multiplier;
		dword_51BFDC[1] = 0x6F / speed_multiplier;
		dword_51BFDC[2] = 0x53 / speed_multiplier;
		dword_51BFDC[3] = 0x43 / speed_multiplier;
		dword_51BFDC[4] = 0x38 / speed_multiplier;
		dword_51BFDC[5] = 0x30 / speed_multiplier;
		dword_51BFDC[6] = 0x2A / speed_multiplier;
	}
	else
	{
		dword_51BFDC[0] = 167 * speed_multiplier;
		dword_51BFDC[1] = 111 * speed_multiplier;
		dword_51BFDC[2] = 83 * speed_multiplier;
		dword_51BFDC[3] = 67 * speed_multiplier;
		dword_51BFDC[4] = 56 * speed_multiplier;
		dword_51BFDC[5] = 48 * speed_multiplier;
		dword_51BFDC[6] = 42 * speed_multiplier;
	}

	GameSpeedModifiers.gameSpeedModifiers[0] = dword_51BFDC[0];
	GameSpeedModifiers.gameSpeedModifiers[1] = dword_51BFDC[1];
	GameSpeedModifiers.gameSpeedModifiers[2] = dword_51BFDC[2];
	GameSpeedModifiers.gameSpeedModifiers[3] = dword_51BFDC[3];
	GameSpeedModifiers.gameSpeedModifiers[4] = dword_51BFDC[4];
	GameSpeedModifiers.gameSpeedModifiers[5] = dword_51BFDC[5];
	GameSpeedModifiers.gameSpeedModifiers[6] = dword_51BFDC[6];

	resetLastInputFrameCounts();
	CanUpdateCurrentButtonSet = 1;
	CanUpdateSelectedUnitPortrait = 1;
	CanUpdateStatDataDialog = 1;
	ctrl_under_mouse = 0;
	ctrl_under_mouse_val = 0;
}

void __cdecl SetGameSpeed_maybe__()
{
	int game_speed;
	unsigned __int8 a2;
	unsigned int speed_multiplier;

	__asm mov game_speed, eax
	__asm mov a2, dl
	__asm mov speed_multiplier, ecx

	SetGameSpeed_maybe_(game_speed, a2, speed_multiplier);
}

FUNCTION_PATCH((void*)0x4DEB90, SetGameSpeed_maybe__);

void SetCursorClipBounds_()
{
	if (has_viewport)
	{
		POINT top_left;
		POINT bottom_right;

		top_left.x = 0;
		top_left.y = 0;
		bottom_right.x = SCREEN_WIDTH;
		bottom_right.y = SCREEN_HEIGHT;
		ClientToScreen(hWndParent, &top_left);
		ClientToScreen(hWndParent, &bottom_right);
		SetRect(&screen, top_left.x, top_left.y, bottom_right.x, bottom_right.y);
	}
}

FUNCTION_PATCH(SetCursorClipBounds, SetCursorClipBounds_);

void InitializeInputProcs_()
{
	if (dword_6D1208)
	{
		dword_6D1208();
		dword_6D1208 = 0;
	}

	memset(input_procedures, 0, sizeof(input_procedures));
	SetCursorClipBounds_();

	dword_6D5DD4 = 0;
	if (has_viewport && dword_6D5DD0)
	{
		ClipCursor(&screen);
	}
	hAccTable = dword_5968F8;
	input_procedures[EventNo::EVN_SYSCHAR] = input_standardSysHotkeys;
}

FAIL_STUB_PATCH(InitializeInputProcs);

int IsOutsideGameScreen_(int x, int y)
{
	if (y < possible_gamescreen_y_min)
	{
		return 0;
	}
	if (y >= possible_gamescreen_y_max)
	{
		return 1;
	}
	if (cached_game_screen_region_check_x != x || cached_game_screen_region_check_y != y)
	{
		cached_game_screen_region_check_y = y;
		cached_game_screen_region_check_x = x;
		cached_game_screen_region_check_result = !STransPointInMask(handle, x, y);
	}

	return cached_game_screen_region_check_result;
}

FAIL_STUB_PATCH(IsOutsideGameScreen);

u16 setBuildingSelPortrait_(UnitType unit_type)
{
	u16 portrait_id = Unit_IdlePortrait[unit_type];

	if (active_campaign)
	{
		if (portrait_id == 38 && CampaignIndex > EMD_protoss10)
		{
			return Unit_IdlePortrait[Special_Cerebrate_Daggoth];
		}
		if (portrait_id == 60 && CampaignIndex >= EMD_protoss07)
		{
			return Unit_IdlePortrait[Hero_Fenix_Dragoon];
		}
	}

	return portrait_id;
}

FAIL_STUB_PATCH(setBuildingSelPortrait);

void __fastcall sub_45EC40_(dialog* a1, __int16 timer_id)
{
	int v11 = 0;
	switch (timer_id)
	{
	case 1:
		if ((a1->lFlags & CTRL_VISIBLE) && video)
		{
			if (!sub_45E4C0(video, &v11, 0))
			{
				if (dword_68AC60)
				{
					displayUpdatePortrait(a1->wUser, (CUnit*)a1->lUser, 2);
				}
				else
				{
					CUnit* v7 = ActivePortraitUnit;
					UnitType LastQueueSlotType = getLastQueueSlotType(ActivePortraitUnit);
					u16 v9 = setBuildingSelPortrait_(LastQueueSlotType);
					displayUpdatePortrait(v9, v7, 1);
				}
			}
			if (byte_6D5CA0)
			{
				if (!v11)
				{
					return;
				}
				byte_6D5CA0 = 0;
				byte_68AC9C = 50;
			}
			if (!v11)
			{
				return;
			}
			goto LABEL_25;
		}
		break;
	case 2:
		if ((a1->lFlags & CTRL_VISIBLE) != 0 && video)
		{
			if (byte_6D5CA0)
			{
				v11 = 1;
			}
			else
			{
				if ((unsigned __int8)byte_68AC9C >= 0x50u)
				{
					return;
				}
				byte_68AC9C += 10;
				v11 = 1;
			}
		LABEL_25:
			if ((a1->lFlags & CTRL_UPDATE) == 0)
			{
				a1->lFlags |= CTRL_UPDATE;
				updateDialog(a1);
			}
		}
		break;
	case 3:
		dword_68AC60 = 0;
		waitLoopCntd(timer_id, a1);
		if (video)
		{
			SVidPlayEnd(video);
			video = 0;
		}
		CUnit* v3 = ActivePortraitUnit;
		if (ActivePortraitUnit)
		{
			UnitType v4 = getLastQueueSlotType(ActivePortraitUnit);
			u16 v5 = setBuildingSelPortrait_(v4);
			displayUpdatePortrait(v5, v3, 1);
		}
		else
		{
			dword_68AC98->lUser = 0;
			dword_68AC98->wUser = -1;
			HideDialog(dword_68AC98);
		}
		if (!v11)
		{
			return;
		}
		goto LABEL_25;
	}
	return;
}

FAIL_STUB_PATCH(sub_45EC40);

void keyPress_Escape_()
{
	if (!multiPlayerMode && active_campaign && CampaignIndex < MapData::MD_xprotoss01)
	{
		for (TriggerListEntry* i = (int)stru_51A280[g_LocalNationID].begin <= 0 ? 0 : stru_51A280[g_LocalNationID].begin; (int)i > 0; i = i->next)
		{
			if ((i->container.dwExecutionFlags & 1) != 0 && (i->container.dwExecutionFlags & 0x40) == 0)
			{
				i->container.dwExecutionFlags |= 0x10;
				byte_6509B8[g_LocalNationID] = 0;
				dword_650980[g_LocalNationID] = 0;
				word_6509A0 = 1;
				Streamed_SFX_FullDestructor(&soundFXList);
				if (dword_68AC4C)
				{
					dword_68AC4C = 0;
					waitLoopCntd(4, dword_68AC98);
					sub_45EC40_(dword_68AC98, 3);
				}
				refreshGameTextIfFlagIsSet();
			}
		}
	}
}

FAIL_STUB_PATCH(keyPress_Escape);

void __fastcall input_Game_UserKeyPress_(dlgEvent* event)
{
	if (event->wVirtKey >= '0' && event->wVirtKey <= '9')
	{
		selectUnitGroup(event->wVirtKey - '0');
	}
	else if (event->wVirtKey == VK_ESCAPE)
	{
		keyPress_Escape_();
	}
}

FAIL_STUB_PATCH(input_Game_UserKeyPress);

void sub_496E90_(unsigned __int8 a1)
{
	centerviewUnitGroup(a1);
	selectUnitGroup(a1);
}

FAIL_STUB_PATCH(sub_496E90);

void __fastcall CMDACT_Hotkey_(dlgEvent* event)
{
	int wVirtKey = (__int16)event->wVirtKey;
	if (wVirtKey == -25498)
	{
		if (registry_options.Music)
		{
			dword_5999B4 = registry_options.Music;
			registry_options.Music = 0;
		}
		else
		{
			registry_options.Music = dword_5999B4;
		}
		muteBgm(&registry_options);
	}
	else if (wVirtKey == -25497)
	{
		muteSfx();
	}
	if (!byte_6D5BC2)
	{
		switch (wVirtKey)
		{
		case -25493:
			ExitGameMenu();
			break;
		case -25491:
			QuitMissionMenu();
			break;
		case -25489:
			loadoptionsMenu();
			return;
		case -25484:
			HelpMenu();
			return;
		case -25483:
			ToggleLeaderboardList();
			break;
		case -25482:
			savegameMenu();
			break;
		case -25481:
			LoadGame_DlgCreate();
			break;
		case -25477:
			gameMenu();
			return;
		default:
			break;
		}
		if (!IS_GAME_PAUSED)
		{
			GroupUnitsCommand buffer;

			switch (wVirtKey)
			{
			case -25645:
				buffer.command_id = CommandId::CMD_GroupUnits;
				buffer.f1 = 2;
				buffer.f2 = 1;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				return;
			case -25644:
				buffer.command_id = CommandId::CMD_GroupUnits;
				buffer.f1 = 2;
				buffer.f2 = 2;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				return;
			case -25643:
				buffer.command_id = CommandId::CMD_GroupUnits;
				buffer.f1 = 2;
				buffer.f2 = 3;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				return;
			case -25642:
				buffer.command_id = CommandId::CMD_GroupUnits;
				buffer.f1 = 2;
				buffer.f2 = 4;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				return;
			case -25641:
				buffer.command_id = CommandId::CMD_GroupUnits;
				buffer.f1 = 2;
				buffer.f2 = 5;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				return;
			case -25640:
				buffer.command_id = CommandId::CMD_GroupUnits;
				buffer.f1 = 2;
				buffer.f2 = 6;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				return;
			case -25639:
				buffer.command_id = CommandId::CMD_GroupUnits;
				buffer.f1 = 2;
				buffer.f2 = 7;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				return;
			case -25638:
				buffer.command_id = CommandId::CMD_GroupUnits;
				buffer.f1 = 2;
				buffer.f2 = 8;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				return;
			case -25637:
				buffer.command_id = CommandId::CMD_GroupUnits;
				buffer.f1 = 2;
				buffer.f2 = 9;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				return;
			case -25636:
				buffer.f1 = 2;
				buffer.f2 = 0;
				buffer.command_id = CommandId::CMD_GroupUnits;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				break;
			case -25635:
				buffer.f1 = 0;
				buffer.f2 = 1;
				buffer.command_id = CommandId::CMD_GroupUnits;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				break;
			case -25634:
				buffer.f1 = 0;
				buffer.f2 = 2;
				buffer.command_id = CommandId::CMD_GroupUnits;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				break;
			case -25633:
				buffer.f1 = 0;
				buffer.f2 = 3;
				buffer.command_id = CommandId::CMD_GroupUnits;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				break;
			case -25632:
				buffer.f1 = 0;
				buffer.f2 = 4;
				buffer.command_id = CommandId::CMD_GroupUnits;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				break;
			case -25631:
				buffer.f1 = 0;
				buffer.f2 = 5;
				buffer.command_id = CommandId::CMD_GroupUnits;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				break;
			case -25630:
				buffer.f1 = 0;
				buffer.f2 = 6;
				buffer.command_id = CommandId::CMD_GroupUnits;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				break;
			case -25629:
				buffer.f1 = 0;
				buffer.f2 = 7;
				buffer.command_id = CommandId::CMD_GroupUnits;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				break;
			case -25628:
				buffer.f1 = 0;
				buffer.f2 = 8;
				buffer.command_id = CommandId::CMD_GroupUnits;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				break;
			case -25627:
				buffer.f1 = 0;
				buffer.f2 = 9;
				buffer.command_id = CommandId::CMD_GroupUnits;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				break;
			case -25626:
				buffer.f1 = 0;
				buffer.f2 = 0;
				buffer.command_id = CommandId::CMD_GroupUnits;
				BWFXN_QueueCommand__(buffer);
				byte_597280 = -1;
				break;
			case -25615:
				sub_496E90_(1);
				break;
			case -25614:
				sub_496E90_(2);
				break;
			case -25613:
				sub_496E90_(3);
				break;
			case -25612:
				sub_496E90_(4);
				break;
			case -25611:
				sub_496E90_(5);
				break;
			case -25610:
				sub_496E90_(6);
				break;
			case -25609:
				sub_496E90_(7);
				break;
			case -25608:
				sub_496E90_(8);
				break;
			case -25607:
				sub_496E90_(9);
				break;
			case -25606:
				sub_496E90_(0);
				break;
			case -25500:
				if (ActivePortraitUnit)
				{
					moveScreenToUnit(ActivePortraitUnit);
				}
				break;
			case -25480:
				if (!multiPlayerMode && registry_options.GameSpeed < 6)
				{
					CMDACT_ChangeGameSpeed(LOBYTE(registry_options.GameSpeed) + 1);
				}
				break;
			case -25478:
				if (!multiPlayerMode && registry_options.GameSpeed)
				{
					CMDACT_ChangeGameSpeed(LOBYTE(registry_options.GameSpeed) - 1);
				}
				break;
			case -25476:
				saveScreenLocation(0);
				break;
			case -25475:
				saveScreenLocation(1);
				break;
			case -25474:
				saveScreenLocation(2);
				break;
			case -25473:
				recallScreenLocation(0);
				break;
			case -25472:
				recallScreenLocation(1);
				break;
			case -25471:
				recallScreenLocation(2);
				break;
			default:
				return;
			}
		}
	}
}

FAIL_STUB_PATCH(CMDACT_Hotkey);

void __fastcall input_placeBuilding_LeftMouseClick_(dlgEvent* event)
{
	if (!IsOutsideGameScreen_((__int16)event->cursor.x, (__int16)event->cursor.y))
	{
		refreshScreen();

		BYTE v1 = 0;
		UnitType v3 = UnitType(0);
		input_placeBuilding_Click_Notify(&v1, &v3, 0);

		if (v3 != 228)
		{
			PlaceBuildingCommand command;
			command.command_id = CommandId::CMD_PlaceBuilding;
			command.position = placeBuilding;
			command.f2 = v1;
			command.unit_type = v3;
			BWFXN_QueueCommand__(command);
			resetGameInputProcs(CUR_ARROW);
		}
	}
}

FAIL_STUB_PATCH(input_placeBuilding_LeftMouseClick);

void __fastcall input_placeBuilding_RightMouseClick_(dlgEvent* event)
{
	if (is_placing_building)
	{
		refreshLayer3And4();
		refreshPlaceBuildingLocation();
	}
}

FAIL_STUB_PATCH(input_placeBuilding_RightMouseClick);

IScriptProgram* sub_4D4D70_(int a1)
{
	return (IScriptProgram*)&iscript_data->data[a1];
}

FAIL_STUB_PATCH(sub_4D4D70);

int sub_42D600_(Anims animation)
{
	IScriptProgram* program = sub_4D4D70_(program_state.iscript_header);
	program_state.anim = animation;
	if (animation > program->scpe_magic)
	{
		return 0;
	}
	program_state.program_counter = program->headers[animation];
	if (program_state.program_counter)
	{
		program_state.wait = 0;
		program_state.return_address = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

FAIL_STUB_PATCH(sub_42D600);

void sub_42D8C0_(CUnit* unit)
{
	if (unit->flingyMovementType == 2)
	{
		program_state = unit->sprite->pImagePrimary->iscript_program;
		if (sub_42D600_(Anims::AE_Walking))
		{
			CUnit* v1 = iscript_unit;
			int v9 = 0;
			for (int i = 0; i < 32; i++)
			{
				int distance_moved = 0;
				iscript_unit = unit;
				BWFXN_PlayIscript_(unit->sprite->pImagePrimary, &program_state, 1, &distance_moved);
				iscript_unit = v1;
				int v5 = 0;
				if (unit->status.stimTimer)
				{
					v5 = 1;
				}
				if (unit->statusFlags & SpeedUpgrade)
				{
					++v5;
				}
				if (unit->status.ensnareTimer)
				{
					--v5;
				}

				if (v5 > 0)
				{
					distance_moved *= 2;
				}
				else if (v5 < 0)
				{
					distance_moved -= ((unsigned int)distance_moved >> 2);
				}

				v9 += distance_moved;
			}
			unit->flingyTopSpeed = v9 / 32;
		}
	}
}

void sub_42D8C0__()
{
	CUnit* unit;

	__asm mov unit, esi

	sub_42D8C0_(unit);
}

FUNCTION_PATCH((void*)0x42D8C0, sub_42D8C0__);


void __fastcall iscriptSomething_Death_(CImage* image)
{
	char v2 = BYTE1(image->coloringData);
	if (v2)
	{
		BYTE1(image->coloringData) = v2 - 1;
	}
	else
	{
		u8 v3 = (unsigned __int8)image->coloringData;
		BYTE1(image->coloringData) = 2;
		if (v3 >= 0x3Fu)
		{
			if (image->iscript_program.anim != Anims::AE_Death)
			{
				init_iscript_program_state(&image->iscript_program, Anims::AE_Death);
				BWFXN_PlayIscript_(image, &image->iscript_program, 0, 0);
			}
			iscript_flingy->orderSignal |= 1u;
		}
		else
		{
			LOBYTE(image->coloringData) = v3 + 1;
		}
		image->flags |= ImageFlags::IF_REDRAW;
	}
}

FUNCTION_PATCH(iscriptSomething_Death, iscriptSomething_Death_);

void DLGErrFatal_()
{
	if (GetCurrentThreadId() == main_thread_id)
	{
		SErrSuppressErrors(1);
		AppExit_(1);
		ProcError(1);
		exit(1);
	}
	ExitProcess(1);
}

FAIL_STUB_PATCH(DLGErrFatal);

bool ErrMessageBox_(__int16 a1, LPCSTR lpText, LPCSTR lpCaption)
{
	UINT type;

	switch (a1)
	{
	case 1:
		type = MB_ICONWARNING;
		break;
	case 2:
		type = MB_ICONQUESTION | MB_OKCANCEL;
		break;
	case 4:
		type = MB_ICONQUESTION | MB_YESNO | MB_OKCANCEL;
		break;
	case 8:
		type = MB_ICONINFORMATION;
		break;
	default:
		type = MB_ICONERROR;
		break;
	}
	bool result = MessageBoxA(hWndParent, lpText, lpCaption, type) != IDCANCEL;
	SetCursor(0);
	return result;
}

FAIL_STUB_PATCH(ErrMessageBox);

void BWFXN_DSoundDestroy_();

void sub_421510_()
{
	SErrSuppressErrors(1);
	SNetLeaveGame(3);
	SNetDestroy();
	if (GetCurrentThreadId() == main_thread_id)
	{
		BWFXN_DDrawDestroy();
		BWFXN_DSoundDestroy_();
	}
}

FAIL_STUB_PATCH(sub_421510);

void FatalError_(const char* arg0, ...)
{
	va_list va;

	va_start(va, arg0);
	vsprintf_s(fatal_error_message, arg0, va);
	fatal_error_message[511] = 0;
	ErrorLogSystemInfo();
	ErrorLog("%s", fatal_error_message);
	ErrorLog("\r\n");
	ProcError(1);
	sub_421510_();
	ErrMessageBox_(16, fatal_error_message, "ERROR");
	DLGErrFatal_();
}

FUNCTION_PATCH(FatalError, FatalError_);

std::unordered_map<u16, u16> iscript_id_to_pc;

void isValidScript_(CImage* image, int iscript_id)
{
	auto pc_iterator = iscript_id_to_pc.find(iscript_id);
	if (pc_iterator == iscript_id_to_pc.end())
	{
		FatalError("script %d does not exist", iscript_id);
	}
	image->iscript_program.iscript_header = pc_iterator->second;
}

void isValidScript__()
{
	CImage* image;
	int a2;

	__asm mov image, ebx
	__asm mov a2, edi

	isValidScript_(image, a2);
}

FUNCTION_PATCH((void*)0x4D6640, isValidScript__);

void PlayWarpInOverlay_(CImage* image)
{
	isValidScript_(image, 193);
	init_iscript_program_state(&image->iscript_program, Anims::AE_Init);
	BWFXN_PlayIscript_(image, &image->iscript_program, 0, 0);
	isValidScript_(image, Images_IscriptEntry[image->imageID]);
	image->paletteType = 12;
	image->updateFunction = update_functions[12].update_function;
	image->renderFunction = (image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) ? render_functions[12].RenderFunction2 : render_functions[12].RenderFunction1;
	image->flags |= ImageFlags::IF_REDRAW;
	BWFXN_PlayIscript_(image, &image->iscript_program, 0, 0);
}

FAIL_STUB_PATCH(PlayWarpInOverlay);

void orders_bldgUnderConstruction_Protoss_(CUnit* unit)
{
	switch (unit->orderState)
	{
	case 0:
		if (!unit->remainingBuildTime)
		{
			for (CImage* image = unit->sprite->pImageHead; image; image = image->next)
			{
				if (image->flags & 0x10)
				{
					init_iscript_program_state(&image->iscript_program, Anims::AE_SpecialState1);
					BWFXN_PlayIscript_(image, &image->iscript_program, 0, 0);
				}
			}
			PlaySound(SFX_Protoss_SHUTTLE_pshBld03, unit, 1, 0);
			unit->orderState = 1;
		}
		break;
	case 1:
		if (unit->orderSignal & 1)
		{
			unit->orderSignal &= ~1;
			ReplaceSpriteOverlayImage(unit->sprite, Sprites_Image[unit->sprite->spriteID], 0);
			PlayWarpInOverlay_(unit->sprite->pImagePrimary);
			unit->orderState = 2;
		}
		return;
	case 2:
		if (unit->orderSignal & 1)
		{
			unit->orderSignal &= ~1;
			ReplaceSpriteOverlayImage(unit->sprite, Sprites_Image[unit->sprite->spriteID], 0);
			for (CImage* image = unit->sprite->pImageHead; image; image = image->next)
			{
				if (image->flags & ImageFlags::IF_HAS_ISCRIPT_ANIMATIONS)
				{
					init_iscript_program_state(&image->iscript_program, Anims::AE_WarpIn);
					BWFXN_PlayIscript_(image, &image->iscript_program, 0, 0);
				}
			}
			unit->orderState = 3;
		}
		return;
	case 3:
		if (unit->orderSignal & 1)
		{
			unit->orderSignal &= ~1;
			updateUnitStatsFinishBuilding(unit);
			updateUnitStrengthAndApplyDefaultOrders(unit);
			getUnitCollision(unit);
			if (unit->statusFlags & DoodadStatesThing)
			{
				for (CImage* image = unit->sprite->pImageHead; image; image = image->next)
				{
					if (image->flags & 0x10)
					{
						init_iscript_program_state(&image->iscript_program, Anims::AE_Disable);
						BWFXN_PlayIscript_(image, &image->iscript_program, 0, 0);
					}
				}
			}
			break;
		}
		return;
	}

	if (unit->remainingBuildTime)
	{
		if (GameCheats & CheatFlags::CHEAT_OperationCwal)
		{
			if (unit->remainingBuildTime < 16)
			{
				unit->remainingBuildTime = 0;
			}
			else
			{
				unit->remainingBuildTime -= 16;
			}
		}
		else
		{
			unit->remainingBuildTime -= 1;
		}
	}
	u16 hp_gain = (GameCheats & CheatFlags::CHEAT_OperationCwal) ? 16 * unit->hpGain : unit->hpGain;
	SetUnitHp(unit, unit->hitPoints + hp_gain);

	u16 shield_gain = (GameCheats & CheatFlags::CHEAT_OperationCwal) ? 16 * unit->shieldGain : unit->shieldGain;
	if (Unit_MaxShieldPoints[unit->unitType] * 256 < (int)(unit->shieldPoints + shield_gain)) // TODO: this looks like a bug
	{
		unit->shieldPoints = Unit_MaxShieldPoints[unit->unitType] * 256;
	}
	else
	{
		unit->shieldPoints += shield_gain;
	}
}

void orders_bldgUnderConstruction_Protoss__()
{
	CUnit* unit;

	__asm mov unit, eax

	orders_bldgUnderConstruction_Protoss_(unit);
}

FUNCTION_PATCH((void*)0x4E4F40, orders_bldgUnderConstruction_Protoss__);

void sub_497A10_(CSprite* sprite, __int16 x, __int16 y)
{
	if (sprite->position.x != x || sprite->position.y != y)
	{
		int v4 = std::clamp(sprite->position.y / 32, 0, map_size.height - 1);
		int v5 = std::clamp(y / 32, 0, map_size.height - 1);
		sprite->position.x = x;
		sprite->position.y = y;

		if (v4 != v5)
		{
			if (_SpritesOnTileRow.heads[v4] == sprite)
			{
				_SpritesOnTileRow.heads[v4] = sprite->next;
			}
			if (_SpritesOnTileRow.tails[v4] == sprite)
			{
				_SpritesOnTileRow.tails[v4] = sprite->prev;
			}
			if (sprite->prev)
			{
				sprite->prev->next = sprite->next;
			}
			CSprite* v6 = sprite->next;
			if (v6)
			{
				v6->prev = sprite->prev;
			}
			sprite->prev = 0;
			sprite->next = 0;
			if (CSprite* v7 = _SpritesOnTileRow.heads[v5])
			{
				if (_SpritesOnTileRow.tails[v5] == v7)
				{
					_SpritesOnTileRow.tails[v5] = sprite;
				}
				sprite->prev = v7;
				sprite->next = v7->next;
				CSprite* v8 = v7->next;
				if (v8)
				{
					v8->prev = sprite;
				}
				v7->next = sprite;
			}
			else
			{
				_SpritesOnTileRow.tails[v5] = sprite;
				_SpritesOnTileRow.heads[v5] = sprite;
			}
		}
		for (CImage* image = sprite->pImageHead; image; image = image->next)
		{
			image->flags |= ImageFlags::IF_REDRAW;
		}
	}
}

void __cdecl sub_497A10__()
{
	CSprite* sprite;
	__int16 x;
	__int16 y;

	__asm {
		mov sprite, ecx
		mov x, bx
		mov y, di
	}

	sub_497A10_(sprite, x, y);
}

FUNCTION_PATCH((void*)0x497A10, sub_497A10__);

void GroundAttackInit_(__int16 x, __int16 y)
{
	wantThingyUpdate = 0;
	sub_497A10_(ThingyList_UsedFirst->sprite, x + 1, y + 1);
	for (CImage* i = ThingyList_UsedFirst->sprite->pImageHead; i; i = i->next)
	{
		if ((i->flags & ImageFlags::IF_HAS_ISCRIPT_ANIMATIONS) != 0)
		{
			init_iscript_program_state(&i->iscript_program, Anims::AE_GndAttkInit);
			BWFXN_PlayIscript_(i, &i->iscript_program, 0, 0);
		}
	}
	wantThingyUpdate = 1;
}

FAIL_STUB_PATCH(GroundAttackInit);

CThingy* sub_456490_(__int16 x, __int16 y, CUnit* unit)
{
	if (CThingy* result = sub_487A10(x, y))
	{
		result->sprite->selectionTimer = 31;
		return result;
	}
	else if (unit)
	{
		unit->sprite->selectionTimer = 31;
		return nullptr;
	}
	else
	{
		GroundAttackInit_(x, y);
		return nullptr;
	}
}

FAIL_STUB_PATCH(sub_456490);

void __fastcall input_targetOrder_LeftMouseClick_(dlgEvent* event)
{
	if (!IsOutsideGameScreen_((__int16)event->cursor.x, (__int16)event->cursor.y))
	{
		int x = MoveToX + (__int16)event->cursor.x;
		int y = MoveToY + (__int16)event->cursor.y;
		CUnit* v5 = FindUnitAtPoint(x, y);
		CThingy* v6 = sub_456490_(x, y, v5);
		sub_46F5B0(x, y, v5, v6 ? LOWORD(v6->hitPoints) : 228);
		resetGameInputProcs(CursorType::CUR_ARROW);
	}
}

FAIL_STUB_PATCH(input_targetOrder_LeftMouseClick);

void __fastcall input_targetOrder_RightMouseClick_(dlgEvent* event)
{
	if (is_placing_order)
	{
		CancelTargetOrder();
	}
}

FAIL_STUB_PATCH(input_targetOrder_RightMouseClick);

void setCursorType_(CursorType cursor_type);

CursorType getCursorType_()
{
	if (PtInRect(&stru_512D00, Mouse))
	{
		return word_6556FC;
	}
	if (IsOutsideGameScreen_(Mouse.x, Mouse.y) || !PtInRect(&game_screen_pos, Mouse))
	{
		return CursorType::CUR_ARROW;
	}
	if (byte_66FF5C)
	{
		return CursorType::CUR_DRAG;
	}
	if (is_placing_building)
	{
		return CursorType::CUR_ARROW;
	}

	CUnit* v0 = FindUnitAtPoint(Mouse.x + MoveToX, MoveToY + Mouse.y);
	if (!v0)
	{
		return is_placing_order ? CursorType::CUR_TARG_N : CursorType::CUR_ARROW;
	}
	else if (v0->playerID == 11)
	{
		return is_placing_order ? CursorType::CUR_TARG_Y : CursorType::CUR_MAG_Y;
	}
	else if (UnitIsEnemy(v0))
	{
		return is_placing_order ? CursorType::CUR_TARG_R : CursorType::CUR_MAG_R;
	}
	else
	{
		return is_placing_order ? CursorType::CUR_TARG_G : CursorType::CUR_MAG_G;
	}
}

FAIL_STUB_PATCH(getCursorType);

void sub_468670_(CUnit* unit)
{
	const points& rally_point = unit->fields3.rally.position;
	if ((rally_point.x || rally_point.y) && (rally_point.x != unit->position.x || rally_point.y != unit->position.y))
	{
		if (CThingy* v4 = sub_487A10(rally_point.x, rally_point.y))
		{
			v4->sprite->selectionTimer = 31;
		}
		else
		{
			GroundAttackInit_(rally_point.x, rally_point.y);
		}
	}
}

void __cdecl sub_468670__()
{
	CUnit* unit;

	__asm mov unit, eax

	sub_468670_(unit);
}

FUNCTION_PATCH((void*)0x468670, sub_468670__);

void getSelectedUnitsInBox_(Box16* box)
{
	CUnit* a2[12] = { 0 };
	memset(a2, 0, sizeof(a2));
	int v3 = SortAllUnits(FindAllUnits(box), a2, 0);
	tempUnitsListCurrentArrayCount = tempUnitsListArraysCountsListLastIndex[tempUnitsListArraysCountsListLastIndex[0]--];
	if (v3)
	{
		if (is_keycode_used[VK_SHIFT] && PlayerSelection[0])
		{
			CUnit* v8[12];
			memcpy(v8, PlayerSelection, sizeof(v8));
			UI_doSelectUnits_IfAltNotHeld(v8, sub_46F290(v8, a2, v3, 0), 1, 1);
			return;
		}
		if (v3 != 1 || !is_keycode_used[18] || !selectSingleUnitFromID(CUnitToUnitID(a2[0])))
		{
			selectMultipleUnitsFromUnitList(v3, a2, 1, 1);
			if (v3 != 1)
			{
				return;
			}
		}
		if (a2[0] && a2[0]->playerID == g_ActiveNationID)
		{
			if (Unit_IsFactoryBuilding(a2[0]))
			{
				sub_468670_(a2[0]);
			}
		}
	}
}

FAIL_STUB_PATCH(getSelectedUnitsInBox);

void getSelectedUnitsAtPoint_(int y, int x)
{
	if (CUnit* unit = FindUnitAtPoint(x, y))
	{
		CUnit* unitsToSelect[12];
		memset(unitsToSelect, 0, sizeof(unitsToSelect));

		if ((unit->sprite->flags & 8) && dword_66FF58 || is_keycode_used[VK_CONTROL])
		{
			rect v26;
			v26.left = MoveToX;
			v26.right = MoveToX + GAME_AREA_WIDTH;
			v26.top = MoveToY;
			v26.bottom = MoveToY + GAME_AREA_HEIGHT;

			int v11 = SortAllUnits(getAllUnitsInBounds(&v26), unitsToSelect, unit);

			if (is_keycode_used[VK_SHIFT])
			{
				CUnit* a2a[12];
				memcpy(a2a, PlayerSelection, sizeof(a2a));

				tempUnitsListCurrentArrayCount = tempUnitsListArraysCountsListLastIndex[tempUnitsListArraysCountsListLastIndex[0]--];
				if (v11)
				{
					int v13 = sub_46F290(a2a, unitsToSelect, v11, unit);
					UI_doSelectUnits_IfAltNotHeld(a2a, v13, 1, 1);
				}
			}
			else
			{
				tempUnitsListCurrentArrayCount = tempUnitsListArraysCountsList[--tempUnitsListArraysCountsListLastIndex[0]];
				if (v11)
				{
					UI_doSelectUnits_IfAltNotHeld(unitsToSelect, v11, 1, 1);
					if (v11 == 1 && unit->playerID == g_ActiveNationID && Unit_IsFactoryBuilding(unit))
					{
						sub_468670_(unit);
					}
				}
			}
		}
		else if (is_keycode_used[VK_SHIFT] && PlayerSelection[0])
		{
			CUnit** v14 = PlayerSelection;
			int i;
			for (i = 0; i < 12; unitsToSelect[i++] = *v14++)
			{
				if (!*v14)
				{
					break;
				}
			}

			if (unit->sprite->flags & 8)
			{
				int v18 = i - 1;
				u8 selection_index = unit->sprite->selectionIndex;
				memcpy(&unitsToSelect[selection_index], &unitsToSelect[selection_index + 1], 4 * (v18 - selection_index));
				unitsToSelect[v18] = 0;
				if (v18 != 1 || !is_keycode_used[VK_MENU] || !selectSingleUnitFromID(CUnitToUnitID(unitsToSelect[0])))
				{
					CreateNewUnitSelectionsFromList(unitsToSelect, v18);
					selectUnits(v18, unitsToSelect);
					ctrl_under_mouse = 0;
					ctrl_under_mouse_val = 0;
					byte_59723C = 1;
					CanUpdateCurrentButtonSet = 1;
					CanUpdateSelectedUnitPortrait = 1;
					CanUpdateStatDataDialog = 1;
				}
			}
			else if (i < 12)
			{
				CUnit* v17 = unitsToSelect[0];
				if (unit_IsStandardAndMovable(unitsToSelect[0]))
				{
					if (unitIsOwnerByCurrentPlayer(v17) && unit_IsStandardAndMovable(unit) && unitIsOwnerByCurrentPlayer(unit))
					{
						unitsToSelect[i] = unit;
						UI_doSelectUnits_IfAltNotHeld(unitsToSelect, i + 1, 1, 1);
					}
				}
			}
		}
		else
		{
			if (!is_keycode_used[VK_MENU] || !selectSingleUnitFromID(CUnitToUnitID(unit)))
			{
				CUnit* a1a = unit;
				CreateNewUnitSelectionsFromList(&a1a, 1);
				selectBuildingSFX(compareUnitRank(a1a, 0) ? a1a : nullptr);
				selectUnits(1, &a1a);
				unit = a1a;
				byte_59723C = 1;
				CanUpdateCurrentButtonSet = 1;
				CanUpdateSelectedUnitPortrait = 1;
				CanUpdateStatDataDialog = 1;
				ctrl_under_mouse = 0;
				ctrl_under_mouse_val = 0;
			}
			if (unit->playerID == g_ActiveNationID && Unit_IsFactoryBuilding(unit))
			{
				sub_468670(unit);
			}
		}
	}
}

FAIL_STUB_PATCH(getSelectedUnitsAtPoint);

void __fastcall input_dragSelect_MouseBtnUp_(dlgEvent* a1)
{
	input_dragSelect_MouseMove(a1);
	SetInGameInputProcs();
	setCursorType_(getCursorType_());
	stru_66FF50.left += MoveToX;
	stru_66FF50.right += MoveToX;
	__int16 v3 = MoveToY + stru_66FF50.bottom;
	stru_66FF50.top += MoveToY;
	byte_66FF5C = 0;
	stru_66FF50.bottom += MoveToY;
	if ((__int16)stru_66FF50.top < (int)(unsigned __int16)map_height_pixels)
	{
		if (v3 >= (int)(unsigned __int16)map_height_pixels)
		{
			v3 = map_height_pixels - 1;
			stru_66FF50.bottom = map_height_pixels - 1;
		}
		if ((__int16)stru_66FF50.right - (__int16)stru_66FF50.left > 4 || v3 - (__int16)stru_66FF50.top > 4)
		{
			getSelectedUnitsInBox_(&stru_66FF50);
		}
		else
		{
			getSelectedUnitsAtPoint_((__int16)stru_66FF50.top, (__int16)stru_66FF50.left);
		}
	}
}

FAIL_STUB_PATCH(input_dragSelect_MouseBtnUp);

void __fastcall input_Game_LeftMouseClick_(dlgEvent* event)
{
	if (!IsOutsideGameScreen_((__int16)event->cursor.x, (__int16)event->cursor.y))
	{
		if (is_placing_order)
		{
			CancelTargetOrder();
		}
		if (is_placing_building)
		{
			refreshLayer3And4();
			refreshPlaceBuildingLocation();
		}
		ClipCursor(&game_screen_pos);
		input_procedures[EventNo::EVN_LBUTTONUP] = input_dragSelect_MouseBtnUp_;
		input_procedures[EventNo::EVN_RBUTTONUP] = input_dragSelect_MouseBtnUp_;
		input_procedures[EventNo::EVN_MOUSEMOVE] = input_dragSelect_MouseMove;
		dword_6D1208 = refreshDragSelectBox;
		setCursorType_(CursorType::CUR_DRAG);
		stru_66FF4C = event->cursor;

		if (event->wNo != EventNo::EVN_MOUSEMOVE && event->wNo != EventNo::EVN_LBUTTONUP)
		{
			dword_66FF58 = (event->wNo == EventNo::EVN_LBUTTONDBLCLK);
		}
	}
}

FAIL_STUB_PATCH(input_Game_LeftMouseClick);

BYTE sub_4BFA40_()
{
	return dword_59724C[0] ? dword_59724C[0]->playerID : -1;
}

FAIL_STUB_PATCH(sub_4BFA40);

void __fastcall input_Game_RightMouseClick_(dlgEvent* event)
{
	if (!IsOutsideGameScreen_((__int16)event->cursor.x, (__int16)event->cursor.y))
	{
		if (sub_4BFA40_() == g_LocalNationID && ActivePortraitUnit && unitIsOwnerByCurrentPlayer(ActivePortraitUnit))
		{
			for (int v4 = 0; v4 < 12; v4++)
			{
				if (CUnit* v5 = ClientSelectionGroup[v4])
				{
					if (DoesAcceptRclickCommands(v5))
					{
						break;
					}
				}
			}
			int v6 = MoveToX + (__int16)event->cursor.x;
			int v7 = MoveToY + (__int16)event->cursor.y;
			CUnit* v8 = FindUnitAtPoint(v6, v7);
			if (ClientSelectionCount != 1 || ActivePortraitUnit != v8 || v8 && (v8->statusFlags & GoundedBuilding) != 0 && Unit_IsFactoryBuilding(v8))
			{
				if (!v8)
				{
					for (int v10 = 0; v10 < 12; v10++)
					{
						if (CUnit* v11 = ClientSelectionGroup[v10])
						{
							if (CanRClickGround_maybe(v11))
							{
								break;
							}
						}
					}
				}
				CThingy* v12 = sub_456490_(v6, v7, v8);
				CMDACT_RightClickOrder(v8, v6, v7, v12 ? v12->hitPoints : 228, is_keycode_used[VK_SHIFT]);
				if (ShowRClickErrorIfNeeded(v8))
				{
					PlayWorkerActionSound(ActivePortraitUnit);
				}
			}
		}
	}
}

FAIL_STUB_PATCH(input_Game_RightMouseClick);

void SetInGameInputProcs_()
{
	InitializeInputProcs_();
	hAccTable = hAccel;

	input_procedures[EventNo::EVN_KEYFIRST] = nullsub_3;
	input_procedures[EventNo::EVN_MOUSEMOVE] = nullptr;
	input_procedures[EventNo::EVN_LBUTTONDOWN] = input_Game_LeftMouseBtnDwn;
	input_procedures[EventNo::EVN_LBUTTONDBLCLK] = input_Game_LeftMouseBtnDwn;
	input_procedures[EventNo::EVN_RBUTTONDOWN] = input_Game_RightMouseBtnDwn;
	input_procedures[EventNo::EVN_RBUTTONDBLCLK] = input_Game_RightMouseBtnDwn;
	input_procedures[EventNo::EVN_MBUTTONDOWN] = input_Game_MiddleMouseBtnDwn;
	input_procedures[EventNo::EVN_MBUTTONUP] = input_Game_MiddleMouseBtnUp;
	input_procedures[EventNo::EVN_IDLE] = input_Game_Idle;
	input_procedures[EventNo::EVN_CHAR] = input_Game_UserKeyPress_;
	input_procedures[EventNo::EVN_SYSCHAR] = CMDACT_Hotkey_;
	input_procedures[EventNo::EVN_WHEELUP] = nullptr;
	input_procedures[EventNo::EVN_WHEELDWN] = nullptr;

	if (is_placing_building)
	{
		GameScreenLClickEvent = input_placeBuilding_LeftMouseClick_;
		GameScreenRClickEvent = input_placeBuilding_RightMouseClick_;
	}
	else if (is_placing_order)
	{
		GameScreenLClickEvent = input_targetOrder_LeftMouseClick_;
		GameScreenRClickEvent = input_targetOrder_RightMouseClick_;
	}
	else
	{
		GameScreenLClickEvent = input_Game_LeftMouseClick_;
		GameScreenRClickEvent = input_Game_RightMouseClick_;
	}
}

FUNCTION_PATCH(SetInGameInputProcs, SetInGameInputProcs_);

dialog* getControlFromIndex_(dialog* dlg, __int16 index)
{
	dialog* v1 = dlg->wCtrlType ? dlg->fields.ctrl.pDlg : dlg;
	dialog* v2 = v1->fields.dlg.pFirstChild;
	while (v2 && v2->wIndex != index)
	{
		v2 = v2->pNext;
	}
	return v2;
}

dialog* __cdecl getControlFromIndex__()
{
	dialog* dlg;
	__int16 index;

	__asm mov dlg, eax
	__asm mov index, cx

	return getControlFromIndex_(dlg, index);
}

FUNCTION_PATCH((void*)0x418080, getControlFromIndex__);

int initSpriteData_(unsigned __int16 x, unsigned __int16 y, int sprite_id, char player_id, CSprite* sprite)
{
	if (x >= (unsigned __int16)map_width_pixels || y >= (unsigned __int16)map_height_pixels)
	{
		return 0;
	}
	sprite->playerID = player_id;
	sprite->spriteID = sprite_id;
	sprite->flags = 0;
	sprite->position.x = x;
	sprite->position.y = y;
	sprite->visibilityFlags = -1;
	sprite->elevationLevel = 4;
	sprite->selectionTimer = 0;
	if (!Sprites_IsVisible[sprite_id])
	{
		sprite->flags = 32;
		refreshAllVisibleImagesAtScreenPosition(sprite, 0);
	}
	if (!CreateImageOverlay(sprite, Sprites_Image[sprite_id], 0, 0, 0))
	{
		return 0;
	}
	CImage* image = sprite->pImagePrimary;

	sprite->unkflags_12 = image->GRPFile->width <= 0xFFu ? image->GRPFile->width : -1;
	sprite->unkflags_13 = (unsigned __int16)image->GRPFile->height <= 0xFFu ? image->GRPFile->height : -1;

	return 1;
}

FAIL_STUB_PATCH(initSpriteData);

void SpriteDestructor_(CSprite* sprite)
{
	CImage* v2;
	for (CImage* v1 = sprite->pImageHead; v1; v1 = v2)
	{
		v2 = v1->next;
		ImageDestructor(v1);
	}

	int v3 = (__int16)sprite->position.y / 32;
	if (v3 < 0)
	{
		v3 = 0;
	}
	else if (v3 >= map_size.height)
	{
		v3 = map_size.height - 1;
	}

	if (_SpritesOnTileRow.heads[v3] == sprite)
	{
		_SpritesOnTileRow.heads[v3] = sprite->next;
	}
	if (_SpritesOnTileRow.tails[v3] == sprite)
	{
		_SpritesOnTileRow.tails[v3] = sprite->prev;
	}
	if (sprite->prev)
	{
		sprite->prev->next = sprite->next;
	}
	if (sprite->next)
	{
		sprite->next->prev = sprite->prev;
	}
	sprite->prev = 0;
	sprite->next = 0;
	if (UnusedSprites)
	{
		if (dword_63FE34 == UnusedSprites)
		{
			dword_63FE34 = sprite;
		}
		sprite->prev = UnusedSprites;
		sprite->next = UnusedSprites->next;
		if (UnusedSprites->next)
		{
			UnusedSprites->next->prev = sprite;
		}
		UnusedSprites->next = sprite;
	}
	else
	{
		dword_63FE34 = sprite;
		UnusedSprites = sprite;
	}
}

void __cdecl SpriteDestructor__()
{
	CSprite* sprite;

	__asm mov sprite, edi

	SpriteDestructor_(sprite);
}

FUNCTION_PATCH((void*)0x497B40, SpriteDestructor__);

CSprite* createSprite_(int sprite_id, int position_x, unsigned __int16 position_y, char player_id)
{
	CSprite* v4 = UnusedSprites;
	if (!UnusedSprites)
	{
		return 0;
	}
	CSprite** v6 = &UnusedSprites->next;
	UnusedSprites = UnusedSprites->next;
	if (dword_63FE34 == v4)
	{
		dword_63FE34 = v4->prev;
	}
	if (v4->prev)
	{
		v4->prev->next = *v6;
	}
	if (*v6)
	{
		(*v6)->prev = v4->prev;
	}
	v4->prev = 0;
	*v6 = 0;
	if (initSpriteData_(position_x, position_y, sprite_id, player_id, v4))
	{
		int v10 = std::clamp(v4->position.y / 32, 0, map_size.height - 1);

		CSprite* v11 = _SpritesOnTileRow.heads[v10];
		if (v11)
		{
			if (_SpritesOnTileRow.tails[v10] == v11)
			{
				_SpritesOnTileRow.tails[v10] = v4;
			}
			v4->prev = v11;
			*v6 = v11->next;
			CSprite* v12 = v11->next;
			if (v12)
			{
				v12->prev = v4;
			}
			v11->next = v4;
		}
		else
		{
			_SpritesOnTileRow.tails[v10] = v4;
			_SpritesOnTileRow.heads[v10] = v4;
		}
		return v4;
	}
	else
	{
		CSprite* v7 = UnusedSprites;
		if (UnusedSprites)
		{
			if (dword_63FE34 == UnusedSprites)
			{
				dword_63FE34 = v4;
			}
			v4->prev = UnusedSprites;
			*v6 = v7->next;
			CSprite* v8 = v7->next;
			if (v8)
			{
				v8->prev = v4;
			}
			v7->next = v4;
		}
		else
		{
			dword_63FE34 = v4;
			UnusedSprites = v4;
		}
		return NULL;
	}
}

CSprite* __stdcall createSprite__(int sprite_id, int position_x, char player_id)
{
	unsigned __int16 position_y;

	__asm mov position_y, di

	return createSprite_(sprite_id, position_x, position_y, player_id);
}

FUNCTION_PATCH((void*)0x4990F0, createSprite__);

void SysWarn_FileNotFound_(const char* a1, int last_error)
{
	char dwInitParam[512];
	char Buffer[256];

	char* v2 = GetErrorString(Buffer, 0x100u, last_error);
	sprintf_s(dwInitParam, "%s\n%s", a1, v2);
	sub_421510_();
	if (DialogBoxParamA(local_dll_library, (LPCSTR)0x6A, hWndParent, DialogFunc, (LPARAM)dwInitParam) == -1)
	{
		FatalError_("GdsDialogBoxParam: %d", 106);
	}
	DLGErrFatal_();
}

void __stdcall SysWarn_FileNotFound__(const char* a1)
{
	int last_error;

	__asm mov last_error, ebx

	throw FileNotFoundException(a1, last_error);
}

FUNCTION_PATCH((void*)0x4212C0, SysWarn_FileNotFound__);

void FileFatal_(HANDLE a1, int a2)
{
	char buffer[260];

	if (!SFileGetFileName(a1, buffer, 260))
	{
		strcpy_s(buffer, "*unknown*");
	}
	throw FileNotFoundException(buffer, a2);
}

void FileFatal__()
{
	HANDLE a1;
	int a2;

	__asm mov a1, ecx
	__asm mov a2, ebx

	FileFatal_(a1, a2);
}

FUNCTION_PATCH((void*)0x4D2880, FileFatal__);

const int app_exit_handles_count = 32;

void AppExit_(bool exit_code)
{
	if (app_exit_handles && !byte_6D63F0)
	{
		for (byte_6D63F0 = 1; word_519F64 != -1; --word_519F64)
		{
			AppExitHandle exit_handle = app_exit_handles[word_519F64];
			if (exit_handle)
			{
				app_exit_handles[word_519F64] = NULL;
				exit_handle(exit_code);
			}
		}
		SMemFree(app_exit_handles, "Starcraft\\SWAR\\lang\\gds\\appexi.cpp", 50, 0);
		app_exit_handles = NULL;
	}
}

FAIL_STUB_PATCH(AppExit);

int AppAddExit_(AppExitHandle handle)
{
	if (!app_exit_handles)
	{
		app_exit_handles = (AppExitHandle *)SMemAlloc(app_exit_handles_count * sizeof(AppExitHandle), "Starcraft\\SWAR\\lang\\gds\\appaddex.cpp", 42, (int)app_exit_handles);
		memset(app_exit_handles, 0, app_exit_handles_count * sizeof(AppExitHandle));
	}
	int v2 = -1;
	for (int i = 0; i < app_exit_handles_count; i++)
	{
		AppExitHandle exit_handle = app_exit_handles[i];
		if (exit_handle == handle)
			return 0;
		if (v2 == -1 && !exit_handle)
			v2 = i;
	}

	if (v2 == -1)
	{
		FatalError_("APPADDEX:1");
	}
	app_exit_handles[v2] = handle;
	return 1;
}

FAIL_STUB_PATCH(AppAddExit);

void __fastcall sub_4C9120_(bool exit_code)
{
	if (dword_596CD0)
	{
		SMemFree(dword_596CD0, "Starcraft\\SWAR\\lang\\opt_dlgs.cpp", 58, 0);
		dword_596CD0 = 0;
		byte_596BC8[0] = 0;
	}
}

FAIL_STUB_PATCH(sub_4C9120);

void sub_4C9C40_(const char* a1)
{
	if (dword_596CD0)
	{
		if (!_stricmp(byte_596BC8, a1))
		{
			return;
		}
		sub_4C9120_(0);
	}
	else
	{
		AppAddExit_(sub_4C9120_);
	}
	dword_596CD0 = LoadGraphic(a1, 0, "Starcraft\\SWAR\\lang\\opt_dlgs.cpp", 80);
	strcpy(byte_596BC8, a1);
}

void sub_4C9C40__()
{
	char* a1;

	__asm mov a1, esi

	sub_4C9C40_(a1);
}

FUNCTION_PATCH((void*)0x4C9C40, sub_4C9C40__);

bool hasMessagesWaiting_(MSG* a1, int a2)
{
	if (dword_6D0530)
	{
		dword_6D0530 = 0;
		return false;
	}

	if (!a2 || is_app_active)
	{
		return PeekMessageA(a1, 0, 0, 0, 1u);
	}

	if (multiPlayerMode || gwGameMode == GamePosition::GAME_EXIT)
	{
		if (PeekMessageA(a1, 0, 0, 0, 1u))
		{
			return true;
		}
		else
		{
			Sleep(0);
			return false;
		}
	}

	return GetMessageA(a1, 0, 0, 0) != -1;
}

FAIL_STUB_PATCH(hasMessagesWaiting);

bool sendInputToAllDialogs_(dlgEvent* evt)
{
	sub_419F80();
	dialog* event_dialog = EventDialogs[evt->wNo];

	if (event_dialog)
	{
		bool result = event_dialog->pfcnInteract(event_dialog, evt);
		if (result)
		{
			pressGlobalDlgHotkey();
			return result;
		}
	}

	for (dialog* dlg = DialogList; dlg; dlg = dlg->pNext)
	{
		int result = dlg->pfcnInteract(dlg, evt);
		if (result)
		{
			pressGlobalDlgHotkey();
			return result;
		}
	}

	pressGlobalDlgHotkey();
	return 0;
}

FAIL_STUB_PATCH(sendInputToAllDialogs);

void RefreshCursorScreen_()
{
	if (ScreenLayers[0].buffers)
	{
		ScreenLayers[0].bits |= 1;
		BWFXN_RefreshTarget(
			(__int16)ScreenLayers[0].left,
			(__int16)ScreenLayers[0].height + (__int16)ScreenLayers[0].top - 1,
			(__int16)ScreenLayers[0].top,
			(__int16)ScreenLayers[0].width + (__int16)ScreenLayers[0].left - 1);
	}
}

FAIL_STUB_PATCH(RefreshCursorScreen);

void __cdecl drawCursor_()
{
	if (last_cursor)
	{
		RefreshCursorScreen_();
		RefreshCursorRect();
		grpFrame* cursor_frame = &last_cursor->frames[dword_597390 % last_cursor->wFrames];
		ScreenLayers[0].width = cursor_frame->wid;
		ScreenLayers[0].height = cursor_frame->hgt;
		ScreenLayers[0].left = cursor_frame->x + Mouse.x - 63;
		ScreenLayers[0].top = cursor_frame->y + Mouse.y - 63;
		RefreshCursorScreen_();
		RefreshCursorRect();
	}
}

FUNCTION_PATCH(drawCursor, drawCursor_);

void UpdateDlgMousePosition_(void)
{
	drawCursor_();
	LOBYTE(InputFlags) = InputFlags & 0xFE;

	dlgEvent v0;
	v0.wNo = EventNo::EVN_MOUSEMOVE;
	v0.cursor.x = Mouse.x;
	v0.cursor.y = Mouse.y;
	if (!sendInputToAllDialogs_(&v0) && input_procedures[EventNo::EVN_MOUSEMOVE])
	{
		input_procedures[EventNo::EVN_MOUSEMOVE](&v0);
	}
	dword_5968EC = 1;
}

FAIL_STUB_PATCH(UpdateDlgMousePosition);

void __fastcall DestroyGluAllStrings_(bool exit_code)
{
	if (gluAllTblDataLoaded && gluAllTblData)
	{
		SMemFree(gluAllTblData, "Starcraft\\SWAR\\lang\\glues.cpp", 688, 0);
	}
}

FAIL_STUB_PATCH(DestroyGluAllStrings);


const char* GetTblString(WORD* table, __int16 tbl_entry)
{
	if (tbl_entry == 0)
	{
		return NULL;
	}
	else if (tbl_entry < *table + 1)
	{
		return (const char*)table + table[tbl_entry];
	}
	else
	{
		return "";
	}
}

const char* GetNetworkTblString_(__int16 network_tbl_entry)
{
	return GetTblString(networkTable, network_tbl_entry);
}

FAIL_STUB_PATCH(GetNetworkTblString);

const char* GetMapTblString(__int16 map_tbl_entry)
{
	if (map_tbl_entry == 0)
	{
		return "";
	}
	else if (MapStringTbl.buffer == 0)
	{
		return NULL;
	}
	else if (map_tbl_entry < *MapStringTbl.buffer + 1)
	{
		return (char*)MapStringTbl.buffer + MapStringTbl.buffer[map_tbl_entry];
	}
	else
	{
		return "";
	}
}

char* __stdcall get_GluAll_String_(GluAllTblEntry tbl_entry)
{
	if (!gluAllTblDataLoaded)
	{
		gluAllTblData = fastFileRead_(0, 0, "rez\\gluAll.tbl", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
		AppAddExit_(DestroyGluAllStrings_);
		gluAllTblDataLoaded = 1;
	}

	if (tbl_entry == STAR_EDIT_NOT_FOUND)
	{
		return 0;
	}

	return tbl_entry - 1 < *(_WORD*)gluAllTblData ? (char*)gluAllTblData + *((unsigned __int16*)gluAllTblData + tbl_entry) : "";
}

FUNCTION_PATCH(get_GluAll_String, get_GluAll_String_);

const MusicTrackDescription* current_music_track = nullptr;

MusicTrackDescription title_music = { "music\\title.wav", MusicTrackType::MENU_MUSIC, 0, 0 };
MusicTrackDescription radio_free_zerg = { "music\\RadioFreeZerg.wav", MusicTrackType::IN_GAME_MUSIC, 0, 1};

void PlayMusic_(const MusicTrackDescription* a1)
{
	if (directsound == NULL || a1 != current_music_track)
	{
		current_music_track = a1;
		if (directsound)
		{
			SFileDdaEnd(directsound);
			SFileCloseFile(directsound);
			directsound = 0;
			byte_6D5BBC = 0;
		}
		if (current_music_track == nullptr)
		{
			return;
		}

		SFileOpenFile(a1->wav_filename, &directsound);
	}

	if (directsound)
	{
		if (getArchiveName(directsound))
		{
			if (gwGameMode == GAME_RUN)
			{
				BWFXN_PrintText(8, (char*) GetNetworkTblString_(140), 0);
			}
		}
		else
		{
			byte_6D5BBD = 0;
			if (registry_options.Music)
			{
				if (SFileDdaBeginEx(directsound, 0x40000, a1->track_type != MusicTrackType::MENU_MUSIC ? 0 : 0x40000, 0, getMusicVolume(), 0, 0))
				{
					byte_6D5BBC = 1;
				}
				else
				{
					if (directsound)
					{
						SFileDdaEnd(directsound);
						SFileCloseFile(directsound);
						directsound = 0;
					}
					byte_6D5BBC = 0;
				}
			}
		}
	}
}

FAIL_STUB_PATCH(PlayMusic);

void playNextMusic_()
{
	if (byte_6D5BBC && current_music_track->track_type == MusicTrackType::IN_GAME_MUSIC)
	{
		unsigned a2;
		unsigned a3;
		SFileDdaGetPos(directsound, (int)&a2, (int)&a3);
		if (a2 >= a3)
		{
			auto& race_ingame_music = Race::races()[consoleIndex].ingame_music;
			PlayMusic_(&race_ingame_music[current_music_track->next_music_track]);
		}
	}
}

FAIL_STUB_PATCH(playNextMusic);

void __stdcall BWFXN_videoLoop_(int flag)
{
	checkLastFileError();
	if (flag)
	{
		MSG Msg;
		while (hasMessagesWaiting_(&Msg, flag & 2))
		{
			if (!hAccTable || !hWndParent || !TranslateAcceleratorA(hWndParent, hAccTable, &Msg))
			{
				DispatchMessageA(&Msg);
				if (!dword_6D0530)
				{
					TranslateMessage(&Msg);
				}
			}
		}
		if (CpuThrottle && (flag & 2))
		{
			Sleep(1);
		}
		if (InputFlags & 1)
		{
			UpdateDlgMousePosition_();
		}
		DWORD ticks = GetTickCount();
		if (ticks + message_handling_tick >= 100)
		{
			dlgEvent v3;
			v3.wNo = EventNo::EVN_IDLE;
			v3.cursor.x = Mouse.x;
			v3.cursor.y = Mouse.y;
			if (!sendInputToAllDialogs_(&v3) && input_procedures[EventNo::EVN_IDLE])
			{
				input_procedures[EventNo::EVN_IDLE](&v3);
			}
			message_handling_tick = -ticks;
			if (ticks + dword_6D637C >= 1000)
			{
				PlayBriefingWAVBegin();
				playNextMusic_();
				dword_6D637C = message_handling_tick;
			}
		}
		if (flag & 1)
		{
			iterateTimers();
		}
	}
	else
	{
		memset(is_keycode_used, 0, sizeof(is_keycode_used));
	}
}

FUNCTION_PATCH(BWFXN_videoLoop, BWFXN_videoLoop_);

void UpdateVisibilityHash_(int a1)
{
	memset(byte_63FD30, 0, sizeof(byte_63FD30));
	for (int i = 0; i < _countof(byte_63FD30); ++i)
	{
		for (CSprite* j = _SpritesOnTileRow.heads[i]; j; j = j->next)
		{
			if (byte_629A88[j->spriteID])
			{
				byte_63FD30[i] ^= j->visibilityFlags;
			}
		}
	}

	int v4 = min(a1 + 13, MAX_MAP_DIMENTION - 1);
	int v5 = 0;
	for (int k = a1; k <= v4; ++k)
	{
		v5 = (((unsigned __int8)byte_629C90[k] ^ v5) >> 31) | (2 * ((unsigned __int8)byte_629C90[k] ^ v5));
	}
	byte_63FE38 = BYTE2(v5) ^ v5 ^ ((unsigned __int16)(HIWORD(v5) ^ v5) >> 8);
	dword_63FD28 = a1;
}

FAIL_STUB_PATCH(UpdateVisibilityHash);

void DoVisibilityUpdate_(int top_row, unsigned int bottom_row)
{
	memset(byte_629C90, 0, sizeof(byte_629C90));
	dword_6C4A2C = 0;

	u8 v3 = InReplay ? ReplayVision : playerVisions;
	for (unsigned i = top_row; i <= bottom_row; i++)
	{
		for (CSprite* sprite = _SpritesOnTileRow.heads[i]; sprite; sprite = sprite->next)
		{
			if ((v3 & sprite->visibilityFlags) || InReplay && replayShowEntireMap && sprite->playerID == 11)
			{
				sub_42D4C0(sprite);
			}
		}
	}

	if (dword_629D90)
	{
		dword_629D90 = 0;
		UpdateVisibilityHash_(top_row);
	}
}

FAIL_STUB_PATCH(DoVisibilityUpdate);

void sub_4BD3A0_()
{
	if (GameState)
	{
		dword_5993A4 = max(MoveToTile.y - 4, 0);
		dword_5993C0 = min(MoveToTile.y + 404, map_size.height - 1);
		DoVisibilityUpdate_(dword_5993A4, dword_5993C0);
	}
}

FAIL_STUB_PATCH(sub_4BD3A0);

void updateAllDlgs_()
{
	for (dialog* dlg = DialogList; dlg; dlg = dlg->pNext)
	{
		if ((dlg->lFlags & CTRL_UPDATE) == 0)
		{
			dlg->lFlags |= CTRL_UPDATE;
			updateDialog(dlg);
		}
	}
}

FAIL_STUB_PATCH(updateAllDlgs);

bool realizePalette_()
{
	if (!PrimarySurface || !is_app_active || !hWndParent || IsIconic(hWndParent))
	{
		return false;
	}
	if (dword_6D5E1C)
	{
		SDrawRealizePalette();
		memset(RefreshRegions, 1u, sizeof(RefreshRegions));
		updateAllDlgs_();
		dword_6D5E1C = 0;
	}
	return true;
}

FAIL_STUB_PATCH(realizePalette);

void DoBltUsingMask_()
{
	int Pitch;
	void* lpSurface;

	if (SDrawLockSurface(0, 0, &lpSurface, &Pitch, 0))
	{
		STransBltUsingMask((int)lpSurface, (int)GameScreenBuffer.data, Pitch, SCREEN_WIDTH, dword_6D5E18);
		SDrawUnlockSurface(0, lpSurface, 0, 0);
	}
}

FAIL_STUB_PATCH(DoBltUsingMask);

void sub_41E000_()
{
	if (handle && dword_6D5E18)
	{
		STransDelete(dword_6D5E18);
		STransIntersectDirtyArray(handle, (int)RefreshRegions, 3u, (int)&dword_6D5E18);
		DoBltUsingMask_();
		memset(RefreshRegions, 0, sizeof(RefreshRegions));
	}
}

FAIL_STUB_PATCH(sub_41E000);

void DirtyArrayHandling_()
{
	u8* v0 = GameScreenBuffer.data;
	if (!GameScreenBuffer.data)
	{
		return;
	}
	dword_6CF4A8 = &GameScreenBuffer;
	if (!byte_51A0E9)
	{
		for (layer* v2 = ScreenLayers + 7; v2 >= ScreenLayers; v2--)
		{
			if (v2->buffers)
			{
				bounds b;
				b.left = -v2->left;
				b.top = -v2->top;
				b.right = -v2->left + SCREEN_WIDTH - 1;
				b.bottom = -v2->top + SCREEN_HEIGHT - 1;
				b.width = SCREEN_WIDTH;
				b.height = SCREEN_HEIGHT;
				if (v2 == ScreenLayers)
				{
					BlitCursorSurface(0, 0, &GameScreenBuffer, 0);
				}
				if (v2->bits & 0x21)
				{
					goto LABEL_12;
				}
				if (refreshRect(
					(__int16)v2->left,
					(__int16)v2->top,
					(__int16)v2->left + (__int16)v2->width,
					(__int16)v2->top + (__int16)v2->height))
				{
					v2->bits |= 4;
				LABEL_12:
					v2->pUpdate(0, 0, v2->pSurface, &b);
					v2->bits &= 0xF8u;
					continue;
				}
				if (v2->bits & 2)
				{
					goto LABEL_12;
				}
			}
		}
	}
	else
	{
		unsigned v1 = GameScreenBuffer.wid * GameScreenBuffer.ht;
		memset(GameScreenBuffer.data, 0, 4 * (v1 >> 2));
		memset(&v0[4 * (v1 >> 2)], 0, v1 & 3);

		RECT screen_rect;
		screen_rect.left = 0;
		screen_rect.top = 0;
		screen_rect.right = SCREEN_WIDTH;
		screen_rect.bottom = SCREEN_HEIGHT;
		BlitDirtyArray(&screen_rect);
	}

	sub_41E000_();

	if (!byte_51A0E9)
	{
		if (ScreenLayers[0].buffers)
		{
			BlitBitmap(&GameScreenBuffer);
		}
	}
	dword_6CF4A8 = 0;
}

FAIL_STUB_PATCH(DirtyArrayHandling);

void BWFXN_RedrawTarget_()
{
	if (has_viewport)
	{
		sub_4BD3A0_();
		if (realizePalette_())
		{
			if (ScreenLayers[1].buffers)
			{
				if (ScreenLayers[1].width)
				{
					RECT rc;
					SetRect(&rc, ScreenLayers[1].left, ScreenLayers[1].top, ScreenLayers[1].width + ScreenLayers[1].left - 1, ScreenLayers[1].height + ScreenLayers[1].top - 1);
					isRectBoundsInside_Assign_32(&rc, &ScrLimit);
					sub_41C2A0(&rc);
					sub_41C2A0(&stru_6D63F4);
					stru_6D63F4 = rc;
				}
				else
				{
					if (stru_6D63F4.left || stru_6D63F4.right)
					{
						sub_41C2A0(&stru_6D63F4);
					}
					if (stru_6D63F4.left || stru_6D63F4.right || stru_6D63F4.top || stru_6D63F4.bottom)
					{
						stru_6D63F4.top = stru_6D63F4.bottom + 1;
					}
				}
			}
			DirtyArrayHandling_();
		}

		if (dword_6D5E2C)
		{
			SRgnDelete(dword_6D5E2C);
			dword_6D5E2C = 0;
		}
	}
}

FUNCTION_PATCH(BWFXN_RedrawTarget, BWFXN_RedrawTarget_);

HANDLE LoadInstallArchiveHD_(const char* a1, char* a2, const char* mpq_filename, DWORD dwFlags)
{
	if (!GetModuleFileNameA(hInst, a2, 0x104u))
	{
		*a2 = 0;
	}
	char* v4 = strrchr(a2, '\\');
	if (v4)
	{
		*v4 = 0;
	}
	SStrNCat(a2, mpq_filename, 260);
	HANDLE hMpq;
	if (!SFileOpenArchive(a2, dwFlags, 2u, &hMpq))
	{
		return 0;
	}
	if (a1)
	{
		HANDLE phFile;
		if (!SFileOpenFileEx(hMpq, a1, 0, &phFile))
		{
			SFileCloseArchive(hMpq);
			return 0;
		}
		SFileCloseFile(phFile);
	}
	return hMpq;
}

FAIL_STUB_PATCH(LoadInstallArchiveHD);

signed int InitializeCDArchives_(const char *filename, int a2)
{
	if (cd_archive_mpq)
		return 1;

	char path_buffer[MAX_PATH];
	cd_archive_mpq = LoadInstallArchiveHD_(filename, path_buffer, "\\BroodWar.mpq", 1000);
	if (cd_archive_mpq || (cd_archive_mpq = LoadInstallArchiveHD_(filename, path_buffer, "\\StarCraft.mpq", 1000)) != 0)
		return 1;

	cd_archive_mpq = LoadInstallArchiveCD(1000u, "\\Install.exe", filename);
	if (cd_archive_mpq)
		return 1;

	while (a2)
	{
		INT_PTR v4 = DialogBoxParamA(local_dll_library, (LPCSTR)107, hWndParent, DialogFunc, 0);
		if (v4 == -1)
		{
			FatalError_("GdsDialogBoxParam: %d", 107);
		LABEL_13:
			AppExit_(0);
			ProcError(1);
			exit(0);
		}
		if (v4 != 1)
			goto LABEL_13;
		cd_archive_mpq = LoadInstallArchiveCD(1000u, "\\Install.exe", (char *)filename);
		if (cd_archive_mpq)
			return 1;
	}
	return 0;
}

int(*signal)(int a1, int a2) = (int(*)(int a1, int a2)) 0x0040C8D5;

void* fastFileRead_(int *bytes_read, int searchScope, const char *filename, int defaultValue, int bytes_to_read, const char *logfilename, int logline)
{
	HANDLE phFile;

	if (!SFileOpenFileEx(0, filename, searchScope, &phFile))
	{
		int last_error = SErrGetLastError();
		if (!bytes_to_read || last_error != 2 && last_error != 1006)
		{
			throw FileNotFoundException(filename, last_error);
		}
		if (bytes_read)
			*bytes_read = 0;
		return 0;
	}

	LONG file_size = SFileGetFileSize(phFile, 0);
	if (file_size == -1)
	{
		FileFatal(phFile, GetLastError());
	}
	if (bytes_read)
		*bytes_read = file_size;
	if (!file_size)
	{
		if (bytes_to_read)
		{
			SFileCloseFile(phFile);
			return 0;
		}
		throw FileNotFoundException(filename, 24);
	}
	void* buffer = (void*)defaultValue;
	if (!defaultValue)
		buffer = SMemAlloc(file_size, logfilename, logline, defaultValue);

	int read;
	if (!SFileReadFile(phFile, buffer, file_size, &read, 0))
	{
		DWORD last_error = GetLastError();
		FileFatal(phFile, last_error == 38 ? 24 : last_error);
	}
	if (read != file_size)
		FileFatal(phFile, 24);
	SFileCloseFile(phFile);
	return buffer;
}

dialog* LoadDialog(const char* bin_path)
{
	dialog* bin_dialog = (dialog*)fastFileRead_(0, 0, bin_path, 0, 0, __FILE__, __LINE__);
	if (bin_dialog)
	{
		bin_dialog->lFlags |= DialogFlags::CTRL_ACTIVE;
		AllocInitDialogData(bin_dialog, bin_dialog, AllocBackgroundImage, __FILE__, __LINE__);
	}

	return bin_dialog;
}

void InitializeFontKey_(void)
{
	char buff[MAX_PATH];
	sprintf_s(buff, "%s\\%s.gid", "font", "font");
	void* v0 = fastFileRead_(&cdkey_encrypted_len, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (v0 && cdkey_encrypted_len == 0)
	{
		SMemFree(v0, "Starcraft\\SWAR\\lang\\grid.cpp", 118, 0);
		v0 = NULL;
	}

	cdkey_encrypted = v0;
	sprintf_s(buff, "%s\\%s.clh", "font", "font");
	void* v1 = fastFileRead_(&cdkeyowner_encrypted_len, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (v1 == NULL)
	{
		v1 = NULL;
	}
	else if (!cdkeyowner_encrypted_len)
	{
		SMemFree(v1, "Starcraft\\SWAR\\lang\\grid.cpp", 118, 0);
		v1 = NULL;
	}
	cdkeyowner_encrypted = v1;
	sprintf_s(buff, "%s\\%s.ccd", "font", "font");
	is_spawn = KeyVerification(buff, "sgubon") == 0;
}

FAIL_STUB_PATCH(InitializeFontKey);

void LoadMainModuleStringInfo_()
{
	int result = GetModuleFileNameA(hInst, tstrFilename, MAX_PATH);
	if (result)
	{
		DWORD dwHandle;
		int v1 = GetFileVersionInfoSizeA(tstrFilename, &dwHandle);
		if (v1)
		{
			void* v2 = SMemAlloc(v1, "Starcraft\\SWAR\\lang\\init.cpp", 345, 0);
			if (GetFileVersionInfoA(tstrFilename, 0, v1, (LPVOID)v2))
			{
				VS_FIXEDFILEINFO* lpBuffer;
				unsigned int puLen;
				if (VerQueryValueA(v2, "\\", (LPVOID*)&lpBuffer, &puLen))
					sprintf_s(
						aInternalVersio,
						"Version %d.%d.%d",
						HIWORD(lpBuffer->dwProductVersionMS),
						LOWORD(lpBuffer->dwProductVersionMS),
						HIWORD(lpBuffer->dwProductVersionLS));
			}
			SMemFree(v2, "Starcraft\\SWAR\\lang\\init.cpp", 408, 0);
		}
	}
	else
	{
		tstrFilename[0] = 0;
	}
}

FAIL_STUB_PATCH(LoadMainModuleStringInfo);

void DetectExpansionInstallation_()
{
	HANDLE phFile;
	if (SFileOpenFileEx(0, "rez\\epilogX.txt", 0, &phFile))
	{
		SFileCloseFile(phFile);
		is_expansion_installed = 1;
	}
	else
	{
		is_expansion_installed = 0;
	}
}

FAIL_STUB_PATCH(DetectExpansionInstallation);

HANDLE magnetar_mpq;

int InitializeArchiveHandles_()
{
	snet_version_data.size = sizeof(snet_version_data);
	snet_version_data.versionstring = aInternalVersio;
	snet_version_data.executablefile = tstrFilename;
	snet_version_data.originalarchivefile = archive_files;
	snet_version_data.patcharchivefile = patch_archive_file;
	LoadMainModuleStringInfo_();

	CHAR starcraft_exe_filename[MAX_PATH];
	if (!GetModuleFileNameA(hInst, starcraft_exe_filename, MAX_PATH))
	{
		starcraft_exe_filename[0] = 0;
	}
	std::filesystem::path starcraft_exe_path = starcraft_exe_filename;
	std::filesystem::path starcraft_path = starcraft_exe_path.parent_path();
	std::filesystem::path stardat_path = starcraft_path / "Stardat.mpq";
	std::filesystem::path broodat_path = starcraft_path / "Broodat.mpq";
	std::filesystem::path patch_rt_path = starcraft_path / "patch_rt.mpq";

	if (!SFileOpenArchive(stardat_path.generic_string().c_str(), 2000, 2, &stardat_mpq))
	{
		throw FileNotFoundException("Stardat.mpq", GetLastError());
	}

	if (SFileOpenArchive(patch_rt_path.generic_string().c_str(), 7000, 2, &patch_rt_mpq))
	{
		strcpy_s(patch_archive_file, patch_rt_path.generic_string().c_str());
	}
	else
	{
		patch_rt_mpq = 0;
	}

	char magnetarDatFilename[MAX_PATH] = { 0 };
	if (!GetModuleFileNameA(NULL, magnetarDatFilename, MAX_PATH))
		*magnetarDatFilename = 0;
	auto separator = strrchr(magnetarDatFilename, '\\');
	if (separator)
		*separator = 0;
	strcat_s(magnetarDatFilename, "\\MagnetarDat.mpq");
	if (!SFileOpenArchive(magnetarDatFilename, 8000, 2, &magnetar_mpq))
	{
		throw FileNotFoundException("MagnetarDat.mpq", GetLastError());
	}

	InitializeFontKey_();
	AppAddExit_(DestroyFontKey);
	if (!is_spawn)
		InitializeCDArchives_(0, 1);

	archive_files[0] = 0;
	if (!is_spawn)
	{
		if (SFileOpenArchive(broodat_path.generic_string().c_str(), 2500, 2, &broodat_mpq))
		{
			strcpy_s(archive_files, broodat_path.generic_string().c_str());
			strcat_s(archive_files, ";");
		}
		else
		{
			broodat_mpq = 0;
		}
	}

	DetectExpansionInstallation_();
	return strcat_s(archive_files, stardat_path.generic_string().c_str());
}

FAIL_STUB_PATCH(InitializeArchiveHandles);

signed int __stdcall FileIOErrProc_(char *source, int a2, unsigned int a3)
{
	if (!a3)
		return 1;
	if (a3 <= 3)
	{
		Sleep(10);
		return 1;
	}
	if (!byte_596910[0])
	{
		EnterCriticalSection(&stru_6D5EDC);
		strcpy_s(byte_596910, source);
		LeaveCriticalSection(&stru_6D5EDC);
	}
	return 0;
}

void * loadTBL_(int a1, int a2, const char *a3, const char *filename, char **a5)
{
	static char* empty_tbl_string = { 0 };

	void *result;
	char** v6 = a5;

	for (result = _fastFileRead(filename, 0, 0, a3, a1); a2; --a2)
	{
		if (*v6)
		{
			unsigned __int16 v9 = *(_WORD *)v6 - 1;
			if (*(_WORD *)v6 == 0)
			{
				*v6 = 0;
			}
			else if (v9 < *(_WORD*)result)
			{
				*v6 = (char*)result + *((_WORD*)result + v9 + 1);
			}
			else
			{
				*v6 = empty_tbl_string;
			}
		}
		++v6;
	}
	return result;
}

void LoadGameData_(DatLoad* a1, const char* a2)
{
	unsigned int offset;

	int bytes_read = 0;
	BYTE* v3 = (BYTE*) fastFileRead_(&bytes_read, 0, a2, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 356);
	const void* v5 = (const void*)v3;
	for (BYTE* i = v3; a1->address; i += offset)
	{
		offset = a1->length * a1->entries;
		if (offset > bytes_read)
		{
			break;
		}
		memcpy(a1->address, v5, offset);
		++a1;
		bytes_read -= offset;
		v5 = (const void*)(offset + i);
	}
	if (bytes_read)
	{
		throw FileNotFoundException(a2, 24);
	}
	SMemFree(v3, "Starcraft\\SWAR\\lang\\gamedata.cpp", 402, 0);
}

FAIL_STUB_PATCH(LoadGameData);

DatLoad* sfxdataDat_;

void LoadSfx_()
{
	// TODO: dynamically allocate sfxdataDat memory
	sfxdataDat_ = new DatLoad[] {
		DatLoad { SFXData_SoundFile, 4, 1144 },
		DatLoad { SFXData_Flags1, 1, 1144 },
		DatLoad { SFXData_Flags2, 1, 1144 },
		DatLoad { SFXData_Race, 2, 1144 },
		DatLoad { SFXData_MuteVolume, 1, 1144 },
	};
	LoadGameData_(sfxdataDat_, "arr\\sfxdata.dat");
	dword_5999B0 = loadTBL_(1711, 1144, "Starcraft\\SWAR\\lang\\snd.cpp", "arr\\sfxdata.tbl", SFXData_SoundFile);
}

FAIL_STUB_PATCH(LoadSfx);

Campaign* active_campaign;
int active_campaign_entry_index;

char* MapdataFilenames_[73];

MEMORY_PATCH(0x4280A2, MapdataFilenames_);
MEMORY_PATCH(0x512BA0, MapdataFilenames_);
MEMORY_PATCH(0x512BA8, _countof(MapdataFilenames_));

MEMORY_PATCH(0x45A2F7, &active_campaign);
MEMORY_PATCH(0x45A5ED, &active_campaign);

void playRadioFreeZerg_()
{
	const MusicTrackDescription* v0;
	const char* v1;

	if (current_music_track == &radio_free_zerg)
	{
		v0 = &Race::races()[RaceId::RACE_Zerg].ingame_music[0];
		v1 = GetNetworkTblString_(66);
	}
	else
	{
		v0 = &radio_free_zerg;
		v1 = GetNetworkTblString_(65);
	}

	if (v1)
	{
		setUnitStatTxtErrorMsg((char*) v1);
	}
	PlayMusic_(v0);
}

FAIL_STUB_PATCH(playRadioFreeZerg);

bool radioFreeZergCheat_(const void* a2, CheatFlags* cheat_flags)
{
	if (gwGameMode == GAME_RUN && gameData.got_file_values.cheats != 1)
	{
		return false;
	}

	if (!memcmp(a2, &cheat_hash_no_glues, 8))
	{
		*cheat_flags ^= CHEAT_NoGlues;
		return true;
	}

	if (!memcmp(a2, &cheat_hash_radio_free_zerg, 8))
	{
		if (gwGameMode == GAME_RUN && is_expansion_installed && consoleIndex == RaceId::RACE_Zerg)
		{
			playRadioFreeZerg_();
		}
		return true;
	}

	return false;
}

FAIL_STUB_PATCH(radioFreeZergCheat);

int campaignTypeCheatStrings_(const char* a2);

BOOL CommandLineCheatCompare_(CheatFlags* game_cheats, const char* a2)
{
	CheatHashMaybe v7;
	CheatHashMaybe a2a;

	if (multiPlayerMode)
	{
		return 0;
	}

	srand(0x75u);
	for (int i = 0; i < 4; ++i)
	{
		v7.parts[i] = rand();
	}
	srand(time(0));
	a2a = v7;
	sub_44E420((CheatHashMaybe*)stru_68F580, &a2a);
	memset(a2a.parts, 0, sizeof(a2a.parts));
	makeCheatHash(a2, (__int64*)&a2a.parts[2]);
	memset(stru_68F580, 0, sizeof(stru_68F580));

	if (verifyCheatCode(&a2a.parts[2], game_cheats) || radioFreeZergCheat_(&a2a.parts[2], game_cheats))
	{
		return 1;
	}
	else
	{
		return campaignTypeCheatStrings_(a2) != 0;
	}
}

FAIL_STUB_PATCH(CommandLineCheatCompare);

void CommandLineCheck_()
{
	const char* command_line = GetCommandLineA();
	if (command_line)
	{
		strTokenize(command_line);
		CheatFlags game_cheats = GameCheats;
		for (char* argument = strTokenize(0); argument; argument = strTokenize(0))
		{
			if (CommandLineCheatCompare_(&game_cheats, argument))
			{
				cheatActivation(game_cheats, 0);
			}
			else
			{
				size_t argument_length = strlen(argument);
				if (!_strnicmp(argument, "nosound", argument_length))
				{
					byte_6D11D0 = 1;
				}
				else if (!_strnicmp(argument, "ddemulate", argument_length))
				{
					byte_6D5DFC = 1;
				}
				else if (!_strnicmp(argument, "-skip-end-mission-prompt", argument_length))
				{
					end_mission_prompt = false;
				}
				else if (!_strnicmp(argument, "-keep-app-active", argument_length))
				{
					keep_app_active_in_background = true;
				}
			}
		}
	}
}

FAIL_STUB_PATCH(CommandLineCheck);

void __fastcall FreeNetworkTBLHandle_(bool exit_code)
{
	if (networkTable)
	{
		SMemFree(networkTable, "Starcraft\\SWAR\\lang\\error.cpp", 21, 0);
	}
	networkTable = NULL;
}

FAIL_STUB_PATCH(FreeNetworkTBLHandle);

void LoadNetworkTBL_()
{
	AppAddExit_(FreeNetworkTBLHandle_);
	networkTable = (WORD*) fastFileRead_(0, 0, "rez\\network.tbl", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
}

FAIL_STUB_PATCH(LoadNetworkTBL);

HACCEL InitLocaleAccelerators_(int a1)
{
	HACCEL result = 0;

	if (local_dll_library != NULL)
	{
		result = LoadAcceleratorsA(local_dll_library, MAKEINTRESOURCEA(a1));
	}
	if (result == 0)
	{
		result = LoadAcceleratorsA(hInst, MAKEINTRESOURCEA(a1));
	}
	return result;
}

FAIL_STUB_PATCH(InitLocaleAccelerators);

void InitAccelerators_()
{
	hAccel = 0;
	HACCEL v1 = InitLocaleAccelerators_(0x65);
	HACCEL v2 = InitLocaleAccelerators_(0x71);
	if (v1)
	{
		if (v2)
		{
			ACCEL AccelDst[256];

			int v3 = CopyAcceleratorTableA(v1, 0, 0);
			int cAccelEntries = CopyAcceleratorTableA(v2, 0, 0);
			int cAccel = v3 + cAccelEntries;
			if (cAccel <= 0x100 && v3 == CopyAcceleratorTableA(v1, AccelDst, v3))
			{
				if (cAccelEntries == CopyAcceleratorTableA(v2, &AccelDst[v3], cAccelEntries))
				{
					hAccel = CreateAcceleratorTableA(AccelDst, cAccel);
				}
			}
		}
		DestroyAcceleratorTable(v1);
	}
	if (v2)
	{
		DestroyAcceleratorTable(v2);
	}
	if (!hAccel)
	{
		hAccel = InitLocaleAccelerators_(0x65);
	}
	dword_5968F4 = InitLocaleAccelerators_(0x66);
	hAccTable = InitLocaleAccelerators_(0x67);
	dword_5968F8 = hAccTable;
	DlgAccelerator = 0;
	AcceleratorTables = 0;
	input_procedures[EventNo::EVN_SYSCHAR] = input_standardSysHotkeys;
}

FAIL_STUB_PATCH(InitAccelerators);

GotFileValues* readTemplate_(const char* template_name, char* got_template_name, char* got_template_label)
{
	char buff[MAX_PATH];
	int got_file_size;

	sprintf_s(buff, "%s%s%s", "Templates\\", template_name, ".got");
	GotFile* got_file_data = (GotFile*)fastFileRead_(&got_file_size, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (!got_file_data)
	{
		return 0;
	}
	if (got_file_size != 97 || got_file_data->version != 3)
	{
		SMemFree(got_file_data, "Starcraft\\SWAR\\lang\\gametype.cpp", 97, 0);
		return 0;
	}
	if (got_file_data->values.template_id >= 129u || got_file_data->values.variation_id >= 8u)
	{
		SMemFree(got_file_data, "Starcraft\\SWAR\\lang\\gametype.cpp", 98, 0);
		return 0;
	}
	memcpy(got_template_name, got_file_data->name, 32u);
	memcpy(got_template_label, got_file_data->label, 32u);
	GotFileValues* result = (GotFileValues*)SMemAlloc(sizeof(GotFileValues), "Starcraft\\SWAR\\lang\\gametype.cpp", 74, 0);
	memcpy(result, &got_file_data->values, sizeof(GotFileValues));
	SMemFree(got_file_data, "Starcraft\\SWAR\\lang\\gametype.cpp", 78, 0);
	return result;
}

FAIL_STUB_PATCH(readTemplate);

int LoadGameTemplates_(TemplateConstructor template_constructor)
{
	char buff[MAX_PATH];
	char v7[MAX_PATH];
	int v10;

	sprintf_s(buff, "%s%s", "Templates\\", "templates.lst");
	BYTE* v1 = (BYTE*)fastFileRead(&v10, 0, buff, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	if (v1 == nullptr)
	{
		FatalError_("Unable to read game templates.");
	}
	BYTE* v11 = v1;
	while (sub_4AAE20(v7, (unsigned int*)&v10, (_BYTE**) &v11, sizeof(v7)))
	{
		char* v2 = strrchr(v7, '.');
		if (v2)
		{
			*v2 = 0;
		}

		char got_template_name[32];
		char got_template_label[32];
		GotFileValues* got_template_values = readTemplate_(v7, got_template_name, got_template_label);
		if (got_template_values)
		{
			template_constructor(got_template_name, got_template_label, got_template_values, 0);
			SMemFree(got_template_values, "Starcraft\\SWAR\\lang\\gametype.cpp", 231, 0);
		}
	}
	if (v1)
	{
		SMemFree(v1, "Starcraft\\SWAR\\lang\\gametype.cpp", 233, 0);
	}
	return 1;
}

FAIL_STUB_PATCH(LoadGameTemplates);
FAIL_STUB_PATCH(sub_4AB970);

void InitializeImage_()
{
	memset(ScreenLayers, 0, sizeof(layer) * 8);
	memset(RefreshRegions, 0, sizeof(RefreshRegions));
	GameScreenBuffer.wid = SCREEN_WIDTH;
	GameScreenBuffer.ht = SCREEN_HEIGHT;
	GameScreenBuffer.data = 0;
	stru_6D5DF0.data = (u8*)SMemAlloc(2304, "Starcraft\\SWAR\\lang\\gds\\image.cpp", 141, 0);
}

FAIL_STUB_PATCH(InitializeImage);

void InitializeDialogScreenLayer_()
{
	AppAddExit_(DestroyScreenLayer);
	ScreenLayers[2].left = 0;
	ScreenLayers[2].top = 0;
	ScreenLayers[2].width = SCREEN_WIDTH;
	ScreenLayers[2].height = SCREEN_HEIGHT;
	ScreenLayers[2].pSurface = 0;
	ScreenLayers[2].pUpdate = DialogUpdateProc;
	ScreenLayers[2].buffers = 1;
	ScreenLayers[2].bits = 32;
}

FAIL_STUB_PATCH(InitializeDialogScreenLayer);

void initializeDefaultPlayerNames_()
{
	for (int i = 0; i < 8; ++i)
	{
		PlayerType nType = Players[i].nType;
		if (nType == PT_Computer || nType == PT_Rescuable || nType == PT_Unknown0 || nType == PT_Neutral)
		{
			int v1 = IsExpansion != 0 ? 145 : 29;

			switch (Players[i].nRace)
			{
			case RaceId::RACE_Terran:
				v1 += 0;
				break;
			case RaceId::RACE_Zerg:
				v1 += 8;
				break;
			case RaceId::RACE_Protoss:
				v1 += 16;
				break;
			}

			int v5 = i < 8 ? factionsColorsOrdering[i] : i;
			const char* player_name = GetNetworkTblString_(v1 + v5);
			if (!player_name)
			{
				player_name = GetNetworkTblString_(11);
			}
			SStrCopy(Players[i].szName, player_name, sizeof(Players[i].szName));
		}
	}

	if (!multiPlayerMode && active_campaign && (CampaignIndex == EMD_zerg04 || CampaignIndex == EMD_zerg06))
	{
		Players[2].szName[0] = 0;
	}
}

FAIL_STUB_PATCH(initializeDefaultPlayerNames);

void setCursor_(grpHead* cursor)
{
	if (last_cursor != cursor)
	{
		last_cursor = cursor;
		drawCursor_();
	}
}

FAIL_STUB_PATCH(setCursor);

void setCursorType_(CursorType cursor_type)
{
	if (last_cursor_type != cursor_type)
	{
		last_cursor_type = cursor_type;
		setCursor_(cursor_graphics[cursor_type]);
	}
}

FAIL_STUB_PATCH(setCursorType);

void __fastcall DestroyCursors_(bool exit_code)
{
	ScreenLayers[0].buffers = 0;
	last_cursor = NULL;

	for (int i = 0; i < _countof(cursor_graphics); i++)
	{
		if (cursor_graphics[i])
		{
			SMemFree(cursor_graphics[i], "Starcraft\\SWAR\\lang\\cur.cpp", 205, 0);
			cursor_graphics[i] = 0;
		}
	}
}

FAIL_STUB_PATCH(DestroyCursors);

CursorType operator++(CursorType& cursor_type)
{
	return CursorType(++reinterpret_cast<int&>(cursor_type));
}

void LoadCursors_()
{
	AppAddExit_(DestroyCursors_);
	for (CursorType i = CursorType::CUR_ARROW; i < CursorType::CUR_MAX; ++i)
	{
		char dest[260];
		strcpy_s(dest, "cursor\\");
		strcat_s(dest, cursor_filenames[i]);
		grpHead* v1 = LoadGraphic(dest, 0, "Starcraft\\SWAR\\lang\\cur.cpp", 212);

		for (int frame_index = (v1->wFrames & 0x7FFF) - 1; frame_index >= 0; frame_index--)
		{
			grpFrame* frame = &v1->frames[frame_index];
			frame->dataOffset = (u32)v1 + (frame->dataOffset & 0x7FFFFFFF);
		}
		cursor_graphics[i] = v1;
	}
	setCursorType_(CUR_ARROW);
	ScreenLayers[0].buffers = 0;
	ScreenLayers[0].pUpdate = cursorUpdateProc;
	dword_597398 = GetTickCount();
}

FAIL_STUB_PATCH(LoadCursors);

DatLoad* weaponsDat_;

void __fastcall FreeMapdataTable_(bool exit_code)
{
	if (dword_51CC30)
	{
		SMemFree(dword_51CC30, "Starcraft\\SWAR\\lang\\init.cpp", 1302, 0);
		dword_51CC30 = NULL;
	}

	// TODO: dynamically clean mapdataDat memory
	// TODO: dynamically clean upgradesDat memory
	// TODO: dynamically clean weaponsDat memory
	delete[] weaponsDat_;
	// TODO: dynamically clean techdataDat memory
	// TODO: dynamically clean portdataDat memory
}

FAIL_STUB_PATCH(FreeMapdataTable);

void PreInitData_()
{
	SFileSetIoErrorMode(1, FileIOErrProc_);
	AppAddExit_(leaveOnQuit);
	if (signal(2, 1) != -1)
	{
		signal(21, 1);
	}
	CommandLineCheck_();
	AppAddExit_(CloseAllArchives);
	InitializeArchiveHandles_();
	DataVersionCheck("rez\\DataVersion.txt");
	if (cd_archive_mpq)
	{
		DataVersionCheck("rez\\CDversion.txt");
	}
	registry_options.field_18 |= 7;
	LoadRegOptions();
	AppAddExit_(saveRegOptions);
	LoadNetworkTBL_();
	InitAccelerators_();
	AppAddExit_(DestroyAccelerators);
	LoadMenuFonts();
	AppAddExit_(DestroyFonts);
	InitializeImage_();
	AppAddExit_(DestroyImage);
	LoadCursors_();
	InitializeDialogScreenLayer_();
	dword_6D5E20 = &GameScreenBuffer;
	CreateHelpContext();
	AppAddExit_(DestroyHelpContext);
	LoadGameData_(mapdataDat, "arr\\mapdata.dat"); // TODO: is this call needed?
	dword_51CC30 = loadTBL_(1577, _countof(MapdataFilenames_), "Starcraft\\SWAR\\lang\\init.cpp", "arr\\mapdata.tbl", MapdataFilenames_);
	AppAddExit_(FreeMapdataTable_);
	LoadGameTemplates_(Template_Constructor);
	AppAddExit_(DestroyGameTemplates);
}

FAIL_STUB_PATCH(PreInitData);

char *GetErrorString_(LPSTR lpBuffer, DWORD a2, unsigned int a3)
{
	switch ((a3 >> 16) & 0x1FFF)
	{
	case 0x878u:
		DSERR_GetString(a2, lpBuffer, a3);
		break;
	case 0x876u:
		DDERR_GetString(a2, lpBuffer, a3);
		break;
	case 0x617u:
		_snprintf(lpBuffer, a2, "MMSYS error 0x%x", a3);
		break;
	default:
		if (!SErrGetErrorStr(a3, lpBuffer, a2) && !FormatMessageA(0x1000, 0, a3, 0x400, lpBuffer, a2, 0))
			_snprintf(lpBuffer, a2, "unknown error 0x%08x", a3);
		break;
	}
	unsigned v6 = strlen(lpBuffer);
	for (char* i = &lpBuffer[v6 - 1]; (signed int)v6 > 0; *i = 0)
	{
		--v6;
		--i;
		char v8 = *i;
		if (v8 != '\r' && v8 != '\n')
			break;
	}
	return lpBuffer;
}

void ErrorDDrawInit_(const char *source_file, const char *function_name, unsigned int last_error, WORD resource, int source_line)
{
	char dwInitParam[512];
	CHAR Buffer[256];

	const char* v5 = source_file;
	const char* v6 = function_name;
	const char* v7 = strrchr(source_file, '\\');
	if (v7)
		v5 = v7 + 1;
	if (!v6)
		v6 = "";
	char* v8 = GetErrorString_(Buffer, sizeof(Buffer), last_error);
	sprintf_s(dwInitParam, "%s\n%s line %d\n%s", v8, v5, source_line, v6);
	sub_421510_();
	if (DialogBoxParamA(local_dll_library, (LPCSTR)resource, hWndParent, DialogFunc, (LPARAM)dwInitParam) == -1)
		FatalError_("GdsDialogBoxParam: %d", resource);
	DLGErrFatal_();
}

FAIL_STUB_PATCH(ErrorDDrawInit);

BOOL BWFXN_DDrawInitialize_()
{
	PALETTEENTRY palette_entries[256];
	DDSURFACEDESC surface_desc;

	ShowWindow(hWndParent, 1);
	GUID* v4 = byte_6D5DFC ? (GUID*)DDCREATE_EMULATIONONLY : NULL;
	HRESULT v5 = DirectDrawCreate(v4, &DDInterface, 0);
	if (v5 == DDERR_NODIRECTDRAWSUPPORT)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "DirectDrawCreate", 0x887600DE, 0x6Eu, 124);
	if (v5)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "DirectDrawCreate", v5, 0x66u, 125);
	HRESULT v6 = DDInterface->SetCooperativeLevel(hWndParent, 16 | DSSCL_PRIORITY | DSSCL_NORMAL);
	if (v6 != DDERR_EXCLUSIVEMODEALREADYSET && v6 != NULL)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "SetCooperativeLevel", v6, 0x66u, 148);
#ifndef BYPASS_DDRAW_STUFF
	if (DDInterface->SetDisplayMode(SCREEN_WIDTH, SCREEN_HEIGHT, 8))
	{
		int v7 = GetSystemMetrics(SM_CXSCREEN);
		int v8 = GetSystemMetrics(SM_CYSCREEN);
		unsigned v9 = DDInterface->SetDisplayMode(v7, v8, 8);
		if (v9)
			ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "SetDisplayMode", v9, 0x67u, 160);
	}
#endif
	memset(palette_entries, 0, sizeof(palette_entries));
	unsigned v10 = DDInterface->CreatePalette(DDPCAPS_ALLOW256 | DDPCAPS_8BIT, palette_entries, &PrimaryPalette, 0);
	if (v10)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "CreatePalette", v10, 0x66u, 182);
	memset(&surface_desc, 0, sizeof(surface_desc));
	surface_desc.dwSize = sizeof(DDSURFACEDESC);
	surface_desc.dwFlags = 1;
	surface_desc.ddsCaps.dwCaps = 512;
	unsigned v11 = DDInterface->CreateSurface(&surface_desc, &PrimarySurface, 0);
	if (v11)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "CreatePrimarySurface", v11, 0x66u, 193);
#ifndef BYPASS_DDRAW_STUFF
	unsigned v12 = PrimarySurface->SetPalette(PrimaryPalette);
	if (v12)
		ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "SetPalette", v12, 0x66u, 203);
#endif
	surface_desc.dwSize = sizeof(DDSURFACEDESC);
	if (PrimarySurface->Lock(0, &surface_desc, 1, 0))
	{
		memset(&surface_desc, 0, sizeof(surface_desc));
		surface_desc.dwSize = sizeof(DDSURFACEDESC);
		surface_desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		surface_desc.ddsCaps.dwCaps = 2112;
		surface_desc.dwWidth = SCREEN_WIDTH;
		surface_desc.dwHeight = SCREEN_HEIGHT;
		unsigned v13 = DDInterface->CreateSurface(&surface_desc, &BackSurface, 0);
		if (v13)
			ErrorDDrawInit_("Starcraft\\SWAR\\lang\\gds\\vidinimo_PC.cpp", "CreateBackSurface", v13, 0x66u, 220);
	}
	else
	{
		PrimarySurface->Unlock(surface_desc.lpSurface);
	}
	return SDrawManualInitialize(hWndParent, DDInterface, PrimarySurface, 0, 0, BackSurface, PrimaryPalette, 0);
}

FAIL_STUB_PATCH(sub_41DDD0);
FAIL_STUB_PATCH(BWFXN_DDrawInitialize);

void BWFXN_updateImageData_()
{
	for (int i = 0; i < map_size.height; ++i)
	{
		for (CSprite* sprites = _SpritesOnTileRow.heads[i]; sprites; sprites = sprites->next)
		{
			for (CImage* image = sprites->pImageHead; image; image = CImage__updateGraphicData(image)->next);
		}
	}
}

FAIL_STUB_PATCH(BWFXN_updateImageData);

void drawMinitileImageData_(int framebuf_pos, int minitile)
{
	char* v4 = (char*)VR4Data + 32 * (minitile & 0xFFFE);
	for (int i = 0; i < 8; i++)
	{
		if (framebuf_pos >= TILE_CACHE_SIZE)
		{
			framebuf_pos -= TILE_CACHE_SIZE;
		}
		for (int j = 0; j < 8; j++)
		{
			GameTerrainCache[framebuf_pos + j] = (minitile & 1) ? v4[8 * i + 7 - j] : v4[8 * i + j];
		}
		framebuf_pos += RENDER_AREA_WIDTH;
	}
}

FAIL_STUB_PATCH(drawMinitileImageData);

void renderTerrainGRPToCache_(grpFrame* a1, int a2)
{
	dword_50CED9 = GameTerrainCache + TILE_CACHE_SIZE;
	int terrain_cache_index = RENDER_AREA_WIDTH * a1->y + a1->x + a2;

	dword_50CECD = a1->wid;
	dword_50CEC5 = RENDER_AREA_WIDTH - dword_50CECD;
	u16* v6 = (u16*)a1->dataOffset;
	int v7 = 0;
	int i = 0;

	for (dword_50CED1 = a1->hgt; dword_50CED1; dword_50CED1--)
	{
		terrain_cache_index %= TILE_CACHE_SIZE;
		int v9 = dword_50CECD;
		byte* v11 = (byte*)v6 + v6[i];
		i++;
		do
		{
			while (1)
			{
				while (1)
				{
					LOBYTE(v7) = *v11;
					v11++;
					if ((v7 & 0x80u) == 0)
					{
						break;
					}
					LOBYTE(v7) = v7 & 0x7F;
					v9 -= v7;
					terrain_cache_index += v7;
					if (v9 <= 0)
					{
						goto LABEL_16;
					}
				}
				if (v7 & 0x40)
				{
					break;
				}
				v9 -= v7;
				memcpy(GameTerrainCache + terrain_cache_index, v11, v7);
				v11 += v7;
				terrain_cache_index += v7;
				v7 = 0;
				if (v9 <= 0)
				{
					goto LABEL_16;
				}
			}
			LOBYTE(v7) = v7 & 0xBF;
			v9 -= v7;
			memset(GameTerrainCache + terrain_cache_index, *v11, v7);
			terrain_cache_index += v7;
			v7 = 0;
			v11++;
		} while (v9 > 0);

	LABEL_16:
		terrain_cache_index += dword_50CEC5;
	}
}

FAIL_STUB_PATCH(renderTerrainGRPToCache);

void drawMegatileImageData_(int megatile, int framebuf_position, int x, int y)
{
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			drawMinitileImageData_(framebuf_position + 8 * RENDER_AREA_WIDTH * i + 8 * j, VX4Data[megatile & 0x7FFF].wImageRef[i][j]);
		}
	}

	if (megatile & 0x8000)
	{
		u8 v20 = CreepEdgeData[y * dword_6D0F08 + x];
		if (v20)
		{
			renderTerrainGRPToCache_((grpFrame*)((char*)TerrainGraphics + 8 * v20 - 2), framebuf_position);
		}
	}
}

void __stdcall drawMegatileImageData__(int x, int y)
{
	int megatile;
	int framebuf_position;

	__asm
	{
		mov megatile, ebx
		mov framebuf_position, edi
	}

	drawMegatileImageData_(megatile, framebuf_position, x, y);
}

FUNCTION_PATCH((void*)0x49B9F0, drawMegatileImageData__);

void drawScreenRowTiles_(__int16* a1, TileID* a2, MegatileFlags* a3, int a4, int a5)
{
	if (a4 < map_size.height)
	{
		int v8 = 0;
		int v6 = MoveToTile.x;

		for (int i = 0; i < min(map_size.width - MoveToTile.x, RENDER_AREA_TILE_COLUMNS); i++)
		{
			if (InReplay ? (ReplayVision & ~a3[i]) : (playerVisions & a3[i]) == 0)
			{
				__int16 megatile_ref = TileSetMap[a2[i].group].megaTileRef[a2[i].tile];
				if (megatile_ref != (a1[i] & 0x7FFF))
				{
					a1[i] = megatile_ref;
					if (a2[i].group == 1)
					{
						a3[i] |= LOCAL_CREEP;
					}
					else
					{
						a3[i] &= ~LOCAL_CREEP;
					}
					iterateDirectionalCreepData(BWFXN_CreepManagementCB, 0, v6 + i, a4);
					drawMegatileImageData_(a1[i], (a5 + i * TILE_HEIGHT) % (9408 * TILE_HEIGHT), v6 + i, a4);
					v8 = 1;
				}
			}
		}

		if (v8)
		{
			HasMegatileUpdate = 1;
		}
	}
}

FAIL_STUB_PATCH(drawScreenRowTiles);

void BWFXN_drawMapTiles_()
{
	int v2 = MoveToTile.y;
	if (v2 >= 16 * RENDER_AREA_TILE_ROWS)
	{
		v2 -= 16 * RENDER_AREA_TILE_ROWS;
	}
	if (v2 >= 8 * RENDER_AREA_TILE_ROWS)
	{
		v2 -= 8 * RENDER_AREA_TILE_ROWS;
	}
	if (v2 >= 4 * RENDER_AREA_TILE_ROWS)
	{
		v2 -= 4 * RENDER_AREA_TILE_ROWS;
	}
	if (v2 >= 2 * RENDER_AREA_TILE_ROWS)
	{
		v2 -= 2 * RENDER_AREA_TILE_ROWS;
	}
	if (v2 >= 1 * RENDER_AREA_TILE_ROWS)
	{
		v2 -= 1 * RENDER_AREA_TILE_ROWS;
	}
	for (int i = 0; i < RENDER_AREA_TILE_ROWS; i++)
	{
		drawScreenRowTiles_(
			&CellMap[MoveToTile.x + (MoveToTile.y + i) * map_size.width],
			&MapTileArray[MoveToTile.x + (MoveToTile.y + i) * map_size.width],
			&active_tiles[MoveToTile.x + (MoveToTile.y + i) * map_size.width],
			MoveToTile.y + i,
			(MoveToTile.x + ((v2 + i) % RENDER_AREA_TILE_ROWS) * RENDER_AREA_TILE_COLUMNS * TILE_WIDTH) * TILE_HEIGHT);
	}
}

FAIL_STUB_PATCH(BWFXN_drawMapTiles);

void BlitToBitmap_(int a1, int a2, byte* a3)
{
	unsigned int v7;
	unsigned int v4;
	byte* a4 = &GameTerrainCache[a1];

	for (int i = 0; i < a2; i++)
	{
		v4 = dword_50CEF0;
		if (dword_50CEF0 + a1 >= TILE_CACHE_SIZE)
		{
			if (a1 < TILE_CACHE_SIZE)
			{
				v7 = (unsigned int)(TILE_CACHE_SIZE - a1);
				memcpy(a3, a4, v7);
				a3 += v7;
				a4 += v7;
				v4 = dword_50CEF0 - (TILE_CACHE_SIZE - a1);
			}
			a1 -= TILE_CACHE_SIZE;
			a4 -= TILE_CACHE_SIZE;
		}
		memcpy(a3, a4, v4);
		a1 += RENDER_AREA_WIDTH;
		a3 += v4 - dword_50CEF0 + SCREEN_WIDTH;
		a4 += v4 - dword_50CEF0 + RENDER_AREA_WIDTH;
	}
}

FAIL_STUB_PATCH(BlitToBitmap);

void BWFXN_blitMapTiles_()
{
	dword_50CEF0 = GAME_AREA_WIDTH;
	BlitToBitmap_((MoveToX + RENDER_AREA_WIDTH * MoveToY) % TILE_CACHE_SIZE, GAME_AREA_HEIGHT, GameScreenBuffer.data);
}

FAIL_STUB_PATCH(BWFXN_blitMapTiles);

void refreshImageRange_(int top_row, int bottom_row)
{
	for (int i = top_row; i <= bottom_row; ++i)
	{
		for (CSprite* sprite = _SpritesOnTileRow.heads[i]; sprite; sprite = sprite->next)
		{
			refreshSpriteData(sprite);
		}
	}
}

FAIL_STUB_PATCH(refreshImageRange);

void __fastcall BlitTerrainCacheToGameBitmap_(int a0, int a1, int a2, int a3)
{
	dword_50CEF0 = 16 * a2;
	BlitToBitmap_(a3, 16, &GameScreenBuffer.data[16 * a1 + 16 * a0 * GameScreenBuffer.wid]);
}

FAIL_STUB_PATCH(BlitTerrainCacheToGameBitmap);

void blitTileCacheOnRefresh_()
{
	u8* v0 = RefreshRegions;
	int v1 = (MoveToX + RENDER_AREA_WIDTH * MoveToY) % TILE_CACHE_SIZE;

	for (int v6 = 0; v6 < GAME_AREA_HEIGHT / 16; v6++)
	{
		for (int i = 0; i < GAME_AREA_WIDTH / 16; ++i)
		{
			if (v1 >= TILE_CACHE_SIZE)
			{
				v1 -= TILE_CACHE_SIZE;
			}
			if (*v0 == 1)
			{
				int v3 = i + 1;
				int v4 = 1;
				if (i + 1 < GAME_AREA_WIDTH / 16)
				{
					do
					{
						if (*v0 == 0)
						{
							break;
						}
						++v0;
						++v4;
						++v3;
					} while (v3 < GAME_AREA_WIDTH / 16);
				}
				BlitTerrainCacheToGameBitmap_(v6, i, v4, v1);
				v1 = v1 + 16 * (v4 - 1);
				i = i + v4 - 1;
				if (v1 >= TILE_CACHE_SIZE)
				{
					v1 -= TILE_CACHE_SIZE;
				}
			}
			v1 += 16;
			++v0;
		}
		v1 += 632 * 16;
		if (v1 >= TILE_CACHE_SIZE)
		{
			v1 -= TILE_CACHE_SIZE;
		}
	}
}

FAIL_STUB_PATCH(blitTileCacheOnRefresh);

void drawImage_(CImage* a1)
{
	if ((a1->flags & 0x40) == 0 && a1->grpBounds.bottom > 0 && a1->grpBounds.right > 0 && ((a1->flags & 1) != 0 || isImageRefreshable(a1)))
	{
		RECT v8;
		v8.left = a1->grpBounds.left;
		v8.right = a1->grpBounds.right;
		v8.top = a1->grpBounds.top;
		v8.bottom = a1->grpBounds.bottom;

		a1->renderFunction(a1->screenPosition.x, a1->screenPosition.y, &a1->GRPFile->frames[a1->frameIndex], (rect*)&v8, (int)a1->coloringData);
	}
	a1->flags &= ~ImageFlags::IF_REDRAW;
}

FAIL_STUB_PATCH(drawImage);

void drawSprite_(CSprite* a1)
{
	dword_6D5BD4 = getColourID(a1->playerID);
	memcpy(byte_50CDC1 + 8, stru_581D76[dword_6D5BD4].colors, sizeof(stru_581D76[dword_6D5BD4].colors));

	if (a1->pImagePrimary)
	{
		unknownColorShiftSomething(a1->pImagePrimary->paletteType, a1->playerID);
	}
	for (CImage* image = a1->pImageTail; image; image = image->prev)
	{
		drawImage_(image);
	}
}

FAIL_STUB_PATCH(drawSprite);

void BWFXN_drawAllSprites_()
{
	for (CSprite* sprite = getFirstSprite(); sprite; sprite = dword_6C2318[dword_6C4A28])
	{
		drawSprite_(sprite);
		if (dword_6C4A28 == 0)
		{
			break;
		}
		--dword_6C4A28;
	}
}

FAIL_STUB_PATCH(BWFXN_drawAllSprites);

void updateAllFog_(int a1)
{
	if (a1)
	{
		updateFog(GAME_AREA_HEIGHT, 0, 0, GAME_AREA_WIDTH);
	}
	else
	{
		int region_index = 0;
		for (int i = 0; i < GAME_AREA_HEIGHT; i += 16)
		{
			for (int j = 0; j < GAME_AREA_WIDTH; j += 16)
			{
				if (RefreshRegions[region_index++])
				{
					int k;
					for (k = 16; j + k < GAME_AREA_WIDTH; k += 16)
					{
						if (RefreshRegions[region_index] == 0)
						{
							break;
						}
						++region_index;
					}
					updateFog(i + 16, i, j, k + j);
					j = k + j - 16;
				}
			}
		}
	}
}

FAIL_STUB_PATCH(updateAllFog);

void BWFXN_drawDragSelBox_()
{
	if (byte_66FF5C)
	{
		if (InputFlags)
		{
			DrawColor = byte_6CEB32;
			BWFXN_Draw(stru_66FF50.top, stru_66FF50.left, stru_66FF50.right - stru_66FF50.left + 1, stru_66FF50.bottom - stru_66FF50.top + 1);
		}
		else
		{
			refreshDragSelectBox();
			SetInGameInputProcs_();
		}
	}
}

FAIL_STUB_PATCH(BWFXN_drawDragSelBox);

void BWFXN_drawAllThingys_()
{
	if (wantThingyUpdate)
	{
		CThingy* currentThingy = ThingyList_UsedFirst;
		for (CImage* image = ThingyList_UsedFirst->sprite->pImageHead; image; image = CImage__updateGraphicData(image)->next);
		drawSprite_(currentThingy->sprite);
		for (CImage* image = ThingyList_UsedFirst->sprite->pImageHead; image; image = image->next)
		{
			image->flags |= ImageFlags::IF_REDRAW;
		}
	}
}

FAIL_STUB_PATCH(BWFXN_drawAllThingys);

MEMORY_PATCH((void*)0x4D5857, GAME_AREA_WIDTH);
MEMORY_PATCH((void*)0x4D5888, GAME_AREA_HEIGHT);

MEMORY_PATCH((void*)0x40B168, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B28E, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B252, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B2E6, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B400, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B3CA, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40BC64, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40BC2E, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B5A9, SCREEN_WIDTH * SCREEN_HEIGHT);
MEMORY_PATCH((void*)0x40B454, SCREEN_WIDTH * SCREEN_HEIGHT);

void __fastcall DrawNullProc_(int _unused1, int _unused2, Bitmap* a1, bounds* a2)
{
}

void __fastcall DrawGameProc_(int _unused1, int _unused2, Bitmap* a1, bounds* a2)
{
	int v2 = ScreenLayers[5].bits & 1;
	if (ScreenLayers[5].bits & 1)
	{
		maskSomething0();
		memcpy(dword_6D5C10, dword_6D5C0C, 0x198u);
		BWFXN_updateImageData_();
		maskSomething2();
		BWFXN_drawMapTiles_();
		BWFXN_blitMapTiles_();
	}
	else
	{
		maskSomething1();
		refreshImageRange_(dword_5993A4, dword_5993C0);
		maskSomething2();
		BWFXN_drawMapTiles_();
		blitTileCacheOnRefresh_();
	}
	BWFXN_drawAllSprites_();
	if (CurrentTileSet == Tileset::Platform)
	{
		if (v2)
			drawStars();
		else
			refreshStars();
	}
	updateAllFog_(v2);
	BWFXN_DrawHighTarget();
	BWFXN_drawDragSelBox_();
	BWFXN_drawAllThingys_();
}

FAIL_STUB_PATCH(DrawGameProc);

void InitializeGameLayer_()
{
	SetRect(&game_screen_pos, 0, 0, GAME_AREA_WIDTH - 1, GAME_AREA_HEIGHT - 1);
	ScreenLayers[5].left = 0;
	ScreenLayers[5].top = 0;
	ScreenLayers[5].pSurface = 0;
	ScreenLayers[5].bits = 0;
	ScreenLayers[5].width = GAME_AREA_WIDTH;
	ScreenLayers[5].height = GAME_AREA_HEIGHT;
	ScreenLayers[5].pUpdate = has_viewport ? DrawGameProc_ : DrawNullProc_;
	memset(RefreshRegions, 1u, sizeof(RefreshRegions));
	for (int i = 3; i <= 5; ++i)
	{
		ScreenLayers[i].bits |= 1;
		int left = ScreenLayers[i].left;
		int top = ScreenLayers[i].top;
		int bottom = top + ScreenLayers[i].height - 1;
		int right = left + ScreenLayers[i].width - 1;
		BWFXN_RefreshTarget(left, bottom, top, right);
	}
}

FAIL_STUB_PATCH(InitializeGameLayer);

void __cdecl refreshSelectionScreen_()
{
	BWFXN_RefreshTarget(ScreenLayers[1].left, ScreenLayers[1].height + ScreenLayers[1].top - 1, ScreenLayers[1].top, ScreenLayers[1].width + ScreenLayers[1].left - 1);
	if (ScreenLayers[1].buffers)
	{
		ScreenLayers[1].bits |= 1;
		BWFXN_RefreshTarget(ScreenLayers[1].left, ScreenLayers[1].height + ScreenLayers[1].top - 1, ScreenLayers[1].top, ScreenLayers[1].width + ScreenLayers[1].left - 1);
	}
	ScreenLayers[1].left = GAME_AREA_WIDTH;
	ScreenLayers[1].top = GAME_AREA_HEIGHT;
	dword_655C48 = 0;
}

FUNCTION_PATCH(refreshSelectionScreen, refreshSelectionScreen_);

int DSoundCreate_(AudioVideoInitializationError* a1)
{
	HRESULT v4 = DirectSoundCreate(0, &direct_sound, 0);
	a1->error_code = v4;
	if (v4 == 0x8878000A)
	{
		HRESULT v5;
		do
		{
			if (dword_51A43C <= 0)
			{
				break;
			}
			Sleep(1000);
			--dword_51A43C;
			v5 = DirectSoundCreate(0, &direct_sound, 0);
			a1->error_code = v5;
		} while (v5 == -2005401590);
	}
	if (a1->error_code)
	{
		a1->function_name = "DirectSoundCreate";
		if (a1->error_code == -2005401590)
		{
			a1->dword4 = 135;
		}
		return 0;
	}
	else
	{
		return 1;
	}
}

FAIL_STUB_PATCH(DSoundCreate);

int SetCooperativeLevel_(HWND hwnd, AudioVideoInitializationError* a2)
{
	a2->error_code = direct_sound->SetCooperativeLevel(hwnd, 2);
	if (!a2->error_code)
	{
		return 1;
	}
	a2->error_code = direct_sound->SetCooperativeLevel(hwnd, 1);
	if (!a2->error_code)
	{
		return 1;
	}
	a2->function_name = "SetCooperativeLevel";
	return 0;
}

FAIL_STUB_PATCH(SetCooperativeLevel);

int CreateSoundBuffer_(AudioVideoInitializationError* a1)
{
	DSBUFFERDESC sound_buffer;

	sound_buffer.dwSize = sizeof(DSBUFFERDESC);
	sound_buffer.dwBufferBytes = 0;
	sound_buffer.dwReserved = 0;
	sound_buffer.lpwfxFormat = 0;
	sound_buffer.dwFlags = 1;

	a1->error_code = direct_sound->CreateSoundBuffer(&sound_buffer, &soundbuffer, 0);
	if (!a1->error_code)
	{
		return 1;
	}
	a1->function_name = "CreateSoundBuffer";
	return 0;
}

FAIL_STUB_PATCH(CreateSoundBuffer);

HRESULT SetAudioFormat_()
{
	WAVEFORMATEX wave_format;
	wave_format.cbSize = 0;
	wave_format.wFormatTag = 1;
	wave_format.nChannels = 2;
	wave_format.nSamplesPerSec = 22050;
	wave_format.wBitsPerSample = 16;
	wave_format.nBlockAlign = 4;
	wave_format.nAvgBytesPerSec = 88200;

	return soundbuffer->SetFormat(&wave_format);
}

FAIL_STUB_PATCH(SetAudioFormat);

void LoadBtnSfxFile_()
{
	char buff[256];

	if (SFXData_SoundFile[SFX_Misc_Button_1])
	{
		sprintf_s(buff, "sound\\%s", SFXData_SoundFile[15]);
		dword_6D1268 = LoadSoundProc(buff, 0);
	}
	else
	{
		dword_6D1268 = (IDirectSoundBuffer*)SFXData_SoundFile[SFX_Misc_Button_1];
	}
}

FAIL_STUB_PATCH(LoadBtnSfxFile);

void DLGMusicFade_(const MusicTrackDescription* music_track)
{
	if (!directsound || !byte_6D5BBC)
	{
		if (music_track && music_track->fade_in_maybe)
		{
			int old_bigvolume = bigvolume;
			bigvolume = -10000;
			PlayMusic_(music_track);
			bigvolume = old_bigvolume;
			if (registry_options.Music)
			{
				dword_6D5BB8 = -3396;
				KillTimer(hWndParent, 3u);
				SetTimer(hWndParent, 3u, 0x14u, FadeMusicProc);
			}
		}
		else
		{
			PlayMusic_(music_track);
		}
	}
}

FAIL_STUB_PATCH(DLGMusicFade);

void muteBgm_(RegistryOptions* a1)
{
	if (direct_sound)
	{
		dword_6D5E3C = volume[99 * a1->Sfx / 100] - dword_6D5A0C;
		if (a1->Music)
		{
			int v2 = dword_5008EC[a1->Music] - dword_6D5A0C;
			bigvolume = v2;
			if (directsound)
			{
				if (byte_6D5BBD)
				{
					v2 -= 750;
					if (v2 < -10000)
					{
						v2 = -10000;
					}
				}
				SFileDdaSetVolume(directsound, v2, 0);
			}
			if (!byte_6D638C)
			{
				byte_6D638C = 1;
				DLGMusicFade_(current_music_track);
			}
		}
		else
		{
			bigvolume = -10000;
			if (directsound)
			{
				SFileDdaSetVolume(directsound, -10000, 0);
				if (directsound)
				{
					SFileDdaEnd(directsound);
					SFileCloseFile(directsound);
					directsound = 0;
				}
			}
			byte_6D5BBC = 0;
			byte_6D638C = 0;
		}
	}
}

void muteBgm__()
{
	RegistryOptions* a1;

	__asm mov a1, esi

	muteBgm_(a1);
}

FUNCTION_PATCH((void*)0x4BC320, muteBgm__);

void initVolume_()
{
	dword_5998E8 = 50;
	if (registry_options.Sfx == -1)
	{
		registry_options.Sfx = 50;
	}
	dword_5999B4 = 25;
	if (registry_options.Music == -1)
	{
		registry_options.Music = 25;
	}
	muteBgm_(&registry_options);
}

FAIL_STUB_PATCH(initVolume);

void stopMusic_()
{
	DWORD v4;

	if (directsound)
	{
		if (byte_6D5BBC)
		{
			int v1 = bigvolume;
			byte_6D5BBC = 0;
			if (!byte_6D5BBD || (v1 = bigvolume - 750, dword_6D5BB8 = -10000, bigvolume - 750 >= -10000))
			{
				dword_6D5BB8 = v1;
			}
			DWORD tick_count = GetTickCount();

			for (int i = 5; ; i += 5)
			{
				do
				{
					v4 = GetTickCount();
				} while ((int)(v4 - tick_count) < 0);
				tick_count = v4 + 20;
				int v5 = dword_6D5BB8 - i;
				dword_6D5BB8 -= i;
				if (dword_6D5BB8 < -4000)
				{
					break;
				}
				SFileDdaSetVolume(directsound, v5, 0);
			}
			dword_6D5BB8 = -10000;
			SFileDdaSetVolume(directsound, -10000, 0);
		}
		if (directsound)
		{
			SFileDdaEnd(directsound);
			SFileCloseFile(directsound);
			directsound = 0;
		}
		byte_6D5BBC = 0;
	}
}

FAIL_STUB_PATCH(stopMusic);

void stopAllSound_(void)
{
	stopSounds();
	stopMusic_();
}

FAIL_STUB_PATCH(stopAllSound);

unsigned __stdcall DSoundThread_(void* a2)
{
	void* location = NULL;
	WaitForSingleObject(dword_6D5A00, INFINITE);

	while (dword_6D5A08)
	{
		LPVOID v20;
		DWORD v21;
		int bytes_read;
		int v22 = 0;

		for (int i = 0; i < _countof(stru_5998F0); i++)
		{
			if (stru_5998F0[i].sfxdata_flags_2 & 8)
			{
				struct_5* v3 = &stru_6D1270[stru_5998F0[i].sfx_id];
				SfxData sfx_id = stru_5998F0[i].sfx_id;
				v3->anonymous_1 = 81 - GetTickCount();
				if (!SFXData_SoundFile[sfx_id])
				{
					goto LABEL_16;
				}
				char buff[MAX_PATH];
				sprintf_s(buff, "sound\\%s", SFXData_SoundFile[sfx_id]);
				location = (void*)fastFileRead(&bytes_read, 1, buff, 0, 1, "Starcraft\\SWAR\\lang\\snd.cpp", 737);
				if (location)
				{
					if (bytes_read)
					{
						WAVEFORMATEX wave_format;
						void* v4 = parseWaveFile((_DWORD*)location, (unsigned int*)&bytes_read, &wave_format);
						if (v4)
						{
							v3->sound_buffer_bytes = bytes_read;
							DSBUFFERDESC v18;
							v18.dwReserved = 0;
							v18.lpwfxFormat = &wave_format;
							v18.dwSize = 20;
							v18.dwFlags = 194;
							v18.dwBufferBytes = bytes_read;
							if (direct_sound->CreateSoundBuffer(&v18, &v3->sound_buffer, 0))
							{
								v3->sound_buffer = 0;
							}
							else if (v3->sound_buffer->Lock(0, bytes_read, &v20, &v21, 0, 0, 0))
							{
								v3->sound_buffer->Release();
								v3->sound_buffer = 0;
							}
							else
							{
								memcpy(v20, v4, v21);
								v3->sound_buffer->Unlock(v20, v21, 0, 0);
								if ((SFXData_Flags2[sfx_id] & 1) == 0)
								{
									v22 += bytes_read;
								}
								v3->anonymous_2 = sub_4BB890(v3);
								if (stru_5998F0[i].sfxdata_flags_2 & 8)
								{
									v3->sound_buffer->SetVolume(stru_5998F0[i].field_10 - dword_6D5A0C);
									v3->sound_buffer->SetPan(stru_5998F0[i].volume_related_maybe);
									v3->sound_buffer->Play(0, 0, 0);
									stru_5998F0[i].sound_buffer = v3->sound_buffer;
									stru_5998F0[i].sfxdata_flags_2 = SFXData_Flags2[sfx_id];
									stru_5998F0[i].sfxdata_flags_1 = SFXData_Flags1[sfx_id];
									stru_5998F0[i].field_8 = 0;
								}
							}
						}
					}

				LABEL_16:
					if (location)
					{
						SMemFree(location, "Starcraft\\SWAR\\lang\\snd.cpp", 823, 0);
						location = NULL;
					}
				}
				SFXData_Flags2[sfx_id] &= 0xF7u;
				stru_5998F0[i].sfxdata_flags_2 &= 0xF7u;
			}
		}
		if (v22)
		{
			EnterCriticalSection(&stru_6D5F4C);
			dword_6D59FC += v22;
			LeaveCriticalSection(&stru_6D5F4C);
		}
		WaitForSingleObject(dword_6D5A00, INFINITE);
	}

	return 0;
}

FAIL_STUB_PATCH(DSoundThread);

void BWFXN_DSoundDestroy_()
{
	if (direct_sound)
	{
		dword_6D5A08 = 0;
		if (sound_thread_handle)
		{
			SetEvent(dword_6D5A00);
			if (ThreadId != GetCurrentThreadId())
			{
				WaitForSingleObject(sound_thread_handle, INFINITE);
			}
			CloseHandle(sound_thread_handle);
			sound_thread_handle = 0;
		}
		if (dword_6D5A00)
		{
			CloseHandle(dword_6D5A00);
			dword_6D5A00 = 0;
		}
		if (dword_6D1268)
		{
			dword_6D1268->Release();
			dword_6D1268 = 0;
		}
		stopAllSound_();
		if (byte_6D1265)
		{
			byte_6D1265 = 0;
			SFileDdaDestroy();
		}
		if (byte_6D1266)
		{
			byte_6D1266 = 0;
			SVidDestroy();
		}
		dword_6D5A0C = 0;
		if (soundbuffer)
		{
			soundbuffer->Release();
			soundbuffer = 0;
		}
		if (direct_sound)
		{
			direct_sound->Release();
			direct_sound = 0;
		}
		byte_6D1264 = 0;
	}
}

FAIL_STUB_PATCH(BWFXN_DSoundDestroy);

BOOL DSoundInit_(AudioVideoInitializationError* a1, HWND a2)
{
	if (direct_sound)
	{
		return 1;
	}
	a1->error_code = 0;
	a1->function_name = 0;
	a1->dword4 = 138;
	memset(stru_5998F0, 0, sizeof(stru_5998F0));
	if (!DSoundCreate_(a1) || !SetCooperativeLevel_(a2, a1) || !CreateSoundBuffer_(a1))
	{
		BWFXN_DSoundDestroy_();
		return 0;
	}
	SetAudioFormat_();
	SFileDdaInitialize(direct_sound);
	byte_6D1265 = 1;
	SVidInitialize(direct_sound);
	byte_6D1266 = 1;
	LoadBtnSfxFile_();
	initVolume_();
	if (SRegLoadValue("Starcraft", "Sound Memory Cache", 0, &value))
	{
		if ((unsigned int)value < 0x100000)
		{
			value = 0x100000;
		}
	}
	else
	{
		value = 0x400000;
	}
	dword_6D59FC = 0;
	dword_6D5A00 = CreateEventA(0, 0, 0, 0);
	if (dword_6D5A00 == NULL)
	{
		BWFXN_DSoundDestroy_();
		return 0;
	}
	dword_6D5A08 = 1;
	sound_thread_handle = (HANDLE) _beginthreadex(0, 0, DSoundThread_, 0, 0, &ThreadId);
	if (sound_thread_handle == NULL)
	{
		BWFXN_DSoundDestroy_();
		return 0;
	}
	byte_6D1264 = 1;
	return 1;
}

FAIL_STUB_PATCH(DSoundInit);

void __fastcall saveColorSettings_(bool exit_code)
{
	if (!exit_code)
	{
		SRegSaveValue("Starcraft", "Gamma", 0, Gamma);
		SRegSaveValue("Starcraft", "ColorCycle", 0, ColorCycle);
		SRegSaveValue("Starcraft", "UnitPortraits", 0, UnitPortraits);
	}
}

FAIL_STUB_PATCH(saveColorSettings);

void loadColorSettings_()
{
	AppAddExit_(saveColorSettings_);

	if (SRegLoadValue("Starcraft", "Gamma", 0, &Gamma))
	{
		if ((unsigned int)Gamma > 0x8C || (unsigned int)Gamma < 0x3C)
		{
			Gamma = 100;
		}
	}

	if (SRegLoadValue("Starcraft", "ColorCycle", 0, &ColorCycle))
	{
		ColorCycle = (ColorCycle != 0);
	}
	else
	{
		ColorCycle = 1;
	}

	if (SRegLoadValue("Starcraft", "UnitPortraits", 0, &UnitPortraits))
	{
		UnitPortraits = min(UnitPortraits, 2);
	}
	else
	{
		UnitPortraits = 2;
	}

	if (dword_68AC98 && ActivePortraitUnit)
	{
		UnitType v2 = getLastQueueSlotType(ActivePortraitUnit);
		WORD v3 = setBuildingSelPortrait_(v2);
		displayUpdatePortrait(v3, ActivePortraitUnit, 1);
	}
}

FAIL_STUB_PATCH(loadColorSettings);

void __fastcall sfxdata_cleanup_(bool exit_code)
{
	// TODO: dynamically clean sfxdataDat memory
	delete[] sfxdataDat_;

	if (dword_5999B0)
	{
		SMemFree(dword_5999B0, "Starcraft\\SWAR\\lang\\snd.cpp", 1719, 0);
	}
	dword_5999B0 = 0;
}

FAIL_STUB_PATCH(sfxdata_cleanup);

void __fastcall j_BWFXN_DSoundDestroy_(bool exit_code)
{
	BWFXN_DSoundDestroy_();
}

FAIL_STUB_PATCH(j_BWFXN_DSoundDestroy);

void audioVideoInit_()
{
	loadColorSettings_();
	GameScreenBuffer.wid = SCREEN_WIDTH;
	GameScreenBuffer.ht = SCREEN_HEIGHT;
	GameScreenBuffer.data = 0;
	GameScreenBuffer.data = (u8 *)SMemAlloc(SCREEN_WIDTH * SCREEN_HEIGHT, "Starcraft\\SWAR\\lang\\gds\\vidinimo.cpp", 55, 0);
	BWFXN_DDrawInitialize_();
	dword_6D5DF8 = 1;
	AppAddExit_(vidinimoDestroy);
	memcpy(stru_6CEB40, &palette, sizeof(PALETTEENTRY[256]));
	BWFXN_RedrawTarget_();
	LoadSfx_();
	AppAddExit_(sfxdata_cleanup_);
	if (!byte_6D11D0)
	{
		AudioVideoInitializationError error;
		if (DSoundInit_(&error, hWndParent))
		{
			AppAddExit_(j_BWFXN_DSoundDestroy_);
		}
	}
}

FAIL_STUB_PATCH(audioVideoInit);

void InitializeDialog_(dialog *a1, FnInteract a2)
{
	dlgEvent event;

	a1->lFlags |= CTRL_DLG_ACTIVE | CTRL_VALIGN_BOTTOM;
	a1->fields.dlg.pModalFcn = 0;
	if (a2)
		a1->pfcnInteract = a2;
	size_t size = a1->fields.dlg.dstBits_wid * a1->fields.dlg.dstBits_ht;
	a1->fields.dlg.dstBits_data = (u8*)SMemAlloc(size, "Starcraft\\SWAR\\lang\\dlg\\dlg.cpp", 481, 0);
	a1->wIndex = 0;

	event.cursor.x = Mouse.x;
	event.cursor.y = Mouse.y;
	event.wNo = EventNo::EVN_USER;
	event.dwUser = EventUser::USER_UNK_7;
	event.wSelection = 0;
	event.wUnk_0x06 = 0;
	a1->pfcnInteract(a1, &event);

	a1->rct.right += a1->rct.left;
	a1->rct.bottom += a1->rct.top;

	event.cursor.x = Mouse.x;
	event.cursor.y = Mouse.y;
	event.wNo = EventNo::EVN_USER;
	event.dwUser = EventUser::USER_INIT;
	event.wSelection = 0;
	event.wUnk_0x06 = 0;
	a1->pfcnInteract(a1, &event);

	event.cursor.x = Mouse.x;
	event.cursor.y = Mouse.y;
	event.wNo = EventNo::EVN_USER;
	event.dwUser = EventUser::USER_CREATE;
	event.wSelection = 0;
	event.wUnk_0x06 = 0;
	a1->pfcnInteract(a1, &event);

	SetCallbackTimer(12, a1, 100, 0);
	a1->lFlags &= ~CTRL_VALIGN_BOTTOM;
}

FAIL_STUB_PATCH(InitializeDialog);

void __fastcall BWFXN_OpenGameDialog_(char* a1, FnInteract a2)
{
	if (!multiPlayerMode)
	{
		TickCountSomething(1);
	}
	if (GameMenuDlg)
	{
		DestroyDialog(GameMenuDlg);
		GameMenuDlg = 0;
		if (gwGameMode == GAME_RUN)
		{
			hAccTable = DlgAccelerator;
			input_procedures[EventNo::EVN_SYSCHAR] = AcceleratorTables;
		}
		byte_6D1214 = 0;
	}
	sub_4195E0();
	if (byte_66FF5C)
	{
		BWFXN_RefreshTarget(stru_66FF50.left, stru_66FF50.bottom, stru_66FF50.top, stru_66FF50.right);
		byte_66FF5C = 0;
		SetInGameInputProcs_();
	}
	setCursorType_(CursorType::CUR_ARROW);
	if (!multiPlayerMode)
	{
		PauseGame_maybe();
	}
	if (gwGameMode == GAME_RUN)
	{
		if (is_placing_building)
		{
			refreshLayer3And4();
			refreshPlaceBuildingLocation();
		}
		if (is_placing_order)
		{
			CancelTargetOrder();
		}
	}
	byte_6D1214 = 1;
	if (gwGameMode == GAME_RUN)
	{
		DlgAccelerator = hAccTable;
		AcceleratorTables = input_procedures[EventNo::EVN_SYSCHAR];
	}
	hAccTable = dword_5968F8;
	input_procedures[EventNo::EVN_SYSCHAR] = input_standardSysHotkeys;
	refreshSelectionScreen_();

	GameMenuDlg = (dialog*)fastFileRead(0, 0, a1, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (GameMenuDlg)
	{
		GameMenuDlg->lFlags |= CTRL_ACTIVE;
		AllocInitDialogData(GameMenuDlg, GameMenuDlg, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\modeless.cpp", 63);
	}

	InitializeDialog_(GameMenuDlg, a2);
}

FUNCTION_PATCH(BWFXN_OpenGameDialog, BWFXN_OpenGameDialog_);

void __fastcall BWFXN_QuitMission_(dialog* dlg);

void video_OK_(dialog* dlg)
{
	if (LastControlID != -2)
	{
		ColorCycle = dword_656188;
		Gamma = (unsigned int)dword_656190 > 0x8C || (unsigned int)dword_656190 < 0x3C ? 100 : dword_656190;
		updatePaletteEntries();
		UnitPortraits = (unsigned int)dword_65618C > 2 ? 2 : dword_65618C;
		if (dword_68AC98 && ActivePortraitUnit)
		{
			UnitType last_queue_slot_type = getLastQueueSlotType(ActivePortraitUnit);
			u16 v6 = setBuildingSelPortrait_(last_queue_slot_type);
			displayUpdatePortrait(v6, ActivePortraitUnit, 1);
		}
	}
	sub_4C9780(dlg);
	char v7 = --byte_6D1224;
	if (byte_6D1224)
	{
		byte_6D1224 = v7 - 1;
		dword_6D1234 = MainMenuOptionsCustomInteract;
		BWFXN_OpenGameDialog("rez\\options.bin", gamemenu_Dlg_Interact);
	}
	else
	{
		DestroyDialog(dlg);
	}
}

FAIL_STUB_PATCH(video_OK);

void sub_480B90_(dialog* dlg)
{
	UnitPortraits = dlg->wIndex > 5 ? 2 : dlg->wIndex - 3;

	if (dword_68AC98 && ActivePortraitUnit)
	{
		UnitType last_queue_slot_type = getLastQueueSlotType(ActivePortraitUnit);
		u16 portrait_id = setBuildingSelPortrait_(last_queue_slot_type);
		displayUpdatePortrait(portrait_id, ActivePortraitUnit, 1);
	}
}

FAIL_STUB_PATCH(sub_480B90);

int __fastcall video_PortraitRadioBtns_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_ACTIVATE:
			sub_480B90_(dlg);
			break;
		case EventUser::USER_INIT:
			sub_480AE0(dlg);
			break;
		}
	}
	return genericOptionInteract(dlg, evt);
}

FAIL_STUB_PATCH(video_PortraitRadioBtns);

void video_CustomCTRLID_(dialog* a1)
{
	static FnInteract functions[] = {
		video_GammaSlider,
		video_CCyclingCheckbox,
		video_PortraitRadioBtns_,
		video_PortraitRadioBtns_,
		video_PortraitRadioBtns_,
	};

	registerUserDialogAction(a1, sizeof(functions), functions);
}

FAIL_STUB_PATCH(video_CustomCTRLID);

int __fastcall video_BINDLG_Main_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_IDLE:
		sub_480B30(dlg, dlg);
		break;
	case EventNo::EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			video_Main(dlg);
			break;
		case USER_DESTROY:
			video_Cancel(dlg, evt);
			return 1;
		case USER_ACTIVATE:
			video_OK_(dlg);
			return 1;
		case USER_INIT:
			video_CustomCTRLID_(dlg);
			break;
		}
	}

	switch (evt->wNo)
	{
	case EventNo::EVN_KEYFIRST:
	case EventNo::EVN_KEYRPT:
	case EventNo::EVN_MOUSEMOVE:
	case EventNo::EVN_LBUTTONDOWN:
	case EventNo::EVN_LBUTTONUP:
	case EventNo::EVN_LBUTTONDBLCLK:
	case EventNo::EVN_RBUTTONDOWN:
	case EventNo::EVN_RBUTTONUP:
	case EventNo::EVN_RBUTTONDBLCLK:
	case EventNo::EVN_CHAR:
		genericDlgInteract(dlg, evt);
		return 1;
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(video_BINDLG_Main);

void __cdecl sub_481060_()
{
	dword_656190 = Gamma;
	dword_65618C = UnitPortraits;
	dword_656188 = ColorCycle;
	BWFXN_OpenGameDialog("rez\\video.bin", video_BINDLG_Main_);
}

FUNCTION_PATCH(sub_481060, sub_481060_);

char gamemenu_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] =
	{
		gameMenu_DLG,
		gameMenu_DLG,
		gameMenu_DLG,
		gameMenu_DLG,
		gameMenu_DLG,
		gameMenu_DLG,
		gameMenu_DLG,
		gameMenu_DLG,
	};

	sub_4C9CC0(dlg, 0);
	sub_4C90C0(dlg);
	registerUserDialogAction(dlg, sizeof(functions), functions);
	UpdateOKButton(dlg, 4, CTRL_USELOCALGRAPHIC);
	UpdateCancelButton(dlg, 4, CTRL_USELOCALGRAPHIC);
	if (dword_6D1234 == gameMenu_BINDLG)
	{
		sub_4C9440(dlg);
	}
	if (dword_6D1234 == sub_4CA450 && (multiPlayerMode || InReplay))
	{
		checkSaveGameDialog(dlg);
	}
	if (dword_6D1234 == BWFXN_QuitMission_)
	{
		dialog* v3 = getControlFromIndex_(dlg, 1);
		HideDialog_(v3);
	}
	if (dword_6D1234 == MainMenuOptionsCustomInteract)
	{
		sub_4C94F0(dlg);
	}
	return ++byte_6D1224;
}

FAIL_STUB_PATCH(gamemenu_CustomCtrlID);

int __fastcall gamemenu_Dlg_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gamemenu_CustomCtrlID_(dlg);
			break;
		case EventUser::USER_DESTROY:
			options_Cancel(dlg, evt);
			return 1;
		case EventUser::USER_ACTIVATE:
			dword_6D1234(dlg);
			return 1;
		default:
			break;
		}
	case EventNo::EVN_KEYFIRST:
	case EventNo::EVN_KEYRPT:
	case EventNo::EVN_MOUSEMOVE:
	case EventNo::EVN_LBUTTONDOWN:
	case EventNo::EVN_LBUTTONUP:
	case EventNo::EVN_LBUTTONDBLCLK:
	case EventNo::EVN_RBUTTONDOWN:
	case EventNo::EVN_RBUTTONUP:
	case EventNo::EVN_RBUTTONDBLCLK:
	case EventNo::EVN_CHAR:
		genericDlgInteract(dlg, evt);
		return 1;
	}

	return genericDlgInteract(dlg, evt);
}

FUNCTION_PATCH(gamemenu_Dlg_Interact, gamemenu_Dlg_Interact_);

void __fastcall BWFXN_QuitReplay_maybe_(dialog* dlg)
{
	dword_6D1234 = 0;
	if (LastControlID == -2)
	{
		GameState = 0;
		if (multiPlayerMode)
		{
			gwNextGameMode = byte_58D700[g_LocalNationID] == 3 ? GamePosition::GAME_WIN : GamePosition::GAME_LOSE;
		}
		else
		{
			gwNextGameMode = GamePosition::GAME_GLUES;
			glGluesMode = IsExpansion ? MenuPosition::GLUE_EX_CAMPAIGN : MenuPosition::GLUE_CAMPAIGN;
		}

		if (!InReplay)
		{
			replay_header.ReplayFrames = ElapsedTimeFrames;
		}
		DestroyDialog(dlg);
	}
	else if (--byte_6D1224 == 0)
	{
		DestroyDialog(dlg);
	}
	else
	{
		byte_6D1224 -= 1;
		dword_6D1234 = gameMenu_BINDLG;
		BWFXN_OpenGameDialog_("rez\\abrtmenu.bin", gamemenu_Dlg_Interact_);
	}
}

FAIL_STUB_PATCH(BWFXN_QuitReplay_maybe);

void sub_460F70_();

void __fastcall BWFXN_QuitMission_(dialog* dlg)
{
	dword_6D1234 = 0;
	if (LastControlID == -2)
	{
		sub_460F70_();

		GameState = 0;
		if (multiPlayerMode)
		{
			gwNextGameMode = byte_58D700[g_LocalNationID] == 3 ? GamePosition::GAME_WIN : GamePosition::GAME_LOSE;
		}
		else
		{
			gwNextGameMode = GamePosition::GAME_GLUES;
			glGluesMode = IsExpansion ? MenuPosition::GLUE_EX_CAMPAIGN : MenuPosition::GLUE_CAMPAIGN;
		}

		if (!InReplay)
		{
			replay_header.ReplayFrames = ElapsedTimeFrames;
		}
		DestroyDialog(dlg);
	}
	else if (LastControlID == 1)
	{
		DestroyDialog(dlg);
		byte_6D1224 = 0;
	}
	else if (--byte_6D1224 == 0)
	{
		DestroyDialog(dlg);
	}
	else
	{
		byte_6D1224 -= 1;
		dword_6D1234 = gameMenu_BINDLG;
		BWFXN_OpenGameDialog_("rez\\abrtmenu.bin", gamemenu_Dlg_Interact_);
	}
}

FAIL_STUB_PATCH(BWFXN_QuitMission);

void QuitMissionMenu_()
{
	if (InReplay)
	{
		dword_6D1234 = BWFXN_QuitReplay_maybe_;
		BWFXN_OpenGameDialog_("rez\\quitrepl.bin", gamemenu_Dlg_Interact_);
	}
	else
	{
		dword_6D1234 = BWFXN_QuitMission_;
		BWFXN_OpenGameDialog_("rez\\quit2mnu.bin", gamemenu_Dlg_Interact_);
	}
}

FUNCTION_PATCH(QuitMissionMenu, QuitMissionMenu_);

void TitlePaletteUpdate_(int a1)
{
	if (has_viewport)
	{
		PALETTEENTRY* v1;
		PALETTEENTRY a2[256];

		if (byte_51A0E9)
		{
			memset(GamePalette, 0, sizeof(GamePalette));
			byte_51A0E9 = 0;
			v1 = GamePalette;
			if (Gamma != 100)
			{
				sub_41DC20(GamePalette, a2, 256);
				v1 = a2;
			}
			SDrawUpdatePalette(0, 0x100u, v1, 1);
			BWFXN_RedrawTarget_();
			memset(stru_6CE720, 0, sizeof(stru_6CE720));
			gluDlgFadePalette(a1);
		}
	}
}

FAIL_STUB_PATCH(TitlePaletteUpdate);



void updateImageDirection_(CImage* image, u8 direction)
{
	if (image->flags & ImageFlags::IF_HAS_DIRECTIONAL_FRAMES)
	{
		ImageFlags new_flip_flag = (ImageFlags)0;
		int new_direction = direction;

		if (new_direction > 16)
		{
			new_direction = 32 - new_direction;
			new_flip_flag = ImageFlags::IF_HORIZONTALLY_FLIPPED;
		}

		if ((image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) != new_flip_flag || image->direction != new_direction)
		{
			image->direction = new_direction;
			image->flags &= ~ImageFlags::IF_HORIZONTALLY_FLIPPED;
			image->flags |= new_flip_flag;
			setImagePaletteType(image, image->paletteType);

			if (image->frameIndex != image->direction + image->frameSet)
			{
				image->flags |= ImageFlags::IF_REDRAW;
				image->frameIndex = image->direction + image->frameSet;
			}

			if (image->flags & ImageFlags::IF_USES_SPECIAL_OFFSET)
			{
				updateImagePositionOffset(image);
			}
		}
	}
}

void __stdcall updateImageDirection__(u8 direction)
{
	CImage* image;

	__asm mov image, eax

	updateImageDirection_(image, direction);
}

FUNCTION_PATCH((void*)0x4D5EA0, updateImageDirection__);

void setImageDirection_(CImage* image, unsigned __int8 direction)
{
	if (image->flags & ImageFlags::IF_USES_SPECIAL_OFFSET)
	{
		updateImagePositionOffset(image);
	}

	if (image->flags & ImageFlags::IF_HAS_DIRECTIONAL_FRAMES)
	{
		int new_direction = (direction + 4) / 8;
		ImageFlags new_flip_flag = (ImageFlags)0;

		if (new_direction > 16)
		{
			new_flip_flag = ImageFlags::IF_HORIZONTALLY_FLIPPED;
			new_direction = 32 - new_direction;
		}

		if (image->direction != new_direction || (image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) != new_flip_flag)
		{
			image->direction = new_direction;
			image->flags &= ~IF_HORIZONTALLY_FLIPPED;
			image->flags |= new_flip_flag;
			setImagePaletteType(image, image->paletteType);

			if (image->frameIndex != image->frameSet + image->direction)
			{
				image->flags |= IF_REDRAW;
				image->frameIndex = image->frameSet + image->direction;
			}
		}
	}
}

void __stdcall setImageDirection__(u8 direction)
{
	CImage* image;

	__asm mov image, esi

	setImageDirection_(image, direction);
}

FUNCTION_PATCH((void*)0x4D5F80, setImageDirection__);

void InitializeImageData_(CImage* image, CSprite* sprite, int image_id, __int8 horizontal_offset, __int8 vertical_offset)
{
	image->imageID = image_id;
	image->GRPFile = ImageGrpGraphics[image_id];
	image->flags = (ImageFlags)0;
	if (Images_IsTurnable[image_id] & 1)
	{
		image->flags |= ImageFlags::IF_HAS_DIRECTIONAL_FRAMES;
	}
	if (Images_IsClickable[image_id] & 1)
	{
		image->flags |= ImageFlags::IF_CLICKABLE;
	}
	image->frameSet = 0;
	image->direction = 0;
	image->frameIndex = 0;
	image->iscript_program.anim = Anims::AE_Init;
	image->spriteOwner = sprite;
	image->horizontalOffset = horizontal_offset;
	image->verticalOffset = vertical_offset;
	memset(&image->grpBounds, 0, sizeof(image->grpBounds));
	image->coloringData = 0;
	memset(&image->iscript_program, 0, sizeof(image->iscript_program));

	if (Image_DrawFunction[image->imageID] == 14)
	{
		image->coloringData = (void*)sprite->playerID;
	}
	if (Image_DrawFunction[image_id] == 9)
	{
		image->coloringData = colorShift[Images_Remapping[image_id]].data;
	}
}

void __stdcall InitializeImageData__(__int8 horizontal_offset, __int8 vertical_offset)
{
	CImage* image;
	CSprite* sprite;
	int image_id;

	__asm mov image, eax
	__asm mov sprite, edi
	__asm mov image_id, esi

	InitializeImageData_(image, sprite, image_id, horizontal_offset, vertical_offset);
}

FUNCTION_PATCH((void*)0x4D5A50, InitializeImageData__);

void LoadImageData_()
{
	OVERLAPPED a5[999];
	_DWORD a6[999];

	u16* images_tbl = (u16*) fastFileRead_(0, 0, "arr\\images.tbl", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	dword_6D1210 = low_memory;
	loadImagesData_lowMem(ImageGrpGraphics, Images_GrpFile, images_tbl, 999, a5, a6);
	dword_6D1210 = 0;

	loadImagesData(lo_files.overlays[OverlayType::OT_ATTACK], Images_AttackOverlayLO, images_tbl, 999, a5, a6);
	loadImagesData(lo_files.overlays[OverlayType::OT_DAMAGE], Images_InjuryOverlayLO, images_tbl, 999, a5, a6);
	loadImagesData(lo_files.overlays[OverlayType::OT_SPECIAL], Images_SpecialOverlayLO, images_tbl, 999, a5, a6);
	loadImagesData(lo_files.overlays[OverlayType::OT_LANDING], Images_LandingDustLO, images_tbl, 999, a5, a6);
	loadImagesData(lo_files.overlays[OverlayType::OT_LIFTOFF], Images_LiftOffDustLO, images_tbl, 999, a5, a6);
	loadImagesData(ShieldOverlays, Images_ShieldOverlayLO, images_tbl, 999, a5, a6);
	if (images_tbl)
	{
		SMemFree(images_tbl, "Starcraft\\SWAR\\lang\\CImage.cpp", 1389, 0);
	}
}

FAIL_STUB_PATCH(LoadImageData);

DatLoad* imagesDat_;

void LoadInitIscriptBIN_()
{
	int iscript_bin_size;
	iscript_data = (IScript*) fastFileRead_(&iscript_bin_size, 0, "scripts\\iscript.bin", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	iscript_id_to_pc.clear();
	unsigned __int16* v2 = (unsigned __int16*)((char*)iscript_data + iscript_data->size_maybe);
	while (v2[0] != 0xFFFF)
	{
		iscript_id_to_pc[v2[0]] = v2[1];
		v2 += 2;
	}

	// TODO: dynamically allocate imagesDat memory
	imagesDat_ = new DatLoad[] {
		DatLoad { Images_GrpFile, 4, 999 },
		DatLoad { Images_IsTurnable, 1, 999 },
		DatLoad { Images_IsClickable, 1, 999 },
		DatLoad { Images_UseFullIscript, 1, 999 },
		DatLoad { Image_DrawIfCloaked, 1, 999 },
		DatLoad { Image_DrawFunction, 1, 999 },
		DatLoad { Images_Remapping, 1, 999 },
		DatLoad { Images_IscriptEntry, 4, 999 },
		DatLoad { Images_ShieldOverlayLO, 4, 999 },
		DatLoad { Images_AttackOverlayLO, 4, 999 },
		DatLoad { Images_InjuryOverlayLO, 4, 999 },
		DatLoad { Images_SpecialOverlayLO, 4, 999 },
		DatLoad { Images_LandingDustLO, 4, 999 },
		DatLoad { Images_LiftOffDustLO, 4, 999 },
	};
	LoadGameData_(imagesDat_, "arr\\images.dat");
	if (!SBmpLoadImage("game\\tselect.pcx", 0, dword_5240B8, 24, 0, 0, 0))
	{
		throw FileNotFoundException("game\\tselect.pcx", SErrGetLastError());
	}
	LoadImageData_();
	InitDamageOverlayCounts();
}

FAIL_STUB_PATCH(LoadInitIscriptBIN);

void __fastcall CleanupIscriptBINHandle_(bool exit_code)
{
	if (iscript_data)
	{
		SMemFree(iscript_data, "Starcraft\\SWAR\\lang\\CImage.cpp", 1626, 0);
		iscript_data = NULL;
	}

	sub_47AB40((LO_Header**)ImageGrpGraphics, 999);
	sub_47AB40(lo_files.overlays[OverlayType::OT_ATTACK], 999);
	sub_47AB40(lo_files.overlays[OverlayType::OT_DAMAGE], 999);
	sub_47AB40(lo_files.overlays[OverlayType::OT_SPECIAL], 999);
	sub_47AB40(lo_files.overlays[OverlayType::OT_LANDING], 999);
	sub_47AB40(lo_files.overlays[OverlayType::OT_LIFTOFF], 999);
	sub_47AB40(ShieldOverlays, 999);

	// TODO: dynamically clean imagesDat memory
	delete[] imagesDat_;
}

FAIL_STUB_PATCH(CleanupIscriptBINHandle);

void CreateInitialMeleeBuildings_(RaceId race, u8 player_index)
{
	UnitType base_building = Race::races()[race].base_building;

	// TODO: test this piece of code against the one in StarCraft.exe:
	int v5 = Unit_Placement[base_building].x;
	int v6 = Unit_Placement[base_building].y;
	int v7 = (v5 - HIWORD(v5)) / 2;
	int v8 = (v6 - HIWORD(v6)) / 2;
	int v9 = v7 + ((startPositions[player_index].x - v7) & 0xFFE0);
	int v10 = v8 + ((startPositions[player_index].y - v8) & 0xFFE0);

	Box16 v14;
	v14.left = v9 - v7;
	v14.top = v10 - v6 / 2;
	v14.bottom = v6 / 2 + v10 - 1;
	v14.right = v7 + v9 - 1;
	ModifyUnit_maybe(&v14, 0, (int(__fastcall*)(CUnit*, void*))SelfDestructCB);

	CUnit* v12 = CreateUnit(base_building, v9, v10, player_index);
	if (v12)
	{
		updateUnitStatsFinishBuilding(v12);
		if (sub_49EC30(v12))
		{
			updateUnitStrengthAndApplyDefaultOrders(v12);
		}
		if (spreadsCreep(v12->unitType, 1) || (Unit_PrototypeFlags[v12->unitType] & CreepBuilding) != 0)
		{
			ApplyCreepAtLocationFromUnitType(v12->unitType, v12->sprite->position.x, v12->sprite->position.y);
			sub_49D660(v12);
			sub_49D660(v12);
		}
	}
}

FAIL_STUB_PATCH(CreateInitialMeleeBuildings);

void CreateInitialMeleeWorker_(RaceId race, unsigned __int8 player_index)
{
	CUnit* worker = CreateUnit(Race::races()[race].worker, startPositions[player_index].x, startPositions[player_index].y, player_index);
	if (worker)
	{
		updateUnitStatsFinishBuilding(worker);
		if (sub_49EC30(worker))
		{
			updateUnitStrengthAndApplyDefaultOrders(worker);
		}
	}
}

FAIL_STUB_PATCH(CreateInitialMeleeWorker);

void CreateInitialMeleeUnits_()
{
	for (int player_index = 7; player_index >= 0; player_index--)
	{
		PlayerInfo* player = Players + player_index;
		if (player->nType == PlayerType::PT_Human || player->nType == PlayerType::PT_Computer)
		{
			StartingUnits starting_units;
			if (gameData.got_file_values.victory_conditions == VictoryConditions::VC_MAP_DEFAULT
				&& gameData.got_file_values.starting_units == StartingUnits::SU_MAP_DEFAULT
				&& !gameData.got_file_values.tournament_mode
				&& player_index < 8
				&& playerForce[player_index])
			{
				starting_units = StartingUnits::SU_WORKER_AND_CENTER;
			}
			else
			{
				starting_units = gameData.got_file_values.starting_units;
			}

			switch (starting_units)
			{
			case StartingUnits::SU_WORKER_AND_CENTER:
				CreateInitialMeleeBuildings_(player->nRace, player_index);
				if (player->nRace == RaceId::RACE_Zerg)
					CreateInitialOverlord(player_index);
				[[fallthrough]];
			case StartingUnits::SU_WORKER_ONLY:
				CreateInitialMeleeWorker_(player->nRace, player_index);
				CreateInitialMeleeWorker_(player->nRace, player_index);
				CreateInitialMeleeWorker_(player->nRace, player_index);
				CreateInitialMeleeWorker_(player->nRace, player_index);
				[[fallthrough]];
			case StartingUnits::SU_MAP_DEFAULT:
				break;
			}
		}
	}
}

FAIL_STUB_PATCH(CreateInitialMeleeUnits);

bool __stdcall ChkLoader_TYPE_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_VER_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_DIM_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_VCOD_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_ERA_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_STR_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_MBRF_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_SPRP_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_FORC_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_MTXM_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_THG2_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_UNIT_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_UPRP_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_COLR_(SectionData* section_data, int section_size, MapChunks* a3);
bool __stdcall ChkLoader_MASK_(SectionData* section_data, int section_size, MapChunks* a3);

ChkSectionLoader CreateChkSectionLoader(const char(&section_name)[5], bool(__stdcall* func)(SectionData*, int, MapChunks*), int flags)
{
	return {
		{section_name[0], section_name[1], section_name[2], section_name[3]},
		func,
		flags,
	};
}

ChkSectionLoader chk_loaders_version_[] = {
	CreateChkSectionLoader("TYPE", ChkLoader_TYPE_, 0),
	CreateChkSectionLoader("VER ", ChkLoader_VER_, 1),
};

ChkSectionLoader chk_loaders_lobby_[] = {
	CreateChkSectionLoader("VER ", ChkLoader_VER_, 1),
	CreateChkSectionLoader("DIM ", ChkLoader_DIM_, 1),
	CreateChkSectionLoader("ERA ", ChkLoader_ERA_, 1),
	CreateChkSectionLoader("OWNR", ChkLoader_OWNR, 1),
	CreateChkSectionLoader("SIDE", ChkLoader_SIDE, 1),
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("SPRP", ChkLoader_SPRP_, 1),
	CreateChkSectionLoader("FORC", ChkLoader_FORC_, 1),
	CreateChkSectionLoader("VCOD", ChkLoader_VCOD_, 1),
};

ChkSectionLoader chk_loaders_briefing_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MBRF", ChkLoader_MBRF_, 1),
};

ChkSectionLoader chk_loaders_melee_vanilla_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT_, 1),
};

ChkSectionLoader chk_loaders_melee_broodwar_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT_, 1),
	CreateChkSectionLoader("COLR", ChkLoader_COLR_, 1),
};

ChkSectionLoader chk_loaders_ums_1_00_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("MASK", ChkLoader_MASK_, 1),
	CreateChkSectionLoader("UNIS", ChkLoader_UNIS, 1),
	CreateChkSectionLoader("UPGS", ChkLoader_UPGS, 1),
	CreateChkSectionLoader("TECS", ChkLoader_TECS, 1),
	CreateChkSectionLoader("PUNI", ChkLoader_PUNI, 1),
	CreateChkSectionLoader("UPGR", ChkLoader_UPGR, 1),
	CreateChkSectionLoader("PTEC", ChkLoader_PTEC, 1),
	CreateChkSectionLoader("UNIx", ChkLoader_UNIx, 0),
	CreateChkSectionLoader("UPGx", ChkLoader_UPGx, 0),
	CreateChkSectionLoader("TECx", ChkLoader_TECx, 0),
	CreateChkSectionLoader("PUPx", ChkLoader_PUPx, 0),
	CreateChkSectionLoader("PTEx", ChkLoader_PTEx, 0),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT_, 1),
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP_, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN_1_00, 1),
	CreateChkSectionLoader("TRIG", ChkLoader_TRIG, 1),
};

ChkSectionLoader chk_loaders_ums_1_04_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("MASK", ChkLoader_MASK_, 1),
	CreateChkSectionLoader("UNIS", ChkLoader_UNIS, 1),
	CreateChkSectionLoader("UPGS", ChkLoader_UPGS, 1),
	CreateChkSectionLoader("TECS", ChkLoader_TECS, 1),
	CreateChkSectionLoader("PUNI", ChkLoader_PUNI, 1),
	CreateChkSectionLoader("UPGR", ChkLoader_UPGR, 1),
	CreateChkSectionLoader("PTEC", ChkLoader_PTEC, 1),
	CreateChkSectionLoader("UNIx", ChkLoader_UNIx, 0),
	CreateChkSectionLoader("UPGx", ChkLoader_UPGx, 0),
	CreateChkSectionLoader("TECx", ChkLoader_TECx, 0),
	CreateChkSectionLoader("PUPx", ChkLoader_PUPx, 0),
	CreateChkSectionLoader("PTEx", ChkLoader_PTEx, 0),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT_, 1),
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP_, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN, 1),
	CreateChkSectionLoader("TRIG", ChkLoader_TRIG, 1),
};

ChkSectionLoader chk_loaders_ums_broodwar_1_04_[] = {
	CreateChkSectionLoader("STR ", ChkLoader_STR_, 1),
	CreateChkSectionLoader("MTXM", ChkLoader_MTXM_, 1),
	CreateChkSectionLoader("THG2", ChkLoader_THG2_, 1),
	CreateChkSectionLoader("MASK", ChkLoader_MASK_, 1),
	CreateChkSectionLoader("UNIx", ChkLoader_UNIx, 1),
	CreateChkSectionLoader("UPGx", ChkLoader_UPGx, 1),
	CreateChkSectionLoader("TECx", ChkLoader_TECx, 1),
	CreateChkSectionLoader("PUNI", ChkLoader_PUNI, 1),
	CreateChkSectionLoader("PUPx", ChkLoader_PUPx, 1),
	CreateChkSectionLoader("PTEx", ChkLoader_PTEx, 1),
	CreateChkSectionLoader("UNIT", ChkLoader_UNIT_, 1),
	CreateChkSectionLoader("UPRP", ChkLoader_UPRP_, 1),
	CreateChkSectionLoader("MRGN", ChkLoader_MRGN, 1),
	CreateChkSectionLoader("TRIG", ChkLoader_TRIG, 1),
	CreateChkSectionLoader("COLR", ChkLoader_COLR_, 1),
};

ChkLoader chk_loaders_[] = {
	// 17 - Warcraft II retail
	// 19 - Warcraft II Expansion
	// 47 - Starcraft Beta
	// 57 - Starcraft Prerelease
	{
		// Loaders for Starcraft 1.00
		59,
		chk_loaders_lobby_, _countof(chk_loaders_lobby_),
		chk_loaders_briefing_, _countof(chk_loaders_briefing_),
		chk_loaders_melee_vanilla_, _countof(chk_loaders_melee_vanilla_),
		chk_loaders_ums_1_00_, _countof(chk_loaders_ums_1_00_),
		0
	},
	// 61 - Brood War internal (same as 63)
	{
		// Loaders for Starcraft 1.04
		63,
		chk_loaders_lobby_, _countof(chk_loaders_lobby_),
		chk_loaders_briefing_, _countof(chk_loaders_briefing_),
		chk_loaders_melee_vanilla_, _countof(chk_loaders_melee_vanilla_),
		chk_loaders_ums_1_04_, _countof(chk_loaders_ums_1_04_),
		0
	},
	// 64 - Starcraft Remastered (1.21) (hybrid)
	// 75 - Broodwar Battle.net Beta
	// 201 - Brood War internal (same as 59 except it uses ver 63's MRGN; has (optional) expansion sections in its ver 59)
	// 203 - Brood War internal (same as 205)
	{
		// Loaders for Brood War 1.04
		205,
		chk_loaders_lobby_, _countof(chk_loaders_lobby_),
		chk_loaders_briefing_, _countof(chk_loaders_briefing_),
		chk_loaders_melee_broodwar_, _countof(chk_loaders_melee_broodwar_),
		chk_loaders_ums_broodwar_1_04_, _countof(chk_loaders_ums_broodwar_1_04_),
		1
	},
	// 206 - Brood War Remastered (1.21)
};

MEMORY_PATCH(0x4BF65A, &chk_loaders_->lobby_loader_count);
MEMORY_PATCH(0x4BF660, &chk_loaders_->lobby_loaders);
MEMORY_PATCH(0x4CC0CD, chk_loaders_);
MEMORY_PATCH(0x4CC0E1, &chk_loaders_->requires_expansion);
MEMORY_PATCH(0x4CCA8A, &chk_loaders_->briefing_loader_count);
MEMORY_PATCH(0x4CCA90, &chk_loaders_->briefing_loaders);
MEMORY_PATCH(0x4CCBFC, &chk_loaders_->lobby_loader_count);
MEMORY_PATCH(0x4CCC02, &chk_loaders_->lobby_loaders);

int sub_413550_(ChkSectionLoader* loader, ChunkNode* a2, int a3, MapChunks* a4)
{
	ChunkData* v6;

	int v4 = 0;
	if (a3 <= 0)
	{
		return 1;
	}
	else
	{
		while (1)
		{
			v6 = a2->f2;
			if (v6 > NULL)
				break;
		LABEL_8:
			++v4;
			if (v4 >= a3)
			{
				return 1;
			}
		}
		while (1)
		{
			if (v6->section_data.chunk_name.as_number == *(DWORD*)loader[v4].name)
			{
				if (loader[v4].func && !loader[v4].func(&v6->section_data, v6->section_data.size, a4))
				{
					break;
				}
			}
			v6 = v6->next;
			if ((signed int)v6 <= 0)
				goto LABEL_8;
		}
		return 0;
	}
}

FAIL_STUB_PATCH(sub_413550);

signed int ReadChunkNodes_(int chk_section_loader_count, int a2, ChkSectionLoader* chk_section_loader, void* chk_data, MapChunks* a4)
{
	ChunkNode v8;

	v8.f1 = (ChunkData*)&v8.f1;
	v8.count = 0;
	v8.f2 = (ChunkData*)~(unsigned int)&v8.f1;
	sub_413670((Chunk*)chk_data, &v8, a2, ChunkNode_Constructor);
	if (sub_4135C0(chk_section_loader, &v8, chk_section_loader_count))
	{
		if (sub_413550_(chk_section_loader, &v8, chk_section_loader_count, a4))
		{
			ChunkNode_Destructor(&v8);
			sub_404B20(&v8);
			return 1;
		}
		else
		{
			ChunkNode_Destructor(&v8);
			sub_404B20(&v8);
			return 0;
		}
	}
	else
	{
		ChunkNode_Destructor(&v8);
		_list_unlink((ListNode*)&v8);
		if (v8.f1)
		{
			if ((signed int)v8.f2 <= 0)
			{
				*(_DWORD*)~(unsigned int)v8.f2 = (DWORD)v8.f1;
				v8.f1->next = v8.f2;
				return 0;
			}
			*(ChunkData**)((char*)&v8.f2->previous
				+ (int)&v8.f1
				- (int)v8.f1->next) = v8.f1;
			v8.f1->next = v8.f2;
		}
		return 0;
	}
}

FAIL_STUB_PATCH(ReadChunkNodes);

int ReadMapChunks_(MapChunks* a1, void* chk_data, int* out_version_loader_index, int chk_size)
{
	if (out_version_loader_index && chk_data)
	{
		MapChunks location;
		SMemZero(&location, sizeof(location));

		*out_version_loader_index = 0;

		if (!a1)
		{
			a1 = &location;
		}
		if (ReadChunkNodes_(_countof(chk_loaders_version_), chk_size, chk_loaders_version_, chk_data, a1))
		{
			for (int i = 0; i < _countof(chk_loaders_); i++)
			{
				if (chk_loaders_[i].version == a1->version)
				{
					if (!chk_loaders_[i].requires_expansion || IsExpansion)
					{
						*out_version_loader_index = i;
						return 1;
					}
					return 0;
				}
			}
		}
	}
	else
	{
		SErrSetLastError(0x57u);
	}
	return 0;
}

FAIL_STUB_PATCH(ReadMapChunks);

int ReadLobbyChunks(void* chk_data, int chk_size, MapChunks* a4)
{
	int loader_index = 0;
	if (!ReadMapChunks_(a4, chk_data, &loader_index, chk_size))
	{
		return 0;
	}

	return ReadChunkNodes_(chk_loaders_[loader_index].lobby_loader_count, chk_size, chk_loaders_[loader_index].lobby_loaders, chk_data, a4);
}

int ReadGameChunks(void* chk_data, int chk_size)
{
	int loader_index = 0;
	if (!ReadMapChunks_(0, chk_data, &loader_index, chk_size))
	{
		return 0;
	}

	ChkSectionLoader* loaders;
	int loader_count;
	if (gameData.got_file_values.victory_conditions || gameData.got_file_values.starting_units || gameData.got_file_values.tournament_mode)
	{
		loaders = chk_loaders_[loader_index].melee_loaders;
		loader_count = chk_loaders_[loader_index].melee_loader_count;
	}
	else
	{
		loaders = chk_loaders_[loader_index].ums_loaders;
		loader_count = chk_loaders_[loader_index].ums_loader_count;
	}
	return ReadChunkNodes_(loader_count, chk_size, loaders, chk_data, 0);
}

FAIL_STUB_PATCH(sub_4CC2A0);

BOOL LoadFileArchiveToSBigBuf_(const char* filename, int* a2, int a3, HANDLE* a4)
{
	HANDLE handle;
	if (!SFileOpenArchive(filename, 0, 0, &handle))
	{
		return 0;
	}

	struct_v3 v5;
	v5.handle = handle;
	v5.dword4 = 0;
	mapAuthProc(mapAuthCallback, &v5);

	int a2a;
	if (v5.dword4 && (!LoadFileToSBigBuf(filename, &a2a) || a2a != 1))
	{
		v5.dword4 = 0;
	}
	if (a3)
	{
		*a4 = handle;
	}
	else
	{
		SFileCloseArchive(handle);
	}
	*a2 = v5.dword4;
	return 1;
}

FAIL_STUB_PATCH(LoadFileArchiveToSBigBuf);

void* sub_4CCAC0_campaign(const char* a1, int* chk_size)
{
	char campaign_map_path[MAX_PATH];
	sprintf_s(campaign_map_path, "%s\\staredit\\scenario.chk", a1);
	return fastFileRead_(chk_size, 0, campaign_map_path, 0, 1, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2060);
}

void* sub_4CCAC0_chk(const char* a1, int* chk_size)
{
	std::ifstream infile(a1, std::ios_base::binary, std::ios_base::binary);

	infile.seekg(0, std::ios::end);
	*chk_size = infile.tellg();

	infile.seekg(0, std::ios::beg);
	char* buffer = (char*)SMemAlloc(*chk_size, __FILE__, __LINE__, NULL);
	infile.read(buffer, *chk_size);

	return buffer;
}

int sub_4CC350_(const char* a2, int* a3, size_t a4)
{
	int a2a;
	if (LoadFileArchiveToSBigBuf_(a2, &a2a, 1, &mapArchiveHandle))
	{
		if (a3)
		{
			*a3 = a2a;
		}
		return 1;
	}
	return 0;
}

FAIL_STUB_PATCH(sub_4CC350);

void* sub_4CCAC0_scm(const char* a1, int* a2, int* chk_size)
{
	if (sub_4CC350_(a1, a2, MAX_PATH))
	{
		return fastFileRead_(chk_size, 0, "staredit\\scenario.chk", 0, 1, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2060);
	}
	return 0;
}

void* read_chk_data(const char* filename, int* a2, int* chk_size)
{
	if (active_campaign)
	{
		return sub_4CCAC0_campaign(filename, chk_size);
	}
	else if (std::filesystem::path(filename).extension() == ".chk")
	{
		return sub_4CCAC0_chk(filename, chk_size);
	}
	else
	{
		return sub_4CCAC0_scm(filename, a2, chk_size);
	}
}

BOOL sub_4CC7F0_(char* a1)
{
	int loader_index;
	int chk_size = 0;
	void* chk_data = read_chk_data(a1, &loader_index, &chk_size);

	int result;
	if (chk_data)
	{
		result = ReadGameChunks(chk_data, chk_size);
		SMemFree(chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2159, 0);
	}
	else
	{
		result = 0;
	}

	if (mapArchiveHandle)
	{
		SFileCloseArchive(mapArchiveHandle);
		mapArchiveHandle = NULL;
	}
	return result;
}

FAIL_STUB_PATCH(sub_4CC7F0);

int LoadMap_()
{
	if (InReplay)
	{
		return ReadGameChunks(scenarioChk, scenarioChkSize);
	}
	else if (CurrentMapFileName[0])
	{
		return sub_4CC7F0_(CurrentMapFileName);
	}

	return 0;
}

FAIL_STUB_PATCH(LoadMap);

UnknownTilesetRelated1* TILESET_PALETTE_RELATED[] = {
	&stru_512778,
	&stru_6D1228,
	&stru_6D1228,
	&stru_5127B8,
	&stru_512778,
	&stru_51279C,
	&stru_51279C,
	&stru_51279C,
};

MEMORY_PATCH(0x4BDD8A, TILESET_PALETTE_RELATED);
MEMORY_PATCH(0x4C99E4, TILESET_PALETTE_RELATED);
MEMORY_PATCH(0x4CB56A, TILESET_PALETTE_RELATED);
MEMORY_PATCH(0x4CB5DF, TILESET_PALETTE_RELATED);
MEMORY_PATCH(0x4CBEDA, TILESET_PALETTE_RELATED);
MEMORY_PATCH(0x4EEEB7, TILESET_PALETTE_RELATED);

void BINDLG_BlitSurface_(dialog* dlg)
{
	Bitmap* v3 = dword_6CF4A8;
	dlg->srcBits.wid = dlg->fields.dlg.dstBits_wid;
	dlg->srcBits.ht = dlg->fields.dlg.dstBits_ht;
	dlg->srcBits.data = (u8*)SMemAlloc(dlg->srcBits.wid * dlg->srcBits.ht, "Starcraft\\SWAR\\lang\\status.cpp", 181, 0);

	bounds a1a;
	a1a.left = dlg->rct.left;
	a1a.height = dlg->srcBits.ht;
	a1a.top = dlg->rct.top;
	a1a.width = dlg->srcBits.wid;
	a1a.right = dlg->srcBits.wid + a1a.left - 1;
	a1a.bottom = a1a.height + dlg->rct.top - 1;
	dword_6CF4A8 = &dlg->srcBits;
	BlitSurface(&a1a, &GameScreenConsole, 0, 0);
	dword_6CF4A8 = v3;
}

FAIL_STUB_PATCH(BINDLG_BlitSurface);

void showDialog_(dialog* dlg)
{
	if (!(dlg->lFlags & DialogFlags::CTRL_VISIBLE))
	{
		dlgEvent event;
		event.cursor.y = Mouse.y;
		event.wNo = EVN_USER;
		event.dwUser = USER_SHOW;
		*(_DWORD*)&event.wSelection = 0;
		event.cursor.x = Mouse.x;

		if (dlg->pfcnInteract(dlg, &event) && !(dlg->lFlags & DialogFlags::CTRL_UPDATE))
		{
			dlg->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog(dlg);
		}
	}
}

void showDialog__()
{
	dialog* dlg;

	__asm mov dlg, esi

	return showDialog_(dlg);
}

FUNCTION_PATCH((void*)0x4186A0, showDialog__);

void HideDialog_(dialog* dlg)
{
	if (dlg->lFlags & DialogFlags::CTRL_VISIBLE)
	{
		dlgEvent event;
		event.cursor.x = Mouse.x;
		event.cursor.y = Mouse.y;
		event.wNo = EventNo::EVN_USER;
		event.dwUser = EventUser::USER_HIDE;
		*(_DWORD*)&event.wSelection = 0;

		if (dlg->pfcnInteract(dlg, &event))
		{
			event.cursor.x = Mouse.x;
			event.cursor.y = Mouse.y;
			event.wNo = EventNo::EVN_USER;
			event.dwUser = EventUser::USER_NEXT;
			*(_DWORD*)&event.wSelection = 0;
			dlg->pfcnInteract(dlg, &event);

			if ((dlg->lFlags & DialogFlags::CTRL_UPDATE) == 0)
			{
				dlg->lFlags |= DialogFlags::CTRL_UPDATE;
				updateDialog(dlg);
			}
		}
	}
}

void __cdecl HideDialog__()
{
	dialog* dlg;

	__asm mov dlg, esi

	return HideDialog_(dlg);
}

FUNCTION_PATCH((void*)0x418700, HideDialog__);

DatLoad* spritesDat_;

const char* TILESET_NAMES[] = {
	"badlands",
	"platform",
	"install",
	"AshWorld",
	"Jungle",
	"Desert",
	"Ice",
	"Twilight",
};

void InitializePresetImageArrays_()
{
	memset(images, 0, sizeof(images));
	memset(stru_57EB78, 0, sizeof(stru_57EB78));
	memset(stru_57D768, 0, sizeof(stru_57D768));
	memset(stru_5244B8, 0, sizeof(stru_5244B8));
	memset(stru_51DE70, 0, sizeof(stru_51DE70));

	dword_57EB6C = stru_57EB78;
	dword_5254B8 = stru_57EB78;

	InitializeHealthBarImage(0, stru_57EB78);
	unsigned v0 = 1;
	CImage* v1 = &stru_57EB78[1];
	do
	{
		if (dword_57EB6C == stru_57EB78)
		{
			dword_57EB6C = v1;
		}
		v1->prev = stru_57EB78;
		v1->next = stru_57EB78[0].next;
		if (stru_57EB78[0].next)
		{
			stru_57EB78[0].next->prev = v1;
		}
		stru_57EB78[0].next = v1;
		InitializeHealthBarImage(v0++, v1++);
	} while (v0 < _countof(stru_57EB78));

	dword_52E4C0 = stru_57D768;
	dword_52F564 = stru_57D768;

	CImage* v2 = &stru_57D768[1];
	int v3 = 79;
	do
	{
		if (dword_52E4C0 == stru_57D768)
		{
			dword_52E4C0 = v2;
		}
		v2->prev = stru_57D768;
		v2->next = stru_57D768[0].next;
		if (stru_57D768[0].next)
		{
			stru_57D768[0].next->prev = v2;
		}
		stru_57D768[0].next = v2++;
		--v3;
	} while (v3);
	dword_51F1F8 = stru_5244B8;
	dword_52E4C4 = stru_5244B8;

	CImage* v4 = &stru_5244B8[1];
	int v5 = 63;
	do
	{
		if (dword_51F1F8 == stru_5244B8)
		{
			dword_51F1F8 = v4;
		}
		v4->prev = stru_5244B8;
		v4->next = stru_5244B8[0].next;
		if (stru_5244B8[0].next)
		{
			stru_5244B8[0].next->prev = v4;
		}
		stru_5244B8[0].next = v4++;
		--v5;
	} while (v5);

	dword_5240B4 = stru_51DE70;
	dword_51EE70 = stru_51DE70;

	stru_51DE70[0].flags &= ~(IF_CLICKABLE | IF_HAS_ISCRIPT_ANIMATIONS | IF_HAS_DIRECTIONAL_FRAMES);
	stru_51DE70[0].imageID = 583;
	stru_51DE70[0].GRPFile = stru_51EE78;
	stru_51EE78[0].wFrames = 1;
	stru_51DE70[0].frameSet = 0;
	stru_51DE70[0].direction = 0;
	stru_51DE70[0].frameIndex = 0;
	stru_51DE70[0].paletteType = 15;
	stru_51DE70[0].updateFunction = update_functions[15].update_function;
	stru_51DE70[0].renderFunction = stru_51DE70[0].flags & ImageFlags::IF_HORIZONTALLY_FLIPPED ? render_functions[15].RenderFunction2 : stru_51DE70[0].renderFunction = render_functions[15].RenderFunction1;
	stru_51DE70[0].flags |= ImageFlags::IF_REDRAW;

	for (int i = 1; i < _countof(stru_51DE70); i++)
	{
		if (dword_5240B4 == stru_51DE70)
		{
			dword_5240B4 = stru_51DE70 + i;
		}
		stru_51DE70[i].prev = stru_51DE70;
		stru_51DE70[i].next = stru_51DE70[0].next;
		if (stru_51DE70[0].next)
		{
			stru_51DE70[0].next->prev = stru_51DE70 + i;
		}
		stru_51DE70[0].next = stru_51DE70 + i;
		stru_51DE70[i].imageID = 583;
		stru_51DE70[i].GRPFile = stru_51EE78 + 1 + i;
		stru_51DE70[i].GRPFile->wFrames = 1;
		stru_51DE70[i].updateFunction = update_functions[15].update_function;
		stru_51DE70[i].renderFunction = stru_51DE70[i].flags & ImageFlags::IF_HORIZONTALLY_FLIPPED ? render_functions[15].RenderFunction2 : render_functions[15].RenderFunction1;
		stru_51DE70[i].flags &= ~(IF_CLICKABLE | IF_HAS_ISCRIPT_ANIMATIONS | IF_HAS_DIRECTIONAL_FRAMES);
		stru_51DE70[i].frameSet = 0;
		stru_51DE70[i].direction = 0;
		stru_51DE70[i].frameIndex = 0;
		stru_51DE70[i].paletteType = 15;
		stru_51DE70[i].flags |= ImageFlags::IF_REDRAW;
	}

	dword_57EB70 = images;
	dword_57EB68 = images;

	for (int i = 1; i < _countof(images); i++)
	{
		CImage* v16 = images + i;
		if (dword_57EB70 == images)
		{
			dword_57EB70 = v16;
		}
		v16->prev = images;
		v16->next = images[0].next;
		if (images[0].next)
		{
			images[0].next->prev = v16;
		}
		images[0].next = v16;
	}

	char buff[MAX_PATH];
	sprintf_s(buff, "Tileset\\%s\\shift.pcx", TILESET_NAMES[CurrentTileSet]);
	if (!SBmpLoadImage(buff, 0, tileset_shift, 256, 0, 0, 0))
	{
		throw FileNotFoundException(buff, SErrGetLastError());
	}
}

FAIL_STUB_PATCH(InitializePresetImageArrays);

void InitializeSpriteArray_()
{
	// TODO: dynamically allocate spritesDat memory
	spritesDat_ = new DatLoad[] {
		DatLoad { Sprites_Image, 2, 517 },
		DatLoad { Sprites_HealthBarSize, 1, 387 },
		DatLoad { Sprites_Unknown, 1, 517 },
		DatLoad { Sprites_IsVisible, 1, 517 },
		DatLoad { Sprites_SelectionCircle, 1, 387 },
		DatLoad { Sprites_SelectionCircleVPos, 1, 387 },
	};

	LoadGameData_(spritesDat_, "arr\\sprites.dat");
	memset(SpriteTable, 0, sizeof(SpriteTable));
	memset(_SpritesOnTileRow.heads, 0, sizeof(_SpritesOnTileRow.heads));
	memset(_SpritesOnTileRow.tails, 0, sizeof(_SpritesOnTileRow.tails));

	SpriteTable[0].index = 0;
	dword_63FE34 = SpriteTable;
	UnusedSprites = SpriteTable;

	for (int index = 1; index < _countof(SpriteTable); index++)
	{
		CSprite* sprite = SpriteTable + index;

		sprite->index = index;
		if (dword_63FE34 == UnusedSprites)
		{
			dword_63FE34 = sprite;
		}
		sprite->prev = UnusedSprites;
		sprite->next = UnusedSprites->next;
		if (UnusedSprites->next)
		{
			UnusedSprites->next->prev = sprite;
		}
		UnusedSprites->next = sprite;
	}
}

FAIL_STUB_PATCH(InitializeSpriteArray);

void InitializeThingyArray_()
{
	memset(stru_6509D8, 0, sizeof(stru_6509D8));
	dword_65487C = stru_6509D8;
	dword_654878 = stru_6509D8;
	dword_65291C = 0;
	first_lone_sprite = 0;
	CThingy* v1 = &stru_6509D8[1];
	int v2 = 499;
	do
	{
		CThingy* v0 = dword_654878;
		if (dword_65487C == v0)
		{
			dword_65487C = v1;
		}
		// insert `v1` after `v0` in a doubly linked list
		v1->prev = v0;
		v1->next = v0->next;
		if (v0->next)
		{
			v0->next->prev = v1;
		}
		v0->next = v1;
		v1++;
		--v2;
	} while (v2);

	memset(stru_652928, 0, sizeof(stru_652928));
	dword_654870 = stru_652928;
	dword_6509D0 = stru_652928;
	dword_65486C = 0;
	dword_654868 = 0;
	CThingy* v5 = &stru_652928[1];
	int v6 = 499;
	do
	{
		CThingy* v7 = dword_6509D0;
		if (dword_654870 == dword_6509D0)
		{
			dword_654870 = v5;
		}
		// insert `v5` after `v7` in a doubly linked list
		v5->prev = v7;
		v5->next = v7->next;
		if (v7->next)
		{
			v7->next->prev = v5;
		}
		v7->next = v5;
		v5++;
		--v6;
	} while (v6);

	ThingyList_UsedFirst = CreateThingy(318, 0, 0, 0);
	wantThingyUpdate = 0;
	ThingyList_UsedFirst->sprite->flags |= 0x20;
	refreshAllVisibleImagesAtScreenPosition(ThingyList_UsedFirst->sprite, 0);
}

FAIL_STUB_PATCH(InitializeThingyArray);

DatLoad* flingyDat_;

void InitializeFlingyDat_()
{
	// TODO: dynamically allocate flingyDat memory
	flingyDat_ = new DatLoad[] {
		DatLoad { Flingy_SpriteID, 2, 209 },
		DatLoad { Flingy_TopSpeed, 4, 209 },
		DatLoad { Flingy_Acceleration, 2, 209 },
		DatLoad { Flingy_HaltDistance, 4, 209 },
		DatLoad { Flingy_TurnSpeed, 1, 209 },
		DatLoad { Flingy_Unused, 1, 209 },
		DatLoad { Flingy_MovementControl, 1, 209 },
	};
	LoadGameData_(flingyDat_, "arr\\flingy.dat");
	memset(dword_63FEE0, 0, 76u);
	dword_63FF3C = (CUnit*)dword_63FEE0;
	dword_63FF38 = (CUnit*)dword_63FEE0;
	dword_63FEC8 = 0;
	dword_63FF34 = 0;
}

FAIL_STUB_PATCH(InitializeFlingyDat);

void ResetDATFiles_()
{
	LoadGameData_(upgradesDat, "arr\\upgrades.dat");

	// TODO: dynamically allocate weaponsDat memory
	weaponsDat_ = new DatLoad[] {
		DatLoad { Weapon_Label, 2, 130 },
		DatLoad { Weapon_Graphic, 4, 130 },
		DatLoad { Weapon_TechHint, 1, 130 },
		DatLoad { Weapon_TargetFlags, 2, 130 },
		DatLoad { Weapon_MinRange, 4, 130 },
		DatLoad { Weapon_MaxRange, 4, 130 },
		DatLoad { Weapon_Upgrade, 1, 130 },
		DatLoad { Weapon_DamageType, 1, 130 },
		DatLoad { Weapon_Behavior, 1, 130 },
		DatLoad { Weapon_RemoveAfter, 1, 130 },
		DatLoad { Weapon_ExplosionType, 1, 130 },
		DatLoad { Weapon_InnerSplashRadius, 2, 130 },
		DatLoad { Weapon_MedianSplashRadius, 2, 130 },
		DatLoad { Weapon_OuterSplashRadius, 2, 130 },
		DatLoad { Weapon_DamageAmount, 2, 130 },
		DatLoad { Weapon_DamageBonus, 2, 130 },
		DatLoad { Weapon_DamageCooldown, 1, 130 },
		DatLoad { Weapon_DamageFactor, 1, 130 },
		DatLoad { Weapon_AttackDirection, 1, 130 },
		DatLoad { Weapon_LaunchSpin, 1, 130 },
		DatLoad { Weapon_XOffset, 1, 130 },
		DatLoad { Weapon_YOffset, 1, 130 },
		DatLoad { Weapon_TargetErrorMessage, 2, 130 },
		DatLoad { Weapon_Icon, 2, 130 },
	};
	LoadGameData_(weaponsDat_, "arr\\weapons.dat");

	LoadGameData_(techdataDat, "arr\\techdata.dat");

	LoadGameData_(portdataDat, "arr\\portdata.dat");

	if (!loadGameFileHandle)
	{
		memset(&UnitAvailability, 1, sizeof(UnitAvailability));
		memset(TechAvailableSC, 1, sizeof(TechAvailableSC));
		memset(TechAvailableBW, 1, sizeof(TechAvailableBW));
		memset(TechResearchSC, 0, sizeof(TechResearchSC));
		memset(TechResearchBW, 0, sizeof(TechResearchBW));
		memset(ResearchProgress, 0, sizeof(ResearchProgress));

		for (int i = 0; i < 12; i++)
		{
			memcpy(UpgradeMaxSC + i, Upgrade_MaxRepeats, sizeof(UpgradesSC));
			memcpy(UpgradeMaxBW[i].items, &Upgrade_MaxRepeats[46], sizeof(UpgradeMaxBW[i].items));
		}

		memset(UpgradeLevelSC, 0, sizeof(UpgradeLevelSC));
		memset(UpgradeLevelBW, 0, sizeof(UpgradeLevelBW));
		memset(UpgradeProgress, 0, sizeof(UpgradeProgress));
		memset(stru_596CD8, 0, sizeof(stru_596CD8));
		memset(next_scenario, 0, sizeof(next_scenario));

		dword_58F442 = 0;
		dword_58F446 = 0;
	}
}

FAIL_STUB_PATCH(ResetDATFiles);

void resetOrdersUnitsDAT_()
{
	sub_4531A0();
	dword_59CCA4 = 10;
	dword_6283E8 = 150;
	LoadGameData_(ordersDat, "arr\\orders.dat");
	LoadGameData_(unitsDat, "arr\\units.dat");
	ReassignTargetAcquisitionRange();
	memset(UnitNodeTable, 0, sizeof(UnitNodeTable));
	memset(UnitNodeList_PlayerFirstUnit, 0, sizeof(UnitNodeList_PlayerFirstUnit));

	CUnit* v0 = 0;
	UnitNodeTable_UsedNodeCount = 0;
	UnitNodeList_VisibleUnit_Last = 0;
	UnitNodeList_VisibleUnit_First = 0;
	UnitNodeList_ScannerSweep_Last = 0;
	UnitNodeList_ScannerSweep_First = 0;
	dword_59CC98 = 0;
	UnitNodeList_HiddenUnit_Last = 0;
	dword_628428 = 0;
	UnitNodeList_HiddenUnit_First = 0;
	UnitNodeList_UnusedUnit_Last_maybe = 0;
	UnitNodeList_UnusedUnit_First_maybe = 0;

	for (int i = 0; i < _countof(UnitNodeTable); i++)
	{
		CUnit* unit = &UnitNodeTable[i];
		if (v0)
		{
			if (UnitNodeList_UnusedUnit_Last_maybe == v0)
			{
				UnitNodeList_UnusedUnit_Last_maybe = unit;
			}
			// insert `unit` after `v0` in a doubly linked list
			unit->prev = v0;
			unit->next = v0->next;
			if (v0->next)
			{
				v0->next->prev = unit;
			}
			v0->next = unit;
			v0 = UnitNodeList_UnusedUnit_First_maybe;
		}
		else
		{
			v0 = unit;
			UnitNodeList_UnusedUnit_Last_maybe = unit;
			UnitNodeList_UnusedUnit_First_maybe = unit;
		}
	}
	memset(UnitOrderingX, 0xFFu, sizeof(UnitOrderingX));
	UnitOrderingCount[0] = 0;
	memset(UnitOrderingY, 0xFFu, sizeof(UnitOrderingY));
	resetUnitBoundsLists_GetMaxUnitTypeSize();
	LOWORD(error_message[0]) = 0;
}

FAIL_STUB_PATCH(resetOrdersUnitsDAT);

void BWFXN_NextFrameHelperFunctionTarget_()
{
	if (!byte_6D1214)
	{
		int a1;
		int a2;
		CursorType cursor_type = (CursorType) getScrollCursorType(&a1, &a2);
		if (cursor_type == CUR_ARROW)
		{
			cursor_type = getCursorType_();
		}
		setCursorType_(cursor_type);
	}
	if (last_cursor && last_cursor->wFrames != 1)
	{
		DWORD tick_count = GetTickCount();
		if (tick_count >= dword_597398)
		{
			dword_597398 = tick_count + 100;
			++dword_597390;
			drawCursor_();
		}
	}
}

FAIL_STUB_PATCH(BWFXN_NextFrameHelperFunctionTarget);

bool __fastcall j_BWFXN_NextFrameHelperFunctionTarget_(dialog*)
{
	BWFXN_NextFrameHelperFunctionTarget_();
	return false;
}

FAIL_STUB_PATCH(j_BWFXN_NextFrameHelperFunctionTarget);

void sub_4E4820_(dialog* dlg)
{
	if (gwGameMode == GamePosition::GAME_RUN)
	{
		dlg->lFlags |= DialogFlags::CTRL_DLG_NOREDRAW;
	}
	dlg->fields.dlg.pModalFcn = j_BWFXN_NextFrameHelperFunctionTarget_;
	dlg->pfcnUpdate = sub_4E46A0;
	getControlFromIndex_(dlg, -2)->pfcnUpdate = sub_4E4770;
	if (dword_68EC88)
	{
		if (dialog* child_dlg = getControlFromIndex_(dlg, -10))
		{
			child_dlg->pszText = dword_68EC88;
			child_dlg->pfcnUpdate = sub_4E4750;
		}
	}
	if (dword_68EC84)
	{
		if (dialog* child_dlg = getControlFromIndex_(dlg, -20))
		{
			child_dlg->pszText = dword_68EC84;
			child_dlg->pfcnUpdate = sub_4E4750;
		}
	}

	setCursorType_(CursorType::CUR_ARROW);
	byte_6D1214 = 1;
	ScreenLayers[0].buffers = 1;
	ScreenLayers[0].bits |= 1;
	BWFXN_RefreshTarget(
		(__int16)ScreenLayers[0].left,
		(__int16)ScreenLayers[0].height + (__int16)ScreenLayers[0].top - 1,
		(__int16)ScreenLayers[0].top,
		(__int16)ScreenLayers[0].width + (__int16)ScreenLayers[0].left - 1);
	DlgAccelerator = hAccTable;
	AcceleratorTables = input_procedures[EventNo::EVN_SYSCHAR];
	hAccTable = dword_5968F8;
	input_procedures[EventNo::EVN_SYSCHAR] = input_standardSysHotkeys;
	refreshSelectionScreen_();
}

FAIL_STUB_PATCH(sub_4E4820);

void sub_4E4590_(dialog* dlg)
{
	SetCursorClipBounds_();
	dword_6D5DD4 = 0;
	if (dword_6D5DD0)
	{
		ClipCursor(&screen);
	}
	if (dialog* child_dlg = getControlFromIndex_(dlg, -10))
	{
		child_dlg->pszText = nullptr;
	}
	if (dialog* child_dlg = getControlFromIndex_(dlg, -20))
	{
		child_dlg->pszText = nullptr;
	}
	if (dword_68EC88)
	{
		SMemFree(dword_68EC88, "Starcraft\\SWAR\\lang\\dlgFatal.cpp", 153, 0);
		dword_68EC88 = 0;
	}
	if (dword_68EC84)
	{
		SMemFree(dword_68EC84, "Starcraft\\SWAR\\lang\\dlgFatal.cpp", 159, 0);
		dword_68EC84 = 0;
	}
	byte_6D1214 = 0;
	hAccTable = DlgAccelerator;
	input_procedures[EventNo::EVN_SYSCHAR] = AcceleratorTables;
}

FAIL_STUB_PATCH(sub_4E4590);

void sub_4E4670_(dialog* dlg)
{
	RECT rect;

	rect.left = dlg->rct.left;
	rect.top = dlg->rct.top;
	rect.right = dlg->rct.right;
	rect.bottom = dlg->rct.bottom;
	ClipCursor(&rect);
}

FAIL_STUB_PATCH(sub_4E4670);

int __fastcall dlgfatal_loop_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			sub_4E4820_(dlg);
			break;
		case EventUser::USER_DESTROY:
			sub_4E4590_(dlg);
			genericDlgInteract(dlg, evt);
			return 1;
		case EventUser::USER_ACTIVATE:
			DestroyDialog(dlg);
			return 1;
		case EventUser::USER_INIT:
			sub_4E4670_(dlg);
			break;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(dlgfatal_loop);

void load_DLGFatal_BIN_(const char* error_location, const char* a2)
{
	if (!byte_51A0E9)
	{
		if (is_placing_building)
		{
			refreshLayer3And4();
			refreshPlaceBuildingLocation();
		}
		if (is_placing_order)
		{
			CancelTargetOrder();
		}
		if (!multiPlayerMode)
		{
			TickCountSomething(1);
		}
		if (GameMenuDlg)
		{
			DestroyDialog(GameMenuDlg);
			GameMenuDlg = 0;
			if (gwGameMode == GamePosition::GAME_RUN)
			{
				hAccTable = DlgAccelerator;
				input_procedures[EventNo::EVN_SYSCHAR] = AcceleratorTables;
			}
			byte_6D1214 = 0;
		}
		if (dlgFatal_Dlg)
		{
			DestroyDialog(dlgFatal_Dlg);
		}
		if (a2)
		{
			dword_68EC88 = (char*) SMemAlloc(strlen(a2) + 1, "Starcraft\\SWAR\\lang\\dlgFatal.cpp", 225, 0);
			strcpy(dword_68EC88, a2);
		}
		if (error_location)
		{
			dword_68EC84 = (char*)SMemAlloc(strlen(error_location) + 1, "Starcraft\\SWAR\\lang\\dlgFatal.cpp", 229, 0);
			strcpy(dword_68EC84, error_location);
		}

		dlgFatal_Dlg = (dialog*) fastFileRead_(0, 0, "rez\\dlgFatal.bin", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
		if (dlgFatal_Dlg)
		{
			dlgFatal_Dlg->lFlags |= DialogFlags::CTRL_ACTIVE;
			AllocInitDialogData(dlgFatal_Dlg, dlgFatal_Dlg, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\dlgFatal.cpp", 234);
		}

		gluLoadBINDlg_(dlgFatal_Dlg, dlgfatal_loop_);
		dlgFatal_Dlg = 0;
	}
}

FAIL_STUB_PATCH(load_DLGFatal_BIN);

void sub_4AD0E0_(const char* text, const char* caption)
{
	HWND window = GetActiveWindow();
	if (!window)
	{
		window = SDrawGetFrameWindow(0);
	}
	BattleErrorDialog(window, text, caption, 0);
}

FAIL_STUB_PATCH(sub_4AD0E0);

void sub_4B6C70_(dialog* dlg)
{
	registerMenuFunctions(dword_6D5A40, dlg, dword_599D98, 0);
	dlg->lFlags |= DialogFlags::CTRL_TRANSLUCENT;
	dlg->srcBits = p_hist_pcx;
	p_hist_pcx.wid = 0;
	p_hist_pcx.ht = 0;
	p_hist_pcx.data = 0;
	if ((dlg->lFlags & CTRL_UPDATE) == 0)
	{
		dlg->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog(dlg);
	}
}

FAIL_STUB_PATCH(sub_4B6C70);

int __fastcall gluPEdit_Main_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			sub_4B6A20(dlg);
			break;
		case USER_DESTROY:
			sub_4B65D0(dlg);
			DestroyChildren(dlg);
			dword_6D5A3C = 0;
			dword_6D5A54 = 0;
			break;
		case USER_UNK_7:
			sub_4CD9C0(dlg);
			break;
		case USER_INIT:
			sub_4B6C70_(dlg);
			break;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluPEdit_Main);

bool __stdcall BWFXN_gluPEdit_MBox_(char* text, char* dest, size_t size, char* restricted)
{
	if (dword_6D5A3C)
	{
		LastControlID = dword_6D5A54;
		if (!dword_6D5A54)
		{
			LastControlID = 3;
		}
		DestroyDialog(dword_6D5A3C);
	}
	SStrCopy(byte_599C98, text, 0xFFu);
	SStrCopy(byte_599A90, dest, 0xFFu);
	dword_599A88 = (int)restricted;
	dword_6D5A40 = off_51A6CC;
	dword_599D98 = 16;

	char fileName[MAX_PATH];
	SStrCopy(fileName, stru_50E06C[stru_4FFAD0[glGluesMode].menu_position].glue_path, sizeof(fileName));
	strcat_s(fileName, "\\pEPopup.pcx");

	u8* buffer;
	int width, height;
	if (!SBmpAllocLoadImage(fileName, (int*) palette, (void**) &buffer, &width, &height, 0, 0, allocFunction))
	{
		throw FileNotFoundException(fileName, SErrGetLastError());
	}
	p_hist_pcx.wid = (unsigned __int16)width;
	p_hist_pcx.ht = (unsigned __int16)height;
	p_hist_pcx.data = (u8*)buffer;

	dword_6D5A3C = (dialog*) fastFileRead_(0, 0, "rez\\gluPEdit.bin", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (dword_6D5A3C)
	{
		dword_6D5A3C->lFlags |= DialogFlags::CTRL_ACTIVE;
		AllocInitDialogData(dword_6D5A3C, dword_6D5A3C, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 432);
	}

	getControlFromIndex_(dword_6D5A3C, 4)->lUser = size;
	if (GetUserDefaultLangID() == 1042)
	{
		HIMC Context = dword_6D6438;
		if (!dword_6D6438)
		{
			Context = ImmGetContext(hWndParent);
			dword_6D6438 = Context;
		}
		ImmAssociateContext(hWndParent, Context);
	}
	int BINDlg = gluLoadBINDlg(dword_6D5A3C, gluPEdit_Main_);
	if (GetUserDefaultLangID() == 1042)
	{
		if (!dword_6D6438)
		{
			dword_6D6438 = ImmGetContext(hWndParent);
		}
		dword_6D6438 = ImmAssociateContext(hWndParent, 0);
	}
	if (BINDlg != 1)
	{
		return 0;
	}

	int v19 = size;
	char* v20 = byte_599A90;
	char v21;
	if (byte_599A90[0])
	{
		do
		{
			v21 = *++v20;
			--v19;
		} while (v21);
	}
	if (v19 > 0)
	{
		memset(v20, 0, v19);
	}
	SStrCopy(dest, byte_599A90, size);
	byte_599A90[0] = 0;
	return 1;
}

FUNCTION_PATCH(BWFXN_gluPEdit_MBox, BWFXN_gluPEdit_MBox_);

int __fastcall Popup_Main_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			sub_4B6810(dlg);
			break;
		case USER_DESTROY:
			DestroyChildren(dlg);
			dword_6D5A3C = 0;
			dword_6D5A54 = 0;
			break;
		case USER_UNK_7:
			sub_4CD9C0(dlg);
			break;
		case USER_INIT:
			sub_4B6C70_(dlg);
			break;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(Popup_Main);

void BWFXN_gluPOK_MBox_(const char* a1)
{
	if (dword_6D5A3C)
	{
		LastControlID = dword_6D5A54;
		if (!dword_6D5A54)
		{
			LastControlID = 3;
		}
		DestroyDialog(dword_6D5A3C);
	}
	dword_6D5A40 = off_51A6DC;
	dword_599D98 = 8;
	strcpy(byte_599C98, a1);
	byte_599B98[0] = 0;

	char fileName[260];
	strcpy_s(fileName, stru_50E06C[stru_4FFAD0[glGluesMode].menu_position].glue_path);
	strcat_s(fileName, "\\pOPopup.pcx");

	u8* data;
	int height, width;
	if (!SBmpAllocLoadImage(fileName, (int*) palette, (void**)&data, &width, &height, 0, 0, allocFunction))
	{
		throw FileNotFoundException(fileName, SErrGetLastError());
	}
	p_hist_pcx.wid = (unsigned __int16)width;
	p_hist_pcx.ht = (unsigned __int16)height;
	p_hist_pcx.data = data;

	dialog* dword_6D5A3C = (dialog * ) fastFileRead_(0, 0, "rez\\gluPOk.bin", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (dword_6D5A3C)
	{
		dword_6D5A3C->lFlags |= DialogFlags::CTRL_ACTIVE;
		AllocInitDialogData(dword_6D5A3C, dword_6D5A3C, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 396);
	}
	gluLoadBINDlg(dword_6D5A3C, Popup_Main_);
}

void __cdecl BWFXN_gluPOK_MBox__()
{
	const char* a1;

	__asm mov a1, eax

	BWFXN_gluPOK_MBox_(a1);
}

FUNCTION_PATCH((void*) 0x4B7180, BWFXN_gluPOK_MBox__);

void load_gluPOKSplitBINDLG_(const char* a1, const char* a2)
{
	if (dword_6D5A3C)
	{
		LastControlID = dword_6D5A54;
		if (!dword_6D5A54)
		{
			LastControlID = 3;
		}
		DestroyDialog(dword_6D5A3C);
	}
	if (!a1 || !*a1)
	{
		BWFXN_gluPOK_MBox_(a2);
		return;
	}
	dword_6D5A40 = off_51A6B8;
	dword_599D98 = 20;
	strcpy(byte_599C98, a2);
	strcpy(byte_599B98, a1);

	char dest[MAX_PATH];
	strcpy_s(dest, stru_50E06C[stru_4FFAD0[glGluesMode].menu_position].glue_path);
	strcat_s(dest, "\\pOPopup.pcx");
	AllocBackgroundImage(dest, &p_hist_pcx, palette, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 473);

	dword_6D5A3C = (dialog*) fastFileRead_(0, 0, "rez\\gluPOkSplit.bin", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (dword_6D5A3C)
	{
		dword_6D5A3C->lFlags |= DialogFlags::CTRL_ACTIVE;
		AllocInitDialogData(dword_6D5A3C, dword_6D5A3C, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 475);
	}
	gluLoadBINDlg_(dword_6D5A3C, Popup_Main_);
}

FAIL_STUB_PATCH(load_gluPOKSplitBINDLG);

void sub_4B5CC0_(const char* error_message, const char* error_details)
{
	if (dword_6D5A58)
	{
		LastControlID = 3;
		DestroyDialog(dword_6D5A58);
	}
	if (error_details == nullptr || strlen(error_details) == 0)
	{
		sub_4B59A0(error_message);
	}
	else if (DialogList && (++dword_6D63AC, dword_6D63AC == 1))
	{
		load_gluPOKSplitBINDLG_(error_details, error_message);
		dword_6D63AC = 0;
	}
	else
	{
		dword_6D5A5C = off_51A7F0;
		dword_59A0D0 = 20;
		strcpy(byte_599FD0, error_message);
		strcpy(byte_599DC8, error_details);
		strcpy(dword_599EC8, "GLUEOK_PCX");

		if (HRSRC resource = FindResource(local_dll_library, L"GLUEOK_BIN", MAKEINTRESOURCE(7004)))
		{
			if (DWORD resource_size = SizeofResource(local_dll_library, resource))
			{
				if (HGLOBAL v7 = LoadResource(local_dll_library, resource))
				{
					resource = (HRSRC)LockResource(v7);

					dword_6D5A58 = (dialog*)SMemAlloc(resource_size, "Starcraft\\SWAR\\lang\\gluError.cpp", 58, 0);
					memcpy(dword_6D5A58, resource, resource_size);
					if (dword_6D5A58)
					{
						dword_6D5A58->lFlags |= CTRL_ACTIVE;
						AllocInitDialogData(dword_6D5A58, dword_6D5A58, LoadPCXFromResource, "Starcraft\\SWAR\\lang\\gluError.cpp", 359);
					}
					gluLoadBINDlg_(dword_6D5A58, gluOK_Dlg_Interact_CB);
					dword_6D63AC = 0;
					return;
				}
			}
		}

		std::string error_with_details;
		error_with_details += error_message;
		error_with_details += + "\n\n";
		error_with_details += error_details;
		throw std::runtime_error(error_with_details);
	}
}

FAIL_STUB_PATCH(sub_4B5CC0);

void doNetTBLError_(int line, const char* error_message, char* file_name, int a4)
{
	if (byte_51A0E9 || !DialogList)
	{
		sub_4BB1A0(error_message, a4, file_name, line);
		return;
	}

	char buff[256] = { 0 };
	if (error_message && file_name && line)
	{
		sprintf_s(buff, "%s file: %s:%d", error_message, file_name, line);
	}

	if (gwGameMode == GAME_RUN)
	{
		int v5 = dword_51267C;
		int v6 = g_ActiveNationID;

		dword_51267C = g_LocalHumanID;
		g_ActiveNationID = g_LocalNationID;
		load_DLGFatal_BIN_(buff, GetNetworkTblString_(a4));

		g_ActiveNationID = v6;
		dword_51267C = v5;
	}
	else if (glGluesMode == GLUE_BATTLE)
	{
		sub_4AD0E0_(GetNetworkTblString_(a4), buff);
	}
	else
	{
		sub_4B5CC0_(GetNetworkTblString_(a4), buff);
	}
}

void __stdcall doNetTBLError__(int a4)
{
	int a1;
	const char* error_message;
	char* file_name;

	__asm mov a1, eax
	__asm mov error_message, edx
	__asm mov file_name, ecx

	doNetTBLError_(a1, error_message, file_name, a4);
}

FUNCTION_PATCH((void*)0x4BB300, doNetTBLError__);

void nextLeaveGameMenu_()
{
	if (glGluesMode == MenuPosition::GLUE_BATTLE)
	{
		DWORD dwProcessId;
		GetWindowThreadProcessId(hWndParent, &dwProcessId);
		EnumWindows(EnumFunc, dwProcessId);
	}
	glGluesMode = MenuPosition::GLUE_GENERIC;
}

FAIL_STUB_PATCH(nextLeaveGameMenu);

void BigPacketError_(int a1, const char* a2, char* a3, int a4, int a5)
{
	if (!outOfGame)
	{
		if (a5)
		{
			leaveGame(3);
			outOfGame = 1;
			doNetTBLError_(a4, a2, a3, a1);
			if (gwGameMode == GamePosition::GAME_RUN)
			{
				GameState = 0;
				gwNextGameMode = GamePosition::GAME_GLUES;
				if (!InReplay)
				{
					replay_header.ReplayFrames = ElapsedTimeFrames;
				}
			}
			nextLeaveGameMenu_();
		}
		else
		{
			doNetTBLError_(0, 0, 0, a1);
		}
	}
}

FAIL_STUB_PATCH(BigPacketError);

void RECV_SetRandomSeed_(int a1, struct_v2* a2)
{
	if (!a1 && gameState == 8)
	{
		if (!InReplay)
		{
			replay_header.seed_related = *a2;
		}
		initialSeed = a2->initial_seed;
		for (int v3 = 7; v3 >= 0; v3--)
		{
			u8 v4 = a2->player_bytes[v3];
			if (v4 > 8)
			{
				BigPacketError_(96, 0, 0, 0, true);
				break;
			}
			byte_66FF34[v3] = v4;
		}
	}
}

void __cdecl RECV_SetRandomSeed__()
{
	int a1;
	struct_v2* a2;

	__asm mov a1, eax
	__asm mov a2, edx

	RECV_SetRandomSeed_(a1, a2);
}

FUNCTION_PATCH((void*)0x472110, RECV_SetRandomSeed__);

int sub_4EEFD0_()
{
	memcpy(stru_59C6C0, palette, sizeof(stru_59C6C0));
	sub_49BB90();
	initMapData_();
	InitializePresetImageArrays_();
	InitializeSpriteArray_();
	InitializeThingyArray_();
	InitializeFlingyDat_();
	InitializeBulletArray();
	InitializeOrderArray();
	if (!loadGameFileHandle)
	{
		InitializeUnitCounts();
	}
	initializePsiFieldData();
	ResetDATFiles_();
	resetOrdersUnitsDAT_();
	createUnitBuildingSpriteValidityArray();
	if (loadGameFileHandle || LoadMap_())
	{
		load_gluMinimap_();
		memcpy(palette, stru_59C6C0, sizeof(palette));
		return 1;
	}
	else
	{
		if (!dword_6D5BF8)
		{
			BigPacketError_(97, 0, 0, 0, true);
		}
		return 0;
	}
}

FAIL_STUB_PATCH(sub_4EEFD0);

void GameInitAI_()
{
	dword_6D5BD0 = 0;
	initAIControllerData();
	initDetailFinder();
	InitRegionCaptains(0);
	load_AIScript_BIN();
	initAITownMgr();
	dword_59CC94 = 60;
	dword_59CC8C = 15;
	dword_59CC90 = 6;
}

FAIL_STUB_PATCH(GameInitAI);

u8 illegalTeamCheck_()
{
	if (gameData.got_file_values.team_mode == 2)
	{
		return 4;
	}
	else if (gameData.got_file_values.team_mode == 3)
	{
		return gameData.got_file_values.team_mode;
	}
	else if (gameData.got_file_values.team_mode == 4)
	{
		return 2;
	}
	else
	{
		BigPacketError_(93, GetNetworkTblString_(73), "Starcraft\\SWAR\\lang\\Teams.cpp", 113, true);
	}
	return 0;
}

FUNCTION_PATCH(illegalTeamCheck, illegalTeamCheck_);

void CreateInitialTeamMeleeUnits_()
{
	int team_count = gameData.got_file_values.template_id != 15 ? gameData.got_file_values.team_mode : 2;

	for (int team_index = 0; team_index < team_count; team_index++)
	{
		for (int player_index = 7; player_index >= 0; player_index--)
		{
			if (Players[player_index].nTeam == team_index + 1 && Players[player_index].nType == PlayerType::PT_Human)
			{
				goto LABEL_15;
			}
		}
		for (int player_index = 7; player_index >= 0; player_index--)
		{
			if (Players[player_index].nTeam == team_index + 1 && (Players[player_index].nType == PlayerType::PT_Computer || Players[player_index].nType == PlayerType::PT_ComputerSlot))
			{
				goto LABEL_15;
			}
		}
		continue;

	LABEL_15:
		u8 a1 = byte_57F1CB[team_index + 1];
		u8 v6 = team_index * illegalTeamCheck_();
		if (gameData.got_file_values.starting_units == StartingUnits::SU_WORKER_AND_CENTER)
		{
			CreateInitialMeleeBuildings_(byte_57F1C0[v6], a1);
			if (byte_57F1C0[v6] == RACE_Zerg)
			{
				CreateInitialOverlord(a1);
			}
		}
		for (int j = 0; j < 4; ++j)
		{
			int v9 = j % illegalTeamCheck_();
			RaceId v10 = v6 + v9 < 8 ? byte_57F1C0[v6 + v9] : byte_57F1C0[v6 + 1];

			if (gameData.got_file_values.starting_units && gameData.got_file_values.starting_units <= StartingUnits::SU_WORKER_AND_CENTER)
			{
				CUnit* worker_unit = CreateUnit(Race::races()[v10].worker, startPositions[a1].x, startPositions[a1].y, a1);
				if (worker_unit)
				{
					updateUnitStatsFinishBuilding(worker_unit);
					if (sub_49EC30(worker_unit))
					{
						updateUnitStrengthAndApplyDefaultOrders(worker_unit);
					}
				}
			}
		}
	}
}

FAIL_STUB_PATCH(CreateInitialTeamMeleeUnits);

signed int GameInit_()
{
	memset(Chat_GameText, 0, 2832u);
	*(_WORD *)&Chat_GameText[12].chars[216] = 0;
	resetTextAndLineData();
	GameInitAI_();
	initMapData_();
	InitializePresetImageArrays_();
	InitializeSpriteArray_();
	InitializeThingyArray_();
	InitializeFlingyDat_();
	InitializeBulletArray();
	InitializeOrderArray();
	sub_4CB5B0(TILESET_PALETTE_RELATED[CurrentTileSet]->y);
	sub_41E450(sub_4BDB30, palette);
	sub_4C99C0();
	if (!loadGameFileHandle)
		InitializeUnitCounts();
	initializePsiFieldData();
	ResetDATFiles_();
	resetOrdersUnitsDAT_();
	calculateUnitStrengths();
	createUnitBuildingSpriteValidityArray();
	if (loadGameFileHandle)
		return 1;
	if (LoadMap_())
	{
		if (!loadGameFileHandle)
		{
			if (LOBYTE(multiPlayerMode) && gameData.got_file_values.team_mode)
				CreateInitialTeamMeleeUnits_();
			else
				CreateInitialMeleeUnits_();
			InitialSetScreenToStartLocation();
			if (!loadGameFileHandle)
			{
				InitRegionCaptains(1);
				sub_446350();
			}
		}
		return 1;
	}
	if (!dword_6D5BF8)
	{
		BigPacketError_(97, 0, 0, 0, 1);
	}
	return 0;
}

FAIL_STUB_PATCH(GameInit);
FAIL_STUB_PATCH(sub_4CD770);
FAIL_STUB_PATCH(sub_4A13B0);

GotFileValues* InitUseMapSettingsTemplate_()
{
	char ununsed[32];

	return readTemplate_("Use Map Settings(1)", ununsed, ununsed);
}

FAIL_STUB_PATCH(InitUseMapSettingsTemplate);

int sub_4CCAC0_(const char* a1, MapChunks* a2)
{
	if (!a2 || !a1 || strlen(a1) == 0)
	{
		SErrSetLastError(0x57u);
		return 0;
	}

	int chk_size = 0;
	void* chk_data = read_chk_data(a1, &a2->data7, &chk_size);

	if (chk_data)
	{
		int v7 = ReadLobbyChunks(chk_data, chk_size, a2);
		SMemFree(chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2077, 0);
		mapHandleDestroy();
		return v7;
	}
	if (mapArchiveHandle)
	{
		SFileCloseArchive(mapArchiveHandle);
		mapArchiveHandle = 0;
	}
	return 0;
}

FAIL_STUB_PATCH(sub_4CCAC0);

void sub_4A91E0_()
{
	for (int i = 0; i < _countof(Players); i++)
	{
		memcpy(&Players[i], &LobbyPlayers[i], sizeof(PlayerInfo));
	}
}

FAIL_STUB_PATCH(sub_4A91E0);

HANDLE custom_campaign_mpq = nullptr;

int ReadMapData_(const char* source, MapChunks* a4, int is_campaign)
{
	CurrentMapFileName[0] = 0;
	if (!is_campaign)
	{
		active_campaign = nullptr;
	}
	memset(LobbyPlayers, 0, sizeof(LobbyPlayers));
	memset(playerForce, 0, sizeof(playerForce));
	a4->tbl_index_title = 0;
	a4->tbl_index_description = 0;
	memset(a4->player_force, 0, sizeof(a4->player_force));
	memset(a4->tbl_index_force_name, 0, sizeof(a4->tbl_index_force_name));
	memset(a4->force_flags, 0, sizeof(a4->force_flags));
	a4->version = 0;
	a4->data7 = 0;

	if (InReplay ? ReadLobbyChunks(scenarioChk, scenarioChkSize, a4) : strlen(source) != 0 && sub_4CCAC0_(source, a4))
	{
		for (int player = 0; player < _countof(LobbyPlayers); player++)
		{
			LobbyPlayers[player].dwPlayerID = player;
			LobbyPlayers[player].dwStormId = -1;
			if (LobbyPlayers[player].nRace == RaceId::RACE_Select)
			{
				LobbyPlayers[player].nRace = RaceId::RACE_Random;
				if (player < 8)
					playerForce[player] = 1;
			}
			if (player >= 8)
			{
				LobbyPlayers[player].nType = PlayerType::PT_NotUsed;
				LobbyPlayers[player].nRace = RaceId::RACE_Zerg;
				LobbyPlayers[player].nTeam = 0;
			}
		}

		sub_4A91E0_();
		sub_45AC10(a4->player_force);
		updatePlayerForce();

		strcpy_s(CurrentMapFileName, source);
		if (!is_campaign)
		{
			active_campaign = nullptr;
		}
		dword_5994DC = 1;
		const char* v13 = GetMapTblString(a4->tbl_index_title);

		strcpy_s(CurrentMapName, v13);
		return 1;
	}
	else
	{
		return 0;
	}
}

FAIL_STUB_PATCH(ReadMapData);

bool ReadCampaignMapData_(MapChunks* map_chunks)
{
	return ReadMapData_(MapdataFilenames_[CampaignIndex], map_chunks, 1) != 0;
}

FAIL_STUB_PATCH(ReadCampaignMapData);

void packetErrHandle_(DWORD last_error, int a2, char* a3, int a4, int a5)
{
	if (!outOfGame && last_error != 0x8510006A && last_error != 288 && last_error != 1223)
	{
		char buffer[256];
		SErrGetErrorStr(last_error, buffer, sizeof(buffer));
		BigPacketError_(a2, buffer, a3, a4, a5);
	}
}

void __stdcall packetErrHandle__(int a2, char* a3, int a4, int a5)
{
	DWORD last_error;

	__asm mov last_error, eax

	packetErrHandle_(last_error, a2, a3, a4, a5);
}

FUNCTION_PATCH((void*)0x4BB0B0, packetErrHandle__);

void __stdcall BWFXN_GlobalPrintText_(s_evt* evt)
{
	if (gwGameMode == GAME_RUN)
	{
		InfoMessage(0, (const char*)evt->pData);
	}
	else if (gwGameMode == GAME_GLUES && glGluesMode == GLUE_CHAT && evt->pData)
	{
		printLobbyString(0xFFFFFFFF, (char*)evt->pData);
	}
}

FAIL_STUB_PATCH(BWFXN_GlobalPrintText);

int InitializeNetworkProvider_(Char4 provider_id)
{
	SNETPROGRAMDATA provider_data;
	initializeProviderVersion(&provider_data);

	SNETPLAYERDATA user_data;
	user_data.dwSize = sizeof(SNETPLAYERDATA);
	user_data.dwUnknown = 0;
	user_data.pszPlayerName = playerName;
	user_data.pszUnknown = (char*)empty_string;

	SNETUIDATA ui_data;
	initializeProviderStruct(&ui_data);
	if (provider_id.as_number == 'BNET' && hEvent && !IsBattleNet)
	{
		SetEvent(hEvent);
		IsBattleNet = 1;
	}
	BOOL a3 = SNetInitializeProvider(provider_id.as_number, &provider_data, &user_data, &ui_data, &snet_version_data);
	ClearAndFreeCdkeyStrings((const char*)provider_data.key, (const char*)provider_data.key_owner);
	memset(&provider_data, 0, sizeof(provider_data));

	if (a3 == 0 && SErrGetLastError() != 0x85100077)
	{
		if (provider_id.as_number)
		{
			if (provider_id.as_number != 'BNET' && !outOfGame)
			{
				packetErrHandle_(SErrGetLastError(), 83, 0, 0, 0);
			}
		}
		else
		{
			BigPacketError_(82, 0, 0, 0, true);
		}
	}
	else if (a3 || GameUpgrade(&a3))
	{
		SNETCAPS caps;
		caps.size = sizeof(SNETCAPS);
		if (SNetGetProviderCaps(&caps))
		{
			if (caps.maxmessagesize >= 0x80)
			{
				MaxTurnSize = 512;
				if (caps.maxmessagesize <= 512)
				{
					MaxTurnSize = caps.maxmessagesize;
				}
				if (provider_id.as_number == 0)
				{
					LatencyCalls = 1;
				}
				else
				{
					LatencyCalls = std::clamp(caps.defaultturnsintransit, 2ul, 8ul);
				}

				NetMode.as_number = provider_id.as_number;
				if (InitializeModem(&user_data, &ui_data, &provider_data, provider_id))
				{
					clearPlayerFlags();
					if (SNetRegisterEventHandler(2, eventPlayerDropped) && SNetRegisterEventHandler(3, eventSetPlayerFlag) && SNetRegisterEventHandler(4, BWFXN_GlobalPrintText_) && SNetRegisterEventHandler(1, eventSetGameType))
					{
						return 1;
					}
					if (!outOfGame)
					{
						packetErrHandle_(SErrGetLastError(), 86, "Starcraft\\SWAR\\lang\\net_glue.cpp", 254, 1);
					}
				}
			}
			else if (!outOfGame)
			{
				doNetTBLError_(0, 0, 0, 85);
			}
		}
		else if (!outOfGame)
		{
			packetErrHandle_(SErrGetLastError(), 85, "Starcraft\\SWAR\\lang\\net_glue.cpp", 214, 0);
		}
	}
	else if (a3)
	{
		if (provider_id.as_number == 'BNET')
		{
			ResetLeagueEvent();
		}
		gwGameMode = GamePosition::GAME_EXIT;
	}
	else
	{
		SNetDestroy();
		if (hEvent && IsBattleNet)
		{
			ResetEvent(hEvent);
			IsBattleNet = 0;
		}
	}

	return 0;
}

void cleanBufferCounts_()
{
	for (int i = 0; i < _countof(arraydatabytes); i++)
	{
		arraydatabytes[i] = 0;
	}
	for (int i = 0; i < _countof(player_left); i++)
	{
		player_left[i] = 0;
	}

	Latency = 0;
	byte_6554B0 = 0;
	InGame = 0;
	turn_counter = 0;
	sgdwBytesInCmdQueue = 0;
	gameState = 1;

	if (gameData.save_timestamp == 0)
	{
		for (int i = 0; i < 8; i++)
		{
			byte_58D700[i] = 0;
		}
	}
}

FAIL_STUB_PATCH(cleanBufferCounts);

void sub_4D35A0_()
{
	NetMode.as_number = -1;
	SNetDestroy();
	if (hEvent && IsBattleNet)
	{
		ResetEvent(hEvent);
		IsBattleNet = 0;
	}
	LOWORD(dword_66FF30) = 0;
}

FAIL_STUB_PATCH(sub_4D35A0);

int CreateGame_(GameData* data)
{
	if (!is_spawn)
	{
		sub_4D35A0_();
		Char4 zero;
		zero.as_number = 0;
		if (InitializeNetworkProvider_(zero))
		{
			if (SNetCreateGame(data->player_name, "", "", 0, 0, 0, data->max_players, Players[g_LocalNationID].szName, "", &playerid))
			{
				dword_57EEC0[playerid] = g_LocalNationID;
				dword_57EE7C[playerid] = g_LocalHumanID;
				Players[g_LocalNationID].dwStormId = playerid;
				memcpy(&gameData, data, sizeof(gameData));
				isHost = 1;
				initializeSlots(playerid);
				cleanBufferCounts_();
				return 1;
			}
			if (!outOfGame)
			{
				packetErrHandle_(SErrGetLastError(), 92, "Starcraft\\SWAR\\lang\\net_glue.cpp", 577, 1);
			}
		}
		return 0;
	}
	BigPacketError_(103, 0, 0, 0, true);
	return 0;
}

FAIL_STUB_PATCH(CreateGame);

int LevelCheatInitGame_()
{
	if (!OpheliaEnabled)
	{
		return 1;
	}

	MapChunks map_chunks;

	OpheliaEnabled = 0;
	if (!playerName[0])
	{
		const char* v1 = GetNetworkTblString_(72);
		strcpy_s(playerName, v1);
	}
	if (!active_campaign)
	{
		char dest[MAX_PATH];
		strcpy_s(dest, CurrentMapFileName);
		if (!ReadMapData_(dest, &map_chunks, 0))
		{
			BigPacketError_(97, 0, 0, 0, true);
			return 0;
		}

		for (int player_index = 0; player_index < 8; player_index++)
		{
			if (Players[player_index].nType == PT_OpenSlot && player_index != 0)
			{
				Players[player_index].nType = PT_ComputerSlot;
			}
		}
		goto LABEL_21;
	}
	if (ReadCampaignMapData_(&map_chunks))
	{
	LABEL_21:
		GameData v6;
		memset(&v6, 0, sizeof(GameData));
		strcpy_s(v6.player_name, playerName);
		strcpy_s(v6.map_name, CurrentMapName);
		v6.active_human_players = 1;
		v6.max_players = 1;
		v6.game_speed = registry_options.GameSpeed;
		GotFileValues* v4 = InitUseMapSettingsTemplate_();
		if (v4)
		{
			memcpy(&v6.got_file_values, v4, sizeof(v6.got_file_values));
			SMemFree(v4, "Starcraft\\SWAR\\lang\\game.cpp", 342, 0);
			if (sub_4DBE50())
			{
				isHost = 0;
				return CreateGame_(&v6);
			}
			else
			{
				BigPacketError_(97, 0, 0, 0, true);
				return 0;
			}
		}
		else
		{
			BigPacketError_(102, 0, 0, 0, true);
			return 0;
		}
	}
	BigPacketError_(97, 0, 0, 0, true);
	return 0;
}

FAIL_STUB_PATCH(LevelCheatInitGame);

int LoadGameCreate_()
{
	if (!loadGameFileHandle)
	{
		return 1;
	}
	if (sub_4CF5F0() && sub_4DBE50())
	{
		isHost = 0;
		return CreateGame_(&gameData);
	}
	else
	{
		BigPacketError_(97, 0, 0, 0, true);
		return 0;
	}
}

FAIL_STUB_PATCH(LoadGameCreate);

int RestartGame_()
{
	if (!next_campaign_mission)
	{
		return 1;
	}
	RaceId v1 = Players[g_LocalNationID].nRace;
	next_campaign_mission = 0;
	char dest[260];
	strcpy_s(dest, CurrentMapFileName);
	MapChunks a4;
	if (ReadMapData_(dest, &a4, 1))
	{
		if (sub_4DBE50())
		{
			Players[g_LocalNationID].nRace = v1;
			isHost = 0;
			return CreateGame_(&gameData);
		}
		else
		{
			BigPacketError_(97, 0, 0, 0, true);
			return 0;
		}
	}
	else
	{
		BigPacketError_(97, 0, 0, 0, true);
		return 0;
	}
}

FAIL_STUB_PATCH(RestartGame);

int SinglePlayerMeleeInitGame_()
{
	if (multiPlayerMode)
	{
		return 0;
	}
	if (customSingleplayer[0] && (gameData.got_file_values.victory_conditions || gameData.got_file_values.starting_units || gameData.got_file_values.tournament_mode))
	{
		ClearMeleeCompSlots();
		Players[g_LocalNationID].nRace = selectedSingleplayerRace;
		for (int i = 0; i < 8; ++i)
		{
			if (single_player_opponent_races[i] != RACE_None)
			{
				append_melee_computer(single_player_opponent_races[i]);
			}
		}
	}
	return 1;
}

FAIL_STUB_PATCH(SinglePlayerMeleeInitGame);

IDirectSoundBuffer* sub_4BCA30_(SfxData sfx_id, struct_5* a2)
{
	if (SFXData_SoundFile[sfx_id])
	{
		char buff[MAX_PATH];
		sprintf_s(buff, "sound\\%s", SFXData_SoundFile[sfx_id]);
		return LoadSoundProc(buff, a2);
	}
	else
	{
		return NULL;
	}
}

FAIL_STUB_PATCH(sub_4BCA30);

void sub_4BCA80_(SfxData a1)
{
	if (direct_sound && registry_options.Sfx && a1 < SFX_MAX)
	{
		struct_5* v1 = &stru_6D1270[a1];
		v1->anonymous_1 = 81 - GetTickCount();
		if (v1->sound_buffer)
		{
			SFXData_Flags2[a1] |= 4;
		}
		else
		{
			v1->sound_buffer = sub_4BCA30_(a1, &stru_6D1270[a1]);
			if (v1->sound_buffer)
			{
				v1->anonymous_2 = sub_4BB890(v1);

				if ((SFXData_Flags2[a1] & 1) == 0)
				{
					dword_6D59FC += v1->sound_buffer_bytes;
				}
				SFXData_Flags2[a1] |= 4;
				if (dword_6D59FC > value)
				{
					sub_4BBC00(dword_6D59FC - value);
				}
			}
		}
	}
}

FAIL_STUB_PATCH(sub_4BCA80);

void playsound_init_UI_(u16* a1)
{
	if (direct_sound == NULL)
	{
		return;
	}

	if (a1)
	{
		for (int i = 0; a1[i]; ++i)
		{
			SfxData v2 = (SfxData)a1[i];

			if (v2 < SFX_MAX && registry_options.Sfx && (SFXData_Flags2[v2] & 1))
			{
				sub_4BCA80_(v2);
			}
		}
	}
	else if (dword_6D6388)
	{
		for (int i = 0; dword_6D6388[i]; ++i)
		{
			SfxData v2 = (SfxData)dword_6D6388[i];

			if (v2 < SFX_MAX)
			{
				sub_4BB8E0(v2);
			}
		}
	}

	dword_6D6388 = a1;
}

FAIL_STUB_PATCH(playsound_init_UI);

int COMMON_SFX_COUNT = 113;
int TERRAN_SFX_COUNT = 299;
int PROTOSS_SFX_COUNT = 282;
int ZERG_SFX_COUNT = 213;

u16* off_513628_[] = {
	zerg_sfx_related,
	terran_sfx_related,
	protoss_sfx_related,
};

void LoadRaceSFX_(char a1)
{
	dword_64086C = 0;
	dword_640870 = 0;
	dword_640874 = 0;
	dword_640878 = 0;
	dword_64087C = 0;
	selectionSoundCounter = 0;

	if (dword_6D5BDC == 0)
	{
		for (int i = 0; i < COMMON_SFX_COUNT; ++i)
		{
			protoss_sfx_related[i] = i + 1;
			terran_sfx_related[i] = i + 1;
			zerg_sfx_related[i] = i + 1;
		}

		for (int i = 0; i < TERRAN_SFX_COUNT; ++i)
		{
			terran_sfx_related[COMMON_SFX_COUNT + i] = i + 174;
		}
		terran_sfx_related[COMMON_SFX_COUNT + TERRAN_SFX_COUNT] = 0;

		for (int i = 0; i < PROTOSS_SFX_COUNT; ++i)
		{
			protoss_sfx_related[COMMON_SFX_COUNT + i] = i + 473;
		}
		protoss_sfx_related[COMMON_SFX_COUNT + PROTOSS_SFX_COUNT] = 0;

		for (int i = 0; i < ZERG_SFX_COUNT; ++i)
		{
			zerg_sfx_related[COMMON_SFX_COUNT + i] = i + 755;
		}
		zerg_sfx_related[COMMON_SFX_COUNT + ZERG_SFX_COUNT] = 0;

		dword_6D5BDC = 1;
	}

	playsound_init_UI_(a1 ? off_513628_[consoleIndex] : 0);
}

FAIL_STUB_PATCH(LoadRaceSFX);

const MusicTrackDescription* current_ingame_music_track = nullptr;

void LoadRaceUI_()
{
	LoadRaceSFX_(1);
	if (consoleIndex == RaceId::RACE_Zerg || consoleIndex == RaceId::RACE_Terran || consoleIndex == RaceId::RACE_Protoss)
	{
		const Race& race = Race::races()[consoleIndex];
		DlgGrp_Constructor(173, "Starcraft\\SWAR\\lang\\game.cpp", (char*)race.dialog_graphics, LoadGraphic);
		current_ingame_music_track = &race.ingame_music[0];
	}

	if (active_campaign)
	{
		current_ingame_music_track += (active_campaign_entry_index % 3);
	}
	else
	{
		if (consoleRaceSpecific == -1)
		{
			consoleRaceSpecific = rand() % 3;
		}
		consoleRaceSpecific = consoleRaceSpecific + 1;
		if (consoleRaceSpecific >= 3)
		{
			consoleRaceSpecific = 0;
		}

		current_ingame_music_track += consoleRaceSpecific;
	}
}

FAIL_STUB_PATCH(LoadRaceUI);

void hotkeyRemapping_()
{
	sub_49BB90();
	MoveToTile.y = -1;
	MoveToTile.x = -1;
	LOWORD(unknown_y_tile) = -1;
	LOWORD(unknown_x_tile) = -1;
	ScreenY = -1;
	ScreenX = -1;
	if (!loadGameFileHandle)
	{
		sub_4AABF0();
		memset(dword_57FE60, 0, 0x1B00u);
		memset(word_63FE40, 0, 0x80u);
		byte_597280 = -1;
		sub_4C3ED0();
		for (int i = 0; i < _countof(byte_58D718); i++)
		{
			byte_58D718[i] = 0x3;
		}
	}
	is_placing_order = 0;
	sub_48D700();
	if (is_placing_building)
	{
		refreshPlaceBuildingLocation();
		if (is_placing_order)
		{
			CancelTargetOrder();
		}
	}
	SetInGameInputProcs_();
	initializeTriggerInfo();
	memset(stru_64DEC8, 0, sizeof(stru_64DEC8));
	byte_6D1224 = 0;
	dword_63FF5C = 0;
	dword_64EEC8 = 0;
	dword_64DEA8 = 0;
	load_Stat_txt();
}

FAIL_STUB_PATCH(hotkeyRemapping);

void setup_HUD_();

int LoadGameCore_()
{
	memset(PlayerSelection, 0, sizeof(PlayerSelection));
	memset(playersSelections, 0, sizeof(playersSelections));
	dword_59724C[0] = 0;
	dword_59724C[1] = 0;
	dword_59724C[2] = 0;
	setAlliance();
	if (!loadGameFileHandle || loadGameFull())
	{
		setup_HUD_();
		dword_51CE8C[0] = -GetTickCount();
		resetLastInputFrameCounts();
		memcpy(dword_596B7C, playersSelections[g_LocalHumanID], sizeof(dword_596B7C));
		memset(&playersSelections, 0, sizeof(playersSelections));
		return 1;
	}
	else
	{
		BigPacketError_(98, 0, 0, 0, true);
		return 0;
	}
}

FAIL_STUB_PATCH(LoadGameCore);

GameActionDataBlock* allocateRepGameActionMemory_(GameActionDataBlock* a1)
{
	a1->net_record_size = 50000;
	a1->net_record_buffer = SMemAlloc(a1->net_record_size, "Starcraft\\SWAR\\lang\\net_record.cpp", 334, 0);
	a1->field4 = 0;
	a1->field6 = 0;
	a1->field7 = -1;
	a1->field1 = 1;
	a1->field8 = 0;
	return a1;
}

FAIL_STUB_PATCH(allocateRepGameActionMemory);

void __fastcall FreeGameActionData_(bool exit_code)
{
	if (replayData)
	{
		SMemFree(replayData->net_record_buffer, "Starcraft\\SWAR\\lang\\net_record.cpp", 348, 0);
		SMemFree(replayData, "delete", -1, 0);
		replayData = NULL;
	}
}

FAIL_STUB_PATCH(FreeGameActionData);

void __cdecl sub_4CDFC0_()
{
	replayData->field2 = 0;
}

FAIL_STUB_PATCH(sub_4CDFC0);

void sub_4CDFD0_()
{
	replayData->field1 = 0;
	replayData->field2 = 1;
	replayData->field8 = replayData->net_record_buffer;
}

FAIL_STUB_PATCH(sub_4CDFD0);

void __cdecl createNewGameActionDataBlock_()
{
	FreeGameActionData_(false);
	GameActionDataBlock* v1 = (GameActionDataBlock*)SMemAlloc(32, "new", -1, 0);
	replayData = v1 ? allocateRepGameActionMemory_(v1) : NULL;
	AppAddExit_(FreeGameActionData_);
}

FUNCTION_PATCH(createNewGameActionDataBlock, createNewGameActionDataBlock_);

bool sub_4CE220_(FILE* file)
{
	BOOL result = DecompressRead(&replayData->field4, 4, file);
	if (!result)
	{
		return 0;
	}
	else
	{
		if (replayData->field4 > 0)
		{
			while (replayData->field4 > replayData->net_record_size)
			{
				sub_4CDD30(replayData);
			}
			result = DecompressRead(replayData->net_record_buffer, replayData->field4, file);
			if (!result)
			{
				return 0;
			}
		}
		return result;
	}
}

FAIL_STUB_PATCH(sub_4CE220);

int LoadReplayFile_(const char* filename, int* a3)
{
	if (a3)
	{
		*a3 = 0;
	}
	FILE* replay_file = fopen(filename, "rb");
	if (replay_file == NULL)
	{
		return 0;
	}

	int v6 = 0;
	if (!DecompressRead(&v6, 4, replay_file) || v6 != 'SRer')
	{
		fclose(replay_file);
		return 0;
	}

	if (!DecompressRead(&replay_header, 633, replay_file) || !replay_header.game_data.is_replay)
	{
		fclose(replay_file);
		return 0;
	}

	if (a3 && replay_header.is_expansion)
	{
		*a3 = 1;
	}

	if (replay_header.is_expansion && !IsExpansion)
	{
		fclose(replay_file);
		return 0;
	}

	CampaignIndex = replay_header.campaign_index;
	createNewGameActionDataBlock();

	if (!sub_4CE220_(replay_file))
	{
		fclose(replay_file);
		return 0;
	}

	if (!DecompressRead(&scenarioChkSize, 4, replay_file))
	{
		fclose(replay_file);
		return 0;
	}

	freeChkFileMem_();
	scenarioChk = SMemAlloc(scenarioChkSize, "Starcraft\\SWAR\\lang\\replay.cpp", 1084, 0);
	int v5 = DecompressRead(scenarioChk, scenarioChkSize, replay_file);
	if (!v5)
	{
		SMemFree(scenarioChk, "Starcraft\\SWAR\\lang\\replay.cpp", 1088, 0);
		scenarioChk = 0;
	}

	fclose(replay_file);
	return v5;
}

int __cdecl LoadReplayFile__()
{
	char* filename;
	int* a3;

	__asm mov filename, eax
	__asm mov a3, edi

	return LoadReplayFile_(filename, a3);
}

FUNCTION_PATCH((void*)0x4DF570, LoadReplayFile__);

int chooseTRGTemplate_()
{
	VictoryConditions v0 = gameData.got_file_values.victory_conditions;
	if ((gameData.got_file_values.player_types == PT_NO_COMPUTERS || gameData.got_file_values.player_types == PT_SINGLE_NO_COMPUTERS)
		&& (gameData.got_file_values.victory_conditions || gameData.got_file_values.starting_units || gameData.got_file_values.tournament_mode))
	{
		beginComputerAIScriptExecution();
		v0 = gameData.got_file_values.victory_conditions;
	}
	else
	{
		for (int i = 0; i < 8; i++)
		{
			*(_WORD*)&AIScriptController[i].flags |= 0x20u;
		}
	}
	switch (v0)
	{
	case VC_MELEE:
		return loadTRGFile("triggers\\Melee.trg");
	case VC_SUSDDEN_DEATH:
		return loadTRGFile("triggers\\SuddenDeath.trg");
	case VC_ONE_ON_ONE:
		return loadTRGFile("triggers\\Melee.trg");
	case VC_RESOURCES:
		if (gameData.got_file_values.variation_value > 0x1D4Cu)
		{
			if (gameData.got_file_values.variation_value == 10000)
			{
				return loadTRGFile("triggers\\Greed10000.trg");
			}
		}
		else
		{
			switch (gameData.got_file_values.variation_value)
			{
			case 0x1D4C:
				return loadTRGFile("triggers\\Greed7500.trg");
			case 0x9C4:
				return loadTRGFile("triggers\\Greed2500.trg");
			case 0x1388:
				return loadTRGFile("triggers\\Greed5000.trg");
			}
		}
		return 0;
	case VC_SLAUGHTER:
		switch (gameData.got_file_values.variation_value)
		{
		case 0xF:
			return loadTRGFile("triggers\\Slaughter15.trg");
		case 0x1E:
			return loadTRGFile("triggers\\Slaughter30.trg");
		case 0x2D:
			return loadTRGFile("triggers\\Slaughter45.trg");
		case 0x3C:
			return loadTRGFile("triggers\\Slaughter60.trg");
		}
		return 0;
	case VC_CAPTURE_THE_FLAG:
		sub_4ABA20();
		break;
	}
	return 1;
}

FAIL_STUB_PATCH(chooseTRGTemplate);

signed int LoadGameInit_()
{
	stopMusic_();
	if (InReplay)
	{
		if (!scenarioChk)
		{
			LoadReplayFile_(CurrentMapFileName, NULL);
		}
		if (InReplay)
		{
			for (int i = 0; i < 8; i++)
			{
				playerForce[i] = replay_header.playerForce[i];
			}
		}
	}
	if (!loadGameFileHandle)
	{
		ElapsedTimeFrames = 0;
	}
	if (!LOBYTE(multiPlayerMode))
	{
		if (!LevelCheatInitGame_() || !LoadGameCreate_() || !RestartGame_() || !SinglePlayerMeleeInitGame_())
		{
			return 0;
		}
		if (InReplay)
		{
			initialSeed = replay_header.seed_related.initial_seed;
		}
		else
		{
			initialSeed = time(0);
			replay_header.seed_related.initial_seed = initialSeed;
		}
	}
	InitializeDialogScreenLayer_();
	ButtonPressSound = mouseOver_Loading_CB;
	LoadGameFonts();
	memset(randomCounts, 0, 0x400u);
	randomCountsTotal = 0;
	LastRandomNumber = initialSeed;
	srand(initialSeed);
	AllocAIPathPool();
	AppAddExit_(nullsub_1);
	if (!loadGameFileHandle)
		initializePlayerColours();
	BWFXN_RandomizePlayerRaces();
	if (InReplay)
		getReplPlayerStructs(Players);
	savePlayerSlotTypesAndRace();
	if (!InReplay)
	{
		if (loadGameFileHandle)
			InitializeLoadedGameSlots();
		else
			RandomizeSlotsForcesColors();
	}
	BWFXN_InitializePlayerConsole();
	initializeDefaultPlayerNames_();
	memset(randomCounts, 0, 0x400u);
	randomCountsTotal = 0;
	LastRandomNumber = initialSeed;
	srand(initialSeed);
	LoadRaceUI_();
	hotkeyRemapping_();
	if (!GameInit_())
		return 0;
	if (InReplay)
		getReplPlayerColors(factionsColorsOrdering);
	if (!loadGameFileHandle)
		sub_49B060();

	int v6;
	if (!InReplay
		&& !active_campaign
		&& !LoadFileArchiveToSBigBuf_(CurrentMapFileName, &v6, 1, &mapArchiveHandle)
		&& !gameData.got_file_values.victory_conditions
		&& !gameData.got_file_values.starting_units
		&& !gameData.got_file_values.tournament_mode)
	{
		if (loadGameFileHandle)
		{
			BigPacketError_(99, CurrentMapFileName, 0, 0, 1);
			return 0;
		}
		BigPacketError_(97, 0, 0, 0, 1);
		return 0;
	}
	GameCheats = (CheatFlags)((int)GameCheats & (CHEAT_NoGlues | CHEAT_Ophelia | 0x8000000)); // TODO: fix the mess with the flags
	if (!LoadGameCore_())
		return 0;
	if (!mapStarted)
	{
		if (!chooseTRGTemplate_())
		{
			BigPacketError_(93, 0, 0, 0, 1);
			return 0;
		}
		if (!mapStarted)
		{
			*(_DWORD *)playerHasLeft = 0;
			lossType = 0;
			*(_DWORD *)&unkVictoryVariable = 0;
		}
	}
	sub_4B2DF0();
	if (!LOBYTE(multiPlayerMode))
		TickCounterInit();
	saveLoadSuccess = (unsigned __int8)mapStarted;
	elapstedTimeModifier = mapStarted != 0 ? savedElapsedSeconds : 0;
	SetGameSpeed_maybe(registry_options.GameSpeed, 0, 1u);
	if (InReplay)
	{
		copyPlayerStructsToReplayPlayerStructs(Players, &gameData);
		dword_6D5BF0 = 0;
		sub_4CDFD0_();
		ReplayVision = 255;
		playerVisions = 255;
		replayShowEntireMap = 0;
		nextReplayCommandFrame = -1;
		playerExploredVisions = 65280;
	}
	else
	{
		createNewGameActionDataBlock_();
	}
	return 1;
}

FAIL_STUB_PATCH(sub_49CC10);
FAIL_STUB_PATCH(LoadGameInit);

void registerMenuFunctions_(FnInteract* functions, dialog* a2, int functions_size);

int loadOKBIN_(int a1, const char* message, HANDLE a3)
{
	dword_6D1244 = a1;
	dword_5128F0 = a1;

	size_t message_size = strlen(message) + 1;
	okcancel_message = (char*) SMemAlloc(message_size, "Starcraft\\SWAR\\lang\\okcancel.cpp", 163, 0);
	strcpy_s(okcancel_message, message_size, message);

	dword_6D1248 = a3;

	dialog* okcancel_bin = LoadDialog("ok.bin");
	registerMenuFunctions_(0, okcancel_bin, 0);
	return gluLoadBINDlg_(okcancel_bin, okcancel_Interact);
}

FAIL_STUB_PATCH(loadOKBIN);

void* getFullMapChunk_(char* filename, int* chk_size)
{
	int loader_index;
	*chk_size = 0;
	void* v2 = read_chk_data(filename, &loader_index, chk_size);

	if (!v2)
	{
		if (!mapArchiveHandle)
		{
			return 0;
		}
		SFileCloseArchive(mapArchiveHandle);
		mapArchiveHandle = 0;
		return 0;
	}
	if (mapArchiveHandle)
	{
		SFileCloseArchive(mapArchiveHandle);
		mapArchiveHandle = 0;
	}
	return v2;
}

FAIL_STUB_PATCH(getFullMapChunk);

int SaveReplay_(const char* a1, int a3)
{
	CHAR FileName[MAX_PATH];
	if (!getDirectoryPath(FileName, sizeof(FileName), a1))
	{
		return 0;
	}

	if (a3 && !DeleteFileA(FileName) && GetFileAttributesA(FileName) != -1)
	{
		char buff[256];
		const char* var = GetNetworkTblString_(2);
		sprintf_s(buff, var, &byte_51BFB8);
		loadOKBIN_(1, buff, dword_6D0F2C);
		return -1;
	}

	FILE* v4 = fopen(FileName, "wb+");
	if (v4 == NULL)
	{
		return 0;
	}

	int v5;
	ReplayHeader a1a;

	replay_header.campaign_index = CampaignIndex;
	replay_header.game_data.is_replay = 1;
	replay_header.is_expansion = IsExpansion;
	memcpy(&a1a, &replay_header, sizeof(ReplayHeader));
	a3 = 'SRer';
	v5 = CompressWrite(&a3, 4, v4);
	if (v5)
	{
		v5 = CompressWrite(&a1a, 633, v4);
		if (v5)
		{
			v5 = WriteGameActions(v4, replayData);
			if (v5)
			{
				int chk_size;
				void* v6 = getFullMapChunk_(CurrentMapFileName, &chk_size);
				if (v6)
				{
					v5 = CompressWrite(&chk_size, 4, v4);
					if (v5)
					{
						v5 = CompressWrite(v6, chk_size, v4);
					}
					SMemFree(v6, "Starcraft\\SWAR\\lang\\replay.cpp", 940, 0);
				}
				else
				{
					v5 = 0;
				}
			}
		}
	}
	fclose(v4);
	if (!v5)
	{
		DeleteFileA(FileName);
	}
	return v5 != 0;
}

FAIL_STUB_PATCH(SaveReplay);

void __cdecl freeChkFileMem_()
{
	if (scenarioChk)
	{
		SMemFree(scenarioChk, "Starcraft\\SWAR\\lang\\replay.cpp", 1106, 0);
		scenarioChk = nullptr;
	}
}

FUNCTION_PATCH(freeChkFileMem, freeChkFileMem_);

struct SAI_PathsEx
{
	__int16 regionCount;
	u16 unknown;
	void* globalBuffer_ptr;
	void* splitTiles_end;
	u16 mapTileRegionId[MAX_MAP_DIMENTION][MAX_MAP_DIMENTION];
	SaiSplit splitTiles[25000];
	SaiRegion regions[5000];
	u16 globalBuffer[10000];
	SaiContourHub* contours;
};

SAI_PathsEx*& SAIPathingEx = *((decltype(&SAIPathingEx))0x6d5bfc);

void AllocateSAI_Paths_()
{
	SAIPathingEx = (SAI_PathsEx*)SMemAlloc(sizeof(SAI_PathsEx), "Starcraft\\SWAR\\lang\\sai_PathCreate.cpp", 210, 0);
	memset(SAIPathingEx, 0, sizeof(SAI_PathsEx));
}

FAIL_STUB_PATCH(AllocateSAI_Paths);

void FreeSAI_Paths_()
{
	SMemFree(SAIPathingEx, "Starcraft\\SWAR\\lang\\sai_PathCreate.cpp", 226, 0);
	SAIPathingEx = NULL;
}

FAIL_STUB_PATCH(FreeSAI_Paths);

void destroy_textbox_bin_()
{
	if (textbox_bin)
	{
		DestroyDialog(textbox_bin);
		textbox_bin = NULL;
	}
}

FAIL_STUB_PATCH(destroy_textbox_bin);

void destroy_statf10_bin_()
{
	if (stat_f10_Dlg)
	{
		DestroyDialog(stat_f10_Dlg);
		stat_f10_Dlg = NULL;
	}
}

FAIL_STUB_PATCH(destroy_statf10_bin);

void destroy_wirefram_grp_()
{
	if (wirefram_grp)
	{
		SMemFree(wirefram_grp, "Starcraft\\SWAR\\lang\\statwire.cpp", 367, 0);
		wirefram_grp = NULL;
	}
}

FAIL_STUB_PATCH(destroy_wirefram_grp);

void destroyStatdata_()
{
	if (stardata_Dlg)
	{
		DestroyDialog(stardata_Dlg);
		stardata_Dlg = NULL;
	}

	if (dword_68C1FC)
	{
		SMemFree(dword_68C1FC, "Starcraft\\SWAR\\lang\\statdata.cpp", 1360, 0);
		dword_68C1FC = NULL;
	}

	if (dword_68C1F4)
	{
		SMemFree(dword_68C1F4, "Starcraft\\SWAR\\lang\\statdata.cpp", 1362, 0);
		dword_68C1F4 = NULL;
	}
}

FAIL_STUB_PATCH(destroyStatdata);

void destroyStatsesBin_()
{
	if (statres_Dlg)
	{
		DestroyDialog(statres_Dlg);
		statres_Dlg = NULL;
	}

	if (game_icons_grp)
	{
		SMemFree(game_icons_grp, "Starcraft\\SWAR\\lang\\statres.cpp", 507, 0);
		game_icons_grp = NULL;
	}
}

FAIL_STUB_PATCH(destroyStatsesBin);

void destroy_statlb_dlg_()
{
	if (statlb_Dlg)
	{
		DestroyDialog(statlb_Dlg);
		statlb_Dlg = NULL;
	}
}

FAIL_STUB_PATCH(destroy_statlb_dlg);

void destroy_statbtn_BIN_()
{
	if (current_dialog)
	{
		DestroyDialog(current_dialog);
		current_dialog = NULL;
	}

	if (cmdbtns_grp)
	{
		SMemFree(cmdbtns_grp, "Starcraft\\SWAR\\lang\\statcmd.cpp", 1107, 0);
		cmdbtns_grp = NULL;
	}

	if (cmdicons_grp)
	{
		SMemFree(cmdicons_grp, "Starcraft\\SWAR\\lang\\statcmd.cpp", 1109, 0);
		cmdicons_grp = NULL;
	}
}

FAIL_STUB_PATCH(destroy_statbtn_BIN);

void clearSelectionPortrait_()
{
	if (has_hud)
	{
		if (video)
		{
			SVidPlayEnd(video);
			video = NULL;
		}

		if (statport_Dlg)
		{
			DestroyDialog(statport_Dlg);
			statport_Dlg = NULL;
		}
	}

	if (dword_6D5C9C)
	{
		SMemFree(dword_6D5C9C, "Starcraft\\SWAR\\lang\\statport.cpp", 882, 0);
		dword_6D5C9C = NULL;
	}
}

FAIL_STUB_PATCH(clearSelectionPortrait);

StatFlufDialog* statfluf_dialogs_[] = {
	statfluf_zerg,
	statfluf_terran,
	statfluf_protoss,
	statfluf_neutral,
};

void destroy_statfluf_bin_()
{
	for (StatFlufDialog* i = statfluf_dialogs_[statfluf_current_race]; i->position.left != -1; ++i)
	{
		if (i->dialog)
		{
			DestroyDialog(i->dialog);
			i->dialog = NULL;
		}
	}
}

FAIL_STUB_PATCH(destroy_statfluf_bin);

void destroyGameHUD_()
{
	if (has_hud)
	{
		if (minimap_Dlg)
		{
			DestroyDialog(minimap_Dlg);
			minimap_Dlg = NULL;
		}

		if (blink_grp)
		{
			SMemFree(blink_grp, "Starcraft\\SWAR\\lang\\minimap.cpp", 2065, 0);
			blink_grp = NULL;
		}

		destroy_textbox_bin_();
		destroy_statf10_bin_();
		destroy_wirefram_grp_();
		destroyStatdata_();
		destroyStatsesBin_();
		destroy_statlb_dlg_();
		destroy_statbtn_BIN_();
		clearSelectionPortrait_();
		destroy_statfluf_bin_();
	}

	refreshSelectionScreen_();

	if (StatTxtTbl.buffer)
	{
		SMemFree(StatTxtTbl.buffer, "Starcraft\\SWAR\\lang\\status.cpp", 236, 0);
		StatTxtTbl.buffer = NULL;
	}

	updateConsoleImage(&MainBltMask->pvoid0);
	ActivePortraitUnit = NULL;
	memset(ClientSelectionGroup, 0, sizeof(ClientSelectionGroup));
}

FAIL_STUB_PATCH(destroyGameHUD);

void DestroyFogSightData_()
{
	for (int i = 0; i < 12; i++)
	{
		if (line_of_sight[i].tiles)
		{
			SMemFree((void*) line_of_sight[i].tiles, "Starcraft\\SWAR\\lang\\los.cpp", 333, 0);
			line_of_sight[i].tiles = 0;
		}
	}

	if (dword_6D5C0C)
	{
		SMemFree(dword_6D5C0C, "Starcraft\\SWAR\\lang\\mask.cpp", 280, 0);
		dword_6D5C0C = 0;
	}
	if (dword_6D5C10)
	{
		SMemFree(dword_6D5C10, "Starcraft\\SWAR\\lang\\mask.cpp", 285, 0);
		dword_6D5C10 = 0;
	}
	if (dword_6D5C14)
	{
		SMemFree(dword_6D5C14, "Starcraft\\SWAR\\lang\\mask.cpp", 290, 0);
		dword_6D5C14 = 0;
	}
	if (dword_6D5C18)
	{
		SMemFree(dword_6D5C18, "Starcraft\\SWAR\\lang\\mask.cpp", 297, 0);
		dword_6D5C18 = 0;
	}
}

FAIL_STUB_PATCH(DestroyFogSightData);

void DestroyMapData_()
{
	if (dword_6D5CD8)
	{
		SMemFree(dword_6D5CD8, "Starcraft\\SWAR\\lang\\repulse.cpp", 315, 0);
		dword_6D5CD8 = NULL;
	}
	if (VR4Data)
	{
		SMemFree(VR4Data, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 470, 0);
		VR4Data = NULL;
	}
	if (VX4Data)
	{
		SMemFree(VX4Data, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 471, 0);
		VX4Data = NULL;
	}
	if (TileSetMap)
	{
		SMemFree(TileSetMap, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 472, 0);
		TileSetMap = NULL;
	}
	if (MiniTileFlags)
	{
		SMemFree(MiniTileFlags, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 473, 0);
		MiniTileFlags = NULL;
	}
	if (active_tiles)
	{
		SMemFree(active_tiles, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 475, 0);
		active_tiles = NULL;
	}
	if (dword_5993A0)
	{
		SMemFree(dword_5993A0, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 476, 0);
		dword_5993A0 = NULL;
	}
	if (GameTerrainCache)
	{
		SMemFree(GameTerrainCache, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 477, 0);
		GameTerrainCache = NULL;
	}
	if (CellMap)
	{
		SMemFree(CellMap, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 478, 0);
		CellMap = NULL;
	}
	if (MapTileArray)
	{
		SMemFree(MapTileArray, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 479, 0);
		MapTileArray = NULL;
	}
	if (megatile_default_flags)
	{
		SMemFree(megatile_default_flags, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 480, 0);
		megatile_default_flags = NULL;
	}
	ZergCreepArray = NULL;

	DestroyFogSightData_();
	if (spkHandle)
	{
		SMemFree(spkHandle, "Starcraft\\SWAR\\lang\\scroll.cpp", 550, 0);
		spkHandle = NULL;
	}
	TransDestroy();
	if (!dword_5993AC && dword_6D125C)
	{
		SMemFree(dword_6D125C, "Starcraft\\SWAR\\lang\\light.cpp", 121, 0);
		dword_6D125C = NULL;
	}

	for (int i = 1; i < _countof(colorShift); i++)
	{
		if (colorShift[i].data)
		{
			SMemFree(colorShift[i].data, "Starcraft\\SWAR\\lang\\light.cpp", 167, 0);
			colorShift[i].data = NULL;
		}
	}
}

FAIL_STUB_PATCH(DestroyMapData);

void CleanupFlingyDat_()
{
	// TODO: dynamically clean flingyDat memory
	delete[] flingyDat_;
}

FAIL_STUB_PATCH(CleanupFlingyDat);

void CleanupSpritesDat_()
{
	// TODO: dynamically clean spritesDat memory
	delete[] spritesDat_;
}

FAIL_STUB_PATCH(CleanupSpritesDat);

void ReportGameResult_()
{
	char details_info[2048];
	char header_info[1024];
	int results[8];

	if (!game_result_reported_maybe && !InReplay)
	{
		ApplyGameVictoryStatus(results, 0);
		if (!endgameData_(header_info, sizeof(header_info), details_info, sizeof(details_info)))
		{
			details_info[0] = 0;
			header_info[0] = 0;
		}
		SNetReportGameResult(0, 8, results, header_info, details_info);
		if (league_maybe ? &league_maybe : 0)
		{
			char* player_names[8];
			SNetGetPlayerNames(player_names);
			GameResultText(results, player_names, header_info, details_info);
		}
		game_result_reported_maybe = 1;
	}
}

FAIL_STUB_PATCH(ReportGameResult);

void sub_460F70_()
{
	if (multiPlayerMode && NetMode.as_number == 'BNET' && !dword_685174)
	{
		dword_685174 = 1;
		ReportGameResult_();
	}
}

FAIL_STUB_PATCH(sub_460F70);

void sub_484D90_()
{
	if (is_placing_building)
	{
		refreshPlaceBuildingLocation();
	}
	if (is_placing_order)
	{
		CancelTargetOrder();
	}
	SetInGameInputProcs_();
}

FAIL_STUB_PATCH(sub_484D90);

void lmissionDlgActivate_()
{
	dword_68517C = 1;

	if (LastControlID == -2)
	{
		lmissionInitSelf(GamePosition::GAME_LOSE);
	}
}

FAIL_STUB_PATCH(lmissionDlgActivate);

int __fastcall lmission_DLG_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
	case EVN_MOUSEMOVE:
	case EVN_LBUTTONDOWN:
	case EVN_LBUTTONUP:
	case EVN_LBUTTONDBLCLK:
	case EVN_RBUTTONDOWN:
	case EVN_RBUTTONUP:
	case EVN_RBUTTONDBLCLK:
	case EVN_CHAR:
		genericDlgInteract(dlg, evt);
		return 1;
	case EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			lmissionDlgCreate(dlg);
			break;
		case USER_DESTROY:
			waitLoopCntd(0, dlg);
			msgfltr_Cancel(0);
			genericPopupDlgInteract(dlg, evt);
			dword_685178 = 0;
			return 1;
		case USER_ACTIVATE:
			lmissionDlgActivate_();
			break;
		case USER_UNK_8:
			if (*(_DWORD*)&evt->wSelection == 0)
			{
				lmissionInitSelf(GamePosition::GAME_LOSE);
				DestroyDialog(dlg);
				return 1;
			}
			break;
		}
	}

LABEL_4:
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(lmission_DLG_Interact);

void open_lose_mission_dialog_()
{
	if (!dword_685178 && !dword_68517C)
	{
		sub_460F70_();
		dword_685178 = 1;
		sub_484D90_();

		if (end_mission_prompt)
		{
			LastControlID = 0;
			BWFXN_OpenGameDialog_("rez\\lmission.bin", lmission_DLG_Interact_);
		}
		else
		{
			lmissionInitSelf(GamePosition::GAME_LOSE);
			dword_68517C = 1;
			dword_685178 = 0;
		}
	}
}

FAIL_STUB_PATCH(open_lose_mission_dialog);

void wmissionDlgActivate_()
{
	dword_68517C = 1;

	if (LastControlID == -2)
	{
		lmissionInitSelf(GamePosition::GAME_WIN);
	}
	else if (LastControlID == 1)
	{
		InfoMessage(0, GetNetworkTblString_(16));
	}
}

FAIL_STUB_PATCH(wmissionDlgActivate);

int __fastcall wmission_BINDLG_Main_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_KEYFIRST:
	case EventNo::EVN_KEYRPT:
	case EventNo::EVN_MOUSEMOVE:
	case EventNo::EVN_LBUTTONDOWN:
	case EventNo::EVN_LBUTTONUP:
	case EventNo::EVN_LBUTTONDBLCLK:
	case EventNo::EVN_RBUTTONDOWN:
	case EventNo::EVN_RBUTTONUP:
	case EventNo::EVN_RBUTTONDBLCLK:
	case EventNo::EVN_CHAR:
		genericDlgInteract(dlg, evt);
		return 1;
	case EventNo::EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			sub_461160(dlg);
			break;
		case EventUser::USER_DESTROY:
			msgfltr_Cancel(0);
			genericPopupDlgInteract(dlg, evt);
			dword_685178 = 0;
			return 1;
		case EventUser::USER_ACTIVATE:
			wmissionDlgActivate_();
			break;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(wmission_BINDLG_Main);

void open_win_mission_dialog_()
{
	if (!dword_685178 && !dword_68517C)
	{
		sub_460F70_();
		dword_685178 = 1;
		sub_484D90_();

		if (end_mission_prompt)
		{
			LastControlID = 0;
			BWFXN_OpenGameDialog_("rez\\wmission.bin", wmission_BINDLG_Main_);
		}
		else
		{
			lmissionInitSelf(GamePosition::GAME_WIN);
			dword_68517C = 1;
			dword_685178 = 0;
		}
	}
}

FAIL_STUB_PATCH(open_win_mission_dialog);

void load_endmission_()
{
	if (!InReplay && word_650970-- == 0)
	{
		word_650970 = 45;
		if (byte_58D700[g_LocalNationID] == 2)
		{
			open_lose_mission_dialog_();
		}
		else if (byte_58D700[g_LocalNationID] == 3 || byte_58D700[g_LocalNationID] == 5)
		{
			open_win_mission_dialog_();
		}
	}
}

FAIL_STUB_PATCH(load_endmission);

void DestroyGame_()
{
	if (isInGame)
	{
		countdownTimeTickCount_0 = GetTickCount();
		isInGame = 0;
	}
	if (multiPlayerMode && NetMode.as_number == 'BNET')
	{
		ReportGameResult_();
	}
	leaveGame(0x40000001);
	if (gwGameMode == GamePosition::GAME_GLUES && glGluesMode == MenuPosition::GLUE_MAIN_MENU)
	{
		initializePlayerValues();
	}
	memset(cycle_colors, 0, sizeof(cycle_colors));
	if (!multiPlayerMode)
	{
		TickCountSomething(1);
	}
	if (GameMenuDlg)
	{
		DestroyDialog(GameMenuDlg);
		GameMenuDlg = 0;
		if (gwGameMode == GamePosition::GAME_RUN)
		{
			hAccTable = DlgAccelerator;
			input_procedures[EventNo::EVN_SYSCHAR] = AcceleratorTables;
		}
		byte_6D1214 = 0;
	}
	if (has_effects_scode_maybe)
	{
		BWFXN_GameEndTarget();
		has_effects_scode_maybe = 0;
	}
	layer* v0 = &ScreenLayers[3];
	do
	{
		v0->buffers = 0;
		++v0;
	} while ((int)v0 <= (int)&ScreenLayers[5]);
	Streamed_SFX_FullDestructor(&soundFXList);
	for (int i = 0; i < _countof(stru_51A280); i++)
	{
		TriggerNode_Destructor(stru_51A280 + i);
	}
	struct_0* v2 = placement_boxes;
	do
	{
		if (v2->field_0)
		{
			SMemFree(v2->field_0, "Starcraft\\SWAR\\lang\\placebox.cpp", 578, 0);
			v2->field_0 = 0;
		}
		++v2;
	} while ((int)v2 < (int)playerReplayWatchers);
	sub_484D90_();
	destroyGameHUD_();
	DestroyMapData_();
	if (dword_6BEE8C)
	{
		SMemFree(dword_6BEE8C, "Starcraft\\SWAR\\lang\\Sai_path.cpp", 792, 0);
		dword_6BEE8C = 0;
	}
	if (SAIPathingEx)
	{
		SaiContourHub* v4 = SAIPathingEx->contours;
		if (v4)
		{
			sai_contoursCreate_Cleanup(v4->contours);
			SMemFree(v4, "Starcraft\\SWAR\\lang\\sai_PathCreate.cpp", 333, 0);
			SAIPathingEx->contours = 0;
		}
		FreeSAI_Paths_();
	}
	if (aiscript_bin_data)
	{
		SMemFree(aiscript_bin_data, "Starcraft\\SWAR\\lang\\SAI_Scripts.cpp", 1546, 0);
		aiscript_bin_data = 0;
	}
	if (aiscript_bin)
	{
		SMemFree(aiscript_bin, "Starcraft\\SWAR\\lang\\SAI_Scripts.cpp", 1551, 0);
		aiscript_bin = 0;
	}
	freeAICaptains();

	// TODO: dynamically clean unitsDat memory
	// TODO: dynamically clean ordersDat memory
	CleanupFlingyDat_();
	CleanupSpritesDat_();

	stopAllSound_();
	stopSounds();
	LoadRaceSFX_(0);
	InitializeInputProcs_();
	if (pylon_power_mask)
	{
		SMemFree(pylon_power_mask, "Starcraft\\SWAR\\lang\\CUnitProtoss.cpp", 102, 0);
	}
	pylon_power_mask = 0;
	if (MapStringTbl.buffer)
	{
		SMemFree(MapStringTbl.buffer, "Starcraft\\SWAR\\lang\\maphdr.cpp", 267, 0);
	}
	MapStringTbl.buffer = 0;
	if (mapArchiveHandle)
	{
		SFileCloseArchive(mapArchiveHandle);
		mapArchiveHandle = 0;
	}
	dword_6D5A60 = (int)InReplay;
	if (InReplay)
	{
		sub_4CDFC0_();
		freeChkFileMem_();
		InReplay = 0;
		game_id_hash = 0;
	}
	else
	{
		SaveReplay_("LastReplay", 1);
		if (league_maybe)
		{
			char a1[260];
			createLeagueFile(a1);
			SNetSendReplayPath(a1, game_id_hash, validation_replay_path[0] != 0 ? validation_replay_path : NULL);
		}
		game_id_hash = 0;
	}
}

FAIL_STUB_PATCH(DestroyGame);

void updateActiveTileInfo_()
{
	int v7 = 0;

	for (int y = 0; y < map_size.height; y++)
	{
		for (int x = 0; x < map_size.width; x++)
		{
			int i = y * map_size.width + x;

			if (InReplay ? (ReplayVision & ~active_tiles[i]) : (playerVisions & active_tiles[i]) == 0)
			{
				if (x - (unsigned int)MoveToTile.x >= 0x15 || y - (unsigned int)MoveToTile.y >= 0xE)
				{
					u16 v4 = TileSetMap[MapTileArray[i].group].megaTileRef[MapTileArray[i].tile];
					if (v4 != (CellMap[i] & 0x7FFF))
					{
						CellMap[i] = v4;
						if (MapTileArray[i].group == 1)
						{
							active_tiles[i] |= LOCAL_CREEP;
						}
						else
						{
							active_tiles[i] &= ~LOCAL_CREEP;
						}
						iterateDirectionalCreepData(BWFXN_CreepManagementCB, 0, x, y);
						v7 = 1;
					}
				}
			}
		}
	}

	if (v7)
	{
		HasMegatileUpdate = 1;
	}
}

FAIL_STUB_PATCH(updateActiveTileInfo);

MegatileFlags VISIBLE_ALL =
	MegatileFlags::VISIBLE_PLAYER_1 | MegatileFlags::VISIBLE_PLAYER_2 |
	MegatileFlags::VISIBLE_PLAYER_3 | MegatileFlags::VISIBLE_PLAYER_4 |
	MegatileFlags::VISIBLE_PLAYER_5 | MegatileFlags::VISIBLE_PLAYER_6 |
	MegatileFlags::VISIBLE_PLAYER_7 | MegatileFlags::VISIBLE_PLAYER_8;

MegatileFlags EXPLORED_ALL =
	MegatileFlags::EXPLORED_PLAYER_1 | MegatileFlags::EXPLORED_PLAYER_2 |
	MegatileFlags::EXPLORED_PLAYER_3 | MegatileFlags::EXPLORED_PLAYER_4 |
	MegatileFlags::EXPLORED_PLAYER_5 | MegatileFlags::EXPLORED_PLAYER_6 |
	MegatileFlags::EXPLORED_PLAYER_7 | MegatileFlags::EXPLORED_PLAYER_8;

void RemoveFoWCheat_()
{
	if (multiPlayerMode)
	{
		GameCheats &= ~(CHEAT_BlackSheepWall | CHEAT_WarAintWhatItUsedToBe);
	}
	if ((ScreenLayers[5].bits & 1) == 0)
	{
		ScreenLayers[5].bits |= 2;
	}

	if ((GameCheats & (CHEAT_BlackSheepWall | CHEAT_WarAintWhatItUsedToBe)) == 0)
	{
		RefreshLayer5();
		for (int i = 0; i < map_size.width * map_size.height; i++)
		{
			active_tiles[i] |= VISIBLE_ALL;
		}
	}
}

FAIL_STUB_PATCH(RemoveFoWCheat);

int sub_42DB50_(CUnit* unit, Position* a4)
{
	char v54 = GetAngle((__int16)unit->sprite->position.x, (__int16)unit->sprite->position.y, (__int16)unit->nextMovementWaypoint.x, (__int16)unit->nextMovementWaypoint.y);
	u8 v55 = unit->velocityDirection1;
	int v48 = (unsigned __int8)getModifiedUnitTurnRadius(unit);
	int a5 = unit->current_speed2;
	int v50 = unit->halt.x;
	int v49 = unit->halt.y;
	int v41 = unit->flingyTopSpeed;
	int v15 = 0;
	int v16 = 0;
	int v51 = 1;

	if (unit->flingyMovementType != 2)
	{
		for (int v45 = 0; v45 < 30; v45++)
		{
			if (v54 != v55)
			{
				v54 -= v55;
				int v29 = v48 / 2;
				if (v54 < v48 / 2)
				{
					if (v54 > v48 / -2)
					{
						LOBYTE(v29) = v54;
					}
					else
					{
						LOBYTE(v29) = v48 / -2;
					}
				}
				v55 += v29;
				v54 = GetAngle((__int16)unit->sprite->position.x, (__int16)unit->sprite->position.y, (__int16)unit->nextMovementWaypoint.x, (__int16)unit->nextMovementWaypoint.y);
				v51 = 1;
			}
			if (a5 < v41)
			{
				a5 += (unsigned __int16)getModifiedUnitAcceleration(unit);
				if (a5 > v41)
				{
					a5 = v41;
				}
				v15 = (a5 * AngleDistance[v55].x) >> 8;
				v16 = (a5 * AngleDistance[v55].y) >> 8;
				v51 = 0;
				goto LABEL_47;
			}
			if (v51)
			{
				v15 = (a5 * AngleDistance[v55].x) >> 8;
				v16 = (a5 * AngleDistance[v55].y) >> 8;
				v51 = 0;
				goto LABEL_47;
			}
		LABEL_47:
			v50 += v15;
			v49 += v16;
		}
	}
	else
	{
		program_state = unit->sprite->pImagePrimary->iscript_program;
		if (program_state.anim != Anims::AE_Walking)
		{
			sub_42D600_(Anims::AE_Walking);
			CUnit* v43 = iscript_unit;
			iscript_unit = unit;
			BWFXN_PlayIscript_(unit->sprite->pImagePrimary, &program_state, 1, &a5);
			iscript_unit = v43;
		}

		for (int i = 0; i < 30; i++)
		{
			if (v55 != v54)
			{
				int v20 = v54 - v55;
				if (v20 >= v48)
				{
					v20 = v48;
				}
				else if (v20 <= -v48)
				{
					v20 = -(char)v48;
				}
				v55 += v20;
				v51 = 1;
			}

			if (v55 == v54 && a5)
			{
				if (v51)
				{
					v15 = (a5 * AngleDistance[v55].x) >> 8;
					v16 = (a5 * AngleDistance[v55].y) >> 8;
					v51 = 0;
				}
				v50 += v15;
				v49 += v16;
			}
			a5 = 0;
			CUnit* v42 = iscript_unit;
			iscript_unit = unit;
			BWFXN_PlayIscript_(unit->sprite->pImagePrimary, &program_state, 1, &a5);
			iscript_unit = v42;
		}
	}

	a4->x = v50 >> 8;
	a4->y = v49 >> 8;
	return 1;
}

int __stdcall sub_42DB50__(int a2, int a3, Position* a4)
{
	CUnit* unit;

	__asm mov unit, ebx

	return sub_42DB50_(unit, a4);
}

FUNCTION_PATCH((void*)0x42DB50, sub_42DB50__);

int UMInitSeq_(CUnit* unit)
{
	if (unit->statusFlags & NoBrkCodeStart)
	{
		return 0;
	}
	unit->movementState = UnitMovementState::UM_Init;
	return 1;
}

FAIL_STUB_PATCH(UMInitSeq);

int UMHidden_()
{
	dword_66FF70 = 0;
	return 0;
}

FAIL_STUB_PATCH(UMHidden);

int UMScoutFree_(CUnit* unit)
{
	unit->movementState = UnitMovementState::UM_FollowPath;
	return 1;
}

FAIL_STUB_PATCH(UMScoutFree);

int UMStartPath_(CUnit* unit)
{
	if (UMAnotherPath(unit, unit->moveTarget.pt))
	{
		if ((unit->userActionFlags & 2) && unit->flingyMovementType == 2)
		{
			unit->path->byte19 = RandBetween(51, 0, 2);
			unit->movementState = UnitMovementState::UM_UIOrderDelay;
		}
		else
		{
			unit->movementState = UnitMovementState::UM_FollowPath;
		}
		unit->userActionFlags &= ~2;

		return 1;
	}
	else
	{
		unit->movementState = UnitMovementState::UM_FailedPath;
		return 0;
	}
}

FAIL_STUB_PATCH(UMStartPath);

int UMFollowPath_(CUnit* unit)
{
	if (DestinationAndCollisionCheck(unit, 1))
	{
		return 1;
	}
	if (!UMAnotherPath(unit, unit->moveTarget.pt))
	{
		unit->movementState = UnitMovementState::UM_AnotherPath;
		return 1;
	}

	sub_495EE0(unit);
	s32 v13 = unit->flingyMovementType ? unit->current_speed2 : unit->current_speed1;
	CUnit* v14 = (unit->statusFlags & StatusFlags::IsNormal) ? FindCollidingUnit(unit) : 0;
	if (v14)
	{
		if (TerrainCollision(unit))
		{
			goto LABEL_18;
		}
		if (unit->current_speed.x < 32 && unit->current_speed.y < 32)
		{
			goto LABEL_18;
		}
		if (dword_63FF2C != unit->current_speed1)
		{
			goto LABEL_18;
		}
		int v6 = dword_63FED8;
		int v5 = dword_63FF40;
		__int16 v12 = dword_63FED4;
		__int16 v11 = dword_63FECC;

		dword_63FED8 = unit->halt.x + unit->current_speed.x / 2;
		dword_63FF40 = unit->halt.y + unit->current_speed.y / 2;
		LOWORD(dword_63FED4) = dword_63FED8 >> 8;
		LOWORD(dword_63FECC) = dword_63FF40 >> 8;
		if (sub_4F2240(unit))
		{
			dword_63FED8 = unit->halt.x + unit->current_speed.x / 4;
			dword_63FF40 = unit->halt.y + unit->current_speed.y / 4;
			LOWORD(dword_63FED4) = dword_63FED8 >> 8;
			LOWORD(dword_63FECC) = dword_63FF40 >> 8;
			if (sub_4F2240(unit))
			{
				dword_63FED8 = v6;
				dword_63FF40 = v5;
				LOWORD(dword_63FED4) = v12;
				LOWORD(dword_63FECC) = v11;

			LABEL_18:
				unit->path->colliding_unit_id = CUnitToUnitID(v14);
				unit->path->speed = v13;
				unit->movementState = UnitMovementState::UM_FixCollision;
				return 1;
			}
		}
	}
	else if (TerrainCollision(unit))
	{
		unit->path->speed = v13;
		unit->movementState = UnitMovementState::UM_FixTerrain;
		return 1;
	}

	if ((_WORD)dword_63FED4 != unit->sprite->position.x || (_WORD)dword_63FECC != unit->sprite->position.y)
	{
		u8 v10 = unit->pathingCollisionInterval;
		if (v10 > 2)
		{
			unit->pathingCollisionInterval = 2;
		}
		else if (v10)
		{
			unit->pathingCollisionInterval = v10 - 1;
		}
	}
	MoveUnit_Partial(unit);
	if (isUnitMovableAndAtDestination(unit))
	{
		unit->movementState = UnitMovementState::UM_AtMoveTarget;
	}
	else if (unitSubtractPathDelayAndCheck(unit))
	{
		unitSetPathDelay(unit, 30);
		unit->movementState = UnitMovementState::UM_ScoutPath;
	}
	return 0;
}

FAIL_STUB_PATCH(UMFollowPath);

void Unit_ExecPathingState_(CUnit* unit)
{
	dword_66FF70 = visionUpdated != 0;
	while (2)
	{
		int v2;

		switch (unit->movementState)
		{
		case UnitMovementState::UM_Init:
			v2 = UMInitialize(unit);
			break;
		case UnitMovementState::UM_InitSeq:
			v2 = UMInitSeq_(unit);
			break;
		case UnitMovementState::UM_Turret:
			v2 = UMTurret(unit);
			break;
		case UnitMovementState::UM_Bunker:
			v2 = UMBunker(unit);
			break;
		case UnitMovementState::UM_BldgTurret:
			v2 = UMBldgTurret(unit);
			break;
		case UnitMovementState::UM_Hidden:
			v2 = UMHidden_();
			break;
		case UnitMovementState::UM_Flyer:
			v2 = UMFlyer(unit);
			break;
		case UnitMovementState::UM_AtRest:
			v2 = UMAtRest(unit);
			break;
		case UnitMovementState::UM_Dormant:
			v2 = UMDormant(unit);
			break;
		case UnitMovementState::UM_AtMoveTarget:
			v2 = UMAtMoveTarget(unit);
			break;
		case UnitMovementState::UM_CheckIllegal:
			v2 = UMCheckIllegal(unit);
			break;
		case UnitMovementState::UM_MoveToLegal:
			v2 = UMMoveToLegal(unit);
			break;
		case UnitMovementState::UM_LumpWannabe:
			v2 = UMLumpWannabe(unit);
			break;
		case UnitMovementState::UM_FailedPath:
			unit->pathingCollisionInterval = 10;
			UMFailedPath(unit);
			unit->movementState = UM_RetryPath;
			return;
		case UnitMovementState::UM_RetryPath:
			v2 = UMRetryPath(unit);
			break;
		case UnitMovementState::UM_StartPath:
			v2 = UMStartPath_(unit);
			break;
		case UnitMovementState::UM_UIOrderDelay:
			v2 = UMUIOrderDelay(unit);
			break;
		case UnitMovementState::UM_TurnAndStart:
			v2 = UMTurnAndStart(unit);
			break;
		case UnitMovementState::UM_FaceTarget:
			v2 = UMFaceTarget(unit);
			break;
		case UnitMovementState::UM_NewMoveTarget:
			v2 = UMNewMoveTarget(unit);
			break;
		case UnitMovementState::UM_AnotherPath:
			unit->movementState = UMAnotherPath(unit, unit->moveTarget.pt) != 0 ? UM_FollowPath : UM_FailedPath;
			return;
		case UnitMovementState::UM_Repath:
			v2 = UMRepath(unit);
			break;
		case UnitMovementState::UM_RepathMovers:
			v2 = UMRepathMovers(unit);
			break;
		case UnitMovementState::UM_FollowPath:
			v2 = UMFollowPath_(unit);
			break;
		case UnitMovementState::UM_ScoutPath:
			v2 = UMScoutPath(unit);
			break;
		case UnitMovementState::UM_ScoutFree:
			v2 = UMScoutFree_(unit);
			break;
		case UnitMovementState::UM_FixCollision:
			v2 = UMFixCollision(unit);
			break;
		case UnitMovementState::UM_WaitFree:
			v2 = UMWaitFree(unit);
			break;
		case UnitMovementState::UM_GetFree:
			v2 = UMGetFree(unit);
			break;
		case UnitMovementState::UM_SlidePrep:
			v2 = UMSlidePrep(unit);
			break;
		case UnitMovementState::UM_SlideFree:
			v2 = UMSlideFree(unit);
			break;
		case UnitMovementState::UM_ForceMoveFree:
			v2 = UMForceMoveFree(unit);
			break;
		case UnitMovementState::UM_FixTerrain:
			v2 = UMFixTerrain(unit);
			break;
		case UnitMovementState::UM_TerrainSlide:
			v2 = UMTerrainSlide(unit);
			break;
		default:
			return;
		}

		if (v2 == 0)
		{
			return;
		}
	}
}

FAIL_STUB_PATCH(Unit_ExecPathingState);

void sub_4EBC30_(CUnit* unit)
{
	u8 v2 = unit->velocityDirection1;
	Unit_ExecPathingState_(unit);
	if (dword_66FF70)
	{
		refreshUnitVision(unit);
	}

	CUnit* subunit = unit->subUnit;
	if ((unit->statusFlags & Completed) && subunit && (Unit_PrototypeFlags[unit->unitType] & Subunit) == 0)
	{
		point p;
		ProgressSubunitDirection(subunit, unit->velocityDirection1 - v2);
		getImageAttackFrame(&p, unit->sprite, 0, 2);
		int x = unit->halt.x;
		int y = unit->halt.y;
		subunit->halt.x = x;
		subunit->halt.y = y;
		subunit->position.x = x >> 8;
		subunit->position.y = y >> 8;
		sub_497A10_(subunit->sprite, subunit->position.x, subunit->position.y);
		ISCRIPT_setPosition_(subunit->sprite->pImagePrimary, LOBYTE(p.x), LOBYTE(p.y));
		iscript_unit = subunit;
		if ((unit->movementFlags & 2) == 0 && (subunit->statusFlags & StatusFlags::UNKNOWN6))
		{
			subunit->statusFlags &= ~StatusFlags::UNKNOWN6;
			if ((unit->statusFlags & StatusFlags::IsABuilding) != 0 && (subunit->movementFlags & 8) == 0)
			{
				for (CImage* image = subunit->sprite->pImageHead; image; image = image->next)
				{
					PlayIscriptAnim_(image, AE_WalkingToIdle);
				}
			}
		}
		else if ((unit->movementFlags & 2) && (subunit->statusFlags & StatusFlags::UNKNOWN6) == 0)
		{
			unit->subUnit->statusFlags |= StatusFlags::UNKNOWN6;
			if ((subunit->movementFlags & 8) == 0)
			{
				for (CImage* image = subunit->sprite->pImageHead; image; image = image->next)
				{
					PlayIscriptAnim_(image, AE_Walking);
				}
			}
		}
		sub_4EBC30_(unit->subUnit);
		iscript_unit = unit;
	}
}

FAIL_STUB_PATCH(sub_4EBC30);

void RefreshUnit_(CUnit* unit)
{
	if ((Unit_PrototypeFlags[unit->unitType] & Subunit) == 0 && (unit->sprite->flags & 0x20) == 0)
	{
		RefreshSprite(unit->sprite, byte_581D6A[unit->playerID]);
	}
}

FAIL_STUB_PATCH(RefreshUnit);

void spriteToIscriptLoop_(CSprite* sprite)
{
	CImage* next_image;

	for (CImage* image = sprite->pImageHead; image; image = next_image)
	{
		next_image = image->next;

		image->updateFunction(image);
		BWFXN_PlayIscript_(image, &image->iscript_program, 0, 0);
		if (sprite->pImageHead == nullptr)
		{
			int v2 = (__int16)sprite->position.y / 32;
			if (v2 < 0)
			{
				v2 = 0;
			}
			else if (v2 >= map_size.height)
			{
				v2 = map_size.height - 1;
			}

			if (_SpritesOnTileRow.heads[v2] == sprite)
			{
				_SpritesOnTileRow.heads[v2] = sprite->next;
			}
			if (_SpritesOnTileRow.tails[v2] == sprite)
			{
				_SpritesOnTileRow.tails[v2] = sprite->prev;
			}
			if (sprite->prev)
			{
				sprite->prev->next = sprite->next;
			}
			CSprite* v3 = sprite->next;
			if (v3)
			{
				v3->prev = sprite->prev;
			}
			sprite->prev = 0;
			sprite->next = 0;

			CSprite* v4 = UnusedSprites;
			if (UnusedSprites)
			{
				if (dword_63FE34 == UnusedSprites)
				{
					dword_63FE34 = sprite;
				}
				sprite->prev = UnusedSprites;
				sprite->next = v4->next;
				CSprite* v5 = v4->next;
				if (v5)
				{
					v5->prev = sprite;
				}
				v4->next = sprite;
			}
			else
			{
				dword_63FE34 = sprite;
				UnusedSprites = sprite;
			}
		}
	}
}

void spriteToIscriptLoop__()
{
	CSprite* sprite;

	__asm mov sprite, esi

	spriteToIscriptLoop_(sprite);
}

FUNCTION_PATCH((void*) 0x497920, spriteToIscriptLoop__);

void sub_4EB5E0_(CUnit* a1)
{
	if (a1->sprite && (a1->sprite->flags & 0x20))
	{
		SpriteDestructor_(a1->sprite);
		a1->sprite = nullptr;
	}

	if (a1->sprite)
	{
		spriteToIscriptLoop_(a1->sprite);
		if (!a1->sprite->pImageHead)
		{
			a1->sprite = nullptr;
		}
	}
	if (a1->statusFlags & UNKNOWN2)
	{
		if (dword_658AE4)
		{
			--dword_658AE4;
			return;
		}
		dword_658AE4 = 3;
		if (!sub_47DF90(a1))
		{
			return;
		}
	}

	if (!a1->sprite)
	{
		removeUnitFromList(a1);
	}
}

FAIL_STUB_PATCH(sub_4EB5E0);

u8 getVisibilityMaskFromPositionAndSize_(__int16 x, __int16 y, __int16 width, __int16 height)
{
	u8 result = ~0;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			result &= active_tiles[(y + i) * map_size.width + x + j];
		}
	}
	return ~result;
}

FAIL_STUB_PATCH(getVisibilityMaskFromPositionAndSize);

bool isThingyOnMap_(int x, int y, CThingy* thingy)
{
	CSprite* v5 = thingy->sprite;
	if (v5 && (v5->flags & 0x20) == 0)
	{
		__int64 v6 = (__int16)v5->position.x - x / 2;
		int v8 = ((BYTE4(v6) & 0x1F) + (int)v6) >> 5;
		int v9 = ((__int16)v5->position.y - y / 2) / 32;
		int v10 = (unsigned int)(x + 31) >> 5;
		int v11 = (unsigned int)(y + 31) >> 5;
		if (v8 < 0)
		{
			v10 += v8;
			if (v10 <= 0)
			{
				return v5->visibilityFlags != 0;
			}
			v8 = 0;
		}
		if (v9 < 0)
		{
			v11 += v9;
			if (v11 <= 0)
			{
				return v5->visibilityFlags != 0;
			}
			v9 = 0;
		}
		if (v10 + v8 > map_size.width)
		{
			v10 = map_size.width - v8;
			if (v10 <= 0)
			{
				return v5->visibilityFlags != 0;
			}
		}
		if (v11 + v9 > map_size.height)
		{
			v11 = map_size.height - v9;
			if (v11 <= 0)
			{
				return v5->visibilityFlags != 0;
			}
		}
		u8 v13 = getVisibilityMaskFromPositionAndSize_(v8, v9, v10, v11);

		if (v5->visibilityFlags != v13)
		{
			char v17 = playerVisions & thingy->sprite->visibilityFlags;
			refreshAllVisibleImagesAtScreenPosition(thingy->sprite, v13);
			if (v17 && ((unsigned __int8)playerVisions & thingy->sprite->visibilityFlags) == 0)
			{
				return 1;
			}
		}
	}

	return 0;
}

FAIL_STUB_PATCH(isThingyOnMap);

void UpdateUnitSpriteInfo_(CUnit* unit)
{
	bool v1 = isThingyOnMap_(Unit_Placement[unit->unitType].x, Unit_Placement[unit->unitType].y, (CThingy*)unit);

	if (unit->subUnit && (unit->subUnit->sprite->flags & 0x20) == 0)
	{
		refreshAllVisibleImagesAtScreenPosition(unit->subUnit->sprite, unit->sprite->visibilityFlags);
	}

	if (v1)
	{
		if (unit->sprite->flags & 8)
		{
			updateBuildingLandUnitSelection(unit);
		}
		removeSelectionCircle(unit->sprite);
		if (unit->sprite->flags & 6)
		{
			unit->sprite->flags &= ~6;
			removeSelectionCircleFromSprite(unit->sprite);
		}
		if ((unit->statusFlags & GoundedBuilding) || unit->unitType >= Special_Floor_Missile_Trap && unit->unitType <= Special_Right_Wall_Flame_Trap)
		{
			if (unit->sprite && (unit->orderID || unit->orderState != 1))
			{
				sub_488410(unit->unitType, unit->sprite);
			}
		}
	}
}

void UpdateUnitSpriteInfo__()
{
	CUnit* unit;

	__asm mov unit, esi

	UpdateUnitSpriteInfo_(unit);
}

FUNCTION_PATCH((void*)0x4EBE10, UpdateUnitSpriteInfo__);

void updateUnitTimers_(CUnit* unit)
{
	if (unit->mainOrderTimer)
	{
		unit->mainOrderTimer -= 1;
	}
	if (unit->groundWeaponCooldown)
	{
		unit->groundWeaponCooldown -= 1;
	}
	if (unit->airWeaponCooldown)
	{
		unit->airWeaponCooldown -= 1;
	}
	if (unit->spellCooldown)
	{
		unit->spellCooldown -= 1;
	}

	if (Unit_ShieldsEnabled[unit->unitType])
	{
		int v8 = Unit_MaxShieldPoints[unit->unitType] << 8;
		if (unit->shieldPoints != v8)
		{
			unit->shieldPoints += 7;
			if (unit->shieldPoints > v8)
			{
				unit->shieldPoints = v8;
			}
			if (unit->sprite->flags & 8)
			{
				setAllImageGroupFlagsPal11(unit->sprite);
			}
		}
	}

	if ((unit->unitType == Zerg_Zergling || unit->unitType == Hero_Devouring_One) && !unit->groundWeaponCooldown)
	{
		unit->orderQueueTimer = 0;
	}
	unit->isBeingHealed = 0;
	if ((unit->statusFlags & Completed) || (unit->sprite->flags & 0x20) == 0)
	{
		unit->status.cycleCounter += 1;
		if (unit->status.cycleCounter >= 8)
		{
			unit->status.cycleCounter = 0;
			updateUnitStatusTimers(unit);
		}
	}
	if (unit->statusFlags & Completed)
	{
		if (SLOBYTE(Unit_PrototypeFlags[unit->unitType]) < 0)
		{
			if (unit->hitPoints > 0 && unit->hitPoints != Unit_MaxHitPoints[unit->unitType])
			{
				SetUnitHp(unit, unit->hitPoints + 4);
			}
		}
		UpdateEnergyTimer(unit);
		if (unit->recentOrderTimer)
		{
			unit->recentOrderTimer -= 1;
		}
		u16 v17;
		if (unit->status.removeTimer == 0 || --unit->status.removeTimer)
		{
			u8 v19 = Unit_GroupFlags[unit->unitType];
			if ((v19 & 5) == 0 && (v19 & 2) != 0 && ((unit->statusFlags & GoundedBuilding) || (Unit_PrototypeFlags[unit->unitType] & FlyingBuilding)))
			{
				if (unitHPbelow33_percent(unit))
				{
					DamageUnitHp(20, unit, 0, unit->lastAttackingPlayer, 1);
				}
			}
		}
		else
		{
			RemoveUnit(unit);
		}
	}
}

FAIL_STUB_PATCH(updateUnitTimers);

void playSpriteIscript_(CSprite* sprite, Anims animation, int a3)
{
	for (CImage* image = sprite->pImageHead; image; image = image->next)
	{
		if (a3 || (sprite->flags & 0x80u) == 0)
		{
			PlayIscriptAnim_(image, animation);
		}
	}
}

void __stdcall playSpriteIscript__(Anims animation, int a3)
{
	CSprite* sprite;

	__asm mov sprite, ecx

	playSpriteIscript_(sprite, animation, a3);
}

FUNCTION_PATCH((void*)0x499D00, playSpriteIscript__);

void orders_Warpin_(CUnit* unit)
{
	if (unit->orderState == 0)
	{
		if (unit->orderSignal & 1)
		{
			unit->orderSignal &= 0xFE;
			ReplaceSpriteOverlayImage(unit->sprite, Sprites_Image[unit->sprite->spriteID], unit->currentDirection1);
			for (CImage* i = unit->sprite->pImageHead; i; i = i->next)
			{
				PlayIscriptAnim_(i, AE_WarpIn);
			}
			unit->orderState = 1;
		}
	}
	else if (unit->orderState == 1)
	{
		if (unit->orderSignal & 1)
		{
			unit->orderSignal &= 0xFE;
			if (Unit_PrototypeFlags[unit->unitType] & PermanentCloak)
			{
				PlaySoundFromDirect(unit, SFX_Terran_PHOENIX_TPhClo00);
			}
			actUnitReturnToIdle(unit);
		}
	}
}

FAIL_STUB_PATCH(orders_Warpin);

void ordersEntries_(CUnit* unit)
{
	switch (unit->orderID)
	{
	case Order::ORD_DIE:
		BWFXN_KillUnitTarget(unit);
		break;
	case Order::ORD_BUILDSELFPROTOSS:
		orders_bldgUnderConstruction_Protoss_(unit);
		break;
	case Order::ORD_NUKE_TRACK:
		orders_NukeTrack(unit);
		break;
	case Order::ORD_PROTOSS_WARP_IN:
		orders_Warpin_(unit);
		break;
	default:
		if ((unit->statusFlags & StatusFlags::DoodadStatesThing) || unit->status.lockdownTimer || unit->status.stasisTimer || unit->status.maelstromTimer)
		{
			getMaelstromTarget(unit);
		}
		else
		{
			if ((unit->statusFlags & StatusFlags::IsABuilding) == 0 && (unit->statusFlags & StatusFlags::CanNotAttack))
			{
				getMaelstromTarget(unit);
			}
			switch (unit->orderID)
			{
			case Order::ORD_TURRET_GUARD:
				orders_TurretGuard(unit);
				break;
			case Order::ORD_TURRET_ATTACK:
				orders_TurretAttack(unit);
				break;
			case Order::ORD_DRONE_BUILD:
				orders_DroneBuild(unit);
				break;
			case Order::ORD_BUILD:
				orders_SCVBuild(unit);
				break;
			case Order::ORD_BUILDPROTOSS:
				orders_ProbeBuild(unit);
				break;
			case Order::ORD_CONSTRUCT_BLDG:
				orders_SCVBuild2(unit);
				break;
			case Order::ORD_REPAIR:
				orders_Repair1(unit);
				break;
			case Order::ORD_ZERG_BIRTH:
				orders_ZergBirth(unit);
				break;
			case Order::ORD_MORPH:
				orders_Morph1(unit);
				break;
			case Order::ORD_BUILDSELF:
				orders_TerranBuildSelf(unit);
				break;
			case Order::ORD_BUILDSELFZERG:
				orders_ZergBuildSelf(unit);
				break;
			case Order::ORD_STRAFE_UNIT_SCARAB:
				orders_StrafeUnit2(unit);
				break;
			case Order::ORD_RECHARGE_UNIT:
				orders_RechargeShields1(unit);
				break;
			case Order::ORD_BLDG_LAND:
				orders_BuildingLand(unit);
				break;
			case Order::ORD_BLDG_LIFT_OFF:
				orders_BuildingLiftoff(unit);
				break;
			case Order::ORD_RESEARCH_TECH:
				orders_ResearchTech(unit);
				break;
			case Order::ORD_UPGRADE:
				orders_Upgrade(unit);
				break;
			case Order::ORD_HARVEST_INTERRUPT:
				orders_HarvestOreInterrupted(unit);
				break;
			case Order::ORD_HARVEST4:
				orders_HarvestWTF(unit);
				break;
			case Order::ORD_HARVEST_INTERRUPT2:
				Orders_HarvestInterrupt(unit);
				break;
			case Order::ORD_SIEGEMODE:
				orders_SiegeMode(unit);
				break;
			case Order::ORD_TANKMODE:
				orders_TankMode(unit);
				break;
			case Order::ORD_SUMMON_ARCHON:
				orders_WarpingArchon(unit);
				break;
			case Order::ORD_SUMMON_COMPLETE:
				orders_CompletingArchonSummon(unit);
				break;
			case Order::ORD_NUKE_TRAIN:
				orders_NukeTrain(unit);
				break;
			case Order::ORD_ARBITER:
				orders_cloakNearbyUnits(unit);
				break;
			case Order::ORD_UNDO_FLYER:
				orders_ResetCollision1(unit);
				break;
			case Order::ORD_UNDO_TRANSPARENT_WORKER:
				orders_ResetCollision2(unit);
				break;
			case Order::ORD_CTF_COP_2:
				orders_CTFCOP2(unit);
				break;
			case Order::ORD_SELF_DESTRUCT:
				RemoveUnit(unit);
				break;
			case Order::ORD_CRITTER:
				orders_Critter(unit);
				break;
			case Order::ORD_MEDIC_HEAL:
				orders_MedicHeal1(unit);
				break;
			case Order::ORD_MEDIC_HEALMOVE:
				orders_HealMove(unit);
				break;
			case Order::ORD_MEDIC_HOLDPOS:
				orders_MedicHoldPosition(unit);
				break;
			case Order::ORD_MEDIC_RETURN_TO_IDLE:
				orders_MedicHeal2(unit);
				break;
			case Order::ORD_SUMMON_DARCHON:
				orders_WarpingDarkArchon(unit);
				break;
			default:
				break;
			}

			if (!unit->orderQueueTimer--)
			{
				unit->orderQueueTimer = 8;
				getTargetSomething(unit);
				unit->AIActionFlag = 0;
				switch (unit->orderID)
				{
				case Order::ORD_STOP:
					orders_stop(unit);
					break;
				case Order::ORD_GUARD:
					orders_Guard(unit);
					break;
				case Order::ORD_GUARD_PLAY:
					orders_guard_play(unit);
					break;
				case Order::ORD_IGNORE:
				case Order::ORD_CARRIER_IGNORE:
					orders_ignore(unit);
					break;
				case Order::ORD_TRILOBYTE_STOP:
					orders_ReaverStop(unit);
					break;
				case Order::ORD_MOVE_ATTACK_UNIT:
				case Order::ORD_DRONE_INFEST2:
				case Order::ORD_REPAIR_MOVE:
				case Order::ORD_CARRIER_ATTACK_MOVE:
				case Order::ORD_REAVER_ATTACK_MOVE:
				case Order::ORD_HARVEST_MOVE:
				case Order::ORD_YAMATO_MOVE:
					orders_Special(unit);
					break;
				case Order::ORD_ATTACK_UNIT:
					orders_AttackUnit(unit);
					break;
				case Order::ORD_ATTACK_FIXED_RNGE:
					orders_AttackFixedRange(unit);
					break;
				case Order::ORD_ATTACK_MOVE:
					orders_AttackMove(unit);
					break;
				case Order::ORD_DRONE_INFEST0:
					orders_InfestMine1(unit);
					break;
				case Order::ORD_POWERUP0:
					orders_Powerup1(unit);
					break;
				case Order::ORD_TOWER_GUARD:
					orders_tower_guard(unit);
					break;
				case Order::ORD_TOWER_ATTACK:
					orders_tower_attack(unit);
					break;
				case Order::ORD_VULTURE_MINE:
					orders_vulture_mine(unit);
					break;
				case Order::ORD_STAY_IN_RANGE:
					orders_StayInRange(unit);
					break;
				case Order::ORD_DRONE_START_BUILD:
					orders_DroneStartBuild(unit);
					break;
				case Order::ORD_DRONE_INFEST:
					orders_drone_infest(unit);
					break;
				case Order::ORD_QUEEN_INFEST:
					orders_InfestMine4(unit);
					break;
				case Order::ORD_PYLON_BUILD:
					orders_pylon_build(unit);
					break;
				case Order::ORD_PLACE_ADDON:
					orders_PlaceAddon(unit);
					break;
				case Order::ORD_ZERG_BLDG_MORPH:
					orders_Morph2(unit);
					break;
				case Order::ORD_BUILDNYDUS:
					orders_Build5(unit);
					break;
				case Order::ORD_ENTER_NYDUS:
					orders_EnterNydusCanal(unit);
					break;
				case Order::ORD_FOLLOW:
					orders_follow(unit);
					break;
				case Order::ORD_CARRIER:
				case Order::ORD_CARRIER_FIGHT:
				case Order::ORD_CARRIER_HOLDPOS:
					orders_Carrier(unit);
					break;
				case Order::ORD_CARRIER_STOP:
					orders_carrier_stop(unit);
					break;
				case Order::ORD_CARRIER_ATTACK:
					orders_CarrierAttack1(unit);
					break;
				case Order::ORD_CARRIER_IGNORE2:
					orders_carrier_ignore2(unit);
					break;
				case Order::ORD_REAVER:
				case Order::ORD_REAVER_FIGHT:
				case Order::ORD_REAVER_HOLD:
					orders_Reaver(unit);
					break;
				case Order::ORD_REAVER_ATTACK:
					orders_ReaverAttack1(unit);
					break;
				case Order::ORD_STRAFE_UNIT:
					orders_StrafeUnit(unit);
					break;
				case Order::ORD_RECHARGE:
					orders_RechargeShields2(unit);
					break;
				case Order::ORD_RETURN:
					orders_return(unit);
					break;
				case Order::ORD_DRONE_LAND:
					orders_DroneLand(unit);
					break;
				case Order::ORD_LIFT_OFF:
					orders_lift_off(unit);
					break;
				case Order::ORD_LARVA:
					orders_Larva(unit);
					break;
				case Order::ORD_HARVEST:
				case Order::ORD_HARVEST_GAS:
					orders_harvest(unit);
					break;
				case Order::ORD_ENTER_GAS:
					orders_enter_gas(unit);
					break;
				case Order::ORD_IN_GAS:
					orders_EnterExitGas(unit);
					break;
				case Order::ORD_RETURN_GAS:
				case Order::ORD_RETURN_MINERALS:
					orders_return_resource(unit);
					break;
				case Order::ORD_HARVEST_MINERALS:
					orders_MoveToHarvestMinerals(unit);
					break;
				case Order::ORD_HARVEST_MINERALS_WAIT:
					orders_CanHarvestMinerals(unit);
					break;
				case Order::ORD_HARVESTING_MINERALS:
					orders_HarvestingMinerals(unit);
					break;
				case Order::ORD_ENTER_TRANSPORT:
					orders_EnterTransport(unit);
					break;
				case Order::ORD_PICK_UP:
					orders_TransportIdle(unit);
					break;
				case Order::ORD_PICK_UP_MOBILE:
					transportIdleOpen(unit);
					break;
				case Order::ORD_PICK_UP_STATIONARY:
					orders_Pickup3_0(unit);
					break;
				case Order::ORD_PICK_UP_UNK:
					orders_Pickup4_0(unit);
					break;
				case Order::ORD_POWERUP:
					orders_Powerup2(unit);
					break;
				case Order::ORD_WATCHTARGET:
					orders_WatchTarget(unit);
					break;
				case Order::ORD_INIT_SPREAD_CREEP:
					orders_InitCreepGrowth(unit);
					break;
				case Order::ORD_STOP_SPREAD_CREEP:
					orders_StopCreepGrowth(unit);
					break;
				case Order::ORD_HOLDPOS:
					orders_hold_position(unit);
					break;
				case Order::ORD_HOLDPOS_QUEEN:
				case Order::ORD_SAP_HOLDPOS:
					if (!unit->orderState)
					{
						orders_HoldPositionSuicidal(unit);
						unit->orderState = 1;
					}
					[[fallthrough]];
				case Order::ORD_NOTHING:
					if (unit->orderQueueHead)
					{
						PrepareForNextOrderFunc(unit);
					}
					break;
				case Order::ORD_UNLOAD:
					orders_Unload(unit);
					break;
				case Order::ORD_MOVE_UNLOAD:
					orders_MoveUnload(unit);
					break;
				case Order::ORD_YAMATO:
				case Order::ORD_MAGNAPULSE:
				case Order::ORD_DARK_SWARM:
				case Order::ORD_CAST_PARASITE:
				case Order::ORD_SUMMON_BROODLINGS:
				case Order::ORD_EMP_SHOCKWAVE:
				case Order::ORD_PSI_STORM:
				case Order::ORD_IRRADIATE:
				case Order::ORD_PLAGUE:
				case Order::ORD_CONSUME:
				case Order::ORD_ENSNARE:
				case Order::ORD_STASIS:
				case Order::ORD_MEDIC_CURE:
				case Order::ORD_CAST_DISRUPTOR:
				case Order::ORD_MYOPIA:
				case Order::ORD_PARALIZE:
					orders_Spell(unit);
					break;
				case Order::ORD_BURROW:
					orders_burrow(unit);
					break;
				case Order::ORD_BURROWED:
					orders_burrowed(unit);
					break;
				case Order::ORD_DEBURROW:
					orders_deburrowed(unit);
					break;
				case Order::ORD_NUKE_LAUNCH:
					orders_nuke_launch(unit);
					break;
				case Order::ORD_NUKE_PAINT:
					orders_nuke_paint(unit);
					break;
				case Order::ORD_NUKE_UNIT:
					orders_nuke_unit(unit);
					break;
				case Order::ORD_NUKE_GROUND:
					orders_NukeGround(unit);
					break;
				case Order::ORD_PLACE_MINE:
					orders_PlaceMine(unit);
					break;
				case Order::ORD_RCLICK:
					orders_RightClickAction(unit);
					break;
				case Order::ORD_SAP_UNIT:
					orders_SapUnit(unit);
					break;
				case Order::ORD_SAP_LOC:
					orders_SapLocation(unit);
					break;
				case Order::ORD_TELEPORT:
					orders_Recall(unit);
					break;
				case Order::ORD_PLACE_SCANNER:
					orders_PlaceScanner(unit);
					break;
				case Order::ORD_SCANNER:
					orders_scanner(unit);
					break;
				case Order::ORD_DEF_MAT:
					orders_DefensiveMatrix(unit);
					break;
				case Order::ORD_HALLUCINATE:
					orders_Hallucination1(unit);
					break;
				case Order::ORD_PATROL:
					orders_Patrol(unit);
					break;
				case Order::ORD_CTF_COP_INIT:
					orders_CTFCOP_init(unit);
					break;
				case Order::ORD_CTF_COP:
					orders_CTFCOP1(unit);
					break;
				case Order::ORD_COMP_AI:
					orders_comp_ai(unit);
					break;
				case Order::ORD_ATTACK_MOVE_EP:
					orders_AttackMoveEP(unit);
					break;
				case Order::ORD_HARASS_MOVE_EP:
					orders_HarassMove(unit);
					break;
				case Order::ORD_AI_PATROL:
					orders_AIPatrol(unit);
					break;
				case Order::ORD_GUARD_POST:
					orders_guard_post(unit);
					break;
				case Order::ORD_RESCUE_PASSIVE:
					orders_RescuePassive(unit);
					break;
				case Order::ORD_NEUTRAL:
					orders_Neutral(unit);
					break;
				case Order::ORD_COMP_RETURN:
					orders_comp_return(unit);
					break;
				case Order::ORD_JUICE_INIT:
					orders_InitPsiProvider(unit);
					break;
				case Order::ORD_HIDDEN_GUN:
					sub_47BF80(unit);
					break;
				case Order::ORD_OPEN_DOOR:
					orders_OpenDoor(unit);
					break;
				case Order::ORD_CLOSE_DOOR:
					orders_CloseDoor(unit);
					break;
				case Order::ORD_HIDE_TRAP:
					orders_HideTrap(unit);
					break;
				case Order::ORD_REVEAL_TRAP:
					orders_RevealTrap(unit);
					break;
				case Order::ORD_ENABLE_DOODAD:
					orders_enable_doodad(unit);
					break;
				case Order::ORD_DISABLE_DOODAD:
					orders_disable_doodad(unit);
					break;
				case Order::ORD_MEDIC:
					orders_Medic(unit);
					break;
				case Order::ORD_MINDCONTROL:
					orders_CastMindControl(unit);
					break;
				case Order::ORD_PSYFEEDBACK:
					orders_Feedback(unit);
					break;
				case Order::ORD_JUNK_YARD_DOG:
					orders_JunkYardDog(unit);
					break;
				default:
					return;
				}
			}
		}
		break;
	}
}

FAIL_STUB_PATCH(ordersEntries);

void UpdateUnitOrderData_(CUnit* unit)
{
	RefreshUnit_(unit);
	updateUnitTimers_(unit);
	ordersEntries_(unit);
	performSecondaryOrders(unit);
	if (unit->subUnit && (Unit_PrototypeFlags[unit->unitType] & Subunit) == 0)
	{
		iscript_unit = unit->subUnit;
		UpdateUnitOrderData_(unit->subUnit);
		iscript_unit = unit;
	}

	if (unit->sprite)
	{
		spriteToIscriptLoop_(unit->sprite);
		if (!unit->sprite->pImageHead)
		{
			unit->sprite = nullptr;
		}
	}
}

FAIL_STUB_PATCH(UpdateUnitOrderData);

void unitUpdate_(CUnit* unit)
{
	if (unit->subUnit && (Unit_PrototypeFlags[unit->unitType] & Subunit) == 0)
	{
		iscript_unit = unit->subUnit;
		unitUpdate_(unit->subUnit);
		iscript_unit = unit;
	}
	Unit_ExecPathingState_(unit);
	updateUnitTimers_(unit);
	ordersIDCases(unit);

	switch (unit->secondaryOrderID)
	{
	case Order::ORD_TRAIN_FIGHTER:
		secondaryOrd_TrainFighter(unit);
		break;
	case Order::ORD_CLOAK:
		secondaryOrd_Cloak(unit);
		break;
	case Order::ORD_DECLOAK:
		unit->secondaryOrderID = Order::ORD_NOTHING;
		unit->secondaryOrderPosition.y = 0;
		unit->secondaryOrderPosition.x = 0;
		unit->currentBuildUnit = nullptr;
		unit->secondaryOrderState = 0;
		break;
	}

	if (unit->sprite)
	{
		spriteToIscriptLoop_(unit->sprite);
		if (!unit->sprite->pImageHead)
		{
			unit->sprite = 0;
		}
	}
}

FAIL_STUB_PATCH(unitUpdate);

void UpdateUnits_()
{
	CUnit* next_unit;

	dword_6957D0 = 0;
	if (isSelectedUnitGroupEnabled())
	{
		if (is_placing_building)
		{
			refreshLayer3And4();
			refreshPlaceBuildingLocation();
		}
		if (is_placing_order)
		{
			CancelTargetOrder();
		}
	}

	if (!--dword_59CCA4)
	{
		u8 i = 0;
		for (CUnit* unit = UnitNodeList_VisibleUnit_First; unit; unit = unit->next)
		{
			unit->orderQueueTimer = i++;
			i %= 8;
		}
		dword_59CCA4 = 150;
	}
	if (!--dword_6283E8)
	{
		u8 i = 0;
		for (CUnit* unit = UnitNodeList_VisibleUnit_First; unit; unit = unit->next)
		{
			unit->secondaryOrderTimer = i++;
			i %= 30;
		}
		dword_6283E8 = 300;
	}
	sub_4EB2F0();

	for (CUnit* unit = UnitNodeList_HiddenUnit_Last; unit; unit = next_unit)
	{
		next_unit = unit->next;
		iscript_flingy = unit;
		iscript_unit = unit;
		sub_4EB5E0_(unit);
		if (unit->sprite)
		{
			if (visionUpdated && Players[unit->playerID].nType == PT_Human)
			{
				revealSightAtLocation(1, PlayerVision[unit->playerID], unit->sprite->position.x, unit->sprite->position.y, unit->statusFlags & 4);
			}
			UpdateUnitSpriteInfo_(unit);
		}
	}

	for (CUnit* unit = UnitNodeList_VisibleUnit_First; unit; unit = unit->next)
	{
		iscript_flingy = unit;
		iscript_unit = unit;
		sub_4EBC30_(unit);
	}

	if (visionUpdated)
	{
		for (CUnit* unit = UnitNodeList_ScannerSweep_First; unit; unit = next_unit)
		{
			next_unit = unit->next;
			iscript_flingy = unit;
			iscript_unit = unit;
			refreshUnitVision(unit);
		}
	}

	for (CUnit* unit = UnitNodeList_VisibleUnit_First; unit; unit = unit->next)
	{
		UpdateUnitSpriteInfo_(unit);
		if (unit->statusFlags & (Cloaked | RequiresDetection))
		{
			unit->isCloaked = 0;
			if (unit->secondaryOrderTimer)
			{
				unit->secondaryOrderTimer -= 1;
			}
			else
			{
				CheckUnitVisibility(unit);
				unit->secondaryOrderTimer = 30;
			}
		}
	}

	for (CUnit* unit = UnitNodeList_VisibleUnit_First; unit; unit = next_unit)
	{
		next_unit = unit->next;
		iscript_flingy = unit;
		iscript_unit = unit;
		UpdateUnitOrderData_(unit);
	}

	for (CUnit* unit = UnitNodeList_HiddenUnit_First; unit; unit = next_unit)
	{
		next_unit = unit->next;
		if (unit->statusFlags & (Cloaked | RequiresDetection))
		{
			unit->isCloaked = 0;
		}
		iscript_flingy = unit;
		iscript_unit = unit;
		unitUpdate_(unit);
	}

	updateBurrowingCloakingUnits();
	updateUnitPower();
	int v14 = ++dword_64EEC8;
	if (dword_64EEC8 == 32)
	{
		v14 = 0;
		dword_64EEC8 = 0;
	}
	memset(&stru_64DEC8[v14], 0, sizeof(struc_64DEC8));

	dword_64DEA8 = 0;
	for (CUnit* unit = UnitNodeList_ScannerSweep_First; unit; unit = next_unit)
	{
		next_unit = unit->next;
		iscript_flingy = unit;
		iscript_unit = unit;
		UpdateUnitOrderData_(unit);
	}
	iscript_flingy = nullptr;
	iscript_unit = nullptr;
}

FAIL_STUB_PATCH(UpdateUnits);

void UpdateImage_(CUnit* unit)
{
	if (unit->sprite)
	{
		spriteToIscriptLoop_(unit->sprite);
		if (unit->sprite->pImageHead == nullptr)
		{
			unit->sprite = nullptr;
		}
	}
	if (unit->sprite)
	{
		byte_63FEC2 = unit->movementFlags;
		Unit_AssignNextWP(unit);
		sub_495CB0((CFlingy*)unit);
		ProgressMovementFlag(unit);
		sub_4956C0(unit);
		byte_63FEC0 = unit->movementFlags;
		unit->movementFlags = byte_63FEC2;
		sub_496030(unit);
		CSprite* v5 = unit->sprite;
		if ((__int16)v5->position.x >= (unsigned int)(unsigned __int16)map_width_pixels || (__int16)v5->position.y >= (unsigned int)(unsigned __int16)map_height_pixels || unit->moveTarget.pt.x == unit->position.x && unit->moveTarget.pt.y == unit->position.y)
		{
			for (CImage* image = v5->pImageHead; image; image = image->next)
			{
				PlayIscriptAnim_(image, AE_Death);
			}
		}
	}
	else
	{
		if (dword_63FF34 == unit)
		{
			dword_63FF34 = unit->next;
		}
		if (dword_63FEC8 == unit)
		{
			dword_63FEC8 = unit->prev;
		}
		if (unit->prev)
		{
			unit->prev->next = unit->next;
		}
		CUnit* v2 = unit->next;
		if (v2)
		{
			v2->prev = unit->prev;
		}
		unit->prev = 0;
		unit->next = 0;
		CUnit* v3 = dword_63FF38;
		if (dword_63FF38)
		{
			if (dword_63FF3C == dword_63FF38)
			{
				dword_63FF3C = unit;
			}
			unit->prev = dword_63FF38;
			unit->next = v3->next;
			CUnit* v4 = v3->next;
			if (v4)
			{
				v4->prev = unit;
			}
			v3->next = unit;
		}
		else
		{
			dword_63FF3C = unit;
			dword_63FF38 = unit;
		}
	}
}

FAIL_STUB_PATCH(UpdateImage);

void UpdateImages_()
{
	CUnit* next_unit;
	for (CUnit* unit = dword_63FF34; unit; unit = next_unit)
	{
		next_unit = unit->next;
		UpdateImage_(unit);
	}
}

FAIL_STUB_PATCH(UpdateImages);

void sub_488020_(CThingy* thingy)
{
	u16 sprite_id = thingy->sprite->spriteID;
	if (sprite_id <= 0x81u || sprite_id >= 0x182u && sprite_id <= 0x1E0u)
	{
		sub_487690(thingy);
	}
	else
	{
		isThingyOnMap_(thingy->sprite->unkflags_12, thingy->sprite->unkflags_13, thingy);
	}

	if (thingy->sprite)
	{
		spriteToIscriptLoop_(thingy->sprite);
		if (thingy->sprite->pImageHead == nullptr)
		{
			thingy->sprite = nullptr;
		}
	}

	if (thingy->sprite == nullptr)
	{
		if (first_lone_sprite == thingy)
		{
			first_lone_sprite = thingy->next;
		}
		if (dword_65291C == thingy)
		{
			dword_65291C = thingy->prev;
		}
		if (thingy->prev)
		{
			thingy->prev->next = thingy->next;
		}
		CThingy* v4 = thingy->next;
		if (v4)
		{
			v4->prev = thingy->prev;
		}
		thingy->prev = 0;
		thingy->next = 0;
		CThingy* v5 = dword_654878;
		if (dword_654878)
		{
			if (dword_65487C == dword_654878)
			{
				dword_65487C = thingy;
			}
			thingy->prev = dword_654878;
			thingy->next = v5->next;
			CThingy* v6 = v5->next;
			if (v6)
			{
				v6->prev = thingy;
			}
			v5->next = thingy;
		}
		else
		{
			dword_65487C = thingy;
			dword_654878 = thingy;
		}
	}
}

FAIL_STUB_PATCH(sub_488020);

void sub_488350_(CThingy* thingy)
{
	RefreshSprite(thingy->sprite, byte_581D6A[thingy->sprite->playerID]);
	if (!CThingyIsVisible(0, thingy))
	{
		removeSelectionCircle(thingy->sprite);
		SpriteDestructor_(thingy->sprite);
		thingy->sprite = 0;
		if (dword_654868 == thingy)
		{
			dword_654868 = thingy->next;
		}
		if (dword_65486C == thingy)
		{
			dword_65486C = thingy->prev;
		}
		if (thingy->prev)
		{
			thingy->prev->next = thingy->next;
		}
		CThingy* v1 = thingy->next;
		if (v1)
		{
			v1->prev = thingy->prev;
		}
		thingy->prev = 0;
		thingy->next = 0;
		CThingy* v2 = dword_6509D0;
		if (dword_6509D0)
		{
			if (dword_654870 == dword_6509D0)
			{
				dword_654870 = thingy;
			}
			thingy->prev = dword_6509D0;
			thingy->next = v2->next;
			CThingy* v3 = v2->next;
			if (v3)
			{
				v3->prev = thingy;
			}
			v2->next = thingy;
		}
		else
		{
			dword_654870 = thingy;
			dword_6509D0 = thingy;
		}
	}
}

FAIL_STUB_PATCH(sub_488350);

void updateThingys_()
{
	CThingy* next_thingy;

	for (CThingy* thingy = first_lone_sprite; thingy; thingy = next_thingy)
	{
		next_thingy = thingy->next;
		sub_488020_(thingy);
	}

	for (CThingy* thingy = dword_654868; thingy; thingy = next_thingy)
	{
		next_thingy = thingy->next;
		sub_488350_(thingy);
	}
}

FAIL_STUB_PATCH(updateThingys);

void GameLoop_()
{
	SetInGameLoop(1);
	InitializeRandomizerInfo();
	AI_Loop();
	if (visionUpdateCount == 0)
	{
		visionUpdateCount = 100;
	}
	visionUpdated = visionUpdateCount-- == 1;
	if (visionUpdated)
	{
		updateActiveTileInfo_();
		RemoveFoWCheat_();
	}
	UpdateUnits_();
	ImageDrawingBulletDrawing_();
	UpdateImages_();
	updateThingys_();
	SetInGameLoop(0);
}

FAIL_STUB_PATCH(GameLoop);

unsigned int DoCycle_(CycleStruct* cycle_struct, unsigned int cycle_struct_index, unsigned int a3)
{
	int v4 = 0, v5 = 255;

	while (cycle_struct->speed)
	{
		if (cycle_struct_index >= 8 || cycle_struct->palette_entry_low > a3)
		{
			if (v4 >= v5)
			{
				int v10 = v4 - v5 + 1;
				PALETTEENTRY* v11 = &GamePalette[v5];
				if (Gamma != 100)
				{
					PALETTEENTRY a1[256];
					sub_41DC20(v11, &a1[v5], v10);
					v11 = &a1[v5];
				}
				SDrawUpdatePalette(v5, v10, v11, 1);
			}

			break;
		}

		if (cycle_struct->active && cycle_struct->wait-- == 1)
		{
			cycle_struct->wait = cycle_struct->speed;
			if (cycle_struct->advanced_cycle_data)
			{
				CyclePaletteAdvanced(cycle_struct_index);
			}
			CyclePalette(cycle_struct_index);
			v4 = max(v4, cycle_struct->palette_entry_high);
			v5 = min(cycle_struct->palette_entry_low, v5);
		}
		++cycle_struct;
		++cycle_struct_index;
	}

	return cycle_struct_index;
}

FAIL_STUB_PATCH(DoCycle);

void colorCycleInterval_()
{
	unsigned int v0 = DoCycle_(cycle_colors, 0, 0x80u);
	if (v0 < 8)
		DoCycle_(&cycle_colors[v0], v0, 0x100u);
}

FAIL_STUB_PATCH(colorCycleInterval);

ButtonOrder button_set_terran_infantry_[] =
{
	{ 1, ICON_MOVE, BTNSCOND_Always, BTNSACT_Move, 0, 0, 0x298, 0 },
	{ 2, ICON_STOP, BTNSCOND_Always, CMDACT_Stop, 0, 0, 0x299, 0 },
	{ 3, ICON_ATTACK, BTNSCOND_CanAttack, BTNSACT_AttackMove, 0, 0, 0x29A, 0},
	{ 4, ICON_PATROL, BTNSCOND_Always, BTNSACT_Patrol, 0, 0, 0x29B, 0 },
	{ 5, ICON_HOLD_POSITION, BTNSCOND_Always, CMDACT_HoldPosition, 0, 0, 0x29C, 0 },
	{ 7, ICON_USE_STIMPACK, BTNSCOND_HasTech, CMDACT_Stimpack, 0, 0, 0x14E, 0x15A },
};

ButtonSet button_sets_[] = {
	{_countof(button_set_terran_infantry_), button_set_terran_infantry_, -1, 0},
	{9, button_set_ghost, -1, 0},
	{6, button_set_vulture, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{0, 0, -1, 0},
	{7, button_set_siege_tank, -1, 0},
	{0, 0, -1, 0},
	{9, button_set_scv, -1, 0},
	{7, button_set_wraith, -1, 0},
	{8, button_set_science_vessel, -1, 0},
	{_countof(button_set_terran_infantry_), button_set_terran_infantry_, -1, 0},
	{7, button_set_dropship, -1, 0},
	{6, button_set_battlecruiser, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{8, button_set_ghost_hero, 1, 0},
	{5, button_set_generic_army, -1, 0},
	{0, 0, -1, 0},
	{6, button_set_vaynor_vulture, 2, 0},
	{_countof(button_set_terran_infantry_), button_set_terran_infantry_, -1, 0},
	{7, button_set_wraith, -1, 0},
	{8, button_set_science_vessel, -1, 0},
	{7, button_set_siege_tank, -1, 0},
	{0, 0, -1, 0},
	{7, button_set_siege_tank, -1, 0},
	{0, 0, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{6, button_set_battlecruiser, -1, 0},
	{6, button_set_battlecruiser, -1, 0},
	{7, button_set_siege_tank, -1, 0},
	{0, 0, -1, 0},
	{_countof(button_set_terran_infantry_), button_set_terran_infantry_, -1, 0},
	{0, 0, -1, 0},
	{7, button_set_medic, -1, 0},
	{9, button_set_larva, -1, 0},
	{1, button_set_egg, -1, 0},
	{7, button_set_zergling, -1, 0},
	{8, button_set_hydralisk, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{9, button_set_drone, -1, 0},
	{7, button_set_dropship, -1, 0},
	{7, button_set_mutalisk, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{9, button_set_queen, -1, 0},
	{9, button_set_deflier, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{9, button_set_queen, -1, 0},
	{7, button_set_infested_terran, -1, 0},
	{10, button_set_infested_kerrigan, -1, 0},
	{9, button_set_deflier, -1, 0},
	{7, button_set_zergling, -1, 0},
	{7, button_set_zergling, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{7, button_set_dropship, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{1, button_set_egg, -1, 0},
	{6, button_set_corsair, -1, 0},
	{7, button_set_dark_templar, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{7, button_set_dark_archon, -1, 0},
	{7, button_set_probe, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{8, button_set_high_templar, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{7, button_set_dropship, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{7, button_set_arbiter, -1, 0},
	{7, button_set_carrier, -1, 0},
	{0, 0, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{7, button_set_tassadar, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{7, button_set_reaver, -1, 0},
	{7, button_set_carrier, -1, 0},
	{7, button_set_reaver, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{0, 0, -1, 0},
	{7, button_set_arbiter, -1, 0},
	{7, button_set_tassadar, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{5, button_set_generic_army, -1, 0},
	{1, button_set_egg, -1, 0},
	{6, button_set_corsair, -1, 0},
	{8, button_set_ghost_hero, -1, 0},
	{8, button_set_ghost_hero, -1, 0},
	{0, 0, -1, 0},
	{6, button_set_battlecruiser, -1, 0},
	{7, button_set_lurker, -1, 0},
	{9, button_set_infested_duran, -1, 0},
	{0, 0, -1, 0},
	{10, button_set_command_center, -1, 0},
	{1, button_set_comsat, -1, 0},
	{2, button_set_nuclear_silo, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0xB, button_set_barracks, -1, 0},
	{7, button_set_academy, -1, 0},
	{0xC, button_set_factory, -1, 0},
	{0xD, button_set_starport, -1, 0},
	{4, button_set_control_tower, -1, 0},
	{0xC, button_set_science_facility, -1, 0},
	{6, button_set_covert_ops, -1, 0},
	{4, button_set_physics_lab, -1, 0},
	{0, 0, -1, 0},
	{6, button_set_machine_shop, -1, 0},
	{0, 0, -1, 0},
	{7, button_set_engineering_bay, -1, 0},
	{5, button_set_armory, -1, 0},
	{2, button_set_missile_turret, -1, 0},
	{2, button_set_bunker, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{7, button_set_infested_command_center, -1, 0},
	{5, button_set_hatchery, -1, 0},
	{9, button_set_lair, -1, 0},
	{8, button_set_hive, -1, 0},
	{1, button_set_nydus_canal, -1, 0},
	{5, button_set_hydralisk_den, -1, 0},
	{5, button_set_defiler_mound, -1, 0},
	{3, button_set_greater_spire, -1, 0},
	{5, button_set_queens_nest, -1, 0},
	{4, button_set_evolution_chamber, -1, 0},
	{3, button_set_ultralisk_cavern, -1, 0},
	{4, button_set_spire, -1, 0},
	{3, button_set_spawning_pool, -1, 0},
	{2, button_set_creep_colony, -1, 0},
	{2, button_set_missile_turret, -1, 0},
	{0, 0, -1, 0},
	{2, button_set_missile_turret, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{3, button_set_nexus, -1, 0},
	{5, button_set_robotics_facility, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{3, button_set_observatory, -1, 0},
	{6, button_set_gateway, -1, 0},
	{0, 0, -1, 0},
	{2, button_set_missile_turret, -1, 0},
	{2, button_set_citadel_of_adun, -1, 0},
	{4, button_set_cybernetics_core, -1, 0},
	{8, button_set_templar_archives, -1, 0},
	{4, button_set_forge, -1, 0},
	{6, button_set_stargate, -1, 0},
	{0, 0, -1, 0},
	{7, button_set_fleet_beacon, -1, 0},
	{5, button_set_arbiter_tribunal, -1, 0},
	{4, button_set_robotics_support_bay, -1, 0},
	{1, button_set_shield_battery, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{1, button_set_flag_beacon, -1, 0},
	{1, button_set_flag_beacon, -1, 0},
	{1, button_set_flag_beacon, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{0, 0, -1, 0},
	{1, stru_515C60, -1, 0},
	{1, stru_515C74, -1, 0},
	{1, stru_515C88, -1, 0},
	{2, stru_515C9C, -1, 0},
	{1, stru_515CC4, -1, 0},
	{2, stru_515CD8, -1, 0},
	{1, stru_515D00, -1, 0},
	{3, stru_515D14, -1, 0},
	{1, stru_515D50, -1, 0},
	{7, button_set_basic_zerg_buildings, -1, 0},
	{9, button_set_basic_terran_buildings, -1, 0},
	{9, stru_516D08, -1, 0},
	{6, button_set_advanced_zerg_buildings, -1, 0},
	{5, button_set_advanced_terran_buildings, -1, 0},
	{9, stru_516DC0, -1, 0},
	{5, stru_515D68, -1, 0},
	{5, stru_515E38, -1, 0},
	{7, button_set_wraith, -1, 0},
	{7, button_set_zergling, -1, 0},
	{3, replay_paused, -1, 0},
	{3, replay_playing, -1, 0},
};

void updateButtonSetEx_()
{
	u16 portrait_button_set = ActivePortraitUnit->currentButtonSet;
	char v7 = 0;
	char v8 = 0;
	char cloackable_count = 0;
	char worker_count = 0;

	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* unit = ClientSelectionGroup[i])
		{
			u16 unit_button_set = unit->currentButtonSet;
			if (Unit_PrototypeFlags[unit->unitType] & UnitPrototypeFlags::Worker)
			{
				++worker_count;
			}
			if (Unit_PrototypeFlags[unit->unitType] & UnitPrototypeFlags::Cloakable)
			{
				++cloackable_count;
			}
			if ((unit->statusFlags & InAir) == 0 && (Unit_GroupFlags[unit->unitType] & 1))
			{
				++v8;
			}
			if (Unit_PrototypeFlags[unit->unitType] & UnitPrototypeFlags::Burrowable)
			{
				v7 = 1;
			}
			if (unit_button_set != portrait_button_set && button_sets_[unit_button_set].button_order != button_sets_[portrait_button_set].button_order && button_sets_[unit_button_set].f3 != portrait_button_set)
			{
				if (button_sets_[portrait_button_set].f3 == unit_button_set)
				{
					portrait_button_set = unit->currentButtonSet;
					word_68C1C4 = portrait_button_set;
				}
				else
				{
					word_68C1C4 = 244;
				}
			}
		}
	}

	if (word_68C1C4 != 228)
	{
		if (ClientSelectionCount == worker_count)
		{
			word_68C1C4 = 245;
		}
		else if (ClientSelectionCount == cloackable_count)
		{
			word_68C1C4 = 246;
		}
		else if (v7 && ClientSelectionCount == v8)
		{
			word_68C1C4 = 247;
		}
	}
}

FAIL_STUB_PATCH(updateButtonSetEx);

void updateButtonSet_()
{
	word_68C1C8 = 228;
	word_68C14C = 228;

	if (InReplay)
	{
		word_68C1C8 = 249 - (is_replay_paused != 0);
	}
	else if (ClientSelectionCount)
	{
		if (is_placing_order)
		{
			word_68C1C8 = 229;
		}
		else if (is_placing_building)
		{
			word_68C1C8 = 230;
		}
	}

	word_68C1C4 = 228;
	if ((u8)ClientSelectionCount > 1 && !InReplay)
	{
		updateButtonSetEx_();
	}
}

FAIL_STUB_PATCH(updateButtonSet);

void updateSelectedUnitData_()
{
	memcpy(ClientSelectionGroup, PlayerSelection, sizeof(ClientSelectionGroup));
	ClientSelectionCount = 0;
	ActivePortraitUnit = NULL;

	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (ClientSelectionGroup[i])
		{
			if (compareUnitRank(ClientSelectionGroup[i], ActivePortraitUnit))
			{
				ActivePortraitUnit = ClientSelectionGroup[i];
			}
			++ClientSelectionCount;
		}
	}

	if (ClientSelectionCount == 1)
	{
		ClientSelectionGroup[0] = ActivePortraitUnit;
		memset(&ClientSelectionGroup[1], 0, 0x2Cu);
	}
	updateButtonSet_();
}

FAIL_STUB_PATCH(updateSelectedUnitData);

void sub_4591D0_()
{
	int v1 = 0;
	ButtonSet* v3 = &button_sets_[word_68C14C];
	ButtonOrder* button_order = v3->button_order;
	ButtonState v10 = ButtonState::BTNST_HIDDEN;

	for (dialog* dlg = getControlFromIndex_(current_dialog, 1); dlg; dlg = dlg->pNext)
	{
		if (dlg->wIndex < 1)
		{
			return;
		}
		if (dlg->wIndex <= 9)
		{
			for (; v1 < LOWORD(v3->button_count); v1++)
			{
				v10 = button_order->condition(button_order->condition_variable, g_LocalNationID, ActivePortraitUnit);
				if (v10)
				{
					break;
				}
				++button_order;
			}
			if ((InReplay || !ActivePortraitUnit || ActivePortraitUnit->playerID == g_LocalNationID)
				&& v1 < LOWORD(v3->button_count)
				&& dlg->wIndex >= (int)button_order->position)
			{
				dlg->lUser = (int)button_order;
				if (dlg->wUser != button_order->icon_id)
				{
					dlg->wUser = button_order->icon_id;
					if ((dlg->lFlags & 1) == 0)
					{
						dlg->lFlags |= DialogFlags::CTRL_UPDATE;
						updateDialog(dlg);
					}
					dlgEvent v9;
					v9.cursor.y = Mouse.y;
					v9.wNo = EVN_USER;
					v9.dwUser = USER_NEXT;
					*(_DWORD*)&v9.wSelection = 0;
					v9.cursor.x = Mouse.x;
					dlg->pfcnInteract(dlg, &v9);
				}
				showDialog_(dlg);
				if (v10 >= BTNST_HIDDEN)
				{
					EnableControl(dlg);
				}
				else
				{
					DisableControl(dlg);
					if (dword_66FF60 == 21)
					{
						button_order->action_string_id = 762;
						++button_order;
						++v1;
						continue;
					}
				}
				++button_order;
				++v1;
			}
			else
			{
				HideDialog_(dlg);
				dlg->wUser = -1;
				dlg->lUser = 0;
			}
		}
	}
}

FAIL_STUB_PATCH(sub_4591D0);

void replayStatBtns_(dialog* dlg)
{
	showDialog(getControlFromIndex_(dlg, 10));
	DLGsetProgressBarValue(dlg, 10, 100 * ElapsedTimeFrames / replay_header.ReplayFrames);
	const char* v3;
	if (*StatTxtTbl.buffer > 0x608u)
	{
		v3 = (char*)StatTxtTbl.buffer + StatTxtTbl.buffer[1545];
	}
	else
	{
		v3 = empty_string;
	}
	char v16[4] = { 3, 0 };
	sprintf_s(byte_6CA988, "%s%s", v16, v3);
	AddTextToDialog(dlg, -45, byte_6CA988);

	char buff[32];
	DWORD v4 = ElapsedTimeFrames * dword_4FF90C[replay_header.game_data.game_speed] / 1000 / 3600;
	DWORD v13 = ElapsedTimeFrames * dword_4FF90C[replay_header.game_data.game_speed] / 1000 % 60;
	DWORD v11 = ElapsedTimeFrames * dword_4FF90C[replay_header.game_data.game_speed] / 1000 / 60 % 60;
	if (v4)
	{
		sprintf_s(buff, "%d:%02d:%02d", v4, v11, v13);
	}
	else
	{
		sprintf_s(buff, "%02d:%02d", v11, v13);
	}
	sprintf_s(byte_6CA7A0, get_GluAll_String((GluAllTblEntry)16), buff);
	AddTextToDialog(dlg, -46, byte_6CA7A0);

	if (is_replay_paused)
	{
		if (dword_6D11B4)
		{
			char* v6;
			if (*StatTxtTbl.buffer > 0x609u)
			{
				v6 = (char*)StatTxtTbl.buffer + StatTxtTbl.buffer[1546];
			}
			else
			{
				v6 = (char*)empty_string;
			}
			char* v7 = (char*)(byte_6CAC10 - v6);
			char v8;
			do
			{
				v8 = *v6;
				v6[(_DWORD)v7] = *v6;
				++v6;
			} while (v8);
		}
		else
		{
			byte_6CAC10[0] = 0;
		}
	}
	else
	{
		char v15[8];
		v15[0] = 0;
		if (replay_speed_multiplier > 1)
		{
			const char* v9 = registry_options.GameSpeed >= 3 ? "x" : "/";
			sprintf_s(v15, " %s %d", v9, replay_speed_multiplier);
		}
		const char* v12 = get_GluAll_String((GluAllTblEntry)(LOWORD(registry_options.GameSpeed) + 85));
		const char* v10 = get_GluAll_String((GluAllTblEntry)0x5D);
		sprintf_s(byte_6CAC10, "%s %s%s", v10, v12, v15);
	}
	AddTextToDialog(dlg, -47, byte_6CAC10);
}

FAIL_STUB_PATCH(replayStatBtns);

void updateSelectedUnitPortrait_()
{
	if (CanUpdateSelectedUnitPortrait && !dword_68AC60)
	{
		CUnit* lUser = (CUnit*)dword_68AC98->lUser;
		CUnit* v2 = ActivePortraitUnit;
		if (!ActivePortraitUnit)
		{
			dword_68AC98->lUser = (int)ActivePortraitUnit;
			dword_68AC98->wUser = -1;
			HideDialog(dword_68AC98);
		}
		else if (lUser != ActivePortraitUnit || lUser && getLastQueueSlotType(lUser) != getLastQueueSlotType(ActivePortraitUnit))
		{
			UnitType LastQueueSlotType = getLastQueueSlotType(v2);
			WORD v5 = setBuildingSelPortrait_(LastQueueSlotType);
			displayUpdatePortrait(v5, v2, 1);
		}
	}
	CanUpdateSelectedUnitPortrait = 0;
}

FAIL_STUB_PATCH(updateSelectedUnitPortrait);

void updateCurrentButtonset_()
{
	u16 v0 = word_68C1C8;
	int v1 = CanUpdateCurrentButtonSet;
	if (CanUpdateCurrentButtonSet && (word_68C1C8 == 228 || InReplay))
	{
		updateButtonSet_();
	}
	if (word_68C1C8 == 228)
	{
		v0 = word_68C1C4;
		if (word_68C1C4 == 228)
		{
			if (ActivePortraitUnit)
			{
				v0 = ActivePortraitUnit->currentButtonSet;
			}
			else
			{
				goto LABEL_14;
			}
		}
	}

	if (word_68C14C != v0)
	{
		word_68C14C = v0;
		CanUpdateCurrentButtonSet = 1;
	}

LABEL_14:
	if (v1 || CanUpdateCurrentButtonSet)
	{
		sub_4591D0_();
		sub_459770();
		if (ActivePortraitUnit)
		{
			Order v6 = ActivePortraitUnit->orderID;
			Order v3 = ActivePortraitUnit->subUnit ? ActivePortraitUnit->subUnit->orderID : (Order) -1;
			sub_458D50();
			byte_68C1E4 = v6;
			byte_68C1B8 = v3;
			sub_458D50();
		}
		if (InReplay)
		{
			replayStatBtns_(current_dialog);
		}
		CanUpdateCurrentButtonSet = 0;
	}
	else if (ActivePortraitUnit)
	{
		Order v7 = ActivePortraitUnit->orderID;
		Order v5 = ActivePortraitUnit->subUnit ? ActivePortraitUnit->subUnit->orderID : (Order) -1;
		if (byte_68C1E4 != ActivePortraitUnit->orderID || byte_68C1B8 != v5)
		{
			sub_458D50();
			byte_68C1E4 = v7;
			byte_68C1B8 = v5;
			sub_458D50();
		}
	}
}

FUNCTION_PATCH(updateCurrentButtonset, updateCurrentButtonset_);

void __fastcall UnitStatAct_Dropship_(dialog* dlg)
{
	if ((InReplay || ActivePortraitUnit->playerID == g_LocalNationID)
		&& (ActivePortraitUnit->statusFlags & IsHallucination) == 0
		&& (sub_424FC0(), isUnitLoaded(ActivePortraitUnit))
		&& (InReplay || ActivePortraitUnit->playerID == g_LocalNationID))
	{
		if (statusScreenFunc != 11)
		{
			sub_457310(dlg);
			statusScreenFunc = 11;
		}
		setSpellSpecialBtnGraphic(dlg);
		setTextStr(dlg);
		getActivePortraitUnitName(dlg);
	}
	else if (ActivePortraitUnit->statusFlags & 2)
	{
		UnitStatAct_Building(dlg);
	}
	else
	{
		UnitStatAct_Standard(dlg);
	}
}

FAIL_STUB_PATCH(UnitStatAct_Dropship);

void __fastcall UnitStatAct_Overlord_(dialog* dlg)
{
	if ((InReplay || ActivePortraitUnit->playerID == g_LocalNationID) && (ActivePortraitUnit->statusFlags & IsHallucination) == 0)
	{
		if (isUnitLoaded(ActivePortraitUnit))
		{
			UnitStatAct_Dropship_(dlg);
		}
		else
		{
			sub_427540(dlg);
			setTextStr(dlg);
			SetUnitStatusStrText(dlg);
			AddTextToDialog(dlg, -5, UnitGetName(ActivePortraitUnit));
		}
	}
	else
	{
		UnitStatAct_Standard(dlg);
	}
}

FAIL_STUB_PATCH(UnitStatAct_Overlord);

UnitStat unit_stats_[] =
{
	{ 0, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 1, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 2, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 3, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 4, UnitStatCond_Never, UnitStatAct_Default },
	{ 5, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 6, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 7, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 8, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 9, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x0A, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x0B, UnitStatCond_Dropship, UnitStatAct_Dropship_ },
	{ 0x0C, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x0D, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{ 0x0E, UnitStatCond_Never, UnitStatAct_Default },
	{ 0x0F, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x10, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x11, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x12, UnitStatCond_Never, UnitStatAct_Default },
	{ 0x13, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x14, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x15, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x16, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x17, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x18, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x19, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1A, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1B, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1C, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1D, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1E, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x1F, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x20, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x21, UnitStatCond_Never,UnitStatAct_Default },
	{ 0x22, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x23, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x24, UnitStatCond_Egg, UnitStatAct_Egg },
	{ 0x25, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x26, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x27, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x28, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x29, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x2A, UnitStatCond_overlord, UnitStatAct_Overlord_ },
	{ 0x2B, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x2C, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x2D, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x2E, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x2F, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x30, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x31, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x32, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x33, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x34, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x35, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x36, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x37, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x38, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x39, UnitStatCond_overlord, UnitStatAct_Overlord_ },
	{ 0x3A, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x3B, UnitStatCond_Egg, UnitStatAct_Egg },
	{ 0x3C, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x3D, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x3E, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x3F, UnitStatCond_Archon, UnitStatAct_Archon },
	{ 0x40, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x41, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x42, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x43, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x44, UnitStatCond_Archon, UnitStatAct_Archon },
	{ 0x45, UnitStatCond_Dropship, UnitStatAct_Dropship_ },
	{ 0x46, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x47, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x48, UnitStatCond_CarrierReaver, UnitStatAct_CarrierReaver },
	{ 0x49, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4A, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4B, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4C, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4D, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4E, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x4F, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x50, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x51, UnitStatCond_CarrierReaver, UnitStatAct_CarrierReaver },
	{ 0x52, UnitStatCond_CarrierReaver, UnitStatAct_CarrierReaver },
	{ 0x53, UnitStatCond_CarrierReaver, UnitStatAct_CarrierReaver },
	{ 0x54, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x55, UnitStatCond_Standard, UnitStatAct_Default },
	{ 0x56, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x57, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x58, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x59, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x5A, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0, UnitStatCond_Never, UnitStatAct_Default },
	{ 0, UnitStatCond_Never, UnitStatAct_Default },
	{ 0x5D, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x5E, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x5F, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x60, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x61, UnitStatCond_Egg, UnitStatAct_Egg },
	{ 0x62, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x63, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x64, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x65, UnitStatCond_Standard, UnitStatAct_Default },
	{ 0x66, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x67, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x68, UnitStatCond_Standard, UnitStatAct_Standard },
	{ 0x69, UnitStatCond_Never, UnitStatAct_Default },
	{ 0x6A, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x6B, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x6C, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x6D, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x6E, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x6F, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x70, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x71, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x72, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x73, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x74, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x75, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x76, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x77, UnitStatCond_Standard, UnitStatAct_Default },
	{ 0x78, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x79, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x7A, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x7B, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x7C, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x7D, UnitStatCond_Dropship, UnitStatAct_Dropship_ },
	{ 0x7E, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x7F, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x80, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{ 0x81, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{ 0x82, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x83, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x84, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x85, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x86, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x87, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x88, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x89, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8A, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8B, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8C, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8D, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8E, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x8F, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x90, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x91, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x92, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x93, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x94, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x95, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x96, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x97, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x98, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x99, UnitStatCond_Standard, UnitStatAct_Default },
	{ 0x9A, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x9B, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x9C, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x9D, UnitStatCond_Building, UnitStatAct_Building },
	{ 0x9E, UnitStatCond_Building, UnitStatAct_Default },
	{ 0x9F, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A0, UnitStatCond_Building, UnitStatAct_Building },
	{ 0, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A2, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A3, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A4, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A5, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A6, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A7, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A8, UnitStatCond_Building, UnitStatAct_Building },
	{0x0A9, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AA, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AB, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AC, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AD, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AE, UnitStatCond_Building, UnitStatAct_Building },
	{0x0AF, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B0, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B1, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B2, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B3, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B4, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B5, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B6, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B7, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B8, UnitStatCond_Building, UnitStatAct_Building },
	{0x0B9, UnitStatCond_Building, UnitStatAct_Building },
	{0x0BA, UnitStatCond_Building, UnitStatAct_Building },
	{0x0BB, UnitStatCond_Building, UnitStatAct_Building },
	{0x0BC, UnitStatCond_Building, UnitStatAct_Building },
	{0x0BD, UnitStatCond_Building, UnitStatAct_Building },
	{0x0BE, UnitStatCond_Building, UnitStatAct_Building },
	{ 0, UnitStatCond_Never, UnitStatAct_Default },
	{ 0, UnitStatCond_Never, UnitStatAct_Default },
	{ 0, UnitStatCond_Never, UnitStatAct_Default },
	{0x0C2, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C3, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C4, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C5, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C6, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C7, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C8, UnitStatCond_Building, UnitStatAct_Building },
	{0x0C9, UnitStatCond_Building, UnitStatAct_Building },
	{ 0, UnitStatCond_Building, UnitStatAct_Building },
	{0x0CB, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0CC, UnitStatCond_Never, UnitStatAct_Default },
	{0x0CD, UnitStatCond_Never, UnitStatAct_Default },
	{0x0CE, UnitStatCond_Never, UnitStatAct_Default },
	{0x0CF, UnitStatCond_Never, UnitStatAct_Default },
	{0x0D0, UnitStatCond_Never, UnitStatAct_Default },
	{0x0D1, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D2, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D3, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D4, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D5, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{ 0, UnitStatCond_Building, UnitStatAct_Building },
	{0x0D7, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D8, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0D9, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DA, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DB, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DC, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DD, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DE, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0DF, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0E0, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0E1, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0E2, UnitStatCond_Powerup, UnitStatAct_Powerup },
	{0x0E3, UnitStatCond_Powerup, UnitStatAct_Powerup },
};

void sub_458120_()
{
	if (ActivePortraitUnit)
	{
		if (ClientSelectionCount != 1)
		{
			if (CanUpdateStatDataDialog || isSelGroupUpdated())
			{
				sub_425960(stardata_Dlg);
				sub_457FE0();
			}
		}
		else if (CanUpdateStatDataDialog || unit_stats_[ActivePortraitUnit->unitType].condition())
		{
			unit_stats_[ActivePortraitUnit->unitType].action(stardata_Dlg);
			sub_4568B0();
			sub_457FE0();
		}
	}
	else if (CanUpdateStatDataDialog)
	{
		for (dialog* v1 = stardata_Dlg->wCtrlType != DialogType::cDLG ? stardata_Dlg : stardata_Dlg->fields.dlg.pFirstChild; v1; v1 = v1->pNext)
		{
			HideDialog_(v1);
		}
		statusScreenFunc = 0;
	}

	CanUpdateStatDataDialog = 0;
}

FAIL_STUB_PATCH(sub_458120);

void sub_4C3B10_()
{
	if (byte_59723C)
	{
		updateSelectedUnitData_();
		byte_59723C = 0;
	}
	updateSelectedUnitPortrait_();
	updateCurrentButtonset_();
	sub_458120_();
}

FAIL_STUB_PATCH(sub_4C3B10);

void sub_4D93B0_()
{
	if (is_app_active && ColorCycle && !IS_GAME_PAUSED)
	{
		DWORD tick_count = GetTickCount();
		if (tick_count >= dword_6D6374 + 10)
		{
			dword_6D6374 = tick_count;
			colorCycleInterval_();
		}
	}
}

FAIL_STUB_PATCH(sub_4D93B0);

void updateHUDInformation_()
{
	if (has_hud)
	{
		sub_4C3B10_();
		refreshScreen();
		sub_4D93B0_();
		refreshGameTextIfCounterActive();
	}
}

FAIL_STUB_PATCH(updateHUDInformation);

void DoGameLoop_()
{
	GameLoop_();
	updateHUDInformation_();
	GameLoop_();
	updateHUDInformation_();
}

FAIL_STUB_PATCH(DoGameLoop);

void PollInput_()
{
	if (!is_app_active || IS_GAME_PAUSED && !multiPlayerMode)
	{
		return;
	}
	if (byte_658AC0)
	{
		DisableDragSelect();
	}
	else if (byte_66FF5C || byte_6D1214)
	{
		return;
	}

	int scroll_speed;
	int a1;
	int a2;
	getScreenMoveState(&a1, &a2);
	if (a2 || a1)
	{
		dword_6D5C1C = 1;
		scroll_speed = multiPlayerMode ? registry_options.MKeyScrollSpeed : registry_options.KeyScrollSpeed;
	}
	else
	{
		getScrollCursorType(&a2, &a1);
		scroll_speed = multiPlayerMode ? registry_options.MMouseScrollSpeed : registry_options.MouseScrollSpeed;
	}

	if (a2 == 0 && a1 == 0)
	{
		dword_6D63DC = 0;
	}
	else if ((unsigned int)dword_6D63DC < 6)
	{
		dword_6D63DC += 1;
	}

	if (byte_658AC0)
	{
		scroll_speed = GetScrollSpeed(&dword_6D63DC);
	}
	if (a2 || a1)
	{
		dword_6D7574 = a2;
		dword_6D7570 = a1;
	}

	if (dword_6D63DC)
	{
		int v5 = 0;
		int v6 = 0;

		if (dword_6D7570 < 0)
		{
			v5 = moveToYScrDecrease(scrollSpeeds.scroll[7 * scroll_speed + dword_6D63DC]);
		}
		else if (dword_6D7570 > 0)
		{
			v5 = moveToYScrIncrease(scrollSpeeds.scroll[7 * scroll_speed + dword_6D63DC]);
		}

		if (dword_6D7574 < 0)
		{
			v6 = moveToXScrDecrease(scrollSpeeds.scroll[7 * scroll_speed + dword_6D63DC]);
		}
		else if (dword_6D7574 > 0)
		{
			v6 = moveToXScrIncrease(scrollSpeeds.scroll[7 * scroll_speed + dword_6D63DC]);
		}

		if (v6 || v5)
		{
			RefreshLayer5();
		}
	}

	if (byte_658AC0 && MoveToX == dword_658ABC && MoveToY == dword_658AA0 && dword_658AA4 != NULL)
	{
		dword_658AA4();
		dword_658AA4 = 0;
	}
}

FAIL_STUB_PATCH(PollInput);

void replayLoop_()
{
	if (!InGame)
	{
		return;
	}
	if ((int)ElapsedTimeFrames >= replay_header.ReplayFrames)
	{
		SetGameSpeed_maybe(registry_options.GameSpeed, 1, replay_speed_multiplier);
		open_win_mission_dialog_();
	}
	else if ((int)ElapsedTimeFrames >= nextReplayCommandFrame)
	{
		if ((int)ElapsedTimeFrames <= nextReplayCommandFrame || (nextReplayCommandFrame = sub_4CDFF0(replayData, &dword_6D5BF0, byte_6554D8, dest, dword_654AA8), nextReplayCommandFrame != -1) && (int)ElapsedTimeFrames >= nextReplayCommandFrame)
		{
			if (dword_6D5BF0)
			{
				byte_6D5BEC = 1;
				IsInGameLoop = 1;
				int v3 = 0;
				for (int v4 = 0; v4 < dword_6D5BF0; v4++)
				{
					for (int v5 = 0; v5 < 8; v5++)
					{
						if (Players[v5].dwStormId == (u8)byte_6554D8[v4])
						{
							dword_51267C = Players[v5].dwPlayerID;
							g_ActiveNationID = dword_51267C;
							break;
						}
					}
					dword_6556E8 = 0;
					Game_RECV(&dest[v3], dword_654AA8[v4], 1);
					v3 += dword_654AA8[v4];
				}
				dword_6D5BF0 = 0;
				g_ActiveNationID = g_LocalNationID;
				dword_51267C = g_LocalHumanID;
				dword_512680 = playerid;
				byte_6D5BEC = 0;
				IsInGameLoop = 0;
			}
		}
	}
}

FAIL_STUB_PATCH(replayLoop);

int BWFXN_IsPaused_()
{
	return IS_GAME_PAUSED;
}

FAIL_STUB_PATCH(BWFXN_IsPaused);

void RefreshAllUnits_()
{
	for (CUnit* unit = UnitNodeList_VisibleUnit_First; unit; unit = unit->next)
	{
		RefreshUnit_(unit);
	}
}

FAIL_STUB_PATCH(RefreshAllUnits);

void RecvMessage_()
{
	DWORD senderplayerid;
	BYTE* data;
	DWORD databytes;

	while (SNetReceiveMessage(&senderplayerid, (LPVOID*) &data, &databytes))
	{
		while (databytes && *data < 3)
		{
			int v1 = funcs_485FB0[*data](senderplayerid, (int)data, databytes);
			if (v1 == 0)
			{
				break;
			}
			data += v1;
			databytes -= v1;
		}
	}
	if (SErrGetLastError() != 0x8510006B && !outOfGame)
	{
		packetErrHandle_(SErrGetLastError(), 'Q', nullptr, 0, 1);
	}
}

FAIL_STUB_PATCH(RecvMessage);

void sub_4C4A80_(int a1, int a2)
{
	stru_66FE20[a1].field_0 = 2;
	if (isHost && map_download)
	{
		map_download->map_size &= ~(1 << a1);
		sub_472BA0(map_download, a1);
	}
	sub_45A0B0(a1);

	unsigned player_id = dword_57EEC0[a1];
	if (player_id >= 8)
	{
		if (!a1)
		{
			bootReason(3);
		}
		return;
	}
	if (glGluesMode == GLUE_CHAT)
	{
		if (!a1)
		{
			bootReason(3);
			return;
		}
		if (stru_66FE20[a1].field_0 && stru_66FE20[a1].player_name)
		{
			char buff[128];
			sprintf_s(buff, GetNetworkTblString_(a2 == 0x40000006 ? 76 : 75), getPlayerName(a1));
			printLobbyString(8, buff);
		}
	}

	PlayerType v8 = Players[player_id].nType;
	RaceId v13 = Players[player_id].nRace;
	u8 v12 = Players[player_id].nTeam;
	memcpy(&Players[player_id], &LobbyPlayers[player_id], sizeof(PlayerInfo));
	if (v8 == PT_ClosedSlot || v8 == PT_ComputerSlot)
	{
		Players[player_id].nType = v8;
	}
	Players[player_id].nRace = v13;
	Players[player_id].nTeam = v12;
	dword_57EEC0[a1] = 8;
	if (glGluesMode == GLUE_CHAT)
	{
		update_lobby_glue = 1;
		if (loadGameFileHandle)
		{
			byte_66FF34[player_id] = 8;
		}
		if (isHost)
		{
			UpdateGameMode(getOpenSlotCount());
		}
	}
}

FAIL_STUB_PATCH(sub_4C4A80);

void sub_4C4FA0_()
{
	if (multiPlayerMode)
	{
		for (int i = 0; i < 8; ++i)
		{
			if (player_left[i])
			{
				if (gwGameMode == GAME_GLUES)
				{
					sub_4C4A80_(i, player_left[i]);
				}
				else if (gwGameMode == GAME_RUN)
				{
					playerLeaveGame(i, 0, player_left[i]);
				}
				player_left[i] = 0;
			}
		}
	}
}

FAIL_STUB_PATCH(sub_4C4FA0);

void timeoutProcDropdown_()
{
	if (!byte_6D5BC2)
	{
		byte_6D5BC2 = 1;
		loadTimeoutDlg();
		dword_59CC84 = 0;
		DWORD v3 = GetTickCount();
		while (!byte_57EE78)
		{
			if (dword_59CC84)
			{
				break;
			}
			RecvMessage_();
			DWORD v0 = GetTickCount();
			if (SNetReceiveTurns(0, 8, (char**)arraydata, (unsigned int*)arraydatabytes, (DWORD*)playerStatusArray))
			{
				int v1 = IsInGameLoop;
				IsInGameLoop = 1;
				sub_4C4FA0_();
				IsInGameLoop = v1;
				dword_6D63D4 = v0;
				byte_57EE78 = 1;
			}
			else
			{
				if (SErrGetLastError() != 0x8510006B && !outOfGame)
				{
					packetErrHandle_(SErrGetLastError(), 81, 0, 0, 1);
				}
				byte_57EE78 = 0;
			}
			BWFXN_videoLoop(3);
			UpdateNetTimeoutPlayers();
			if (GetTickCount() > v3 + 1000)
			{
				updateCountdownDropTimer();
				v3 = GetTickCount();
			}
			BWFXN_RedrawTarget_();
		}
		if (dword_6D5BC4)
		{
			DestroyDialog(dword_6D5BC4);
			dword_6D5BC4 = 0;
		}
		BWFXN_RedrawTarget_();
		byte_6D5BC2 = 0;
		countdownTimeRemaining = GetTickCount();
	}
}

FAIL_STUB_PATCH(timeoutProcDropdown);

bool ReceiveTurns_(unsigned int* arraydatabytes, char** arraydata, DWORD* a3, int a4, int arraysize)
{
	if (SNetReceiveTurns(a4, arraysize, arraydata, arraydatabytes, a3))
	{
		int v5 = IsInGameLoop;
		IsInGameLoop = 1;
		sub_4C4FA0_();
		IsInGameLoop = v5;
		return true;
	}
	else
	{
		if (SErrGetLastError() != 0x8510006B && !outOfGame)
		{
			packetErrHandle_(SErrGetLastError(), 81, 0, 0, 1);
		}
		return false;
	}
}

FAIL_STUB_PATCH(ReceiveTurns);

bool RecvSaveTurns_()
{
	DWORD tick_count = GetTickCount();
	if (ReceiveTurns_((unsigned int*)arraydatabytes, (char**)arraydata, (DWORD*)playerStatusArray, 0, 8))
	{
		dword_6D63D4 = tick_count;
		byte_57EE78 = 1;
		return true;
	}
	else
	{
		byte_57EE78 = 0;
		return tick_count < dword_6D63D4 + 2500;
	}
}

FAIL_STUB_PATCH(RecvSaveTurns);

void GameKeepAlive_()
{
	int turns;
	if (SNetGetTurnsInTransit(&turns))
	{
		unsigned v1 = LatencyCalls;
		if (InGame)
		{
			v1 = Latency + LatencyCalls;
		}

		for (; turns < v1; turns++)
		{
			if (!sgdwBytesInCmdQueue)
			{
				TurnBuffer[0] = 5;
				sgdwBytesInCmdQueue = 1;
			}
			if (!SNetSendTurn(TurnBuffer, sgdwBytesInCmdQueue) && !outOfGame)
			{
				packetErrHandle_(SErrGetLastError(), '[', nullptr, 0, 1);
			}
			sgdwBytesInCmdQueue = 0;
			if (InGame)
			{
				CMDACT_GameHash();
			}
		}
	}
	else if (!outOfGame)
	{
		packetErrHandle(SErrGetLastError(), 81, "Starcraft\\SWAR\\lang\\net_mgr.cpp", 2292, 1);
	}
}

FAIL_STUB_PATCH(GameKeepAlive);

BOOL sub_4C4870_()
{
	if (!multiPlayerMode || NetMode.as_number != 'BNET' || gameData.save_timestamp || !gameData.approval_status || InReplay)
	{
		return 1;
	}

	int a2;
	if (!LoadFileArchiveToSBigBuf_(CurrentMapFileName, &a2, 0, 0))
	{
		return 0;
	}

	switch (gameData.approval_status)
	{
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		return a2 == gameData.approval_status;
	}
	return 0;
}

FUNCTION_PATCH(sub_4C4870, sub_4C4870_);

void Cls2RecvFrom_()
{
	for (int v0 = _countof(playerStatusArray) - 1; v0 >= 0; v0--)
	{
		dword_512680 = v0;
		if (playerStatusArray[v0] & 0x20000)
		{
			LOBBYCLASS2_RECV(arraydata[v0], arraydatabytes[v0], v0);
		}
	}
	dword_512680 = 8;
}

FAIL_STUB_PATCH(Cls2RecvFrom);

int gameLoopTurns_()
{
	if (glGluesMode == GLUE_GENERIC)
	{
		return 0;
	}
	if (!RecvSaveTurns_() && InGame)
	{
		timeoutProcDropdown_();
	}
	if (!byte_57EE78)
	{
		return 0;
	}
	if (glGluesMode == GLUE_GENERIC)
	{
		return 0;
	}
	GameKeepAlive_();
	if (glGluesMode == GLUE_GENERIC)
	{
		return 0;
	}
	if (InGame)
	{
		ParseGameRecvInfo();
	}
	else
	{
		Cls2RecvFrom_();
	}
	if (glGluesMode == GLUE_GENERIC)
	{
		return 0;
	}
	++turn_counter;
	if (InGame)
	{
		UpdateGameHash();
	}
	return 1;
}

FAIL_STUB_PATCH(gameLoopTurns);

int GameLoopWaitSendTurn_(int* a1)
{
	*a1 = 0;
	if (--FramesUntilNextTurn)
	{
		return 1;
	}
	else if (gameLoopTurns_())
	{
		if (byte_51CE9D)
		{
			dword_6D6370 = GetTickCount() - dword_6D6370;
			if ((unsigned int)dword_6D6370 >= 0x64)
			{
				dword_51CE94 = ((LatencyFrames[registry_options.GameSpeed] * GameSpeedModifiers.gameSpeedModifiers[registry_options.GameSpeed]) >> 1) + GetTickCount();
			}
			else
			{
				dword_51CE94 += GameSpeedModifiers.gameSpeedModifiers[registry_options.GameSpeed] >> 1;
			}
		}
		byte_51CE9D = 0;
		FramesUntilNextTurn = LatencyFrames[registry_options.GameSpeed];
		if (multiPlayerMode)
		{
			*a1 = 1;
			EnableVisibilityHashUpdate();
			minimapPreviewUpdateState_();
			saveMinimapCounts();
		}
		return 1;
	}
	else
	{
		FramesUntilNextTurn = 1;
		if (!byte_51CE9D)
		{
			byte_51CE9D = 1;
			dword_6D6370 = GetTickCount();
		}
		return 0;
	}
}

FAIL_STUB_PATCH(GameLoopWaitSendTurn);

void replayFrameComputation_()
{
	int v0 = ElapsedTimeFrames * dword_4FF90C[replay_header.game_data.game_speed];
	int v1 = 0;
	if (v0 / 1000 != dword_50E05C)
	{
		dword_50E05C = v0 / 1000;
		v1 = 1;
	}
	int v2 = 0;
	if (is_replay_paused)
	{
		DWORD v3 = GetTickCount();
		if (v3 - dword_6D11B8 > 600)
		{
			dword_6D11B8 = v3;
			v2 = 1;
			dword_6D11B4 = dword_6D11B4 == 0;
		}
	}
	if (v1 || v2)
	{
		CanUpdateCurrentButtonSet = 1;
		CanUpdateSelectedUnitPortrait = 1;
		CanUpdateStatDataDialog = 1;
		ctrl_under_mouse = 0;
		ctrl_under_mouse_val = 0;
	}
}

FAIL_STUB_PATCH(replayFrameComputation);

void BWFXN_ExecuteGameTriggers_(signed int dwMillisecondsPerFrame);

void GameLoop_State_()
{
	DWORD v10 = GetTickCount() + 2000;
	dword_6D11F0 = 0;

	if (InReplay)
	{
		replayLoop_();
	}

	int v5;
	if (!GameLoopWaitSendTurn_(&v5))
	{
		dword_6D11F0 = 1;
		return;
	}
	if (InReplay && is_replay_paused)
	{
		replayFrameComputation_();
		dword_6D11F0 = 2;
		return;
	}
	if (!GameState)
	{
		dword_6D11F0 = 4;
		return;
	}

	int v9 = 0;
	if (is_app_active || multiPlayerMode)
	{
		ScreenLayers[5].bits |= 2;
		if (BWFXN_IsPaused_())
		{
			RefreshAllUnits_();
		}
		else
		{
			++ElapsedTimeFrames;
			v9 = 1;
			GameLoop_();
		}
		SetInGameLoop(1);
		BWFXN_ExecuteGameTriggers_(GameSpeedModifiers.gameSpeedModifiers[registry_options.GameSpeed]);
		SetInGameLoop(0);
		if (InReplay)
		{
			replayFrameComputation_();
		}
	}
	dword_51CE94 += GameSpeedModifiers.gameSpeedModifiers[registry_options.GameSpeed];
	DWORD v6 = GetTickCount();
	if (v6 < dword_51CE94)
	{
		IsRunning = v9;
		dword_6D11F0 = 5;
		return;
	}
	if (v10 < v6)
	{
		IsRunning = v9;
		dword_6D11F0 = 6;
		return;
	}
	if (v5 && (!InReplay || !dword_6D11E8))
	{
		IsRunning = v9;
		dword_6D11F0 = 7;
		return;
	}
}

FAIL_STUB_PATCH(GameLoop_State);

void GameLoop_Top_()
{
	if (has_viewport)
	{
		FramesUntilNextTurn = 1;
		byte_51CE9D = 0;
		dword_51CE94 = GetTickCount();
		bool v2 = false;
		while (GameState)
		{
			BWFXN_videoLoop_(3);
			DWORD v1 = GetTickCount();
			if (!byte_51CE9D && abs(int(dword_51CE94 - v1)) > GameSpeedModifiers.altSpeedModifiers[registry_options.GameSpeed])
			{
				dword_51CE94 = v1;
			}
			BWFXN_NextFrameHelperFunctionTarget_();
			if (v1 + dword_51CE8C[0] > 0xA)
			{
				dword_51CE8C[0] = -v1;
				PollInput_();
				v2 = true;
			}
			RecvMessage_();
			LeagueChatFilter();
			if (v1 >= dword_51CE94)
			{
				GameLoop_State_();
				v2 = true;
			}
			updateHUDInformation_();
			if (dword_5968EC || v2)
			{
				dword_5968EC = 0;
				v2 = false;
				BWFXN_RedrawTarget_();
			}
		}
	}
	else
	{
		while (GameState)
		{
			GameLoop_State_();
			updateHUDInformation_();
		}
	}
}

FAIL_STUB_PATCH(GameLoop_Top);

GamePosition BeginGame_()
{
	visionUpdateCount = 1;
	DLGMusicFade_(current_ingame_music_track);
	if (has_viewport)
	{
		_SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	}
	GameState = 1;
	TickCountSomething(0);
	DoGameLoop_();
	RefreshLayer5();
	setCursorType_(getCursorType_());
	cursorRefresh();
	if (!multiPlayerMode && !getMapStartStatus() && !InReplay && (registry_options.field_18 & 0x100) != 0)
	{
		loadTips_BINDLG(1);
	}
	SetMapStartStatus();
	SetCurrentPaletteInfo(palette, 0x100u, 0);
	get_tFontGam_PCX();
	TitlePaletteUpdate_(3);
	if (multiPlayerMode)
	{
		registry_options.GameSpeed = gameData.game_speed;
	}
	else if (!active_campaign)
	{
		registry_options.GameSpeed = 4;
	}
	newGame(1);
	BWFXN_videoLoop_(0);
	loseSightSelection();
	turn_counter = 0;
	GameKeepAlive_();
	while (GameState && !gameLoopTurns_())
	{
		BWFXN_RedrawTarget_();
	}
	GameLoop_Top_();
	if (on_end_game)
	{
		on_end_game();
	}
	newGame(0);
	stopAllSound_();
	sub_41E9E0(3);
	get_tFontGam_PCX_0();
	RefreshCursor_0();
	return gwNextGameMode;
}

FAIL_STUB_PATCH(BeginGame);

void GameRun_()
{
	IsInGameLoop = 1;
	int v1 = LoadGameInit_();
	IsInGameLoop = 0;
	if (!InReplay)
	{
		if (!gameData.width)
			gameData.width = map_size.width;
		if (!gameData.height)
			gameData.height = map_size.height;
		if (!gameData.tileset)
			gameData.tileset = CurrentTileSet;
		if (!gameData.game_type)
			gameData.game_type = (GameType) gameData.got_file_values.template_id;
		if (!gameData.game_type_unk)
			gameData.game_type_unk = gameData.got_file_values.unused1;
		if (!gameData.game_type_param)
			gameData.game_type_param = gameData.got_file_values.variation_id;
		SetReplayData(&gameData, Players, factionsColorsOrdering);
	}
	if (loadGameFileHandle)
	{
		fclose(loadGameFileHandle);
		loadGameFileHandle = 0;
	}
	GamePosition next_game_position = v1 ? BeginGame_() : GAME_GLUES;
	DestroyGame_();
	gwGameMode = next_game_position;
}

FAIL_STUB_PATCH(GameRun);

bool statBtn_dlg_CharPress_(dlgEvent* evt)
{
	if (evt->wUnk_0x0A & 0x100)
	{
		return 0;
	}
	char v3 = tolower(evt->wVirtKey);
	if (!DLG_IterateChildren(current_dialog->fields.dlg.pFirstChild, (int(__fastcall*)(_DWORD, _DWORD))sub_4588C0, (int)&v3))
	{
		return 0;
	}

	dlgEvent v2;
	v2.wNo = EVN_USER;
	v2.dwUser = USER_HOTKEY;
	v2.wSelection = 0;
	v2.wUnk_0x06 = 0;
	v2.cursor.x = Mouse.x;
	v2.cursor.y = Mouse.y;
	return interrupting_child->pfcnInteract(interrupting_child, &v2);
}

FAIL_STUB_PATCH(statBtn_dlg_CharPress);

void sub_458E70_(dialog* a1)
{
	a1->wUser = -1;
	a1->pfcnUpdate = statbtn_Btn_Update;
	HideDialog_(a1);
}

FAIL_STUB_PATCH(sub_458E70);

void sub_458BB0_(dialog* dlg)
{
	ButtonOrder* order = (ButtonOrder*)dlg->lUser;
	order->action(order->action_variable, is_keycode_used[VK_SHIFT]);
}

FAIL_STUB_PATCH(sub_458BB0);

void statflufDlgUpdate_(dialog* dlg)
{
	dlg->lFlags |= CTRL_TRANSPARENT;
	BINDLG_BlitSurface_(dlg);
	if ((dlg->lFlags & CTRL_UPDATE) == 0)
	{
		dlg->lFlags |= CTRL_UPDATE;
		updateDialog(dlg);
	}
}

FAIL_STUB_PATCH(statflufDlgUpdate);

int __fastcall statfluf_DLG_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
	case EVN_MOUSEMOVE:
	case EVN_LBUTTONDOWN:
	case EVN_LBUTTONDBLCLK:
	case EVN_RBUTTONDOWN:
	case EVN_RBUTTONDBLCLK:
	case EVN_CHAR:
		return 0;
	case EVN_USER:
		if (evt->dwUser == EventUser::USER_CREATE)
		{
			statflufDlgUpdate_(dlg);
		}
		else if (evt->dwUser == EventUser::USER_MOUSEMOVE)
		{
			return 0;
		}
		break;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(statfluf_DLG_Interact);

void load_statfluf_BIN_()
{
	statfluf_current_race = InReplay ? RaceId::RACE_Other : consoleIndex;
	StatFlufDialog* dlgs = statfluf_dialogs_[statfluf_current_race];
	for (int i = 0; dlgs[i].position.left != -1; i++)
	{
		dlgs[i].dialog = LoadDialog("rez\\statfluf.bin"); // The top decoratios of the main (bottom-of-the-screen) in-game UI
		dlgs[i].dialog->rct.left = dlgs[i].position.left;
		dlgs[i].dialog->rct.top = dlgs[i].position.top;
		dlgs[i].dialog->rct.right = dlgs[i].position.right - 1;
		dlgs[i].dialog->rct.bottom = dlgs[i].position.bottom - 1;
		dlgs[i].dialog->fields.dlg.dstBits_wid = dlgs[i].position.right;
		dlgs[i].dialog->fields.dlg.dstBits_ht = dlgs[i].position.bottom;
		InitializeDialog_(dlgs[i].dialog, statfluf_DLG_Interact_);
	}
}

FAIL_STUB_PATCH(load_statfluf_BIN);

void __fastcall statPortBtnUpdate_(dialog* dlg, int x, int y, rect* dst)
{
	bounds a1;

	if (UnitPortraits)
	{
		a1.width = stru_68AC80.wid;
		a1.right = stru_68AC80.wid - 1;
		a1.height = stru_68AC80.ht;
		a1.bottom = stru_68AC80.ht - 1;
		a1.left = 0;
		a1.top = 0;
		BlitSurface(&a1, &stru_68AC80, (unsigned __int16)dlg->rct.left, dlg->rct.top);
		if ((unsigned __int8)byte_68AC9C < 80)
		{
			sub_45E570(&dlg->rct, byte_68AC9C);
		}
	}

	a1.width = stru_68AC78.wid;
	a1.right = stru_68AC78.wid - 1;
	a1.height = stru_68AC78.ht;
	a1.bottom = stru_68AC78.ht - 1;
	a1.left = 0;
	a1.top = 0;
	BWFXN_PrintIcon(&a1, &stru_68AC78, (unsigned __int16)dlg->rct.left, dlg->rct.top);
}

FAIL_STUB_PATCH(statPortBtnUpdate);

void sub_45E770_(dialog* dlg)
{
	dlg->lUser = 0;
	dlg->wUser = word_51521C[consoleIndex];
	dlg->pfcnUpdate = statPortBtnUpdate_;
	dword_68AC98 = dlg;
}

FAIL_STUB_PATCH(sub_45E770);

void moveScreenToUnit_(CUnit* unit)
{
	BWFXN_MoveScreen(unit->sprite->position.x - GAME_AREA_WIDTH / 2, unit->sprite->position.y - GAME_AREA_HEIGHT / 2);
}

void moveScreenToUnit__(__int16 x)
{
	CUnit* unit;

	__asm mov unit, eax

	moveScreenToUnit_(unit);
}

FUNCTION_PATCH((void*)0x4E6020, moveScreenToUnit__);

int __fastcall statPortBtnInteract_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			sub_45E770_(dlg);
			SetCallbackTimer(1, dlg, 50, sub_45EC40_);
			SetCallbackTimer(2, dlg, 30, sub_45EC40_);
			return 1;
		case EventUser::USER_DESTROY:
			waitLoopCntd(1, dlg);
			waitLoopCntd(2, dlg);
			waitLoopCntd(3, dlg);
			waitLoopCntd(4, dlg);
			dword_68AC98 = 0;
			break;
		case EventUser::USER_ACTIVATE:
			if (CUnit* unit = (CUnit*) dlg->lUser)
			{
				moveScreenToUnit_(unit);
			}
			break;
		}
	}

	return GenericDlgInteractFxns[dlg->wCtrlType](dlg, evt);
}

FAIL_STUB_PATCH(statPortBtnInteract);

void statport_Buttonpress_(dialog* dlg)
{
	dialog* v2 = getControlFromIndex_(dlg, 1);
	stru_68AC80.wid = v2->rct.right - v2->rct.left + 1;
	dword_68AC90 = (__int16)stru_68AC80.wid;
	dword_68AC6C = (__int16)stru_68AC80.wid - 1;
	dword_68AC64 = 0;
	dword_68AC68 = 0;
	stru_68AC80.ht = v2->rct.bottom - v2->rct.top + 1;
	dword_68AC94 = (__int16)stru_68AC80.ht;
	dword_68AC70 = (__int16)stru_68AC80.ht - 1;

	unsigned v5 = (__int16)stru_68AC80.wid * (__int16)stru_68AC80.ht;
	stru_68AC80.data = (u8*)SMemAlloc((__int16)stru_68AC80.wid * dword_68AC94, "Starcraft\\SWAR\\lang\\statport.cpp", 635, 0);

	memset(stru_68AC80.data, HIBYTE(dword_6CEB2C), v5);
	memset(&stru_68AC80.data[4 * (v5 >> 2)], HIBYTE(dword_6CEB2C), v5 & 3);

	static FnInteract functions[] = {
		statPortBtnInteract_,
	};

	registerUserDialogAction(dlg, sizeof(functions), functions);
	BINDLG_BlitSurface_(dlg);

	char buff[MAX_PATH];
	sprintf_s(buff, "%s%c%s", "game\\", InReplay ? 'n' : race_lowercase_char_id[consoleIndex], "conover.pcx");
	stru_68AC78.data = 0;

	void* buffer;
	int width;
	int height;
	if (!SBmpAllocLoadImage(buff, 0, &buffer, &width, &height, 0, 0, allocFunction))
	{
		throw FileNotFoundException(buff, SErrGetLastError());
	}
	stru_68AC78.wid = (unsigned __int16)width;
	stru_68AC78.ht = height;
	stru_68AC78.data = (u8*)buffer;
	dword_68AC88 = getControlFromIndex_(dlg, -10)->pszText;
}

FAIL_STUB_PATCH(statport_Buttonpress);

int __fastcall statport_Dlg_Interact_(dialog* dlg, struct dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
	case EVN_MOUSEMOVE:
		return 0;
	case EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			statport_Buttonpress_(dlg);
			break;
		case USER_DESTROY:
			statport_alloc();
			break;
		case USER_MOUSEMOVE:
			return 1;
		}
		break;
	case EVN_CHAR:
		return sub_45E9F0(evt);
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(statport_Dlg_Interact);

void loadPortdata_BINDLG_()
{
	unsigned __int16* v4 = (unsigned short*) fastFileRead_(0, 0, "arr\\portdata.tbl", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	dword_6D5C9C = v4;
	for (int i = 0; i < 110; i++)
	{
		const char* v9;
		if (LOWORD(dword_655C58[i]))
		{
			v9 = LOWORD(dword_655C58[i]) - 1 < *v4 ? (char*)v4 + v4[LOWORD(dword_655C58[i])] : "";
		}
		else
		{
			v9 = 0;
		}
		dword_655C58[i] = (int)v9;

		if (LOWORD(dword_655E80[i]))
		{
			v9 = LOWORD(dword_655E80[i]) - 1 < *v4 ? (char*)v4 + v4[LOWORD(dword_655E80[i])] : "";
		}
		else
		{
			v9 = 0;
		}
		dword_655E80[i] = (int)v9;
	}

	DatLoad* v28;
	for (v28 = portdataDat; v28->address != byte_655E10; ++v28);

	dword_57FD34 = MoveToX;
	dword_57FD38 = MoveToY;
	dword_68AC50[0] = (BYTE*)v28[0].address;
	dword_68AC50[1] = (BYTE*)v28[1].address;
	dword_68AC50[2] = (BYTE*)v28[2].address;
	dword_68AC50[3] = (BYTE*)v28[3].address;
	dword_68AC60 = 0;
	dword_68AC4C = 0;
	CanUpdateSelectedUnitPortrait = 1;

	statport_Dlg = LoadDialog("rez\\statport.bin");
	InitializeDialog_(statport_Dlg, statport_Dlg_Interact_);
}

FAIL_STUB_PATCH(loadPortdata_BINDLG);

void load_statlb_()
{
	if (!multiPlayerMode
		|| gameData.got_file_values.victory_conditions == VC_RESOURCES
		|| gameData.got_file_values.victory_conditions == VC_SLAUGHTER
		|| gameData.got_file_values.victory_conditions == VC_MAP_DEFAULT && gameData.got_file_values.starting_units == SU_MAP_DEFAULT && !gameData.got_file_values.tournament_mode
		|| InReplay)
	{
		byte_59728C = 0;
		dword_597288 = 1;

		statlb_Dlg = LoadDialog("rez\\statlb.bin");
		InitializeDialog_(statlb_Dlg, statlb_Dlg_Interact);
	}
}

FAIL_STUB_PATCH(load_statlb);

void __stdcall hideLeftmostResource_(int a1)
{
	if (statres_Dlg)
	{
		dialog* v3 = getControlFromIndex_(statres_Dlg, 6);
		if (a1)
		{
			showDialog_(v3);
		}
		else
		{
			HideDialog_(v3);
		}
		v3->lUser = 0;
	}
}

FAIL_STUB_PATCH(hideLeftmostResource);

int __fastcall StatRes_CustomCtrl_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			dlg->lUser = 0;
			dlg->pfcnUpdate = statRes_Text_Update;
			dlg->pfcnInteract = statRes_Text_Interact;
			dlg->wUser = dlg->wIndex == 6 ? 10 : 0;
			[[fallthrough]];
		case USER_NEXT:
		case USER_UNK_8:
		case USER_INIT:
		case USER_SHOW:
		case USER_HIDE:
			return genericCommonInteract(evt, dlg);
		}
	}

	return 0;
}

FAIL_STUB_PATCH(StatRes_CustomCtrl);

void StatRes_RegisterCustomProcs_(dialog* dlg)
{
	static FnInteract functions[] = {
		StatRes_CustomCtrl_,
		StatRes_CustomCtrl_,
		StatRes_CustomCtrl_,
		StatRes_CustomCtrl_,
		StatRes_CustomCtrl_,
		StatRes_CustomCtrl_,
	};

	dlg->lFlags |= CTRL_USELOCALGRAPHIC | CTRL_REVERSE | CTRL_TRANSPARENT;

	registerUserDialogAction(dlg, sizeof(functions), functions);
	SetCallbackTimer(1, dlg, 20, StatRes_MainProc);
	hideLeftmostResource_(0);
}

FAIL_STUB_PATCH(StatRes_RegisterCustomProcs);

int __fastcall StatRes_DialogInteract_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_KEYFIRST:
	case EventNo::EVN_KEYRPT:
	case EventNo::EVN_MOUSEMOVE:
	case EventNo::EVN_LBUTTONDOWN:
	case EventNo::EVN_LBUTTONDBLCLK:
	case EventNo::EVN_RBUTTONDOWN:
	case EventNo::EVN_RBUTTONDBLCLK:
	case EventNo::EVN_CHAR:
		return 0;
	case EventNo::EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			StatRes_RegisterCustomProcs_(dlg);
			break;
		case EventUser::USER_DESTROY:
			waitLoopCntd(1, dlg);
			break;
		case EventUser::USER_MOUSEMOVE:
			return 0;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(StatRes_DialogInteract);

void load_StatRes_BIN_()
{
	game_icons_grp = (void*)LoadGraphic("game\\icons.grp", 0, "Starcraft\\SWAR\\lang\\statres.cpp", 490);
	dword_68C230 = 0;
	dword_68C22C = 0;

	statres_Dlg = LoadDialog("rez\\statres.bin"); // The topmost row of in-game UI
	InitializeDialog_(statres_Dlg, StatRes_DialogInteract_);
}

FAIL_STUB_PATCH(load_StatRes_BIN);

void ProgressBar_Create_(dialog* a1)
{
	char buff[MAX_PATH];

	a1->pfcnUpdate = statdata_ProgressBarUpdate;
	a1->wUser = 0;
	if (!progress_bar_empty_pcx.data)
	{
		sprintf_s(buff, "%s%c%s", "game\\", race_lowercase_char_id[consoleIndex], "pbrempt.pcx");
		AllocBackgroundImage(buff, &progress_bar_empty_pcx, 0, "Starcraft\\SWAR\\lang\\statdata.cpp", 222);
	}
	if (!progress_bar_full_pcx.data)
	{
		sprintf_s(buff, "%s%c%s", "game\\", race_lowercase_char_id[consoleIndex], "pbrfull.pcx");
		AllocBackgroundImage(buff, &progress_bar_full_pcx, 0, "Starcraft\\SWAR\\lang\\statdata.cpp", 226);
	}
}

FAIL_STUB_PATCH(ProgressBar_Create);

int __fastcall statdata_ProgressBarInteract_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			ProgressBar_Create_(dlg);
			break;
		case EventUser::USER_DESTROY:
			ProgressBar_Destroy();
			break;
		}
	}
	return GenericDlgInteractFxns[dlg->wCtrlType](dlg, evt);
}

FAIL_STUB_PATCH(statdata_ProgressBarInteract);

int __fastcall statdata_UnitWireframeTransit_(dialog* dlg, dlgEvent* evn)
{
	if (evn->wNo == EventNo::EVN_MOUSEMOVE)
	{
		sub_457DE0(dlg, evn);
	}
	else if (evn->wNo == EventNo::EVN_USER)
	{
		switch (evn->dwUser)
		{
		case EventUser::USER_CREATE:
			dlg->pfcnUpdate = statdata_UnitWireframeTransUpdate;
			dlg->wUser = 0;
			dlg->lUser = 0;
			break;
		case EventUser::USER_ACTIVATE:
			CMDACT_Unload((CUnit*) dlg->lUser);
			return 1;
		case EventUser::USER_MOUSEMOVE:
			if (dlg->lFlags & DialogFlags::CTRL_VISIBLE)
			{
				return 1;
			}
			break;
		case EventUser::USER_NEXT:
			if (dlg == ctrl_under_mouse)
			{
				refreshSelectionScreen_();
				ctrl_under_mouse = 0;
			}
			break;
		}
	}

	return GenericDlgInteractFxns[dlg->wCtrlType](dlg, evn);
}

FAIL_STUB_PATCH(statdata_UnitWireframeTransit);

int __fastcall statdata_UnitWireframeSelection_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_MOUSEMOVE)
	{
		sub_457DE0(dlg, evt);
	}
	else if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			dlg->pfcnUpdate = statdata_UnitWireframeSelectUpdate;
			dlg->wUser = 0;
			dlg->lUser = (int)SMemAlloc(8, "Starcraft\\SWAR\\lang\\statdata.cpp", 775, 0);
			break;
		case EventUser::USER_DESTROY:
			if (dlg->lUser)
			{
				SMemFree((void*) dlg->lUser, "Starcraft\\SWAR\\lang\\statdata.cpp", 780, 0);
			}
			break;
		case EventUser::USER_ACTIVATE:
			StatusScreenButton(dlg, dlg);
			return 1;
		case EventUser::USER_MOUSEMOVE:
			if (dlg->lFlags & DialogFlags::CTRL_VISIBLE)
			{
				return 1;
			}
			break;
		case EventUser::USER_NEXT:
			if (dlg == ctrl_under_mouse)
			{
				refreshSelectionScreen_();
				ctrl_under_mouse = 0;
			}
			break;
		}
	}

	return GenericDlgInteractFxns[dlg->wCtrlType](dlg, evt);
}

FAIL_STUB_PATCH(statdata_UnitWireframeSelection);

void statdata_extendedCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		statdata_UnitWireframeInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_ProgressBarInteract_,
		statdata_Unknown,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_buttonInteract,
		statdata_ProgressBarInteract_,
		statdata_ProgressBarInteract_,
		statdata_buttonInteract,
		statdata_ProgressBarInteract_,
		statdata_buttonInteract,
		statdata_UnitWireframeTransit_, // large unit (leftmost)
		statdata_UnitWireframeTransit_, // large unit (rightmost)
		statdata_UnitWireframeTransit_, // large unit (middle)
		statdata_UnitWireframeTransit_, // medium unit (1st left to right)
		statdata_UnitWireframeTransit_, // medium unit (2nd left to right)
		statdata_UnitWireframeTransit_, // medium unit (3rd left to right)
		statdata_UnitWireframeTransit_, // medium unit (4th left to right)
		statdata_UnitWireframeTransit_, // small unit (top row, 1st left to right)
		statdata_UnitWireframeTransit_, // small unit (bottom row, 1st left to right)
		statdata_UnitWireframeTransit_, // small unit (top row, 2nd left to right)
		statdata_UnitWireframeTransit_, // small unit (bottom row, 2nd left to right)
		statdata_UnitWireframeTransit_, // small unit (top row, 3rd left to right)
		statdata_UnitWireframeTransit_, // small unit (bottom row, 3rd left to right)
		statdata_UnitWireframeTransit_, // small unit (top row, 4th left to right)
		statdata_UnitWireframeTransit_, // small unit (bottom row, 4th left to right)
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
		statdata_UnitWireframeSelection_,
	};

	registerUserDialogAction(dlg, sizeof(functions), functions);
	BINDLG_BlitSurface_(dlg);
	CanUpdateStatDataDialog = 1;
}

FAIL_STUB_PATCH(statdata_extendedCtrlID);

int __fastcall statdata_dlg_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
	case EVN_CHAR:
		return 0;
		break;
	case EVN_MOUSEMOVE:
		statdata_Destroy(dlg, evt);
		break;
	case EVN_USER:
		if (evt->dwUser == USER_CREATE)
		{
			statdata_extendedCtrlID_(dlg);
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(statdata_dlg_Interact);

void load_Statdata_BIN_()
{
	dword_68C1F4 = LoadGraphic("unit\\wirefram\\tranwire.grp", 0, "Starcraft\\SWAR\\lang\\statdata.cpp", 1335);
	dword_68C1FC = LoadGraphic("unit\\wirefram\\grpwire.grp", 0, "Starcraft\\SWAR\\lang\\statdata.cpp", 1337);
	CanUpdateStatDataDialog = 1;
	statusScreenFunc = 0;
	ctrl_under_mouse = 0;
	ctrl_under_mouse_val = -1;

	stardata_Dlg = LoadDialog("rez\\statdata.bin"); // The middle area of the main (bottom-of-the-screen) in-game UI
	InitializeDialog_(stardata_Dlg, statdata_dlg_Interact_);
}

FAIL_STUB_PATCH(load_Statdata_BIN);

int __fastcall statbtn_Btn_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_MOUSEMOVE:
		sub_4597C0(dlg, evt);
		break;
	case EVN_LBUTTONDOWN:
	case EVN_LBUTTONDBLCLK:
		if (dlg->lFlags & CTRL_DISABLED)
		{
			return 0;
		}
		break;
	case EVN_LBUTTONUP:
		GenericDlgInteractFxns[dlg->wCtrlType](dlg, evt);
		statBtn_dlg_MouseMove(current_dialog, evt);
		return 1;
	case EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			sub_458E70_(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			sub_458BB0_(dlg);
			return 1;
		case EventUser::USER_MOUSEMOVE:
			if (dlg->lFlags & CTRL_DISABLED)
			{
				return 1;
			}
			break;
		}
	}

	return GenericDlgInteractFxns[dlg->wCtrlType](dlg, evt);
}

FAIL_STUB_PATCH(statbtn_Btn_Interact);

void statbtn_BIN_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_Btn_Interact_,
		statbtn_BIN_ReplayProgressbar,
	};

	registerUserDialogAction(dlg, sizeof(functions), functions);
	BINDLG_BlitSurface_(dlg);
}

FAIL_STUB_PATCH(statbtn_BIN_CustomCtrlID);

int __fastcall statbtn_DLG_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
		return 0;
	case EVN_MOUSEMOVE:
		statBtn_dlg_MouseMove(dlg, evt);
		break;
	case EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			statbtn_BIN_CustomCtrlID_(dlg);
			break;
		case USER_MOUSEMOVE:
			return 1;
		}
		break;
	case EVN_CHAR:
		return statBtn_dlg_CharPress_(evt);
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(statbtn_DLG_Interact);

void load_statbtn_BIN_()
{
	char buff[MAX_PATH];

	word_68C14C = 228;
	word_68C1C8 = 228;
	word_68C1C4 = 228;
	word_68C1BC = -1;
	sprintf_s(buff, "unit\\cmdbtns\\%ccmdbtns.grp", race_lowercase_char_id[consoleIndex]);
	cmdbtns_grp = (void*)LoadGraphic(buff, 0, "Starcraft\\SWAR\\lang\\statcmd.cpp", 1086);
	cmdicons_grp = (void*)LoadGraphic("unit\\cmdbtns\\cmdicons.grp", 0, "Starcraft\\SWAR\\lang\\statcmd.cpp", 1089);
	if (!SBmpLoadImage("unit\\cmdbtns\\ticon.pcx", 0, byte_68C150, 96, 0, 0, 0))
	{
		throw FileNotFoundException("unit\\cmdbtns\\ticon.pcx", SErrGetLastError());
	}

	sprintf_s(buff, "rez\\statbtn%c.bin", InReplay ? 'n' : race_lowercase_char_id[consoleIndex]);
	current_dialog = LoadDialog(buff);
	InitializeDialog_(current_dialog, statbtn_DLG_Interact_);
}

FAIL_STUB_PATCH(load_statbtn_BIN);

void drawGameMenuContextHelp_(dialog* dlg)
{
	char* v1;

	if (*StatTxtTbl.buffer > 0x32Du)
	{
		v1 = (char*)StatTxtTbl.buffer + StatTxtTbl.buffer[814];
	}
	else
	{
		v1 = "";
	}

	CreateContextHelp(dlg->rct.right + dlg->fields.ctrl.pDlg->rct.left, dlg->rct.top + dlg->fields.ctrl.pDlg->rct.top, v1);
}

FAIL_STUB_PATCH(drawGameMenuContextHelp);

void setActiveDlgElement_(dialog* dlg, struct dlgEvent* evt)
{
	if (IS_GAME_PAUSED && !multiPlayerMode || dlg != dlgSetMouseOver(dlg->fields.ctrl.pDlg, evt))
	{
		dword_68C220 = 0;
		refreshSelectionScreen_();
	}
	else if (dlg != dword_68C220 || !dword_655C48)
	{
		dword_68C220 = dlg;
		drawGameMenuContextHelp_(dlg);
	}
}

FAIL_STUB_PATCH(setActiveDlgElement);

void load_gamemenu_(dialog* dlg)
{
	if (dlg->wIndex == 1)
	{
		dword_6D1234 = sub_4CA450;
		BWFXN_OpenGameDialog_("rez\\gamemenu.bin", gamemenu_Dlg_Interact_);
	}
}

FAIL_STUB_PATCH(load_gamemenu);

void Statf10_MouseMoveEvt_(dialog* dlg, struct dlgEvent* evt)
{
	dialog* v3;

	if (!IS_GAME_PAUSED || multiPlayerMode)
	{
		v3 = dlgSetMouseOver(dlg, evt);
		if (v3 && dlg != v3)
		{
			if (v3 != dword_68C220 || !dword_655C48)
			{
				dword_68C220 = v3;
				drawGameMenuContextHelp_(v3);
			}
			return;
		}
	}
	else
	{
		v3 = 0;
	}

	if (v3 != dword_68C220)
	{
		dword_68C220 = v3;
		refreshSelectionScreen_();
	}
}

FAIL_STUB_PATCH(Statf10_MouseMoveEvt);

int __fastcall statf10_ButtonInteract_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_MOUSEMOVE:
		setActiveDlgElement_(dlg, evt);
		break;
	case EventNo::EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			dlg->wUser = word_51276C;
			dlg->pfcnUpdate = statf10_ButtonUpdate;
			break;
		case EventUser::USER_ACTIVATE:
			load_gamemenu_(dlg);
			break;
		}

		if (dlg->wIndex == -3 || dlg->wIndex == -2)
		{
			LastControlID = dlg->wIndex;
			dlg->fields.ctrl.pDlg->pfcnInteract(dlg->fields.ctrl.pDlg, evt);
			return 1;
		}
	}

	return GenericControlInteract(dlg, evt);
}

FAIL_STUB_PATCH(statf10_ButtonInteract);

void Statf10_RegisterCustomProcs_(dialog* dlg)
{
	static FnInteract functions[] = {
		statf10_ButtonInteract_,
	};

	dlg->lFlags |= DialogFlags::CTRL_USELOCALGRAPHIC;
	registerUserDialogAction(dlg, sizeof(functions), functions);
	BINDLG_BlitSurface_(dlg);
}

FAIL_STUB_PATCH(Statf10_RegisterCustomProcs);

int __fastcall Statf10_DialogProc_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_KEYRPT:
		return 0;
	case EventNo::EVN_MOUSEMOVE:
		Statf10_MouseMoveEvt_(dlg, evt);
		[[fallthough]];
	case EventNo::EVN_KEYFIRST:
	case EventNo::EVN_LBUTTONDOWN:
	case EventNo::EVN_LBUTTONUP:
	case EventNo::EVN_LBUTTONDBLCLK:
	case EventNo::EVN_RBUTTONDOWN:
	case EventNo::EVN_RBUTTONUP:
	case EventNo::EVN_RBUTTONDBLCLK:
	case EventNo::EVN_CHAR:
		if (IS_GAME_PAUSED)
		{
			genericDlgInteract(dlg, evt);
			return 1;
		}
		break;
	case EventNo::EVN_USER:
		if (evt->dwUser == USER_CREATE)
		{
			Statf10_RegisterCustomProcs_(dlg);
		}
		else if (evt->dwUser == USER_MOUSEMOVE)
		{
			return 1;
		}
		break;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(Statf10_DialogProc);

void load_Statf10_BIN_()
{
	stat_f10_Dlg = LoadDialog("rez\\stat_f10.bin"); // The menu button in the in-game UI
	InitializeDialog_(stat_f10_Dlg, Statf10_DialogProc_);
}

FAIL_STUB_PATCH(load_Statf10_BIN);

void sendChatMessage_(const char* message)
{
	short previous_text_filter = SendTextFilter;

	if (byte_68C144 == 2)
	{
		SendTextFilter = -1;
	}
	else if (byte_68C144 == 3)
	{
		prepareMessageAllyFilter();
		BWFXN_SendPublicCallTarget(message);
		SendTextFilter = previous_text_filter;
	}
	else
	{
		BWFXN_SendPublicCallTarget(message);
		SendTextFilter = previous_text_filter;
	}
}

FAIL_STUB_PATCH(sendChatMessage);

void onSendText_(dialog* a1, dlgEvent* a2, CheatFlags a3)
{
	if (byte_68C144)
	{
		// this is an inlined sub_4D16F0(0)
		if (GetUserDefaultLangID() == 1042)
		{
			if (!dword_6D6438)
			{
				dword_6D6438 = ImmGetContext(hWndParent);
			}
			dword_6D6438 = ImmAssociateContext(hWndParent, 0);
		}

		dialog* v4 = getControlFromIndex_(a1, 6);
		if ((_BYTE)a3)
		{
			char dest[256];
			SStrCopy(dest, v4->pszText, 0x7FFFFFFFu);
			a3 = GameCheats;
			if (CommandLineCheatCompare_(&a3, dest))
			{
				if (a3 != GameCheats)
				{
					CMDACT_UseCheat(a3);
				}
			}
			else if ((!InReplay || !replayCommand(dest)) && multiPlayerMode)
			{
				sendChatMessage_(dest);
			}
		}
		SStrCopy(v4->pszText, empty_string, 0x100u);
		v4->fields.edit.bCursorPos = 0;
		v4->fields.optn.bEnabled |= CTRL_UPDATE;
		if ((v4->lFlags & CTRL_VISIBLE) != 0 && (v4->lFlags & CTRL_UPDATE) == 0)
		{
			v4->lFlags |= CTRL_UPDATE;
			updateDialog(v4);
		}
		hAccTable = hAccel;
		input_procedures[16] = CMDACT_Hotkey_;
		byte_68C144 = 0;
	}
	else
	{
		// this is an inlined sub_4D16F0(1)
		if (GetUserDefaultLangID() == 1042)
		{
			HIMC v8 = dword_6D6438;
			if (!dword_6D6438)
			{
				v8 = ImmGetContext(hWndParent);
				dword_6D6438 = v8;
			}
			ImmAssociateContext(hWndParent, v8);
		}

		SNetGetLeaguePlayerName((int*)curPlayerID, 0x19u);
		hAccTable = dword_5968F4;
		input_procedures[16] = CMDACT_Hotkey_;

		if (!multiPlayerMode)
		{
			byte_68C144 = 1;
		}
		else if (a2->wUnk_0x0A & 3)
		{
			byte_68C144 = 2;
		}
		else if (sub_4AABB0() && (a2->wUnk_0x0A & 4))
		{
			byte_68C144 = 3;
		}
		else if (byte_581D60 == 8)
		{
			byte_68C144 = 2;
		}
		else if (!sub_4AABB0())
		{
			byte_68C144 = 4;
		}
		else if (a2->wUnk_0x0A != 9)
		{
			byte_68C144 = 4;
		}
		else
		{
			byte_68C144 = 3;
		}
	}
	sub_4F3040(a1);
	dialog* v13 = getControlFromIndex_(a1, 5);
	if (byte_68C144 || v13->pszText == NULL)
	{
		HideDialog_(v13);
	}
	else
	{
		showDialog_(v13);
	}
}

FAIL_STUB_PATCH(sub_4D3220);
FAIL_STUB_PATCH(onSendText);

int __fastcall textbox_CtrlInteract_(dialog* dlg, struct dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EventNo::EVN_KEYFIRST:
	case EventNo::EVN_KEYRPT:
		if (VK_PRIOR <= evt->wVirtKey && evt->wVirtKey <= VK_DOWN)
		{
			return 0;
		}
		break;
	case EventNo::EVN_USER:
		if (evt->dwUser == USER_NEXT && byte_68C144)
		{
			onSendText_(dlg->fields.ctrl.pDlg, evt, (CheatFlags) 0);
		}
		break;
	case EventNo::EVN_CHAR:
		textBox_ctrl_charEvent(dlg, evt);
		return 1;
	}

	return genericEditInteract(dlg, evt);
}

FAIL_STUB_PATCH(textbox_CtrlInteract);

void textbox_DLG_Init_Evt_(dialog* dlg)
{
	Font* v2;

	int v1 = (LOWORD(dlg->lFlags) | CTRL_TRANSPARENT) & 0x4C00;
	dlg->lFlags |= CTRL_USELOCALGRAPHIC | CTRL_REVERSE | CTRL_TRANSPARENT;
	byte_68C144 = 0;
	switch (v1)
	{
	case 0x400:
		v2 = FontBase[0];
		break;
	case 0x800:
		v2 = FontBase[2];
		break;
	case 0x4000:
		v2 = FontBase[3];
		break;
	default:
		v2 = FontBase[1];
		break;
	}
	BWFXN_SetFont(v2);

	dialog* v3 = dlg->wCtrlType == DialogType::cDLG ? dlg : dlg->fields.ctrl.pDlg;
	dialog* v4 = getControlFromIndex_(dlg, 1);
	if (v4)
	{
		do
		{
			if (v4->wIndex > 3)
			{
				break;
			}
			v4->rct.right = getMessageWidth(v4->pszText) + v4->rct.left;
			v4 = v4->pNext;
		} while (v4);
	}

	BWFXN_SetFont(0);
	dialog* v6 = getControlFromIndex_(dlg, 6);
	v6->pfcnInteract = textbox_CtrlInteract_;
	if (InReplay)
	{
		dlg->rct.top -= 24;
		dlg->rct.bottom -= 24;
	}
}

FAIL_STUB_PATCH(textbox_DLG_Init_Evt);

int __fastcall textbox_DLG_Interact_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
		return isSinglePaused();
	case EVN_MOUSEMOVE:
	case EVN_LBUTTONDOWN:
	case EVN_LBUTTONDBLCLK:
	case EVN_RBUTTONDOWN:
	case EVN_RBUTTONDBLCLK:
		return IS_GAME_PAUSED && !multiPlayerMode;
	case EVN_USER:
		if (evt->dwUser == USER_CREATE)
		{
			textbox_DLG_Init_Evt_(dlg);
		}
		else if (evt->dwUser == USER_MOUSEMOVE)
		{
			return 0;
		}
		break;
	case EVN_CHAR:
		if (isSinglePaused())
		{
			if (evt->wVirtKey == VK_ESCAPE)
			{
				keyPress_Escape_();
			}
			return 1;
		}
		if (evt->wVirtKey == 10 || evt->wVirtKey == VK_RETURN)
		{
			onSendText_(dlg, evt, (CheatFlags) 1);
		}
		else if (evt->wVirtKey == VK_ESCAPE && byte_68C144)
		{
			onSendText_(dlg, evt, (CheatFlags) 0);
		}
		else
		{
			break;
		}

		return 1;
	}

	bool result = genericDlgInteract(dlg, evt);
	if (IS_GAME_PAUSED && !multiPlayerMode)
	{
		return 1;
	}
	return result;
}

FAIL_STUB_PATCH(textbox_DLG_Interact);

void load_textbox_BIN_()
{
	char buff[80];
	sprintf_s(buff, "rez\\%ctextbox.bin", race_lowercase_char_id[consoleIndex]);
	textbox_bin = LoadDialog(buff);
	InitializeDialog_(textbox_bin, textbox_DLG_Interact_);
}

FAIL_STUB_PATCH(load_textbox_BIN);

void LoadConsoleImage_()
{
	char buff[MAX_PATH];
	char v0 = InReplay ? 'n' : race_lowercase_char_id[consoleIndex];
	sprintf_s(buff, "game\\%c%s", v0, "console.pcx");

	void* buffer;
	int width;
	int height;
	if (!SBmpAllocLoadImage(buff, 0, &buffer, &width, &height, 0, 0, allocFunction))
	{
		throw FileNotFoundException(buff, SErrGetLastError());
	}

	GameScreenConsole.ht = height;
	GameScreenConsole.wid = width;
	GameScreenConsole.data = (u8*)buffer;
	setHudBeginY(&GameScreenConsole);
	MainBltMask = BltMask_Constructor(&GameScreenConsole, 0, 0);
}

FAIL_STUB_PATCH(LoadConsoleImage);

void sub_47AAC0_()
{
	if (!SBmpLoadImage("game\\thpbar.pcx", 0, byte_66FBE4, 19, 0, 0, 0))
	{
		throw FileNotFoundException("game\\thpbar.pcx", SErrGetLastError());
	}
}

FAIL_STUB_PATCH(sub_47AAC0);

void setup_HUD_()
{
	if (has_hud)
	{
		LoadConsoleImage_();
		sub_47AAC0_();
		load_statfluf_BIN_();
		loadPortdata_BINDLG_();
		load_statlb_();
		load_StatRes_BIN_();
		load_Statdata_BIN_();
		load_WireframeGRP();
		load_statbtn_BIN_();
		load_Statf10_BIN_();
		load_textbox_BIN_();
		load_gluMinimap_();
	}
	else
	{
		u8 buffer[SCREEN_WIDTH * SCREEN_HEIGHT] = { 0 };
		GameScreenConsole.ht = SCREEN_HEIGHT;
		GameScreenConsole.wid = SCREEN_WIDTH;
		GameScreenConsole.data = buffer;
		MainBltMask = BltMask_Constructor(&GameScreenConsole, 0, 0);

		setMinimapConstants();
	}

	if (GameScreenConsole.data != NULL)
	{
		SMemFree(GameScreenConsole.data, "Starcraft\\SWAR\\lang\\status.cpp", 217, NULL);
		GameScreenConsole.data = NULL;
	}
	memset(ClientSelectionGroup, 0, sizeof(ClientSelectionGroup));
	ClientSelectionCount = 0;
	ActivePortraitUnit = NULL;
	ctrl_under_mouse = NULL;
	ctrl_under_mouse_val = NULL;
	CanUpdateCurrentButtonSet = 1;
	CanUpdateSelectedUnitPortrait = 1;
	CanUpdateStatDataDialog = 1;
}

FAIL_STUB_PATCH(setup_HUD);

bool __stdcall ChkLoader_TYPE_(SectionData* section_data, int section_size, MapChunks* a3)
{
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_TYPE);

bool __stdcall ChkLoader_VER_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 2)
		return 0;

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}
	memcpy(&a3->version, section_data->start_address, section_data->size);
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_VER);

bool __stdcall ChkLoader_DIM_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 4)
	{
		return 0;
	}
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}
	memcpy(&map_size, section_data->start_address, section_data->size);
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_DIM);

bool __stdcall ChkLoader_ERA_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 2)
		return 0;
	if (section_data->start_address + section_data->size > section_data->next_section)
		return 0;

	CurrentTileSet = *(Tileset*)section_data->start_address;
	if (CurrentTileSet > Tileset::Jungle && !IsExpansion)
		return 0;

	if (CurrentTileSet >= _countof(TILESET_NAMES))
	{
		CurrentTileSet = (Tileset) int(CurrentTileSet % 8);
	}

	return 1;
}

FAIL_STUB_PATCH(ChkLoader_ERA);

bool __stdcall ChkLoader_STR_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (MapStringTbl.buffer)
	{
		SMemFree(MapStringTbl.buffer, "Starcraft\\SWAR\\lang\\maphdr.cpp", 356, 0);
	}
	MapStringTbl.buffer = (u16*)SMemAlloc(section_size, "Starcraft\\SWAR\\lang\\maphdr.cpp", 357, 0);
	if (MapStringTbl.buffer == NULL)
	{
		return false;
	}

	chk_string_section_size = section_size;
	AppAddExit_(freeCHKStringHandle);
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return false;
	}

	memcpy(MapStringTbl.buffer, section_data->start_address, section_data->size);
	return true;
}

FAIL_STUB_PATCH(ChkLoader_STR);

bool __stdcall ChkLoader_MBRF_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size % sizeof(BriefingEntry) != 0)
	{
		return false;
	}
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return false;
	}

	for (int i = 0; i < section_size / sizeof(BriefingEntry); i++)
	{
		if (!AddBriefingTrigger((BriefingEntry*) section_data->start_address + i * sizeof(BriefingEntry)))
		{
			break;
		}
	}
	return true;
}

FAIL_STUB_PATCH(ChkLoader_MBRF);

bool __stdcall ChkLoader_SPRP_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 4)
	{
		return false;
	}

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return false;
	}
	memcpy(a3, section_data->start_address, section_data->size);
	return true;
}

FAIL_STUB_PATCH(ChkLoader_SPRP);

bool __stdcall ChkLoader_FORC_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size > 20)
	{
		return false;
	}

	for (int i = 0; i < 8; i++)
	{
		a3->player_force[i] = 0;
	}

	for (int i = 0; i < 4; i++)
	{
		a3->tbl_index_force_name[i] = 0;
		a3->force_flags[i] = (ForceFlags) 0;
	}

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return false;
	}
	memcpy(a3->player_force, section_data->start_address, section_data->size);
	return true;
}

FAIL_STUB_PATCH(ChkLoader_FORC);

bool __stdcall ChkLoader_VCOD_(SectionData *section_data, int section_size, MapChunks* a3)
{
	HRSRC hVCOD = FindResourceA(hInst, (LPCSTR)0xCA, "VCOD");
	if (!hVCOD)
		return 0;

	DWORD VCOD_Size = SizeofResource(hInst, hVCOD);
	if (section_size != VCOD_Size)
		return 0;

	HGLOBAL v6 = LoadResource(hInst, hVCOD);
	if (!v6)
		return 0;

	void *VCOD_Locked = LockResource(v6);
	if (!VCOD_Locked)
		return 0;

	void *hCHKData = SMemAlloc(section_size, "Starcraft\\SWAR\\lang\\maphdr.cpp", 1171, 0);
	if (!hCHKData)
		return 0;

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		SMemFree(hCHKData, "Starcraft\\SWAR\\lang\\maphdr.cpp", 1174, 0);
		return 0;
	}
	memcpy(hCHKData, section_data->start_address, section_data->size);

	int Hash_Chk = CHK_PerformVCODcheck(LobbyPlayers, 0x1B0u, (int) hCHKData, section_size);
	int Hash_VCOD = CHK_PerformVCODcheck(LobbyPlayers, 0x1B0u, (int) VCOD_Locked, section_size);

	SMemFree(hCHKData, "Starcraft\\SWAR\\lang\\maphdr.cpp", 1182, 0);
	return Hash_VCOD == Hash_Chk;
}

FAIL_STUB_PATCH(ChkLoader_VCOD);
FAIL_STUB_PATCH(CopySectionData);

u16 SAI_GetRegionIdFromPx_(__int16 y, __int16 x)
{
	u16 region_id = SAIPathingEx->mapTileRegionId[y / TILE_HEIGHT][x / TILE_WIDTH];
	if (region_id >= 0x2000)
	{
		return SAIPathingEx->splitTiles[region_id - 0x2000].rgn1;
	}
	return region_id;
}

u16 SAI_GetRegionIdFromPx__(__int16 x)
{
	__int16 y;

	__asm mov y, ax

	return SAI_GetRegionIdFromPx_(y, x);
}

FUNCTION_PATCH((void*)0x49C9A0, SAI_GetRegionIdFromPx__);

int __stdcall sub_422FA0_(struct_a1_1* a1, int a2)
{
	u16 v2 = u16(a2 << 6) >> 6;
	u16 x = std::clamp(a1->unk_posintion3.x, SAIPathingEx->regions[v2].rgnBox.left, SAIPathingEx->regions[v2].rgnBox.right) / 32;
	u16 y = std::clamp(a1->unk_posintion3.y, SAIPathingEx->regions[v2].rgnBox.top, SAIPathingEx->regions[v2].rgnBox.bottom) / 32;

	for (int v19 = 1; v19 < 16; v19 += 2)
	{
		int v12 = 0;
		do
		{
			for (int v20 = 0; v20 < v19; v20++)
			{
				if (x < map_size.width && y < map_size.height && SAI_GetRegionIdFromPx_(32 * y, 32 * x) == (_WORD)a2)
				{
					a1->unk_posintion3.x = 32 * x + 16;
					a1->unk_posintion3.y = 32 * y + 16;
					a1->byte1F = 1;
					return 1;
				}
				x += word_519F54[v12];
				y += word_519F5C[v12];
			}

			x -= word_519F54[v12];
			y -= word_519F5C[v12];
			v12++;
		} while (v12 < 4);
		--x;
		--y;
	}

	a1->unk_posintion3.x = SAIPathingEx->regions[v2].rgnCenterX >> 8;
	a1->unk_posintion3.y = SAIPathingEx->regions[v2].rgnCenterY >> 8;
	a1->byte1F = 1;

	return 1;
}

FUNCTION_PATCH(sub_422FA0, sub_422FA0_);

u16 GetRegionIdAtPosEx_(int y, int x)
{
	u16 region_id = SAIPathingEx->mapTileRegionId[y / TILE_HEIGHT][x / TILE_WIDTH];
	if (region_id >= 0x2000)
	{
		if ((1 << (((x / 8) & 3) + 4 * ((y / 8) & 3))) & SAIPathingEx->splitTiles[region_id - 0x2000].minitileMask)
		{
			return SAIPathingEx->splitTiles[region_id - 0x2000].rgn2;
		}
		else
		{
			return SAIPathingEx->splitTiles[region_id - 0x2000].rgn1;
		}
	}
	return region_id;
}

u16 GetRegionIdAtPosEx__()
{
	int x, y;

	__asm
	{
		mov x, edi
		mov y, ecx
	}

	return GetRegionIdAtPosEx_(y, x);
}

FUNCTION_PATCH((void*)0x49C9F0, GetRegionIdAtPosEx__);

int sub_422A90_(struct_a1_1* a1, Position* a2)
{
	int v18 = 1;
	while (1)
	{
		int v19 = 0;
		do
		{
			int v16 = 0;
			while (1)
			{
				u16 v5 = a2->x + a1->word158.x;
				u16 v6 = a2->y + a1->word158.y;
				u16 v7 = a2->y + a1->word15C.y;
				u16 v8 = a2->x + a1->word15C.x;
				if (v5 < (u16)map_width_pixels && v6 < (u16)map_height_pixels && v8 < (u16)map_width_pixels && v7 < (u16)map_height_pixels)
				{
					int v9 = 1;
					int v10 = v7 / 32 - v6 / 32;
					int i = 0;
					while (v10 >= 0)
					{
						for (int v11 = v8 / 32 - v5 / 32; v11 >= 0; --v11)
						{
							u16 region_id = SAIPathingEx->mapTileRegionId[v6 / 32 + i][v5 / 32 + v11];
							if (region_id < 0x2000 && SAIPathingEx->regions[region_id].accessabilityFlags == SAF_Inaccessible)
							{
								v9 = 0;
								break;
							}
						}
						--v10;
						i++;
						if (v9 == 0)
						{
							goto LABEL_19;
						}
					}
					return 1;
				}
			LABEL_19:
				++v16;
				a2->x += 8 * word_519F54[v19];
				a2->y += 8 * word_519F5C[v19];
				if (v16 >= v18)
				{
					break;
				}
			}

			a2->x += -8 * word_519F54[v19];
			a2->y += -8 * word_519F5C[v19];
			v19++;
		} while (v19 < 4);

		a2->x -= 8;
		a2->y -= 8;
		v18 += 2;
		if (v18 >= 10)
		{
			return 0;
		}
	}
}

int __stdcall sub_422A90__(Position* a2)
{
	struct_a1_1* a1;

	__asm mov a1, eax

	return sub_422A90_(a1, a2);
}

FUNCTION_PATCH((void*)0x422A90, sub_422A90__);

void SAI_PathCreate_Sub3_0_1_(__int16 a1, rect* a2, SAI_PathsEx* a3)
{
	for (int v8 = a2->top; v8 < a2->bottom; v8++)
	{
		for (int v6 = a2->left; v6 < a2->right; v6++)
		{
			a3->mapTileRegionId[v8][v6] = a1;
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_0_1);

void SAI_PathCreate_Sub3_0_2_(int a2, SAI_PathsEx* a3, rect* a4, SaiAccessabilityFlags a5)
{
	for (int y = a4->top; y < a4->bottom; ++y)
	{
		for (int x = a4->left; x < a4->right; x++)
		{
			if (a3->mapTileRegionId[y][x] == a5)
			{
				if (y > 0 && a2 == a3->mapTileRegionId[y - 1][x] ||
					x > 0 && a2 == a3->mapTileRegionId[y][x - 1] ||
					x + 1 < map_size.width && a2 == a3->mapTileRegionId[y][x + 1] ||
					y + 1 < map_size.height && a2 == a3->mapTileRegionId[y + 1][x])
				{
					a3->mapTileRegionId[y][x] = a2;
				}
			}
		}
	}

	for (int y = a4->top; y < a4->bottom; ++y)
	{
		for (int x = a4->left; x < a4->right; x++)
		{
			if (a3->mapTileRegionId[y][x] == a5)
			{
				if (y > 0 && a2 == a3->mapTileRegionId[y - 1][x] ||
					x > 0 && a2 == a3->mapTileRegionId[y][x - 1] ||
					x + 1 < map_size.width && a2 == a3->mapTileRegionId[y][x + 1] ||
					y + 1 < map_size.height && a2 == a3->mapTileRegionId[y + 1][x])
				{
					a3->mapTileRegionId[y][x] = a2;
				}
			}
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_0_2);

int SAI_PathCreate_Sub3_0_(SAI_PathsEx* a1, Position a2, MapSize size)
{
	int x = a2.x;
	int y = a2.y;
	int v23 = a1->regionCount;
	dword_6D5BF8 = 0;
	int v24 = 0;
	SaiAccessabilityFlags a5 = (SaiAccessabilityFlags) 0;
	int v28 = 56;
	while (1)
	{
		rect v19 = { 0, 0, 0, 0 };
		int v29 = 0;
		int v30 = 0;

		if (x >= (__int16)size.width)
		{
			x = (__int16)a2.x;
			y += 1;
			if (y >= (__int16)size.height)
			{
				y = (__int16)a2.y;
			}
		}

		u16* v20 = &a1->mapTileRegionId[y][x];
		while (1)
		{
			if (a1->mapTileRegionId[y][x] >= 5000)
			{
				rect a4;
				SAI_PathCreate_Sub3_0_0(y, x, (SAI_Paths*)a1, &a4, a1->mapTileRegionId[y][x]);
				++v29;
				if (v30 < (a4.right - a4.left) * (a4.bottom - a4.top))
				{
					v30 = (a4.right - a4.left) * (a4.bottom - a4.top);
					v19 = a4;
					a5 = (SaiAccessabilityFlags)a1->mapTileRegionId[y][x];
				}
				if (v30 >= v28 || v29 > 25)
				{
					x = v19.right;
					y = v19.top;
					break;
				}
				x = a4.right;
			}
			else
			{
				x += 1;
			}

			if (x >= size.width)
			{
				x = (__int16)a2.x;
				++y;
				if (y >= (__int16)size.height)
				{
					y = (__int16)a2.y;
				}
			}
			if (&a1->mapTileRegionId[y][x] == v20)
			{
				break;
			}
		}

		if (a1->regionCount >= 5000)
		{
			dword_6D5BF8 = 1;
			if (dword_6CA54C)
			{
				SMemFree(dword_6CA54C, "Starcraft\\SWAR\\lang\\sai_PathCreate.cpp", 882, 0);
				dword_6CA54C = 0;
			}
			if (!outOfGame)
			{
				leaveGame(3);
				outOfGame = 1;
				doNetTBLError_(
					0,
					"The map could not be loaded because it had too many obstructions. "
					"Try widening corridors and reducing the number of small nooks and crannies to correct the problem.\n\n",
					0,
					97);
				if (gwGameMode == GAME_RUN)
				{
					GameState = 0;
					gwNextGameMode = GAME_GLUES;
					if (!InReplay)
					{
						replay_header.ReplayFrames = ElapsedTimeFrames;
					}
				}
				if (glGluesMode == GAME_GLUES)
				{
					GetWindowThreadProcessId(hWndParent, (LPDWORD)&size);
					EnumWindows(EnumFunc, *(_DWORD*)&size);
				}
				glGluesMode = GLUE_GENERIC;
			}
			return 0;
		}

		v28 = (v19.right - v19.left) * (v19.bottom - v19.top);
		if (v28 == 0)
		{
			return 1;
		}

		SAI_PathCreate_Sub3_0_1_(a1->regionCount, &v19, a1);
		a1->regions[a1->regionCount].rgnCenterX = (v19.left + v19.right) / 2;
		a1->regions[a1->regionCount].rgnCenterY = (v19.top + v19.bottom) / 2;
		if (v19.left > 0)
		{
			--v19.left;
		}
		if (v19.right < (int)map_size.width)
		{
			++v19.right;
		}
		if (v19.top > 0)
		{
			--v19.top;
		}
		if (v19.bottom < (int)map_size.height)
		{
			++v19.bottom;
		}

		SAI_PathCreate_Sub3_0_2_(a1->regionCount, a1, &v19, a5);
		a1->regions[a1->regionCount].rgnBox.left = v19.left;
		a1->regions[a1->regionCount].rgnBox.top = v19.top;
		a1->regions[a1->regionCount].rgnBox.right = v19.right;
		a1->regions[a1->regionCount].rgnBox.bottom = v19.bottom;
		a1->regions[a1->regionCount].accessabilityFlags = a5;
		a1->regionCount++;
		if (v28 <= 6 && v24 == 0)
		{
			v24 = 1;
			for (int v12 = v23; v12 < a1->regionCount; v12++)
			{
				if ((__int16)a1->regions[v12].rgnBox.left > 0)
				{
					a1->regions[v12].rgnBox.left -= 1;
				}
				if ((__int16)a1->regions[v12].rgnBox.right < map_size.width)
				{
					a1->regions[v12].rgnBox.right += 1;
				}
				if ((__int16)a1->regions[v12].rgnBox.top > 0)
				{
					a1->regions[v12].rgnBox.top -= 1;
				}
				if ((__int16)a1->regions[v12].rgnBox.bottom < map_size.height)
				{
					a1->regions[v12].rgnBox.bottom += 1;
				}
				SAI_PathCreate_Sub3_0_2_(v12, a1, (rect*)&a1->regions[v12].rgnBox, a1->regions[v12].accessabilityFlags);
			}
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_0);

void SAI_PathCreate_Sub3_1_0_(SAI_PathsEx* a1)
{
	for (int i = 0; i < a1->regionCount; i++)
	{
		a1->regions[i].tileCount = 0;
		a1->regions[i].rgnBox.left = 0x7FFF;
		a1->regions[i].rgnBox.top = 0x7FFF;
		a1->regions[i].rgnBox.bottom = -1;
		a1->regions[i].rgnBox.right = -1;
	}

	for (int v15 = 0; v15 < map_size.height; v15++)
	{
		for (int v5 = 0; v5 < map_size.width; v5++)
		{
			WORD v6 = a1->mapTileRegionId[v15][v5];
			if (v6 < 5000u)
			{
				a1->regions[v6].tileCount += 1;

				Box16* box = &a1->regions[v6].rgnBox;
				if ((__int16)box->top > 32 * v15)
				{
					box->top = 32 * v15;
				}
				if ((__int16)box->left > 32 * v5)
				{
					box->left = 32 * v5;
				}
				if ((__int16)box->bottom < 32 * v15 + 32)
				{
					box->bottom = 32 * (v15 + 1);
				}
				if ((__int16)box->right < 32 * v5 + 32)
				{
					box->right = 32 * (v5 + 1);
				}
			}
		}
	}

	for (int i = 0; i < a1->regionCount; ++i)
	{
		if (a1->regions[i].tileCount == 0 && a1->regions[i].accessabilityFlags != (SAF_Inaccessible | SAF_UNK))
		{
			a1->regions[i].accessabilityFlags = SAF_Inaccessible | SAF_UNK;
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_1_0);

void SAI_PathCreate_Sub3_1_1_(SAI_PathsEx* a1)
{
	a1->globalBuffer_ptr = a1->globalBuffer;

	for (int i = 0; i < a1->regionCount; i++)
	{
		SaiRegion* sai_region = &a1->regions[i];
		if (sai_region->tileCount)
		{
			SAI_PathCreate_Sub3_1_1_0((SAI_Paths*)a1, i, sai_region);
			SAI_PathCreate_Sub3_1_1_1((SAI_Paths*)a1, sai_region);
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_1_1);

void SAI_PathCreate_Sub3_1_(int a1, SAI_PathsEx* a2)
{
	a2->globalBuffer_ptr = a2->globalBuffer;
	SAI_PathCreate_Sub3_1_0_(a2);
	SAI_PathCreate_Sub3_1_1_(a2);

	for (int i = a1; i < a2->regionCount; i++)
	{
		if (a2->regions[i].rgnCenterX < 256)
		{
			a2->regions[i].rgnCenterX = (a2->regions[i].rgnCenterX << 13) + 4096;
		}
		if (a2->regions[i].rgnCenterY < 256)
		{
			a2->regions[i].rgnCenterY = (a2->regions[i].rgnCenterY << 13) + 4096;
		}
	}

	SAI_CreateRegionGroupings((SAI_Paths*) a2);
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_1);

void SAI_PathCreate_Sub3_2_(SAI_PathsEx* a1)
{
	int v17 = 0;
	int v24 = 4;
	do
	{
		v24 += 2;
		for (int v20 = a1->regionCount - 1; v20 >= 0; --v20)
		{
			SaiRegion* region = &a1->regions[v20];
			if (region->tileCount && region->groupIndex < 0x4000 && region->tileCount < v24 && region->neighborCount)
			{
				SaiRegion* v6 = 0;
				for (int neighbor_index = 0; neighbor_index < region->neighborCount; neighbor_index++)
				{
					u16 neighbor = region->neighbors[neighbor_index];
					SaiRegion* neighboring_region = &a1->regions[neighbor];
					if (neighboring_region->tileCount && neighboring_region->groupIndex < 0x4000 && region->accessabilityFlags == neighboring_region->accessabilityFlags && (!v6 || neighboring_region->tileCount < v6->tileCount))
					{
						v6 = neighboring_region;
					}
				}

				if (v6)
				{
					for (int v9 = (__int16)region->rgnBox.top / 32; v9 < (__int16)region->rgnBox.bottom / 32; v9++)
					{
						for (int v10 = (__int16)region->rgnBox.left / 32; v10 < (__int16)region->rgnBox.right / 32; v10++)
						{
							if (a1->mapTileRegionId[v9][v10] == v20)
							{
								a1->mapTileRegionId[v9][v10] = v6 - a1->regions;
							}
						}
					}
					v6->tileCount += region->tileCount;
					region->accessabilityFlags = (SaiAccessabilityFlags)0x1FFF;
					region->tileCount = 0;

					if ((__int16)region->rgnBox.top < (__int16)v6->rgnBox.top)
					{
						v6->rgnBox.top = region->rgnBox.top;
					}
					if ((__int16)region->rgnBox.bottom > (__int16)v6->rgnBox.bottom)
					{
						v6->rgnBox.bottom = (__int16)region->rgnBox.bottom;
					}
					if ((__int16)region->rgnBox.left < (__int16)v6->rgnBox.left)
					{
						v6->rgnBox.left = (__int16)region->rgnBox.left;
					}
					if ((__int16)region->rgnBox.right > (__int16)v6->rgnBox.right)
					{
						v6->rgnBox.right = (__int16)region->rgnBox.right;
					}
				}
			}
		}
		for (int i = 0; i < a1->regionCount; i++)
		{
			if (a1->regions[i].tileCount)
			{
				++v17;
			}
		}
	} while (v17 >= 2500);
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_2);

void SAI_PathCreate_Sub3_3_(SAI_PathsEx* a1)
{
	__int16 v16[5000];
	int v18 = 0;
	for (int v3 = 0; v3 < a1->regionCount; v3++)
	{
		v16[v3] = v18;
		if (a1->regions[v3].tileCount != 0)
		{
			++v18;
		}
	}

	int region_with_tiles_index = 0;
	for (int region_index = 0; region_index < a1->regionCount; region_index++)
	{
		SaiRegion* region = &a1->regions[region_index];
		if (region->tileCount)
		{
			for (int v8 = 0; v8 < region->neighborCount; v8++)
			{
				u16* v9 = &region->neighbors[v8];
				*v9 = v16[*v9];
			}
			memcpy(&a1->regions[region_with_tiles_index], region, sizeof(SaiRegion));
			region_with_tiles_index++;
		}
	}

	a1->regionCount = v18;

	for (int v11 = 0; v11 < map_size.height; v11++)
	{
		for (int v15 = 0; v15 < map_size.width; v15++)
		{
			u16* v12 = &a1->mapTileRegionId[v11][v15];
			*v12 = v16[*v12];
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3_3);

int SAI_PathCreate_Sub3_(PathCreateRelated* a1, SAI_PathsEx* a2)
{
	int old_region_count = a2->regionCount;

	if (!SAI_PathCreate_Sub3_0_(a2, a1->position, a1->map_size))
	{
		return 0;
	}

	SAI_PathCreate_Sub3_1_(old_region_count, a2);
	SAI_PathCreate_Sub3_2_(a2);
	SAI_PathCreate_Sub3_3_(a2);
	SAI_PathCreate_Sub3_1_(old_region_count, a2);
	a2->splitTiles_end = a2->splitTiles;
	SAI_PathCreate_Sub3_4();
	memset(a2->regions + a2->regionCount, 0, (_countof(a2->regions) - a2->regionCount) * sizeof(SaiRegion));
	return 1;
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub3);

MEMORY_PATCH((void*)0x46EAA0, sizeof(SAI_PathsEx));

void SAI_PathCreate_Sub1_(MegatileFlags* megatile_flags)
{
	SAI_PathCreate_Sub1_0();

	for (int i = 0; i < map_size.height; i++)
	{
		for (int j = 0; j < map_size.width; j++)
		{
			MegatileFlags flags = megatile_flags[i * map_size.width + j];
			if ((flags & (REAL_CREEP | MORE_THAN_12_WALKABLE)) == 0)
			{
				SAIPathingEx->mapTileRegionId[i][j] = SAF_Inaccessible;
			}
			else if ((flags & (MORE_THAN_12_HIGH_HEIGHT | MORE_THAN_12_MEDIUM_HEIGHT | MORE_THAN_12_WALKABLE)) == 0)
			{
				SAIPathingEx->mapTileRegionId[i][j] = SAF_Inaccessible;
			}
			else if ((flags & (MORE_THAN_12_HIGH_HEIGHT | MORE_THAN_12_MEDIUM_HEIGHT | MORE_THAN_12_WALKABLE)) == (MORE_THAN_12_MEDIUM_HEIGHT | MORE_THAN_12_WALKABLE))
			{
				SAIPathingEx->mapTileRegionId[i][j] = SAF_HighGround;
			}
			else if ((flags & (MORE_THAN_12_HIGH_HEIGHT | MORE_THAN_12_MEDIUM_HEIGHT | MORE_THAN_12_WALKABLE)) == (MORE_THAN_12_HIGH_HEIGHT | MORE_THAN_12_WALKABLE))
			{
				SAIPathingEx->mapTileRegionId[i][j] = 8186;
			}
			else
			{
				SAIPathingEx->mapTileRegionId[i][j] = SAF_LowGround;
			}
		}
	}
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub1);

void CreateUIUnreachableRegion_(SAI_PathsEx* paths)
{
	short original_region_count = paths->regionCount;

	SaiRegion* v2 = &paths->regions[paths->regionCount++];
	v2->accessabilityFlags = SAF_Inaccessible;
	v2->rgnCenterX = (32 * map_size.width / 2 + 16) << 8;
	v2->rgnCenterY = (map_size.height << 13) - 4096;
	v2->rgnBox.left = 0;
	v2->rgnBox.right = 32 * map_size.width;
	v2->rgnBox.top = 32 * map_size.height - 32;
	v2->rgnBox.bottom = 32 * map_size.height;

	for (int v3 = map_size.height - 1; v3 < map_size.height; v3++)
	{
		for (int v6 = 0; v6 < map_size.width; v6++)
		{
			paths->mapTileRegionId[v3][v6] = original_region_count;
		}
	}

	v2->tileCount = map_size.width;
	v2->groupIndex = 0x4000;
}

FAIL_STUB_PATCH(CreateUIUnreachableRegion);

int SAI_ContoursCreate_1_(BYTE(*a1)[4 * MAX_MAP_DIMENTION], int* a2, int* a3)
{
	int x = *a3;
	int y = *a2;

	if (x >= 4 * map_size.width)
	{
		x = 0;
		y = (y + 1) % (4 * map_size.height);
	}
	x = x & ~3;

	BYTE* v11 = &a1[y][x];
	while ((*(_DWORD*)&a1[y][x] & 0x7F7F7F7F) == 0)
	{
		x += 4;
		if (x == 4 * map_size.width)
		{
			x = 0;
			y = (y + 1) % (4 * map_size.height);
		}
		if (&a1[y][x] == v11)
		{
			return 0;
		}
	}

	while ((char)a1[y][x] <= 0)
	{
		++x;
	}
	*a3 = x;
	*a2 = y;
	return 1;
}

FAIL_STUB_PATCH(SAI_ContoursCreate_1);

void SAI_ContoursCreate_(SaiContourHub* a1)
{
	BYTE* v7;
	char v10;
	unsigned __int8 v17;
	SaiContour* v26;
	int v37;
	int v44;
	int v45;
	char v47;

	word_515B58[0] = -4 * MAX_MAP_DIMENTION;
	word_515B58[2] = 4 * MAX_MAP_DIMENTION;
	stru_515B60[0].y = MAX_MAP_DIMENTION - 1;
	stru_515B60[3].x = MAX_MAP_DIMENTION - 1;

	BYTE(*a1a)[4 * MAX_MAP_DIMENTION] = (BYTE(*)[4 * MAX_MAP_DIMENTION])dword_6CA54C;
	a1->contourMax[0] = 5000;
	a1->contourMax[1] = 5000;
	a1->contourMax[2] = 5000;
	a1->contourMax[3] = 5000;
	a1->contours[0] = (SaiContour*)SMemAlloc(sizeof(SaiContour) * a1->contourMax[0], "Starcraft\\SWAR\\lang\\sai_ContoursCreate.cpp", 198, 0);
	a1->contours[1] = (SaiContour*)SMemAlloc(sizeof(SaiContour) * a1->contourMax[1], "Starcraft\\SWAR\\lang\\sai_ContoursCreate.cpp", 199, 0);
	a1->contours[2] = (SaiContour*)SMemAlloc(sizeof(SaiContour) * a1->contourMax[2], "Starcraft\\SWAR\\lang\\sai_ContoursCreate.cpp", 200, 0);
	a1->contours[3] = (SaiContour*)SMemAlloc(sizeof(SaiContour) * a1->contourMax[3], "Starcraft\\SWAR\\lang\\sai_ContoursCreate.cpp", 201, 0);
	memset(a1->contours[0], 0, 8 * a1->contourMax[0]);
	memset(a1->contours[1], 0, 8 * a1->contourMax[1]);
	memset(a1->contours[2], 0, 8 * a1->contourMax[2]);
	memset(a1->contours[3], 0, 8 * a1->contourMax[3]);
	a1->contourCount[0] = 0;
	a1->contourCount[1] = 0;
	a1->contourCount[2] = 0;
	a1->contourCount[3] = 0;

	int a2 = 0;
	int a3 = 0;
	while (SAI_ContoursCreate_1_(a1a, &a2, &a3))
	{
		int v42 = a2;
		int v43 = a3;
		v7 = &a1a[a2][a3];
		char v9 = byte_515AF8[*v7 & 0xF];
		v10 = byte_515B08[*v7 & 0xF];
		if (v9 == 2)
		{
			v42 += 1;
			v43 -= 1;
			v7 = &a1a[v42][v43];
			v9 = byte_515AF8[*v7 & 0xF];
			v10 = byte_515B08[*v7 & 0xF];
		}
		v47 = v10;
		int v15 = 2 * (v9 + 4 * (v10 & 1));
		LOWORD(v45) = word_515B18[v15] + 8 * v43;
		HIWORD(v45) = word_515B1A[v15] + 8 * v42;
		v37 = v45;
		*v7 ^= 1 << v9;
		while (1)
		{
			char v16 = (v9 + 1) & 3;
			int v36 = word_515B58[v16];
			int v35 = (char)stru_515B60[v16].y;
			int v34 = (char)stru_515B60[v16].x;
			v17 = 1 << v9;
			*v7 ^= v17;
			u8 v48 = 1 << v16;
			if ((*v7 & v48) == 0)
			{
				while (v17 & v7[v36])
				{
					v7 += v36;
					v42 += v35;
					v43 += v34;
					*v7 ^= v17;
					if (v48 & *v7)
					{
						goto LABEL_10;
					}
				}
				v48 = 1;
			}
			else
			{
			LABEL_10:
				v48 = 0;
			}
			int v21 = 2 * (v9 + 4 * (v48 & 1));
			LOWORD(v44) = word_515B38[v21] + 8 * v43;
			HIWORD(v44) = word_515B3A[v21] + 8 * v42;
			if (a1->contourCount[0] == a1->contourMax[0] ||
				a1->contourCount[1] == a1->contourMax[1] ||
				a1->contourCount[2] == a1->contourMax[2] ||
				a1->contourCount[3] == a1->contourMax[3])
			{
				SAI_ContoursCreate_2(a1);
				v10 = v47;
			}
			char v23 = v9 ^ (2 * v10) ^ ~(2 * v9) & 3;
			v47 = v9 ^ (2 * (v48 ^ v9 & 1)) ^ 1;
			if (v9 == 0)
			{
				v26 = &a1->contours[0][a1->contourCount[0]++];
				v26->v[0] = HIWORD(v45);
				v26->v[1] = v45;
				v26->v[2] = v44;
				v26->unk_relation = v23 & 3 | v26->unk_relation & 0xC0 | (4 * (v47 & 3 | (4 * (v10 & 1 | (2 * (v48 & 1))))));
			}
			else if (v9 == 1)
			{
				v26 = &a1->contours[1][a1->contourCount[1]++];
				*(_DWORD*)v26->v = v45;
				v26->v[2] = HIWORD(v44);
				v26->unk_relation = v23 & 3 | v26->unk_relation & 0xC0 | (4 * (v47 & 3 | (4 * (v10 & 1 | (2 * (v48 & 1))))));
			}
			else if (v9 == 2)
			{
				v26 = &a1->contours[2][a1->contourCount[2]++];
				v26->v[0] = HIWORD(v45);
				v26->v[1] = v44;
				v26->v[2] = v45;
				v26->unk_relation = v47 & 3 | v26->unk_relation & 0xC0 | (4 * ((v23 & 3) | (4 * (v48 & 1 | (2 * (v10 & 1))))));
			}
			else
			{
				v26 = &a1->contours[3][a1->contourCount[3]++];
				v26->v[0] = v45;
				v26->v[1] = HIWORD(v44);
				v26->v[2] = HIWORD(v45);
				v26->unk_relation = v47 & 3 | v26->unk_relation & 0xC0 | (4 * ((v23 & 3) | (4 * (v48 & 1 | (2 * (v10 & 1))))));
			}

			v26->type = v9;
			if (v48 == 0)
			{
				v9 = v16;
			}
			else if (v48 == 1)
			{
				v43 += byte_515B68[2 * v9];
				v42 += byte_515B69[2 * v9];
				v7 = &a1a[v42][v43];
				v9 = (v9 - 1) & 3;
			}
			else
			{
				FatalError_("write me!");
			}
			v45 = v44;
			v47 = v48;
			if (v44 == v37)
			{
				break;
			}
			v10 = v47;
		}
		*v7 ^= 1 << v9;
	}
	SMemFree(a1a, "Starcraft\\SWAR\\lang\\sai_ContoursCreate.cpp", 437, 0);
	SAI_ContoursRealloc(a1);
	qsort(a1->contours[0], a1->contourCount[0], sizeof(SaiContour), PtFuncCompare);
	qsort(a1->contours[1], a1->contourCount[1], sizeof(SaiContour), PtFuncCompare);
	qsort(a1->contours[2], a1->contourCount[2], sizeof(SaiContour), PtFuncCompare);
	qsort(a1->contours[3], a1->contourCount[3], sizeof(SaiContour), PtFuncCompare);
}

FAIL_STUB_PATCH(SAI_ContoursCreate);

void SAI_PathCreate_Sub4_(SAI_PathsEx* a1)
{
	a1->contours = (SaiContourHub*)SMemAlloc(56, "Starcraft\\SWAR\\lang\\sai_ContoursCreate.cpp", 129, 0);
	memset(a1->contours, 0, sizeof(SaiContourHub));
	a1->contours->a = 1;
	a1->contours->b = -1;
	a1->contours->c = -1;
	a1->contours->d = 1;
	SAI_ContoursCreate_(a1->contours);
}

FAIL_STUB_PATCH(SAI_PathCreate_Sub4);

bool SAI_PathCreate_(MegatileFlags* a1)
{
	AllocateSAI_Paths_();

	SAI_PathCreate_Sub1_(a1);

	PathCreateRelated v5;
	v5.position.x = 0;
	v5.position.y = 0;
	v5.map_size = map_size;
	CreateUIUnreachableRegion_(SAIPathingEx);

	if (!SAI_PathCreate_Sub3_(&v5, SAIPathingEx))
	{
		return false;
	}

	SAI_PathCreate_Sub4_(SAIPathingEx);
	for (int i = 0; i < SAIPathingEx->regionCount; i++)
	{
		SaiRegion* sai_region = SAIPathingEx->regions + i;
		sai_region->defencePriority = SAI_PathCreate_Sub5(sai_region);
	}
	return true;
}

FAIL_STUB_PATCH(SAI_PathCreate);

bool __stdcall ChkLoader_MTXM_(SectionData *section_data, int a2, MapChunks *a3)
{
	if (a2 > MAX_MAP_DIMENTION * MAX_MAP_DIMENTION * sizeof(TileID) || section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}

	memcpy(MapTileArray, section_data->start_address, section_data->size);
	sub_4BCEA0();

	MegatileFlags* lowerLeftCournerTiles = &active_tiles[map_size.width * (map_size.height - 2)];
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			lowerLeftCournerTiles[i * map_size.width + j] &= ~(CLIFF_EDGE | REAL_CREEP | MORE_THAN_12_WALKABLE);
			lowerLeftCournerTiles[i * map_size.width + j] |= ALWAYS_UNBUILDABLE;
		}
	}

	MegatileFlags* lowerRightCournerTiles = &active_tiles[map_size.width * (map_size.height - 1) - 5];
	for (int i = 0; i < 2; ++i)
	{
		for (int j = 0; j < 5; ++j)
		{
			lowerRightCournerTiles[i * map_size.width + j] &= ~(CLIFF_EDGE | REAL_CREEP | MORE_THAN_12_WALKABLE);
			lowerRightCournerTiles[i * map_size.width + j] |= ALWAYS_UNBUILDABLE;
		}
	}

	SetFogMask(CLIFF_EDGE | REAL_CREEP | MORE_THAN_12_WALKABLE, 1, map_size.width, 0, map_size.height - 1);
	AddFogMask(1, map_size.width, ALWAYS_UNBUILDABLE, 0, map_size.height - 1);
	return SAI_PathCreate_(active_tiles) != 0;
}

FAIL_STUB_PATCH(ChkLoader_MTXM);
FAIL_STUB_PATCH(sub_49CC30);

struct Thingy2Entry
{
	union {
		UnitType unit_type;
		u16 sprite_type;
	};
	Position position;
	u8 player_id;
	u8 _unused1;
	u16 _unused2 : 12;
	u16 draw_as_sprite : 1;
	u16 unused3 : 2;
	u16 disabled : 1;
};

bool __stdcall ChkLoader_THG2_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size % 10u)
	{
		return 0;
	}

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}

	Thingy2Entry* entries = (Thingy2Entry*)section_data->start_address;
	for (int i = 0; i < section_size / 10; i++)
	{
		if (entries[i].draw_as_sprite)
		{
			CreateThingy(entries[i].sprite_type, entries[i].position.x, entries[i].position.y, entries[i].player_id);
		}
		else
		{
			UnitType unit_type = entries[i].unit_type;
			if (unit_type == Special_Upper_Level_Door || unit_type == Special_Right_Upper_Level_Door || unit_type == Special_Pit_Door || unit_type == Special_Right_Pit_Door)
			{
				entries[i].player_id = 11;
			}
			if (gameData.got_file_values.victory_conditions == VC_MAP_DEFAULT && gameData.got_file_values.starting_units == SU_MAP_DEFAULT && !gameData.got_file_values.tournament_mode || entries[i].player_id == 11)
			{
				CUnit* unit = CreateUnitAtPos(entries[i].player_id, entries[i].unit_type, entries[i].position.x, entries[i].position.y);
				if (unit && entries[i].disabled)
				{
					Thg2SpecialDIsableUnit(unit);
				}
			}
		}
	}

	return 1;
}

FAIL_STUB_PATCH(ChkLoader_THG2);

bool __stdcall ChkLoader_MASK_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}

	u8* v4 = section_data->start_address;
	for (int i = 0; i < section_size; i++)
	{
		active_tiles[i] &= (MegatileFlags)~0xFFFF;
		active_tiles[i] |= (MegatileFlags)v4[i];
		active_tiles[i] |= (MegatileFlags)(v4[i] << 8);
	}
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_MASK);

int CHK_UNIT_StartLocationSub_(Position* a1, ChunkUnitEntry* a2)
{
	if (a2->unit_type != Special_Start_Location)
	{
		return 0;
	}
	a1[a2->properties.player] = a2->position;

	bool v3 = InReplay ? MoveToTile.x == 0xFFFF : a2->properties.player == g_LocalNationID;
	if (v3)
	{
		MoveToTile.x = max(a2->position.x - SCREEN_WIDTH / 2, 0) / TILE_WIDTH;
		MoveToTile.y = max(a2->position.y - (SCREEN_HEIGHT - INTERFACE_HEIGHT) / 2, 0) / TILE_HEIGHT;
	}
	return 1;
}

FAIL_STUB_PATCH(CHK_UNIT_StartLocationSub);

bool IsCritter(UnitType unit_type)
{
	return unit_type == Critter_Rhynadon
		|| unit_type == Critter_Bengalaas
		|| unit_type == Critter_Ragnasaur
		|| unit_type == Critter_Scantid
		|| unit_type == Critter_Kakaru
		|| unit_type == Critter_Ursadon;
}

bool IsResource(UnitType unit_type)
{
	return unit_type == Resource_Mineral_Field || unit_type == Resource_Mineral_Field_Type_2 || unit_type == Resource_Mineral_Field_Type_3 || unit_type == Resource_Vespene_Geyser;
}

bool unitIsNeutral(ChunkUnitEntry* unit_entry)
{
	if (gameData.got_file_values.starting_units == StartingUnits::SU_MAP_DEFAULT)
	{
		return true;
	}
	else
	{
		return unit_entry->properties.player == 11 && (IsResource(unit_entry->unit_type) || IsCritter(unit_entry->unit_type));
	}
}

FAIL_STUB_PATCH(unitNotNeutral);

bool __stdcall ChkLoader_UNIT_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size % sizeof(ChunkUnitEntry))
	{
		return 0;
	}
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}

	memset(startPositions, 0, 8 * sizeof(Position));

	UnitRelated20* v18 = NULL;
	UnitRelated20* v19 = NULL;
	ChunkUnitEntry* unit_entries = (ChunkUnitEntry*)section_data->start_address;
	int unit_count = section_size / sizeof(ChunkUnitEntry);
	for (int i = 0; i < unit_count; i++)
	{
		ChunkUnitEntry* unit_entry = unit_entries + i;

		if (!CHK_UNIT_StartLocationSub_(startPositions, unit_entry)
			&& (unit_entry->properties.player >= 8u || Players[unit_entry->properties.player].nType != PT_NotUsed && (Players[unit_entry->properties.player].nType <= PT_Unknown0 || Players[unit_entry->properties.player].nType == PT_Neutral))
			&& unitIsNeutral(unit_entry)
			&& (gameData.got_file_values.victory_conditions
				|| gameData.got_file_values.starting_units
				|| gameData.got_file_values.tournament_mode
				|| !getPlayerForce(unit_entry->properties.player)
				|| (Unit_GroupFlags[unit_entry->unit_type] & 0x80u) != 0))
		{
			CUnit* v11 = sub_4CD740(unit_entry);
			if (v11)
			{
				v19 = CHK_UNIT_Nydus(unit_entry->linked_unit_id, v11, v19, unit_entry->id);
				v18 = CHK_UNIT_Addon(v11, v18, unit_entry->linked_unit_id, unit_entry->id);
			}
		}
	}

	for (; v19; v19 = sub_4CBD30(v19));
	for (; v18; v18 = sub_4CCF90(v18));
	for (CUnit* i = UnitNodeList_VisibleUnit_First; i; i = i->next)
	{
		UpdateUnitSpriteInfo_(i);
	}
	CanUpdatePoweredStatus = 1;

	return 1;
}

FAIL_STUB_PATCH(ChkLoader_UNIT);
FAIL_STUB_PATCH(sub_4CD090);

bool __stdcall ChkLoader_UPRP_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 1280)
	{
		return 0;
	}
	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}
	memcpy(stru_596CD8, section_data->start_address, section_data->size);
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_UPRP);

bool __stdcall ChkLoader_COLR_(SectionData* section_data, int section_size, MapChunks* a3)
{
	if (section_size != 8)
	{
		return 0;
	}

	if (section_data->start_address + section_data->size > section_data->next_section)
	{
		return 0;
	}

	setPlayerColours(8, (char*) section_data->start_address);
	return 1;
}

FAIL_STUB_PATCH(ChkLoader_COLR);

void __fastcall FreeCreepData_(bool exit_code)
{
	if (location)
	{
		SMemFree(location, "Starcraft\\SWAR\\MapComn\\creep.cpp", 368, 0);
	}
	if (CreepEdgeData)
	{
		SMemFree(CreepEdgeData, "Starcraft\\SWAR\\MapComn\\creep.cpp", 369, 0);
	}
	if (TerrainGraphics)
	{
		SMemFree(TerrainGraphics, "Starcraft\\SWAR\\MapComn\\creep.cpp", 370, 0);
	}
}

FAIL_STUB_PATCH(FreeCreepData);

void InitTerrainGraphicsAndCreep_(struct_a1* a1, TileID* a2, int a3, int a4, void* a5)
{
	dword_6D0C74 = a1->isCreepCovered;
	dword_6D0C70 = a1->pfunc0;
	dword_6D0C7C = a1->pfuncC;
	dword_6D0E84 = a2;
	dword_6D0F08 = a3;

	int v8 = a4;
	dword_6D0C78 = a1->isTileVisible;
	dword_6D0C6C = a4;
	if (location)
	{
		SMemFree(location, "Starcraft\\SWAR\\MapComn\\creep.cpp", 419, 0);
		v8 = dword_6D0C6C;
	}
	location = (TileID*)SMemAlloc(2 * dword_6D0F08 * v8, "Starcraft\\SWAR\\MapComn\\creep.cpp", 420, 8);
	if (CreepEdgeData)
	{
		SMemFree(CreepEdgeData, "Starcraft\\SWAR\\MapComn\\creep.cpp", 423, 0);
	}
	CreepEdgeData = (u8*)SMemAlloc(dword_6D0F08 * dword_6D0C6C, "Starcraft\\SWAR\\MapComn\\creep.cpp", 424, 8);
	if (TerrainGraphics)
	{
		SMemFree(TerrainGraphics, "Starcraft\\SWAR\\MapComn\\creep.cpp", 427, 0);
	}
	TerrainGraphics = readCreepFile((char*)a5);
	if (!dword_6D6414)
	{
		dword_6D6414 = 1;
		sub_413C50();
		sub_413C00();
		AppAddExit_(FreeCreepData_);
	}
}

FAIL_STUB_PATCH(InitTerrainGraphicsAndCreep);

void loadParallaxStarGfx_(const char* parallaxFile)
{
	char parallaxFilePath[MAX_PATH];
	sprintf_s(parallaxFilePath, "parallax\\%s.spk", parallaxFile);

	parallaxSomethingWidth = 165888;
	parallaxSomethingHeight = 124928;

	HANDLE hFile;
	if (!SFileOpenFileEx(0, parallaxFilePath, 0, &hFile))
	{
		throw FileNotFoundException(parallaxFilePath, SErrGetLastError());
		throw std::exception("Could not find SPK file");
	}
	LONG fileSize = SFileGetFileSize(hFile, 0);
	if (fileSize == -1)
	{
		FileFatal(hFile, GetLastError());
		return;
	}
	if (!fileSize)
	{
		throw FileNotFoundException(parallaxFilePath, 24);
	}
	void* spkData = SMemAlloc(fileSize, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210, 0);
	int read;
	if (SFileReadFile(hFile, spkData, fileSize, &read, 0))
	{
		if (read != fileSize)
		{
			FileFatal(hFile, 24);
			return;
		}
		SFileCloseFile(hFile);
		int numberOfLayers = *(unsigned __int16*)spkData;
		int v8 = 0;
		spkHandle = spkData;
		if (numberOfLayers)
		{
			u16* v10 = (u16*)spkData;
			for (int layerIndex = 0; layerIndex < numberOfLayers; ++layerIndex) {
				++v10;
				spkLayer[layerIndex] = *v10;
				v8 += *v10;
			}
		}
		int v12 = (int)spkData + 2 * numberOfLayers + 2;
		if (v8 > 0)
		{
			_DWORD* v13 = (_DWORD*)(v12 + 4);
			do
			{
				*v13 += (int)spkData;
				v13 += 2;
				--v8;
			} while (v8);
		}
		dword_658AAC = v12 + 8 * spkLayer[0];
		dword_658AA8 = v12;
		dword_658AB0 = dword_658AAC + 8 * spkLayer[1];
		dword_658AB4 = dword_658AB0 + 8 * spkLayer[2];
		dword_658AB8 = dword_658AB0 + 8 * spkLayer[2] + 8 * spkLayer[3];
	}
	else
	{
		DWORD last_error = GetLastError();
		FileFatal(hFile, last_error == 38 ? 24 : last_error);
	}
}

FAIL_STUB_PATCH(loadParallaxStarGfx);

void GenerateMegatileDefaultFlags_()
{
	megatile_default_flags = (MegatileFlags*)SMemAlloc(4 * megatileCount, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 195, 8);

	for (int megatile_index = 0; megatile_index < megatileCount; megatile_index++)
	{
		int v1 = 0;
		unsigned v2 = 0;
		int mid_elevation_minitiles = 0;
		int high_elevation_minitiles = 0;
		MiniTileFlagArray minitile_flags = MiniTileFlags->tile[megatile_index];
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				u16 v9 = minitile_flags.miniTile[4 * i + j];
				if (v9 & 1)
				{
					++v2;
				}
				if (v9 & 4)
				{
					++high_elevation_minitiles;
				}
				if (v9 & 2)
				{
					++mid_elevation_minitiles;
				}
				if (v9 & 8)
				{
					++v1;
				}
			}
		}

		MegatileFlags v18 = (MegatileFlags)0;
		if (v2 > 0xC)
		{
			v18 |= MORE_THAN_12_WALKABLE;
		}
		else
		{
			v18 |= LESS_THAN_13_WALKABLE;
		}
		if (v2 && v2 < 0x10)
		{
			v18 |= CLIFF_EDGE;
		}
		if (high_elevation_minitiles >= 12)
		{
			v18 |= MORE_THAN_12_HIGH_HEIGHT;
		}
		else if (mid_elevation_minitiles + high_elevation_minitiles >= 12)
		{
			v18 |= MORE_THAN_12_MEDIUM_HEIGHT;
		}
		if (v1)
		{
			v18 |= HAS_RAMP;
		}
		megatile_default_flags[megatile_index] = v18;
	}
}

FAIL_STUB_PATCH(GenerateMegatileDefaultFlags);

void sub_4BDDD0_(const char* tileset_name)
{
	char buff[MAX_PATH];

	for (int i = 0; i < _countof(byte_50CDC1); ++i)
	{
		byte_50CDC1[i] = i;
	}
	sprintf_s(buff, "Tileset\\%s\\dark.pcx", tileset_name);
	if (!SBmpLoadImage(buff, 0, byte_5973A0, 0x2000, 0, 0, 0))
	{
		throw FileNotFoundException(buff, SErrGetLastError());
	}
	for (int i = 0; i < _countof(byte_5992A0); ++i)
	{
		byte_5992A0[i] = i;
	}
}

FAIL_STUB_PATCH(sub_4BDDD0);

void sub_480960_()
{
	for (int i = 0; i < MAX_MAP_DIMENTION * MAX_MAP_DIMENTION; i++)
	{
		active_tiles[i] |= EXPLORED_ALL | VISIBLE_ALL;
	}
	sub_480430();
	InitializeSightValues(12, line_of_sight, map_size.width);
	byte_657A9C = 31;
	dword_6D5C0C = SMemAlloc(408, "Starcraft\\SWAR\\lang\\mask.cpp", 254, 0);
	memset(dword_6D5C0C, 0, 408);
	dword_6D5C10 = SMemAlloc(408, "Starcraft\\SWAR\\lang\\mask.cpp", 257, 0);
	memset(dword_6D5C10, 0, 408);
	dword_6D5C14 = SMemAlloc(408, "Starcraft\\SWAR\\lang\\mask.cpp", 260, 0);
	memset(dword_6D5C14, 0, 408);
	dword_6D5C18 = SMemAlloc(5280, "Starcraft\\SWAR\\lang\\mask.cpp", 266, 0);
	memset(dword_6D5C18, 0, 5280);
}

FAIL_STUB_PATCH(sub_480960);

void initMapData_()
{
	char filename[MAX_PATH];
	int bytes_read;

	word_6556FC = CursorType::CUR_ARROW;
	byte_66FF5C = 0;
	MapTileArray = (TileID *)SMemAlloc(MAX_MAP_DIMENTION * MAX_MAP_DIMENTION * sizeof(TileID), "Starcraft\\SWAR\\lang\\Gamemap.cpp", 603, 0);
	CellMap = (__int16*)SMemAlloc(MAX_MAP_DIMENTION * MAX_MAP_DIMENTION * sizeof(*CellMap), "Starcraft\\SWAR\\lang\\Gamemap.cpp", 604, 0);
	GameTerrainCache = (byte *)SMemAlloc(TILE_CACHE_SIZE, "Starcraft\\SWAR\\lang\\Gamemap.cpp", 605, 0);
	active_tiles = (MegatileFlags*)SMemAlloc(MAX_MAP_DIMENTION * MAX_MAP_DIMENTION * sizeof(*active_tiles), "Starcraft\\SWAR\\lang\\Gamemap.cpp", 606, 0);
	memset(active_tiles, 0, MAX_MAP_DIMENTION * MAX_MAP_DIMENTION * sizeof(*active_tiles));
	dword_6D5CD8 = SMemAlloc(29241, "Starcraft\\SWAR\\lang\\repulse.cpp", 323, 8);

	sprintf_s(filename, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".wpe");
	fastFileRead_(0, 0, filename, (int)palette, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	sprintf_s(filename, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".vf4");
	MiniTileFlags = (MiniTileMaps_type *)fastFileRead_(&bytes_read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	megatileCount = bytes_read / sizeof(MiniTileFlagArray);

	GenerateMegatileDefaultFlags_();
	sprintf_s(filename, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".cv5");
	TileSetMap = (TileType *)fastFileRead_(&bytes_read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	TileSetMapSize = bytes_read / sizeof(TileType);

	sprintf_s(filename, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".grp");

	struct_a1 a1;
	a1.pfunc0 = sub_47E2D0;
	a1.isCreepCovered = isCreepCovered;
	a1.isTileVisible = isTileVisible;
	a1.pfuncC = 0;
	InitTerrainGraphicsAndCreep_(&a1, MapTileArray, map_size.width, map_size.height, filename);
	ZergCreepArray = location;

	sprintf_s(filename, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".vx4");
	VX4Data = (vx4entry *)fastFileRead_(&bytes_read, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	sprintf_s(filename, "%s%s%s", "Tileset\\", TILESET_NAMES[CurrentTileSet], ".vr4");
	VR4Data = (vr4entry*) fastFileRead_(0, 0, filename, 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (!dword_5993AC)
	{
		memcpy(stru_6CEB40, palette, sizeof(PALETTEENTRY[256]));
		sub_4BCD70(palette);
		sub_4BDD60();
	}
	loadColorShiftTilesetImages(TILESET_NAMES[CurrentTileSet]);
	sub_4BDDD0_(TILESET_NAMES[CurrentTileSet]);
	if (!dword_5993AC)
	{
		ScreenLayers[5].buffers = 1;
		sub_480960_();
		InitializeGameLayer_();
	}
	byte_658AC0 = 0;
	dword_658AA4 = 0;
	loadParallaxStarGfx_("star");
	sub_47D660();
}

FAIL_STUB_PATCH(initMapData);

unsigned int GetGroundHeightAtPos_(int x, int y)
{
	int megatile_index = x / TILE_WIDTH + y / TILE_HEIGHT * map_size.width;
	TileID megatile = *(u16*)&ZergCreepArray[megatile_index] ? ZergCreepArray[megatile_index] : MapTileArray[megatile_index];

	u16 v1 = TileSetMap[megatile.group].megaTileRef[megatile.tile];
	u16 v2 = MiniTileFlags->tile[v1].miniTile[4 * ((y >> 3) & 3) + ((x >> 3) & 3)];

	int ground_height = (v2 & 6) >> 1;
	return min(ground_height, 2); // TODO: allow a fourth ground height level
}

int GetGroundHeightAtPos__()
{
	int x, y;

	__asm {
		mov x, ecx
		mov y, eax
	}

	return GetGroundHeightAtPos_(x, y);
}

FUNCTION_PATCH((void*)0x4BD0F0, GetGroundHeightAtPos__);

int isUnitAtHeight_(CUnit* unit, char location_flags)
{
	Position position = unit->sprite->position;
	unsigned ground_height = GetGroundHeightAtPos_(position.x, position.y);

	if (unit->statusFlags & StatusFlags::InAir)
	{
		switch (ground_height)
		{
		case 0: return location_flags & 8;
		case 1: return location_flags & 0x10;
		case 2: return location_flags & 0x20;
		}
	}
	else
	{
		switch (ground_height)
		{
		case 0: return location_flags & 1;
		case 1: return location_flags & 2;
		case 2: return location_flags & 4;
		}
	}
	return 0;
}

int __cdecl isUnitAtHeight__()
{
	CUnit* unit;
	int location_flags;

	__asm {
		mov unit, eax
		mov location_flags, esi
	}

	return isUnitAtHeight_(unit, location_flags);
}

FUNCTION_PATCH((void*)0x45F8D0, isUnitAtHeight__);

int revealSightAtLocation_(int sight_range, MegatileFlags vision_mask, signed int x, signed int y, int reveal_from_air)
{
	if (vision_mask <= 0xFF)
	{
		int(__fastcall * *v14)(int, int, SightStruct*, MegatileFlags*, unsigned int, unsigned int);

		v14 = &off_504524;
		MegatileFlags v15 = vision_mask;
		if (reveal_from_air)
		{
			v14 = &off_504528;
		}
		else
		{
			switch (GetGroundHeightAtPos_(x, y))
			{
			case 0:
				v15 |= MORE_THAN_12_MEDIUM_HEIGHT;
				[[fallthrough]];
			case 1:
				v15 |= MORE_THAN_12_HIGH_HEIGHT;
				[[fallthrough]];
			default:
				v15 |= HAS_RAMP;
			}
		}

		SightStruct* v6 = &line_of_sight[sight_range];
		int v11 = v6->tileSightWidth;
		int v13 = v6->tileSightHeight;
		if (x / TILE_WIDTH - v11 / 2 < 0 || x / TILE_WIDTH + v11 / 2 >= map_size.width ||
			y / TILE_HEIGHT - v13 / 2 < 0 || y / TILE_HEIGHT + v13 / 2 >= map_size.height)
		{
			v14 += 2;
		}
		unsigned v7 = ~vision_mask & ~(vision_mask << 8);
		sight_range = (*v14)(x / TILE_WIDTH, y / TILE_HEIGHT, v6, &active_tiles[x / TILE_WIDTH + (y / TILE_HEIGHT) * map_size.width], v15, v7);
	}
	return sight_range;
}

int __stdcall revealSightAtLocation__(signed int x, signed int y, int reveal_from_air)
{
	int sight_range;
	MegatileFlags vision_mask;

	__asm {
		mov sight_range, eax
		mov vision_mask, ecx
	}

	return revealSightAtLocation_(sight_range, vision_mask, x, y, reveal_from_air);
}

FUNCTION_PATCH((void*)0x4806F0, revealSightAtLocation__);

void sub_4CC990_()
{
	int loader_index;
	int chk_size = 0;
	void* chk_data = read_chk_data(CurrentMapFileName, &loader_index, &chk_size);

	if (chk_data)
	{
		loader_index = 0;
		if (ReadMapChunks_(0, chk_data, &loader_index, chk_size))
		{
			ReadChunkNodes_(chk_loaders_[loader_index].briefing_loader_count, chk_size, chk_loaders_[loader_index].briefing_loaders, chk_data, 0);
		}
		SMemFree(chk_data, "Starcraft\\SWAR\\lang\\maphdr.cpp", 2113, 0);
	}
}

FAIL_STUB_PATCH(sub_4CC990);

void loadDlgGrp_()
{
	for (int i = 0; i < 12; i++)
	{
		if (!stru_50E06C[i].field_110 || !is_spawn)
		{
			char dest[MAX_PATH];
			strcpy_s(dest, stru_50E06C[i].glue_path);
			strcat_s(dest, "\\Dlg.grp");
			dword_51C5C8[i] = DlgGrp_Constructor(388, "Starcraft\\SWAR\\lang\\glues.cpp", dest, LoadGraphic);
		}
	}
}

FAIL_STUB_PATCH(loadDlgGrp);

void init_gluesounds_()
{
	if (!dword_6D6364)
	{
		word_6DC2AC[0] = 165;
		word_6DC2AC[1] = 166;
		word_6DC2AC[2] = 167;
		word_6DC2AC[3] = 168;
		word_6DC2AC[4] = 169;
		word_6DC2AC[5] = 170;
		word_6DC2AC[6] = 171;
		word_6DC2AC[7] = 172;
		word_6DC2AC[8] = 173;
		dword_6D6364 = 1;
	}
	playsound_init_UI_(word_6DC2AC);
	ButtonPressSound = buttonMouseOver;
}

FAIL_STUB_PATCH(init_gluesounds);

int gluLoadBINDlg_(dialog* a1, FnInteract fn_interact)
{
	sub_4195E0();
	a1->lFlags |= DialogFlags::CTRL_LBOX_NORECALC;
	LastControlID = -3;
	InitializeDialog_(a1, fn_interact);
	while (true)
	{
		if (dword_6D5E38)
		{
			dword_6D5E38();
		}
		for (dialog* v3 = DialogList; v3; v3 = v3->pNext)
		{
			if (v3->fields.dlg.pModalFcn)
			{
				v3->fields.dlg.pModalFcn(v3);
			}
		}
		BWFXN_videoLoop_(3);
		BWFXN_RedrawTarget_();

		if (a1 == NULL)
		{
			return LastControlID;
		}

		for (dialog* v6 = DialogList; v6 != a1; v6 = v6->pNext)
		{
			if (v6 == NULL)
			{
				return LastControlID;
			}
		}
	}
}

int __stdcall gluLoadBINDlg__(FnInteract fn_interact)
{
	dialog* dlg;

	__asm mov dlg, eax

	return gluLoadBINDlg_(dlg, fn_interact);
}

FUNCTION_PATCH((void*)0x41A080, gluLoadBINDlg__);

void AnimateVideos_(dialog* result)
{
	for (dialog* i = result->fields.dlg.pFirstChild; i; i = i->pNext)
	{
		if (i->wCtrlType == DialogType::cFLCBTN)
		{
			SetCallbackTimer(72, i, 30, PlayVideoFrame);
		}
	}
}

FAIL_STUB_PATCH(AnimateVideos);

void registerMenuFunctions_(FnInteract* functions, dialog* a2, int functions_size)
{
	AnimateVideos_(a2);
	a2->lFlags |= CTRL_USELOCALGRAPHIC;
	if (functions)
	{
		registerUserDialogAction(a2, functions_size, functions);
	}
	if (dialog_count++ == 0)
	{
		if (!glue_background_palette[0].data || a2 == (dialog*)-12)
		{
			char dest[260];
			strcpy_s(dest, stru_50E06C[stru_4FFAD0[glGluesMode].menu_position].glue_path);
			strcat_s(dest, "\\BackGnd.pcx");
			AllocBackgroundImage(dest, &a2->srcBits, palette, "Starcraft\\SWAR\\lang\\glues.cpp", 1052);
			glue_background_palette[0].wid = a2->srcBits.wid;
			glue_background_palette[0].ht = a2->srcBits.ht;
			glue_background_palette[0].data = a2->srcBits.data;
			memcpy(glue_background_palette + 1, palette, sizeof(palette));
		}
		else
		{
			a2->srcBits.wid = glue_background_palette[0].wid;
			a2->srcBits.ht = glue_background_palette[0].ht;
			a2->srcBits.data = glue_background_palette[0].data;
			memcpy(palette, glue_background_palette + 1, sizeof(palette));
		}
		a2->lFlags |= CTRL_UNKOWN1;
		sub_41E450(sub_4BDB30, palette);
		sub_4178B0(palette, NULL);
		getBGPalInfo(glGluesMode);
		MenuPosition v10 = stru_4FFAD0[glGluesMode].menu_position;
		DlgGrp* v11 = dword_51C5C8[v10];
		memcpy(&stru_6CE000, &dword_51C40C[v10], sizeof(stru_6CE000));
		sub_419290(v11);
		grpHead* v12 = stru_50E06C[v10].grp_head;
		setCursor_(v12);
		SetCallbackTimer(24, a2, 50, sub_4DCEA0);
		dword_6D5E20 = &a2->srcBits;
		dword_51C418 = a2;
		checkLastFileError();
		memset(is_keycode_used, 0, sizeof(is_keycode_used));
	}
	else
	{
		if (dword_51C418)
		{
			dlgEvent v14;
			v14.wNo = EVN_USER;
			v14.dwUser = USER_NEXT;
			v14.wSelection = 0;
			v14.wUnk_0x06 = 0;
			v14.cursor.x = Mouse.x;
			v14.cursor.y = Mouse.y;
			dword_51C418->pfcnInteract(dword_51C418, &v14);
		}
		a2->lFlags |= CTRL_UNKOWN4;
	}
}

void __stdcall registerMenuFunctions__(dialog* a2, int functions_size, int a4)
{
	FnInteract* functions;

	__asm mov functions, eax

	registerMenuFunctions_(functions, a2, functions_size);
}

// TODO: patch the remaining usages:
// gluModemList_CustomCtrlID, gluModemStatus_CustomCtrlID, gluModemEntry_CustomCtrlID
FUNCTION_PATCH((void*) 0x4DD9E0, registerMenuFunctions__);

int CreateCampaignGame_(const CampaignMenuEntryEx& campaign_entry)
{
	MapChunks mapChunks;

	CampaignIndex = (MapData)campaign_entry.next_mission;
	if (ReadCampaignMapData_(&mapChunks))
	{
		GameData v4;
		memset(&v4, 0, sizeof(v4));
		strcpy_s(v4.player_name, playerName);
		strcpy_s(v4.map_name, CurrentMapName);
		v4.game_speed = registry_options.GameSpeed;
		v4.active_human_players = 1;
		v4.max_players = 1;

		GotFileValues* v2 = InitUseMapSettingsTemplate_();

		if (v2)
		{
			memcpy(&v4.got_file_values, v2, sizeof(v4.got_file_values));
			SMemFree(v2, "Starcraft\\SWAR\\lang\\uiSingle.cpp", 270, 0);
			if (sub_4DBE50())
			{
				isHost = 0;
				return CreateGame_(&v4) != 0;
			}
		}
	}

	return 0;
}

FAIL_STUB_PATCH(CreateCampaignGame);

int dword_6D5A48_ex;
const std::vector<CampaignMenuEntryEx>* dword_6D5A4C_ex;

void gluHist_Create_(dialog* dlg)
{
	dialog* mission_list_dlg = getControlFromIndex_(dlg, 6);
	const std::vector<CampaignMenuEntryEx>& mission_entries = *dword_6D5A4C_ex;

	mission_list_dlg->lFlags |= CTRL_LBOX_NORECALC | CTRL_PLAIN;

	for (int i = 0; i < mission_entries.size(); i++)
	{
		if (mission_entries[i].next_mission <= dword_6D5A50 && !mission_entries[i].description.empty())
		{
			u8 list_box_entry = ListBox_AddEntry(mission_entries[i].description.c_str(), mission_list_dlg, 0);
			mission_list_dlg->fields.list.pdwData[list_box_entry] = i;
		}
	}

	if (mission_list_dlg->lFlags & CTRL_LBOX_NORECALC)
	{
		mission_list_dlg->lFlags &= ~CTRL_LBOX_NORECALC;
		List_Update(mission_list_dlg);
	}

	if (mission_list_dlg->fields.list.bStrs)
	{
		dlgEvent evt;
		evt.wNo = EVN_USER;
		evt.dwUser = USER_SELECT;
		*(_DWORD*)&evt.wSelection = 0;
		mission_list_dlg->pfcnInteract(mission_list_dlg, &evt);
		DlgSetSelected_UpdateScrollbar(0, mission_list_dlg);
	}
}

FAIL_STUB_PATCH(gluHist_Create);

void gluHist_Activate_(dialog* dlg)
{
	if (LastControlID == 1)
	{
		dialog * v1 = getControlFromIndex_(dlg, 6);

		if (v1 && v1->fields.list.bStrs && v1->fields.list.bCurrStr != 0xFF)
		{
			dword_6D5A48_ex = v1->fields.list.pdwData[v1->fields.list.bCurrStr];
		}
	}
}

FAIL_STUB_PATCH(gluHist_Activate);

int __fastcall gluHist_Interact_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			gluHist_Create_(dlg);
			break;
		case EventUser::USER_ACTIVATE:
			gluHist_Activate_(dlg);
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

		dialog* v4 = getControlFromIndex_(dlg, 6)->fields.list.pScrlBar;
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

		dialog* v4 = getControlFromIndex_(dlg, 6)->fields.list.pScrlBar;
		v4->pfcnInteract(v4, &event);
		return 1;
	}
	return Popup_Main_(dlg, evt);
}

FAIL_STUB_PATCH(gluHist_Interact);

BOOL sub_4B6530_(Campaign* campaign, unsigned int a2)
{
	unsigned i = 0;
	for (auto& a1 : campaign->entries)
	{
		if (a1.next_mission <= a2 && !a1.description.empty())
		{
			++i;
		}
	}
	return i > 1;
}

FAIL_STUB_PATCH(sub_4B6530);

int loadmenu_GluHist_(int a1, Campaign* campaign)
{
	if (!sub_4B6530_(campaign, a1))
	{
		return 0;
	}

	dword_6D5A48_ex = -1;
	dword_6D5A4C_ex = &campaign->entries;
	dword_6D5A50 = a1;
	dword_6D5A40 = off_51A69C;
	dword_599D98 = 28;
	dword_6D5A44 = (WORD*)fastFileRead_(0, 0, "rez\\gluHist.tbl", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210); // TODO: rename
	AllocBackgroundImage("glue\\campaign\\pHist.pcx", &p_hist_pcx, palette, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 602);

	dword_6D5A3C = LoadDialog("rez\\gluHist.bin");

	gluLoadBINDlg_(dword_6D5A3C, gluHist_Interact_);
	if (dword_6D5A44)
	{
		SMemFree(dword_6D5A44, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 609, 0);
		dword_6D5A44 = NULL;
	}

	return dword_6D5A48_ex;
}

FAIL_STUB_PATCH(loadmenu_GluHist);

int parseCmpgnCheatTypeString_(Campaign* campaign, char* campaign_index, int* a5)
{
	char* campaign_index_ = campaign_index;
	int v5 = strtoul(campaign_index, &campaign_index_, 10) - campaign->first_mission_index;
	if (v5 < 0)
	{
		return 0;
	}

	for (auto v8 = campaign->entries.begin(); v8 != campaign->entries.end(); v8++)
	{
		if (!(v8->entry_type == CampaignMenuEntryType::CINEMATIC || v8->hide || v5--))
		{
			if (campaign_index_ && *campaign_index_)
			{
				int v10 = tolower(*campaign_index_) - 'a';
				while (v10)
				{
					--v10;
					if (v8 == campaign->entries.end())
					{
						return 0;
					}
					++v8;
					if (!v8->hide)
					{
						return 0;
					}
				}
				if (v8 == campaign->entries.end())
				{
					return 0;
				}
			}
			*a5 = v8 - campaign->entries.begin();
			return 1;
		}
	}
	return 0;
}

FAIL_STUB_PATCH(parseCmpgnCheatTypeString);

void ContinueCampaignWithLevelCheat_(Campaign* campaign, int campaign_menu_entry_index)
{
	Ophelia = 1;
	active_campaign = campaign;
	active_campaign_entry_index = campaign_menu_entry_index;
}

FAIL_STUB_PATCH(ContinueCampaignWithLevelCheat);

int campaignTypeCheatStrings_(const char* a2)
{
	if (multiPlayerMode || (GameCheats & CheatFlags::CHEAT_Ophelia) == 0)
	{
		return 0;
	}

	Campaign* relevant_campaign = NULL;
	for (Campaign& campaign : active_campaign_set->campaigns)
	{
		int prefix_length = strlen(campaign.campaign_id);
		if (!SStrCmpI(a2, campaign.campaign_id, prefix_length))
		{
			relevant_campaign = &campaign;
			break;
		}
	}
	if (relevant_campaign == NULL)
	{
		return 0;
	}
	int campaign_index = strtoul(a2 + strlen(relevant_campaign->campaign_id), nullptr, 10) - relevant_campaign->first_mission_index;

	int campaign_menu_entry_index;
	int prefix_length = strlen(relevant_campaign->campaign_id);
	if (parseCmpgnCheatTypeString_(relevant_campaign, (char*)a2 + prefix_length, &campaign_menu_entry_index))
	{
		ContinueCampaignWithLevelCheat_(relevant_campaign, campaign_menu_entry_index);
		if (gwGameMode == GAME_RUN)
		{
			GameState = 0;
			gwNextGameMode = GAME_GLUES;
			if (!InReplay)
			{
				replay_header.ReplayFrames = ElapsedTimeFrames;
				glGluesMode = GLUE_MAIN_MENU;
				return 1;
			}
		}
		else
		{
			gwGameMode = GAME_GLUES;
		}
		glGluesMode = GLUE_MAIN_MENU;
	}
	return 1;
}

FAIL_STUB_PATCH(campaignTypeCheatStrings);

bool LoadCampaignWithCharacter_(Campaign& campaign)
{
	customSingleplayer[0] = 0;
	dword_51CA1C = 0;
	CharacterData character_data;
	if (!LoadCharacterData(&character_data, playerName))
	{
		const char* v1 = GetNetworkTblString_(72);
		if ((_stricmp(playerName, v1) || !verifyCharacterFile(&character_data, playerName)) && !outOfGame)
		{
			doNetTBLError_(0, 0, 0, 88);
		}
	}

	active_campaign = &campaign;
	int* unlocked_mission;
	if (IsExpansion)
	{
		unlocked_mission = &character_data.unlocked_expcampaign_mission[active_campaign->race];
	}
	else
	{
		unlocked_mission = &character_data.unlocked_campaign_mission[active_campaign->race];
	}
	active_campaign_entry_index = loadmenu_GluHist_(*unlocked_mission, active_campaign);
	if (active_campaign_entry_index != -1)
	{
		if (*unlocked_mission < active_campaign->entries[active_campaign_entry_index].next_mission)
		{
			*unlocked_mission = active_campaign->entries[active_campaign_entry_index].next_mission;
			CreateCharacterFile(&character_data);
		}

		switch (active_campaign->entries[active_campaign_entry_index].entry_type)
		{
		case CampaignMenuEntryType::CINEMATIC:
			CampaignIndex = (MapData)active_campaign->entries[active_campaign_entry_index].next_mission;
			next_scenario[0] = 0;
			gwGameMode = GAME_CINEMATIC;
			break;
		default:
			CreateCampaignGame_(active_campaign->entries[active_campaign_entry_index]);
			break;
		}
	}
	return active_campaign_entry_index != -1;
}

FAIL_STUB_PATCH(LoadCampaignWithCharacter);

bool LoadPrecursorCampaign()
{
	customSingleplayer[0] = 0;
	dword_51CA1C = 0;
	CharacterData character_data;
	if (!LoadCharacterData(&character_data, playerName))
	{
		const char* v1 = GetNetworkTblString_(72);
		if ((_stricmp(playerName, v1) || !verifyCharacterFile(&character_data, playerName)) && !outOfGame)
		{
			doNetTBLError_(0, 0, 0, 88);
		}
	}

	active_campaign = &active_campaign_set->campaigns[0];
	int unlocked_mission = 6;
	active_campaign_entry_index = loadmenu_GluHist_(unlocked_mission, active_campaign);
	if (active_campaign_entry_index != -1)
	{
		switch (active_campaign->entries[active_campaign_entry_index].entry_type)
		{
		case CampaignMenuEntryType::CINEMATIC:
			CampaignIndex = (MapData)active_campaign->entries[active_campaign_entry_index].next_mission;
			next_scenario[0] = 0;
			gwGameMode = GAME_CINEMATIC;
			break;
		default:
			CreateCampaignGame_(active_campaign->entries[active_campaign_entry_index]);
			break;
		}
	}
	return active_campaign_entry_index != -1;
}

char __stdcall DLG_SwishOut_(dialog* dlg)
{
	RefreshCursor_0();
	BWFXN_RedrawTarget();
	dword_51C4A8 = 1;
	ListNodeUnknown* v1 = dword_51C4B4;
	if (dword_51C4B4)
	{
		_list_unlink((ListNode*)dword_51C4B4);
		if (v1->next)
		{
			int v4 = (int)v1->unknown;
			int v5 = v4 > 0 ? (char*)&v1->next - (char*)v1->next->next + v4 : ~v4;
			*(_DWORD*)v5 = (_DWORD)v1->next;
			v1->next->next = v1->unknown;
			v1->next = 0;
			v1->unknown = 0;
		}
		SMemFree(v1, "delete", -1, 0);
	}

	int v6 = 0;
	dialog* v7;
	dword_51C4B4 = 0;
	if (timer_related.active_timers_count_maybe > 0)
	{
		do
		{
			v7 = dlg;
			getControlFromIndex_(dlg, timer_related.timers[v6].wIndex)->wUser = 1;
			timer_related.active_timers_maybe[v6++] = { 1, 0 };
		} while (v6 < timer_related.active_timers_count_maybe);
		if (v6 && direct_sound && registry_options.Sfx)
		{
			PlayTransmissionLocation(SFX_glue_swishout, volume[99 * registry_options.Sfx / 100], 0, 0);
		}
	}
	else
	{
		v7 = dlg;
	}

	SetCallbackTimer(7, v7, 20, DLGSwishOutProc);
	for (dialog* i = dword_51C4B0; i; i = i->pNext)
	{
		HideDialog_(i);
	}

	dialog* v11 = v7;
	while (v11)
	{
		v11->lFlags &= ~CTRL_EVENTS;
		v11 = v11->wCtrlType ? v11->pNext : v11->fields.dlg.pFirstChild;
	}
	return 1;
}

FUNCTION_PATCH(DLG_SwishOut, DLG_SwishOut_);

void DlgSwooshin_(dialog* dlg, swishTimer* timers, size_t timers_count, __int16 a4)
{
	memset(&timer_related, 0, 0x90u);
	timer_related.active_timers_count_maybe = timers_count;
	int v6 = 0;
	timer_related.timers = timers;
	dword_51C4AC = (unsigned __int16)a4 / 20;
	dword_51C4A8 = 0;
	__int16 v20 = 0;
	__int16 a1a = 0;
	for (int v6 = 0; v6 < timers_count; v6++)
	{
		dialog* v8 = getControlFromIndex_(dlg, timers[v6].wIndex);
		v8->lFlags |= CTRL_PLAIN;
		__int16 v9 = 0;
		switch (timers[v6].wType)
		{
		case 0u:
			v9 = v8->rct.right + 1;
			v8->rct.left -= v9;
			v8->rct.right = -1;
			break;
		case 1u:
			v9 = v8->rct.bottom + 1;
			v8->rct.top -= v9;
			v8->rct.bottom = -1;
			break;
		case 2u:
			v9 = SCREEN_WIDTH - v8->rct.left;
			v8->rct.right += v9;
			v8->rct.left = SCREEN_WIDTH;
			break;
		case 3u:
			v9 = SCREEN_HEIGHT - v8->rct.top;
			v8->rct.bottom += v9;
			v8->rct.top = SCREEN_HEIGHT;
			break;
		default:
			break;
		}
		word_51C480[v6] = v9;
		v8->wUser = v9;
		a1a = max(a1a, v9);
		if ((__int16)timers[v6].wIndex > v20)
		{
			v20 = timers[v6].wIndex;
			dword_51C4B0 = v8;
		}
		timer_related.active_timers_maybe[v6] = { 0, 1 };
	}

	if (dword_51C4B0)
	{
		dword_51C4B0 = dword_51C4B0->pNext;
		ListNodeUnknown* v10 = (ListNodeUnknown*)SMemAlloc(12, "new", -1, 0);
		if (v10)
		{
			v10->next = 0;
			v10->unknown = 0;
			v10->next = (ListNodeUnknown*)&v10->next;
			v10->previous = 0;
			v10->unknown = (ListNodeUnknown*)~(unsigned int)&v10->next;
			dword_51C4B4 = v10;
		}
		else
		{
			dword_51C4B4 = 0;
		}
	}
	SetCallbackTimer(7, dlg, 20, DLG_SwishInLock);
	for (dialog* a1b = dword_51C4B0; a1b; a1b = a1b->pNext)
	{
		if (a1b->lFlags & DialogFlags::CTRL_VISIBLE)
		{
			ListNodeUnknown* v11 = dword_51C4B4;
			const char* v12 = type_info__szName((type_info*)0x51A368);
			ListNodeUnknown* v13 = (ListNodeUnknown*)SMemAlloc(12, v12, -2, 8);
			ListNodeUnknown* v14 = v13;
			if (v13)
			{
				v13->previous = 0;
				v13->next = 0;
			}
			else
			{
				v13 = (ListNodeUnknown*)&v11->next;
			}
			ListNodeUnknown* v15 = v13->previous;
			if (v13->previous)
			{
				int v16 = (int)v13->next;
				int v17 = v16 > 0 ? (char*)v13 - (char*)v15->next + v16 : ~v16;
				*(_DWORD*)v17 = (_DWORD)v15;
				v13->previous->next = v13->next;
				v13->previous = 0;
				v13->next = 0;
			}
			ListNodeUnknown* v18 = v11->next;
			v13->previous = v18;
			v13->next = v18->next;
			v18->next = v14;
			v11->next = v13;
			v14->unknown = (ListNodeUnknown*)a1b;
			HideDialog_(a1b);
		}
	}
}

FAIL_STUB_PATCH(DlgSwooshin);

void DLG_SwishIn_(dialog* a1)
{
	ScreenLayers[0].bits |= 1;
	ScreenLayers[0].buffers = 1;
	BWFXN_RefreshTarget(ScreenLayers[0].left, ScreenLayers[0].height + ScreenLayers[0].top - 1, ScreenLayers[0].top, ScreenLayers[0].width + ScreenLayers[0].left - 1);
	if (dword_50E064 != stru_4FFAD0[glGluesMode].menu_position)
	{
		memcpy(stru_6CEB40, palette, sizeof(stru_6CEB40));
		if ((a1->lFlags & DialogFlags::CTRL_UPDATE) == 0)
		{
			a1->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog(a1);
		}
		TitlePaletteUpdate_(3);
		dword_50E064 = stru_4FFAD0[glGluesMode].menu_position;
	}
	SetCallbackTimer(46, a1, 100, DLGSwishInProc);
}

FAIL_STUB_PATCH(DLG_SwishIn);

dialog* loadFullMenuDLG_(const char* filename, dialog* buffer, int* read, const char* logfilename, int logline)
{
	return (dialog*) fastFileRead_(read, 0, filename, (int)buffer, 1, logfilename, logline);
}

FAIL_STUB_PATCH(loadFullMenuDLG);

dialog* loadAndInitFullMenuDLG_(const char* filename)
{
	dialog* dlg = loadFullMenuDLG_(filename, 0, 0, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
	if (dlg)
	{
		dlg->lFlags |= DialogFlags::CTRL_ACTIVE;
		AllocInitDialogData(dlg, dlg, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\glues.cpp", 1168);
	}
	return dlg;
}

FAIL_STUB_PATCH(loadAndInitFullMenuDLG);

void changeMenu_()
{
	if (dword_51C4B4)
	{
		HiddenCtrl_Destructor(dword_51C4B4);
		if (dword_51C4B4)
		{
			_Unknown_Destructor(dword_51C4B4, 1);
		}
		dword_51C4B4 = 0;
	}
	if (dword_50E064 != stru_4FFAD0[glGluesMode].menu_position)
	{
		if (glue_background_palette[0].data)
		{
			SMemFree(glue_background_palette[0].data, "Starcraft\\SWAR\\lang\\glues.cpp", 442, 0);
		}
		memset(glue_background_palette, 0, sizeof(glue_background_palette));
		sub_41E9E0(3);
		RefreshCursor_0();
	}
	BWFXN_RedrawTarget_();
	memset(&timer_related, 0, 0x90u);
	dialog_count = 0;
}

FAIL_STUB_PATCH(changeMenu);

int CreateLadderGame_(GameData* a1, int a2)
{
	char gameDataString[128];

	if (is_spawn)
	{
		BigPacketError_(104, 0, 0, 0, true);
	}
	else if (GotFileValues* v3 = sub_4AAC90(a1->game_type_param, a1->game_type_unk, a1->game_type))
	{
		GotFileValues* GameTemplateData = &a1->got_file_values;
		memcpy(&a1->got_file_values, v3, sizeof(a1->got_file_values));
		a1->active_human_players = 1;
		if (strlen(playerName) < 0x19)
		{
			strcpy_s(a1->host_name, playerName);
			if (a1->got_file_values.team_mode && !a1->save_timestamp)
			{
				a1->max_players = 8;
			}
			int a5 = 0;
			switch (a1->got_file_values.tournament_mode)
			{
			case 0:
				a5 = 0;
				break;
			case 1:
				a5 = 1;
				break;
			case 2:
				a5 = 3;
				break;
			case 3:
				a5 = 5;
				break;
			default:
				break;
			}
			if (gameData.got_file_values.template_id >= 0x11u && gameData.got_file_values.template_id <= (GT_Greed | 0x10))
			{
				struct_a4* v5 = sub_4AAC60(a1->game_type_unk, a1->game_type_param, a1->game_type);
				if (v5)
				{
					a5 = v5->dword124;
				}
			}
			if (a1->got_file_values.victory_conditions == VC_ONE_ON_ONE && !a1->save_timestamp)
			{
				if (a1->max_players >= 2)
				{
					a1->max_players = 2;
				}
			}
			if (IsScenarioGame(a1) && !a1->save_timestamp)
			{
				a1->max_players = getNumOpenSlots();
			}
			a1->cdkey_hash = makeStringHash();
			sub_472300(gameDataString, (int)&a1->save_timestamp, 0x80u);
			int v7 = (unsigned __int8)GameTemplateData->template_id | ((GameTemplateData->unused1 | (GameTemplateData->variation_id << 8)) << 8);
			int v8 = sub_4AADA0(GameTemplateData);
			if (SNetCreateLadderGame(a1->player_name, (char*)a2, gameDataString, v7, a5, v8, (char*)GameTemplateData, 32, (unsigned __int8)a1->max_players, playerName, "", &playerid))
			{
				memcpy(&gameData, a1, sizeof(gameData));
				isHost = 1;
				return 1;
			}
		}
	}
	else
	{
		if (!outOfGame)
		{
			doNetTBLError_(0, 0, 0, 93);
		}
	}

	return 0;
}

FAIL_STUB_PATCH(CreateLadderGame);

const char* __stdcall get_Tileset_String(Tileset tileset);

void FullyLoadMapDirEntry_(MapDirEntry* map_dir_entry)
{
	if (!map_dir_entry->fully_loaded)
	{
		map_dir_entry->fully_loaded = 1;
		map_dir_entry->error = 1;
		strcpy_s(map_dir_entry->title, get_GluAll_String(SCENARIO_FILENAME_TOO_LONG));
		int v4 = strlen(map_dir_entry->filename);
		int v5 = strlen(map_dir_entry->full_path);
		if (v4 < 32 && v5 + v4 < 260)
		{
			MapChunks a4;
			memset(&a4, 0, sizeof(a4));
			if (std::filesystem::path(map_dir_entry->full_path).extension() == ".scn")
			{
				if (SFileOpenArchive(map_dir_entry->full_path, 9000, 0, &custom_campaign_mpq))
				{
					if (WORD* table = (WORD*)fastFileRead_(0, 0, "rez\\campaign.tbl", 0, 1, __FILE__, __LINE__))
					{
						strcpy_s(map_dir_entry->title, GetTblString(table, 1));
						strcpy_s(map_dir_entry->description, GetTblString(table, 2));

						SMemFree(table, __FILE__, __LINE__, 0);
					}

					if (strlen(map_dir_entry->title) == 0)
					{
						strcpy_s(map_dir_entry->title, map_dir_entry->filename);
					}

					SFileCloseArchive(custom_campaign_mpq);
					custom_campaign_mpq = nullptr;

					map_dir_entry->error = 0;
				}
			}
			else if (ReadMapData_(map_dir_entry->full_path, &a4, 0))
			{
				map_dir_entry->human_player_slots_maybe = getTotalValidSlotCount();
				map_dir_entry->computer_slots = getComputerSlotCount();
				map_dir_entry->human_player_slots = getOpenSlotCount();
				map_dir_entry->unknown3 = 1;
				map_dir_entry->map_width_tiles = map_size.width;
				map_dir_entry->map_height_tiles = map_size.height;
				map_dir_entry->tileset = CurrentTileSet;
				strcpy_s(map_dir_entry->title, a4.tbl_index_title ? get_chk_String(a4.tbl_index_title) : "");
				if (strlen(map_dir_entry->title) == 0)
				{
					strcpy_s(map_dir_entry->title, map_dir_entry->filename);
				}
				strcpy_s(map_dir_entry->description, a4.tbl_index_description ? get_chk_String(a4.tbl_index_description) : "");
				sprintf_s(map_dir_entry->unknown_x478, get_GluAll_String(NUMBER_OF_PLAYERS), map_dir_entry->human_player_slots_maybe);
				sprintf_s(map_dir_entry->map_dimension_string, get_GluAll_String(MAP_SIZE), map_dir_entry->map_width_tiles, map_dir_entry->map_height_tiles);
				sprintf_s(map_dir_entry->computer_players_string, get_GluAll_String(COMPUTER_SLOTS), map_dir_entry->computer_slots);
				sprintf_s(map_dir_entry->human_players_string, get_GluAll_String(HUMAN_SLOTS), map_dir_entry->human_player_slots);
				strcpy_s(map_dir_entry->tileset_string, get_Tileset_String(map_dir_entry->tileset));
				for (int i = 0; i < _countof(map_dir_entry->player_forces); i++)
				{
					map_dir_entry->player_forces[i] = playerForce[i];
				}
				map_dir_entry->unknown2 = a4.data7;
				map_dir_entry->error = 0;
			}
			else if (LOWORD(a4.version) <= 59)
			{
				strcpy_s(map_dir_entry->description, get_GluAll_String(INVALID_SAVE_GAME));
			}
			else
			{
				map_dir_entry->error = 2;
				strcpy_s(map_dir_entry->description, get_GluAll_String(INVALID_SCENARIO));
			}
		}
		else
		{
			strcpy_s(map_dir_entry->description, get_GluAll_String(SCENARIO_INVALID_OR_CORRUPTED));
		}
	}
}

FAIL_STUB_PATCH(FullyLoadMapDirEntry);

void LoadReplayMapDirEntry_(MapDirEntry* replay)
{
	InReplay = 1;
	if (!replay->fully_loaded)
	{
		replay->fully_loaded = 1;
		replay->error = 1;
		strcpy_s(replay->title, get_GluAll_String(SCENARIO_FILENAME_TOO_LONG));
		int v4 = strlen(replay->filename);
		int a3 = strlen(replay->full_path);
		strcpy_s(replay->title, get_GluAll_String(GluAllTblEntry(SELECTED_SCENARIO_NOT_VALID | 0x80)));
		if (v4 >= 32 || a3 + v4 + 1 > 260)
		{
			strcpy_s(replay->description, get_GluAll_String(SCENARIO_INVALID_OR_CORRUPTED));
			return;
		}
		if (!LoadReplayFile(replay->full_path, &a3))
		{
			unsigned v6 = a3 != 0 ? -3u : 0;
			replay->error = 2;
			strcpy_s(replay->description, get_GluAll_String((GluAllTblEntry)(v6 + 63)));
			return;
		}

		replay->human_player_slots = 0;
		replay->computer_slots = 0;

		PlayerInfo player_info[12];
		copyPlayerStructsToReplayPlayerStructs(player_info, &replay->game_data);
		for (int i = 0; i < _countof(player_info); i++)
		{
			if (player_info[i].nType == PT_Computer)
			{
				replay->computer_slots += 1;
			}
			else if (player_info[i].nType == PT_Human)
			{
				replay->human_player_slots += 1;
			}
		}
		replay->tileset = replay->game_data.tileset;
		MapChunks a4;
		memset(&a4, 0, 12);
		replay->map_width_tiles = replay->game_data.width;
		replay->save_hash = replay->game_data.save_timestamp;
		for (int i = 0; i < _countof(a4.tbl_index_force_name); i++)
		{
			a4.tbl_index_force_name[i] = 0;
		}
		replay->unknown3 = 1;
		replay->human_player_slots_maybe = 8;
		replay->map_height_tiles = replay->game_data.height;
		bool v14 = replay_header.campaign_index == MapData::MD_none;
		for (int i = 0; i < _countof(a4.force_flags); i++)
		{
			a4.force_flags[i] = ForceFlags(0);
		}
		a4.version = 0;
		replay->unknown6 = (unsigned __int16)replay_header.campaign_index;
		a4.data7 = 0;
		const char* source_file = v14 ? replay->full_path : MapdataFilenames_[CampaignIndex];
		if (ReadMapData_(source_file, &a4, !v14))
		{
			sprintf_s(replay->description, get_GluAll_String(UNABLE_TO_LOAD_MAP), replay->game_data.map_name);
			strcat_s(replay->description, "\n\n");
			strcpy_s(replay->name, replay->filename);
			sprintf_s(replay->unknown_x478, get_GluAll_String(NUMBER_OF_PLAYERS), replay->unknown3);
			sprintf_s(replay->map_dimension_string, get_GluAll_String(MAP_SIZE), replay->map_width_tiles, replay->map_height_tiles);
			sprintf_s(replay->computer_players_string, get_GluAll_String(COMPUTER_SLOTS), replay->computer_slots);
			sprintf_s(replay->human_players_string, get_GluAll_String(HUMAN_SLOTS), replay->human_player_slots);
			strcpy_s(replay->tileset_string, get_GluAll_String((GluAllTblEntry)(replay->tileset + 39)));
			replay->error = 0;
		}
		else if (LOWORD(a4.version) <= 59)
		{
			strcpy_s(replay->description, get_GluAll_String(INVALID_SAVE_GAME));
		}
		else
		{
			replay->error = 2;
			strcpy_s(replay->description, get_GluAll_String(INVALID_SCENARIO));
		}
	}
}

FAIL_STUB_PATCH(LoadReplayMapDirEntry);

void sub_4ADB10_()
{
	struct_a2 v17;

	u8 bSliderGraphic = map_listbox->fields.scroll.bSliderSkip ? map_listbox->fields.scroll.bSliderGraphic : -1;
	MapDirEntry* map_entry = (MapDirEntry*)map_listbox->fields.list.pdwData[bSliderGraphic];
	if (GetMapDirEntryInformation(map_entry, &v17))
	{
		dialog* v3 = 0;

		dialog* v19 = getControlFromIndex_(map_listbox, 16);
		dialog* v21 = getControlFromIndex_(map_listbox, 18);
		dialog* game_type_label = getControlFromIndex_(map_listbox, 38);
		dialog* game_type_dropdown = getControlFromIndex_(map_listbox, 17);

		if (v17.fully_loaded && v17.flags & MDEF_REPLAY)
		{
			if (game_type_dropdown)
			{
				if (multiPlayerMode)
				{
					for (int v14 = 0; game_type_dropdown->fields.scroll.bSliderSkip < v14; v14++)
					{
						if (game_type_dropdown->fields.list.pdwData[v14] == (unsigned __int8)v17.game_data.got_file_values.template_id)
						{
							if (v14 != -1 && v14 != game_type_dropdown->fields.scroll.bSliderGraphic)
							{
								setSelectedIndexDirect(v14, game_type_dropdown);
							}
							break;
						}
					}

					DisableControl(game_type_dropdown);
					v3 = v19;
				}
				else
				{
					HideDialog(game_type_dropdown);
					if (game_type_label)
					{
						HideDialog(game_type_label);
					}
				}
			}
			if (v21)
			{
				for (int v16 = 0; v21->fields.scroll.bSliderSkip < v16; ++v16)
				{
					if (v21->fields.list.pdwData[v16] == v17.game_data.got_file_values.variation_id)
					{
						if (v16 != -1 && v16 != v21->fields.scroll.bSliderGraphic)
						{
							setSelectedIndexDirect(v16, v21);
						}
						break;
					}
				}

				DisableControl(v21);
				v3 = v19;
			}
			if (v3)
			{
				dlgEvent v18;
				v18.wNo = EVN_USER;
				v18.dwUser = USER_SELECT;
				*(_DWORD*)&v18.wSelection = (unsigned __int8)v17.game_data.game_speed;
				v3->pfcnInteract(v3, &v18);
				DisableControl(v3);
			}
		}
		else if (std::filesystem::path(map_entry->filename).extension() == ".scn")
		{
			// add a 'campaign' game type and use `v17.game_data.game_type` as condition
			HideDialog_(game_type_label);
			HideDialog_(game_type_dropdown);
		}
		else if (v17.flags & MDEF_DIRECTORY)
		{
			// TODO: hide game type variant, speed, and other irrelevant UI elements
			HideDialog_(game_type_label);
			HideDialog_(game_type_dropdown);
		}
		else
		{
			if (game_type_dropdown)
			{
				showDialog(game_type_dropdown);
				EnableControl(game_type_dropdown);
			}
			if (game_type_label)
			{
				showDialog(game_type_label);
			}
			if (v21)
			{
				EnableControl(v21);
			}
			if (v3)
			{
				EnableControl(v3);
			}
		}
	}
}

FAIL_STUB_PATCH(sub_4ADB10);

void sub_4AE830_(int player_slots)
{
	if (dword_6D5A74)
	{
		dword_6D5A78 = player_slots;
		if (selectedGameType == GT_TopVsBottom)
		{
			sub_4AE790(custom_game_submode);
		}
		sub_4ADB10_();
	}
}

FAIL_STUB_PATCH(sub_4AE830);

void sub_4A79D0_(MapDirEntry* a1)
{
	if (a1->error == 1)
	{
		SStrCopy(a1->unknown, "", sizeof(a1->unknown));
	}
	else if (!(a1->flags & (MDEF_REPLAY | MDEF_SAVEGAME)))
	{
		SStrCopy(a1->unknown, byte_68FC88, sizeof(a1->unknown));
	}
	else if (a1->game_data.got_file_values.template_id)
	{
		char* v10 = sub_4AB0E0(a1->game_data.got_file_values.unused1, a1->game_data.got_file_values.template_id);
		SStrCopy(a1->unknown, v10, sizeof(a1->unknown));
	}
	else
	{
		a1->unknown[0] = 0;
	}

	int flag = (a1->flags & MDEF_REPLAY) || sub_4AAF50(dword_68F710[0], dword_68F6FC, template_id);
	if (dword_6D5A8C)
	{
		dword_6D5A8C->fn2(6, dword_6D5A8C->hwnd, flag);
	}
	if (dword_6D5A90)
	{
		dword_6D5A90->fn2(9, dword_6D5A90->hwnd, flag);
	}

	char* i;
	char* description = a1->description;
	unsigned __int8 v3 = a1->description[0];
	int v4 = 256;
	for (i = a1->description; v3; v3 = *++description)
	{
		if (!v4)
		{
			break;
		}
		if (v3 >= ' ' || v3 == '\n' || v3 == '\r' || v3 == '\t')
		{
			*i++ = v3;
			--v4;
		}
	}

	*i = 0;
	if (dword_6D5A7C)
	{
		dword_6D5A7C->fn1(0, dword_6D5A7C->hwnd, a1->title);
	}
	if (dword_6D5A80)
	{
		dword_6D5A80->fn1(1, dword_6D5A80->hwnd, a1->description);
	}
	if (dword_6D5A84)
	{
		dword_6D5A84->fn1(3, dword_6D5A84->hwnd, a1->unknown);
	}
	if (dword_6D5A88)
	{
		dword_6D5A88->fn1(4, dword_6D5A88->hwnd, a1->unknown_x478);
	}
	if (dword_6D5A8C)
	{
		dword_6D5A8C->fn1(6, dword_6D5A8C->hwnd, a1->computer_players_string);
	}
	if (dword_6D5A94)
	{
		dword_6D5A94->fn1(7, dword_6D5A94->hwnd, a1->human_players_string);
	}
	if (dword_6D5A98)
	{
		dword_6D5A98->fn1(5, dword_6D5A98->hwnd, a1->map_dimension_string);
	}
	if (dword_6D5A9C)
	{
		dword_6D5A9C->fn1(2, dword_6D5A9C->hwnd, a1->unknown7);
	}
	if (dword_6D5AA0)
	{
		dword_6D5AA0->fn1(8, dword_6D5AA0->hwnd, a1->tileset_string);
	}

	memcpy_s(playerForce, sizeof(playerForce), a1->player_forces, sizeof(a1->player_forces));
	if (!InReplay)
	{
		memcpy_s(replay_header.playerForce, sizeof(replay_header.playerForce), a1->player_forces, sizeof(a1->player_forces));
	}
	if (dword_6D5D68 && !dword_6D5D64)
	{
		sub_44BA90(a1->human_player_slots_maybe);
	}

	sub_4AE830_(InReplay ? replay_header.game_data.max_players : a1->human_player_slots_maybe);
}

void __cdecl sub_4A79D0__()
{
	MapDirEntry* a1;

	__asm mov a1, esi

	return sub_4A79D0_(a1);
}

FUNCTION_PATCH((void*)0x4A79D0, sub_4A79D0__);

void __stdcall sub_4A7F50_(HWND hWnd, UINT a2, UINT uIDEvent, DWORD a4)
{
	if (hWnd)
	{
		KillTimer(hWnd, uIDEvent);
	}

	InReplay = 0;
	if (replay->flags & MapDirEntryFlags::MDEF_SCENARIO)
	{
		FullyLoadMapDirEntry_(replay);
	}
	else if (replay->flags & MapDirEntryFlags::MDEF_SAVEGAME)
	{
		sub_4A7540(replay);
	}
	else if (replay->flags & MapDirEntryFlags::MDEF_REPLAY)
	{
		LoadReplayMapDirEntry_(replay);
	}

	sub_4A79D0_(replay);
	replay = 0;
}

FUNCTION_PATCH(sub_4A7F50, sub_4A7F50_);

unsigned sub_4A8050_(MapDirEntry* a1, char* source, int a3, unsigned int a4, unsigned __int8 a5, char* dest)
{
	KillTimer(hWndParent, 0xCu);
	if (!source && !dest)
	{
		return 0x80000006;
	}

	MapDirEntry* v8 = (MapDirEntry*)((int)dword_51A27C <= 0 ? 0 : (unsigned int)dword_51A27C);
	if ((int)v8 <= 0)
	{
		return 0x80000006;
	}
	while (v8 != a1)
	{
		v8 = v8->next;
		if ((int)v8 <= 0)
		{
			return 0x80000006;
		}
	}

	InReplay = 0;
	switch (v8->flags & 0x8F)
	{
	case MapDirEntryFlags::MDEF_SAVEGAME:
		if (!v8->fully_loaded)
		{
			sub_4A7540(v8);
		}
		if (v8->error == 1)
		{
			return 0x80000007;
		}
		if (v8->error == 2)
		{
			return 0x80000001;
		}
		if (!source)
		{
			SStrCopy(dest, v8->full_path, MAX_PATH);
			return 0;
		}
		if (a3 && !*(_BYTE*)a3)
		{
			return 0x80000009;
		}
		GameData game_data;
		memcpy(&game_data, &v8->game_data, sizeof(game_data));
		strcpy_s(game_data.player_name, source);
		if (CreateLadderGame_(&game_data, a3) == 1)
		{
			return 0;
		}
		return SErrGetLastError() == 183 ? 0x80000004 : 0x80000006;
	case MapDirEntryFlags::MDEF_SCENARIO:
		if (!v8->fully_loaded)
		{
			FullyLoadMapDirEntry_(v8);
		}
		if (v8->error == 1)
		{
			return 0x80000007;
		}
		if (v8->error == 2)
		{
			return 0x80000001;
		}

		MapChunks a4a;
		a4a.tbl_index_title = 0;
		a4a.tbl_index_description = 0;
		for (int i = 0; i < _countof(a4a.player_force); i++)
		{
			a4a.player_force[i] = 0;
		}
		for (int i = 0; i < _countof(a4a.tbl_index_force_name); i++)
		{
			a4a.tbl_index_force_name[i] = 0;
		}
		for (int i = 0; i < _countof(a4a.force_flags); i++)
		{
			a4a.force_flags[i] = (ForceFlags)0;
		}
		a4a.version = 0;
		a4a.data7 = 0;
		if (!ReadMapData_(v8->full_path, &a4a, 0))
		{
			return LOWORD(a4a.version) > 0x3B ? 0x80000001 : 0x80000007;
		}
		if (!source)
		{
			SStrCopy(dest, v8->full_path, 0x104u);
			return 0;
		}
		if (!multiPlayerMode)
		{
			if (sub_4AAF50(SBYTE1(a4), SHIWORD(a4), (GameType)a4))
			{
				if (!v8->human_player_slots)
				{
					return 0x8000000C;
				}
				if (!v8->computer_slots)
				{
					return 0x8000000D;
				}
			}
			else if (v8->human_player_slots_maybe < 2)
			{
				return 0x8000000D;
			}
		}

		memset(&game_data, 0, sizeof(game_data));
		strcpy_s(game_data.player_name, source);
		strcpy_s(game_data.map_name, v8->title);
		game_data.height = v8->map_height_tiles;
		game_data.width = v8->map_width_tiles;
		*(_DWORD*)&game_data.game_type = a4;
		game_data.max_players = v8->human_player_slots_maybe;
		game_data.tileset = CurrentTileSet;
		game_data.approval_status = v8->unknown2;
		game_data.active_human_players = 1;
		if (unsigned result = sub_4A68D0(&game_data, a5))
		{
			return result;
		}
		isHost = 0;
		char v13;
		if (multiPlayerMode)
		{
			v13 = CreateLadderGame_(&game_data, a3);
		}
		else if (initSingle())
		{
			v13 = CreateGame_(&game_data);
		}
		else
		{
			return 0x80000007;
		}

		if (v13)
		{
			save_Recent_Map_Data(v8);
			return 0;
		}
		switch (SErrGetLastError())
		{
		case 68:
			return 0x80000005;
		case 183:
			return 0x80000004;
		case 0x85100085:
			return 0x8000000F;
		default:
			return 0x80000006;
		}
	case MapDirEntryFlags::MDEF_DIRECTORY:
		SStrCopy(dest, v8->full_path, MAX_PATH);
		return 0x8000000B;
	case MapDirEntryFlags::MDEF_REPLAY:
		InReplay = 1;
		v8->fully_loaded = 0;
		LoadReplayMapDirEntry_(v8);
		if (v8->error == 1)
		{
			return 0x80000007;
		}
		if (v8->error == 2)
		{
			return 0x80000001;
		}
		if (!source)
		{
			SStrCopy(dest, v8->full_path, 0x104u);
			return 0;
		}
		if (a3 && !*(_BYTE*)a3)
		{
			return 0x8000000A;
		}
		memcpy(&game_data, &v8->game_data, sizeof(game_data));
		strcpy_s(game_data.player_name, source);
		if (!multiPlayerMode)
		{
			if (!initSingle())
			{
				return 0x80000007;
			}
			if (!CreateGame_(&game_data))
			{
				return 0x80000006;
			}
			return 0;
		}
		if (CreateLadderGame_(&game_data, a3))
		{
			return 0;
		}
		if (SErrGetLastError() == 183)
		{
			return 0x80000004;
		}
		return 0x80000006;
	default:
		return 0x80000006;
	}
}

unsigned __stdcall sub_4A8050__(char* source, int a3, unsigned int a4, unsigned __int8 a5, char* dest)
{
	MapDirEntry* a1;

	__asm mov a1, eax

	return sub_4A8050_(a1, source, a3, a4, a5, dest);
}

FUNCTION_PATCH((void*)0x4A8050, sub_4A8050__);

void fileExt_(const char* a1, MapDirEntryFlags flags)
{
	MapDirEntryFlags v15 = MapDirEntryFlags(flags & 0x70);

	AddDirectoryToMapDirListing("..", a1);

	for (const auto& entry : std::filesystem::directory_iterator(a1))
	{
		std::filesystem::path filename = entry.path().filename();

		if (entry.is_directory())
		{
			AddDirectoryToMapDirListing(filename.generic_string().c_str(), a1);
		}
		else if ((GetFileAttributes(entry.path().c_str()) & FILE_ATTRIBUTE_HIDDEN) == 0)
		{
			std::filesystem::path extension = filename.extension();

			if (IsExpansion && extension == ".scx")
			{
				AddFileToMapDirListing(a1, filename.generic_string().c_str(), v15 | MapDirEntryFlags::MDEF_SCENARIO);
			}
			else if (extension == ".scm" || extension == ".chk" || extension == ".scn")
			{
				AddFileToMapDirListing(a1, filename.generic_string().c_str(), v15 | MapDirEntryFlags::MDEF_SCENARIO);
			}
			else if (multiPlayerMode && extension == getSaveExtension())
			{
				AddFileToMapDirListing(a1, filename.generic_string().c_str(), v15 | MapDirEntryFlags::MDEF_SAVEGAME);
			}
			else if (extension == ".rep")
			{
				AddFileToMapDirListing(a1, filename.generic_string().c_str(), v15 | MapDirEntryFlags::MDEF_REPLAY);
			}
		}
	}
}

FAIL_STUB_PATCH(fileExt);

int getMapListEntryCount_(int (__stdcall* callback)(MapDirEntry*, char*, MapDirEntryFlags), MapDirEntryFlags flags, char* directory, char* filename)
{
	if (!callback)
	{
		return 255;
	}
	if (!byte_6D5AA8)
	{
		strcpy(&byte_6D5AA8, get_GluAll_String(SAVE));
	}
	vector_delete(&stru_51A274);
	if (flags & 0x10)
	{
		LoadRecentMapsCombobox();
	}
	else
	{
		fileExt_(directory, flags);
	}

	for (MapDirEntry* entry = (MapDirEntry*)((int)dword_51A27C <= 0 ? 0 : (unsigned int)dword_51A27C); (int)entry > 0; entry = entry->next)
	{
		entry->listbox_index_maybe = callback(entry, entry->name, entry->flags);
	}

	if (filename)
	{
		for (MapDirEntry* entry = (MapDirEntry*)((int)dword_51A27C <= 0 ? 0 : (unsigned int)dword_51A27C); (int)entry > 0; entry = entry->next)
		{
			if (!_strnicmp(filename, entry->name, strlen(entry->name)))
			{
				return entry->listbox_index_maybe;
			}
			if (!entry)
			{
				entry = (MapDirEntry*)&stru_51A274.next;
			}
		}
	}

	for (MapDirEntry* entry = (MapDirEntry*)((int)dword_51A27C <= 0 ? 0 : (unsigned int)dword_51A27C); (int)entry > 0; entry = entry->next)
	{
		if (char* extension = strrchr(entry->full_path, '.'))
		{
			if (!_stricmp(extension + 1, "scx") || !_stricmp(extension + 1, "scm"))
			{
				return entry->listbox_index_maybe;
			}
		}
		if (!entry)
		{
			entry = (MapDirEntry*)&stru_51A274.next;
		}
	}

	unsigned entry = (int)dword_51A27C <= 0 ? 0 : (unsigned int)dword_51A27C;
	return (u8)((entry != 0) - 1);
}

int __stdcall getMapListEntryCount__(MapDirEntryFlags flags, char* directory, char* filename)
{
	int(__stdcall * callback)(MapDirEntry*, char*, MapDirEntryFlags);

	__asm mov callback, eax

	return getMapListEntryCount_(callback, flags, directory, filename);
}

FUNCTION_PATCH((void*)0x4A73C0, getMapListEntryCount__);

const char* __stdcall getRaceString(RaceId race)
{
	static TblFile tbl_file("rez\\races.tbl");

	return tbl_file[race];
}

std::vector<RaceId> SELECTABLE_RACES = {
	RaceId::RACE_Zerg,
	RaceId::RACE_Terran,
	RaceId::RACE_Protoss,
	RaceId::RACE_Random,
};

void loadMenu_None_()
{
	glGluesMode = GLUE_MAIN_MENU;
	LastControlID = -1;
	if (dword_6D5A3C)
	{
		LastControlID = dword_6D5A54;
		if (!dword_6D5A54)
		{
			LastControlID = 3;
		}
		DestroyDialog(dword_6D5A3C);
	}
	if (dword_6D5A58)
	{
		LastControlID = 3;
		DestroyDialog(dword_6D5A58);
	}
	if (dword_51C418)
	{
		DestroyDialog(dword_51C418);
	}
	dword_51C418 = 0;
}

FAIL_STUB_PATCH(loadMenu_None);

void jmpNoMenu_()
{
	if (glGluesMode == GLUE_GENERIC)
	{
		loadMenu_None_();
	}
}

FAIL_STUB_PATCH(jmpNoMenu);

int load_gluGameMode_BINDLG_()
{
	if (dword_6D5A3C)
	{
		LastControlID = dword_6D5A54;
		if (!dword_6D5A54)
		{
			LastControlID = 3;
		}
		DestroyDialog(dword_6D5A3C);
	}
	dword_6D5A40 = off_51A6F0;
	dword_599D98 = 28;
	SStrCopy(byte_599C98, get_GluAll_String((GluAllTblEntry)0xAA), 0xFFu);
	byte_599B98[0] = 0;

	char fileName[260];
	strcpy_s(fileName, stru_50E06C[stru_4FFAD0[glGluesMode].menu_position].glue_path);
	strcat_s(fileName, "\\retail_ex.pcx");

	int width;
	int height;
	int read;
	if (!SBmpAllocLoadImage(fileName, (int*)palette, (void**)&read, &width, &height, 0, 0, allocFunction))
	{
		throw FileNotFoundException(fileName, SErrGetLastError());
	}
	p_hist_pcx.wid = width;
	p_hist_pcx.ht = height;
	p_hist_pcx.data = (u8*)read;

	dword_6D5A3C = LoadDialog("rez\\gluGameMode.bin");
	return gluLoadBINDlg_(dword_6D5A3C, Popup_Main_);
}

FAIL_STUB_PATCH(load_gluGameMode_BINDLG);

int BWFXN_gluPOKCancel_MBox_(const char* a1)
{
	if (dword_6D5A3C)
	{
		LastControlID = dword_6D5A54;
		if (!dword_6D5A54)
		{
			LastControlID = 3;
		}
		DestroyDialog(dword_6D5A3C);
	}
	dword_6D5A40 = off_51A6E4;
	dword_599D98 = 12;
	strcpy(byte_599C98, a1);
	byte_599B98[0] = 0;

	char fileName[MAX_PATH];
	strcpy_s(fileName, stru_50E06C[stru_4FFAD0[glGluesMode].menu_position].glue_path);
	strcat_s(fileName, "\\pDPopup.pcx");

	u8* data;
	int width, height;
	if (!SBmpAllocLoadImage(fileName, (int*) palette, (void**)&data, &width, &height, 0, 0, allocFunction))
	{
		throw FileNotFoundException(fileName, SErrGetLastError());
	}
	p_hist_pcx.wid = width;
	p_hist_pcx.ht = height;
	p_hist_pcx.data = data;

	dialog* dword_6D5A3C = (dialog*) fastFileRead_(NULL, 0, "rez\\gluPOkCancel.bin", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
	if (dword_6D5A3C)
	{
		dword_6D5A3C->lFlags |= DialogFlags::CTRL_ACTIVE;
		AllocInitDialogData(dword_6D5A3C, dword_6D5A3C, AllocBackgroundImage, "Starcraft\\SWAR\\lang\\gluPopup.cpp", 369);
	}
	return gluLoadBINDlg(dword_6D5A3C, Popup_Main_) == 1;
}

int __cdecl BWFXN_gluPOKCancel_MBox__()
{
	const char* a1;

	__asm mov a1, eax

	return BWFXN_gluPOKCancel_MBox_(a1);
}

FUNCTION_PATCH((void*)0x4B73B0, BWFXN_gluPOKCancel_MBox__);

BOOL BriefingLoopTurns_(int* a1)
{
	*a1 = 0;
	DWORD tick_count = GetTickCount();
	if (tick_count < dword_6D63D0 + 250)
	{
		return 1;
	}
	gameLoopTurns_();
	if (byte_57EE78)
	{
		dword_6D63D0 = tick_count;
	}
	*a1 = sub_485820();
	return glGluesMode != GLUE_GENERIC;
}

FAIL_STUB_PATCH(BriefingLoopTurns);

bool __fastcall sub_46D340_(dialog* dlg)
{
	int v5;
	int v2 = BriefingLoopTurns_(&v5);
	if (!v2)
	{
		LastControlID = 100;
		dlgEvent event;
		event.cursor.x = Mouse.x;
		event.cursor.y = Mouse.y;
		*(_DWORD*)&event.wSelection = 0;
		event.dwUser = USER_ACTIVATE;
		event.wNo = EVN_USER;
		v2 = dlg->pfcnInteract(dlg, &event);
		dlg->fields.dlg.pModalFcn = 0;
		return v2;
	}
	if (v5)
	{
		LastControlID = 101;
		dlgEvent event;
		event.cursor.x = Mouse.x;
		event.cursor.y = Mouse.y;
		*(_DWORD*)&event.wSelection = 0;
		event.dwUser = USER_ACTIVATE;
		event.wNo = EVN_USER;
		v2 = dlg->pfcnInteract(dlg, &event);
		dlg->fields.dlg.pModalFcn = 0;
		return v2;
	}
	return v2;
}

FAIL_STUB_PATCH(sub_46D340);

void sub_46D3C0_(dialog* dlg)
{
	if (multiPlayerMode)
	{
		dialog* v2 = getControlFromIndex_(dlg, 5);
		HideDialog_(v2);
		dialog* v4 = getControlFromIndex_(dlg, 20);
		HideDialog_(v4);
	}
	dword_6556D8 = 0;
	byte_6554B0 = 1;
	dword_6556DC = 0;
	GameKeepAlive_();
	dlg->fields.dlg.pModalFcn = sub_46D340_;
}

FAIL_STUB_PATCH(sub_46D3C0);

int __fastcall gluRdyZ_Secret_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_INIT:
			ButtonVideo(dlg);
			return genericCommonInteract(evt, dlg);
		case EventUser::USER_DESTROY:
			genericLightupBtnUserDestroyEventHandler(dlg);
			return genericCommonInteract(evt, dlg);
		}
	}

	return 0;
}

FAIL_STUB_PATCH(gluRdyZ_Secret);

void loadPortdataDAT_()
{
	LoadGameData_(unitsDat, "arr\\units.dat");
	LoadGameData_(portdataDat, "arr\\portdata.dat");

	portdata_tbl = (WORD*)fastFileRead_(NULL, 0, "arr\\portdata.tbl", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);

	for (int i = 0; i < 110; i++)
	{
		dword_655C58[i] = (int)GetTblString(portdata_tbl, LOWORD(dword_655C58[i]));
		dword_655E80[i] = (int)GetTblString(portdata_tbl, LOWORD(dword_655E80[i]));
	}

	// TODO: dynamically allocate portdataDat memory

	dword_68F528[0] = (int)portdataDat[0].address;
	dword_68F528[1] = (int)portdataDat[1].address;
	dword_68F528[2] = (int)portdataDat[2].address;
	dword_68F528[3] = (int)portdataDat[3].address;
}

FAIL_STUB_PATCH(loadPortdataDAT);

void ShowPortrait_(unsigned __int8 a1, int a2, UnitType unit_type)
{
	dialog* v6 = getControlFromIndex_(dword_66FF6C, a1 + 15);
	u16 portrait_id = setBuildingSelPortrait_(unit_type);
	Portrait* portrait = (Portrait*)v6->lUser;

	if (portrait->video)
	{
		SVidPlayEnd(portrait->video);
		portrait->video = 0;
	}
	portrait->flags = 2;
	portrait->portrait_id = portrait_id;
	StartUnitPortrait(v6);
}

FAIL_STUB_PATCH(ShowPortrait);

int __fastcall BRFACT_SkipTutorial_(Action* action, BYTE action_index)
{
	if (!multiPlayerMode && active_campaign)
	{
		CreateSkipTutorialButton();
	}
	return 1;
}

FUNCTION_PATCH(BRFACT_SkipTutorial, BRFACT_SkipTutorial_);

int __fastcall BRFACT_ShowPortrait_(Action* action, BYTE action_index)
{
	ShowPortrait_(action->player, action_index, (UnitType) action->unit);
	return 1;
}

FUNCTION_PATCH(BRFACT_ShowPortrait, BRFACT_ShowPortrait_);

int __fastcall BRFACT_PlayWAV_(Action* action, BYTE action_index)
{
	char buff[MAX_PATH];

	if (MapStringTbl.buffer && action->wavString)
	{
		const char* wav_filename = GetTblString(MapStringTbl.buffer, action->wavString);

		if (active_campaign)
		{
			sprintf_s(buff, "%s\\%s", MapdataFilenames_[CampaignIndex], wav_filename);
		}
		else
		{
			strcpy_s(buff, wav_filename);
		}

		PlayWavByFilename_maybe(buff);
	}

	return 1;
}

FUNCTION_PATCH(BRFACT_PlayWAV, BRFACT_PlayWAV_);

void BriefingStart_(dialog* dlg, int buffer)
{
	dlg->lUser = 0;
	DisableControl(getControlFromIndex_(dlg, 13));
	if (multiPlayerMode)
	{
		showDialog(getControlFromIndex_(dlg, -12));
	}

	if (buffer)
	{
		leaveGame(3);
		if (byte_6554B0 == 1)
		{
			byte_6554B0 = 0;
		}
		dlg->fields.dlg.pModalFcn = 0;
	}
	else if (byte_6554B0 == 1)
	{
		// TODO: handle this properly
		BYTE command = 0x54;
		BWFXN_QueueCommand(&command, 1);
		byte_6554B0 = 2;
	}
}

FAIL_STUB_PATCH(BriefingStart);

void __fastcall MissionBriefingLoop_(dialog* dlg, __int16 a2)
{
	TriggerListEntry* lUser = (TriggerListEntry*)dlg->lUser;
	if (lUser && BriefingActionsLoop(&lUser->container))
	{
		if ((int)lUser->next <= 0)
		{
			dlg->lUser = 0;
			if (multiPlayerMode)
			{
				BriefingStart_(dlg, 0);
			}
		}
		else
		{
			dlg->lUser = (int)lUser->next;
		}
	}
}

FAIL_STUB_PATCH(MissionBriefingLoop);

void sub_46D220_(dialog* a1)
{
	dword_66FF6C = a1;
	sub_4CC990_();
	loadPortdataDAT_();
	dialog* v3 = getControlFromIndex_(a1, -10);
	v3->pfcnUpdate = (FnUpdate)sub_46C6E0;
	stru_66FF64.wid = v3->rct.right - v3->rct.left + 1;
	stru_66FF64.ht = 20 * (v3->rct.bottom - v3->rct.top + 1);
	stru_66FF64.data = (u8*)SMemAlloc((__int16)(v3->rct.right - v3->rct.left + 1) * (__int16)stru_66FF64.ht, "Starcraft\\SWAR\\lang\\gluRdy.cpp", 228, 0);
	SetCallbackTimer(10, a1, 100, MissionBriefingLoop_);
	sub_46CBC0(a1);
	if (!a1->lUser)
	{
		showDialog_(getControlFromIndex_(a1, -14));
		if (multiPlayerMode)
		{
			BriefingStart_(a1, 0);
		}
		else
		{
			DisableControl(getControlFromIndex_(a1, 20));
		}
	}
}

FAIL_STUB_PATCH(sub_46D220);

void sub_46D200_(const MusicTrackDescription* music_track)
{
	stopMusic_();
	DLGMusicFade_(music_track);
}

FAIL_STUB_PATCH(sub_46D200);

MenuPosition BWFXN_NetSelectReturnMenu_()
{
	switch (NetMode.as_number)
	{
	case 'BNET': return MenuPosition::GLUE_BATTLE;
	case 'ATLK': return MenuPosition::GLUE_GAME_SELECT;
	case 'IPXN': return MenuPosition::GLUE_GAME_SELECT;
	case 'IPXX': return MenuPosition::GLUE_GAME_SELECT;
	case 'UDPN': return MenuPosition::GLUE_GAME_SELECT;
	case 'MODM': return MenuPosition::GLUE_MODEM;
	case 'MDMX': return MenuPosition::GLUE_MODEM;
	case 'SCBL': return MenuPosition::GLUE_DIRECT;
	}
	return MenuPosition::GLUE_MAIN_MENU;
}

FAIL_STUB_PATCH(BWFXN_NetSelectReturnMenu);

void sub_46D1F0_()
{
	sub_46D200_(&title_music);
}

FAIL_STUB_PATCH(sub_46D1F0);


void gluModem_CustomCtrlID_(dialog* a1)
{
	static FnInteract functions[] = {
		NULL,
		NULL,
		NULL,
		NULL,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		Menu_Generic_Button,
		genericLabelInteract,
		genericListboxInteract,
		genericLabelInteract,
		genericLabelInteract,
	};

	static swishTimer timers[] = {
		{1, 0},
		{2, 2},
		{3, 3},
		{4, 2},
	};

	registerMenuFunctions_(functions, a1, sizeof(functions));
	DlgSwooshin_(a1, timers, 0);
}

FAIL_STUB_PATCH(gluModem_CustomCtrlID);

int __fastcall gluModem_Main_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			sub_4B0AE0(dlg);
			DLG_SwishIn_(dlg);
			break;
		case USER_DESTROY:
			Sleep(400);
			waitLoopCntd(56, dlg);
			break;
			return genericDlgInteract(dlg, evt);
		case USER_ACTIVATE:
			switch (LastControlID)
			{
			case 5:
				if (!load_gluModemStatus(dlg))
				{
					return 1;
				}
				LastControlID = 5;
				return DLG_SwishOut_(dlg);
			case 6:
				if (dword_59B82C && !BWFXN_gluPOKCancel_MBox_(get_GluAll_String((GluAllTblEntry)82)))
				{
					return 1;
				}
				LastControlID = 6;
				return DLG_SwishOut_(dlg);
			case 7:
				AU_PHONENUMBER(dlg);
				return 1;
			case 8:
				DeleteGluModemEntry(dlg);
				return 1;
			case 9:
				if (is_spawn)
				{
					BWFXN_gluPOK_MBox_(GetNetworkTblString_(104));
					return 1;
				}
				return DLG_SwishOut_(dlg);
			default:
				return DLG_SwishOut_(dlg);
			}
			break;
		case USER_INIT:
			gluModem_CustomCtrlID_(dlg);
			return genericDlgInteract(dlg, evt);
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(gluModem_Main);

void loadMenu_gluModem_()
{
	dialog* gluModem_bin = LoadDialog("rez\\gluModem.bin");
	switch (gluLoadBINDlg_(gluModem_bin, gluModem_Main_))
	{
	case 5:
		glGluesMode = GLUE_CHAT;
		break;
	case 6:
		glGluesMode = GLUE_CONNECT;
		break;
	case 9:
		glGluesMode = GLUE_CREATE;
		break;
	default:
		glGluesMode = GLUE_MAIN_MENU;
		break;
	}

	changeMenu_();
}

FAIL_STUB_PATCH(loadMenu_gluModem);

void CreateRaceDropdown_(dialog* dlg, RaceId race)
{
	HIBYTE(dlg->wUser) |= 1;
	if (!dword_68F520)
	{
		showDialog_(dlg);
	}
	EnableControl(dlg);
	if ((dlg->lFlags & CTRL_UPDATE) == 0)
	{
		dlg->lFlags |= CTRL_UPDATE;
		updateDialog(dlg);
	}

	dlgEvent v11;
	v11.cursor.y = Mouse.y;
	v11.wNo = EVN_USER;
	v11.dwUser = USER_NEXT;
	*(_DWORD*)&v11.wSelection = 0;
	v11.cursor.x = Mouse.x;
	dlg->pfcnInteract(dlg, &v11);
	dlg->lFlags |= CTRL_LBOX_NORECALC;
	dlg->pfcnInteract = genericComboboxInteract;
	ClearListBox(dlg);

	u8 v5 = 0;
	u8 v12 = 0;

	for (int i = 0; i < SELECTABLE_RACES.size(); i++)
	{
		const char* race_name = getRaceString(SELECTABLE_RACES[i]);
		u8 v9 = ListBox_AddEntry(race_name, dlg, 0);
		if (v9 == 0xFF)
		{
			break;
		}
		dlg->fields.list.pdwData[v9] = SELECTABLE_RACES[i];
		if (SELECTABLE_RACES[i] == race)
		{
			v12 = v9;
		}
		v5 = v12;
	}

	if (dlg->lFlags & CTRL_LBOX_NORECALC)
	{
		dlg->lFlags &= ~CTRL_LBOX_NORECALC;
		List_Update(dlg);
	}
	if (v5 < dlg->fields.scroll.bSliderSkip || v5 == 0xFF)
	{
		*(_DWORD*)&v11.wSelection = v5;
		v11.wNo = EVN_USER;
		v11.dwUser = USER_SELECT;
		dlg->pfcnInteract(dlg, &v11);
		DlgSetSelected_UpdateScrollbar(v5, dlg);
	}
	dlg->pfcnInteract = sub_450A60;
}

void __stdcall CreateRaceDropdown__(RaceId race)
{
	dialog* dlg;

	__asm mov dlg, eax

	CreateRaceDropdown_(dlg, race);
}

FUNCTION_PATCH((void*)0x450AB0, CreateRaceDropdown__);

int killTimerFunc_()
{
	if (uIDEvent)
	{
		if (gwGameMode == GamePosition::GAME_GLUES && glGluesMode == MenuPosition::GLUE_CHAT)
		{
			sub_4B89A0();
		}
		KillTimer(0, uIDEvent);
		uIDEvent = 0;
		countdown_seconds = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

FAIL_STUB_PATCH(killTimerFunc);

void CALLBACK TimerFunc_(HWND a2, UINT a3, UINT_PTR a4, DWORD a5)
{
	if (gwGameMode != GamePosition::GAME_GLUES || glGluesMode != MenuPosition::GLUE_CHAT)
	{
		killTimerFunc_();
	}
	else if (countdown_seconds)
	{
		DLG_GlueCountdown(--countdown_seconds);
	}
	else
	{
		killTimerFunc_();
		countdown_finished_maybe = 1;
	}
}

FAIL_STUB_PATCH(TimerFunc);

void __cdecl SetTimerFunc_()
{
	dword_6D5C28 = 1;
	countdown_seconds = 5;
	uIDEvent = SetTimer(0, 0, 1000, TimerFunc_);
	if (gwGameMode == GamePosition::GAME_GLUES && glGluesMode == MenuPosition::GLUE_CHAT)
	{
		DLG_GlueCountdown(countdown_seconds);
	}
}

FUNCTION_PATCH(SetTimerFunc, SetTimerFunc_);

void sub_4D3860_()
{
	if (map_download)
	{
		sub_472C10(&map_download);
		map_download = 0;
	}
	while (dword_51A300 > 0)
	{
		mapTransferVector2_clear(dword_51A300);
	}
	mapTransferVector_clear();
	JoinGame();
	clearGameNextMenu();
	leaveGame(1073741825);
	killTimerFunc_();
	g_LocalHumanID = -1;
	g_LocalNationID = -1;
	playerid = -1;
	memset(Players, 0, sizeof(Players));
	isHost = 0;
	if (loadGameFileHandle)
	{
		fclose(loadGameFileHandle);
		loadGameFileHandle = 0;
	}
}

FAIL_STUB_PATCH(sub_4D3860);

BOOL extensionIsValidMap_(const char* path)
{
	if (const char* extension_separator = strrchr(path, '.'))
	{
		const char* extension = extension_separator + 1;
		if (!_stricmp(extension, "scx") || !_stricmp(extension, "scm") || !_stricmp(extension, "rep") || !_stricmp(extension, "chk"))
		{
			return 1;
		}
	}
	return 0;
}

BOOL __cdecl extensionIsValidMap__()
{
	const char* path;

	__asm mov path, eax

	return extensionIsValidMap_(path);
}

FUNCTION_PATCH((void*)0x459FB0, extensionIsValidMap__);

bool userHasMap_()
{
	CHAR Filename[MAX_PATH];
	if (!GetModuleFileNameA(0, Filename, sizeof(Filename)))
	{
		Filename[0] = 0;
	}
	char* v0 = strrchr(Filename, '\\');
	if (v0)
	{
		v0[1] = 0;
	}

	char dest[260];
	strcpy_s(dest, Filename);
	strcat_s(dest, "maps\\download\\");

	char base[260];
	strcpy_s(base, Filename);
	strcat_s(base, "maps\\");

	map_download = 0;
	mapStringCreate(playerid, (int)LOBSEND_SendMapData, MaxTurnSize - 4, dest, base);
	if (!isHost)
	{
		return true;
	}
	if (loadGameFileHandle)
	{
		return true;
	}
	map_download = CreateMapDownload(CurrentMapFileName);

	return map_download;
}

FAIL_STUB_PATCH(userHasMap);

signed int sub_4D4130_()
{
	game_starting_maybe = 0;
	if (!playerid && !isHost)
	{
		bootReason(4);
		return 0;
	}
	dword_596BB0 = 0;
	for (int i = 0; i < 8; i++)
	{
		byte_66FF34[i] = 8;
	}

	if (gameData.save_timestamp)
	{
		char buff[28];
		if (!(sub_4CFE40(gameData.save_timestamp, buff, 0x1Cu) && CMDRECV_LoadGame(buff) && sub_4CF5F0()))
		{
			sub_4D3860_();
			if (!outOfGame)
			{
				doNetTBLError_(0, 0, 0, 95);
			}
			return 0;
		}

		if (isHost)
		{
			sub_4AA620();
		}
	}

	memset(Players, 0, sizeof(Players));
	initializeSlots(-1);
	memset(stru_66FE20, 0, sizeof(stru_66FE20));
	game_result_reported_maybe = 0;
	sub_4728E0();
	if (isHost)
	{
		if (!loadGameFileHandle)
		{
			if (gameData.got_file_values.victory_conditions || gameData.got_file_values.starting_units || gameData.got_file_values.tournament_mode)
			{
				setAllValidPlayerOwnersToOpen();
			}
			updatePlayerOwnerInternal();
			sub_4AA4B0();
		}
		if (isHost)
		{
			sub_4A91E0_();
		}
	}
	g_LocalHumanID = -1;
	g_LocalNationID = -1;
	byte_596894 = 0;
	unignored_player_flags = -1;
	countdown_finished_maybe = 0;
	dword_6D5C28 = 0;
	cleanBufferCounts_();
	if (userHasMap_())
	{
		download_percentage = -1;
		dword_66FF48 = GetTickCount();
		GameKeepAlive_();
		countdownTimerInterval = 0;
		gameState = 2;
		if (isHost)
		{
			gameState = 3;
			u8 save_player_unique_id = loadGameFileHandle == 0 ? 0 : dword_596BAC;
			createVersionBuffer(dword_66FF30, 1, 5, save_player_unique_id, loadGameFileHandle == 0 ? 0 : dword_57F1B0, dword_596BB4, 1);
		}
		return 1;
	}
	else
	{
		sub_4D3860_();
		if (!outOfGame)
		{
			doNetTBLError_(0, 0, 0, 100);
		}
		return 0;
	}
}

FAIL_STUB_PATCH(sub_4D4130);

bool LobbyLoopTurns_()
{
	DWORD tick_count = GetTickCount();
	if (tick_count < 250 + dword_66FF48)
	{
		return 0;
	}
	if (!gameLoopTurns_())
	{
		if (tick_count >= 20000 + dword_66FF48)
		{
			GetPlayerNames();
		}
		return 0;
	}
	ProgressDownload_maybe();
	JoinGame();
	dword_66FF48 = tick_count;
	GameKeepAlive_();
	update_lobby_glue = 1;
	ProgressCountdown();
	return gameState == 9;
}

FAIL_STUB_PATCH(LobbyLoopTurns);

int LobbyLoopCnt_()
{
	LeagueChatFilter();

	if (!LobbyLoopTurns_())
	{
		return LobbyRecv();
	}
	if (gameData.got_file_values.victory_conditions || gameData.got_file_values.starting_units || gameData.got_file_values.tournament_mode || loadGameFileHandle || InReplay)
	{
		gwGameMode = GAME_RUNINIT;
	}
	else
	{
		glGluesMode = Race::races()[Players[g_LocalNationID].nRace].ready_room_menu;
	}

	clearGameNextMenu();
	return 83;
}

FAIL_STUB_PATCH(LobbyLoopCnt);

int sub_4EE210_()
{
	DestroyMapData_();
	if (dword_6BEE8C)
	{
		SMemFree(dword_6BEE8C, "Starcraft\\SWAR\\lang\\Sai_path.cpp", 792, 0);
		dword_6BEE8C = 0;
	}
	freeSaiPaths();
	uselessIterateUnitsAndOrdersDatLoadTables();

	CleanupFlingyDat_();
	CleanupSpritesDat_();

	if (pylon_power_mask)
	{
		SMemFree(pylon_power_mask, "Starcraft\\SWAR\\lang\\CUnitProtoss.cpp", 102, 0);
		pylon_power_mask = NULL;
	}

	if (minimap_Dlg)
	{
		DestroyDialog(minimap_Dlg);
		minimap_Dlg = NULL;
	}

	if (blink_grp)
	{
		SMemFree(blink_grp, "Starcraft\\SWAR\\lang\\minimap.cpp", 2065, 0);
		blink_grp = NULL;
	}

	return 1;
}

FAIL_STUB_PATCH(sub_4EE210);

void saveGame_Create_(dialog* dlg)
{
	DLG_SwishIn_(dlg);
	UpdateCancelButton(dlg, 5, DialogFlags::CTRL_USELOCALGRAPHIC);
	UpdateOKButton(dlg, 3, DialogFlags::CTRL_USELOCALGRAPHIC);
	dialog* v3 = getControlFromIndex_(dlg, 3);
	if (v3)
	{
		HideDialog_(v3);
	}

	dialog* v6 = getControlFromIndex_(dlg, 1);
	if (byte_51BFD8 < v6->fields.scroll.bSliderSkip || byte_51BFD8 == -1)
	{
		dlgEvent v11;
		v11.wNo = EVN_USER;
		v11.dwUser = USER_SELECT;
		*(_DWORD*)&v11.wSelection = (unsigned __int8)byte_51BFD8;
		v6->pfcnInteract(v6, &v11);
		DlgSetSelected_UpdateScrollbar(byte_51BFD8, v6);
	}

	dialog* v9 = getControlFromIndex_(dlg, 4);
	v9->pszText = (char*)get_GluAll_String((GluAllTblEntry)0xB3);
	if ((v9->lFlags & DialogFlags::CTRL_UPDATE) == 0)
	{
		v9->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog(v9);
	}
}

FAIL_STUB_PATCH(saveGame_Create);

void savegameBIN_CustomCtrlID_(dialog* dlg)
{
	static FnInteract functions[] = {
		savegame_Listbox,
		savegame_TextBox,
		savegame_Delete,
		NULL
	};

	registerMenuFunctions_(functions, dlg, sizeof(functions));
	saveGame_InitChildren(dlg);
}

FAIL_STUB_PATCH(savegameBIN_CustomCtrlID);

int __fastcall savegameBIN_Main_(dialog* dlg, struct dlgEvent* evt)
{
	if (evt->wNo == EVN_USER)
	{
		switch (evt->dwUser)
		{
		case USER_CREATE:
			saveGame_Create_(dlg);
			break;
		case USER_DESTROY:
			saveGame_Destroy(dlg);
			break;
		case USER_ACTIVATE:
			if (dialog* v5 = getControlFromIndex_(dlg, 2))
			{
				SStrCopy(byte_51BFB8, v5->pszText, 0x7FFFFFFFu);
				trimTrailingSpaces(byte_51BFB8);
			}
			break;
		case USER_INIT:
			savegameBIN_CustomCtrlID_(dlg);
			break;
		}
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(savegameBIN_Main);

int loadOKCancelBIN_(int a1, char* message, HANDLE a3)
{
	dword_6D1244 = a1;
	dword_5128F0 = a1;

	size_t message_size = strlen(message) + 1;
	okcancel_message = (char*) SMemAlloc(message_size, "Starcraft\\SWAR\\lang\\okcancel.cpp", 174, 0);
	strcpy_s(okcancel_message, message_size, message);

	dword_6D1248 = a3;

	dialog* okcancel_bin = LoadDialog("rez\\okcancel.bin");
	registerMenuFunctions_(0, okcancel_bin, 0);
	return gluLoadBINDlg_(okcancel_bin, okcancel_Interact);
}

FAIL_STUB_PATCH(loadOKCancelBIN);

int ConfirmReplayOverwrite_(char* filename, __int16 a2)
{
	char* v3 = (char*)malloc(strlen(filename) + 3);
	BWFXN_SetFont(FontBase[2]);
	size_t v4 = sub_41FC20(filename, 230);
	BWFXN_SetFont(0);
	strncpy(v3, filename, v4);
	v3[v4] = 0;
	if (v4 != strlen(filename))
	{
		*(_DWORD*)&v3[strlen(v3)] = '...';
	}

	char buff[256];
	sprintf_s(buff, GetNetworkTblString_(a2), v3);

	free(v3);
	return loadOKCancelBIN_(1, buff, dword_6D0F2C) == -2;
}

FAIL_STUB_PATCH(ConfirmReplayOverwrite);

int CopyLastReplayTo_(char* a1)
{
	CHAR FileName[MAX_PATH];
	if (!getDirectoryPath(FileName, sizeof(FileName), a1))
	{
		return 0;
	}

	DWORD v2 = GetFileAttributesA(FileName) != -1;
	if (v2 && !ConfirmReplayOverwrite_(a1, 3))
	{
		return -1;
	}

	return CopyLastReplay(a1);
}

FAIL_STUB_PATCH(CopyLastReplayTo);

int LoadSaveGameBIN_Main_(int a1, RaceId a2)
{
	dword_51BFD4 = a1;
	dword_51BFB4 = a2;
	byte_51BFD8 = 0;
	dword_6D0F2C = (&off_50E040[a2])[a1 != 0 ? 3 : 0];

	savegame_Dlg = LoadDialog("rez\\savegame.bin");
	if (GetUserDefaultLangID() == 1042)
	{
		if (!dword_6D6438)
		{
			dword_6D6438 = ImmGetContext(hWndParent);
		}
		ImmAssociateContext(hWndParent, dword_6D6438);
	}

	int v12 = gluLoadBINDlg_(savegame_Dlg, savegameBIN_Main_);
	if (GetUserDefaultLangID() == 1042)
	{
		if (!dword_6D6438)
		{
			dword_6D6438 = ImmGetContext(hWndParent);
		}
		dword_6D6438 = ImmAssociateContext(hWndParent, 0);
	}

	int v13;
	switch (v12)
	{
	case -2:
		v13 = CopyLastReplayTo_(byte_51BFB8);
		if (v13 == 1)
		{
			dword_6D0F2C = 0;
			return 1;
		}
		else if (v13 == 0)
		{
			const char* v14 = GetNetworkTblString_(101);
			loadOKBIN_(1, v14, dword_6D0F2C);
		}
		break;
	case 3:
		DeleteFileIfExists();
		break;
	}
	dword_6D0F2C = 0;
	return 0;
}

FAIL_STUB_PATCH(LoadSaveGameBIN_Main);

void sub_4DBEE0_(CampaignMenuEntryEx* a1)
{
	CharacterData v5;

	if (!LoadCharacterData(&v5, playerName))
	{
		const char* v1 = GetNetworkTblString_(72);
		if ((_stricmp(playerName, v1) || !verifyCharacterFile(&v5, playerName)) && !outOfGame)
		{
			doNetTBLError_(0, 0, 0, 88);
		}
	}
	int* v3 = IsExpansion ? v5.unlocked_expcampaign_mission + a1->race : v5.unlocked_campaign_mission + a1->race;
	if (*v3 < a1->next_mission)
	{
		*v3 = a1->next_mission;
		CreateCharacterFile(&v5);
	}
}

FAIL_STUB_PATCH(sub_4DBEE0);

void sub_4DBF80_()
{
	if (!multiPlayerMode && !dword_51CA1C)
	{
		if (active_campaign_entry_index != -1 && active_campaign_entry_index + 1 < active_campaign->entries.size())
		{
			sub_4DBEE0_(&active_campaign->entries[active_campaign_entry_index + 1]);
		}
	}
}

FAIL_STUB_PATCH(sub_4DBF80);

void SelectGame_()
{
	SNETUIDATA ui_data;
	SNETPROGRAMDATA program_data;
	SNETPLAYERDATA player_data;
	int playerid;

	initializeProviderVersion(&program_data);
	player_data.dwUnknown = 0;
	player_data.dwSize = 16;
	player_data.pszPlayerName = playerName;
	player_data.pszUnknown = (char*)empty_string;
	initializeProviderStruct(&ui_data);
	SDlg224(CpuThrottle);
	int v0 = SNetSelectGame(1, &program_data, &player_data, &ui_data, 0, &playerid);
	ClearAndFreeCdkeyStrings((const char*)program_data.key, (const char*)program_data.key_owner);
	memset(&program_data, 0, sizeof(program_data));

	if (v0)
	{
		if (!SNetGetPlayerName(playerid, playerName, 0x19u))
		{
			playerName[0] = 0;
		}
		::playerid = playerid;
		glGluesMode = GLUE_CHAT;
		sub_4DCEE0();
		if (!isHost && !sub_452900())
		{
			glGluesMode = GLUE_BATTLE;
		}
	}
	else
	{
		sub_4DCEE0();
		checkLastFileError();
		memset(is_keycode_used, 0, sizeof(is_keycode_used));
		glGluesMode = GLUE_CONNECT;
	}
}

FAIL_STUB_PATCH(SelectGame);

void loadMenu_gluBNRes_()
{
	if (!dword_59BD8C)
	{
		sub_449390();
		BNProfileImageCreate();
		LoadBNIcons();
		dword_59BD9C = (void*)fastFileRead(&dword_59BDA0, 0, "rez\\gluBNRes.res", 0, 0, "Starcraft\\SWAR\\lang\\gamedata.cpp", 210);
		dword_59BD8C = 1;
	}
	SelectGame_();
	sub_4ACF20();
	dword_50E064 = -1;
}

FAIL_STUB_PATCH(loadMenu_gluBNRes);

int SwitchMenu_()
{
	if (!GetModuleFileNameA(NULL, main_directory, MAX_PATH))
		main_directory[0] = 0;

	char* lastDirectorySeparator = strrchr(main_directory, '\\');
	if (lastDirectorySeparator)
		*lastDirectorySeparator = 0;

	for (int i = 0; i < _countof(byte_50CDC1); ++i)
	{
		byte_50CDC1[i] = i;
	}
	LoadMenuFonts();
	loadtEffectPcx();
	loadCursor();
	loadTFontPcx();
	loadDlgGrp_();
	init_gluesounds_();

	if (gwGameMode == GAME_WIN)
	{
		gwGameMode = GAME_GLUES;
		glGluesMode = GLUE_SCORE_T_VICTORY;
		goto LABEL_28;
	}
	if (gwGameMode == GAME_LOSE)
	{
		gwGameMode = GAME_GLUES;
		glGluesMode = GLUE_SCORE_T_VICTORY;
		goto LABEL_28;
	}
	if (Ophelia && !multiPlayerMode)
	{
		Ophelia = 0;
		if (!playerName[0])
		{
			strcpy_s(playerName, GetNetworkTblString_(72));
		}
		customSingleplayer[0] = 0;
		IsExpansion = active_campaign->is_expansion;
		if (active_campaign->is_expansion && !is_expansion_installed || !loadCampaignBIN() || !CreateCampaignGame_(active_campaign->entries[active_campaign_entry_index]))
		{
			glGluesMode = MenuPosition::GLUE_MAIN_MENU;
			IsExpansion = 0;
			goto LABEL_28;
		}
		if (active_campaign->race == RaceId::RACE_Zerg || active_campaign->race == RaceId::RACE_Terran || active_campaign->race == RaceId::RACE_Protoss)
		{
			glGluesMode = Race::races()[active_campaign->race].ready_room_menu;
		}
		else
		{
			glGluesMode = MenuPosition::GLUE_MAIN_MENU;
			IsExpansion = 0;
			goto LABEL_28;
		}

		OpheliaEnabled = 1;
		if (GameCheats & CheatFlags::CHEAT_NoGlues)
			gwGameMode = GAME_RUNINIT;
	}
LABEL_28:
	// TODO: drop the loading screen entirely, maybe?
	Sleep(600);
	if (load_screen)
	{
		DestroyDialog(load_screen);
		load_screen = 0;
	}
	dword_50E064 = -1;
	if (!byte_51A0E9)
	{
		memset(stru_6CEB40, 0, sizeof(PALETTEENTRY[256]));
		byte_51A0E9 = 1;
		memcpy(stru_6CE720, GamePalette, sizeof(PALETTEENTRY[256]));
		gluDlgFadePalette(3);
		BWFXN_RedrawTarget_();
	}
	RefreshCursor_0();
	dword_6D5E38 = jmpNoMenu_;
	if (glue_background_palette[0].data)
		SMemFree(glue_background_palette[0].data, "Starcraft\\SWAR\\lang\\glues.cpp", 442, 0);
	memset(glue_background_palette, 0, sizeof(Bitmap[129]));

	while (gwGameMode == GAME_GLUES)
	{
		switch (glGluesMode)
		{
		case GLUE_GENERIC:
			loadMenu_None_();
			break;
		case GLUE_MAIN_MENU:
			outOfGame = 0;
			loadMenu_gluMain_();
			break;
		case GLUE_LOGIN:
			dword_51C414 = 0;
			loadMenu_gluLogin_();
			break;
		case GLUE_CAMPAIGN:
			loadMenu_gluCmpgn_();
			break;
		case GLUE_READY_T:
			loadMenu_gluRdy(RaceId::RACE_Terran);
			break;
		case GLUE_READY_Z:
			loadMenu_gluRdy(RaceId::RACE_Zerg);
			break;
		case GLUE_READY_P:
			loadMenu_gluRdy(RaceId::RACE_Protoss);
			break;
		case GLUE_EX_CAMPAIGN:
			loadMenu_gluExpCmpgn_();
			break;
		case GLUE_CONNECT:
			loadMenu_gluConn_();
			break;
		case GLUE_MODEM:
			loadMenu_gluModem_();
			break;
		case GLUE_GAME_SELECT:
		case GLUE_DIRECT:
			loadMenu_gluJoin_();
			break;
		case GLUE_CREATE:
			loadMenu_gluCustm_(0);
			break;
		case GLUE_CREATE_MULTI:
			loadMenu_gluCustm_(1);
			break;
		case GLUE_CHAT:
			loadMenu_gluChat_();
			break;
		case GLUE_LOAD:
			loadMenu_gluLoad_();
			break;
		case GLUE_SCORE_Z_DEFEAT:
		case GLUE_SCORE_Z_VICTORY:
		case GLUE_SCORE_T_DEFEAT:
		case GLUE_SCORE_T_VICTORY:
		case GLUE_SCORE_P_DEFEAT:
		case GLUE_SCORE_P_VICTORY:
			loadMenu_gluScore_();
			break;
		case GLUE_BATTLE:
			dword_51C414 = 1;
			dword_50E064 = -1;
			loadMenu_gluBNRes_();
			break;
		default:
			glGluesMode = MenuPosition::GLUE_MAIN_MENU;
			break;
		}
	}
	if (glue_background_palette[0].data)
		SMemFree(glue_background_palette[0].data, "Starcraft\\SWAR\\lang\\glues.cpp", 442, 0);
	memset(glue_background_palette, 0, sizeof(Bitmap[129]));
	stopAllSound_();
	dword_6D5E20 = &GameScreenBuffer;
	if (!byte_51A0E9)
	{
		memset(stru_6CEB40, 0, sizeof(PALETTEENTRY[256]));
		byte_51A0E9 = 1;
		memcpy(stru_6CE720, GamePalette, sizeof(PALETTEENTRY[256]));
		gluDlgFadePalette(3);
		BWFXN_RedrawTarget_();
	}
	RefreshCursor_0();
	playsound_init_UI_(0);
	if (dword_51C60C)
		SMemFree(dword_51C60C, "Starcraft\\SWAR\\lang\\glues.cpp", 338, 0);
	BWFXN_GameEndTarget();
	if (dword_6D125C)
		SMemFree(dword_6D125C, "Starcraft\\SWAR\\lang\\light.cpp", 121, 0);
	dword_6D125C = 0;
	sub_4DC940();
	if (dword_51C40C)
		SMemFree(dword_51C40C, "Starcraft\\SWAR\\lang\\glues.cpp", 370, 0);
	return sub_4DC870();
}

FAIL_STUB_PATCH(SwitchMenu);

void Game_Close_()
{
	dword_5967F0 = 1;
	if (glGluesMode == GLUE_BATTLE)
	{
		DWORD dwProcessId;
		GetWindowThreadProcessId(hWndParent, &dwProcessId);
		EnumWindows(EnumFunc, dwProcessId);
	}
	glGluesMode = GLUE_GENERIC;
	if (gwGameMode == GAME_RUN)
	{
		GameState = 0;
		gwNextGameMode = GAME_EXIT;
		if (!InReplay)
		{
			replay_header.ReplayFrames = ElapsedTimeFrames;
		}
	}
	else
	{
		gwGameMode = GAME_EXIT;
	}
}

FAIL_STUB_PATCH(Game_Close);

void GameShowCursor_(bool show_cursor)
{
	if (cursor == NULL)
	{
		cursor = LoadCursor(NULL, IDC_ARROW);
	}
	SetCursor(show_cursor ? cursor : NULL);

	POINT Point;
	GetCursorPos(&Point);
	SetCursorPos(Point.x, Point.y);
	if (is_cursor_shown != show_cursor)
	{
		is_cursor_shown = show_cursor;
		ShowCursor(show_cursor);
	}
}

FAIL_STUB_PATCH(GameShowCursor);

void doCursorClip_(int a1)
{
	if (a1 != dword_6D5DD0)
	{
		dword_6D5DD0 = a1;
		if (a1 && !dword_6D5DD4)
		{
			SetCursorClipBounds_();
		}
		if (has_viewport)
		{
			ClipCursor(dword_6D5DD0 ? &screen : NULL);
		}
	}
}

FAIL_STUB_PATCH(doCursorClip);

void Game_NumLockInit_()
{
	is_keycode_used[VK_NUMPAD0] = 0;
	is_keycode_used[VK_NUMPAD1] = 0;
	is_keycode_used[VK_NUMPAD2] = 0;
	is_keycode_used[VK_NUMPAD3] = 0;
	is_keycode_used[VK_NUMPAD4] = 0;
	is_keycode_used[VK_NUMPAD5] = 0;
	is_keycode_used[VK_NUMPAD6] = 0;
	is_keycode_used[VK_NUMPAD7] = 0;
	is_keycode_used[VK_NUMPAD8] = 0;
	is_keycode_used[VK_NUMPAD9] = 0;
	is_keycode_used[VK_PRIOR] = 0;
	is_keycode_used[VK_NEXT] = 0;
	is_keycode_used[VK_END] = 0;
	is_keycode_used[VK_HOME] = 0;
	is_keycode_used[VK_LEFT] = 0;
	is_keycode_used[VK_UP] = 0;
	is_keycode_used[VK_RIGHT] = 0;
	is_keycode_used[VK_DOWN] = 0;
}

FAIL_STUB_PATCH(Game_NumLockInit);

void TakeScreenshot_()
{
	SYSTEMTIME SystemTime;
	GetLocalTime(&SystemTime);

	char buff[MAX_PATH];
	sprintf_s(
		buff,
		"SCScrnShot_%02d%02d%02d_%02d%02d%02d.pcx",
		SystemTime.wMonth,
		SystemTime.wDay,
		SystemTime.wYear % 100,
		SystemTime.wHour,
		SystemTime.wMinute,
		SystemTime.wSecond);

	if (SDrawCaptureScreen(buff) && gwGameMode == GAME_RUN)
	{
		const char* v0 = GetNetworkTblString_(110);
		char text[512];
		sprintf_s(text, v0, buff);
		InfoMessage(2000, text);
	}
}

FAIL_STUB_PATCH(TakeScreenshot);

void BWFXN_Game_ButtonDown_(int a1, EventNo a4, LPARAM lParam)
{
	if (((unsigned __int8)InputFlags & (unsigned __int8)~(_BYTE)a1 & 0x2A) == 0)
	{
		LOWORD(InputFlags) = a1 | InputFlags;
		SetCapture(hWndParent);

		dlgEvent event;
		event.wNo = a4;

		Mouse.x = event.cursor.x = min(GET_X_LPARAM(lParam), SCREEN_WIDTH - 1);
		Mouse.y = event.cursor.y = min(GET_Y_LPARAM(lParam), SCREEN_HEIGHT - 1);

		if (!sendInputToAllDialogs_(&event) && input_procedures[a4])
		{
			input_procedures[a4](&event);
		}
	}
}

FAIL_STUB_PATCH(BWFXN_Game_ButtonDown);

void BWFXN_Game_ButtonUp_(int a1, EventNo a4, LPARAM lParam)
{
	__int16 v4 = InputFlags & ~(_WORD)a1;
	if ((v4 & 0x2A) == 0)
	{
		LOWORD(InputFlags) = v4;
		ReleaseCapture();

		dlgEvent event;
		event.wNo = a4;

		Mouse.x = event.cursor.x = min(GET_X_LPARAM(lParam), SCREEN_WIDTH - 1);
		Mouse.y = event.cursor.y = min(GET_Y_LPARAM(lParam), SCREEN_HEIGHT - 1);

		if (!sendInputToAllDialogs_(&event) && input_procedures[a4])
		{
			input_procedures[a4](&event);
		}
	}
}

FAIL_STUB_PATCH(BWFXN_Game_ButtonUp);

void Game_BtnDoubleClick_(int a1, EventNo a4, LPARAM lParam)
{
	if (((unsigned __int8)InputFlags & (unsigned __int8)~(_BYTE)a1 & 0x2A) == 0)
	{
		LOWORD(InputFlags) = a1 | InputFlags;
		SetCapture(hWndParent);

		dlgEvent event;
		event.wNo = a4;

		Mouse.x = event.cursor.x = min(GET_X_LPARAM(lParam), SCREEN_WIDTH - 1);
		Mouse.y = event.cursor.y = min(GET_Y_LPARAM(lParam), SCREEN_HEIGHT - 1);

		if (!sendInputToAllDialogs_(&event) && input_procedures[a4])
		{
			input_procedures[a4](&event);
		}
	}
}

FAIL_STUB_PATCH(Game_BtnDoubleClick);

void Game_MouseWheel_(EventNo wNo, int a2)
{
	dlgEvent v3;

	v3.dwUser = USER_CREATE;
	v3.wSelection = LOWORD(a2);
	v3.wUnk_0x06 = HIWORD(a2);
	v3.wVirtKey = 0;
	v3.wUnk_0x0A = 0;
	v3.wNo = wNo;
	v3.cursor.x = 0;
	v3.cursor.y = 0;
	if (!sendInputToAllDialogs_(&v3) && input_procedures[wNo])
	{
		input_procedures[wNo](&v3);
	}
}

FAIL_STUB_PATCH(Game_MouseWheel);

void Game_Capturechanged_()
{
	if (InputFlags & 0x2A)
	{
		InputFlags &= 0xD5;
		if (gwGameMode == GAME_RUN)
		{
			SetInGameInputProcs_();
		}
		else
		{
			InitializeInputProcs_();
		}
	}
}

FAIL_STUB_PATCH(Game_Capturechanged);

LRESULT __stdcall MainWindowProc_(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	dlgEvent v16;

	switch (Msg)
	{
	case WM_DESTROY:
		hWndParent = NULL;
		break;
	case WM_PAINT:
		{
			PAINTSTRUCT paint;
			BeginPaint(hWnd, &paint);
			EndPaint(hWnd, &paint);
		}
		dword_6D5E1C = 1;
		return 0;
	case WM_CLOSE:
		Game_Close_();
		return 0;
	case WM_ERASEBKGND:
	case WM_SETCURSOR:
		return 1;
	case WM_ACTIVATEAPP:
		is_app_active = keep_app_active_in_background || wParam;
		GameShowCursor_(!is_app_active);
		doCursorClip_(is_app_active);
		memset(is_keycode_used, 0, sizeof(is_keycode_used));
		if (is_app_active)
		{
			dword_6D5E1C = 1;
			if (gwGameMode == GAME_GLUES && glGluesMode == MenuPosition::GLUE_BATTLE)
			{
				SetFocus(FindWindowA("SDlgDialog", 0));
			}
		}
		break;
	case WM_NCACTIVATE:
		if (wParam)
		{
			memset(is_keycode_used, 0, sizeof(is_keycode_used));
		}
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		is_keycode_used[wParam] = 1;
		if (wParam == VK_NUMLOCK)
		{
			Game_NumLockInit_();
		}
		is_keycode_used[VK_MENU] = (GetKeyState(VK_MENU) & 0x8000) != 0;
		if ((InputFlags & 0x2A) == 0)
		{
			WORD key_flags = HIWORD(lParam);
			v16.wVirtKey = wParam;
			v16.wUnk_0x0A = BWFXN_Game_KeyState();
			v16.wNo = (key_flags & KF_REPEAT) ? EVN_KEYRPT : EVN_KEYFIRST;
			if (!sendInputToAllDialogs_(&v16))
			{
				InputProcedure v11 = (key_flags & KF_REPEAT) ? input_procedures[EventNo::EVN_KEYRPT] : input_procedures[EventNo::EVN_KEYDOWN];
				if (v11)
				{
					v11(&v16);
				}
			}
		}
		if (wParam == VK_SCROLL)
		{
			dword_6D5E1C = 1;
			return DefWindowProcA(hWnd, Msg, VK_SCROLL, lParam);
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		is_keycode_used[wParam] = 0;
		is_keycode_used[VK_MENU] = (GetKeyState(VK_MENU) & 0x8000) != 0;
		if (wParam == VK_SNAPSHOT)
		{
			TakeScreenshot_();
			return 1;
		}
		if ((InputFlags & 0x2A) == 0)
		{
			v16.wVirtKey = wParam;
			v16.wUnk_0x0A = BWFXN_Game_KeyState();
			v16.wNo = EventNo::EVN_KEYUP;
			if (!sendInputToAllDialogs_(&v16) && input_procedures[EventNo::EVN_KEYUP])
			{
				input_procedures[EventNo::EVN_KEYUP](&v16);
			}
		}
		break;
	case WM_CHAR:
		if ((InputFlags & 0x2A) == 0)
		{
			v16.wNo = EventNo::EVN_CHAR;
			v16.wVirtKey = wParam;
			v16.wUnk_0x0A = BWFXN_Game_KeyState();
			dlg = 0;
			if (lParam & 0x40000000)
			{
				HIBYTE(v16.wUnk_0x0A) |= 1;
			}
			if (!sendInputToAllDialogs_(&v16) && input_procedures[EventNo::EVN_CHAR])
			{
				input_procedures[EventNo::EVN_CHAR](&v16);
			}
		}
		return 1;
	case WM_IME_STARTCOMPOSITION:
	case WM_IME_ENDCOMPOSITION:
		return 1;
	case WM_IME_COMPOSITION:
		if (GetUserDefaultLangID() != 1042)
		{
			break;
		}
		if (InputFlags & 0x2A)
		{
			return 1;
		}

		{
			HIMC v7 = ImmGetContext(hWnd);
			DWORD v12;
			if (lParam & 2048)
			{
				v12 = 2048;
			}
			else if (lParam & 8)
			{
				v12 = 8;
			}
			else
			{
				ImmReleaseContext(hWnd, v7);
				break;
			}
			DWORD v14 = ImmGetCompositionStringA(v7, v12, 0, 0);
			unsigned __int8* v13 = (unsigned __int8*)malloc(v14);
			ImmGetCompositionStringA(v7, v12, v13, v14);
			ImmReleaseContext(hWnd, v7);
			if (dword_6DC2E0 == 1)
			{
				v16.wNo = EventNo::EVN_CHAR;
				v16.wVirtKey = VK_BACK;
				v16.wUnk_0x0A = 160;
				if (!sendInputToAllDialogs_(&v16) && input_procedures[EventNo::EVN_CHAR])
				{
					input_procedures[EventNo::EVN_CHAR](&v16);
				}
				dword_6DC2E0 = 0;
			}
			if (v14 >= 2)
			{
				dlg = (dialog*)1;
				dword_596A14 = 0;
			}
			if (v14 == 0)
			{
				goto LABEL_52;
			}

			int v10 = 0;
			while (1)
			{
				v16.wNo = EventNo::EVN_CHAR;
				v16.wVirtKey = v13[v10];
				v16.wUnk_0x0A = 160;
				if (!sendInputToAllDialogs_(&v16) && input_procedures[EventNo::EVN_CHAR])
				{
					input_procedures[EventNo::EVN_CHAR](&v16);
				}
				if (dword_596A14 == 1)
				{
					break;
				}
				if (++v10 >= v14)
				{
					if ((lParam & 8) != 0 && v14)
					{
						dword_6DC2E0 = 1;
					}
					goto LABEL_52;
				}
			}
			ImmSetCompositionStringA(v7, 9u, 0, 0, 0, 0);
			if (v10 == 1)
			{
				v16.wNo = EventNo::EVN_CHAR;
				v16.wVirtKey = VK_BACK;
				v16.wUnk_0x0A = 160;
				if (!sendInputToAllDialogs_(&v16) && input_procedures[EventNo::EVN_CHAR])
				{
					input_procedures[EventNo::EVN_CHAR](&v16);
				}
				dword_6DC2E0 = 0;
			}

		LABEL_52:
			free(v13);
			ImmReleaseContext(hWnd, v7);
			return 1;
		}
	case WM_COMMAND:
		if (input_procedures[EventNo::EVN_SYSCHAR])
		{
			v16.wNo = EventNo::EVN_SYSCHAR;
			v16.wVirtKey = wParam;
			input_procedures[EventNo::EVN_SYSCHAR](&v16);
		}
		return 1;
	case WM_SYSCOMMAND:
		if (wParam != SC_CLOSE)
		{
			if (wParam != SC_KEYMENU && wParam != SC_PREVWINDOW)
			{
				break;
			}
		}
		else if (!load_screen)
		{
			if (gwGameMode != GAME_RUN)
			{
				SNetLeagueLogout(playerName);
				Game_Close_();
				return 0;
			}
			PostMessageA(hWnd, WM_COMMAND, 0xFFFF9C6B, 0);
			return DefWindowProcA(hWnd, Msg, SC_RESTORE, lParam);
		}

		return 0;
	case WM_MOUSEMOVE:
		LOBYTE(InputFlags) = InputFlags | 1;
		Mouse.x = min(GET_X_LPARAM(lParam), SCREEN_WIDTH - 1);
		Mouse.y = min(GET_Y_LPARAM(lParam), SCREEN_HEIGHT - 1);
		return 1;
	case WM_LBUTTONDOWN:
		BWFXN_Game_ButtonDown_(2, EventNo::EVN_LBUTTONDOWN, lParam);
		return 1;
	case WM_LBUTTONUP:
		BWFXN_Game_ButtonUp_(2, EventNo::EVN_LBUTTONUP, lParam);
		return 1;
	case WM_LBUTTONDBLCLK:
		Game_BtnDoubleClick_(2, EventNo::EVN_LBUTTONDBLCLK, lParam);
		return 1;
	case WM_RBUTTONDOWN:
		BWFXN_Game_ButtonDown_(8, EventNo::EVN_RBUTTONDOWN, lParam);
		return 1;
	case WM_RBUTTONUP:
		BWFXN_Game_ButtonUp_(8, EventNo::EVN_RBUTTONUP, lParam);
		return 1;
	case WM_RBUTTONDBLCLK:
		Game_BtnDoubleClick_(8, EventNo::EVN_RBUTTONDBLCLK, lParam);
		return 1;
	case WM_MBUTTONDOWN:
		BWFXN_Game_ButtonDown_(32, EventNo::EVN_MBUTTONDOWN, lParam);
		return 1;
	case WM_MBUTTONUP:
		BWFXN_Game_ButtonUp_(32, EventNo::EVN_MBUTTONUP, lParam);
		return 1;
	case WM_MBUTTONDBLCLK:
		Game_BtnDoubleClick_(32, EventNo::EVN_MBUTTONDBLCLK, lParam);
		return 1;
	case WM_MOUSEWHEEL:
		if (GET_WHEEL_DELTA_WPARAM(wParam) >= WHEEL_DELTA)
		{
			Game_MouseWheel_(EventNo::EVN_WHEELUP, lParam);
		}
		else if (GET_WHEEL_DELTA_WPARAM(wParam) <= -WHEEL_DELTA)
		{
			Game_MouseWheel_(EventNo::EVN_WHEELDWN, lParam);
		}
		return 1;
	case WM_CAPTURECHANGED:
		Game_Capturechanged_();
		break;
	case WM_IME_NOTIFY:
		GetUserDefaultLangID();
		return DefWindowProcA(hWnd, WM_IME_NOTIFY, wParam, lParam);
	case WM_QUERYNEWPALETTE:
		dword_6D5E1C = 1;
		return 1;
	case WM_PALETTECHANGED:
		if ((HWND)wParam != hWnd)
		{
			dword_6D5E1C = 1;
		}
		break;
	}
	return DefWindowProcA(hWnd, Msg, wParam, lParam);
}

FAIL_STUB_PATCH(MainWindowProc);

void CreateMainWindow_()
{
	WNDCLASSEXA window_class;

	memset(&window_class, 0, sizeof(window_class));
	window_class.cbSize = sizeof(window_class);
	window_class.style = 8;
	window_class.lpfnWndProc = MainWindowProc_;
	window_class.hInstance = hInst;
	window_class.hIcon = LoadIconA(hInst, (LPCSTR)0x66);
	window_class.hIconSm = (HICON)LoadImageA(hInst, (LPCSTR)0x66, 1u, 16, 16, 0x8000u);
	window_class.hCursor = LoadCursorA(0, (LPCSTR)0x7F00);
	window_class.hbrBackground = (HBRUSH)GetStockObject(5);
	window_class.lpszClassName = "SWarClass";

	if (!RegisterClassExA(&window_class))
	{
		FatalError_("RegisterClass");
	}

	const char* window_name = is_expansion_installed ? "Brood War" : "Starcraft";
	int screen_width = GetSystemMetrics(SM_CXSCREEN);
	int screen_height = GetSystemMetrics(SM_CYSCREEN);
	unsigned window_style = WS_POPUP | WS_SYSMENU;
	if (has_viewport)
	{
		window_style |= WS_VISIBLE;
	}
	hWndParent = CreateWindowExA(0, "SWarClass", window_name, window_style, 0, 0, screen_width, screen_height, 0, 0, hInst, 0);
	if (!hWndParent)
	{
		FatalError_("CreateWindowEx");
	}

	UpdateWindow(hWndParent);
	SetFocus(hWndParent);
	SetCursor(0);
	if (GetUserDefaultLangID() == 1042)
	{
		if (!dword_6D6438)
		{
			dword_6D6438 = ImmGetContext(hWndParent);
		}
		dword_6D6438 = ImmAssociateContext(hWndParent, 0);
	}
}

FAIL_STUB_PATCH(CreateMainWindow);
FAIL_STUB_PATCH(PlayMovieWithIntro);
FAIL_STUB_PATCH(playActiveCinematic);

int sub_4DBD20_(const char* a1, size_t a2, int* a3)
{
	if (*a3 >= _countof(MapdataFilenames_))
	{
		return 0;
	}
	while (1)
	{
		char* v3 = SStrChrR(MapdataFilenames_[*a3], '\\');
		if (v3 && !SStrCmpI(v3 + 1, a1, a2))
		{
			return 1;
		}
		*a3 += 1;
		if (*a3 >= _countof(MapdataFilenames_))
		{
			break;
		}
	}
	return 0;
}

FAIL_STUB_PATCH(sub_4DBD20);

int sub_4DBDA0_(const char* a1)
{
	char* v2 = SStrChrR(a1, '.');
	if (!v2)
	{
		return active_campaign_entry_index;
	}

	size_t v4 = v2 - a1;
	for (int v8 = 0; sub_4DBD20_(a1, v4, &v8); v8++)
	{
		for (auto result = active_campaign->entries.begin(); result != active_campaign->entries.end(); result++)
		{
			if (result->entry_type == CampaignMenuEntryType::MISSION && result->next_mission == v8)
			{
				return result - active_campaign->entries.begin();
			}
		}
	}
	return active_campaign_entry_index;
}

FAIL_STUB_PATCH(sub_4DBDA0);

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

void creditsEndPage_(dialog* a1)
{
	if (byte_51CEAC)
	{
		byte_51CEAC = 0;
		sub_41E9E0(byte_51CEC8);
		memcpy(stru_6CEB40, palette, sizeof(stru_6CEB40));
		if ((a1->lFlags & DialogFlags::CTRL_UPDATE) == 0)
		{
			a1->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog(a1);
		}
		TitlePaletteUpdate_(byte_51CEC8);
	}
}

FAIL_STUB_PATCH(creditsEndPage);

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
				updateDialog(dword_51CEB0);
			}
			showDialog_(dword_51CEB0);
		}
	}
}

FAIL_STUB_PATCH(sub_4D8840);

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
		for (auto& position: establishingShotPositions_)
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

FAIL_STUB_PATCH(runCreditsScriptCommands);

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

FAIL_STUB_PATCH(runCredits);

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

FAIL_STUB_PATCH(credits_keyDwn);

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

FAIL_STUB_PATCH(credits_idle);

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

FAIL_STUB_PATCH(sub_4D8930);

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

FAIL_STUB_PATCH(creditsDlgInit);

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

FAIL_STUB_PATCH(creditsDlgDestroy);

int __fastcall creditsDlgInteract_(dialog* dlg, struct dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
		if (!credits_keyDwn_(dlg, evt))
		{
			return false;
		}
		break;
	case EVN_LBUTTONDOWN:
	case EVN_RBUTTONDOWN:
		if (!runCredits_(dlg))
		{
			return false;
		}
		break;
	case EVN_IDLE:
		if (!credits_idle_(dlg))
		{
			return false;
		}
		break;
	case EVN_USER:
		if (evt->dwUser == 0)
		{
			if (!creditsDlgInit_(dlg))
			{
				return false;
			}
			break;
		}
		else if (evt->dwUser == USER_DESTROY)
		{
			creditsDlgDestroy_(dlg);
		}
		break;
	}

	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(creditsDlgInteract);

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

FAIL_STUB_PATCH(loadInitCreditsBIN);

unsigned LoadScenarioSingle_(char* a1, int a2, const char* a3, unsigned __int8 game_speed)
{
	if (!a1 || !a3)
	{
		return 0x80000006;
	}

	MapChunks a4;
	a4.tbl_index_title = 0;
	a4.tbl_index_description = 0;
	for (int i = 0; i < _countof(a4.player_force); i++)
	{
		a4.player_force[i] = 0;
	}
	for (int i = 0; i < _countof(a4.tbl_index_force_name); i++)
	{
		a4.tbl_index_force_name[i] = 0;
	}
	for (int i = 0; i < _countof(a4.force_flags); i++)
	{
		a4.force_flags[i] = (ForceFlags) 0;
	}
	a4.version = 0;
	a4.data7 = 0;

	if (!ReadMapData_(a1, &a4, 0))
	{
		return LOWORD(a4.version) > 59u ? 0x80000001 : 0x80000006;
	}

	GameData game_data;
	memset(&game_data, 0, sizeof(game_data));
	SStrCopy(game_data.player_name, a3, 0x20u);
	const char* v6 = a4.tbl_index_title ? get_chk_String(a4.tbl_index_title) : "";
	SStrCopy(game_data.map_name, v6, sizeof(game_data.map_name));
	game_data.active_human_players = 1;
	game_data.max_players = getTotalValidSlotCount();
	game_data.width = map_size.width;
	game_data.height = map_size.height;
	game_data.tileset = CurrentTileSet;
	*(_DWORD*)&game_data.game_type = a2;
	game_data.approval_status = a4.data7;

	if (int result = sub_4A68D0(&game_data, game_speed))
	{
		return result;
	}

	isHost = 0;
	dword_51CA1C = 1;
	customSingleplayer[0] = 1;
	if (sub_4DBE50() == 0)
	{
		return 0x80000007;
	}
	else if (CreateGame_(&game_data))
	{
		return 0;
	}
	else if (SErrGetLastError() == 183)
	{
		return 0x80000004;
	}
	else
	{
		return 0x80000006;
	}
}

FAIL_STUB_PATCH(LoadScenarioSingle);

int CreateNextCampaignGame_()
{
	if (!next_scenario[0])
	{
		glGluesMode = IsExpansion != 0 ? GLUE_EX_CAMPAIGN : GLUE_CAMPAIGN;
		return 1;
	}
	GotFileValues* v1 = InitUseMapSettingsTemplate_();
	if (v1)
	{
		int v2 = v1->template_id | ((v1->unused1 | (v1->variation_id << 8)) << 8);
		SMemFree(v1, "Starcraft\\SWAR\\lang\\uiSingle.cpp", 319, 0);

		char dest[260];
		strcpy_s(dest, CurrentMapFileName);
		char* v3 = strrchr(dest, '\\');

		if (v3)
		{
			v3[1] = 0;
			strcat_s(dest, next_scenario);

			if (!LoadScenarioSingle_(dest, v2, playerName, registry_options.GameSpeed))
			{
				switch (Players[g_LocalNationID].nRace)
				{
				case RACE_Zerg:
					glGluesMode = GLUE_READY_Z;
					return 1;
				case RACE_Terran:
					glGluesMode = GLUE_READY_T;
					return 1;
				case RACE_Protoss:
					glGluesMode = GLUE_READY_P;
					return 1;
				}
			}
		}
	}
	if (!outOfGame)
	{
		doNetTBLError_(0, 0, 0, 106);
	}
	glGluesMode = IsExpansion != 0 ? GLUE_EX_CAMPAIGN : GLUE_CAMPAIGN;
	return 0;
}

FAIL_STUB_PATCH(CreateNextCampaignGame);

int ContinueCampaign_(int a1)
{
	if (dword_51CA1C)
	{
		CreateNextCampaignGame_();
		return 1;
	}
	if (!a1)
	{
		gwGameMode = GAME_RESTART;
		return 1;
	}
	gwGameMode = GAME_GLUES;
	if (active_campaign_entry_index == -1 || active_campaign_entry_index >= active_campaign->entries.size())
	{
		return 0;
	}
	if (next_scenario[0])
	{
		active_campaign_entry_index = sub_4DBDA0_(next_scenario);
		next_scenario[0] = 0;
	}
	else
	{
		active_campaign_entry_index += 1;
	}
	if (active_campaign_entry_index >= active_campaign->entries.size())
	{
		glGluesMode = active_campaign->post_epilog_menu;
		gwGameMode = GAME_GLUES;
		active_campaign = nullptr;
		active_campaign_entry_index = -1;
		return 1;
	}
	sub_4DBEE0_(&active_campaign->entries[active_campaign_entry_index]);

	switch (active_campaign->entries[active_campaign_entry_index].entry_type)
	{
	case CampaignMenuEntryType::EPILOG:
		gwGameMode = GAME_EPILOG;
		return 1;
	case CampaignMenuEntryType::CINEMATIC:
		CampaignIndex = (MapData)active_campaign->entries[active_campaign_entry_index].next_mission;
		gwGameMode = GAME_CINEMATIC;
		return 1;
	case CampaignMenuEntryType::MISSION:
		if (CreateCampaignGame_(active_campaign->entries[active_campaign_entry_index]))
		{
			glGluesMode = Race::races()[active_campaign->entries[active_campaign_entry_index].race].ready_room_menu;
			return 1;
		}
	}

	return 0;
}

FAIL_STUB_PATCH(DisplayMissionEpilog);
FAIL_STUB_PATCH(sub_4D8F90);
FAIL_STUB_PATCH(getCampaignIndex);
FAIL_STUB_PATCH(updateActiveCampaignMission);
FAIL_STUB_PATCH(ContinueCampaign);

void BeginEpilog_()
{
	int v0 = registry_options.Music;
	if (!registry_options.Music && registry_options.Sfx)
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
		registry_options.Music = 50;
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

	registry_options.Music = v0;
}

FAIL_STUB_PATCH(BeginEpilog);

void BeginCredits_()
{
	int v0 = registry_options.Music;
	if (!registry_options.Music && registry_options.Sfx)
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
		registry_options.Music = 50;
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
	registry_options.Music = v0;
}

FAIL_STUB_PATCH(BeginCredits);
FAIL_STUB_PATCH(sub_4A60D0);
FAIL_STUB_PATCH(sub_48EB90);

void GameMainLoop_()
{
	HANDLE phFile;

	gwGameMode = GAME_GLUES;
	PreInitData_();
	InitializeInputProcs_();
	CreateMainWindow_();
	audioVideoInit_();
	AppAddExit_(SaveCPUThrottleOption);
	if (SRegLoadValue("Starcraft", "CPUThrottle", 0, (int*)&phFile))
		CpuThrottle = phFile != 0;
	if (has_viewport)
	{
		_SetCursorPos(SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);
	}

	if (cd_archive_mpq && SFileOpenFileEx(cd_archive_mpq, "rez\\gluexpcmpgn.bin", 0, &phFile))
	{
		SFileCloseFile(phFile);
		if (registry_options.field_18 & 0x800)
		{
			registry_options.field_18 &= ~0x800;
			PlayMovie_("smk\\blizzard.smk", SVID_AUTOCUTSCENE);
			try
			{
				PlayMovie_("smk\\starXintr.smk", SVID_AUTOCUTSCENE);
			}
			catch (const FileNotFoundException& e)
			{
			}
		}
	}
	else
	{
		if (registry_options.field_18 & 0x200)
		{
			registry_options.field_18 &= ~0x200;
			PlayMovie_("smk\\blizzard.smk", SVID_AUTOCUTSCENE);
			try
			{
				PlayMovie_("smk\\starintr.smk", SVID_AUTOCUTSCENE);
			}
			catch (const FileNotFoundException& e)
			{
			}
		}
	}

	LoadTitle_();
	LoadInitIscriptBIN_();
	AppAddExit_(CleanupIscriptBINHandle_);
	if (gwGameMode != GAME_GLUES && load_screen)
	{
		DestroyDialog(load_screen);
		load_screen = NULL;
	}

	while (1)
	{
		setCursorType_(CursorType::CUR_TIME);
		BWFXN_RedrawTarget_();
		if (!is_expansion_installed)
			IsExpansion = 0;
		switch (gwGameMode)
		{
		case GAME_RUN:
			GameRun_();
			break;
		case GAME_CINEMATIC:
			PlayMovie_(active_campaign->entries[active_campaign_entry_index]);
			if (gwGameMode == GAME_CINEMATIC)
				ContinueCampaign_(1);
			break;
		case GAME_RESTART:
			next_campaign_mission = 1;
			[[fallthrough]];
		case GAME_RUNINIT:
			gwGameMode = GAME_RUN;
			break;
		case GAME_INTRO:
			PlayMovie_("smk\\blizzard.smk", SVID_AUTOCUTSCENE);
			if (gwGameMode == GAME_INTRO)
			{
				if (cd_archive_mpq && SFileOpenFileEx(cd_archive_mpq, "rez\\gluexpcmpgn.bin", GLUE_MAIN_MENU, &phFile))
				{
					SFileCloseFile(phFile);
					try
					{
						PlayMovie_("smk\\starXintr.smk", SVID_AUTOCUTSCENE);
					}
					catch (const FileNotFoundException& e)
					{
					}
				}
				else
				{
					try
					{
						PlayMovie_("smk\\starintr.smk", SVID_AUTOCUTSCENE);
					}
					catch (const FileNotFoundException& e)
					{
					}
				}
				if (gwGameMode == GAME_INTRO)
					gwGameMode = GAME_GLUES;
			}
			break;
		case GAME_GLUES:
		case GAME_WIN:
		case GAME_LOSE:
			SwitchMenu_();
			break;
		case GAME_CREDITS:
			BeginCredits_();
			if (gwGameMode == GAME_CREDITS)
			{
				gwGameMode = GAME_GLUES;
				glGluesMode = GLUE_MAIN_MENU;
			}
			break;
		case GAME_EPILOG:
			BeginEpilog_();
			if (gwGameMode == GAME_EPILOG)
			{
				ContinueCampaign_(1);
			}
			break;
		default:
			AppExit_(0);
			ProcError(1);
			exit(0);
			return;
		}
	}
}

FAIL_STUB_PATCH(sub_4DA790);
FAIL_STUB_PATCH(sub_481CF0);
FAIL_STUB_PATCH(GameMainLoop);

unsigned int LocalGetLang_()
{
	CHAR Buffer[16];
	char *v2;

	if (local_dll_library && LoadStringA(local_dll_library, 3u, Buffer, 16) || LoadStringA(hInst, 3u, Buffer, 16))
		return strtoul(Buffer, &v2, 16);
	else
		return 1033;
}

FUNCTION_PATCH(LocalGetLang, LocalGetLang_);

void __fastcall FreeLocalDLL_(bool exit_code)
{
	if (local_dll_library)
	{
		FreeLibrary(local_dll_library);
		local_dll_library = 0;
	}
}

FAIL_STUB_PATCH(FreeLocalDLL);

void localDll_Init_(HINSTANCE a1)
{
	char Filename[MAX_PATH];

	if (!GetModuleFileNameA(a1, Filename, MAX_PATH))
		Filename[0] = 0;
	char* v1 = strrchr(Filename, '\\');
	if (!v1)
		v1 = &Filename[-1];
	const char* v2 = "local.dll";
	int i = 0;
	while (true)
	{
		v1[i + 1] = v2[i];
		if (v2[i] == '\0')
			break;
		++i;
	};
	local_dll_library = LoadLibraryA(Filename);
	if (!local_dll_library)
	{
		DialogBoxParamA(a1, (LPCSTR)106, hWndParent, LocalErrProc, (LPARAM)Filename);
		SErrSuppressErrors(1);
		AppExit_(1);
		ProcError(1);
		exit(1);
	}
	int local_lang = LocalGetLang_();
	SFileSetLocale(local_lang);
	AppAddExit_(FreeLocalDLL_);
}

FAIL_STUB_PATCH(localDll_Init);

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

FAIL_STUB_PATCH(DisplayEstablishingShot);

const char* __stdcall get_Tileset_String(Tileset tileset)
{
	static TblFile tbl_file("rez\\tilesets.tbl");

	return tbl_file[tileset];
}

signed int __fastcall packColorShifts_(int a1, void* a2)
{
	for (int i = 0; i < 8; i++)
	{
		if (colorShift[i].data == a2)
			return i;
	}
	return 0;
}

FUNCTION_PATCH(packColorShifts, packColorShifts_);

void UpdateCountdownTimer_()
{
	char buff[32];

	if (CountdownTimer / 3600)
	{
		sprintf_s(buff, "%d:%02d:%02d", CountdownTimer / 3600, CountdownTimer % 3600 / 60, CountdownTimer % 60);
	}
	else
	{
		sprintf_s(buff, "%d:%02d", CountdownTimer / 60, CountdownTimer % 60);
	}
	setCountdownTimerString(buff);
}

FAIL_STUB_PATCH(UpdateCountdownTimer);

void DisableCountdownTimer_()
{
	dialog* v1 = getControlFromIndex_(statres_Dlg, -10);
	HideDialog_(v1);
}

FAIL_STUB_PATCH(DisableCountdownTimer);

void countdownTimersExecute_(unsigned int a2)
{
	for (int i = 0; i < 8; ++i)
	{
		if (byte_6509B8[i])
		{
			if (dword_650980[i] != -1)
			{
				if (dword_650980[i] > a2)
				{
					dword_650980[i] -= a2;
				}
				else
				{
					byte_6509B8[i] = 0;
					dword_650980[i] = 0;
					word_6509A0 = 1;
				}
			}
		}
	}

	if (!IS_GAME_PAUSED)
	{
		bool v3 = word_65097C-- == 0;
		if (v3)
		{
			word_65097C = 15;
			updateLeaderboardSorting();
		}
		v3 = word_6509C0-- == 0;
		if (v3)
		{
			++ElapsedTimeSeconds;
			if (CountdownTimer)
			{
				if (!TimerIsPaused && !--CountdownTimer)
				{
					word_6509A0 = 1;
				}
				if (has_hud)
				{
					UpdateCountdownTimer_();
				}
			}
			else
			{
				if (has_hud)
				{
					DisableCountdownTimer_();
				}
			}
			word_6509C0 = 15;
		}
	}
}

FAIL_STUB_PATCH(countdownTimersExecute);

int __fastcall TriggerAction_NoAction_(Action* a1)
{
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_NoAction);

int __fastcall TriggerAction_Victory_(Action* a1)
{
	if (Players[active_trigger_player].nType == PT_Human || Players[active_trigger_player].nType == PT_Computer)
	{
		endgame_state[active_trigger_player] = VICTORY;
	}
	word_650970 = byte_6509B4 ? 1 : 45;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_Victory);

int __fastcall TriggerAction_Defeat_(Action* a1)
{
	if (endgame_state[active_trigger_player] == EndgameState::VICTORY || endgame_state[active_trigger_player] == EndgameState::UNKNOWN)
	{
		return 0;
	}
	if (Players[active_trigger_player].nType == PT_Human || Players[active_trigger_player].nType == PT_Computer)
	{
		endgame_state[active_trigger_player] = EndgameState::DEFEAT;
	}
	word_650970 = byte_6509B4 ? 1 : 45;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_Defeat);

int __fastcall TriggerAction_PreserveTrigger_(Action* a1)
{
	dword_6509AC->container.dwExecutionFlags |= 4;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_PreserveTrigger);

int __fastcall TriggerAction_Wait_(Action* a1)
{
	if (byte_6509B8[active_trigger_player])
	{
		return 0;
	}
	if (a1->flags & 1)
	{
		a1->flags = a1->flags & 0xFE;
		return 1;
	}
	if ((dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		dword_650980[active_trigger_player] = a1->time;
		byte_6509B8[active_trigger_player] = 1;
		a1->flags |= 1;
		return 0;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_Wait);

int __fastcall TriggerAction_PauseGame_(Action* a1)
{
	if (!multiPlayerMode && !byte_6509B4)
	{
		PlayerType player_type = Players[active_trigger_player].nType;
		if (player_type != PT_Computer && player_type != PT_Rescuable && player_type != PT_Unknown0 && player_type != PT_Neutral && (dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
		{
			DlgAccelerator = hAccTable;
			AcceleratorTables = input_procedures[16];
			hAccTable = dword_5968F8;
			input_procedures[16] = input_standardSysHotkeys;
			RefreshCursor_0();
			PauseGame_maybe();
			dword_6509AC->container.dwExecutionFlags |= 0x20;
			byte_6509B4 = 1;
		}
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_PauseGame);

int __fastcall TriggerAction_UnpauseGame_(Action* a1)
{
	if (!multiPlayerMode && byte_6509B4)
	{
		PlayerType player_type = Players[active_trigger_player].nType;
		if (player_type != PT_Computer && player_type != PT_Rescuable && player_type != PT_Unknown0 && player_type != PT_Neutral)
		{
			dword_6509AC->container.dwExecutionFlags &= ~0x20;
			byte_6509B4 = 0;
			if (IS_GAME_PAUSED)
			{
				IS_GAME_PAUSED = 0;
				if (InReplay && !dword_6D5BE8)
				{
					SetGameSpeed_maybe(registry_options.GameSpeed, 0, replay_speed_multiplier);
				}
			}
			cursorRefresh();
			if (has_viewport)
			{
				_SetCursorPos(GAME_AREA_WIDTH / 2, GAME_AREA_HEIGHT / 2);
			}
			hAccTable = DlgAccelerator;
			input_procedures[EVN_SYSCHAR] = AcceleratorTables;
		}
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_UnpauseGame);

unsigned int getTextDisplayTime_(const char* text)
{
	if (text == NULL)
	{
		return 0;
	}

	return max(strlen(text) * 50, 4000);
}

FAIL_STUB_PATCH(getTextDisplayTime);

int __fastcall TriggerAction_PlayWav_(Action* a1)
{
	char buff[260];

	if (!InReplay && active_trigger_player == g_LocalNationID && a1->wavString && (dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		const char* chk_string = get_chk_String(a1->wavString);
		if (active_campaign)
		{
			sprintf_s(buff, "%s\\%s", MapdataFilenames_[CampaignIndex], chk_string);
		}
		else
		{
			strcpy_s(buff, chk_string);
		}
		PlayWavByFilename_maybe(buff);
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_PlayWav);

int DisplayTalkingPortrait_maybe_(int a2, UnitType unit_type, int x, int y)
{
	dword_68AC4C = 1;
	SetCallbackTimer(4, dword_68AC98, a2, GameTalkingPortrait_CB);

	WORD portrait = setBuildingSelPortrait_(unit_type);
	displayUpdatePortrait(portrait, 0, 2);
	SetCallbackTimer(3, dword_68AC98, a2, sub_45EC40_);
	if (x != -1)
	{
		dword_57FD34 = (x - GAME_AREA_WIDTH / 2) & ((x - GAME_AREA_WIDTH / 2 < 0) - 1);
		dword_57FD38 = (y - GAME_AREA_HEIGHT / 2) & ((y - GAME_AREA_HEIGHT / 2 < 0) - 1);
	}
	return 1;
}

FAIL_STUB_PATCH(DisplayTalkingPortrait_maybe);

void __fastcall sub_45EF50_(dialog* dlg, __int16 a2)
{
	if (a2 == 3)
	{
		unsigned v2 = 1000 * (stru_6D1270[dword_68ACA0].anonymous_2 / 1000);
		if (v2 <= 1000)
		{
			dword_68AC60 = 0;
			waitLoopCntd(3, dlg);
			if (!ActivePortraitUnit)
			{
				dword_68AC98->lUser = 0;
				dword_68AC98->wUser = -1;
				HideDialog(dword_68AC98);
			}
		}
		else
		{
			SetCallbackTimer(3, dword_68AC98, v2 - 1000, sub_45EC40_);
		}
	}
}

FAIL_STUB_PATCH(sub_45EF50);

void DoUnitEventNotify_(CUnit* unit, char a2, int a3, int* a4, unsigned int a5)
{
	if (!dword_68AC4C)
	{
		WORD portrait = setBuildingSelPortrait_(getLastQueueSlotType(unit));
		displayUpdatePortrait(portrait, unit, 2);
		if (a3 || a5 >= 1144)
		{
			SetCallbackTimer(3, dword_68AC98, a3, sub_45EC40_);
		}
		else
		{
			dword_68ACA0 = a5;
			SetCallbackTimer(3, dword_68AC98, 1000, sub_45EF50_);
		}

		if (unit && a4)
		{
			setLastEventPosition((__int16)unit->sprite->position.x, (__int16)unit->sprite->position.y);
			unit->lastEventTimer = 8;
			unit->lastEventColor = a2;
			MinimapPing_maybe_(unit->sprite->position.x, unit->sprite->position.y, a2);
		}
	}
}

void __stdcall DoUnitEventNotify__(int* a4, unsigned int a5)
{
	CUnit* unit;
	char a2;
	int a3;

	__asm mov unit, eax;
	__asm mov a2, bl;
	__asm mov a3, edi;

	DoUnitEventNotify_(unit, a2, a3, a4, a5);
}

FUNCTION_PATCH((void*)0x45EFE0, DoUnitEventNotify__);

int __fastcall TriggerAction_Transmission_(Action* a1)
{
	if (a1->location == 0)
	{
		return 1;
	}

	PlayerType player_type = Players[active_trigger_player].nType;
	if (player_type == PlayerType::PT_Computer || player_type == PlayerType::PT_Rescuable || player_type == PlayerType::PT_Unknown0 || player_type == PlayerType::PT_Neutral)
	{
		return 1;
	}
	if (!byte_6509B8[active_trigger_player])
	{
		if (a1->flags & 1)
		{
			a1->flags &= ~1;
			return 1;
		}
		if (dword_6509AC->container.dwExecutionFlags & 0x10)
		{
			return 1;
		}

		unsigned int v7;
		switch (a1->number2)
		{
		case 7:
			v7 = a1->number;
			break;
		case 8:
			v7 = a1->time + a1->number;
			break;
		case 9:
			v7 = max(a1->time - a1->number, 0);
			break;
		default:
			v7 = a1->time;
		}

		a1->flags |= 1;
		dword_650980[active_trigger_player] = multiPlayerMode ? v7 : -1;
		byte_6509B8[active_trigger_player] = 1;
		if (active_trigger_player == g_LocalNationID)
		{
			TriggerAction_PlayWav_(a1);
			CUnit* unit = getUnitForDoodadState(a1->location - 1, a1->unit, 17);
			if (unit)
			{
				unit->sprite->selectionTimer = 45;
				MinimapPing_maybe_(unit->sprite->position.x, unit->sprite->position.y, 17);
				DisplayTalkingPortrait_maybe_(v7, (UnitType) a1->unit, unit->sprite->position.x, unit->sprite->position.y);
			}
			else
			{
				DisplayTalkingPortrait_maybe_(v7, (UnitType) a1->unit, -1, -1);
			}
			if ((registry_options.field_18 & 0x400) || (a1->flags & 4))
			{
				const char* text_message = get_chk_String(a1->string);
				unsigned display_time = max(v7, getTextDisplayTime_(text_message));
				createTextMessageWithTimer(text_message, display_time);
			}
		}
	}
	return 0;
}

FAIL_STUB_PATCH(TriggerAction_Transmission);

int __fastcall TriggerAction_SetMissionObjectives_(Action* a1)
{
	MissionObjectives[active_trigger_player] = a1->string;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetMissionObjectives);

int __fastcall TriggerAction_DisplayTextMessage_(Action* a1)
{
	const char* text_message;

	if (active_trigger_player == g_LocalNationID && ((registry_options.field_18 & 0x400) || (a1->flags & 4)) && (dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		text_message = GetMapTblString(a1->string);

		if (text_message)
		{
			int display_time = getTextDisplayTime_(text_message);
			PrintText(text_message, 2u, display_time + GetTickCount(), 1);
		}
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_DisplayTextMessage);

int __fastcall TriggerAction_CenterView_(Action* a1)
{
	if (a1->location == 0)
	{
		return 1;
	}
	if (IS_GAME_PAUSED)
	{
		return 1;
	}

	PlayerType player_type = Players[active_trigger_player].nType;
	if (player_type == PT_Computer || player_type == PT_Rescuable || player_type == PT_Unknown0 || player_type == PT_Neutral)
	{
		return 1;
	}
	if (multiPlayerMode)
	{
		if (active_trigger_player == g_LocalNationID)
		{
			BWFXN_MoveScreen(
				(LocationTable[a1->location - 1].dimensions.left + LocationTable[a1->location - 1].dimensions.right - GAME_AREA_WIDTH) / 2,
				(LocationTable[a1->location - 1].dimensions.top + LocationTable[a1->location - 1].dimensions.bottom - GAME_AREA_HEIGHT) / 2);
		}
		return 1;
	}
	if (!byte_6509B8[active_trigger_player])
	{
		if (a1->flags & 1)
		{
			a1->flags = a1->flags & 0xFE;
			CenterCursorGameScreen();
			return 1;
		}
		if (dword_6509AC->container.dwExecutionFlags & 0x10)
		{
			return 1;
		}
		if (active_trigger_player == g_LocalNationID)
		{
			Box32* dimensions = &LocationTable[a1->location - 1].dimensions;

			int v11 = std::clamp((dimensions->left + dimensions->right - GAME_AREA_WIDTH) / 2, 0, map_width_pixels - GAME_AREA_WIDTH - 1);
			int v12 = std::clamp((dimensions->top + dimensions->bottom - GAME_AREA_HEIGHT) / 2, 0, map_height_pixels - GAME_AREA_HEIGHT - 1);

			assignCenterViewProc(v12, v11, defCenterViewProc);
		}
		a1->flags |= 1;
		byte_6509B8[active_trigger_player] = 1;
		dword_650980[active_trigger_player] = -1;
	}
	return 0;
}

FAIL_STUB_PATCH(TriggerAction_CenterView);

void EnableSwitch(unsigned index)
{
	TriggerSwitches[index / 32] |= 1 << (index % 32);
}

void DisableSwitch(unsigned index)
{
	TriggerSwitches[index / 32] &= ~(1 << (index % 32));
}

void ToggleSwitch(unsigned index)
{
	TriggerSwitches[index / 32] ^= 1 << (index % 32);
}

int __fastcall TriggerAction_SetSwitch_(Action* a1)
{
	unsigned int v5;

	if (a1->number >= 256)
	{
		return 1;
	}
	switch (a1->number2)
	{
	case 0xBu:
		if (IsInGameLoop)
		{
			++dword_51C6F4;
			++randomCountsTotal;
			LastRandomNumber = 22695477 * LastRandomNumber + 1;
			v5 = (unsigned)(LastRandomNumber) >> 16;
		}
		else
		{
			v5 = 0;
		}
		if ((v5 & 0x80u) == 0)
		{
			DisableSwitch(a1->number);
		}
		else
		{
			EnableSwitch(a1->number);
		}
		return 1;
	case 4u:
		EnableSwitch(a1->number);
		return 1;
	case 5u:
		DisableSwitch(a1->number);
		return 1;
	case 6u:
		ToggleSwitch(a1->number);
		return 1;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetSwitch);

int __fastcall TriggerAction_SetCountdownTimer_(Action* a1)
{
	switch (a1->number2)
	{
	case 7u:
		CountdownTimer = a1->time;
		break;
	case 8u:
		CountdownTimer = CountdownTimer + a1->time;
		break;
	case 9u:
		CountdownTimer = CountdownTimer - a1->time;
		break;
	}

	if (CountdownTimer < 0) CountdownTimer = 0;
	if (CountdownTimer > 359999) CountdownTimer = 359999;

	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetCountdownTimer);

int __fastcall TriggerAction_RunAiScript_(Action* a1)
{
	if (isAIScriptNameValid(active_trigger_player, a1->number))
	{
		int v5;
		int v3 = ParseAIScriptName(a1->number, &v5);
		if (v3)
		{
			AI_RunAIScript(0, active_trigger_player, v3, v5);
		}
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_RunAiScript);

int __fastcall TriggerAction_RunAiScriptAtLocation_(Action* a1)
{
	if (a1->location)
	{
		if (isAIScriptNameValid(active_trigger_player, a1->number))
		{
			int v5 = 0;
			int v3 = ParseAIScriptName(a1->number, &v5);
			if (v3)
			{
				AI_RunAIScript(&LocationTable[a1->location - 1], active_trigger_player, v3, v5);
			}
		}
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_RunAiScriptAtLocation);

int __fastcall TriggerAction_LeaderBoard_(Action* a1)
{
	word_65097C = 1;
	if (a1->action == 40)
	{
		byte_58D70C = a1->action;
		word_58D70E = 2;
		dword_58D710 = a1->number;
	}
	else if (a1->action != 18 && a1->action != 34)
	{
		byte_58D70C = a1->action;
		word_58D70E = a1->unit;
		dword_58D710 = a1->number;
		dword_58D714 = a1->string;
	}
	else if (a1->location)
	{
		byte_58D70D = a1->location - 1;
		byte_58D70C = a1->action;
		word_58D70E = a1->unit;
		dword_58D710 = a1->number;
		dword_58D714 = a1->string;
	}
	return 1;
}

FUNCTION_PATCH(TriggerAction_LeaderBoard, TriggerAction_LeaderBoard_);

int __fastcall TriggerAction_LeaderBoardComputerPlayers_(Action* a1)
{
	switch (a1->number2)
	{
	case 4:
		dword_58D708 = 1;
		break;
	case 5:
		dword_58D708 = 0;
		break;
	case 6:
		dword_58D708 ^= 1;
		break;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_LeaderBoardComputerPlayers);

int __fastcall TriggerAction_KillUnit_(Action* a1)
{
	dword_5971DC = 0;
	DestroyUnit_maybe(a1->player, a1->unit, 255, 999999);
	byte_685180 = 0;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_KillUnit);

int __fastcall TriggerAction_KillUnitAtLocation_(Action* a1)
{
	if (a1->location)
	{
		dword_5971DC = 0;
		DestroyUnit_maybe(a1->player, a1->unit, a1->location - 1, a1->number2 ? a1->number2 : 999999);
		byte_685180 = 0;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_KillUnitAtLocation);

int __fastcall TriggerAction_RemoveUnit_(Action* a1)
{
	dword_5971DC = 1;
	DestroyUnit_maybe(a1->player, a1->unit, 255, 999999);
	byte_685180 = 0;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_RemoveUnit);

int __fastcall TriggerAction_RemoveUnitAtLocation_(Action* a1)
{
	if (a1->location)
	{
		dword_5971DC = 1;
		DestroyUnit_maybe(a1->player, a1->unit, a1->location - 1, a1->number2 ? a1->number2 : 999999);
		byte_685180 = 0;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_RemoveUnitAtLocation);

int __fastcall TriggerAction_SetResources_(Action* a1)
{
	switch (a1->number2)
	{
	case 7u:
		SetResource(a1->player, a1->unit, a1->number);
		return 1;
	case 8u:
		AddResource(a1->player, a1->unit, a1->number);
		return 1;
	case 9u:
		SubtractResource(a1->player, a1->unit, a1->number);
		return 1;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetResources);

int __fastcall TriggerAction_SetScore_(Action* a1)
{
	switch (a1->number2)
	{
	case 7u:
		SetScore(a1->player, a1->unit, a1->number);
		return 1;
	case 8u:
		AddScore(a1->player, a1->unit, a1->number);
		return 1;
	case 9u:
		SubtractScore(a1->player, a1->unit, a1->number);
		return 1;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetScore);

int __fastcall TriggerAction_MinimapPing_(Action* a1)
{
	if (a1->location && active_trigger_player == g_LocalNationID && (dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		Box32& dimensions = LocationTable[a1->location - 1].dimensions;
		MinimapPing_maybe_((dimensions.left + dimensions.right) / 2, (dimensions.top + dimensions.bottom) / 2, 17);
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_MinimapPing);

int __fastcall TriggerAction_TalkingPortrait_(Action* a1)
{
	if (active_trigger_player == g_LocalNationID && (dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		DisplayTalkingPortrait_maybe_(a1->time, (UnitType) a1->unit, -1, -1);
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_TalkingPortrait);

int __fastcall TriggerAction_MuteUnitSpeech_(Action* a1)
{
	if (active_trigger_player == g_LocalNationID && (dword_6509AC->container.dwExecutionFlags & 0x10) == 0)
	{
		MuteUnitSpeech_maybe();
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_MuteUnitSpeech);

int __fastcall TriggerAction_UnmuteUnitSpeech_(Action* a1)
{
	if (active_trigger_player == g_LocalNationID)
	{
		dword_64086C = 0;
		if (byte_6D5BBD)
		{
			byte_6D5BBD = 0;
			if (directsound)
			{
				SFileDdaSetVolume(directsound, bigvolume, 0);
			}
		}
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_UnmuteUnitSpeech);

int __fastcall TriggerAction_SetNextScenario_(Action* a1)
{
	if (a1->string)
	{
		strcpy_s(next_scenario, GetMapTblString(a1->string));
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetNextScenario);

int __fastcall TriggerAction_SetDoodadState_(Action* a1)
{
	if (a1->location == 0)
	{
		return 1;
	}

	CUnit* unit = getUnitForDoodadState(a1->location - 1, a1->unit, a1->player);
	if (!unit)
	{
		return 1;
	}
	switch (a1->number2)
	{
	case 0u:
	case 6u:
		ToggleDoodadState(unit);
		return 1;
	case 4u:
		EnableDoodadState(unit);
		return 1;
	case 5u:
		DisableDoodadState(unit);
		return 1;
	default:
		return 1;
	}
}

FAIL_STUB_PATCH(TriggerAction_SetDoodadState);

int __fastcall TriggerAction_SetDeaths_(Action* a1)
{
	switch (a1->number2)
	{
	case 7u:
		SetDeaths(a1->player, a1->unit, a1->number);
		return 1;
	case 8u:
		AddDeaths(a1->player, a1->unit, a1->number);
		return 1;
	case 9u:
		SubtractDeaths(a1->player, a1->unit, a1->number);
		return 1;
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetDeaths);

int __fastcall TriggerAction_PauseTimer_(Action* a1)
{
	TimerIsPaused = 1;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_PauseTimer);

int __fastcall TriggerAction_UnpauseTimer_(Action* a1)
{
	TimerIsPaused = 0;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_UnpauseTimer);

int __fastcall TriggerAction_Draw_(Action* a1)
{
	if (endgame_state[active_trigger_player] == EndgameState::VICTORY || endgame_state[active_trigger_player] == EndgameState::UNKNOWN)
	{
		return 0;
	}
	if (Players[active_trigger_player].nType == PT_Human || Players[active_trigger_player].nType == PT_Computer)
	{
		endgame_state[active_trigger_player] = EndgameState::DRAW;
	}
	word_650970 = byte_6509B4 ? 1 : 45;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_Draw);

int __fastcall TriggerAction_SetAllianceStatus_(Action* a1)
{
	SetAlliance_maybe(a1->player, a1->unit, 0);
	StopAttackingAllies_maybe(active_trigger_player);
	if (active_trigger_player == g_LocalNationID)
	{
		setAlliance();
	}
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_SetAllianceStatus);

int __fastcall TriggerAction_DisableDebugMode_(Action* a1)
{
	dword_6509AC->container.dwExecutionFlags |= 0x40;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_DisableDebugMode);

int __fastcall TriggerAction_EnableDebugMode_(Action* a1)
{
	dword_6509AC->container.dwExecutionFlags &= ~0x40;
	return 1;
}

FAIL_STUB_PATCH(TriggerAction_EnableDebugMode);

#include <sol/sol.hpp>

void lua_print(const char* message)
{
	if (active_trigger_player == g_LocalNationID && message)
	{
		int display_time = getTextDisplayTime_(message);
		PrintText(message, 2, display_time + GetTickCount(), 1);
	}
}

int __fastcall TriggerAction_ExecuteLua(Action* a1)
{
	const char* script = GetMapTblString(a1->string);

	sol::state lua;
	lua.set_function("print", lua_print);
	lua.script(script);

	return 1;
}

ActionPointer ActionTable_[] = {
	TriggerAction_NoAction_,
	TriggerAction_Victory_,
	TriggerAction_Defeat_,
	TriggerAction_PreserveTrigger_,
	TriggerAction_Wait_,
	TriggerAction_PauseGame_,
	TriggerAction_UnpauseGame_,
	TriggerAction_Transmission_,
	TriggerAction_PlayWav_,
	TriggerAction_DisplayTextMessage_,
	TriggerAction_CenterView_,
	TriggerAction_CreateUnitWithProperties,
	TriggerAction_SetMissionObjectives_,
	TriggerAction_SetSwitch_,
	TriggerAction_SetCountdownTimer_,
	TriggerAction_RunAiScript_,
	TriggerAction_RunAiScriptAtLocation_,
	TriggerAction_LeaderBoard_, // Control / Text, TUnit
	TriggerAction_LeaderBoard_, // Control At Location / Text, TUnit, Loc
	TriggerAction_LeaderBoard_, // Resources / Text, ResType
	TriggerAction_LeaderBoard_, // Kills / Text, TUnit
	TriggerAction_LeaderBoard_, // Points / Text, Score
	TriggerAction_KillUnit_,
	TriggerAction_KillUnitAtLocation_,
	TriggerAction_RemoveUnit_,
	TriggerAction_RemoveUnitAtLocation_,
	TriggerAction_SetResources_,
	TriggerAction_SetScore_,
	TriggerAction_MinimapPing_,
	TriggerAction_TalkingPortrait_,
	TriggerAction_MuteUnitSpeech_,
	TriggerAction_UnmuteUnitSpeech_,
	TriggerAction_LeaderBoardComputerPlayers_,
	TriggerAction_LeaderBoard_, // Control / Text, TUnit, Number
	TriggerAction_LeaderBoard_, // Control At Location / Text, TUnit, Number, Loc
	TriggerAction_LeaderBoard_, // Resources / Text, TUnit, Number, ResType
	TriggerAction_LeaderBoard_, // Kills / Text, TUnit, Number
	TriggerAction_LeaderBoard_, // Points / Text, Number, Score
	TriggerAction_MoveLocation,
	TriggerAction_MoveUnit,
	TriggerAction_LeaderBoard_,
	TriggerAction_SetNextScenario_,
	TriggerAction_SetDoodadState_,
	TriggerAction_SetInvincibility,
	TriggerAction_CreateUnitWithProperties,
	TriggerAction_SetDeaths_,
	TriggerAction_Order,
	TriggerAction_NoAction_, // Comment
	TriggerAction_GiveUnitsToPlayer,
	TriggerAction_ModifyUnitHitPoints,
	TriggerAction_ModifyUnitEnergy,
	TriggerAction_ModifyUnitShieldPoints,
	TriggerAction_ModifyUnitResourceAmount,
	TriggerAction_ModifyUnitHangarCount,
	TriggerAction_PauseTimer_,
	TriggerAction_UnpauseTimer_,
	TriggerAction_Draw_,
	TriggerAction_SetAllianceStatus_,
	TriggerAction_DisableDebugMode_,
	TriggerAction_EnableDebugMode_,
	// TriggerAction_ExecuteLua,
};

void ExecuteTriggerActions_(TriggerListEntry* a1)
{
	a1->container.dwExecutionFlags = a1->container.dwExecutionFlags | 1;
	if (a1->container.dwExecutionFlags & 2)
	{
		endgame_state[active_trigger_player] = EndgameState::UNKNOWN;
	}
	while (1)
	{
		if (a1->container.bCurrentActionIndex >= 64)
		{
			break;
		}
		Action* action = a1->container.actions + a1->container.bCurrentActionIndex;
		if ((action->flags & 2) == 0)
		{
			if (action->action == 0)
			{
				a1->container.bCurrentActionIndex = 64;
				break;
			}
			if (ActionTable_[action->action](action) == 0)
			{
				break;
			}
		}
		a1->container.bCurrentActionIndex += 1;
	}
	if (a1->container.bCurrentActionIndex >= 64)
	{
		if ((a1->container.dwExecutionFlags & 0x20) != 0)
		{
			ActionTable_[6](0);
		}
		if (a1->container.dwExecutionFlags & 4)
		{
			a1->container.dwExecutionFlags = a1->container.dwExecutionFlags & ~0x51;
			a1->container.bCurrentActionIndex = 0;
		}
		else
		{
			a1->container.dwExecutionFlags = a1->container.dwExecutionFlags | 8;
		}
	}
}

FAIL_STUB_PATCH(ExecuteTriggerActions);

void executeGameTrigger_(TriggerList* a1)
{
	if (a1->begin <= 0)
	{
		return;
	}

	TriggerListEntry* entry = a1->begin;

	for (TriggerListEntry* entry = a1->begin; (int)entry > 0; entry = entry->next)
	{
		if ((entry->container.dwExecutionFlags & 8) == 0)
		{
			dword_6509AC = entry;
			if (entry->container.dwExecutionFlags & 1)
			{
				ExecuteTriggerActions_(entry);
			}
			else if (ExecuteTriggerConditions(entry))
			{
				entry->container.bCurrentActionIndex = 0;
				ExecuteTriggerActions_(entry);
			}
		}
	}
}

FAIL_STUB_PATCH(executeGameTrigger);

void BWFXN_ExecuteGameTriggers_(signed int dwMillisecondsPerFrame)
{
	if (!IS_GAME_PAUSED || byte_6509B4)
	{
		load_endmission_();
		countdownTimersExecute_(dwMillisecondsPerFrame);
		if (word_6509A0-- == 0)
		{
			memset(endgame_state, EndgameState::INITIAL, 8);
			word_6509A0 = 30;
			byte_685180 = 0;
			bool trigger_has_executed = false;
			unsigned player_related = -1;

			while (true)
			{
				active_trigger_player = getNextActivePlayer(&player_related);

				if (active_trigger_player >= 8)
				{
					break;
				}

				if ((int)stru_51A280[active_trigger_player].begin > 0)
				{
					executeGameTrigger_(stru_51A280 + active_trigger_player);
					trigger_has_executed = true;
				}
			}

			if (trigger_has_executed)
			{
				endgameCheck();
			}
		}
	}
}

FAIL_STUB_PATCH(BWFXN_ExecuteGameTriggers);

ButtonState CanUseTech_(CUnit* unit, Tech2 tech_id, int player_id)
{
	dword_66FF60 = 0;
	if (tech_id >= TECH_none)
	{
		dword_66FF60 = 17;
		return ButtonState::BTNST_HIDDEN;
	}

	if (Tech_Unknown2[tech_id] == -1)
	{
		parseTechUseData();
	}
	if (player_id != unit->playerID)
	{
		dword_66FF60 = 1;
		return ButtonState::BTNST_HIDDEN;
	}

	if ((unit->statusFlags & Completed) == 0)
	{
		dword_66FF60 = 20;
		return ButtonState::BTNST_HIDDEN;
	}
	if (UnitIsFrozenOrInAir(unit))
	{
		dword_66FF60 = 10;
		return ButtonState::BTNST_HIDDEN;
	}
	if (unit->statusFlags & IsHallucination)
	{
		dword_66FF60 = 22;
		return ButtonState::BTNST_HIDDEN;
	}
	if (techIsResearchedSCBW(player_id, tech_id) || (GameCheats & CheatFlags::CHEAT_MedievalMan))
	{
		ButtonState button_state = (ButtonState)(u16)Tech_Unknown2[tech_id];
		if (button_state)
		{
			return parseRequirementOpcodes(button_state, unit, tech_id, player_id, (int)word_514A48);
		}
		else
		{
			dword_66FF60 = 23;
			return ButtonState::BTNST_HIDDEN;
		}
	}

	dword_66FF60 = 2;
	return ButtonState::BTNST_HIDDEN;
}

ButtonState __stdcall CanUseTech__(int player_id)
{
	CUnit* unit;
	Tech2 tech_id;

	__asm mov unit, eax
	__asm mov tech_id, di

	return CanUseTech_(unit, tech_id, player_id);
}

FUNCTION_PATCH((void*)0x46DD80, CanUseTech__);

void BWFXN_sendTurn_()
{
	if (sgdwBytesInCmdQueue == 0)
	{
		TurnBuffer[0] = 5;
		sgdwBytesInCmdQueue = 1;
	}
	if (!SNetSendTurn(TurnBuffer, sgdwBytesInCmdQueue) && !outOfGame)
	{
		packetErrHandle_(SErrGetLastError(), 91, 0, 0, 1);
	}
	sgdwBytesInCmdQueue = 0;
	if (InGame)
	{
		CMDACT_GameHash();
	}
}

FAIL_STUB_PATCH(BWFXN_sendTurn);

void __fastcall BWFXN_QueueCommand_(const void* buffer, unsigned int buffer_size)
{
	if (buffer_size + sgdwBytesInCmdQueue > MaxTurnSize)
	{
		if (gwGameMode == GAME_GLUES)
		{
			return;
		}

		int turns;
		if (SNetGetTurnsInTransit(&turns))
		{
			if (turns + LatencyCalls >= 16)
			{
				return;
			}
			BWFXN_sendTurn_();
		}
		else
		{
			if (!outOfGame)
			{
				packetErrHandle_(SErrGetLastError(), 81, "Starcraft\\SWAR\\lang\\net_mgr.cpp", 225, 1);
			}
			return;
		}
	}

	memcpy(&TurnBuffer[sgdwBytesInCmdQueue], buffer, buffer_size);
	sgdwBytesInCmdQueue += buffer_size;
}

FUNCTION_PATCH(BWFXN_QueueCommand, BWFXN_QueueCommand_);

void CMDACT_RightClick_(dlgEvent* dlg)
{
	if (ActivePortraitUnit && ActivePortraitUnit->playerID == g_LocalNationID)
	{
		if (std::none_of(ClientSelectionGroup, ClientSelectionGroup + _countof(ClientSelectionGroup), [](CUnit* unit) { return unit && DoesAcceptRclickCommands(unit); }))
		{
			return;
		}

		if (std::none_of(ClientSelectionGroup, ClientSelectionGroup + _countof(ClientSelectionGroup), [](CUnit* unit) { return unit && CanRClickGround_maybe(unit); }))
		{
			return;
		}

		int x = dlg->cursor.x;
		int y = dlg->cursor.y;
		getMinimapCursorPos_(&x, &y);

		RightClickCommand command;
		command.command_id = CommandId::CMD_RightClick;
		command.x = x;
		command.y = y;
		command.target_id = 0;
		command.unit_type = UnitType::UT_MAX;
		command.is_shift_used = is_keycode_used[VK_SHIFT];
		BWFXN_QueueCommand__(command);
		PlayWorkerActionSound(ActivePortraitUnit);
		GroundAttackInit_(x, y);
	}
}

FAIL_STUB_PATCH(CMDACT_RightClick);

ButtonState __fastcall BTNSCOND_ReplayPlayPause_(u16 variable, int player_id, CUnit* unit)
{
	if (InReplay && getActivePlayerId() == playerid)
	{
		return ButtonState::BTNST_ENABLED;
	}
	else
	{
		return ButtonState::BTNST_HIDDEN;
	}
}

FUNCTION_PATCH(BTNSCOND_ReplayPlayPause, BTNSCOND_ReplayPlayPause_);

void CMDACT_SetReplaySpeed_()
{
	ReplaySpeedCommand command;
	command.game_speed = registry_options.GameSpeed;
	command.replay_speed_multiplier = replay_speed_multiplier;
	command.command_id = CommandId::CMD_ReplaySpeed;
	command.is_paused = !is_replay_paused;

	BWFXN_QueueCommand__(command);
}

FUNCTION_PATCH(CMDACT_SetReplaySpeed, CMDACT_SetReplaySpeed_);

const int MAX_REPLAY_SPEED = 1 << 16;

ButtonState __fastcall BTNSCOND_ReplaySlowDown_(u16 variable, int player_id, CUnit* unit)
{
	if (!InReplay || getActivePlayerId() != playerid)
	{
		return BTNST_HIDDEN;
	}
	else if (!registry_options.GameSpeed && replay_speed_multiplier >= 1 || is_replay_paused)
	{
		return BTNST_DISABLED;
	}
	else
	{
		return BTNST_ENABLED;
	}
}

FUNCTION_PATCH(BTNSCOND_ReplaySlowDown, BTNSCOND_ReplaySlowDown_);

void __fastcall CMDACT_DecreaseReplaySpeed_(int a1, bool a2)
{
	if (is_replay_paused)
	{
		return;
	}

	ReplaySpeedCommand command;
	command.command_id = CommandId::CMD_ReplaySpeed;
	command.is_paused = 0;
	command.game_speed = registry_options.GameSpeed;
	command.replay_speed_multiplier = replay_speed_multiplier;

	if (registry_options.GameSpeed == 6)
	{
		if (replay_speed_multiplier > 1)
		{
			command.replay_speed_multiplier /= 2;
		}
		else
		{
			command.game_speed -= 1;
		}
	}
	else
	{
		if (registry_options.GameSpeed)
		{
			command.game_speed -= 1;
		}
		else if (replay_speed_multiplier < 1)
		{
			command.replay_speed_multiplier *= 2;
		}
	}

	BWFXN_QueueCommand__(command);
}

FUNCTION_PATCH(CMDACT_DecreaseReplaySpeed, CMDACT_DecreaseReplaySpeed_);

ButtonState __fastcall BTNSCOND_ReplaySpeedUp_(u16 variable, int player_id, CUnit* unit)
{
	if (!InReplay || getActivePlayerId() != playerid)
	{
		return BTNST_HIDDEN;
	}
	else if (registry_options.GameSpeed == 6 && replay_speed_multiplier >= MAX_REPLAY_SPEED || is_replay_paused)
	{
		return BTNST_DISABLED;
	}
	else
	{
		return BTNST_ENABLED;
	}
}

FUNCTION_PATCH(BTNSCOND_ReplaySpeedUp, BTNSCOND_ReplaySpeedUp_);

void __fastcall CMDACT_IncreaseReplaySpeed_(int a1, bool a2)
{
	if (is_replay_paused)
	{
		return;
	}

	ReplaySpeedCommand command;
	command.command_id = CommandId::CMD_ReplaySpeed;
	command.is_paused = 0;
	command.game_speed = registry_options.GameSpeed;
	command.replay_speed_multiplier = replay_speed_multiplier;

	if (registry_options.GameSpeed)
	{
		if (registry_options.GameSpeed < 6)
		{
			command.game_speed += 1;
		}
		else if (replay_speed_multiplier < MAX_REPLAY_SPEED)
		{
			command.replay_speed_multiplier *= 2;
		}
	}
	else
	{
		if (replay_speed_multiplier > 1)
		{
			command.replay_speed_multiplier /= 2;
		}
		else
		{
			command.game_speed += 1;
		}
	}

	BWFXN_QueueCommand__(command);
}

FUNCTION_PATCH(CMDACT_IncreaseReplaySpeed, CMDACT_IncreaseReplaySpeed_);

ButtonState __fastcall BTNSCOND_Always_(u16 variable, int player_id, CUnit* unit)
{
	return ButtonState::BTNST_ENABLED;
}

FUNCTION_PATCH(BTNSCOND_Always, BTNSCOND_Always_);

ButtonState __fastcall BTNSCOND_Rally_(u16 variable, int player_id, CUnit* unit)
{
	dword_66FF60 = 0;
	if (Orders_Unknown17[40] == -1)
	{
		parseOrdersDatReqs();
	}

	if (player_id != unit->playerID)
	{
		dword_66FF60 = 1;
		return BTNST_HIDDEN;
	}
	else if ((unit->statusFlags & StatusFlags::DoodadStatesThing) || unit->status.lockdownTimer || unit->status.stasisTimer || unit->status.maelstromTimer)
	{
		dword_66FF60 = 10;
		return BTNST_HIDDEN;
	}
	else if ((unit->statusFlags & StatusFlags::Burrowed) && unit->unitType != Zerg_Lurker)
	{
		dword_66FF60 = 11;
		return BTNST_HIDDEN;
	}
	else if (unit->unitType == Terran_SCV && unit->orderID == Order::ORD_CONSTRUCT_BLDG
		|| UnitIsGhost(unit) && unit->orderID == Order::ORD_NUKE_TRACK
		|| unit->unitType == Protoss_Archon && unit->orderID == Order::ORD_SUMMON_COMPLETE
		|| unit->unitType == Protoss_Dark_Archon && unit->orderID == Order::ORD_SUMMON_COMPLETE)
	{
		dword_66FF60 = 8;
		return BTNST_HIDDEN;
	}
	else if (Orders_Unknown17[40])
	{
		return (ButtonState) parseRequirementOpcodes((unsigned __int16)Orders_Unknown17[40], unit, TECH2_unknown_tech40, player_id, (int)word_514CF8);
	}
	else
	{
		dword_66FF60 = 23;
		return (ButtonState) Orders_Unknown17[40];
	}
}

FUNCTION_PATCH(BTNSCOND_Rally, BTNSCOND_Rally_);

ButtonState __fastcall BTNSCOND_IsConstructing_(u16 variable, int player_id, CUnit* unit)
{
	return (unit->statusFlags & StatusFlags::Completed) ? ButtonState::BTNST_HIDDEN : ButtonState::BTNST_ENABLED;
}

FUNCTION_PATCH(BTNSCOND_IsConstructing, BTNSCOND_IsConstructing_);

ButtonState __fastcall BTNSCOND_HatcheryLairHive_(u16 variable, int player_id, CUnit* unit)
{
	switch (unit->unitType)
	{
	case UnitType::Zerg_Hatchery:
	case UnitType::Zerg_Lair:
	case UnitType::Zerg_Hive:
		return ButtonState::BTNST_ENABLED;
	default:
		return ButtonState::BTNST_HIDDEN;
	}
}

FUNCTION_PATCH(BTNSCOND_HatcheryLairHive, BTNSCOND_HatcheryLairHive_);

ButtonState __fastcall BTNSCOND_HatcheryLairHiveRally_(u16 variable, int player_id, CUnit* unit)
{
	switch (unit->unitType)
	{
	case UnitType::Zerg_Hatchery:
	case UnitType::Zerg_Lair:
	case UnitType::Zerg_Hive:
		return BTNSCOND_Rally_(variable, player_id, unit);
	default:
		return ButtonState::BTNST_HIDDEN;
	}
}

FUNCTION_PATCH(BTNSCOND_HatcheryLairHiveRally, BTNSCOND_HatcheryLairHiveRally_);

ButtonState __fastcall BTNSCOND_CanMove_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		CUnit* unit = ClientSelectionGroup[i];
		if (ClientSelectionGroup[i] == NULL)
		{
			continue;
		}
		else if (unit->unitType == Zerg_Lurker && (unit->statusFlags & Burrowed))
		{
			continue;
		}
		else if (Unit_RightClickAction[unit->unitType] == 3)
		{
			continue;
		}
		else if (Unit_RightClickAction[unit->unitType] != 0)
		{
			return ButtonState::BTNST_ENABLED;
		}
		else if ((unit->statusFlags & GoundedBuilding) && Unit_IsFactoryBuilding(unit))
		{
			return ButtonState::BTNST_ENABLED;
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_CanMove, BTNSCOND_CanMove_);

ButtonState __fastcall BTNSCOND_CanMoveSpecialCase_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		CUnit* unit = ClientSelectionGroup[i];
		if (ClientSelectionGroup[i] == NULL)
		{
			continue;
		}
		else if (unit->unitType == Zerg_Lurker && (unit->statusFlags & Burrowed))
		{
			return ButtonState::BTNST_ENABLED;
		}
		else if (Unit_RightClickAction[unit->unitType] || (unit->statusFlags & GoundedBuilding) != 0 && Unit_IsFactoryBuilding(unit))
		{
			return ButtonState::BTNST_ENABLED;
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_CanMoveSpecialCase, BTNSCOND_CanMoveSpecialCase_);

ButtonState __fastcall BTNSCOND_CanAttack_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* v4 = ClientSelectionGroup[i])
		{
			if (v4->unitType == Zerg_Lurker && (v4->statusFlags & Burrowed) || Unit_RightClickAction[v4->unitType] || (v4->statusFlags & StatusFlags::GoundedBuilding) && Unit_IsFactoryBuilding(v4))
			{
				if (AI_UnitCanAttack(v4))
				{
					return ButtonState::BTNST_ENABLED;
				}
			}
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_CanAttack, BTNSCOND_CanAttack_);

ButtonState __fastcall BTNSCOND_NoCargo_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		CUnit* unit = ClientSelectionGroup[i];

		if (unit && (unit->resourceType & 3) == 0)
		{
			return ButtonState::BTNST_ENABLED;
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_NoCargo, BTNSCOND_NoCargo_);

ButtonState __fastcall BTNSCOND_HasCargo_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (ClientSelectionGroup[i] && (ClientSelectionGroup[i]->resourceType & 3))
		{
			return ButtonState::BTNST_ENABLED;
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_HasCargo, BTNSCOND_HasCargo_);

ButtonState __fastcall BTNSCOND_HasRoom_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* unit = ClientSelectionGroup[i])
		{
			if ((unit->statusFlags & IsHallucination) != 0)
			{
				return ButtonState::BTNST_HIDDEN;
			}
			if (unit->unitType == Zerg_Overlord && !UpgradeLevelSC[unit->playerID].items[24])
			{
				return ButtonState::BTNST_HIDDEN;
			}
			if (!Unit_SpaceProvided[unit->unitType])
			{
				return ButtonState::BTNST_HIDDEN;
			}
			if (getLoadedSpaceAmount(unit) < (unsigned int)Unit_SpaceProvided[unit->unitType])
			{
				return ButtonState::BTNST_ENABLED;
			}
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_HasRoom, BTNSCOND_HasRoom_);

ButtonState __fastcall BTNSCOND_HasUnit_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (ClientSelectionGroup[i] && isUnitLoaded(ClientSelectionGroup[i]))
		{
			return ButtonState::BTNST_ENABLED;
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_HasUnit, BTNSCOND_HasUnit_);

ButtonState __fastcall BTNSCOND_Stationary_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* unit = ClientSelectionGroup[i])
		{
			if ((unit->statusFlags & StatusFlags::DoodadStatesThing) == 0 && !unit->status.lockdownTimer && !unit->status.stasisTimer && !unit->status.maelstromTimer && AI_UnitCanAttack(unit))
			{
				return ButtonState::BTNST_ENABLED;
			}
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_Stationary, BTNSCOND_Stationary_);

ButtonState __fastcall BTNSCOND_CanBuildUnit_(u16 variable, int player_id, CUnit* unit)
{
	if ((u8)ClientSelectionCount <= 1 || unit->unitType == Zerg_Larva || unit->unitType == Zerg_Mutalisk || unit->unitType == Zerg_Hydralisk)
	{
		return TTAllowed((UnitType)variable, unit, player_id);
	}
	else
	{
		return ButtonState::BTNST_HIDDEN;
	}
}

FUNCTION_PATCH(BTNSCOND_CanBuildUnit, BTNSCOND_CanBuildUnit_);

ButtonState __fastcall BTNSCOND_Movement_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (ClientSelectionGroup[i] && (ClientSelectionGroup[i]->statusFlags & Burrowed))
		{
			return ButtonState::BTNST_HIDDEN;
		}
	}

	return ButtonState::BTNST_ENABLED;
}

FUNCTION_PATCH(BTNSCOND_Movement, BTNSCOND_Movement_);

ButtonState __fastcall BTNSCOND_BattleOrders_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (ClientSelectionGroup[i] && (ClientSelectionGroup[i]->statusFlags & Burrowed))
		{
			return ButtonState::BTNST_HIDDEN;
		}
	}

	return BTNSCOND_CanAttack(variable, player_id, unit) != ButtonState::BTNST_HIDDEN ? ButtonState::BTNST_ENABLED : ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_BattleOrders, BTNSCOND_BattleOrders_);

ButtonState __fastcall BTNSCOND_ZergNoCargo_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (ClientSelectionGroup[i] || (ClientSelectionGroup[i]->statusFlags & Burrowed))
		{
			return ButtonState::BTNST_HIDDEN;
		}
	}

	return BTNSCOND_NoCargo(variable, player_id, unit) != BTNST_HIDDEN ? ButtonState::BTNST_ENABLED : ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_ZergNoCargo, BTNSCOND_ZergNoCargo_);

ButtonState __fastcall BTNSCOND_ZergHasCargo_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (ClientSelectionGroup[i] || (ClientSelectionGroup[i]->statusFlags & Burrowed))
		{
			return ButtonState::BTNST_HIDDEN;
		}
	}

	return BTNSCOND_HasCargo(variable, player_id, unit) != BTNST_HIDDEN ? ButtonState::BTNST_ENABLED : ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_ZergHasCargo, BTNSCOND_ZergHasCargo_);

ButtonState __fastcall BTNSCOND_ZergBasic_(u16 variable, int player_id, CUnit* unit)
{
	if (ClientSelectionCount == 1 && (unit->statusFlags & StatusFlags::Burrowed) == 0)
	{
		if (TTAllowed(Zerg_Hatchery, unit, player_id)
			|| TTAllowed(Zerg_Creep_Colony, unit, player_id)
			|| TTAllowed(Zerg_Extractor, unit, player_id)
			|| TTAllowed(Zerg_Spawning_Pool, unit, player_id)
			|| TTAllowed(Zerg_Evolution_Chamber, unit, player_id)
			|| TTAllowed(Zerg_Hydralisk_Den, unit, player_id))
		{
			return ButtonState::BTNST_ENABLED;
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_ZergBasic, BTNSCOND_ZergBasic_);

ButtonState __fastcall BTNSCOND_ZergAdvanced_(u16 variable, int player_id, CUnit* unit)
{
	if (ClientSelectionCount == 1 && (unit->statusFlags & StatusFlags::Burrowed) == 0)
	{
		if (TTAllowed(Zerg_Nydus_Canal, unit, player_id)
			|| TTAllowed(Zerg_Spire, unit, player_id)
			|| TTAllowed(Zerg_Queens_Nest, unit, player_id)
			|| TTAllowed(Zerg_Ultralisk_Cavern, unit, player_id)
			|| TTAllowed(Zerg_Defiler_Mound, unit, player_id))
		{
			return ButtonState::BTNST_ENABLED;
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_ZergAdvanced, BTNSCOND_ZergAdvanced_);

ButtonState __fastcall BTNSCOND_CanBurrow_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* unit = ClientSelectionGroup[i])
		{
			if ((Unit_PrototypeFlags[unit->unitType] & Burrowable))
			{
				ButtonState result = CanUseTech_(unit, (Tech2)variable, player_id);

				if (result != ButtonState::BTNST_ENABLED)
				{
					return (ButtonState) result;
				}
				if ((unit->statusFlags & StatusFlags::Burrowed) == 0 && unit->orderID != Order::ORD_BURROW && unit->orderID != Order::ORD_DEBURROW)
				{
					return ButtonState::BTNST_ENABLED;
				}
			}
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_CanBurrow, BTNSCOND_CanBurrow_);

ButtonState __fastcall BTNSCOND_IsBurrowed_(u16 variable, int player_id, CUnit* unit)
{
	Tech tech = (Tech)variable;
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* unit = ClientSelectionGroup[i])
		{
			if (Unit_PrototypeFlags[unit->unitType] & Burrowable)
			{
				if ((unit->statusFlags & Burrowed) == 0)
				{
					return ButtonState::BTNST_HIDDEN;
				}
				if (unit->orderID == Order::ORD_BURROW || unit->orderID == Order::ORD_DEBURROW)
				{
					return ButtonState::BTNST_HIDDEN;
				}
				if (CanUseTech_(unit, (Tech2)tech, player_id) != ButtonState::BTNST_ENABLED)
				{
					return ButtonState::BTNST_HIDDEN;
				}
			}
		}
	}

	return ButtonState::BTNST_ENABLED;
}

FUNCTION_PATCH(BTNSCOND_IsBurrowed, BTNSCOND_IsBurrowed_);

ButtonState __fastcall BTNSCOND_CanMorphLurker_(u16 variable, int player_id, CUnit* unit)
{
	return CanUseTech_(unit, TECH2_lurker_aspect, player_id);
}

FUNCTION_PATCH(BTNSCOND_CanMorphLurker, BTNSCOND_CanMorphLurker_);

ButtonState __fastcall BTNSCOND_LurkerStop_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* unit = ClientSelectionGroup[i])
		{
			if ((ActivePortraitUnit->statusFlags & StatusFlags::Burrowed) != (unit->statusFlags & StatusFlags::Burrowed))
			{
				return ButtonState::BTNST_HIDDEN;
			}
		}
	}

	return ButtonState::BTNST_ENABLED;
}

FUNCTION_PATCH(BTNSCOND_LurkerStop, BTNSCOND_LurkerStop_);

ButtonState __fastcall BTNSCOND_HasTech_(u16 variable, int player_id, CUnit* unit)
{
	return CanUseTech_(unit, (Tech2)variable, player_id);
}

FUNCTION_PATCH(BTNSCOND_HasTech, BTNSCOND_HasTech_);

ButtonState __fastcall BTNSCOND_CanCloak_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* unit = ClientSelectionGroup[i])
		{
			Tech tech;
			if (unit->unitType == UnitType::Terran_Ghost
				|| unit->unitType == UnitType::Hero_Sarah_Kerrigan
				|| unit->unitType == UnitType::Hero_Alexei_Stukov
				|| unit->unitType == UnitType::Hero_Samir_Duran
				|| unit->unitType == UnitType::Hero_Infested_Duran
				|| unit->unitType == UnitType::Hero_Infested_Kerrigan)
			{
				tech = TECH_personnel_cloaking;
			}
			else
			{
				tech = unit->unitType == UnitType::Terran_Wraith || unit->unitType == UnitType::Hero_Tom_Kazansky ? Tech::TECH_cloaking_field : Tech::TECH_none;
			}

			if (CanUseTech_(unit, (Tech2) tech, player_id) == ButtonState::BTNST_ENABLED)
			{
				if (((unit->statusFlags & CloakingForFree) == 0 || (unit->statusFlags & Burrowed)) && (unit->statusFlags & RequiresDetection) == 0)
				{
					return ButtonState::BTNST_ENABLED;
				}
			}
		}
	}


	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_CanCloak, BTNSCOND_CanCloak_);

ButtonState __fastcall BTNSCOND_IsCloaked_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* unit = ClientSelectionGroup[i])
		{
			Tech tech;
			if (unit->unitType == UnitType::Terran_Ghost
				|| unit->unitType == UnitType::Hero_Sarah_Kerrigan
				|| unit->unitType == UnitType::Hero_Alexei_Stukov
				|| unit->unitType == UnitType::Hero_Samir_Duran
				|| unit->unitType == UnitType::Hero_Infested_Duran
				|| unit->unitType == UnitType::Hero_Infested_Kerrigan)
			{
				tech = TECH_personnel_cloaking;
			}
			else
			{
				tech = unit->unitType == UnitType::Terran_Wraith || unit->unitType == UnitType::Hero_Tom_Kazansky ? Tech::TECH_cloaking_field : Tech::TECH_none;
			}

			if (CanUseTech_(unit, (Tech2)tech, player_id) != ButtonState::BTNST_ENABLED)
			{
				return ButtonState::BTNST_HIDDEN;
			}
			if ((unit->statusFlags & StatusFlags::CloakingForFree) && (unit->statusFlags & StatusFlags::Burrowed) == 0)
			{
				return ButtonState::BTNST_HIDDEN;
			}
			if ((unit->statusFlags & StatusFlags::RequiresDetection) == 0)
			{
				return ButtonState::BTNST_HIDDEN;
			}
		}
	}

	return ButtonState::BTNST_ENABLED;
}

FUNCTION_PATCH(BTNSCOND_IsCloaked, BTNSCOND_IsCloaked_);

ButtonState __fastcall BTNSCOND_CanResearch_(u16 variable, int player_id, CUnit* unit)
{
	return ReasearchAllowed((Tech2)variable, player_id, unit);
}

FUNCTION_PATCH(BTNSCOND_CanResearch, BTNSCOND_CanResearch_);

ButtonState __fastcall BTNSCOND_IsResearching_(u16 variable, int player_id, CUnit* unit)
{
	return unit->fields1.building.techType != Tech::TECH_none ? ButtonState::BTNST_ENABLED : ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_IsResearching, BTNSCOND_IsResearching_);

ButtonState __fastcall BTNSCOND_CanUpgrade_(u16 variable, int player_id, CUnit* unit)
{
	return UpgradeAllowed((Tech2)variable, player_id, unit);
}

FUNCTION_PATCH(BTNSCOND_CanUpgrade, BTNSCOND_CanUpgrade_);

ButtonState __fastcall BTNSCOND_IsUpgrading_(u16 variable, int player_id, CUnit* unit)
{
	return unit->fields1.building.upgradeType != 61 ? ButtonState::BTNST_ENABLED : ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_IsUpgrading, BTNSCOND_IsUpgrading_);

ButtonState __fastcall BTNSCOND_NoNydusExit_(u16 variable, int player_id, CUnit* unit)
{
	return unit->fields2.nydus.exit ? ButtonState::BTNST_HIDDEN : ButtonState::BTNST_ENABLED;
}

FUNCTION_PATCH(BTNSCOND_NoNydusExit, BTNSCOND_NoNydusExit_);

ButtonState __fastcall BTNSCOND_IsTraining_(u16 variable, int player_id, CUnit* unit)
{
	return unit->buildQueue[unit->buildQueueSlot % 5] <= 105 ? ButtonState::BTNST_ENABLED : ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_IsTraining, BTNSCOND_IsTraining_);

ButtonState __fastcall BTNSCOND_TrainingFighter_(u16 variable, int player_id, CUnit* unit)
{
	ButtonState result = BTNST_HIDDEN;

	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* selected_unit = ClientSelectionGroup[i])
		{
			if (selected_unit->unitType != unit->unitType)
			{
				return BTNST_HIDDEN;
			}
			if (result == BTNST_HIDDEN)
			{
				result = TTAllowed((UnitType)variable, selected_unit, player_id);
			}
		}
	}

	return result;
}

FUNCTION_PATCH(BTNSCOND_TrainingFighter, BTNSCOND_TrainingFighter_);

ButtonState __fastcall BTNSCOND_ProtossBasic_(u16 variable, int player_id, CUnit* unit)
{
	if (ClientSelectionCount == 1)
	{
		if ((TTAllowed(Protoss_Nexus, unit, player_id)
			|| TTAllowed(Protoss_Pylon, unit, player_id)
			|| TTAllowed(Protoss_Assimilator, unit, player_id)
			|| TTAllowed(Protoss_Gateway, unit, player_id)
			|| TTAllowed(Protoss_Forge, unit, player_id)
			|| TTAllowed(Protoss_Photon_Cannon, unit, player_id)
			|| TTAllowed(Protoss_Cybernetics_Core, unit, player_id)
			|| TTAllowed(Protoss_Shield_Battery, unit, player_id)))
		{
			return ButtonState::BTNST_ENABLED;
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_ProtossBasic, BTNSCOND_ProtossBasic_);

ButtonState __fastcall BTNSCOND_ProtossAdvanced_(u16 variable, int player_id, CUnit* unit)
{
	if (ClientSelectionCount == 1)
	{
		if (TTAllowed(Protoss_Robotics_Facility, unit, player_id)
			|| TTAllowed(Protoss_Stargate, unit, player_id)
			|| TTAllowed(Protoss_Citadel_of_Adun, unit, player_id)
			|| TTAllowed(Protoss_Robotics_Support_Bay, unit, player_id)
			|| TTAllowed(Protoss_Fleet_Beacon, unit, player_id)
			|| TTAllowed(Protoss_Templar_Archives, unit, player_id)
			|| TTAllowed(Protoss_Observatory, unit, player_id)
			|| TTAllowed(Protoss_Arbiter_Tribunal, unit, player_id))
		{
			return ButtonState::BTNST_ENABLED;
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_ProtossAdvanced, BTNSCOND_ProtossAdvanced_);

ButtonState __fastcall BTNSCOND_CanMergeArchonOneSelected_(u16 variable, int player_id, CUnit* unit)
{
	return CanUseTech(unit, (Tech2)(u8)variable, player_id) != BTNST_DISABLED ? ButtonState::BTNST_HIDDEN : ButtonState::BTNST_DISABLED;
}

FUNCTION_PATCH(BTNSCOND_CanMergeArchonOneSelected, BTNSCOND_CanMergeArchonOneSelected_);

ButtonState __fastcall BTNSCOND_CanMergeArchonTwoSelected_(u16 variable, int player_id, CUnit* unit)
{
	if (CanUseTech(unit, TECH2_archon_warp, player_id) != BTNST_ENABLED)
	{
		return ButtonState::BTNST_HIDDEN;
	}
	else if (ClientSelectionCount > 1)
	{
		return ButtonState::BTNST_ENABLED;
	}
	else
	{
		return ButtonState::BTNST_DISABLED;
	}
}

FUNCTION_PATCH(BTNSCOND_CanMergeArchonTwoSelected, BTNSCOND_CanMergeArchonTwoSelected_);

ButtonState __fastcall BTNSCOND_CanMergeDarkArchonOneSelected_(u16 variable, int player_id, CUnit* unit)
{
	return CanUseTech(unit, (Tech2)(u8)variable, player_id) != BTNST_DISABLED ? ButtonState::BTNST_HIDDEN : ButtonState::BTNST_DISABLED;
}

FUNCTION_PATCH(BTNSCOND_CanMergeDarkArchonOneSelected, BTNSCOND_CanMergeDarkArchonOneSelected_);

ButtonState __fastcall BTNSCOND_CanMergeDarkArchonTwoSelected_(u16 variable, int player_id, CUnit* unit)
{
	if (CanUseTech(unit, TECH2_dark_archon_meld, player_id) != BTNST_ENABLED)
	{
		return ButtonState::BTNST_HIDDEN;
	}
	else if (ClientSelectionCount > 1)
	{
		return ButtonState::BTNST_ENABLED;
	}
	else
	{
		return ButtonState::BTNST_DISABLED;
	}
}

FUNCTION_PATCH(BTNSCOND_CanMergeDarkArchonTwoSelected, BTNSCOND_CanMergeDarkArchonTwoSelected_);

ButtonState __fastcall BTNSCOND_HasScarabs_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* selected_unit = ClientSelectionGroup[i])
		{
			if (selected_unit->unitType == Protoss_Reaver || selected_unit->unitType == Hero_Warbringer)
			{
				if (selected_unit->fields1.carrier.inHangerCount)
				{
					return ButtonState::BTNST_ENABLED;
				}
			}
		}
	}

	return BTNST_DISABLED;
}

FUNCTION_PATCH(BTNSCOND_HasScarabs, BTNSCOND_HasScarabs_);

ButtonState __fastcall BTNSCOND_ScvIsBuilding_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* selected_unit = ClientSelectionGroup[i])
		{
			if (selected_unit->orderID != Order::ORD_CONSTRUCT_BLDG)
			{
				return ButtonState::BTNST_HIDDEN;
			}
		}
	}

	return ButtonState::BTNST_ENABLED;
}

FUNCTION_PATCH(BTNSCOND_ScvIsBuilding, BTNSCOND_ScvIsBuilding_);

ButtonState __fastcall BTNSCOND_ScvNotConstructing(u16 variable, int player_id, CUnit* unit)
{
	return BTNSCOND_ScvIsBuilding_(variable, player_id, unit) == ButtonState::BTNST_ENABLED ? ButtonState::BTNST_HIDDEN : ButtonState::BTNST_ENABLED;
}

FUNCTION_PATCH(BTNSCOND_SCVCanMove, BTNSCOND_ScvNotConstructing);
FUNCTION_PATCH(BTNSCOND_SCVCanStop, BTNSCOND_ScvNotConstructing);
FUNCTION_PATCH(BTNSCOND_SCVCanAttack, BTNSCOND_ScvNotConstructing);
FUNCTION_PATCH(BTNSCOND_CanRepair, BTNSCOND_ScvNotConstructing);

ButtonState __fastcall BTNSCOND_NoCargoTerran_(u16 variable, int player_id, CUnit* unit)
{
	if (BTNSCOND_NoCargo(variable, player_id, unit) == ButtonState::BTNST_HIDDEN)
	{
		return ButtonState::BTNST_HIDDEN;
	}
	return BTNSCOND_ScvNotConstructing(variable, player_id, unit);
}

FUNCTION_PATCH(BTNSCOND_NoCargoTerran, BTNSCOND_NoCargoTerran_);

ButtonState __fastcall BTNSCOND_HasCargoTerran_(u16 variable, int player_id, CUnit* unit)
{
	if (BTNSCOND_HasCargo(variable, player_id, unit) == ButtonState::BTNST_HIDDEN)
	{
		return ButtonState::BTNST_HIDDEN;
	}
	return BTNSCOND_ScvNotConstructing(variable, player_id, unit);
}

FUNCTION_PATCH(BTNSCOND_HasCargoTerran, BTNSCOND_HasCargoTerran_);

ButtonState __fastcall BTNSCOND_TerranBasic_(u16 variable, int player_id, CUnit* unit)
{
	if (ClientSelectionCount == 1)
	{
		if (BTNSCOND_ScvNotConstructing(variable, player_id, unit) == ButtonState::BTNST_ENABLED)
		{
			if (TTAllowed(Terran_Command_Center, unit, player_id)
				|| TTAllowed(Terran_Supply_Depot, unit, player_id)
				|| TTAllowed(Terran_Refinery, unit, player_id)
				|| TTAllowed(Terran_Barracks, unit, player_id)
				|| TTAllowed(Terran_Engineering_Bay, unit, player_id)
				|| TTAllowed(Terran_Missile_Turret, unit, player_id)
				|| TTAllowed(Terran_Academy, unit, player_id)
				|| TTAllowed(Terran_Bunker, unit, player_id))
			{
				return ButtonState::BTNST_ENABLED;
			}
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_TerranBasic, BTNSCOND_TerranBasic_);

ButtonState __fastcall BTNSCOND_TerranAdvanced_(u16 variable, int player_id, CUnit* unit)
{
	if (ClientSelectionCount == 1)
	{
		if (BTNSCOND_ScvNotConstructing(variable, player_id, unit) == ButtonState::BTNST_ENABLED)
		{
			if (TTAllowed(Terran_Factory, unit, player_id)
				|| TTAllowed(Terran_Starport, unit, player_id)
				|| TTAllowed(Terran_Science_Facility, unit, player_id)
				|| TTAllowed(Terran_Armory, unit, player_id))
			{
				return ButtonState::BTNST_ENABLED;
			}
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_TerranAdvanced, BTNSCOND_TerranAdvanced_);

ButtonState __fastcall BTNSCOND_CanCloak_0_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* v4 = ClientSelectionGroup[i])
		{
			ButtonState result = CanUseTech(v4, (Tech2)(u8)variable, player_id);
			if (result != ButtonState::BTNST_ENABLED)
			{
				return result;
			}
			if (((v4->statusFlags & StatusFlags::CloakingForFree) == 0 || (v4->statusFlags & StatusFlags::Burrowed) != 0) && (v4->statusFlags & StatusFlags::RequiresDetection) == 0)
			{
				return ButtonState::BTNST_ENABLED;
			}
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_CanCloak_0, BTNSCOND_CanCloak_0_);

ButtonState __fastcall BTNSCOND_IsCloaked_0_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* v4 = ClientSelectionGroup[i])
		{
			if (CanUseTech(v4, (Tech2)(u8)variable, player_id) != ButtonState::BTNST_ENABLED)
			{
				return ButtonState::BTNST_HIDDEN;
			}
			if ((v4->statusFlags & StatusFlags::CloakingForFree) != 0 && (v4->statusFlags & StatusFlags::Burrowed) == 0)
			{
				return ButtonState::BTNST_HIDDEN;
			}
			if ((v4->statusFlags & StatusFlags::RequiresDetection) == 0)
			{
				return ButtonState::BTNST_HIDDEN;
			}
		}
	}

	return ButtonState::BTNST_ENABLED;
}

FUNCTION_PATCH(BTNSCOND_IsCloaked_0, BTNSCOND_IsCloaked_0_);

ButtonState __fastcall BTNSCOND_HasNuke_(u16 variable, int player_id, CUnit* unit)
{
	for (CUnit* player_unit = UnitNodeList_PlayerFirstUnit[unit->playerID]; player_unit; player_unit = player_unit->nextPlayerUnit)
	{
		if (player_unit->unitType == Terran_Nuclear_Silo && player_unit->fields2.silo.bReady)
		{
			return ButtonState::BTNST_ENABLED;
		}
	}
	return ButtonState::BTNST_DISABLED;
}

FUNCTION_PATCH(BTNSCOND_HasNuke, BTNSCOND_HasNuke_);

ButtonState __fastcall BTNSCOND_HasSpidermines_(u16 variable, int player_id, CUnit* unit)
{
	ButtonState v7 = ButtonState::BTNST_HIDDEN;
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* selected_unit = ClientSelectionGroup[i])
		{
			ButtonState v5 = CanUseTech(selected_unit, (Tech2)(u8)variable, player_id);
			if (v5 == ButtonState::BTNST_ENABLED)
			{
				return ButtonState::BTNST_ENABLED;
			}
			else if (v5 == ButtonState::BTNST_DISABLED)
			{
				v7 = ButtonState::BTNST_DISABLED;
			}
		}
	}

	return v7;
}

FUNCTION_PATCH(BTNSCOND_HasSpidermines, BTNSCOND_HasSpidermines_);

ButtonState __fastcall BTNSCOND_TankMove_(u16 variable, int player_id, CUnit* unit)
{
	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		if (CUnit* selected_unit = ClientSelectionGroup[i])
		{
			if (selected_unit->unitType == Terran_Siege_Tank_Siege_Mode || selected_unit->unitType == Hero_Edmund_Duke_Siege_Mode)
			{
				return ButtonState::BTNST_HIDDEN;
			}
		}
	}

	return ButtonState::BTNST_ENABLED;
}

FUNCTION_PATCH(BTNSCOND_TankMove, BTNSCOND_TankMove_);

ButtonState __fastcall BTNSCOND_IsUnsieged_(u16 variable, int player_id, CUnit* unit)
{
	ButtonState button_state = CanUseTech(unit, (Tech2)(u8)variable, player_id);

	if (button_state == ButtonState::BTNST_ENABLED)
	{
		for (int i = 0; i < _countof(ClientSelectionGroup); i++)
		{
			if (CUnit* selected_unit = ClientSelectionGroup[i])
			{
				if (selected_unit->unitType == Terran_Siege_Tank_Siege_Mode || selected_unit->unitType == Hero_Edmund_Duke_Siege_Mode)
				{
					return ButtonState::BTNST_HIDDEN;
				}
			}
		}
	}

	return button_state;
}

FUNCTION_PATCH(BTNSCOND_IsUnsieged, BTNSCOND_IsUnsieged_);

ButtonState __fastcall BTNSCOND_IsSieged_(u16 variable, int player_id, CUnit *unit)
{
	if (CanUseTech_(unit, (Tech2)variable, player_id) != 1)
	{
		return ButtonState::BTNST_HIDDEN;
	}

	for (int i = 0; i < _countof(ClientSelectionGroup); i++)
	{
		CUnit* unit = ClientSelectionGroup[i];

		if (unit && (unit->unitType == Terran_Siege_Tank_Siege_Mode || unit->unitType == Hero_Edmund_Duke_Siege_Mode))
		{
			return ButtonState::BTNST_ENABLED;
		}
	}

	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_IsSieged, BTNSCOND_IsSieged_);

ButtonState __fastcall BTNSCOND_IsLiftedCanMove_(u16 variable, int player_id, CUnit* unit)
{
	if (unit->statusFlags & StatusFlags::GoundedBuilding)
	{
		return ButtonState::BTNST_HIDDEN;
	}
	else
	{
		return ButtonState::BTNST_ENABLED;
	}
}

FUNCTION_PATCH(BTNSCOND_IsLiftedCanMove, BTNSCOND_IsLiftedCanMove_);

ButtonState __fastcall BTNSCOND_IsLifted_(u16 variable, int player_id, CUnit* unit)
{
	if (ClientSelectionCount == 1 && !(unit->statusFlags & StatusFlags::GoundedBuilding))
	{
		return ButtonState::BTNST_ENABLED;
	}
	else
	{
		return ButtonState::BTNST_HIDDEN;
	}
}

FUNCTION_PATCH(BTNSCOND_IsLifted, BTNSCOND_IsLifted_);

ButtonState __fastcall BTNSCOND_IsLanded_(u16 variable, int player_id, CUnit* unit)
{
	if ((unit->statusFlags & StatusFlags::GoundedBuilding) && !UnitIsTrainingOrMorphing(unit) && unit->fields1.building.techType == Tech::TECH_none && unit->fields1.building.upgradeType == 61)
	{
		return ButtonState::BTNST_ENABLED;
	}
	else
	{
		return ButtonState::BTNST_HIDDEN;
	}
}

FUNCTION_PATCH(BTNSCOND_IsLanded, BTNSCOND_IsLanded_);

ButtonState __fastcall BTNSCOND_IsBuildingAddon_(u16 variable, int player_id, CUnit* unit)
{
	if (unit->secondaryOrderID == Order::ORD_BUILD_ADDON && (unit->statusFlags & StatusFlags::GoundedBuilding))
	{
		if (unit->currentBuildUnit && (unit->currentBuildUnit->statusFlags & StatusFlags::Completed) == 0)
		{
			return ButtonState::BTNST_ENABLED;
		}
	}
	return ButtonState::BTNST_HIDDEN;
}

FUNCTION_PATCH(BTNSCOND_IsBuildingAddon, BTNSCOND_IsBuildingAddon_);

ButtonState __fastcall BTNSCOND_SiloHasNoNuke_(u16 variable, int player_id, CUnit* unit)
{
	if (ClientSelectionCount == 1)
	{
		return TTAllowed((UnitType)variable, unit, player_id);
	}
	else
	{
		return ButtonState::BTNST_HIDDEN;
	}
}

FUNCTION_PATCH(BTNSCOND_SiloHasNoNuke, BTNSCOND_SiloHasNoNuke_);
