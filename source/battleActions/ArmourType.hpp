#pragma once

enum class ArmourType
{
    MALE = 1,
    FEMALE = 2,
    UNISEX = MALE | FEMALE,
    ROBOT = 4,
    DOG = 8,
    ENEMY = 16
};
