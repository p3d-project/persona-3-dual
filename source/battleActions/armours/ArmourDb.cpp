#include "ArmourDb.hpp"

Armour ArmourDb::plainShirt;
Armour ArmourDb::rashGuard;
Armour ArmourDb::enemyArmour;

void ArmourDb::Initialize()
{
    plainShirt.defense = 24;
    plainShirt.armourType = ArmourType::Male;

    rashGuard.defense = 46;
    rashGuard.armourType = ArmourType::Unisex;

    enemyArmour.defense = 10;
    enemyArmour.armourType = ArmourType::Enemy;
}
