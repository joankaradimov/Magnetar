#include "CBullet.h"
#include "CFlingy.h"
#include "starcraft.h"
#include "patching.h"

void PlayIscriptAnim_(CImage* image, Anims new_animation);
void playSpriteIscript_(CSprite* sprite, Anims animation, int a3);

void ProgressBulletMovement_(CBullet* bullet)
{
    byte_63FEC2 = bullet->movementFlags;
    Unit_AssignNextWP((CUnit*)bullet);
    sub_495CB0((CFlingy*)bullet);
    ProgressMovementFlag((CUnit*)bullet);
    sub_4956C0((CUnit*)bullet);
    byte_63FEC0 = bullet->movementFlags;
    bullet->movementFlags = byte_63FEC2;
    sub_496030((CUnit*)bullet);
    if ((__int16)bullet->position.x < 0 || (__int16)bullet->position.x >= (int)(u16)map_width_pixels || (__int16)bullet->position.y < 0 || (__int16)bullet->position.y >= (int)(u16)map_height_pixels)
    {
        bullet->time_remaining = 0;
    }
    sub_4958C0_((CFlingy*) bullet);
}

FAIL_STUB_PATCH(ProgressBulletMovement, "starcraft");

void ProgressBulletState_(CBullet* bullet);

void BulletBehaviour_ReAssign_(CBullet* bullet)
{
    if (bullet->unknown_0x4F & 1)
    {
        bullet->unknown_0x4F &= 0xFE;
        switch (Weapon_Behavior[bullet->weaponType])
        {
        case WeaponBehavior::WB_AppearOnTargetUnit:
        case WeaponBehavior::WB_AppearOnTargetSite:
        case WeaponBehavior::WB_AppearOnAttacker:
        case WeaponBehavior::WB_AttackAndSelfDestruct:
            bullet->behaviourTypeInternal = ReachedDestination;
            if (CUnit* v7 = bullet->attackTarget.pUnit)
            {
                bullet->attackTarget.pt.x = v7->sprite->position.x;
                bullet->attackTarget.pt.y = v7->sprite->position.y;
            }
            else
            {
                bullet->attackTarget.pUnit = 0;
            }
            bullet->unknown_0x4E = 0;
            bullet->someUnitType = 228;
            playSpriteIscript_(bullet->sprite, Anims::AE_Death, 1);
            break;
        case WeaponBehavior::WB_Fly_FollowTarget:
            assignWpnData(bullet, MovingToUnit);
            playSpriteIscript_(bullet->sprite, Anims::AE_GndAttkInit, 1);
            break;
        case WeaponBehavior::WB_Bounce:
            assignWpnData(bullet, Bounce);
            playSpriteIscript_(bullet->sprite, Anims::AE_GndAttkInit, 1);
            break;
        case WeaponBehavior::WB_PersistOnTargetSite:
            assignWpnData(bullet, TargetGround);
            playSpriteIscript_(bullet->sprite, Anims::AE_SpecialState2, 1);
            break;
        case WeaponBehavior::WB_AttackNearbyArea:
            assignWpnData(bullet, MovingNearUnit);
            playSpriteIscript_(bullet->sprite, Anims::AE_GndAttkInit, 1);
            break;
        default:
            assignWpnData(bullet, MovingToPosition);
            playSpriteIscript_(bullet->sprite, Anims::AE_GndAttkInit, 1);
            break;
        }
        ProgressBulletState_(bullet);
    }
}

FAIL_STUB_PATCH(BulletBehaviour_ReAssign, "starcraft");

void BulletBehaviour_Fly_(CBullet* bullet)
{
    ProgressBulletMovement_(bullet);
    u8 v2 = bullet->time_remaining;
    bullet->time_remaining = v2 - 1;
    if (!v2 || bullet->moveTarget.pt.x == bullet->position.x && bullet->moveTarget.pt.y == bullet->position.y)
    {
        bullet->behaviourTypeInternal = BulletState::ReachedDestination;
        if (CUnit* target_unit = bullet->attackTarget.pUnit)
        {
            bullet->attackTarget.pt.x = target_unit->sprite->position.x;
            bullet->attackTarget.pt.y = target_unit->sprite->position.y;
        }
        else
        {
            bullet->attackTarget.pUnit = 0;
        }
        bullet->unknown_0x4E = 0;
        bullet->someUnitType = 228;
        playSpriteIscript_(bullet->sprite, Anims::AE_Death, 1);
    }
}

