#include "lua/trigger_action_state.h"

#include "race.h"
#include "starcraft.h"

class Scenario
{
public:
	auto get_next_scenario()
	{
		return next_scenario;
	}

	void set_next_scenario(const char* scenario)
	{
		strcpy_s(next_scenario, scenario);
	}

	std::string to_string()
	{
		return "<Scenario>";
	}
};

TriggerActionState::TriggerActionState()
{
	state.open_libraries();

	state.new_usertype<Race>("Race",
		"new", sol::no_constructor,

		"__tostring", [](Race& race) {
			return (std::string)"<Race '" + race.id + "'>"; // TODO: use the full race name
		},

		"id", sol::property([](Race& race) { return race.id; })

		// TODO: expose default music playlist
	);

	state.new_usertype<PlayerInfo>("Player",
		"new", sol::no_constructor,

		"__tostring", [](PlayerInfo& player) {
			return (std::string)"<Player " + std::to_string(player.dwPlayerID + 1) + '>';
		},

		"name", sol::property([](PlayerInfo& player) {
			return player.szName; // TODO: fix indeterminisim by computer player name localizations
		}),

		"race", sol::property([](PlayerInfo& player) { return Race::races()[player.nRace]; })

		// TODO: expose force/team
		// TODO: expose type (human/computer/neutral)
		// TODO: expose music playlist
		// TODO: expose color, maybe?
	);

	state.new_usertype<Scenario>("Scenario",
		"new", sol::no_constructor,

		"next_scenario", sol::property(&Scenario::get_next_scenario, &Scenario::set_next_scenario),

		"players", sol::property([this](Scenario& scenario) {
			// TODO: fix indeterminism caused by the string representation including an address
			// TODO: avoid all this copying
			auto result = state.create_table();
			for (int i = 0; i < _countof(Players); i++)
			{
				result[i] = std::ref(Players[i]);
			}
			return result;
		})
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
