#include <peglib.h>

#include "starcraft.h"

bool parse_iscript_txt()
{
    peg::parser iscript_parser(R"(
        ROOT <- (_ (NL / COMMENT NL / HEADER / LABEL / OP _ COMMENT? NL))* EOF

        HEADER   <- '.headerstart' _ NL _ (!'.' HEADER_LINE? _ NL _)* '.headerend' _ NL
        ~COMMENT <- '#' [^\r\n]*
        LABEL    <- <ID> _ ':' _ NL?

        # header rules:
        HEADER_IS_ID            <- 'IsId' __ INT               { no_ast_opt }
        HEADER_TYPE             <- 'Type' __ INT               { no_ast_opt }
        HEADER_INIT             <- 'Init' __ ID_MAYBE          { no_ast_opt }
        HEADER_DEATH            <- 'Death' __ ID_MAYBE         { no_ast_opt }
        HEADER_GND_ATTK_INIT    <- 'GndAttkInit' __ ID_MAYBE   { no_ast_opt }
        HEADER_AIR_ATTK_INIT    <- 'AirAttkInit' __ ID_MAYBE   { no_ast_opt }
        HEADER_UNUSED1          <- 'Unused1' __ ID_MAYBE       { no_ast_opt }
        HEADER_GND_ATTK_RPT     <- 'GndAttkRpt' __ ID_MAYBE    { no_ast_opt }
        HEADER_AIR_ATTK_RPT     <- 'AirAttkRpt' __ ID_MAYBE    { no_ast_opt }
        HEADER_CAST_SPELL       <- 'CastSpell' __ ID_MAYBE     { no_ast_opt }
        HEADER_GND_ATTK_TO_IDLE <- 'GndAttkToIdle' __ ID_MAYBE { no_ast_opt }
        HEADER_AIR_ATTK_TO_IDLE <- 'AirAttkToIdle' __ ID_MAYBE { no_ast_opt }
        HEADER_UNUSED2          <- 'Unused2' __ ID_MAYBE       { no_ast_opt }
        HEADER_WALKING          <- 'Walking' __ ID_MAYBE       { no_ast_opt }
        HEADER_WALKING_TO_IDLE  <- 'WalkingToIdle' __ ID_MAYBE { no_ast_opt }
        HEADER_SPECIAL_STATE1   <- 'SpecialState1' __ ID_MAYBE { no_ast_opt }
        HEADER_SPECIAL_STATE2   <- 'SpecialState2' __ ID_MAYBE { no_ast_opt }
        HEADER_ALMOST_BUILT     <- 'AlmostBuilt' __ ID_MAYBE   { no_ast_opt }
        HEADER_BUILT            <- 'Built' __ ID_MAYBE         { no_ast_opt }
        HEADER_LANDING          <- 'Landing' __ ID_MAYBE       { no_ast_opt }
        HEADER_LIFTOFF          <- 'LiftOff' __ ID_MAYBE       { no_ast_opt }
        HEADER_IS_WORKING       <- 'IsWorking' __ ID_MAYBE     { no_ast_opt }
        HEADER_WORKING_TO_IDLE  <- 'WorkingToIdle' __ ID_MAYBE { no_ast_opt }
        HEADER_WARP_IN          <- 'WarpIn' __ ID_MAYBE        { no_ast_opt }
        HEADER_UNUSED3          <- 'Unused3' __ ID_MAYBE       { no_ast_opt }
        HEADER_START_EDIT_INIT  <- 'StarEditInit' __ ID_MAYBE  { no_ast_opt }
        HEADER_DISABLE          <- 'Disable' __ ID_MAYBE       { no_ast_opt }
        HEADER_BURROW           <- 'Burrow' __ ID_MAYBE        { no_ast_opt }
        HEADER_UNBURROW         <- 'UnBurrow' __ ID_MAYBE      { no_ast_opt }
        HEADER_ENABLE           <- 'Enable' __ ID_MAYBE        { no_ast_opt }
        HEADER_LINE <- HEADER_IS_ID / HEADER_TYPE / HEADER_INIT / HEADER_DEATH /
                       HEADER_GND_ATTK_INIT / HEADER_AIR_ATTK_INIT / HEADER_UNUSED1 /
                       HEADER_GND_ATTK_RPT / HEADER_AIR_ATTK_RPT / HEADER_CAST_SPELL /
                       HEADER_GND_ATTK_TO_IDLE / HEADER_AIR_ATTK_TO_IDLE / HEADER_UNUSED2 /
                       HEADER_WALKING / HEADER_WALKING_TO_IDLE /
                       HEADER_SPECIAL_STATE1 / HEADER_SPECIAL_STATE2 /
                       HEADER_ALMOST_BUILT / HEADER_BUILT /
                       HEADER_LANDING / HEADER_LIFTOFF / HEADER_IS_WORKING / HEADER_WORKING_TO_IDLE /
                       HEADER_WARP_IN / HEADER_UNUSED3 / HEADER_START_EDIT_INIT /
                       HEADER_DISABLE / HEADER_BURROW / HEADER_UNBURROW / HEADER_ENABLE /
                       { error_message "Unrecognized animation" }

        # opcode rules
        OPC_IMGUL             <- 'imgul' __ INT __ INT __ INT           { no_ast_opt }
        OPC_IMGULNEXTID       <- 'imgulnextid' __ INT __ INT            { no_ast_opt }
        OPC_IMGULUSELO        <- 'imguluselo' __ INT __ INT __ INT      { no_ast_opt }
        OPC_IMGOL             <- 'imgol' __ INT __ INT __ INT           { no_ast_opt }
        OPC_IMGOLORIG         <- 'imgolorig' __ INT                     { no_ast_opt }
        OPC_IMGOLUSELO        <- 'imgoluselo' __ INT __ INT __ INT      { no_ast_opt }
        OPC_SPROL             <- 'sprol' __ INT __ INT __ INT           { no_ast_opt }
        OPC_SPROLUSELO        <- 'sproluselo' __ INT __ INT             { no_ast_opt }
        OPC_SPRUL             <- 'sprul' __ INT __ INT __ INT           { no_ast_opt }
        OPC_SPRULUSELO        <- 'spruluselo' __ INT __ INT __ INT      { no_ast_opt }
        OPC_GRDSPROL          <- 'grdsprol' __ INT __ INT __ INT        { no_ast_opt }
        OPC_WARPOVERLAY       <- 'warpoverlay' __ INT                   { no_ast_opt }
        OPC_SWITCHUL          <- 'switchul' __ INT                      { no_ast_opt }
        OPC_WAITRAND          <- 'waitrand' __ INT __ INT               { no_ast_opt }
        OPC_SETVERTPOS        <- 'setvertpos' __ INT                    { no_ast_opt }
        OPC_GOTO              <- 'goto' __ ID                           { no_ast_opt }
        OPC_WAIT              <- 'wait' __ INT                          { no_ast_opt }
        OPC_PLAYFRAM          <- 'playfram' __ INT                      { no_ast_opt }
        OPC_PLAYFRAMTILE      <- 'playframtile' __ INT                  { no_ast_opt }
        OPC_ENGFRAME          <- 'engframe' __ INT                      { no_ast_opt }
        OPC_PLAYSND           <- 'playsnd' __ INT                       { no_ast_opt }
        OPC_END               <- 'end'                                  { no_ast_opt }
        OPC_ATTACKWITH        <- 'attackwith' __ INT                    { no_ast_opt }
        OPC_LOWSPRUL          <- 'lowsprul' __ INT __ INT __ INT        { no_ast_opt }
        OPC_NOBRKCODESTART    <- 'nobrkcodestart'                       { no_ast_opt }
        OPC_NOBRKCODEEND      <- 'nobrkcodeend'                         { no_ast_opt }
        OPC_ATTACKMELEE       <- 'attackmelee' __ INT (__ INT)*         { no_ast_opt }
        OPC_GOTOREPEATATTK    <- 'gotorepeatattk'                       { no_ast_opt }
        OPC_CALL              <- 'call' __ ID                           { no_ast_opt }
        OPC_RETURN            <- 'return'                               { no_ast_opt }
        OPC_IGNOREREST        <- 'ignorerest'                           { no_ast_opt }
        OPC_RANDCONDJMP       <- 'randcondjmp' __ INT __ ID             { no_ast_opt }
        OPC_LIFTOFFCONDJMP    <- 'liftoffcondjmp' __ ID                 { no_ast_opt }
        OPC_TRGTARCCONDJMP    <- 'trgtarccondjmp' __ INT __ INT __ ID   { no_ast_opt }
        OPC_TRGTRANGECONDJMP  <- 'trgtrangecondjmp' __ INT __ ID        { no_ast_opt }
        OPC_CURDIRECTCONDJMP  <- 'curdirectcondjmp' __ INT __ INT __ ID { no_ast_opt }
        OPC_PWRUPCONDJMP      <- 'pwrupcondjmp' __ ID                   { no_ast_opt }
        OPC_MOVE              <- 'move' __ INT                          { no_ast_opt }
        OPC_SETFLDIRECT       <- 'setfldirect' __ INT                   { no_ast_opt }
        OPC_SIGORDER          <- 'sigorder' __ INT                      { no_ast_opt }
        OPC_ORDERDONE         <- 'orderdone' __ INT                     { no_ast_opt }
        OPC_ATTACK            <- 'attack'                               { no_ast_opt }
        OPC_ATTKSHIFTPROJ     <- 'attkshiftproj' __ INT                 { no_ast_opt }
        OPC_CASTSPELL         <- 'castspell'                            { no_ast_opt }
        OPC_USEWEAPON         <- 'useweapon' __ INT                     { no_ast_opt }
        OPC_DOMISSILEDMG      <- 'domissiledmg'                         { no_ast_opt }
        OPC_DOGRDDAMAGE       <- 'dogrddamage'                          { no_ast_opt }
        OPC_FOLLOWMAINGRAPHIC <- 'followmaingraphic'                    { no_ast_opt }
        OPC_TURN1CWISE        <- 'turn1cwise'                           { no_ast_opt }
        OPC_TURNCWISE         <- 'turncwise' __ INT                     { no_ast_opt }
        OPC_TURNCCWISE        <- 'turnccwise' __ INT                    { no_ast_opt }
        OPC_TURNRAND          <- 'turnrand' __ INT                      { no_ast_opt }
        OPC_SETFLSPEED        <- 'setflspeed' __ INT                    { no_ast_opt }
        OPC_TMPRMGRAPHICSTART <- 'tmprmgraphicstart'                    { no_ast_opt }
        OPC_TMPRMGRAPHICEND   <- 'tmprmgraphicend'                      { no_ast_opt }
        OPC_PLAYSNDBTWN       <- 'playsndbtwn' __ INT __ INT            { no_ast_opt }
        OPC_PLAYSNDRAND       <- 'playsndrand' __ INT (__ INT)+         { no_ast_opt }
        OPC_CREATEGASOVERLAYS <- 'creategasoverlays' __ INT             { no_ast_opt }
        OPC_ENGSET            <- 'engset' __ INT                        { no_ast_opt }
        OPC_SETFILPSTATE      <- 'setflipstate' __ INT                  { no_ast_opt }
        OPC_SETPOS            <- 'setpos' __ INT __ INT                 { no_ast_opt }
        OPC_SETSPAWNFRAME     <- 'setspawnframe' __ INT                 { no_ast_opt }
        OPC_SETHORPOS         <- 'sethorpos' __ INT                     { no_ast_opt }
        OPC___2D              <- '__2d'                                 { no_ast_opt }
        OP <- OPC_WAIT / OPC_PLAYFRAM / OPC_PLAYFRAMTILE / OPC_ENGFRAME /
              OPC_IMGUL / OPC_IMGULNEXTID / OPC_IMGULUSELO /
              OPC_IMGOL / OPC_IMGOLUSELO / OPC_IMGOLORIG /
              OPC_SPROL / OPC_SPROLUSELO / OPC_SPRUL / OPC_SPRULUSELO /
              OPC_GRDSPROL /
              OPC_WARPOVERLAY / OPC_SWITCHUL / OPC_WAITRAND /
              OPC_SETVERTPOS / OPC_GOTO / OPC_RANDCONDJMP / OPC_PLAYSND / OPC_END /
              OPC_LOWSPRUL / OPC_NOBRKCODESTART / OPC_NOBRKCODEEND /
              OPC_GOTOREPEATATTK / OPC_CALL / OPC_RETURN / OPC_IGNOREREST /
              OPC_LIFTOFFCONDJMP / OPC_TRGTARCCONDJMP / OPC_TRGTRANGECONDJMP /
              OPC_CURDIRECTCONDJMP / OPC_PWRUPCONDJMP /
              OPC_MOVE / OPC_SIGORDER / OPC_ORDERDONE /
              OPC_ATTACKMELEE / OPC_ATTACKWITH / OPC_ATTACK / OPC_ATTKSHIFTPROJ /
              OPC_CASTSPELL / OPC_USEWEAPON / OPC_DOMISSILEDMG / OPC_DOGRDDAMAGE /
              OPC_FOLLOWMAINGRAPHIC / OPC_SETFLDIRECT / OPC_CREATEGASOVERLAYS /
              OPC_TURN1CWISE / OPC_TURNCWISE / OPC_TURNCCWISE / OPC_TURNRAND /
              OPC_SETFLSPEED / OPC_TMPRMGRAPHICSTART / OPC_TMPRMGRAPHICEND /
              OPC_PLAYSNDBTWN / OPC_PLAYSNDRAND / OPC_ENGSET /
              OPC_SETFILPSTATE / OPC_SETPOS / OPC_SETSPAWNFRAME / OPC_SETHORPOS /
              OPC___2D /
              { error_message "Unrecognized instruction" }

        ID       <- [_a-zA-Z][_a-zA-Z0-9]*
        ID_MAYBE <- ID / '[none]'i
        INT      <- '0x' [0-9a-fA-F][0-9a-fA-F]* / [1-9][0-9]* / '0'

        ~EOF <- !.
        ~NL  <- ('\n' | '\r\n' | '\r')
        ~_   <- [ \t]*
        ~__  <- [ \t]+
    )");

    int size = 0;
    const char* iscript_txt = (const char*)fastFileRead_(&size, 0, "scripts\\iscript.txt", 0, 0, __FILE__, __LINE__);

    bool result = iscript_parser.parse(iscript_txt);
    return result;
}
