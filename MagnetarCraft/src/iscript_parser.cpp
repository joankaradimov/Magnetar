#include <peglib.h>
#include <array>
#include <unordered_map>
#include <stdexcept>
#include <string>
#include <vector>

#include "starcraft.h"

struct LabelReference
{
    LabelReference() : LabelReference(std::string_view(), std::pair(0, 0))
    {
    }

    LabelReference(std::string_view name, std::pair<size_t, size_t> line_info):
        name(name), line(line_info.first), column(line_info.second)
    {
    }

    const LabelReference& operator=(const LabelReference& other)
    {
        name = other.name;
        line = other.line;
        column = other.column;

        return *this;
    }

    size_t line;
    size_t column;
    std::string_view name;
};

struct AnimationHeader
{
    AnimationHeader(std::pair<size_t, size_t> line_info) :
        line(line_info.first), column(line_info.second)
    {
        is_id = 0;
        type = Anims::AE_COUNT - 2;
        for (int i = 0; i < Anims::AE_COUNT; i++)
        {
            animations[i] = LabelReference();
        }
    }

    int infer_type() const
    {
        for (int i = Anims::AE_COUNT - 1; i > 0; i--)
        {
            if (!animations[i].name.empty())
            {
                return i - 1;
            }
        }
        return 0;
    }

    size_t line;
    size_t column;
    unsigned __int16 is_id;
    unsigned __int16 type;
    std::array<LabelReference, Anims::AE_COUNT> animations;

};

class IScriptBuilder
{
public:
    template <typename T> IScriptBuilder& operator<<(T data)
    {
        for (int i = 0; i < sizeof(T); i++)
        {
            bytes.append(1, std::byte(BYTEn(data, i)));
        }
        return *this;
    }

    IScriptBuilder& operator<<(LabelReference label)
    {
        if (label_offsets.find(label.name) != label_offsets.end())
        {
            auto label_offset = label_offsets[label.name];
            return *this << label_offset;
        }

        if (label_backpatch_list.find(label.name) == label_backpatch_list.end())
        {
            label_backpatch_list[label.name] = std::vector<std::pair<unsigned __int16, LabelReference>>();
        }
        label_backpatch_list[label.name].push_back(std::pair(current_offset(), label));

        return *this << (unsigned __int16) 0xDEAD;
    }

    const std::basic_string<std::byte>& iscript_bin() const
    {
        for (const auto& label_backpatches : label_backpatch_list)
        {
            for (const auto& backpatch_item : label_backpatches.second)
            {
                const auto& label = backpatch_item.second;

                std::string error_messge = "IScript error: label '" + std::string(label.name) + "' not found\n";
                error_messge += "\nline: " + std::to_string(label.line);
                error_messge += "\ncolumn: " + std::to_string(label.column);
                throw std::runtime_error(error_messge);
            }
        }

        return bytes;
    }

    const std::vector<unsigned __int16> build_headers() const
    {
        std::vector<unsigned __int16> result;

        for (const auto& header : headers)
        {
            int inferred_type = header.infer_type();

            if (inferred_type > header.type)
            {
                std::string error_messge = "IScript error: labels defined outside the type limitations\n";
                error_messge += "\nIsId: " + std::to_string(header.is_id);
                error_messge += "\nline: " + std::to_string(header.line);
                error_messge += "\ncolumn: " + std::to_string(header.column);
                throw std::runtime_error(error_messge);
            }

            result.push_back(header.is_id);
            for (int i = 0; i < inferred_type + 2; i++)
            {
                LabelReference label = header.animations[i];
                if (label.name.empty())
                {
                    result.push_back(0);
                }
                else if (label_offsets.find(label.name) == label_offsets.end())
                {
                    std::string error_messge = "IScript error: label '" + std::string(label.name) + "' not found\n";
                    error_messge += "\nIsId: " + std::to_string(header.is_id);
                    error_messge += "\nline: " + std::to_string(label.line);
                    error_messge += "\ncolumn: " + std::to_string(label.column);
                    throw std::runtime_error(error_messge);
                }
                else
                {
                    result.push_back(label_offsets.at(label.name));
                }
            }
        }

        return result;
    }

