#include "ArmourDb.hpp"

Armour ArmourDb::plainShirt;
Armour ArmourDb::rashGuard;
Armour ArmourDb::enemyArmour;

void ArmourDb::initialize()
{
    plainShirt.defense = 24;
    plainShirt.armourType = ArmourType::MALE;

    rashGuard.defense = 46;
    rashGuard.armourType = ArmourType::UNISEX;

    enemyArmour.defense = 10;
    enemyArmour.armourType = ArmourType::ENEMY;
}
