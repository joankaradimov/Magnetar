#pragma once

#include "starcraft.h"

DEFINE_ENUM_FLAG_OPERATORS(StormVideoFlags);

const StormVideoFlags SVID_AUTOCUTSCENE =
    StormVideoFlags::SVID_FLAG_TOSCREEN |
    StormVideoFlags::SVID_FLAG_CLEARSCREEN |
    StormVideoFlags::SVID_FLAG_FULLSCREEN |
    StormVideoFlags::SVID_FLAG_AUTOSIZE |
    StormVideoFlags::SVID_FLAG_AUTOQUALITY;

void PlayMovie_(Cinematic cinematic);