    void mark_label(const LabelReference& label)
    {
        unsigned __int16 label_offset = current_offset();
        label_offsets[label.name] = label_offset;

        for (const auto& item: label_backpatch_list[label.name])
        {
            unsigned __int16 address = item.first;
            bytes[address + 0] = (std::byte)BYTEn(label_offset, 0);
            bytes[address + 1] = (std::byte)BYTEn(label_offset, 1);
        }
        label_backpatch_list.erase(label.name);
    }

    void append_header(std::pair<size_t, size_t> line_info)
    {
        headers.emplace_back(line_info);
    }

    void set_current_is_id(int is_id)
    {
        current_header().is_id = is_id;
    }

    void set_current_type(int type)
    {
        current_header().type = type;
    }

    void set_animation_label(Anims animation, LabelReference label)
    {
        current_header().animations[animation] = label;
    }

private:
    unsigned __int16 current_offset() const
    {
        return bytes.size();
    }

    AnimationHeader& current_header()
    {
        return headers.back();
    }

    std::basic_string<std::byte> bytes;
    std::unordered_map<std::string_view, unsigned __int16> label_offsets;
    std::unordered_map<std::string_view, std::vector<std::pair<unsigned __int16, LabelReference>>> label_backpatch_list;
    std::vector<AnimationHeader> headers;
    int max_is_id;
};

