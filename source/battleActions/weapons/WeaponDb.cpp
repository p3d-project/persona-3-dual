#include "WeaponDb.hpp"
Weapon WeaponDb::shortSword;
Weapon WeaponDb::imitationKatana;
Weapon WeaponDb::practiceBow;

void WeaponDb::Initialize()
{
    shortSword.weaponPower = 38;
    shortSword.hitRate = 95;
    shortSword.weaponType = WeaponType::OneHandedSword;

    imitationKatana.weaponPower = 45;
    imitationKatana.hitRate = 92;
    imitationKatana.weaponType = WeaponType::TwoHandedSword;

    practiceBow.weaponPower = 30;
    practiceBow.hitRate = 98;
    practiceBow.weaponType = WeaponType::Bow;
}
