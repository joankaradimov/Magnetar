#pragma once

#include <array>
#include <memory>
#include <vector>

#include <peglib.h>

#include "starcraft_exe/types.h"

struct IScriptAnimationSet
{
    IScriptAnimationSet()
    {
        for (int i = 0; i < animations.size(); i++)
        {
            animations[i] = 0xFFFF;
        }
    }

    std::shared_ptr<std::byte> opcodes;
    std::array<unsigned __int16, game::starcraft::Anims::AE_COUNT> animations = { 0 };
};

extern std::vector<IScriptAnimationSet> animation_sets;

class IScriptParser
{
public:
    IScriptParser();
    bool parse(const char* iscript_path);

private:
    peg::parser iscript_parser;
};
