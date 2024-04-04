#include "lua/trigger_action_state.h"

#include "starcraft.h"

void lua_print(const char* message)
{
	if (active_trigger_player == g_LocalNationID && message)
	{
		int display_time = getTextDisplayTime_(message);
		PrintText(message, 2, display_time + GetTickCount(), 1);
	}
}

TriggerActionState::TriggerActionState()
{
	state.set_function("print", lua_print);
}

void TriggerActionState::script(std::string_view code)
{
	state.script(code);
}
