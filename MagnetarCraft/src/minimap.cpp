#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"
#include "exception.h"

void __stdcall MinimapPing_maybe_(unsigned __int16 pos_x, unsigned __int16 pos_y, char a3)
{
	for (int i = 0; i < _countof(stru_59C1B8); i++)
	{
		if (stru_59C1B8[i].a0 == 0)
		{
			unsigned v6 = (unsigned __int16)word_59CC68;
			stru_59C1B8[i].a0 = 1;
			stru_59C1B8[i].d = 0;
			stru_59C1B8[i].b = (pos_x >> 5 << word_59C1B0) / v6 - 64;
			stru_59C1B8[i].c = (pos_y >> 5 << word_59C1B0) / v6 - 64;
			LOBYTE(stru_59C1B8[i].e) = a3;

			break;
		}
	}
}

FUNCTION_PATCH(MinimapPing_maybe, MinimapPing_maybe_);

void CreateMinimapSurface_()
{
	minimap_surface.ht = minimap_surface_height;
	minimap_surface.wid = minimap_surface_width;
	minimap_surface.data = (u8*)SMemAlloc(minimap_surface.wid * minimap_surface.ht, "Starcraft\\SWAR\\lang\\minimap.cpp", 1637, 0);

	minimap_surface_no_fog.ht = minimap_surface_height;
	minimap_surface_no_fog.wid = minimap_surface_width;
	minimap_surface_no_fog.data = (u8*)SMemAlloc(minimap_surface_width * minimap_surface_height, "Starcraft\\SWAR\\lang\\minimap.cpp", 1636, 0);
}

FAIL_STUB_PATCH(CreateMinimapSurface);

void sub_4A3A00_()
{
	for (int i = 0; i < 256; ++i)
	{
		byte_59C2C0[i] = HIBYTE(dword_6CEB2C);
		byte_59C3C0[i] = byte_5982A0[i];
		byte_59C4C0[i] = i;
		byte_59C5C0[i] = i;
	}
}

FAIL_STUB_PATCH(sub_4A3A00);

template <int PIXEL_STRIDE>
void minimapVisionUpdateMegatile(int flags, int x, int y)
{
	for (int i = 0; i < PIXEL_STRIDE; i++)
	{
		for (int j = 0; j < PIXEL_STRIDE; j++)
		{
			int index = (PIXEL_STRIDE * x + i) * minimap_surface_width + (PIXEL_STRIDE * y + j);
			minimap_surface.data[index] = byte_59C2C0[256 * flags + minimap_surface_no_fog.data[index]];
		}
	}
}

template <int PIXEL_STRIDE>
void minimapVisionUpdate_()
{
	for (int i = 0; i < map_size.height / word_59CC68; i++)
	{
		for (int j = 0; j < map_size.width / word_59CC68; j++)
		{
			MegatileFlags active_tile = active_tiles[(i * map_size.width + j) * word_59CC68];
			BOOL v2 = (playerVisions & active_tile) == 0;
			int v3 = (playerExploredVisions & active_tile) == 0;
			if (InReplay)
			{
				if (replayShowEntireMap)
				{
					v2 = 1;
					v3 = 0;
				}
				else
				{
					v3 = 0;
					BYTE1(v3) = ReplayVision;
					int v4 = ~active_tile;
					if ((v3 & v4) != 0)
					{
						if (((unsigned __int8)v4 & (unsigned __int8)ReplayVision) != 0)
						{
							v2 = 1;
							v3 = 0;
						}
						else
						{
							v2 = 0;
							v3 = 1;
						}
					}
					else
					{
						v2 = 0;
						v3 = 0;
					}
				}
			}

			minimapVisionUpdateMegatile<PIXEL_STRIDE>(v3 | (2 * v2), i, j);
		}
	}
}

FAIL_STUB_PATCH(minimapVisionUpdate_64);
FAIL_STUB_PATCH(minimapVisionUpdate_96_128);
FAIL_STUB_PATCH(minimapVisionUpdate_192_256);

char sub_4A3D40_(__int16 cell, int minitile_y, int minitile_x)
{
	return byte_59CB60[VR4Data->cdata[4 * (VX4Data[cell].wImageRef[minitile_y][minitile_x] & 0xFFFE) + 6][7]];
}

