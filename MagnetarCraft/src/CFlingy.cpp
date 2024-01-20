#include "CFlingy.h"
#include "starcraft.h"
#include "patching.h"

void sub_497A10_(CSprite* sprite, __int16 x, __int16 y);

void AppearOnTarget_(CFlingy* flingy, __int16 x, __int16 y)
{
    flingy->position.x = x;
    flingy->position.y = y;
    flingy->halt.x = x * 256;
    flingy->halt.y = y * 256;
    sub_497A10_(flingy->sprite, x, y);
}

FAIL_STUB_PATCH(AppearOnTarget, "starcraft");

void sub_4958C0_(CFlingy* flingy)
{
    if ((__int16) flingy->position.x < 0)
    {
        AppearOnTarget_(flingy, 0, flingy->position.y);
    }
    else if ((__int16)flingy->position.x >= (int)(unsigned __int16)map_width_pixels)
    {
        AppearOnTarget_(flingy, map_width_pixels - 1, flingy->position.y);
    }

    if ((__int16)flingy->position.y < 0)
    {
        AppearOnTarget_(flingy, flingy->position.x, 0);
    }
    else if ((__int16)flingy->position.y >= (unsigned __int16)map_height_pixels)
    {
        AppearOnTarget_(flingy, flingy->position.x, map_height_pixels - 1);
    }
}

FAIL_STUB_PATCH(sub_4958C0, "starcraft");
