#pragma once

#include <array>
#include <memory>
#include <vector>

#include <peglib.h>

#include "starcraft_exe/types.h"

struct IScriptAnimationSet
{
    IScriptAnimationSet() : opcodes(nullptr)
    {
        for (size_t i = 0; i < animations.size(); i++)
        {
            animations[i] = 0xFFFF;
        }
    }

    bool is_used() const
    {
        return opcodes != nullptr;
    }

    std::shared_ptr<std::byte> opcodes;
    std::array<unsigned __int16, game::starcraft::Anims::AE_COUNT> animations = { 0 };
};

extern std::vector<IScriptAnimationSet> animation_sets;

class IScriptParser
{
public:
    IScriptParser();
    void parse(std::vector<IScriptAnimationSet>& animation_sets, const char* iscript_path);

private:
    static void merge_animation_sets(std::vector<IScriptAnimationSet>& destination, const std::vector<IScriptAnimationSet>& source);

    peg::parser iscript_parser;
};