bool parse_iscript_txt()
{
    peg::parser iscript_parser(R"(
        ROOT <- (_ (EOL / &'.' HEADER / LABEL / OP _ EOL))* EOF

        HEADER <- '.headerstart' _ EOL _ (!'.' HEADER_LINE? _ EOL _)* '.headerend' _ EOL
        LABEL  <- <ID> _ ':' _ NL?

        # header rules:
        HEADER_IS_ID     <- 'IsId' __ INT
        HEADER_TYPE      <- 'Type' __ INT
        HEADER_ANIMATION <- ANIMATION __ ID_MAYBE

        ANIMATION <- 'Init' / 'Death' / 'GndAttkInit' / 'AirAttkInit' / 'Unused1' / 'GndAttkRpt' /
                     'AirAttkRpt' / 'CastSpell' / 'GndAttkToIdle' / 'AirAttkToIdle' / 'Unused2' /
                     !'WalkingTo' 'Walking' / 'WalkingToIdle' / 'SpecialState1' / 'SpecialState2' /
                     'AlmostBuilt' / 'Built' / 'Landing' / 'LiftOff' / 'IsWorking' / 'WorkingToIdle' /
                     'WarpIn' / 'Unused3' / 'StarEditInit' / 'Disable' / 'Burrow' / 'UnBurrow' / 'Enable'

        HEADER_LINE <- HEADER_IS_ID / HEADER_TYPE / HEADER_ANIMATION / { error_message "Unrecognized animation" }

        # opcode rules
        OPC_IMGUL             <- 'imgul' __ INT __ INT __ INT
        OPC_IMGULNEXTID       <- 'imgulnextid' __ INT __ INT
        OPC_IMGULUSELO        <- 'imguluselo' __ INT __ INT __ INT
        OPC_IMGOL             <- 'imgol' __ INT __ INT __ INT
        OPC_IMGOLORIG         <- 'imgolorig' __ INT
        OPC_IMGOLUSELO        <- 'imgoluselo' __ INT __ INT __ INT
        OPC_SPROL             <- 'sprol' __ INT __ INT __ INT
        OPC_SPROLUSELO        <- 'sproluselo' __ INT __ INT
        OPC_SPRUL             <- 'sprul' __ INT __ INT __ INT
        OPC_SPRULUSELO        <- 'spruluselo' __ INT __ INT __ INT
        OPC_GRDSPROL          <- 'grdsprol' __ INT __ INT __ INT
        OPC_WARPOVERLAY       <- 'warpoverlay' __ INT
        OPC_SWITCHUL          <- 'switchul' __ INT
        OPC_WAITRAND          <- 'waitrand' __ INT __ INT
        OPC_SETVERTPOS        <- 'setvertpos' __ INT
        OPC_GOTO              <- 'goto' __ ID
        OPC_WAIT              <- 'wait' __ INT
        OPC_PLAYFRAM          <- 'playfram' __ INT
        OPC_PLAYFRAMTILE      <- 'playframtile' __ INT
        OPC_ENGFRAME          <- 'engframe' __ INT
        OPC_PLAYSND           <- 'playsnd' __ INT
        OPC_END               <- 'end'
        OPC_ATTACKWITH        <- 'attackwith' __ INT
        OPC_LOWSPRUL          <- 'lowsprul' __ INT __ INT __ INT
        OPC_NOBRKCODESTART    <- 'nobrkcodestart'
        OPC_NOBRKCODEEND      <- 'nobrkcodeend'
        OPC_ATTACKMELEE       <- 'attackmelee' __ INT (__ INT)*
        OPC_GOTOREPEATATTK    <- 'gotorepeatattk'
        OPC_CALL              <- 'call' __ ID
        OPC_RETURN            <- 'return'
        OPC_IGNOREREST        <- 'ignorerest'
        OPC_RANDCONDJMP       <- 'randcondjmp' __ INT __ ID
        OPC_LIFTOFFCONDJMP    <- 'liftoffcondjmp' __ ID
        OPC_TRGTARCCONDJMP    <- 'trgtarccondjmp' __ INT __ INT __ ID
        OPC_TRGTRANGECONDJMP  <- 'trgtrangecondjmp' __ INT __ ID
        OPC_CURDIRECTCONDJMP  <- 'curdirectcondjmp' __ INT __ INT __ ID
        OPC_PWRUPCONDJMP      <- 'pwrupcondjmp' __ ID
        OPC_MOVE              <- 'move' __ INT
        OPC_SETFLDIRECT       <- 'setfldirect' __ INT
        OPC_SIGORDER          <- 'sigorder' __ INT
        OPC_ORDERDONE         <- 'orderdone' __ INT
        OPC_ATTACK            <- 'attack'
        OPC_ATTKSHIFTPROJ     <- 'attkshiftproj' __ INT
        OPC_CASTSPELL         <- 'castspell'
        OPC_USEWEAPON         <- 'useweapon' __ INT
        OPC_DOMISSILEDMG      <- 'domissiledmg'
        OPC_DOGRDDAMAGE       <- 'dogrddamage'
        OPC_FOLLOWMAINGRAPHIC <- 'followmaingraphic'
        OPC_TURN1CWISE        <- 'turn1cwise'
        OPC_TURNCWISE         <- 'turncwise' __ INT
        OPC_TURNCCWISE        <- 'turnccwise' __ INT
        OPC_TURNRAND          <- 'turnrand' __ INT
        OPC_SETFLSPEED        <- 'setflspeed' __ INT
        OPC_TMPRMGRAPHICSTART <- 'tmprmgraphicstart'
        OPC_TMPRMGRAPHICEND   <- 'tmprmgraphicend'
        OPC_PLAYSNDBTWN       <- 'playsndbtwn' __ INT __ INT
        OPC_PLAYSNDRAND       <- 'playsndrand' __ INT (__ INT)*
        OPC_CREATEGASOVERLAYS <- 'creategasoverlays' __ INT
        OPC_ENGSET            <- 'engset' __ INT
        OPC_SETFILPSTATE      <- 'setflipstate' __ INT
        OPC_SETPOS            <- 'setpos' __ INT __ INT
        OPC_SETSPAWNFRAME     <- 'setspawnframe' __ INT
        OPC_SETHORPOS         <- 'sethorpos' __ INT
        OPC___2D              <- '__2d'
        OP <- OPC_WAIT / OPC_PLAYFRAM / OPC_GOTO / OPC_END / OPC_MOVE / OPC_PLAYFRAMTILE /
              OPC_IMGUL / OPC_IMGULNEXTID / OPC_IMGULUSELO /
              OPC_IMGOL / OPC_IMGOLUSELO / OPC_IMGOLORIG /
              OPC_SPROL / OPC_SPROLUSELO / OPC_SPRUL / OPC_SPRULUSELO / OPC_GRDSPROL /
              OPC_WARPOVERLAY / OPC_SWITCHUL / OPC_WAITRAND /
              OPC_SETVERTPOS / OPC_RANDCONDJMP / OPC_PLAYSND /
              OPC_LOWSPRUL / OPC_NOBRKCODESTART / OPC_NOBRKCODEEND /
              OPC_GOTOREPEATATTK / OPC_CALL / OPC_RETURN / OPC_IGNOREREST /
              OPC_LIFTOFFCONDJMP / OPC_TRGTARCCONDJMP / OPC_TRGTRANGECONDJMP /
              OPC_CURDIRECTCONDJMP / OPC_PWRUPCONDJMP /
              OPC_SIGORDER / OPC_ORDERDONE /
              OPC_ATTACKMELEE / OPC_ATTACKWITH / OPC_ATTACK / OPC_ATTKSHIFTPROJ /
              OPC_CASTSPELL / OPC_USEWEAPON / OPC_DOMISSILEDMG / OPC_DOGRDDAMAGE /
              OPC_FOLLOWMAINGRAPHIC / OPC_SETFLDIRECT / OPC_CREATEGASOVERLAYS /
              OPC_TURN1CWISE / OPC_TURNCWISE / OPC_TURNCCWISE / OPC_TURNRAND /
              OPC_SETFLSPEED / OPC_TMPRMGRAPHICSTART / OPC_TMPRMGRAPHICEND /
              OPC_PLAYSNDBTWN / OPC_PLAYSNDRAND / OPC_ENGSET / OPC_ENGFRAME /
              OPC_SETFILPSTATE / OPC_SETPOS / OPC_SETSPAWNFRAME / OPC_SETHORPOS /
              OPC___2D /
              { error_message "Unrecognized instruction" }

        ID       <- [_a-zA-Z][_a-zA-Z0-9]*
        ID_MAYBE <- '[none]'i / ID
        DEC      <- '-'? [1-9][0-9]* / '-'? '0'
        HEX      <- '0x' [0-9a-fA-F]+
        INT      <- HEX / DEC

        ~EOF     <- !.
        ~EOL     <- &'#' COMMENT NL / NL
        ~COMMENT <- '#' [^\r\n]*
        ~NL      <- ('\n' | '\r\n' | '\r')
        ~_       <- [ \t]* # Optional whitespace
        ~__      <- [ \t]+ # Mandatory whitespace
    )");

    IScriptBuilder builder;

    iscript_parser["ID"] = [](const peg::SemanticValues& vs) {
        return LabelReference(vs.token(), vs.line_info());
    };

    iscript_parser["ID_MAYBE"] = [](const peg::SemanticValues& vs) {
        return LabelReference(vs.choice() == 0 ? std::string_view() : vs.token(), vs.line_info());
    };

    iscript_parser["DEC"] = [](const peg::SemanticValues& vs) {
        return vs.token_to_number<int>();
    };

    iscript_parser["HEX"] = [](const peg::SemanticValues& vs) {
        int result = 0;
        std::string_view hex_digits = vs.token().substr(2); // skip the initial "0x"

        for (auto digit : hex_digits)
        {
            // This abuses the ASCII order a bit... but it's fine.
            result = 0x10 * result + (digit >= 'a' ? digit - 'a' + 10 : digit >= 'A' ? digit - 'A' + 10 : digit - '0');
        }
        return result;
    };

    iscript_parser["HEADER"].enter = [&builder](const peg::Context& context, const char* s, size_t n, std::any& dt) {
        builder.append_header(context.line_info(s));
    };

    iscript_parser["ANIMATION"] = [&builder](const peg::SemanticValues& vs) {
        return (Anims) vs.choice();
    };

    iscript_parser["HEADER_IS_ID"] = [&builder](const peg::SemanticValues& vs) {
        auto arg = std::any_cast<int>(vs[0]);

        builder.set_current_is_id(arg);
    };

    iscript_parser["HEADER_TYPE"] = [&builder](const peg::SemanticValues& vs) {
        auto arg = std::any_cast<int>(vs[0]);

        builder.set_current_type(arg);
    };

    iscript_parser["HEADER_ANIMATION"] = [&builder](const peg::SemanticValues& vs) {
        auto animation = std::any_cast<Anims>(vs[0]);
        auto label = std::any_cast<LabelReference>(vs[1]);

        builder.set_animation_label(animation, label);
    };

    iscript_parser["LABEL"] = [&builder](const peg::SemanticValues& vs) {
        auto label = std::any_cast<LabelReference>(vs[0]);
        builder.mark_label(label);
    };

    iscript_parser["OPC_PLAYFRAM"] = [&builder](const peg::SemanticValues& vs) {
        u16 frame = std::any_cast<int>(vs[0]);

        builder << IScriptOpcodes::opc_playfram << frame;
    };

    iscript_parser["OPC_PLAYFRAMTILE"] = [&builder](const peg::SemanticValues& vs) {
        u16 frame = std::any_cast<int>(vs[0]);

        builder << IScriptOpcodes::opc_playframtile << frame;
    };

    iscript_parser["OPC_SETHORPOS"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);

        builder << IScriptOpcodes::opc_sethorpos << arg;
    };

    iscript_parser["OPC_SETVERTPOS"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);

        builder << IScriptOpcodes::opc_setvertpos << arg;
    };

    iscript_parser["OPC_SETPOS"] = [&builder](const peg::SemanticValues& vs) {
        u8 x = std::any_cast<int>(vs[0]);
        u8 y = std::any_cast<int>(vs[1]);

        builder << IScriptOpcodes::opc_setpos << x << y;
    };

    iscript_parser["OPC_WAIT"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);

        builder << IScriptOpcodes::opc_wait << arg;
    };

    iscript_parser["OPC_WAITRAND"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg1 = std::any_cast<int>(vs[0]);
        u8 arg2 = std::any_cast<int>(vs[1]);

        builder << IScriptOpcodes::opc_waitrand << arg1 << arg2;
    };

    iscript_parser["OPC_GOTO"] = [&builder](const peg::SemanticValues& vs) {
        auto label = std::any_cast<LabelReference>(vs[0]);

        builder << IScriptOpcodes::opc_goto << label;
    };

    iscript_parser["OPC_IMGOL"] = [&builder](const peg::SemanticValues& vs) {
        u16 image_id = std::any_cast<int>(vs[0]);
        u8 x = std::any_cast<int>(vs[1]);
        u8 y = std::any_cast<int>(vs[2]);

        builder << IScriptOpcodes::opc_imgol << image_id << x << y;
    };

    iscript_parser["OPC_IMGUL"] = [&builder](const peg::SemanticValues& vs) {
        u16 image_id = std::any_cast<int>(vs[0]);
        u8 x = std::any_cast<int>(vs[1]);
        u8 y = std::any_cast<int>(vs[2]);

        builder << IScriptOpcodes::opc_imgul << image_id << x << y;
    };

    iscript_parser["OPC_IMGOLORIG"] = [&builder](const peg::SemanticValues& vs) {
        u16 arg = std::any_cast<int>(vs[0]);

        builder << IScriptOpcodes::opc_imgolorig << arg;
    };

    iscript_parser["OPC_SWITCHUL"] = [&builder](const peg::SemanticValues& vs) {
        u16 arg = std::any_cast<int>(vs[0]);

        builder << IScriptOpcodes::opc_switchul << arg;
    };

    // iscript_parser["OPC___0C"] = [&builder](const peg::SemanticValues& vs) {
    //     u16 arg = std::any_cast<int>(vs[0]);
    //
    //     builder << IScriptOpcodes::opc___0c << arg;
    // };

    iscript_parser["OPC_IMGOLUSELO"] = [&builder](const peg::SemanticValues& vs) {
        u16 image_id = std::any_cast<int>(vs[0]);
        OverlayType x = (OverlayType)std::any_cast<int>(vs[1]);
        u8 y = std::any_cast<int>(vs[2]);

        builder << IScriptOpcodes::opc_imgoluselo << image_id << x << y;
    };


    iscript_parser["OPC_IMGULUSELO"] = [&builder](const peg::SemanticValues& vs) {
        u16 image_id = std::any_cast<int>(vs[0]);
        OverlayType x = (OverlayType)std::any_cast<int>(vs[1]);
        u8 y = std::any_cast<int>(vs[2]);

        builder << IScriptOpcodes::opc_imguluselo << image_id << x << y;
    };

    iscript_parser["OPC_SPROL"] = [&builder](const peg::SemanticValues& vs) {
        u16 sprite_id = std::any_cast<int>(vs[0]);
        u8 x = std::any_cast<int>(vs[1]);
        u8 y = std::any_cast<int>(vs[2]);

        builder << IScriptOpcodes::opc_sprol << sprite_id << x << y;
    };

    //iscript_parser["OPC_HIGHSPROL"] = [&builder](const peg::SemanticValues& vs) {
    //    u16 sprite_id = std::any_cast<int>(vs[0]);
    //    u8 x = std::any_cast<int>(vs[1]);
    //    u8 y = std::any_cast<int>(vs[2]);
    //
    //    builder << IScriptOpcodes::opc_highsprol << sprite_id << x << y;
    //};

    iscript_parser["OPC_LOWSPRUL"] = [&builder](const peg::SemanticValues& vs) {
        u16 sprite_id = std::any_cast<int>(vs[0]);
        u8 x = std::any_cast<int>(vs[1]);
        u8 y = std::any_cast<int>(vs[2]);

        builder << IScriptOpcodes::opc_lowsprul << sprite_id << x << y;
    };

    //iscript_parser["OPC_UFLUNSTABLE"] = [&builder](const peg::SemanticValues& vs) {
    //    u16 flingy_id = std::any_cast<int>(vs[0]);
    //
    //    builder << IScriptOpcodes::opc_uflunstable << flingy_id;
    //};

    iscript_parser["OPC_SPRULUSELO"] = [&builder](const peg::SemanticValues& vs) {
        u16 sprite_id = std::any_cast<int>(vs[0]);
        u8 x = std::any_cast<int>(vs[1]);
        u8 y = std::any_cast<int>(vs[2]);

        builder << IScriptOpcodes::opc_spruluselo << sprite_id << x << y;
    };

    iscript_parser["OPC_SPRUL"] = [&builder](const peg::SemanticValues& vs) {
        u16 sprite_id = std::any_cast<int>(vs[0]);
        u8 x = std::any_cast<int>(vs[1]);
        u8 y = std::any_cast<int>(vs[2]);

        builder << IScriptOpcodes::opc_sprul << sprite_id << x << y;
    };

    iscript_parser["OPC_SPROLUSELO"] = [&builder](const peg::SemanticValues& vs) {
        u16 sprite_id = std::any_cast<int>(vs[0]);
        OverlayType overlay_type = (OverlayType) std::any_cast<int>(vs[1]);
        builder << IScriptOpcodes::opc_sproluselo << sprite_id << overlay_type;
    };

    iscript_parser["OPC_END"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_end;
    };

    //iscript_parser["OPC_SETFLIPSTATE"] = [&builder](const peg::SemanticValues& vs) {
    //    u8 arg = std::any_cast<int>(vs[0]);
    //    builder << IScriptOpcodes::opc_setflipstate << arg;
    //};

    iscript_parser["OPC_PLAYSND"] = [&builder](const peg::SemanticValues& vs) {
        u16 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_playsnd << arg;
    };

    iscript_parser["OPC_PLAYSNDRAND"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg_count = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_playsndrand << arg_count;
        for (int i = 0; i < arg_count; i++)
        {
            u16 sound_id = std::any_cast<int>(vs[i + 1]);
            builder << sound_id;
        }
    };

    iscript_parser["OPC_PLAYSNDBTWN"] = [&builder](const peg::SemanticValues& vs) {
        u16 min = std::any_cast<int>(vs[0]);
        u16 max = std::any_cast<int>(vs[1]);
        builder << IScriptOpcodes::opc_playsndbtwn << min << max;
    };

    iscript_parser["OPC_DOMISSILEDMG"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_domissiledmg;
    };

    iscript_parser["OPC_ATTACKMELEE"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg_count = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_attackmelee << arg_count;
        for (int i = 0; i < arg_count; i++)
        {
            u16 sound_id = std::any_cast<int>(vs[i + 1]);
            builder << sound_id;
        }
    };

    iscript_parser["OPC_FOLLOWMAINGRAPHIC"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_followmaingraphic;
    };

    iscript_parser["OPC_RANDCONDJMP"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        auto label = std::any_cast<LabelReference>(vs[1]);

        builder << IScriptOpcodes::opc_randcondjmp << arg << label;
    };

    iscript_parser["OPC_TURNCCWISE"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_turnccwise << arg;
    };

    iscript_parser["OPC_TURNCWISE"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_turncwise << arg;
    };

    iscript_parser["OPC_TURN1CWISE"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_turn1cwise;
    };

    iscript_parser["OPC_TURNRAND"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_turnrand << arg;
    };

    iscript_parser["OPC_SETSPAWNFRAME"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_setspawnframe << arg;
    };

    iscript_parser["OPC_SIGORDER"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_sigorder << arg;
    };

    iscript_parser["OPC_ATTACKWITH"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_attackwith << arg;
    };

    iscript_parser["OPC_ATTACK"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_attack;
    };

    iscript_parser["OPC_CASTSPELL"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_castspell;
    };

    iscript_parser["OPC_USEWEAPON"] = [&builder](const peg::SemanticValues& vs) {
        WeaponType weapon_Id = (WeaponType) std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_useweapon << weapon_Id;
    };

    iscript_parser["OPC_MOVE"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_move << arg;
    };

    iscript_parser["OPC_GOTOREPEATATTK"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_gotorepeatattk;
    };

    iscript_parser["OPC_ENGFRAME"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_engframe << arg;
    };

    iscript_parser["OPC_ENGSET"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_engset << arg;
    };

    iscript_parser["OPC___2D"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc___2d;
    };

    iscript_parser["OPC_NOBRKCODESTART"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_nobrkcodestart;
    };

    iscript_parser["OPC_NOBRKCODEEND"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_nobrkcodeend;
    };

    iscript_parser["OPC_IGNOREREST"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_ignorerest;
    };

    iscript_parser["OPC_ATTKSHIFTPROJ"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_attkshiftproj << arg;
    };

    iscript_parser["OPC_TMPRMGRAPHICSTART"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_tmprmgraphicstart;
    };

    iscript_parser["OPC_TMPRMGRAPHICEND"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_tmprmgraphicend;
    };

    iscript_parser["OPC_SETFLDIRECT"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_setfldirect << arg;
    };

    iscript_parser["OPC_CALL"] = [&builder](const peg::SemanticValues& vs) {
        auto label = std::any_cast<LabelReference>(vs[0]);
        builder << IScriptOpcodes::opc_call << label;
    };

    iscript_parser["OPC_RETURN"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_return;
    };

    iscript_parser["OPC_SETFLSPEED"] = [&builder](const peg::SemanticValues& vs) {
        u16 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_setflspeed << arg;
    };

    iscript_parser["OPC_CREATEGASOVERLAYS"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);
        builder << IScriptOpcodes::opc_creategasoverlays << arg;
    };

    iscript_parser["OPC_PWRUPCONDJMP"] = [&builder](const peg::SemanticValues& vs) {
        auto label = std::any_cast<LabelReference>(vs[0]);
        builder << IScriptOpcodes::opc_pwrupcondjmp << label;
    };

    iscript_parser["OPC_TRGTRANGECONDJMP"] = [&builder](const peg::SemanticValues& vs) {
        u16 arg = std::any_cast<int>(vs[0]);
        auto label = std::any_cast<LabelReference>(vs[1]);
        builder << IScriptOpcodes::opc_trgtrangecondjmp << arg << label;
    };

    iscript_parser["OPC_TRGTARCCONDJMP"] = [&builder](const peg::SemanticValues& vs) {
        u16 arg1 = std::any_cast<int>(vs[0]);
        u16 arg2 = std::any_cast<int>(vs[1]);
        auto label = std::any_cast<LabelReference>(vs[2]);
        builder << IScriptOpcodes::opc_trgtarccondjmp << arg1 << arg2 << label;
    };

    iscript_parser["OPC_CURDIRECTCONDJMP"] = [&builder](const peg::SemanticValues& vs) {
        u16 arg1 = std::any_cast<int>(vs[0]);
        u16 arg2 = std::any_cast<int>(vs[1]);
        auto label = std::any_cast<LabelReference>(vs[2]);
        builder << IScriptOpcodes::opc_curdirectcondjmp << arg1 << arg2 << label;
    };

    iscript_parser["OPC_IMGULNEXTID"] = [&builder](const peg::SemanticValues& vs) {
        u8 x = std::any_cast<int>(vs[0]);
        u8 y = std::any_cast<int>(vs[1]);

        builder << IScriptOpcodes::opc_imgulnextid << x << y;
    };

    // iscript_parser["OPC___3E"] = [&builder](const peg::SemanticValues& vs) {
    //     builder << IScriptOpcodes::opc___3e;
    // };

    iscript_parser["OPC_LIFTOFFCONDJMP"] = [&builder](const peg::SemanticValues& vs) {
        auto label = std::any_cast<LabelReference>(vs[0]);
        builder << IScriptOpcodes::opc_liftoffcondjmp << label;
    };

    iscript_parser["OPC_WARPOVERLAY"] = [&builder](const peg::SemanticValues& vs) {
        u16 arg = std::any_cast<int>(vs[0]);

        builder << IScriptOpcodes::opc_warpoverlay << arg;
    };

    iscript_parser["OPC_ORDERDONE"] = [&builder](const peg::SemanticValues& vs) {
        u8 arg = std::any_cast<int>(vs[0]);

        builder << IScriptOpcodes::opc_orderdone << arg;
    };

    iscript_parser["OPC_GRDSPROL"] = [&builder](const peg::SemanticValues& vs) {
        u16 sprite_id = std::any_cast<int>(vs[0]);
        u8 x = std::any_cast<int>(vs[1]);
        u8 y = std::any_cast<int>(vs[2]);

        builder << IScriptOpcodes::opc_grdsprol << sprite_id << x << y;
    };

    // iscript_parser["OPC___43"] = [&builder](const peg::SemanticValues& vs) {
    //     builder << IScriptOpcodes::opc___43;
    // };

    iscript_parser["OPC_DOGRDDAMAGE"] = [&builder](const peg::SemanticValues& vs) {
        builder << IScriptOpcodes::opc_dogrddamage;
    };

    int size = 0;
    const char* iscript_txt = (const char*)fastFileRead_(&size, 0, "scripts\\iscript.txt", 0, 0, __FILE__, __LINE__);

    bool result = iscript_parser.parse(iscript_txt);
    return result;
}
