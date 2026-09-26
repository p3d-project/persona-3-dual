#pragma once
#include "../ArmourType.hpp"
#include <nds.h>

// TODO: we need some kind of selection and matching armourType validation outside of battle
// in the future
struct Armour
{
    u32 defense = 10;
    ArmourType armourType = ArmourType::Unisex;
    // TODO: add stat boosts / other shenanigans later
};