FAIL_STUB_PATCH(sub_4A3D40);

void getMinimapCursorPos_(int* x, int* y)
{
	*x -= minimap_dialog->rct.left;
	if (*x < 0)
	{
		*x = 0;
	}
	else if (*x >= minimap_surface_no_fog.wid)
	{
		*x = minimap_surface_no_fog.wid - 1;
	}

	*x *= word_59CC6C;

	*y -= (315 + minimap_dialog->rct.top);
	if (*y < 0)
	{
		*y = 0;
	}
	else if (*y >= minimap_surface_no_fog.ht)
	{
		*y = minimap_surface_no_fog.ht - 1;
	}
	*y *= word_59CC6C;
}

FAIL_STUB_PATCH(getMinimapCursorPos);

void minimapGameResetMouseInput_(dialog* dlg)
{
	removeDlgFromTimerTracking(dlg);
	SetCursorClipBounds_();

	dword_6D5DD4 = 0;

	if (has_viewport && dword_6D5DD0)
	{
		ClipCursor(&screen);
	}
}

FAIL_STUB_PATCH(minimapGameResetMouseInput);

void sub_4A4150_()
{
	if (dword_5993AC == 1 || !byte_6D5BBF)
	{
		sub_4A3A00_();
		minimapVisionUpdate();
	}
	else
	{
		memset(minimap_surface.data, 0, minimap_surface.ht * minimap_surface.wid);
	}
}

FAIL_STUB_PATCH(sub_4A4150);

template <int PIXEL_STRIDE>
void minimapSurfaceUpdateMegatile(int x, int y)
{
	int cellMapIndex = (x * map_size.width + y) * word_59CC68;
	__int16 cell = CellMap[cellMapIndex] & 0x7FFF;
	for (int i = 0; i < PIXEL_STRIDE; i++)
	{
		for (int j = 0; j < PIXEL_STRIDE; j++)
		{
			minimap_surface_no_fog.data[(PIXEL_STRIDE * x + i) * minimap_surface_width + PIXEL_STRIDE * y + j] = sub_4A3D40_(cell, i, j);
		}
	}
}

template <int PIXEL_STRIDE>
void minimapSurfaceUpdate_()
{
	for (int i = 0; i < map_size.height / word_59CC68; i++)
	{
		for (int j = 0; j < map_size.width / word_59CC68; j++)
		{
			minimapSurfaceUpdateMegatile<PIXEL_STRIDE>(i, j);
		}
	}
}

FAIL_STUB_PATCH(minimapSurfaceUpdate_64);
FAIL_STUB_PATCH(minimapSurfaceUpdate_96_128);
FAIL_STUB_PATCH(minimapSurfaceUpdate_192_256);

void drawAllMinimapBoxes_()
{
	Bitmap* v0 = dword_6CF4A8;
	dword_6CF4A8 = &minimap_surface;
	dword_59C2B8 = 0;
	dword_59C1A8 = 0;

	for (int player = 11; player >= 0; player--)
	{
		if (player >= 8)
		{
			drawMinimapUnitBox(player);
		}
		else if (g_LocalNationID != player || InReplay)
		{
			drawMinimapUnitBox2(player);
		}
	}

	if (!InReplay)
	{
		drawAllMinimapUnitBoxes(g_LocalNationID);
	}

	if (dword_654868)
	{
		for (CThingy* thingy = dword_654868; thingy; thingy = thingy->next)
		{
			if (thingy->hitPoints < 203 || thingy->hitPoints > 213)
			{
				if (!InReplay || !CThingyIsVisible(1, thingy))
				{
					char v8;
					CSprite* sprite = thingy->sprite;
					if (sprite->spriteID == 275 || sprite->spriteID == 279 || sprite->spriteID == 280 || sprite->spriteID == 281)
					{
						v8 = byte_6CEB39;
					}
					else if (byte_6D5BBE == 0)
					{
						v8 = PlayerColors[sprite->playerID];
					}
					else if (Alliance[g_LocalNationID].player[sprite->playerID])
					{
						v8 = byte_6CEB31;
					}
					else
					{
						v8 = byte_6CEB34;
					}

					drawUnitBox(v8, sprite->position.x, sprite->position.y, Unit_Placement[thingy->hitPoints].x, Unit_Placement[thingy->hitPoints].y, 1);
					--dword_59C2B8;
				}
			}
		}
	}

	dword_6CF4A8 = v0;
}

