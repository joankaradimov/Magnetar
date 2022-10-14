#include "iscript.h"
#include "starcraft.h"
#include "patching.h"

template<typename T>
T* take_iscript_data(IScriptProgram* program_state, int count)
{
    BYTE* data = (BYTE*)iscript_data + program_state->program_counter;
    program_state->program_counter += sizeof(T) * count;
    return (T*)data;
}

template<typename T>
T take_iscript_datum(IScriptProgram* program_state)
{
    return *take_iscript_data<T>(program_state, 1);
}

void ISCRIPT_PlayFrame_(CImage* image, int a2)
{
    if (image->frameSet != a2)
    {
        image->frameSet = a2;
        int v3 = (unsigned __int16)a2 + image->direction;
        if (image->frameIndex != v3)
        {
            image->flags |= ImageFlags::IF_REDRAW;
            image->frameIndex = v3;
        }
    }
}

FAIL_STUB_PATCH(ISCRIPT_PlayFrame);

void ISCRIPT_setPosition_(CImage* image, char x, char y)
{
    if (image->horizontalOffset != x || image->verticalOffset != y)
    {
        image->flags |= IF_REDRAW;
        image->horizontalOffset = x;
        image->verticalOffset = y;
    }
}

FAIL_STUB_PATCH(ISCRIPT_setPosition);

CThingy* ISCRIPT_CreateSprite_(CImage* image, unsigned __int16 sprite_id, int x, int y, char elevation_level)
{
    CThingy* result = CreateThingy(sprite_id, x + image->horizontalOffset + (__int16)image->spriteOwner->position.x, y + image->verticalOffset + image->spriteOwner->position.y, 0);
    if (result)
    {
        result->sprite->elevationLevel = elevation_level;
        sub_4878F0(result);
    }
    return result;
}

FAIL_STUB_PATCH(ISCRIPT_CreateSprite);

void ISCRIPT_CastSpell_(CUnit* unit, WeaponType weapon_id)
{
    FireUnitWeapon(unit, weapon_id);
    if (Weapon_DamageFactor[weapon_id] == 2)
    {
        FireUnitWeapon(unit, weapon_id);
    }
}

FAIL_STUB_PATCH(ISCRIPT_CastSpell);

void ISCRIPT_AttackWith_(CUnit* attacker, u8 is_ground_weapon)
{
    if (attacker->orderTarget.pUnit)
    {
        WeaponType weapon_type;

        if (is_ground_weapon != 1)
        {
            weapon_type = Unit_AirWeapon[attacker->unitType];
        }
        else if (attacker->unitType != Zerg_Lurker || (attacker->statusFlags & Burrowed))
        {
            weapon_type = Unit_GroundWeapon[attacker->unitType];
        }
        else
        {
            weapon_type = WT_None;
        }

        ISCRIPT_CastSpell_(attacker, weapon_type);
    }
}

FAIL_STUB_PATCH(ISCRIPT_AttackWith);

void ISCRIPT_NoBrkCodeEnd_(CUnit* unit)
{
    unit->statusFlags &= ~StatusFlags::NoBrkCodeStart;
    unit->sprite->flags &= ~0x80;
    if (unit->orderQueueHead && (unit->userActionFlags & 1))
    {
        IgnoreAllScriptAndGotoIdle(unit);
        PrepareForNextOrderFunc(unit);
    }
}

FAIL_STUB_PATCH(ISCRIPT_NoBrkCodeEnd);

