#pragma once
#include "../Element.hpp"
#include "SkillRace.hpp"
#include "SkillTarget.hpp"
#include "SkillType.hpp"
#include <nds.h>
#include <string>

struct Skill
{
    std::string name;
    s32 cost;
    Element element;
    u32 hitRate;
    u32 movePower;
    SkillType skillType;
    SkillRace skillRace;
    SkillTarget skillTarget;
};
