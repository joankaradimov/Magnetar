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

void BWFXN_PlayIscript_(CImage* image, IScriptProgram* program_state, int noop, _DWORD* distance_moved)
{
    _BYTE* v37; // edi
    char* v51; // edi
    char* v54; // edi
    CImage* v84; // eax
    CImage* v86; // eax
    point v124; // [esp+0h] [ebp-40h] BYREF

    if (program_state->wait)
    {
        program_state->wait -= 1;
        return;
    }
    char* v5 = (char*)iscript_data + program_state->program_counter;

    while (2)
    {
        program_state->program_counter = v5 - (char*)iscript_data;
        IScriptOpcodes opcode = take_iscript_datum<IScriptOpcodes>(program_state);
        v5 = (char*)iscript_data + program_state->program_counter;
        switch (opcode)
        {
        case opc_playfram:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            u16 arg = take_iscript_datum<u16>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_PlayFrame(image, arg);
            continue;
        }
        case opc_playframtile:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            _WORD arg = take_iscript_datum<_WORD>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            unsigned __int16 v9 = CurrentTileSet + arg;
            if (v9 < (unsigned __int16)(image->GRPFile->wFrames & 0x7FFF))
            {
                ISCRIPT_PlayFrame(image, v9);
            }
            continue;
        }
        case opc_sethorpos:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            s8 x = take_iscript_datum<s8>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            if (image->horizontalOffset != x)
            {
                image->flags |= ImageFlags::IF_REDRAW;
                image->horizontalOffset = x;
            }
            continue;
        }
        case opc_setvertpos:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            s8 y = take_iscript_datum<s8>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            if ((iscript_unit == nullptr || (iscript_unit->statusFlags & (StatusFlags::Cloaked | StatusFlags::RequiresDetection)) == 0) && image->verticalOffset != y)
            {
                image->flags |= ImageFlags::IF_REDRAW;
                image->verticalOffset = y;
            }
            continue;
        }
        case opc_setpos:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_setPosition(image, x, y);
            continue;
        }
        case opc_wait:
            program_state->wait = *v5 - 1;
            program_state->program_counter = (BYTE*)v5 + 1 - (BYTE*)iscript_data;
            return;
        case opc_waitrand:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int8 v13 = take_iscript_datum<unsigned char>(program_state);
            unsigned __int8 v15 = take_iscript_datum<unsigned char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            unsigned __int8 v16 = RandomizeShort(3);
            program_state->program_counter = (BYTE*)v5 - (BYTE*)iscript_data;
            program_state->wait = v13 + v16 % (v15 - v13 + 1) - 1;
            return;
        }
        case opc_goto:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            u16 new_pc = take_iscript_datum<u16>(program_state);
            program_state->program_counter = new_pc;
            v5 = (char*)iscript_data + program_state->program_counter;
            continue;
        }
        case opc_imgol:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            u16 image_id = take_iscript_datum<u16>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_CreateImage(image, image_id, x + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + y), ImageOrder::IMGORD_ABOVE);
            continue;
        }
        case opc_imgul:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            u16 image_id = take_iscript_datum<u16>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_CreateImage(image, image_id, x + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + y), ImageOrder::IMGORD_BELOW);
            continue;
        }
        case opc_imgolorig:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            u16 v24 = take_iscript_datum<u16>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            if (CImage* new_image = ISCRIPT_CreateImage(image, v24, 0, 0, ImageOrder::IMGORD_ABOVE))
            {
                if ((new_image->flags & ImageFlags::IF_USES_SPECIAL_OFFSET) == 0)
                {
                    new_image->flags |= ImageFlags::IF_USES_SPECIAL_OFFSET;
                    updateImagePositionOffset(new_image);
                }
            }
            continue;
        }
        case opc_switchul:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            u16 v27 = take_iscript_datum<u16>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            if (CImage* new_image = ISCRIPT_CreateImage(image, v27, 0, 0, ImageOrder::IMGORD_BELOW))
            {
                if ((new_image->flags & ImageFlags::IF_USES_SPECIAL_OFFSET) == 0)
                {
                    new_image->flags |= ImageFlags::IF_USES_SPECIAL_OFFSET;
                    updateImagePositionOffset(new_image);
                }
            }
            continue;
        }
        case opc_imgoluselo:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 image_id = take_iscript_datum<_WORD>(program_state);
            unsigned __int8 v30 = take_iscript_datum<unsigned __int8>(program_state);
            char v29 = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            point pt;
            ISCRIPT_UseLOFile(&pt, image, v30, v29);
            ISCRIPT_CreateImage(image, image_id, LOBYTE(pt.x) + image->horizontalOffset, (unsigned __int8)(LOBYTE(pt.y) + image->verticalOffset), ImageOrder::IMGORD_ABOVE);
            continue;
        }
        case opc_imguluselo:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 image_id = take_iscript_datum<_WORD>(program_state);
            unsigned __int8 v34 = take_iscript_datum<unsigned __int8>(program_state);
            char v33 = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            point pt;
            ISCRIPT_UseLOFile(&pt, image, v34, v33);
            ISCRIPT_CreateImage(image, image_id, LOBYTE(pt.x) + image->horizontalOffset, (unsigned __int8)(LOBYTE(pt.y) + image->verticalOffset), ImageOrder::IMGORD_BELOW);
            continue;
        }
        case opc_sprol:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            if (iscript_bullet && iscript_bullet->sourceUnit && UnitIsGoliath(iscript_bullet->sourceUnit)
                && (UpgradeLevelBW[iscript_bullet->sourceUnit->playerID].items[8]
                    || (Unit_PrototypeFlags[iscript_bullet->sourceUnit->unitType] & UnitPrototypeFlags::Hero) && IsExpansion))
            {
                sprite_id = 0x1F9;
            }
            ISCRIPT_CreateSprite_(image, sprite_id, x, y, image->spriteOwner->elevationLevel + 1);
            continue;
        }
        case opc_highsprol:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_CreateSprite_(image, sprite_id, x, y, image->spriteOwner->elevationLevel - 1);
            continue;
        }
        case opc_lowsprul:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_CreateSprite_(image, sprite_id, x, y, 1);
            continue;
        }
        case opc_uflunstable:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 flingy_id = take_iscript_datum<_WORD>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            if (CFlingy* v45 = ISCRIPT_CreateFlingy(0, image->spriteOwner->position.y + image->verticalOffset, (__int16)image->spriteOwner->position.x + image->horizontalOffset, (FlingyID)flingy_id))
            {
                CThingy* v46 = (CThingy*)uflunstableRandomize(v45);
                sub_4878F0(v46);
            }
            continue;
        }
        case opc_spruluselo:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            if (iscript_unit && (iscript_unit->statusFlags & (StatusFlags::Cloaked | StatusFlags::RequiresDetection)) && !Image_DrawIfCloaked[Sprites_Image[sprite_id]])
            {
                continue;
            }
            if (CThingy* thingy = ISCRIPT_CreateSprite_(image, sprite_id, x, y, image->spriteOwner->elevationLevel))
            {
                setAllOverlayDirectionsGeneric(thingy, (image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) ? 32 - image->direction : image->direction);
            }
            continue;
        }
        case opc_sprul:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 v61 = take_iscript_datum<_WORD>(program_state);
            char v63 = take_iscript_datum<char>(program_state);
            char v64 = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            if (iscript_unit && (iscript_unit->statusFlags & (StatusFlags::Cloaked | StatusFlags::RequiresDetection)) && !Image_DrawIfCloaked[Sprites_Image[v61]])
            {
                continue;
            }
            if (CThingy* thingy = ISCRIPT_CreateSprite_(image, v61, v63, v64, image->spriteOwner->elevationLevel - 1))
            {
                setAllOverlayDirectionsGeneric(thingy, (image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) ? 32 - image->direction : image->direction);
            }
            continue;
        }
        case opc_sproluselo:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char v62 = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            point v125;
            ISCRIPT_UseLOFile(&v125, image, v62, 0);
            if (CThingy* thingy = ISCRIPT_CreateSprite_(image, sprite_id, v125.x, v125.y, image->spriteOwner->elevationLevel + 1))
            {
                setAllOverlayDirectionsGeneric(thingy, (image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) ? 32 - image->direction : image->direction);
            }
            continue;
        }
        case opc_end:
            if (noop)
            {
                continue;
            }
            ImageDestructor(image);
            program_state->program_counter = (BYTE*)v5 - (BYTE*)iscript_data;
            return;
        case opc_setflipstate:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char arg = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            flipImage(image, arg);
            continue;
        }
        case opc_playsnd:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 sfx = take_iscript_datum<unsigned __int16>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_PlaySnd((SfxData)sfx, image);
            continue;
        }
        case opc_playsndbtwn:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 v79 = take_iscript_datum<unsigned __int16>(program_state);
            unsigned __int16 v81 = take_iscript_datum<unsigned __int16>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_PlaySnd((SfxData)(v79 + RandomizeShort(5) % ((unsigned int)v81 - v79 + 1)), image);
            continue;
        }
        case opc_domissiledmg:
            if (noop)
            {
                continue;
            }
            CBullet_Damage(iscript_bullet);
            continue;
        case opc_attackmelee:
            if (noop)
            {
                continue;
            }
            ISCRIPT_AttackMelee(iscript_unit);
            [[fallthrough]];
        case opc_playsndrand:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int8 arg_count = take_iscript_datum<char>(program_state);
            u16* args = take_iscript_data<u16>(program_state, arg_count);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_PlaySnd((SfxData)args[RandomizeShort(4) % arg_count], image);
            continue;
        }
        case opc_followmaingraphic:
            if (noop)
            {
                continue;
            }
            v84 = image->spriteOwner->pImagePrimary;
            if (v84 == nullptr || image->frameIndex == v84->frameIndex && ((image->flags ^ v84->flags) & ImageFlags::IF_HORIZONTALLY_FLIPPED) == 0)
            {
                continue;
            }
            image->frameSet = v84->frameSet;
            image->direction = v84->direction;
            image->flags ^= ((image->flags ^ v84->flags) & ImageFlags::IF_HORIZONTALLY_FLIPPED);
            v86 = setImagePaletteType(image, image->paletteType);
            updateImageFrameIndex(v86);
            continue;
        case opc_randcondjmp:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int8 v92 = take_iscript_datum<char>(program_state);
            unsigned __int16 new_pc = take_iscript_datum<unsigned short>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;

            if ((unsigned __int8)RandomizeShort(7) <= (unsigned int)v92)
            {
                program_state->program_counter = new_pc;
                v5 = (char*)iscript_data + program_state->program_counter;
            }
            continue;
        }
        case opc_turnccwise:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char arg = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            turn_unit_left(iscript_unit, arg);
            continue;
        }
        case opc_turncwise:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char arg = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            turnUnit(iscript_unit, iscript_unit->currentDirection1 + 8 * arg);
            continue;
        }
        case opc_turn1cwise:
            if (noop)
            {
                continue;
            }
            if (iscript_unit->orderTarget.pUnit == nullptr)
            {
                turnUnit(iscript_unit, iscript_unit->currentDirection1 + 8);
            }
            continue;
        case opc_turnrand:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char arg = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            if ((RandomizeShort(6) & 3) == 1)
            {
                turn_unit_left(iscript_unit, arg);
            }
            else
            {
                turnUnit(iscript_unit, iscript_unit->currentDirection1 + 8 * arg);
            }
            continue;
        }
        case opc_setspawnframe:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char arg = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            unitSetRetreatPoint(arg, iscript_unit);
            continue;
        }
        case opc_sigorder:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char arg = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            iscript_flingy->orderSignal |= arg;
            continue;
        }
        case opc_attackwith:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char arg = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_AttackWith(iscript_unit, arg);
            continue;
        }
        case opc_attack:
            if (noop)
            {
                continue;
            }
            if (iscript_unit->orderTarget.pUnit == nullptr || (iscript_unit->orderTarget.pUnit->statusFlags & StatusFlags::InAir) == 0)
            {
                ISCRIPT_AttackWith(iscript_unit, 1);
            }
            else
            {
                ISCRIPT_CastSpell(iscript_unit, Unit_AirWeapon[iscript_unit->unitType]);
            }
            continue;
        case opc_castspell:
            if (noop)
            {
                continue;
            }
            if (Orders_TargetingWeapon[iscript_unit->orderID] < WT_None && !canCastSpell_0(iscript_unit))
            {
                ISCRIPT_CastSpell(iscript_unit, Orders_TargetingWeapon[iscript_unit->orderID]);
            }
            continue;
        case opc_useweapon:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char arg = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_UseWeapon(iscript_unit, (WeaponType)arg);
            continue;
        }
        case opc_move:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char arg = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            unsigned int v117 = 0;
            BYTE1(v117) = arg;
            if (distance_moved)
            {
                *distance_moved = GetModifiedUnitSpeed(v117, iscript_unit);
            }
            if (noop)
            {
                continue;
            }
            int v120 = GetModifiedUnitSpeed(v117, iscript_unit);
            SetUnitMovementSpeed(iscript_unit, v120);
            continue;
        }
        case opc_gotorepeatattk:
            if (noop)
            {
                continue;
            }
            if (iscript_unit)
            {
                iscript_unit->movementFlags &= ~8;
            }
            continue;
        case opc_engframe:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int8 arg5 = take_iscript_datum<unsigned __int8>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            image->frameSet = arg5;
            CImage* v88 = image->spriteOwner->pImagePrimary;
            image->direction = v88->direction;
            image->flags ^= ((image->flags ^ v88->flags) & ImageFlags::IF_HORIZONTALLY_FLIPPED);
            CImage* v86 = setImagePaletteType(image, image->paletteType);
            updateImageFrameIndex(v86);
            continue;
        }
        case opc_engset:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int8 v89 = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            CImage* v88 = image->spriteOwner->pImagePrimary;
            image->direction = v88->direction;
            image->frameSet = v88->frameSet + v89 * (v88->GRPFile->wFrames & 0x7FFF);
            image->flags ^= ((image->flags ^ v88->flags) & ImageFlags::IF_HORIZONTALLY_FLIPPED);
            CImage* v86 = setImagePaletteType(image, image->paletteType);
            updateImageFrameIndex(v86);
            continue;
        }
        case opc___2d:
            if (noop)
            {
                continue;
            }
            wantThingyUpdate = 0;
            continue;
        case opc_nobrkcodestart:
            if (noop)
            {
                continue;
            }
            iscript_unit->statusFlags |= StatusFlags::NoBrkCodeStart;
            iscript_unit->sprite->flags |= 0x80u;
            continue;
        case opc_nobrkcodeend:
            if (noop)
            {
                continue;
            }
            if (iscript_unit)
            {
                ISCRIPT_NoBrkCodeEnd(iscript_unit);
            }
            continue;
        case opc_ignorerest:
            if (noop)
            {
                continue;
            }
            if (iscript_unit->orderTarget.pUnit)
            {
                program_state->program_counter = (BYTE*)v5 - (BYTE*)iscript_data - 1;
                program_state->wait = 10;
                return;
            }
            IgnoreAllScriptAndGotoIdle(iscript_unit);
            continue;
        case opc_attkshiftproj:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char arg = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            Weapon_XOffset[UnitGetGrndWeapon(iscript_unit)] = arg;
            ISCRIPT_AttackWith(iscript_unit, 1);
            continue;
        }
        case opc_tmprmgraphicstart:
            if (noop)
            {
                continue;
            }
            hideImage(image);
            continue;
        case opc_tmprmgraphicend:
            if (noop)
            {
                continue;
            }
            showImage(image);
            continue;
        case opc_setfldirect:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char arg = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            turnUnit(iscript_unit, 8 * arg);
            continue;
        }
        case opc_call:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            u16 new_pc = take_iscript_datum<u16>(program_state);
            program_state->unsigned4 = program_state->program_counter;
            program_state->program_counter = new_pc;
            v5 = (char*)iscript_data + program_state->program_counter;
            continue;
        }
        case opc_return:
            program_state->program_counter = program_state->unsigned4;
            v5 = (char*)iscript_data + program_state->program_counter;
            continue;
        case opc_setflspeed:
            v5 += 2;
            if (noop)
            {
                continue;
            }
            iscript_unit->flingyTopSpeed = *((unsigned __int16*)v5 - 1);
            continue;
        case opc_creategasoverlays:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char v65 = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_UseLOFile(&v124, image, 2, v65);
            int v67 = v65 + (iscript_unit->fields2.resource.resourceCount != 0 ? 430 : 435);
            if (CImage* v68 = sub_4D4E30())
            {
                image_Insert(v68, &image->spriteOwner->pImageHead, image);
                somePlayImageCrapThatCrashes(LOBYTE(v124.y) + image->verticalOffset, LOBYTE(v124.x) + image->horizontalOffset, v68, image->spriteOwner, (unsigned __int16)v67);
            }
            continue;
        }
        case opc_pwrupcondjmp:
            v5 += 2;
            if (image->spriteOwner && image->spriteOwner->pImagePrimary != image)
            {
                v5 = (char*)iscript_data + *((unsigned __int16*)v5 - 1);
            }
            continue;
        case opc_trgtrangecondjmp:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 v95 = take_iscript_datum<_WORD>(program_state);
            unsigned __int16 v97 = take_iscript_datum<unsigned short>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            if (iscript_unit->orderTarget.pUnit)
            {
                int a3;
                int a4;
                sub_4762C0(iscript_unit, (int)&a4, (int)&a3);
                if (isDistanceGreaterThanHaltDistance(a3, iscript_unit, v95, a4))
                {
                    program_state->program_counter = v97;
                    v5 = (char*)iscript_data + program_state->program_counter;
                }
            }
            continue;
        }
        case opc_trgtarccondjmp:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 v99 = take_iscript_datum<_WORD>(program_state);
            unsigned __int16 v128 = take_iscript_datum<unsigned short>(program_state);
            u16 new_pc = take_iscript_datum<u16>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
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
                    v5 = (char*)iscript_data + program_state->program_counter;
                }
            }
            continue;
        }
        case opc_curdirectcondjmp:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 v109 = take_iscript_datum<_WORD>(program_state);
            unsigned __int16 v111 = take_iscript_datum<unsigned short>(program_state);
            u16 new_pc = take_iscript_datum<u16>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            if (sub_494BD0(v109, iscript_unit->currentDirection1) < v111)
            {
                program_state->program_counter = new_pc;
                v5 = (char*)iscript_data + program_state->program_counter;
            }
            continue;
        }
        case opc_imgulnextid:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            ISCRIPT_CreateImage(image, image->imageID + 1, x + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + y), ImageOrder::IMGORD_BELOW);
            continue;
        }
        case opc_liftoffcondjmp:
            v5 += 2;
            if (noop)
            {
                continue;
            }
            if (iscript_unit->statusFlags & StatusFlags::InAir)
            {
                v5 = (char*)iscript_data + *((unsigned __int16*)v5 - 1);
            }
            continue;
        case opc_warpoverlay:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 arg = take_iscript_datum<unsigned __int16>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            image->flags |= ImageFlags::IF_REDRAW;
            image->coloringData = (void*)arg;
            continue;
        }
        case opc_orderdone:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            u8 arg = take_iscript_datum<u8>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;
            if (noop)
            {
                continue;
            }
            iscript_flingy->orderSignal &= ~arg;
            continue;
        }
        case opc_grdsprol:
        {
            program_state->program_counter = v5 - (char*)iscript_data;
            unsigned __int16 sprite_id = take_iscript_datum<_WORD>(program_state);
            char x = take_iscript_datum<char>(program_state);
            char y = take_iscript_datum<char>(program_state);
            v5 = (char*)iscript_data + program_state->program_counter;

            if (noop)
            {
                continue;
            }
            if (canUnitTypeFitAt(x + image->spriteOwner->position.x + image->horizontalOffset, Terran_Marine, y + image->spriteOwner->position.y + image->verticalOffset))
            {
                ISCRIPT_CreateSprite_(image, sprite_id, x, y, image->spriteOwner->elevationLevel + 1);
            }
            continue;
        }
        case opc_dogrddamage:
            if (noop)
            {
                continue;
            }
            CBullet_Damage(iscript_bullet);
            continue;
        default:
            continue;
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
