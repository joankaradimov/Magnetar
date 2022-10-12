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

void BWFXN_PlayIscript_(CImage* image, IScriptProgram* program_state, int noop, _DWORD* distance_moved)
{
    unsigned __int8 v13; // bl
    _BYTE* v14; // edi
    unsigned __int8 v15; // dl
    unsigned __int8 v16; // al
    unsigned __int16 v17; // ax
    char* v18; // edi
    char v19; // cl
    unsigned __int16 v20; // ax
    char* v21; // edi
    char v22; // cl
    char v23; // al
    unsigned __int16 v24; // ax
    unsigned __int16 v27; // ax
    unsigned __int16 v28; // bx
    unsigned __int8* v29; // edi
    unsigned __int8 v30; // cl
    unsigned __int16 v32; // bx
    unsigned __int8* v33; // edi
    unsigned __int8 v34; // cl
    _BYTE* v37; // edi
    unsigned __int16 v41; // ax
    char* v42; // edi
    char v43; // bl
    unsigned __int16 v44; // bx
    unsigned __int16 v47; // bx
    unsigned __int16 v50; // ax
    char* v51; // edi
    char v52; // cl
    unsigned __int16 v53; // ax
    char* v54; // edi
    char v55; // cl
    unsigned __int16 v56; // ax
    char* v57; // edi
    char v58; // cl
    char v59; // dl
    unsigned __int16 v61; // cx
    char* v62; // edi
    char v63; // dl
    char v64; // bl
    char v65; // bl
    int v67; // esi
    CImage* v84; // eax
    CImage* v86; // eax
    CImage* v88; // eax
    unsigned __int8 v89; // cl
    char v90; // dl
    unsigned __int8 v92; // dl
    _WORD* v93; // edi
    unsigned __int16 v94; // bx
    unsigned __int16 v95; // dx
    _WORD* v96; // edi
    unsigned __int16 v97; // bx
    unsigned __int16 v99; // dx
    __int16* v100; // edi
    char v107; // al
    unsigned __int16 v109; // ax
    _WORD* v110; // edi
    unsigned __int16 v111; // dx
    unsigned int v117; // ebx
    int v120; // eax
    point v124; // [esp+0h] [ebp-40h] BYREF
    point v125; // [esp+8h] [ebp-38h] BYREF
    point v126; // [esp+10h] [ebp-30h] BYREF
    point a1; // [esp+18h] [ebp-28h] BYREF
    unsigned __int16 v128; // [esp+20h] [ebp-20h]

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
            v13 = *v5;
            v14 = v5 + 1;
            v15 = *v14;
            v5 = v14 + 1;
            if (noop)
            {
                continue;
            }
            v16 = RandomizeShort(3);
            program_state->program_counter = (BYTE*)v5 - (BYTE*)iscript_data;
            program_state->wait = v13 + v16 % (v15 - v13 + 1) - 1;
            return;
        case opc_goto:
            v5 = (char*)iscript_data + *(unsigned __int16*)v5;
            continue;
        case opc_imgol:
            v17 = *(_WORD*)v5;
            v18 = v5 + 2;
            v19 = *v18;
            v5 = v18 + 2;
            if (noop)
            {
                continue;
            }
            ISCRIPT_CreateImage(image, v17, v19 + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + *(v5 - 1)), ImageOrder::IMGORD_ABOVE);
            continue;
        case opc_imgul:
            v20 = *(_WORD*)v5;
            v21 = v5 + 2;
            v22 = *v21;
            v5 = v21 + 2;
            if (noop)
            {
                continue;
            }
            ISCRIPT_CreateImage(image, v20, v22 + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + *(v5 - 1)), ImageOrder::IMGORD_BELOW);
            continue;
        case opc_imgolorig:
            v24 = *(_WORD*)v5;
            v5 += 2;
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
        case opc_switchul:
            v27 = *(_WORD*)v5;
            v5 += 2;
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
        case opc_imgoluselo:
            v28 = *(_WORD*)v5;
            v29 = (unsigned __int8*)(v5 + 2);
            v30 = *v29;
            v5 = (char*)(v29 + 2);
            if (noop)
            {
                continue;
            }
            ISCRIPT_UseLOFile(&a1, image, v30, *(v5 - 1));
            ISCRIPT_CreateImage(image, v28, LOBYTE(a1.x) + image->horizontalOffset, (unsigned __int8)(LOBYTE(a1.y) + image->verticalOffset), ImageOrder::IMGORD_ABOVE);
            continue;
        case opc_imguluselo:
            v32 = *(_WORD*)v5;
            v33 = (unsigned __int8*)(v5 + 2);
            v34 = *v33;
            v5 = (char*)(v33 + 2);
            if (noop)
            {
                continue;
            }
            ISCRIPT_UseLOFile(&v126, image, v34, *(v5 - 1));
            ISCRIPT_CreateImage(image, v32, LOBYTE(v126.x) + image->horizontalOffset, (unsigned __int8)(LOBYTE(v126.y) + image->verticalOffset), ImageOrder::IMGORD_BELOW);
            continue;
        case opc_sprol:
            v41 = *(_WORD*)v5;
            v42 = v5 + 2;
            v43 = *v42;
            v5 = v42 + 2;
            if (noop)
            {
                continue;
            }
            if (iscript_bullet && iscript_bullet->sourceUnit && UnitIsGoliath(iscript_bullet->sourceUnit)
                && (UpgradeLevelBW[iscript_bullet->sourceUnit->playerID].items[8]
                    || (Unit_PrototypeFlags[iscript_bullet->sourceUnit->unitType] & UnitPrototypeFlags::Hero) && IsExpansion))
            {
                v41 = 0x1F9;
            }
            ISCRIPT_CreateSprite(image, v41, v43, *(v5 - 1), image->spriteOwner->elevationLevel + 1);
            continue;
        case opc_highsprol:
            v50 = *(_WORD*)v5;
            v51 = v5 + 2;
            v52 = *v51;
            v5 = v51 + 2;
            if (noop)
            {
                continue;
            }
            ISCRIPT_CreateSprite(image, v50, v52, *(v5 - 1), image->spriteOwner->elevationLevel - 1);
            continue;
        case opc_lowsprul:
            v53 = *(_WORD*)v5;
            v54 = v5 + 2;
            v55 = *v54;
            v5 = v54 + 2;
            if (noop)
            {
                continue;
            }
            ISCRIPT_CreateSprite(image, v53, v55, *(v5 - 1), 1);
            continue;
        case opc_uflunstable:
            v44 = *(_WORD*)v5;
            v5 += 2;
            if (noop)
            {
                continue;
            }
            if (CFlingy* v45 = ISCRIPT_CreateFlingy(0, image->spriteOwner->position.y + image->verticalOffset, (__int16)image->spriteOwner->position.x + image->horizontalOffset, (FlingyID)v44))
            {
                CThingy* v46 = (CThingy*)uflunstableRandomize(v45);
                sub_4878F0(v46);
            }
            continue;
        case opc_spruluselo:
            v56 = *(_WORD*)v5;
            v57 = v5 + 2;
            v58 = *v57++;
            v59 = *v57;
            v5 = v57 + 1;
            if (noop)
            {
                continue;
            }
            if (iscript_unit && (iscript_unit->statusFlags & 0x300) && !Image_DrawIfCloaked[Sprites_Image[v56]])
            {
                continue;
            }
            if (CThingy* thingy = ISCRIPT_CreateSprite(image, v56, v58, v59, image->spriteOwner->elevationLevel))
            {
                setAllOverlayDirectionsGeneric(thingy, (image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) ? 32 - image->direction : image->direction);
            }
            continue;
        case opc_sprul:
            v61 = *(_WORD*)v5;
            v62 = v5 + 2;
            v63 = *v62++;
            v64 = *v62;
            v5 = v62 + 1;
            if (noop)
            {
                continue;
            }
            if (iscript_unit && (iscript_unit->statusFlags & 0x300) && !Image_DrawIfCloaked[Sprites_Image[v61]])
            {
                continue;
            }
            if (CThingy* thingy = ISCRIPT_CreateSprite(image, v61, v63, v64, image->spriteOwner->elevationLevel - 1))
            {
                setAllOverlayDirectionsGeneric(thingy, (image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) ? 32 - image->direction : image->direction);
            }
            continue;
        case opc_sproluselo:
            v47 = *(_WORD*)v5;
            v5 += 3;
            if (noop)
            {
                continue;
            }
            ISCRIPT_UseLOFile(&v125, image, *(v5 - 1), 0);
            if (CThingy* thingy = ISCRIPT_CreateSprite(image, v47, v125.x, v125.y, image->spriteOwner->elevationLevel + 1))
            {
                setAllOverlayDirectionsGeneric(thingy, (image->flags & ImageFlags::IF_HORIZONTALLY_FLIPPED) ? 32 - image->direction : image->direction);
            }
            continue;
        case opc_end:
            if (noop)
            {
                continue;
            }
            ImageDestructor(image);
            program_state->program_counter = (BYTE*)v5 - (BYTE*)iscript_data;
            return;
        case opc_setflipstate:
            ++v5;
            if (noop)
            {
                continue;
            }
            flipImage(image, *(v5 - 1));
            continue;
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
            v92 = *v5;
            v93 = (short*)(v5 + 1);
            v94 = *v93;
            v5 = (char*)(v93 + 1);
            if ((unsigned __int8)RandomizeShort(7) <= (unsigned int)v92)
            {
                v5 = (char*)iscript_data + v94;
            }
            continue;
        case opc_turnccwise:
            ++v5;
            if (noop)
            {
                continue;
            }
            turn_unit_left(iscript_unit, *(v5 - 1));
            continue;
        case opc_turncwise:
            ++v5;
            if (noop)
            {
                continue;
            }
            turnUnit(iscript_unit, iscript_unit->currentDirection1 + 8 * *(v5 - 1));
            continue;
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
            v90 = *v5++;
            if (noop)
            {
                continue;
            }
            if ((RandomizeShort(6) & 3) == 1)
            {
                turn_unit_left(iscript_unit, v90);
            }
            else
            {
                turnUnit(iscript_unit, iscript_unit->currentDirection1 + 8 * v90);
            }
            continue;
        case opc_setspawnframe:
            ++v5;
            if (noop)
            {
                continue;
            }
            unitSetRetreatPoint(*(v5 - 1), iscript_unit);
            continue;
        case opc_sigorder:
            ++v5;
            if (noop)
            {
                continue;
            }
            iscript_flingy->orderSignal |= *(v5 - 1);
            continue;
        case opc_attackwith:
            ++v5;
            if (noop)
            {
                continue;
            }
            ISCRIPT_AttackWith(iscript_unit, *(v5 - 1));
            continue;
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
            ++v5;
            if (noop)
            {
                continue;
            }
            ISCRIPT_UseWeapon(iscript_unit, (WeaponType) * (v5 - 1));
            continue;
        case opc_move:
            ++v5;
            v117 = 0;
            BYTE1(v117) = *(v5 - 1);
            if (distance_moved)
            {
                *distance_moved = GetModifiedUnitSpeed(v117, iscript_unit);
            }
            if (noop)
            {
                continue;
            }
            v120 = GetModifiedUnitSpeed(v117, iscript_unit);
            SetUnitMovementSpeed(iscript_unit, v120);
            continue;
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
            ++v5;
            if (noop)
            {
                continue;
            }
            image->frameSet = (unsigned __int8)*(v5 - 1);
            v88 = image->spriteOwner->pImagePrimary;
            image->direction = v88->direction;
            image->flags ^= ((image->flags ^ v88->flags) & ImageFlags::IF_HORIZONTALLY_FLIPPED);
            v86 = setImagePaletteType(image, image->paletteType);
            updateImageFrameIndex(v86);
            continue;
        case opc_engset:
            v89 = *v5++;
            if (noop)
            {
                continue;
            }
            v88 = image->spriteOwner->pImagePrimary;
            image->direction = v88->direction;
            image->frameSet = v88->frameSet + v89 * (v88->GRPFile->wFrames & 0x7FFF);
            image->flags ^= ((image->flags ^ v88->flags) & ImageFlags::IF_HORIZONTALLY_FLIPPED);
            v86 = setImagePaletteType(image, image->paletteType);
            updateImageFrameIndex(v86);
            continue;
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
            ++v5;
            if (noop)
            {
                continue;
            }
            Weapon_XOffset[UnitGetGrndWeapon(iscript_unit)] = *(v5 - 1);
            ISCRIPT_AttackWith(iscript_unit, 1);
            continue;
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
            ++v5;
            if (noop)
            {
                continue;
            }
            turnUnit(iscript_unit, 8 * *(v5 - 1));
            continue;
        case opc_call:
            program_state->unsigned4 = (BYTE*)v5 + 2 - (BYTE*)iscript_data;
            v5 = (char*)iscript_data + *(unsigned __int16*)v5;
            continue;
        case opc_return:
            v5 = (char*)iscript_data + program_state->unsigned4;
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
            v65 = *v5++;
            if (noop)
            {
                continue;
            }
            ISCRIPT_UseLOFile(&v124, image, 2, v65);
            v67 = v65 + (iscript_unit->fields2.resource.resourceCount != 0 ? 430 : 435);
            if (CImage* v68 = sub_4D4E30())
            {
                image_Insert(v68, &image->spriteOwner->pImageHead, image);
                somePlayImageCrapThatCrashes(LOBYTE(v124.y) + image->verticalOffset, LOBYTE(v124.x) + image->horizontalOffset, v68, image->spriteOwner, (unsigned __int16)v67);
            }
            continue;
        case opc_pwrupcondjmp:
            v5 += 2;
            if (image->spriteOwner && image->spriteOwner->pImagePrimary != image)
            {
                v5 = (char*)iscript_data + *((unsigned __int16*)v5 - 1);
            }
            continue;
        case opc_trgtrangecondjmp:
            v95 = *(_WORD*)v5;
            v96 = (short*)(v5 + 2);
            v97 = *v96;
            v5 = (char*)(v96 + 1);
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
                    v5 = (char*)iscript_data + v97;
                }
            }
            continue;
        case opc_trgtarccondjmp:
            v99 = *(_WORD*)v5;
            v100 = (__int16*)(v5 + 2);
            v128 = *v100;
            v5 = (char*)(v100 + 2);
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
                v107 = GetAngle(v106, v123, v105, v104);
                if (sub_494BD0((int)v99, v107) < v128)
                {
                    v5 = (char*)iscript_data + *((unsigned __int16*)v5 - 1);
                }
            }
            continue;
        case opc_curdirectcondjmp:
            v109 = *(_WORD*)v5;
            v110 = (short*)(v5 + 2);
            v111 = *v110;
            v5 = (char*)(v110 + 2);
            if (noop)
            {
                continue;
            }
            if (sub_494BD0(v109, iscript_unit->currentDirection1) < v111)
            {
                v5 = (char*)iscript_data + *((unsigned __int16*)v5 - 1);
            }
            continue;
        case opc_imgulnextid:
            v23 = *v5;
            v5 += 2;
            if (noop)
            {
                continue;
            }
            ISCRIPT_CreateImage(image, image->imageID + 1, v23 + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + *(v5 - 1)), ImageOrder::IMGORD_BELOW);
            continue;
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
                ISCRIPT_CreateSprite(image, sprite_id, x, y, image->spriteOwner->elevationLevel + 1);
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
