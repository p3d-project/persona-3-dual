/**
 * @file EnemyProfile.h
 * @brief Holds enemy data which an enemy is created from
 * @author Nolan Kolb (themoonwalker8692 / TrueGiles)
 */

#pragma once
#include "../BattleStats.hpp"
#include "../ProfileBase.hpp"
#include "../skills/Skill.hpp"
#include <nds.h>
#include <string>

struct EnemyProfile : ProfileBase
{
    BattleStats battleStats;
    Skill** skill;
    u32 skillCount;
};
