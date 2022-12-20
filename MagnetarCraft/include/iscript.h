#include "starcraft_exe/types.h"

void ISCRIPT_setPosition_(CImage* image, char x, char y);
void init_iscript_program_state(IScriptProgramState* program_state, Anims animation);
void BWFXN_PlayIscript_(CImage* image, IScriptProgramState* program_state, int noop, _DWORD* distance_moved);
void PlayIscriptAnim_(CImage* image, Anims new_animation);