FAIL_STUB_PATCH(BulletBehaviour_Fly, "starcraft");

void BulletBehaviour_Bounce_(CBullet* bullet)
{
    CUnit* v1 = bullet->attackTarget.pUnit;
    if (v1 && (bullet->hitFlags & 1) == 0)
    {
        ChangeMovePos(bullet, (__int16)v1->sprite->position.y, (__int16)v1->sprite->position.x);
    }
    ProgressBulletMovement_(bullet);
    if (bullet->moveTarget.pt.x == bullet->position.x && bullet->moveTarget.pt.y == bullet->position.y)
    {
        CUnit* v3;
        if (--bullet->remainingBounces && (v3 = FindNextBounceTarget(bullet), bullet->nextBounceUnit = bullet->attackTarget.pUnit, v3))
        {
            playSpriteIscript_(bullet->sprite, Anims::AE_SpecialState1, 1);
            bullet->attackTarget.pUnit = v3;
        }
        else
        {
            bullet->behaviourTypeInternal = ReachedDestination;
            if (CUnit* v5 = bullet->attackTarget.pUnit)
            {
                bullet->attackTarget.pt.x = v5->sprite->position.x;
                bullet->attackTarget.pt.y = v5->sprite->position.y;
            }
            else
            {
                bullet->attackTarget.pUnit = 0;
            }
            bullet->unknown_0x4E = 0;
            bullet->someUnitType = 228;
            playSpriteIscript_(bullet->sprite, Anims::AE_Death, 1);
        }
    }
}

FAIL_STUB_PATCH(BulletBehaviour_Bounce, "starcraft");

