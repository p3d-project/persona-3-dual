/**
 * @file ProfileBase.h
 * @brief Holds generic data for profiles which are used to create BattleParticipants from
 * @author Nolan Kolb (themoonwalker8692 / TrueGiles)
 */
#pragma once
#include "armours/Armour.hpp"
#include "shoes/Shoe.hpp"
#include "skills/Skill.hpp"
#include <nds.h>
#include <string>

struct ProfileBase
{
    std::string name;
    s32 maxHp;
    s32 hp;
    s32 maxSp;
    s32 sp;
    u32 lv;

    Skill* baseAttackAction;

    Armour armour;
    Shoe shoe;
};
