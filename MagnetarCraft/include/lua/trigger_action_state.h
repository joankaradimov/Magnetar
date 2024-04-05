#pragma once

#include <sol/sol.hpp>
#include <string_view>

class TriggerActionState
{
public:
    TriggerActionState();

    void script(std::string_view code);
    void print(const sol::object& obj);
    void print(const std::string& message);

private:
    sol::state state;
};
