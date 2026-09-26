#pragma once
#include "Weapon.hpp"

struct WeaponDb
{
    static Weapon shortSword;
    static Weapon imitationKatana;
    static Weapon practiceBow;

    static void Initialize();
};
