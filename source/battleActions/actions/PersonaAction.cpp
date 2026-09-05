#include "PersonaAction.hpp"
#include "battleActions/skills/BattleCalcs.hpp"
#include <stdlib.h>

TurnResult PersonaAction::resolve(PartyMember* user, BattleParticipant* target, Skill* skill)
{
    //TODO: same as attack actioon, get rid off in the futrue
    if (skill->skillType == SkillType::Attack || skill->skillType == SkillType::MultiAttack)
    {
        Enemy* enemy = static_cast<Enemy*>(target);

        bool hit = BattleCalcs::hit(*user, *target, *skill);

        if (!hit)
            return {false, 0, false, "Miss"};

        u32 damage = BattleCalcs::attack(*user, *target, *skill);
        bool oneMore = false;

        u32 affinity = enemy->battleStats.affinities[(u32)skill->element];
        if (affinity == BattleStats::Affinity::Weak && !enemy->knockedDown)
        {
            oneMore = true;
            enemy->knockedDown = true;
        }

        return {true, -(s32)damage, oneMore, skill->name};
    }
    else if (skill->skillType == SkillType::Heal || skill->skillType == SkillType::MultiHeal)
    {
        u32 healed = BattleCalcs::healing(*user, *skill);
        return {true, (s32)healed, false, skill->name};
    }

    return {false, 0, false, skill->name};
}
