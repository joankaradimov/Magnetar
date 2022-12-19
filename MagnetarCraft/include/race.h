#include "string"
#include "vector"
#include "starcraft.h"

class Race
{
public:
	const static std::vector<Race> races;

	const char id;
	UnitType base_building;
	UnitType worker;
	const MenuPosition ready_room_menu;
	const MenuPosition victory_menu;
	const MenuPosition defeat_menu;

	const char* get_ready_room_bin() const
	{
		return ready_room_bin.c_str();
	}

private:
	Race(char id, UnitType base_building, UnitType worker, MenuPosition ready_room_menu, MenuPosition victory_menu, MenuPosition defeat_menu) :
		id(id), base_building(base_building), worker(worker), ready_room_menu(ready_room_menu), victory_menu(victory_menu), defeat_menu(defeat_menu)
	{
		ready_room_bin = (std::string)"rez\\glurdy" + id + ".bin";
	}

	std::string ready_room_bin;
};
