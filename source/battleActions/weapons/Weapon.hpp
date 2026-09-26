#pragma once
#include "../WeaponType.hpp"
#include <nds.h>

// TODO: we need some kind of selection and matching weaponType validation outside of battle
// in the future
struct Weapon
{
    u32 weaponPower;
    u32 hitRate;
    WeaponType weaponType;
    // TODO: add stat boosts / other shenanigans later
};
