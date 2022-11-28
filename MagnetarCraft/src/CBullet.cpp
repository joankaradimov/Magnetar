#include "CBullet.h"
#include "starcraft.h"
#include "patching.h"

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
    if (!InitializeBullet(a1, y, a5, first_free_bullet, weapon_id, x, a6))
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