FAIL_STUB_PATCH(drawAllMinimapBoxes);

void minimapGameUpdate_(dialog* a1)
{
	a1->pfcnUpdate = MinimapImageUpdate;
	if (dword_5993AC == 0)
	{
		minimapSurfaceUpdate();
		sub_4A4150_();
		drawAllMinimapBoxes_();
	}
	if ((a1->lFlags & CTRL_UPDATE) == 0)
	{
		a1->lFlags |= CTRL_UPDATE;
		updateDialog(a1);
	}
}

FAIL_STUB_PATCH(minimapGameUpdate);

void minimapGameMouseUpdate_(dialog* dlg)
{
	if (byte_658AC0)
	{
		removeDlgFromTimerTracking(dlg);
		SetCursorClipBounds();
		dword_6D5DD4 = 0;
		if (dword_6D5DD0)
		{
			ClipCursor(&screen);
		}
	}
	else
	{
		int v1 = 2 * (unsigned __int16)word_59CC68;
		int x = Mouse.x - (20 << word_59C1B0) / v1;
		int y = Mouse.y - (13 << word_59C1B0) / v1;
		getMinimapCursorPos_(&x, &y);
		if ((unsigned __int16)x >> 5 != MoveToTile.x || (unsigned __int16)y >> 5 != MoveToTile.y)
		{
			BWFXN_MoveScreen(32 * ((unsigned __int16)x >> 5), 32 * (unsigned __int16)y >> 5);
		}
		if ((dlg->lFlags & DialogFlags::CTRL_UPDATE) == 0)
		{
			dlg->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog(dlg);
		}
	}
}

FAIL_STUB_PATCH(minimapGameMouseUpdate);

void setMinimapConstants()
{
	int larger_dimension = map_size.width <= map_size.height ? map_size.height : map_size.width;

	if (larger_dimension <= 32)
	{
		word_59CC68 = 1;
		word_59CC6C = 8;
		minimapSurfaceUpdate = minimapSurfaceUpdate_<4>;
		minimapVisionUpdate = minimapVisionUpdate_<4>;
		minimap_surface_height = 4 * map_size.height;
		minimap_surface_width = 4 * map_size.width;
		word_59C184 = 0;
		word_59C1B0 = 2;
	}
	else if (larger_dimension <= 64)
	{
		word_59CC68 = 1;
		word_59CC6C = 16;
		minimapSurfaceUpdate = minimapSurfaceUpdate_<2>;
		minimapVisionUpdate = minimapVisionUpdate_<2>;
		minimap_surface_height = 2 * map_size.height;
		minimap_surface_width = 2 * map_size.width;
		word_59C184 = 0;
		word_59C1B0 = 1;
	}
	else if (larger_dimension <= 128)
	{
		word_59CC68 = 1;
		word_59CC6C = 32;
		minimapSurfaceUpdate = minimapSurfaceUpdate_<1>;
		minimapVisionUpdate = minimapVisionUpdate_<1>;
		minimap_surface_height = map_size.height;
		minimap_surface_width = map_size.width;
		word_59C184 = 0;
		word_59C1B0 = 0;
	}
	else if (larger_dimension <= 256)
	{
		word_59CC68 = 2;
		word_59CC6C = 64;
		minimapSurfaceUpdate = minimapSurfaceUpdate_<1>;
		minimapVisionUpdate = minimapVisionUpdate_<1>;
		minimap_surface_height = map_size.height >> 1;
		minimap_surface_width = map_size.width >> 1;
		word_59C184 = 0;
		word_59C1B0 = 0;
	}
	else if (larger_dimension <= 512)
	{
		word_59CC68 = 4;
		word_59CC6C = 128;
		minimapSurfaceUpdate = minimapSurfaceUpdate_<1>;
		minimapVisionUpdate = minimapVisionUpdate_<1>;
		minimap_surface_height = map_size.height >> 2;
		minimap_surface_width = map_size.width >> 2;
		word_59C184 = 0;
		word_59C1B0 = 0;
	}
}

