#include "CFlingy.h"
#include "starcraft.h"
#include "patching.h"

void AppearOnTarget_(CFlingy* flingy, __int16 x, __int16 y)
{
    flingy->position.x = x;
    flingy->position.y = y;
    flingy->halt.x = x * 256;
    flingy->halt.y = y * 256;
    sub_497A10(flingy->sprite, x, y);
}

FAIL_STUB_PATCH(AppearOnTarget);
