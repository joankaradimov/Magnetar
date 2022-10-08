#include "iscript.h"
#include "starcraft.h"
#include "patching.h"

__declspec(naked) void BWFXN_PlayIscript_original()
{
    __asm
    {
        push ebp
        mov ebp, esp
        sub esp, 40h
        mov eax, 0x4D74C6
        jmp eax
    }
}

void __stdcall BWFXN_PlayIscript__(IScriptProgram* program_state, int noop, int* distance_moved)
{
    CImage* image;

    __asm mov image, ecx

    __asm
    {
        mov ecx, image
        push distance_moved
        push noop
        push program_state
        call BWFXN_PlayIscript_original
    }
}

FUNCTION_PATCH((void*)0x4D74C0, BWFXN_PlayIscript__);
