#include "string"
#include "vector"
#include "starcraft.h"

class Race
{
public:
	static const std::vector<Race>& races();

	const char id;
	UnitType base_building;
	UnitType worker;
	const char* dialog_graphics;
	const MenuPosition ready_room_menu;
	const MenuPosition victory_menu;
	const MenuPosition defeat_menu;
	const MusicTrackDescription briefing_music;
	const MusicTrackDescription victory_music;
	const MusicTrackDescription defeat_music;
	const std::vector<MusicTrackDescription> ingame_music;

	const char* get_ready_room_bin() const
	{
		return ready_room_bin.c_str();
	}

private:
	Race(char id, UnitType base_building, UnitType worker,
		char* dialog_graphics,
		MenuPosition ready_room_menu, MenuPosition victory_menu, MenuPosition defeat_menu,
		MusicTrackDescription briefing_music, MusicTrackDescription victory_music, MusicTrackDescription defeat_music,
		std::initializer_list<MusicTrackDescription> ingame_music
	):
		id(id), base_building(base_building), worker(worker),
		dialog_graphics(dialog_graphics),
		ready_room_menu(ready_room_menu), victory_menu(victory_menu), defeat_menu(defeat_menu),
		briefing_music(briefing_music), victory_music(victory_music), defeat_music(defeat_music), ingame_music(ingame_music)
	{
		ready_room_bin = (std::string)"rez\\glurdy" + id + ".bin";
	}

	std::string ready_room_bin;
};
