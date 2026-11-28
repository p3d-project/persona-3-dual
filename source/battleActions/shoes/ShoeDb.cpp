#include "ShoeDb.hpp"

Shoe ShoeDb::rubberSole;
Shoe ShoeDb::enemyShoe;

void ShoeDb::initialize()
{
    rubberSole.evasion = 19;
    rubberSole.armourType = ArmourType::UNISEX;

    enemyShoe.evasion = 10;
    enemyShoe.armourType = ArmourType::ENEMY;
}
