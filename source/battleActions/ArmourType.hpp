#pragma once

enum class ArmourType
{
    Male = 1,
    Female = 2,
    Unisex = Male | Female,
    Robot = 4,
    Dog = 8,
    Enemy = 16
};
