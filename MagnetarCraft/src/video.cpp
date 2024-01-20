#include "magnetorm.h"
#include "patching.h"

#include "video.h"
#include "exception.h"

void sub_4D4440_()
{
	InitializeInputProcs_();
	input_procedures[EventNo::EVN_CHAR] = endVideoProc;
	input_procedures[EventNo::EVN_LBUTTONUP] = endVideoProc;
	input_procedures[EventNo::EVN_RBUTTONUP] = endVideoProc;
	dword_5967F0 = 0;
}

FAIL_STUB_PATCH(sub_4D4440, "starcraft");

void PlayMovie_(const char* cinematic, StormVideoFlags flags)
{
	RefreshCursor_0();
	sub_4D4440_();

	HANDLE video;

	SVidPlayBegin((char*) cinematic, 0, 0, 0, 0, flags, &video);
	if (video)
	{
		while (!dword_5967F0)
		{
			if (is_app_active && !SVidPlayContinueSingle(video, 0, 0))
			{
				break;
			}
			BWFXN_videoLoop_(3);
			Sleep(0);
		}
		SVidPlayEnd(video);
	}
	else
	{
		throw FileNotFoundException(cinematic, 2);
	}
}

void PlayMovie_(const struct CampaignMenuEntryEx& entry)
{
	PlayMovie_(cinematics[entry.cinematic], entry.video_flags);
}

FAIL_STUB_PATCH(PlayMovie, "starcraft");
