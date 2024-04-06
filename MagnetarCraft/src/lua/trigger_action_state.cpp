#include "lua/trigger_action_state.h"

#include "starcraft.h"

class Scenario
{
public:
	Scenario() {}

	std::string get_next_scenario()
	{
		return next_scenario;
	}

	void set_next_scenario(const char* scenario)
	{
		strcpy_s(next_scenario, scenario);
	}
};

TriggerActionState::TriggerActionState()
{
	state.new_usertype<Scenario>("Scenario",
		"new", sol::no_constructor,
		"next_scenario", sol::property(&Scenario::get_next_scenario, &Scenario::set_next_scenario)
	);

	state.set_function("print", [this](sol::object obj) {
		print(obj);
	});

	// TODO: tweak the 'globals' metatable so that this can't be overwriten
	state.globals()["scenario"] = Scenario();
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