void setMapSizeConstants_()
{
	int v5 = (128 - minimap_surface_width) / 2;
	int v7 = (128 - minimap_surface_height) / 2;
	minimap_dialog->rct.left += v5;
	minimap_dialog->rct.right -= v5;
	minimap_dialog->rct.top += v7;
	minimap_dialog->rct.bottom -= v7;

	CreateMinimapSurface_();

	stru_512D00.left = minimap_dialog->rct.left;
	stru_512D00.top = minimap_dialog->rct.top + 315;
	stru_512D00.right = minimap_dialog->rct.left + minimap_surface_width - 1;
	stru_512D00.bottom = minimap_dialog->rct.top + minimap_surface_height + 314;
}

FAIL_STUB_PATCH(setMapSizeConstants);

void __fastcall Minimap_TimerRefresh_(dialog* dlg, __int16 timer_id)
{
	if (HasMegatileUpdate)
	{
		minimapSurfaceUpdate();
	}
	sub_4A4150_();
	drawAllMinimapBoxes_();
	sub_4A3870();

	if ((minimap_dialog->lFlags & DialogFlags::CTRL_UPDATE) == 0)
	{
		minimap_dialog->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog(minimap_dialog);
	}
	HasMegatileUpdate = 0;
}

FAIL_STUB_PATCH(Minimap_TimerRefresh);

void __fastcall updateMinimapSurfaceInfoProc_(dialog* a1, __int16 a2)
{
	byte_6D5BC0 ^= 1;
	if (byte_6D5BC1)
	{
		int i = 0;
		while (stru_59C1B8[i].a0 == 0)
		{
			i += 1;
			if (i >= _countof(stru_59C1B8))
			{
				if (HasMegatileUpdate)
				{
					minimapSurfaceUpdate();
				}
				sub_4A4150_();
				drawAllMinimapBoxes_();
				sub_4A3870();
				if ((minimap_dialog->lFlags & DialogFlags::CTRL_UPDATE) == 0)
				{
					minimap_dialog->lFlags |= DialogFlags::CTRL_UPDATE;
					updateDialog(minimap_dialog);
				}
				HasMegatileUpdate = 0;
				return;
			}
		}
	}
}

FAIL_STUB_PATCH(updateMinimapSurfaceInfoProc);

void __fastcall updateMinimapSurfaceInfo2Proc_(dialog* a1, __int16 a2)
{
	int v2 = 0;
	for (int i = 0; i < _countof(stru_59C1B8); i++)
	{
		if (stru_59C1B8[i].a0)
		{
			if (stru_59C1B8[i].d == 19)
			{
				stru_59C1B8[i].a0 = 0;
			}
			else
			{
				stru_59C1B8[i].d++;
			}
			v2 = 1;
		}
	}
	if (v2)
	{
		if (HasMegatileUpdate)
		{
			minimapSurfaceUpdate();
		}
		sub_4A4150_();
		drawAllMinimapBoxes_();
		sub_4A3870();
		if ((minimap_dialog->lFlags & DialogFlags::CTRL_UPDATE) == 0)
		{
			minimap_dialog->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog(minimap_dialog);
		}
		HasMegatileUpdate = 0;
	}
}

FAIL_STUB_PATCH(updateMinimapSurfaceInfo2Proc);

void minimapPreviewUpdateState_()
{
	sub_4A4150_();
	drawAllMinimapBoxes_();
	sub_4A3870();
	if ((minimap_dialog->lFlags & DialogFlags::CTRL_UPDATE) == 0)
	{
		minimap_dialog->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog(minimap_dialog);
	}
}

FAIL_STUB_PATCH(minimapPreviewUpdateState);

