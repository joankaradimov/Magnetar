#include "CBullet.h"
#include "starcraft.h"
#include "patching.h"

int InitializeBullet_(CUnit* unit, __int16 a2, char player_id, CBullet* bullet, WeaponType weapon_type, int a6, int a7)
{
    if (sub_496360(Weapon_Graphic[weapon_type], a2, a6, (CFlingy*)bullet, player_id, a7))
    {
        u16 v15;
        int v16;

        bullet->srcPlayer = player_id;
        HIBYTE(bullet->unknown_0x4E) = 0;
        LOBYTE(bullet->unknown_0x4E) = 0;
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
        BulletBehaviour_ReAssign(bullet);
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
        case WeaponBehavior::AppearOnTargetUnit:
        case WeaponBehavior::AppearOnTargetSite:
            if (target_unit)
            {
                if (CUnit* source_unit = bullet->sourceUnit)
                {
                    if ((unsigned __int8)RandomizeShort(1) > weaponBehaviourCanHitTargetCheck(target_unit, source_unit))
                    {
                        return 1;
                    }
                    AppearOnTarget(
                        bullet->sprite->position.y - ((30 * AngleDistance[a7].y) >> 8),
                        bullet->sprite->position.x - ((30 * AngleDistance[a7].x) >> 8),
                        (CFlingy*)bullet);
                    bullet->hitFlags |= 1;
                }
            }
            return 1;
        case WeaponBehavior::PersistOnTargetSite:
            AppearOnTarget(bullet->attackTarget.pt.y, bullet->attackTarget.pt.x, (CFlingy*)bullet);
            return 1;
        case WeaponBehavior::AppearOnAttacker:
            return 1;
        case WeaponBehavior::AttackAndSelfDestruct:
            if (CUnit* source_unit = bullet->sourceUnit)
            {
                source_unit->statusFlags |= StatusFlags::IsSelfDestructing;
                source_unit->userActionFlags |= 4;
                RemoveUnit(source_unit);
            }
            return 1;
        case WeaponBehavior::AttackAndSelfDestruct | Fly_FollowTarget:
            bullet->remainingBounces = 3;
            [[fallthrough]];
        case WeaponBehavior::Fly_DoNotFollowTarget:
        case WeaponBehavior::Fly_FollowTarget:
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
        case WeaponBehavior::AttackNearbyArea:
            v15 = unit->status.bulletBehaviour3by3AttackSequence;
            v16 = (unsigned __int8)v15;
            LOBYTE(bullet->cyclicMissileIndex) = v15;
            bullet->attackTarget.pt.x -= LOWORD(bullet_random_offsets[v16].x);
            bullet->attackTarget.pt.y -= LOWORD(bullet_random_offsets[v16].y);
            AttackFlyToTarget(bullet, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y);
            unit->status.bulletBehaviour3by3AttackSequence = bullet_random_offsets[v15 + 1].x ? v15 + 1 : 0;
            return 1;
        case WeaponBehavior::GoToMaxRange:
            bullet->attackTarget.pt.x = unit->sprite->position.x + ((unsigned int)((Weapon_MaxRange[bullet->weaponType] + 20) * 256 * AngleDistance[bullet->direction1].x) >> 16);
            bullet->attackTarget.pt.y = unit->sprite->position.y + ((unsigned int)((Weapon_MaxRange[bullet->weaponType] + 20) * 256 * AngleDistance[bullet->direction1].y) >> 16);
            AttackFlyToTarget(bullet, bullet->attackTarget.pt.x, bullet->attackTarget.pt.y);
            return 1;
        }
    }

    return 0;
}

FAIL_STUB_PATCH(InitializeBullet);

void CreateBullet_(CUnit* a1, WeaponType weapon_id, int x, __int16 y, char a5, int a6)
{
    CBullet* v6 = first_free_bullet;
    if (!first_free_bullet || weapon_id == Halo_Rockets && (unsigned int)bullet_count >= 0x50)
    {
        return;
    }
    if (a1->statusFlags & StatusFlags::CanNotAttack)
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
    if (!InitializeBullet_(a1, y, a5, first_free_bullet, weapon_id, x, a6))
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

FAIL_STUB_PATCH(CreateBullet);

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

        switch (bullet->behaviourTypeInternal)
        {
        case Init:
            BulletBehaviour_ReAssign(bullet);
            break;
        case MovingToPosition:
            BulletBehaviour_Fly(bullet);
            break;
        case MovingToUnit:
            BulletBehaviour_Follow(bullet);
            break;
        case Bounce:
            BulletBehaviour_Bounce(bullet);
            break;
        case TargetGround:
            BulletBehaviour_Persist(bullet);
            break;
        case ReachedDestination:
            BulletBehaviour_Instant(bullet);
            break;
        case MovingNearUnit:
            BulletBehaviour_Area(bullet);
            break;
        }
    }

    iscript_flingy = nullptr;
    iscript_bullet = nullptr;
}

FAIL_STUB_PATCH(ImageDrawingBulletDrawing);
