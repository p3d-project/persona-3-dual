#pragma once
#include "../skills/Skill.hpp"

struct SkillDb
{
    static Skill agi;
    static Skill bash;
    static Skill bufu;
    static Skill dia;
    static Skill garu;
    static Skill magaru;
    static Skill pierceAttack;
    static Skill powerSlash;
    static Skill slashAttack;
    static Skill strikeAttack;
    static Skill allOutAttack;

    static void Initialize();
};