void MinimapGameRightclickEventMoveto_(dialog* dlg)
{
	_ClipCursor(&stru_512D00);

	if (EventDialogs[EventNo::EVN_RBUTTONUP])
	{
		dlgEvent event;
		event.wNo = EventNo::EVN_USER;
		event.dwUser = EventUser::USER_NEXT;
		event.cursor.x = Mouse.x;
		event.cursor.y = Mouse.y;
		*(_DWORD*)&event.wSelection = 0;
		EventDialogs[EventNo::EVN_RBUTTONUP]->pfcnInteract(EventDialogs[EventNo::EVN_RBUTTONUP], &event);
	}
	EventDialogs[EventNo::EVN_RBUTTONUP] = dlg;

	if (EventDialogs[EventNo::EVN_MOUSEMOVE])
	{
		dlgEvent event;
		event.wNo = EventNo::EVN_USER;
		event.dwUser = EventUser::USER_NEXT;
		event.cursor.x = Mouse.x;
		event.cursor.y = Mouse.y;
		*(_DWORD*)&event.wSelection = 0;
		EventDialogs[EventNo::EVN_MOUSEMOVE]->pfcnInteract(EventDialogs[EventNo::EVN_MOUSEMOVE], &event);
	}
	EventDialogs[EventNo::EVN_MOUSEMOVE] = dlg;

	minimapGameMouseUpdate_(dlg);
}

FAIL_STUB_PATCH(MinimapGameRightclickEventMoveto);

void MinimapGameClickEvent_(dialog* dlg, dlgEvent* event)
{
	if (is_keycode_used[VK_MENU])
	{
		int x = event->cursor.x;
		int y = event->cursor.y;
		getMinimapCursorPos_(&x, &y);

		MinimapPingCommand command = { CommandId::CMD_MinimapPing, x, y };
		BWFXN_QueueCommand__(command);
	}
	else
	{
		ClipCursor(&stru_512D00);
		assignNextActiveDlgElement(dlg, 5);
		assignNextActiveDlgElement(dlg, 3);
		minimapGameMouseUpdate_(dlg);
	}
}

FAIL_STUB_PATCH(MinimapGameClickEvent);

void minimap_dlg_Activate_(dialog* dlg)
{
	switch (dlg->wIndex)
	{
	case 2:
		byte_6D5BBF ^= 1;
		dword_59C1A4 = 0;
		sub_4A4150_();
		drawAllMinimapBoxes_();
		sub_4A3870();
		if ((minimap_dialog->lFlags & CTRL_UPDATE) == 0)
		{
			minimap_dialog->lFlags |= CTRL_UPDATE;
			updateDialog(minimap_dialog);
		}
		drawShowHideTerrainContextHelp(dlg);
		break;
	case 3:
		refreshSelectionScreen_();
		byte_63FF70 = byte_581D60;
		BWFXN_OpenGameDialog_("rez\\msgfltr.bin", msgfltr_Main);
		break;
	case 4:
		refreshSelectionScreen_();
		MinimapControl_ShowAllianceDialog();
		break;
	}
}

FAIL_STUB_PATCH(minimap_dlg_Activate);

int __fastcall MinimapButton_EventHandler_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_MOUSEMOVE:
		minimap_dlg_MouseMove(dlg, evt);
		break;
	case EVN_USER:
		if (evt->dwUser == USER_CREATE)
		{
			Minimap_InitVisionButton(dlg);
		}
		else if (evt->dwUser == USER_ACTIVATE)
		{
			minimap_dlg_Activate_(dlg);
			if (dlg->wIndex == -3 || dlg->wIndex == -2)
			{
				LastControlID = dlg->wIndex;
				dlg->fields.ctrl.pDlg->pfcnInteract(dlg->fields.ctrl.pDlg, evt);
				return 1;
			}
		}
		break;
	}

	return GenericControlInteract(dlg, evt);
}

FAIL_STUB_PATCH(MinimapButton_EventHandler);

void MinimapGameTargetOrder_(dlgEvent* event)
{
	point p;
	p.x = event->cursor.x;
	p.y = event->cursor.y;

	getMinimapCursorPos_(&p.x, &p.y);
	resetGameInputProcs(CursorType::CUR_ARROW);
	__int16 x = p.x;
	__int16 y = p.y;
	sub_46F5B0(LOWORD(p.x), LOWORD(p.y), 0, 228);
	GroundAttackInit_(x, y);
}

FAIL_STUB_PATCH(MinimapGameTargetOrder);

void CMDACT_RightClick_(dlgEvent* dlg);

