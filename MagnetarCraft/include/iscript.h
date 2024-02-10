#include "starcraft_exe/types.h"

using namespace game::starcraft;

void ISCRIPT_setPosition_(CImage* image, char x, char y);
void init_iscript_program_state(IScriptProgramState* program_state, Anims animation);
void BWFXN_PlayIscript_(CImage* image, IScriptProgramState* program_state);
void BWFXN_PlayIscript_noop(CImage* image, IScriptProgramState* program_state, _DWORD* distance_moved);
void PlayIscriptAnim_(CImage* image, Anims new_animation);
