#include "WeaponDb.hpp"
Weapon WeaponDb::shortSword;
Weapon WeaponDb::imitationKatana;
Weapon WeaponDb::practiceBow;

void WeaponDb::initialize()
{
    shortSword.weaponPower = 38;
    shortSword.hitRate = 95;
    shortSword.weaponType = WeaponType::ONE_HANDED_SWORD;

    imitationKatana.weaponPower = 45;
    imitationKatana.hitRate = 92;
    imitationKatana.weaponType = WeaponType::TWO_HANDED_SWORD;

    practiceBow.weaponPower = 30;
    practiceBow.hitRate = 98;
    practiceBow.weaponType = WeaponType::BOW;
}