int __fastcall MinimapImageInteract_(dialog* dlg, dlgEvent* evt)
{
	switch (evt->wNo)
	{
	case EVN_MOUSEMOVE:
		minimapGameMouseUpdate_(dlg);
		return 1;
	case EVN_LBUTTONDOWN:
	case EVN_LBUTTONDBLCLK:
		if (IS_GAME_PAUSED)
		{
			return 1;
		}
		else if (is_placing_order)
		{
			MinimapGameTargetOrder_(evt);
			return 1;
		}
		else
		{
			MinimapGameClickEvent_(dlg, evt);
			return 1;
		}
	case EVN_LBUTTONUP:
	case EVN_RBUTTONUP:
		minimapGameResetMouseInput_(dlg);
		return 1;
	case EVN_RBUTTONDOWN:
	case EVN_RBUTTONDBLCLK:
		if (IS_GAME_PAUSED)
		{
			return 1;
		}
		else if (is_placing_building || is_placing_order)
		{
			MinimapGameRightclickEventMoveto_(dlg);
			return 1;
		}
		else
		{
			CMDACT_RightClick_(evt);
			return 1;
		}
	case EVN_USER:
		switch (evt->dwUser)
		{
		case USER_CREATE:
			minimapGameUpdate_(dlg);
			break;
		case USER_DESTROY:
		case USER_NEXT:
			minimapGameResetMouseInput_(dlg);
			break;
		case USER_MOUSEMOVE:
			return 1;
		default:
			break;
		}
	}

	return GenericDlgInteractFxns[dlg->wCtrlType](dlg, evt);
}

FAIL_STUB_PATCH(MinimapImageInteract);

void updateMinimapPreviewDlg_(dialog* dlg)
{
	static FnInteract menu_functions[] = {
		MinimapImageInteract_,
	};

	static FnInteract ingame_functions[] = {
		MinimapImageInteract_,
		MinimapButton_EventHandler_,
		MinimapButton_EventHandler_,
		MinimapButton_EventHandler_,
	};

	dlg->pfcnUpdate = MiniMapUpdate;
	dlg->lFlags = dlg->lFlags | CTRL_USELOCALGRAPHIC;
	int v1 = dword_5993AC;
	if (dword_5993AC == 1)
	{
		dlg->lFlags = dlg->lFlags | CTRL_USELOCALGRAPHIC | CTRL_TRANSPARENT;
		registerUserDialogAction(dlg, sizeof(menu_functions), menu_functions);
	}
	else if (dword_5993AC == 0)
	{
		registerUserDialogAction(dlg, sizeof(ingame_functions), ingame_functions);
		BINDLG_BlitSurface_(dlg);
		v1 = dword_5993AC;
	}
	else
	{
		registerUserDialogAction(dlg, sizeof(menu_functions), menu_functions);
	}

	minimap_dialog = getControlFromIndex_(minimap_Dlg, 1);
	if (v1 == 0)
	{
		setMinimapConstants();
		setMapSizeConstants_();
		if (dword_5993AC == 0)
		{
			SetCallbackTimer(1, dlg, 200, Minimap_TimerRefresh_);
			SetCallbackTimer(2, dlg, 200, updateMinimapPositioninfoProc);
			SetCallbackTimer(3, dlg, 200, updateMinimapSurfaceInfoProc_);
			SetCallbackTimer(6, dlg, 100, updateMinimapSurfaceInfo2Proc_);
			if (multiPlayerMode && (GetActivePlayerCount() > 1 || multiPlayerMode && gameData.got_file_values.template_id == 15) || InReplay)
			{
				SetCallbackTimer(5, dlg, 500, playerInfoSomethingTvBProc);
			}
			if (dword_5993AC == 0)
			{
				showDialog_(minimap_dialog);
			}
		}
	}
	if ((minimap_dialog->lFlags & CTRL_UPDATE) == 0)
	{
		minimap_dialog->lFlags |= CTRL_UPDATE;
		updateDialog(minimap_dialog);
	}
}

FAIL_STUB_PATCH(updateMinimapPreviewDlg);

void setMinimapConstants();
void setMapSizeConstants_();