int InitializeBullet_(CUnit* unit, __int16 a2, char player_id, CBullet* bullet, WeaponType weapon_type, int a6, int a7)
{
    if (sub_496360(Weapon_Graphic[weapon_type], a2, a6, (CFlingy*)bullet, player_id, a7))
    {
        u16 v15;

        bullet->srcPlayer = player_id;
        bullet->unknown_0x4E = 0;
        bullet->unknown_0x4F = 0;
        bullet->attackTarget.pUnit = 0;
        bullet->attackTarget.pt.y = 0;
        bullet->attackTarget.pt.x = 0;
        bullet->unknown_0x54 = 0;
        bullet->weaponType = weapon_type;
        bullet->time_remaining = Weapon_RemoveAfter[weapon_type];
        bullet->someUnitType = 228;
        bullet->remainingBounces = 0;
        bullet->hitFlags = 0;
        bullet->movementFlags |= 8;
        bullet->behaviourTypeInternal = Init;
        bullet->attackTarget.pUnit = 0;
        bullet->attackTarget.pt.y = 0;
        bullet->attackTarget.pt.x = 0;
        BulletBehaviour_ReAssign_(bullet);
        u8 v10 = Weapon_LaunchSpin[weapon_type];
        if (v10)
        {
            dword_64DEC0 = unit == dword_64DEB0 ? (dword_64DEC0 == 0) : (RandomizeShort(0) & 1);
            int v11 = dword_64DEC0 ? v10 : -v10;
            u8 v13 = v11 + bullet->direction2;
            bullet->direction2 = v13;
            bullet->direction1 = v13;
            dword_64DEB0 = unit;
        }
        bullet->sourceUnit = unit;
        if (Unit_PrototypeFlags[unit->unitType] & Subunit)
        {
            bullet->sourceUnit = unit->subUnit;
        }
        if (unit->unitType == Protoss_Scarab)
        {
            bullet->sourceUnit = unit->fields1.carrier.pInHanger;
        }
        if (unit->statusFlags & IsHallucination)
        {
            bullet->hitFlags |= 2;
        }
        bullet->nextBounceUnit = 0;
        CUnit* target_unit = unit->orderTarget.pUnit;
        if (target_unit)
        {
            bullet->sprite->elevationLevel = target_unit->sprite->elevationLevel + 1;
            bullet->attackTarget.pUnit = target_unit;
            bullet->attackTarget.pt.x = target_unit->sprite->position.x;
            bullet->attackTarget.pt.y = target_unit->sprite->position.y;
        }
        else
        {
            bullet->sprite->elevationLevel = unit->sprite->elevationLevel + 1;
            bullet->attackTarget.pUnit = 0;
            bullet->attackTarget.pt.x = unit->orderTarget.pt.x;
            bullet->attackTarget.pt.y = unit->orderTarget.pt.y;
        }
        switch (Weapon_Behavior[weapon_type])
        {
        case WeaponBehavior::WB_AppearOnTargetUnit:
        case WeaponBehavior::WB_AppearOnTargetSite:
            if (target_unit)
            {
                if (CUnit* source_unit = bullet->sourceUnit)
                {
                    if ((unsigned __int8)RandomizeShort(1) > weaponBehaviourCanHitTargetCheck(target_unit, source_unit))
                    {
                        return 1;
                    }
                    AppearOnTarget_(
                        (CFlingy*)bullet,
                        bullet->sprite->position.x - ((30 * AngleDistance[a7].x) >> 8),
                        bullet->sprite->position.y - ((30 * AngleDistance[a7].y) >> 8));
                    bullet->hitFlags |= 1;
                }
            }
            return 1;
        case WeaponBehavior::WB_PersistOnTargetSite:
            AppearOnTarget_((CFlingy*)bullet, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y);
            return 1;
        case WeaponBehavior::WB_AppearOnAttacker:
            return 1;
        case WeaponBehavior::WB_AttackAndSelfDestruct:
            if (CUnit* source_unit = bullet->sourceUnit)
            {
                source_unit->statusFlags |= StatusFlags::IsSelfDestructing;
                source_unit->userActionFlags |= 4;
                RemoveUnit(source_unit);
            }
            return 1;
        case WeaponBehavior::WB_Bounce:
            bullet->remainingBounces = 3;
            [[fallthrough]];
        case WeaponBehavior::WB_Fly_DoNotFollowTarget:
        case WeaponBehavior::WB_Fly_FollowTarget:
            if (target_unit)
            {
                if (CUnit* source_unit = bullet->sourceUnit)
                {
                    if ((unsigned __int8)RandomizeShort(1) <= weaponBehaviourCanHitTargetCheck(target_unit, source_unit))
                    {
                        bullet->attackTarget.pt.y -= (30 * AngleDistance[a7].y) >> 8;
                        bullet->attackTarget.pt.x -= (30 * AngleDistance[a7].x) >> 8;
                        bullet->hitFlags = bullet->hitFlags | 1;
                    }
                }
            }
            AttackFlyToTarget(bullet, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y);
            return 1;
        case WeaponBehavior::WB_AttackNearbyArea:
            v15 = unit->status.bulletBehaviour3by3AttackSequence;
            LOBYTE(bullet->cyclicMissileIndex) = v15;
            bullet->attackTarget.pt.x -= LOWORD(bullet_random_offsets[v15].x);
            bullet->attackTarget.pt.y -= LOWORD(bullet_random_offsets[v15].y);
            AttackFlyToTarget(bullet, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y);
            unit->status.bulletBehaviour3by3AttackSequence = bullet_random_offsets[v15 + 1].x ? v15 + 1 : 0;
            return 1;
        case WeaponBehavior::WB_GoToMaxRange:
            bullet->attackTarget.pt.x = unit->sprite->position.x + (unsigned)(Weapon_MaxRange[bullet->weaponType] + 20) * AngleDistance[bullet->direction1].x / 256;
            bullet->attackTarget.pt.y = unit->sprite->position.y + (unsigned)(Weapon_MaxRange[bullet->weaponType] + 20) * AngleDistance[bullet->direction1].y / 256;
            AttackFlyToTarget(bullet, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y);
            return 1;
        }
    }

    return 0;
}

FAIL_STUB_PATCH(InitializeBullet, "starcraft");

