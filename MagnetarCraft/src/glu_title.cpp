#include "starcraft.h"
#include "magnetorm.h"
#include "patching.h"
#include "exception.h"

void DrawBINDialog_(dialog* dlg)
{
	sub_4D45A0(dlg);
	if (!byte_51A0E9)
	{
		memset(stru_6CEB40, 0, sizeof(stru_6CEB40));
		byte_51A0E9 = 1;
		memcpy(stru_6CE720, GamePalette, sizeof(stru_6CE720));
		gluDlgFadePalette(3);
		BWFXN_RedrawTarget_();
	}
}

FAIL_STUB_PATCH(DrawBINDialog, "starcraft");

void titleInit_(dialog* dlg)
{
	void* buffer;
	int height;
	int width;

	if (!SBmpAllocLoadImage("glue\\title\\title.pcx", (int*)palette, &buffer, &width, &height, 0, 0, allocFunction))
	{
		throw FileNotFoundException("glue\\title\\title.pcx", SErrGetLastError());
	}
	dlg->srcBits.wid = width;
	dlg->srcBits.ht = height;
	dlg->srcBits.data = (u8*)buffer;
	memset(&stru_6CE000, 0, sizeof(stru_6CE000));

	if (!SBmpLoadImage("glue\\title\\tFont.pcx", 0, &stru_6CE000, 192, 0, 0, 0))
	{
		throw FileNotFoundException("glue\\title\\tFont.pcx", SErrGetLastError());
	}
	if (!low_memory)
	{
		DLGMusicFade_(&title_music);
	}

	if ((dlg->lFlags & DialogFlags::CTRL_UPDATE) == 0)
	{
		dlg->lFlags |= DialogFlags::CTRL_UPDATE;
		updateDialog_(dlg);
	}
	RefreshCursor_0_();
	memcpy(stru_6CEB40, palette, sizeof(stru_6CEB40));
	TitlePaletteUpdate_(3);
	TitleBlitAndLoop(dlg);
}

FAIL_STUB_PATCH(titleInit, "starcraft");

int __fastcall TitleDlgProc_(dialog* dlg, dlgEvent* evt)
{
	if (evt->wNo == EventNo::EVN_USER)
	{
		switch (evt->dwUser)
		{
		case EventUser::USER_CREATE:
			titleInit_(dlg);
			break;
		case EventUser::USER_DESTROY:
			DrawBINDialog_(dlg);
			break;
		}
	}
	return genericDlgInteract(dlg, evt);
}

FAIL_STUB_PATCH(TitleDlgProc, "starcraft");

void LoadTitle_()
{
	load_screen = LoadDialog("rez\\titledlg.bin");
	InitializeDialog_(load_screen, TitleDlgProc_);
}

FAIL_STUB_PATCH(LoadTitle, "starcraft");
