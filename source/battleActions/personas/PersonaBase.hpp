#pragma once
#include "../BattleStats.hpp"
#include "../skills/Skill.hpp"
#include <nds.h>
#include <string>

/*
St	Represents strength and physical damage.
Ma	Represents magic and magical damage.
En	Represents endurance, which determines your defense and how much damage you can take.
Ag	Represents agility, which determines your place in the turn order.
Lu	Represents luck, which is taken into account when using certain skills involving status afflictions or insta-death abilities.*/

struct PersonaBase
{
    std::string name;
    u32 lv;
    Skill** skills;
    u32 skillCount;
    BattleStats battleStats;

    PersonaBase()
    {
    }
    ~PersonaBase() {};
};