int __fastcall MiniMapPreviewInteract_(dialog* dlg, dlgEvent* evt)
{
	dialog* v6;
	dialog* v7;

	switch (evt->wNo)
	{
	case EVN_KEYFIRST:
	case EVN_KEYRPT:
		return 0;
	case EVN_MOUSEMOVE:
		if (isSinglePaused())
		{
			return 0;
		}
		minimapPreviewMouseUpdate(dlg, evt);
		break;
	case EVN_LBUTTONDOWN:
	case EVN_LBUTTONDBLCLK:
	case EVN_RBUTTONDOWN:
	case EVN_RBUTTONDBLCLK:
		if (IS_GAME_PAUSED && !multiPlayerMode)
		{
			return 0;
		}
		v7 = dlgSetMouseOver(dlg, evt);
		if (!v7)
		{
			return 0;
		}
		if (v7 != dlg)
		{
			v7->pfcnInteract(v7, evt);
		}
		return 1;
	case EVN_USER:
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			updateMinimapPreviewDlg_(dlg);
			break;
		case EventUser::USER_DESTROY:
			killMinimapPreviewDlg(dlg);
			break;
		case EventUser::USER_MOUSEMOVE:
			return 1;
		}

		break;
	case EVN_CHAR:
		if (isSinglePaused() || dword_5993AC || evt->wVirtKey != VK_TAB)
		{
			return 0;
		}
		else
		{
			if (is_keycode_used[VK_SHIFT])
			{
				++byte_6D5BBE;
				if (byte_6D5BBE > 2)
				{
					byte_6D5BBE = 0;
					dword_6D5E1C = 1;
				}
				else if (byte_6D5BBE != 1)
				{
					dword_6D5E1C = 1;
				}
			}
			else
			{
				byte_6D5BBF = !byte_6D5BBF;
			}
			minimapPreviewUpdateState_();
			v6 = getControlFromIndex_(dlg, 2);
			if (SLOBYTE(v6->lFlags) < 0)
			{
				drawShowHideTerrainContextHelp(v6);
			}
			return 1;
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(MiniMapPreviewInteract);

void load_gluMinimap_()
{
	word_59C19C = MoveToTile.y;
	word_59CC70 = MoveToTile.x;
	dword_59C2B8 = 0;
	dword_59C1A8 = 0;
	blink_grp = (void*)LoadGraphic("game\\blink.grp", 0, "Starcraft\\SWAR\\lang\\minimap.cpp", 2011);
	if (!SBmpLoadImage("game\\tblink.pcx", 0, byte_59CAC0, sizeof(byte_59CAC0), 0, 0, 0))
	{
		throw FileNotFoundException("game\\tblink.pcx", SErrGetLastError());
	}

	for (int i = 0; i < _countof(palette); i++)
	{
		if (dword_5993AC)
		{
			byte_59CB60[i] = sub_4BDB30(stru_59C6C0, palette[i]);
		}
		else if (sub_4CB560(i))
		{
			byte_59CB60[i] = sub_4BDB30(palette, palette[i]);
		}
		else
		{
			byte_59CB60[i] = i;
		}
	}

	for (int i = 0; i < _countof(stru_59C1B8); i++)
	{
		stru_59C1B8[i].a0 = 0;
	}

	if (dword_5993AC)
	{
		setMinimapConstants();
		setMapSizeConstants_();
		minimapSurfaceUpdate();
		sub_4A4150_();
		drawAllMinimapBoxes_();
		sub_4A3870();
		if ((minimap_dialog->lFlags & DialogFlags::CTRL_UPDATE) == 0)
		{
			minimap_dialog->lFlags |= DialogFlags::CTRL_UPDATE;
			updateDialog(minimap_dialog);
		}
	}
	else
	{
		minimap_Dlg = LoadDialog("rez\\minimap.bin");
		InitializeDialog_(minimap_Dlg, MiniMapPreviewInteract_);
	}
}

FAIL_STUB_PATCH(load_gluMinimap);

void load_MinimapPreview_()
{
	minimap_Dlg = LoadDialog("rez\\minimappreview.bin");
	InitializeDialog_(minimap_Dlg, MiniMapPreviewInteract_);
}

FAIL_STUB_PATCH(load_MinimapPreview);
