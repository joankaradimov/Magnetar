#pragma once

#include <sol/sol.hpp>
#include <string_view>

class TriggerActionState
{
public:
    TriggerActionState();

    void script(std::string_view code);

private:
    sol::state state;
};
