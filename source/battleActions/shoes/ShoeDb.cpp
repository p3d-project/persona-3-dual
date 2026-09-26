#include "ShoeDb.hpp"

Shoe ShoeDb::rubberSole;
Shoe ShoeDb::enemyShoe;

void ShoeDb::Initialize()
{
    rubberSole.evasion = 19;
    rubberSole.armourType = ArmourType::Unisex;

    enemyShoe.evasion = 10;
    enemyShoe.armourType = ArmourType::Enemy;
}