void BWFXN_PlayIscript_(CImage* image, IScriptProgram* program_state, int noop, _DWORD* distance_moved)
{
    if (program_state->wait)
    {
        program_state->wait -= 1;
        return;
    }

    while (2)
    {
        IScriptOpcodes opcode = take_iscript_datum<IScriptOpcodes>(program_state);
        switch (opcode)
        {
        case opc_playfram:
        {
            u16 arg = take_iscript_datum<u16>(program_state);
            if (noop)
            {
                break;
            }
            ISCRIPT_PlayFrame_(image, arg);
            break;
        }
        case opc_playframtile:
        {
            _WORD arg = take_iscript_datum<_WORD>(program_state);
            if (noop)
            {
                break;
            }
            unsigned __int16 v9 = CurrentTileSet + arg;
            if (v9 < (unsigned __int16)(image->GRPFile->wFrames & 0x7FFF))
            {
                ISCRIPT_PlayFrame_(image, v9);
            }
            break;
        }
        case opc_sethorpos:
        {
            s8 x = take_iscript_datum<s8>(program_state);
            if (noop)
            {
                break;
            }
            if (image->horizontalOffset != x)
            {
                image->flags |= ImageFlags::IF_REDRAW;
                image->horizontalOffset = x;
            }
            break;
        }
        case opc_setvertpos:
        {
            s8 y = take_iscript_datum<s8>(program_state);
            if (noop)
            {
                break;
            }
            if ((iscript_unit == nullptr || (iscript_unit->statusFlags & (StatusFlags::Cloaked | StatusFlags::RequiresDetection)) == 0) && image->verticalOffset != y)
            {
                image->flags |= ImageFlags::IF_REDRAW;
                image->verticalOffset = y;
            }
            break;
        }
        case opc_setpos:
        {
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            ISCRIPT_setPosition_(image, x, y);
            break;
        }
        case opc_wait:
        {
            char arg = take_iscript_datum<char>(program_state);
            program_state->wait = arg - 1;
            return;
        }
        case opc_waitrand:
        {
            unsigned __int8 v13 = take_iscript_datum<unsigned char>(program_state);
            unsigned __int8 v15 = take_iscript_datum<unsigned char>(program_state);
            if (noop)
            {
                break;
            }
            unsigned __int8 v16 = RandomizeShort(3);
            program_state->wait = v13 + v16 % (v15 - v13 + 1) - 1;
            return;
        }
        case opc_goto:
        {
            u16 new_pc = take_iscript_datum<u16>(program_state);
            program_state->program_counter = new_pc;
            break;
        }
        case opc_imgol:
        {
            u16 image_id = take_iscript_datum<u16>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            ISCRIPT_CreateImage(image, image_id, x + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + y), ImageOrder::IMGORD_ABOVE);
            break;
        }
        case opc_imgul:
        {
            u16 image_id = take_iscript_datum<u16>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            ISCRIPT_CreateImage(image, image_id, x + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + y), ImageOrder::IMGORD_BELOW);
            break;
        }
        case opc_imgolorig:
        {
            u16 v24 = take_iscript_datum<u16>(program_state);
            if (noop)
            {
                break;
            }
            if (CImage* new_image = ISCRIPT_CreateImage(image, v24, 0, 0, ImageOrder::IMGORD_ABOVE))
            {
                if ((new_image->flags & ImageFlags::IF_USES_SPECIAL_OFFSET) == 0)
                {
                    new_image->flags |= ImageFlags::IF_USES_SPECIAL_OFFSET;
                    updateImagePositionOffset(new_image);
                }
            }
            break;
        }
        case opc_switchul:
        {
            u16 v27 = take_iscript_datum<u16>(program_state);
            if (noop)
            {
                break;
            }
            if (CImage* new_image = ISCRIPT_CreateImage(image, v27, 0, 0, ImageOrder::IMGORD_BELOW))
            {
                if ((new_image->flags & ImageFlags::IF_USES_SPECIAL_OFFSET) == 0)
                {
                    new_image->flags |= ImageFlags::IF_USES_SPECIAL_OFFSET;
                    updateImagePositionOffset(new_image);
                }
            }
            break;
        }
        case opc_imgoluselo:
        {
            unsigned __int16 image_id = take_iscript_datum<_WORD>(program_state);
            unsigned __int8 v30 = take_iscript_datum<unsigned __int8>(program_state);
            char v29 = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            point pt;
            ISCRIPT_UseLOFile(&pt, image, v30, v29);
            ISCRIPT_CreateImage(image, image_id, LOBYTE(pt.x) + image->horizontalOffset, (unsigned __int8)(LOBYTE(pt.y) + image->verticalOffset), ImageOrder::IMGORD_ABOVE);
            break;
        }
        case opc_imguluselo:
        {
            unsigned __int16 image_id = take_iscript_datum<_WORD>(program_state);
            unsigned __int8 v34 = take_iscript_datum<unsigned __int8>(program_state);
            char v33 = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            point pt;
            ISCRIPT_UseLOFile(&pt, image, v34, v33);
            ISCRIPT_CreateImage(image, image_id, LOBYTE(pt.x) + image->horizontalOffset, (unsigned __int8)(LOBYTE(pt.y) + image->verticalOffset), ImageOrder::IMGORD_BELOW);
            break;
        }
        case opc_sprol:
        {
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            if (iscript_bullet && iscript_bullet->sourceUnit && UnitIsGoliath(iscript_bullet->sourceUnit)
                && (UpgradeLevelBW[iscript_bullet->sourceUnit->playerID].items[8]
                    || (Unit_PrototypeFlags[iscript_bullet->sourceUnit->unitType] & UnitPrototypeFlags::Hero) && IsExpansion))
            {
                sprite_id = 0x1F9;
            }
            ISCRIPT_CreateSprite_(image, sprite_id, x, y, image->spriteOwner->elevationLevel + 1);
            break;
        }
        case opc_highsprol:
        {
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            ISCRIPT_CreateSprite_(image, sprite_id, x, y, image->spriteOwner->elevationLevel - 1);
            break;
        }
        case opc_lowsprul:
        {
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            ISCRIPT_CreateSprite_(image, sprite_id, x, y, 1);
            break;
        }
        case opc_uflunstable:
        {
            unsigned __int16 flingy_id = take_iscript_datum<_WORD>(program_state);
            if (noop)
            {
                break;
            }
            if (CFlingy* v45 = ISCRIPT_CreateFlingy(0, image->spriteOwner->position.y + image->verticalOffset, (__int16)image->spriteOwner->position.x + image->horizontalOffset, (FlingyID)flingy_id))
            {
                CThingy* v46 = (CThingy*)uflunstableRandomize(v45);
                sub_4878F0(v46);
            }
            break;
        }
        case opc_spruluselo:
        {
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            if (iscript_unit && (iscript_unit->statusFlags & (StatusFlags::Cloaked | StatusFlags::RequiresDetection)) && !Image_DrawIfCloaked[Sprites_Image[sprite_id]])
            {
                break;
            }
            if (CThingy* thingy = ISCRIPT_CreateSprite_(image, sprite_id, x, y, image->spriteOwner->elevationLevel))
            {
                setAllOverlayDirectionsGeneric(thingy, (image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) ? 32 - image->direction : image->direction);
            }
            break;
        }
        case opc_sprul:
        {
            unsigned __int16 v61 = take_iscript_datum<_WORD>(program_state);
            char v63 = take_iscript_datum<char>(program_state);
            char v64 = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            if (iscript_unit && (iscript_unit->statusFlags & (StatusFlags::Cloaked | StatusFlags::RequiresDetection)) && !Image_DrawIfCloaked[Sprites_Image[v61]])
            {
                break;
            }
            if (CThingy* thingy = ISCRIPT_CreateSprite_(image, v61, v63, v64, image->spriteOwner->elevationLevel - 1))
            {
                setAllOverlayDirectionsGeneric(thingy, (image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) ? 32 - image->direction : image->direction);
            }
            break;
        }
        case opc_sproluselo:
        {
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char v62 = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            point v125;
            ISCRIPT_UseLOFile(&v125, image, v62, 0);
            if (CThingy* thingy = ISCRIPT_CreateSprite_(image, sprite_id, v125.x, v125.y, image->spriteOwner->elevationLevel + 1))
            {
                setAllOverlayDirectionsGeneric(thingy, (image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) ? 32 - image->direction : image->direction);
            }
            break;
        }
        case opc_end:
            if (noop)
            {
                break;
            }
            ImageDestructor(image);
            return;
        case opc_setflipstate:
        {
            char arg = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            flipImage(image, arg);
            break;
        }
        case opc_playsnd:
        {
            unsigned __int16 sfx = take_iscript_datum<unsigned __int16>(program_state);
            if (noop)
            {
                break;
            }
            ISCRIPT_PlaySnd((SfxData)sfx, image);
            break;
        }
        case opc_playsndbtwn:
        {
            unsigned __int16 v79 = take_iscript_datum<unsigned __int16>(program_state);
            unsigned __int16 v81 = take_iscript_datum<unsigned __int16>(program_state);
            if (noop)
            {
                break;
            }
            ISCRIPT_PlaySnd((SfxData)(v79 + RandomizeShort(5) % ((unsigned int)v81 - v79 + 1)), image);
            break;
        }
        case opc_domissiledmg:
            if (noop)
            {
                break;
            }
            CBullet_Damage(iscript_bullet);
            break;
        case opc_attackmelee:
            if (noop)
            {
                break;
            }
            ISCRIPT_AttackMelee(iscript_unit);
            [[fallthrough]];
        case opc_playsndrand:
        {
            unsigned __int8 arg_count = take_iscript_datum<char>(program_state);
            u16* args = take_iscript_data<u16>(program_state, arg_count);
            if (noop)
            {
                break;
            }
            ISCRIPT_PlaySnd((SfxData)args[RandomizeShort(4) % arg_count], image);
            break;
        }
        case opc_followmaingraphic:
        {
            if (noop)
            {
                break;
            }
            CImage* v84 = image->spriteOwner->pImagePrimary;
            if (v84 == nullptr || image->frameIndex == v84->frameIndex && ((image->flags ^ v84->flags) & ImageFlags::IF_HORIZONTALLY_FLIPPED) == 0)
            {
                break;
            }
            image->frameSet = v84->frameSet;
            image->direction = v84->direction;
            image->flags ^= ((image->flags ^ v84->flags) & ImageFlags::IF_HORIZONTALLY_FLIPPED);
            CImage* v86 = setImagePaletteType(image, image->paletteType);
            updateImageFrameIndex(v86);
            break;
        }
        case opc_randcondjmp:
        {
            unsigned __int8 v92 = take_iscript_datum<char>(program_state);
            unsigned __int16 new_pc = take_iscript_datum<unsigned short>(program_state);
            if ((unsigned __int8)RandomizeShort(7) <= (unsigned int)v92)
            {
                program_state->program_counter = new_pc;
            }
            break;
        }
        case opc_turnccwise:
        {
            char arg = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            turn_unit_left(iscript_unit, arg);
            break;
        }
        case opc_turncwise:
        {
            char arg = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            turnUnit(iscript_unit, iscript_unit->currentDirection1 + 8 * arg);
            break;
        }
        case opc_turn1cwise:
            if (noop)
            {
                break;
            }
            if (iscript_unit->orderTarget.pUnit == nullptr)
            {
                turnUnit(iscript_unit, iscript_unit->currentDirection1 + 8);
            }
            break;
        case opc_turnrand:
        {
            char arg = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            if ((RandomizeShort(6) & 3) == 1)
            {
                turn_unit_left(iscript_unit, arg);
            }
            else
            {
                turnUnit(iscript_unit, iscript_unit->currentDirection1 + 8 * arg);
            }
            break;
        }
        case opc_setspawnframe:
        {
            char arg = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            unitSetRetreatPoint(arg, iscript_unit);
            break;
        }
        case opc_sigorder:
        {
            char arg = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            iscript_flingy->orderSignal |= arg;
            break;
        }
        case opc_attackwith:
        {
            char arg = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            ISCRIPT_AttackWith_(iscript_unit, arg);
            break;
        }
        case opc_attack:
            if (noop)
            {
                break;
            }
            if (iscript_unit->orderTarget.pUnit && (iscript_unit->orderTarget.pUnit->statusFlags & StatusFlags::InAir))
            {
                ISCRIPT_CastSpell_(iscript_unit, Unit_AirWeapon[iscript_unit->unitType]);
            }
            else
            {
                ISCRIPT_AttackWith_(iscript_unit, 1);
            }
            break;
        case opc_castspell:
            if (noop)
            {
                break;
            }
            if (Orders_TargetingWeapon[iscript_unit->orderID] < WT_None && !canCastSpell_0(iscript_unit))
            {
                ISCRIPT_CastSpell_(iscript_unit, Orders_TargetingWeapon[iscript_unit->orderID]);
            }
            break;
        case opc_useweapon:
        {
            char arg = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            ISCRIPT_UseWeapon(iscript_unit, (WeaponType)arg);
            break;
        }
        case opc_move:
        {
            char arg = take_iscript_datum<char>(program_state);
            unsigned int v117 = arg << 8;
            if (distance_moved)
            {
                *distance_moved = GetModifiedUnitSpeed(v117, iscript_unit);
            }
            if (noop)
            {
                break;
            }
            int v120 = GetModifiedUnitSpeed(v117, iscript_unit);
            SetUnitMovementSpeed(iscript_unit, v120);
            break;
        }
        case opc_gotorepeatattk:
            if (noop)
            {
                break;
            }
            if (iscript_unit)
            {
                iscript_unit->movementFlags &= ~8;
            }
            break;
        case opc_engframe:
        {
            unsigned __int8 arg5 = take_iscript_datum<unsigned __int8>(program_state);
            if (noop)
            {
                break;
            }
            image->frameSet = arg5;
            CImage* v88 = image->spriteOwner->pImagePrimary;
            image->direction = v88->direction;
            image->flags ^= ((image->flags ^ v88->flags) & ImageFlags::IF_HORIZONTALLY_FLIPPED);
            CImage* v86 = setImagePaletteType(image, image->paletteType);
            updateImageFrameIndex(v86);
            break;
        }
        case opc_engset:
        {
            unsigned __int8 v89 = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            CImage* v88 = image->spriteOwner->pImagePrimary;
            image->direction = v88->direction;
            image->frameSet = v88->frameSet + v89 * (v88->GRPFile->wFrames & 0x7FFF);
            image->flags ^= ((image->flags ^ v88->flags) & ImageFlags::IF_HORIZONTALLY_FLIPPED);
            CImage* v86 = setImagePaletteType(image, image->paletteType);
            updateImageFrameIndex(v86);
            break;
        }
        case opc___2d:
            if (noop)
            {
                break;
            }
            wantThingyUpdate = 0;
            break;
        case opc_nobrkcodestart:
            if (noop)
            {
                break;
            }
            iscript_unit->statusFlags |= StatusFlags::NoBrkCodeStart;
            iscript_unit->sprite->flags |= 0x80u;
            break;
        case opc_nobrkcodeend:
            if (noop)
            {
                break;
            }
            if (iscript_unit)
            {
                u16 v5 = program_state->program_counter;
                ISCRIPT_NoBrkCodeEnd_(iscript_unit);
                program_state->program_counter = v5;
            }
            break;
        case opc_ignorerest:
        {
            if (noop)
            {
                break;
            }
            if (iscript_unit->orderTarget.pUnit)
            {
                program_state->program_counter -= 1;
                program_state->wait = 10;
                return;
            }
            u16 v5 = program_state->program_counter;
            IgnoreAllScriptAndGotoIdle(iscript_unit);
            program_state->program_counter = v5;
            break;
        }
        case opc_attkshiftproj:
        {
            char arg = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            Weapon_XOffset[UnitGetGrndWeapon(iscript_unit)] = arg;
            ISCRIPT_AttackWith_(iscript_unit, 1);
            break;
        }
        case opc_tmprmgraphicstart:
            if (noop)
            {
                break;
            }
            hideImage(image);
            break;
        case opc_tmprmgraphicend:
            if (noop)
            {
                break;
            }
            showImage(image);
            break;
        case opc_setfldirect:
        {
            char arg = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            turnUnit(iscript_unit, 8 * arg);
            break;
        }
        case opc_call:
        {
            u16 new_pc = take_iscript_datum<u16>(program_state);
            program_state->unsigned4 = program_state->program_counter;
            program_state->program_counter = new_pc;
            break;
        }
        case opc_return:
            program_state->program_counter = program_state->unsigned4;
            break;
        case opc_setflspeed:
        {
            u16 arg = take_iscript_datum<u16>(program_state);
            if (noop)
            {
                break;
            }
            iscript_unit->flingyTopSpeed = arg;
            break;
        }
        case opc_creategasoverlays:
        {
            char v65 = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            point v124;
            ISCRIPT_UseLOFile(&v124, image, 2, v65);
            int v67 = v65 + (iscript_unit->fields2.resource.resourceCount != 0 ? 430 : 435);
            if (CImage* v68 = sub_4D4E30())
            {
                image_Insert(v68, &image->spriteOwner->pImageHead, image);
                somePlayImageCrapThatCrashes(LOBYTE(v124.y) + image->verticalOffset, LOBYTE(v124.x) + image->horizontalOffset, v68, image->spriteOwner, (unsigned __int16)v67);
            }
            break;
        }
        case opc_pwrupcondjmp:
        {
            u16 new_pc = take_iscript_datum<u16>(program_state);
            if (image->spriteOwner && image->spriteOwner->pImagePrimary != image)
            {
                program_state->program_counter = new_pc;
            }
            break;
        }
        case opc_trgtrangecondjmp:
        {
            unsigned __int16 v95 = take_iscript_datum<_WORD>(program_state);
            unsigned __int16 v97 = take_iscript_datum<unsigned short>(program_state);
            if (noop)
            {
                break;
            }
            if (iscript_unit->orderTarget.pUnit)
            {
                int a3;
                int a4;
                sub_4762C0(iscript_unit, (int)&a4, (int)&a3);
                if (isDistanceGreaterThanHaltDistance(a3, iscript_unit, v95, a4))
                {
                    program_state->program_counter = v97;
                }
            }
            break;
        }
        case opc_trgtarccondjmp:
        {
            unsigned __int16 v99 = take_iscript_datum<_WORD>(program_state);
            unsigned __int16 v128 = take_iscript_datum<unsigned short>(program_state);
            u16 new_pc = take_iscript_datum<u16>(program_state);
            if (noop)
            {
                break;
            }
            if (CUnit* target = iscript_unit->orderTarget.pUnit)
            {
                int v104 = (__int16)target->sprite->position.y;
                int v105 = (__int16)target->sprite->position.x;
                int v106 = (__int16)iscript_unit->sprite->position.x;
                int v123 = (__int16)iscript_unit->sprite->position.y;
                char v107 = GetAngle(v106, v123, v105, v104);
                if (sub_494BD0((int)v99, v107) < v128)
                {
                    program_state->program_counter = new_pc;
                }
            }
            break;
        }
        case opc_curdirectcondjmp:
        {
            unsigned __int16 v109 = take_iscript_datum<_WORD>(program_state);
            unsigned __int16 v111 = take_iscript_datum<unsigned short>(program_state);
            u16 new_pc = take_iscript_datum<u16>(program_state);
            if (noop)
            {
                break;
            }
            if (sub_494BD0(v109, iscript_unit->currentDirection1) < v111)
            {
                program_state->program_counter = new_pc;
            }
            break;
        }
        case opc_imgulnextid:
        {
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            if (noop)
            {
                break;
            }
            ISCRIPT_CreateImage(image, image->imageID + 1, x + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + y), ImageOrder::IMGORD_BELOW);
            break;
        }
        case opc_liftoffcondjmp:
        {
            u16 new_pc = take_iscript_datum<u16>(program_state);
            if (noop)
            {
                break;
            }
            if (iscript_unit->statusFlags & StatusFlags::InAir)
            {
                program_state->program_counter = new_pc;
            }
            break;
        }
        case opc_warpoverlay:
        {
            unsigned __int16 arg = take_iscript_datum<unsigned __int16>(program_state);
            if (noop)
            {
                break;
            }
            image->flags |= ImageFlags::IF_REDRAW;
            image->coloringData = (void*)arg;
            break;
        }
        case opc_orderdone:
        {
            u8 arg = take_iscript_datum<u8>(program_state);
            if (noop)
            {
                break;
            }
            iscript_flingy->orderSignal &= ~arg;
            break;
        }
        case opc_grdsprol:
        {
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);

            if (noop)
            {
                break;
            }
            if (canUnitTypeFitAt(x + image->spriteOwner->position.x + image->horizontalOffset, Terran_Marine, y + image->spriteOwner->position.y + image->verticalOffset))
            {
                ISCRIPT_CreateSprite_(image, sprite_id, x, y, image->spriteOwner->elevationLevel + 1);
            }
            break;
        }
        case opc_dogrddamage:
            if (noop)
            {
                break;
            }
            CBullet_Damage(iscript_bullet);
            break;
        default:
            break;
        }
    }
}

void __stdcall BWFXN_PlayIscript__(IScriptProgram* program_state, int noop, int* distance_moved)
{
    CImage* image;

    __asm mov image, ecx

    BWFXN_PlayIscript_(image, program_state, noop, distance_moved);
}

FUNCTION_PATCH((void*)0x4D74C0, BWFXN_PlayIscript__);
