#include <stdio.h>
#include "starcraft.h"

struct SpriteTileDataEx
{
    CSprite* tails[MAX_MAP_DIMENTION];
    CSprite* heads[MAX_MAP_DIMENTION];
};

extern SpriteTileDataEx _SpritesOnTileRow;