void CreateBullet_(CUnit* unit, WeaponType weapon_id, int x, __int16 y)
{
    CBullet* v6 = first_free_bullet;
    if (!first_free_bullet || weapon_id == Halo_Rockets && (unsigned int)bullet_count >= 0x50)
    {
        return;
    }
    if (unit->statusFlags & StatusFlags::CanNotAttack)
    {
        switch (weapon_id)
        {
        case WeaponType::Spider_Mines:
        case WeaponType::Lockdown:
        case WeaponType::EMP_Shockwave:
        case WeaponType::Irradiate:
        case 0x32:
        case 0x33:
        case WeaponType::Suicide_Infested_Terran:
        case WeaponType::Parasite:
        case WeaponType::Spawn_Broodlings:
        case WeaponType::Ensnare:
        case WeaponType::Dark_Swarm:
        case WeaponType::Plague:
        case WeaponType::Consume:
        case 0x44:
        case WeaponType::Psi_Assault:
        case WeaponType::Scarab:
        case WeaponType::Stasis_Field:
        case WeaponType::Psionic_Storm:
        case WeaponType::Restoration:
        case WeaponType::Mind_Control:
        case WeaponType::Feedback:
        case WeaponType::Optical_Flare:
        case WeaponType::Maelstrom:
            break;
        default:
            return;
        }
    }
    if (!InitializeBullet_(unit, y, unit->playerID, first_free_bullet, weapon_id, x, unit->currentDirection1))
    {
        return;
    }
    if (first_free_bullet == v6)
    {
        first_free_bullet = v6->next;
    }
    if (last_free_bullet == v6)
    {
        last_free_bullet = v6->prev;
    }
    if (v6->prev)
    {
        v6->prev->next = v6->next;
    }
    if (v6->next)
    {
        v6->next->prev = v6->prev;
    }
    v6->prev = 0;
    v6->next = 0;
    if (BulletNodeTable_FirstElement)
    {
        if (BulletNodeTable_LastElement == BulletNodeTable_FirstElement)
        {
            BulletNodeTable_LastElement = v6;
        }
        v6->prev = BulletNodeTable_FirstElement;
        v6->next = BulletNodeTable_FirstElement->next;
        if (BulletNodeTable_FirstElement->next)
        {
            BulletNodeTable_FirstElement->next->prev = v6;
        }
        BulletNodeTable_FirstElement->next = v6;
    }
    else
    {
        BulletNodeTable_LastElement = v6;
        BulletNodeTable_FirstElement = v6;
    }
    ++bullet_count;
}

FAIL_STUB_PATCH(CreateBullet, "starcraft");

void ProgressBulletState_(CBullet* bullet)
{
    switch (bullet->behaviourTypeInternal)
    {
    case BulletState::Init:
        BulletBehaviour_ReAssign_(bullet);
        break;
    case BulletState::MovingToPosition:
        BulletBehaviour_Fly_(bullet);
        break;
    case BulletState::MovingToUnit:
        BulletBehaviour_Follow(bullet);
        break;
    case BulletState::Bounce:
        BulletBehaviour_Bounce_(bullet);
        break;
    case BulletState::TargetGround:
        BulletBehaviour_Persist(bullet);
        break;
    case BulletState::ReachedDestination:
        BulletBehaviour_Instant(bullet);
        break;
    case BulletState::MovingNearUnit:
        BulletBehaviour_Area(bullet);
    default:
        return;
    }
}

FAIL_STUB_PATCH(ProgressBulletState, "starcraft");

void spriteToIscriptLoop_(CSprite* sprite);

void ImageDrawingBulletDrawing_()
{
    CBullet* next_bullet;

    for (CBullet* bullet = BulletNodeTable_FirstElement; bullet; bullet = next_bullet)
    {
        next_bullet = bullet->next;

        iscript_flingy = (CUnit*)bullet;
        iscript_bullet = bullet;
        sub_4878F0((CThingy*)bullet);

        if (bullet->sprite)
        {
            spriteToIscriptLoop_(bullet->sprite);
            if (bullet->sprite->pImageHead == nullptr)
            {
                bullet->sprite = 0;
            }
        }

        ProgressBulletState_(bullet);
    }

    iscript_flingy = nullptr;
    iscript_bullet = nullptr;
}

FAIL_STUB_PATCH(ImageDrawingBulletDrawing, "starcraft");
