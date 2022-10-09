#include "iscript.h"
#include "starcraft.h"
#include "patching.h"

void BWFXN_PlayIscript_(CImage* image, IScriptProgram* program_state, int noop, _DWORD* distance_moved)
{
    void* v8; // eax
    unsigned __int16 v9; // ax
    s8 v10; // al
    s8 v11; // al
    char v12; // dl
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
    unsigned __int16 v36; // dx
    _BYTE* v37; // edi
    char v38; // dl
    unsigned __int16 v41; // ax
    char* v42; // edi
    char v43; // bl
    unsigned __int16 v44; // bx
    CThingy* v46; // eax
    unsigned __int16 v47; // bx
    CImage* v48; // edx
    CThingy* v49; // eax
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
    CThingy* v60; // eax
    unsigned __int16 v61; // cx
    char* v62; // edi
    char v63; // dl
    char v64; // bl
    char v65; // bl
    int v66; // ecx
    int v67; // esi
    CImage* v68; // eax
    CImage* v69; // ebx
    CSprite* v71; // eax
    bool v72; // zf
    unsigned __int8 v73; // bl
    char* v74; // edi
    unsigned __int8 v75; // dl
    int v76; // edi
    int v77; // ecx
    SfxData v78; // ebx
    unsigned __int16 v79; // ax
    char* v80; // edi
    unsigned __int16 v81; // dx
    int v82; // ebx
    unsigned int v83; // eax
    CImage* v84; // eax
    CImage* v86; // eax
    CSprite* v87; // edx
    CImage* v88; // eax
    unsigned __int8 v89; // cl
    char v90; // dl
    CUnit* v91; // eax
    unsigned __int8 v92; // dl
    _WORD* v93; // edi
    unsigned __int16 v94; // bx
    unsigned __int16 v95; // dx
    _WORD* v96; // edi
    unsigned __int16 v97; // bx
    CUnit* v98; // esi
    unsigned __int16 v99; // dx
    __int16* v100; // edi
    int v104; // eax
    int v105; // ecx
    int v106; // edx
    char v107; // al
    unsigned int v108; // eax
    unsigned __int16 v109; // ax
    _WORD* v110; // edi
    unsigned __int16 v111; // dx
    CUnit* v112; // eax MAPDST
    struct CUnit* v113; // ecx
    unsigned int v117; // ebx
    unsigned int v118; // eax
    CUnit* v119; // ebx
    int v120; // eax
    CUnit* v121; // eax
    int v123; // [esp-10h] [ebp-50h]
    point v124; // [esp+0h] [ebp-40h] BYREF
    point v125; // [esp+8h] [ebp-38h] BYREF
    point v126; // [esp+10h] [ebp-30h] BYREF
    point a1; // [esp+18h] [ebp-28h] BYREF
    unsigned __int16 v128; // [esp+20h] [ebp-20h]
    int a3; // [esp+24h] [ebp-1Ch] BYREF
    int a4; // [esp+28h] [ebp-18h] BYREF
    CUnit* v131; // [esp+2Ch] [ebp-14h] FORCED
    WeaponType weapon_id; // [esp+30h] [ebp-10h]
    char v136; // [esp+3Fh] [ebp-1h]

    if (program_state->wait)
    {
        program_state->wait -= 1;
        return;
    }
    char* v5 = (char*)iscript_data + program_state->program_counter;

    while (2)
    {
        IScriptOpcodes v7 = (IScriptOpcodes) *v5++;
        switch (v7)
        {
        case opc_playfram:
            v5 += 2;
            if (!noop)
            {
                ISCRIPT_PlayFrame(image, *((unsigned __int16*)v5 - 1));
            }
            continue;
        case opc_playframtile:
            v5 += 2;
            if (!noop)
            {
                v9 = CurrentTileSet + *((_WORD*)v5 - 1);
                if (v9 < (unsigned __int16)(image->GRPFile->wFrames & 0x7FFF))
                {
                    ISCRIPT_PlayFrame(image, v9);
                }
            }
            continue;
        case opc_sethorpos:
            v10 = *v5++;
            if (!noop && image->horizontalOffset != v10)
            {
                image->flags |= ImageFlags::IF_REDRAW;
                image->horizontalOffset = v10;
            }
            continue;
        case opc_setvertpos:
            v11 = *v5++;
            if (!noop && (!iscript_unit || (iscript_unit->statusFlags & 0x300) == 0) && image->verticalOffset != v11)
            {
                image->flags |= ImageFlags::IF_REDRAW;
                image->verticalOffset = v11;
            }
            continue;
        case opc_setpos:
            v12 = *v5;
            v5 += 2;
            if (!noop)
            {
                ISCRIPT_setPosition(image, v12, *(v5 - 1));
            }
            continue;
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
            if (!noop)
            {
                ISCRIPT_CreateImage(image, v17, v19 + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + *(v5 - 1)), IMGORD_ABOVE);
            }
            continue;
        case opc_imgul:
            v20 = *(_WORD*)v5;
            v21 = v5 + 2;
            v22 = *v21;
            v5 = v21 + 2;
            if (!noop)
            {
                ISCRIPT_CreateImage(image, v20, v22 + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + *(v5 - 1)), IMGORD_BELOW);
            }
            continue;
        case opc_imgolorig:
            v24 = *(_WORD*)v5;
            v5 += 2;
            if (noop)
            {
                continue;
            }
            if (CImage* new_image = ISCRIPT_CreateImage(image, v24, 0, 0, IMGORD_ABOVE))
            {
                if ((new_image->flags & IF_USES_SPECIAL_OFFSET) == 0)
                {
                    new_image->flags |= IF_USES_SPECIAL_OFFSET;
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
            if (CImage* new_image = ISCRIPT_CreateImage(image, v27, 0, 0, IMGORD_BELOW))
            {
                if ((new_image->flags & IF_USES_SPECIAL_OFFSET) == 0)
                {
                    new_image->flags |= IF_USES_SPECIAL_OFFSET;
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
            ISCRIPT_CreateImage(image, v28, LOBYTE(a1.x) + image->horizontalOffset, (unsigned __int8)(LOBYTE(a1.y) + image->verticalOffset), IMGORD_ABOVE);
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
            ISCRIPT_CreateImage(image, v32, LOBYTE(v126.x) + image->horizontalOffset, (unsigned __int8)(LOBYTE(v126.y) + image->verticalOffset), IMGORD_BELOW);
            continue;
        case opc_sprol:
            v41 = *(_WORD*)v5;
            v42 = v5 + 2;
            v43 = *v42;
            v5 = v42 + 2;
            if (!noop)
            {
                if (iscript_bullet && (v131 = iscript_bullet->sourceUnit) != 0 && UnitIsGoliath(v131)
                    && (UpgradeLevelBW[v131->playerID].items[8]
                        || (Unit_PrototypeFlags[v131->unitType] & 0x40) != 0 && IsExpansion))
                {
                    ISCRIPT_CreateSprite(image, 0x1F9u, v43, *(v5 - 1), image->spriteOwner->elevationLevel + 1);
                }
                else
                {
                    ISCRIPT_CreateSprite(image, v41, v43, *(v5 - 1), image->spriteOwner->elevationLevel + 1);
                }
            }
            continue;
        case opc_highsprol:
            v50 = *(_WORD*)v5;
            v51 = v5 + 2;
            v52 = *v51;
            v5 = v51 + 2;
            if (!noop)
            {
                ISCRIPT_CreateSprite(image, v50, v52, *(v5 - 1), image->spriteOwner->elevationLevel - 1);
            }
            continue;
        case opc_lowsprul:
            v53 = *(_WORD*)v5;
            v54 = v5 + 2;
            v55 = *v54;
            v5 = v54 + 2;
            if (!noop)
            {
                ISCRIPT_CreateSprite(image, v53, v55, *(v5 - 1), 1);
            }
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
                v46 = (CThingy*)uflunstableRandomize(v45);
                sub_4878F0(v46);
            }
            continue;
        case opc_spruluselo:
            v56 = *(_WORD*)v5;
            v57 = v5 + 2;
            v58 = *v57++;
            v59 = *v57;
            v5 = v57 + 1;
            if (noop || iscript_unit && (BYTE1(iscript_unit->statusFlags) & 3) != 0 && !Image_DrawIfCloaked[Sprites_Image[v56]])
            {
                continue;
            }
            v60 = ISCRIPT_CreateSprite(image, v56, v58, v59, image->spriteOwner->elevationLevel);
            goto LABEL_80;
        case opc_sprul:
            v61 = *(_WORD*)v5;
            v62 = v5 + 2;
            v63 = *v62++;
            v64 = *v62;
            v5 = v62 + 1;
            if (noop || iscript_unit && (BYTE1(iscript_unit->statusFlags) & 3) != 0 && !Image_DrawIfCloaked[Sprites_Image[v61]])
            {
                continue;
            }
            v60 = ISCRIPT_CreateSprite(image, v61, v63, v64, image->spriteOwner->elevationLevel - 1);
        LABEL_80:
            if (!v60)
            {
                continue;
            }
            if (image->flags & 2)
            {
                setAllOverlayDirectionsGeneric(v60, 32 - image->direction);
            }
            else
            {
                setAllOverlayDirectionsGeneric(v60, image->direction);
            }
            continue;
        case opc_sproluselo:
            v47 = *(_WORD*)v5;
            v5 += 3;
            if (noop)
            {
                continue;
            }
            v48 = image;
            ISCRIPT_UseLOFile(&v125, image, *(v5 - 1), 0);
            v49 = ISCRIPT_CreateSprite(v48, v47, v125.x, v125.y, v48->spriteOwner->elevationLevel + 1);
            if (v49)
            {
                if (image->flags & 2)
                {
                    setAllOverlayDirectionsGeneric(v49, 32 - image->direction);
                }
                else
                {
                    setAllOverlayDirectionsGeneric(v49, image->direction);
                }
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
            if (!noop)
            {
                flipImage(image, *(v5 - 1));
            }
            continue;
        case opc_playsnd:
            v5 += 2;
            if (!noop)
            {
                ISCRIPT_PlaySnd((SfxData) * ((unsigned __int16*)v5 - 1), image);
            }
            continue;
        case opc_playsndrand:
            goto LABEL_98;
        case opc_playsndbtwn:
            v79 = *(_WORD*)v5;
            v80 = v5 + 2;
            v81 = *(_WORD*)v80;
            v5 = v80 + 2;
            if (!noop)
            {
                v82 = v79;
                int v131 = v79;
                v83 = RandomizeShort(5);
                ISCRIPT_PlaySnd((SfxData)(v131 + v83 % ((unsigned int)v81 - v82 + 1)), image);
            }
            continue;
        case opc_domissiledmg:
            if (noop)
            {
                continue;
            }
            CBullet_Damage(iscript_bullet);
            continue;
        case opc_attackmelee:
            if (!noop)
            {
                ISCRIPT_AttackMelee(iscript_unit);
            }
        LABEL_98:
            v73 = *v5;
            v74 = v5 + 1;
            if (noop)
            {
                v5 = &v74[2 * v73];
            }
            else
            {
                v75 = RandomizeShort(4) % v73;
                v76 = (int)&v74[2 * v75 + 2];
                v77 = (unsigned __int8)(v73 - v75 - 1);
                v78 = SfxData(*(unsigned __int16*)(v76 - 2));
                v5 = (char*)(v76 + 2 * v77);
                ISCRIPT_PlaySnd(v78, image);
            }
            continue;
        case opc_followmaingraphic:
            if (noop)
            {
                continue;
            }
            v84 = image->spriteOwner->pImagePrimary;
            if (v84 == nullptr || image->frameIndex == v84->frameIndex && ((image->flags ^ v84->flags) & IF_HORIZONTALLY_FLIPPED) == 0)
            {
                continue;
            }
            image->frameSet = v84->frameSet;
            image->direction = v84->direction;
            image->flags ^= ((image->flags ^ v84->flags) & IF_HORIZONTALLY_FLIPPED);
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
            if (!noop)
            {
                turn_unit_left(iscript_unit, *(v5 - 1));
            }
            continue;
        case opc_turncwise:
            ++v5;
            if (!noop)
            {
                turnUnit(iscript_unit, iscript_unit->currentDirection1 + 8 * *(v5 - 1));
            }
            continue;
        case opc_turn1cwise:
            if (!noop && !iscript_unit->orderTarget.pUnit)
            {
                turnUnit(iscript_unit, iscript_unit->currentDirection1 + 8);
            }
            continue;
        case opc_turnrand:
            v90 = *v5++;
            if (!noop)
            {
                v72 = (RandomizeShort(6) & 3) == 1;
                v91 = iscript_unit;
                if (v72)
                {
                    turn_unit_left(v91, v90);
                }
                else
                {
                    turnUnit(v91, v91->currentDirection1 + 8 * v90);
                }
            }
            continue;
        case opc_setspawnframe:
            ++v5;
            if (!noop)
            {
                unitSetRetreatPoint(*(v5 - 1), iscript_unit);
            }
            continue;
        case opc_sigorder:
            ++v5;
            if (!noop)
            {
                iscript_flingy->orderSignal |= *(v5 - 1);
            }
            continue;
        case opc_attackwith:
            ++v5;
            if (!noop)
            {
                ISCRIPT_AttackWith(iscript_unit, *(v5 - 1));
            }
            continue;
        case opc_attack:
            if (noop)
            {
                continue;
            }
            v112 = iscript_unit;
            v113 = iscript_unit->orderTarget.pUnit;
            if (!v113 || (v113->statusFlags & 4) == 0)
            {
                ISCRIPT_AttackWith(v112, 1);
                continue;
            }
            weapon_id = Unit_AirWeapon[v112->unitType];
            FireUnitWeapon(v112, weapon_id);
            if (Weapon_DamageFactor[weapon_id] == 2)
            {
                FireUnitWeapon(v112, weapon_id);
            }
            continue;
        case opc_castspell:
            if (noop || Orders_TargetingWeapon[iscript_unit->orderID] >= WT_None)
            {
                continue;
            }
            if (!canCastSpell_0(iscript_unit))
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
            if (!noop)
            {
                v118 = v117;
                v119 = iscript_unit;
                v120 = GetModifiedUnitSpeed(v118, iscript_unit);
                SetUnitMovementSpeed(v119, v120);
            }
            continue;
        case opc_gotorepeatattk:
            if (!noop && iscript_unit)
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
            v87 = image->spriteOwner;
            image->frameSet = (unsigned __int8)*(v5 - 1);
            v88 = v87->pImagePrimary;
            image->direction = v88->direction;
            image->flags ^= ImageFlags((LOBYTE(image->flags) ^ LOBYTE(v88->flags)) & 2);
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
            image->flags ^= ImageFlags((LOBYTE(image->flags) ^ LOBYTE(v88->flags)) & 2);
            v86 = setImagePaletteType(image, image->paletteType);
            updateImageFrameIndex(v86);
            continue;
        case opc___2d:
            if (!noop)
            {
                wantThingyUpdate = 0;
            }
            continue;
        case opc_nobrkcodestart:
            if (!noop)
            {
                v121 = iscript_unit;
                v121->statusFlags |= StatusFlags(0x2000);
                v121->sprite->flags |= 0x80u;
            }
            continue;
        case opc_nobrkcodeend:
            if (!noop && iscript_unit)
            {
                ISCRIPT_NoBrkCodeEnd(iscript_unit);
            }
            continue;
        case opc_ignorerest:
            if (noop)
            {
                continue;
            }
            if (!iscript_unit->orderTarget.pUnit)
            {
                IgnoreAllScriptAndGotoIdle(iscript_unit);
                continue;
            }
            program_state->program_counter = (BYTE*)v5 - (BYTE*)iscript_data - 1;
            program_state->wait = 10;
            return;
        case opc_attkshiftproj:
            ++v5;
            if (!noop)
            {
                Weapon_XOffset[UnitGetGrndWeapon(iscript_unit)] = *(v5 - 1);
                ISCRIPT_AttackWith(iscript_unit, 1);
            }
            continue;
        case opc_tmprmgraphicstart:
            if (noop)
            {
                continue;
            }
            hideImage(image);
            continue;
        case opc_tmprmgraphicend:
            if (!noop)
            {
                showImage(image);
            }
            continue;
        case opc_setfldirect:
            ++v5;
            if (!noop)
            {
                turnUnit(iscript_unit, 8 * *(v5 - 1));
            }
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
            if (!noop)
            {
                iscript_unit->flingyTopSpeed = *((unsigned __int16*)v5 - 1);
            }
            continue;
        case opc_creategasoverlays:
            v65 = *v5++;
            if (noop)
            {
                continue;
            }
            ISCRIPT_UseLOFile(&v124, image, 2, v65);
            LOWORD(v66) = v65;
            v67 = v66 + (iscript_unit->fields2.resource.resourceCount != 0 ? 430 : 435);
            v68 = sub_4D4E30();
            v69 = v68;
            if (v68)
            {
                image_Insert(v68, &image->spriteOwner->pImageHead, image);
                somePlayImageCrapThatCrashes(LOBYTE(v124.y) + image->verticalOffset, LOBYTE(v124.x) + image->horizontalOffset, v69, image->spriteOwner, (unsigned __int16)v67);
            }
            continue;
        case opc_pwrupcondjmp:
            v71 = image->spriteOwner;
            v5 += 2;
            if (!v71)
            {
                continue;
            }
            v72 = v71->pImagePrimary == image;
            goto LABEL_88;
        case opc_trgtrangecondjmp:
            v95 = *(_WORD*)v5;
            v96 = (short*)(v5 + 2);
            v97 = *v96;
            v5 = (char*)(v96 + 1);
            LOWORD(v131) = v95;
            if (noop || !iscript_unit->orderTarget.pUnit)
            {
                continue;
            }
            v98 = iscript_unit;
            sub_4762C0(iscript_unit, (int)&a4, (int)&a3);
            if (!isDistanceGreaterThanHaltDistance(a3, v98, (unsigned __int16)v131, a4))
            {
                continue;
            }
            v5 = (char*)iscript_data + v97;
            continue;
        case opc_trgtarccondjmp:
            v99 = *(_WORD*)v5;
            v100 = (__int16*)(v5 + 2);
            v128 = *v100;
            v5 = (char*)(v100 + 2);
            if (!noop)
            {
                if (CUnit* target = iscript_unit->orderTarget.pUnit)
                {
                    v104 = (__int16)target->sprite->position.y;
                    v105 = (__int16)target->sprite->position.x;
                    v106 = (__int16)iscript_unit->sprite->position.x;
                    v123 = (__int16)iscript_unit->sprite->position.y;
                    v107 = GetAngle(v106, v123, v105, v104);
                    v108 = sub_494BD0((int)v99, v107);
                    if (v108 < v128)
                    {
                        v5 = (char*)iscript_data + *((unsigned __int16*)v5 - 1);
                    }
                }
            }
            continue;
        case opc_curdirectcondjmp:
            v109 = *(_WORD*)v5;
            v110 = (short*)(v5 + 2);
            v111 = *v110;
            v5 = (char*)(v110 + 2);
            if (noop || sub_494BD0(v109, iscript_unit->currentDirection1) >= (unsigned int)v111)
            {
                continue;
            }
            v5 = (char*)iscript_data + *((unsigned __int16*)v5 - 1);
            continue;
        case opc_imgulnextid:
            v23 = *v5;
            v5 += 2;
            if (!noop)
            {
                ISCRIPT_CreateImage(image, image->imageID + 1, v23 + image->horizontalOffset, (unsigned __int8)(image->verticalOffset + *(v5 - 1)), IMGORD_BELOW);
            }
            continue;
        case opc_liftoffcondjmp:
            v5 += 2;
            if (noop)
            {
                continue;
            }
            v72 = (iscript_unit->statusFlags & 4) == 0;
        LABEL_88:
            if (!v72)
            {
                v5 = (char*)iscript_data + *((unsigned __int16*)v5 - 1);
            }
            continue;
        case opc_warpoverlay:
            v5 += 2;
            if (!noop)
            {
                v8 = (void*)*((unsigned __int16*)v5 - 1);
                LOBYTE(image->flags) |= 1u;
                image->coloringData = v8;
            }
            continue;
        case opc_orderdone:
            ++v5;
            if (!noop)
            {
                iscript_flingy->orderSignal &= ~*(v5 - 1);
            }
            continue;
        case opc_grdsprol:
            v36 = *(_WORD*)v5;
            v37 = v5 + 3;
            v136 = *v37;
            v38 = *(v37 - 1);
            v5 = v37 + 1;
            if (noop)
            {
                continue;
            }
            if (canUnitTypeFitAt(v38 + image->spriteOwner->position.x + image->horizontalOffset, Terran_Marine, v136 + image->spriteOwner->position.y + image->verticalOffset))
            {
                ISCRIPT_CreateSprite(image, v36, v38, v136, image->spriteOwner->elevationLevel + 1);
            }
            continue;
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
