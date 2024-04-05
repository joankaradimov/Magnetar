#include "lua/trigger_action_state.h"

#include "starcraft.h"

TriggerActionState::TriggerActionState()
{
	state.set_function("print", [this](sol::object obj) {
		print(obj);
	});
}

void TriggerActionState::script(std::string_view code)
{
	state.script(code);
}

void TriggerActionState::print(const sol::object& obj)
{
	// TODO: this may include addresss; fix this indeterminism
	std::string message = state["tostring"](obj);
	print(message);
}

void TriggerActionState::print(const std::string& message)
{
	if (active_trigger_player == g_LocalNationID && !message.empty())
	{
		int display_time = getTextDisplayTime_(message.c_str());
		PrintText(message.c_str(), 2, display_time + GetTickCount(), 1);
	}
}
