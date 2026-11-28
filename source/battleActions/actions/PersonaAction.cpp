#include "PersonaAction.hpp"
#include "battleActions/skills/BattleCalcs.hpp"
#include "core/logger.hpp"
#include <stdlib.h>

TurnResult PersonaAction::resolve(PartyMember* user, BattleParticipant* target, Skill* skill)
{
    if (user == nullptr || skill == nullptr)
    {
        logMessage(LogSeverity::ERROR, "Called with null user or skill");
        return {false, 0, false, "Miss"};
    }

    logMessage(LogSeverity::DEBUG, "PersonaAction " + skill->name + " resolving for user: " + user->name);

    //TODO: same as attack actioon, get rid off in the futrue
    if (skill->skillType == SkillType::ATTACK || skill->skillType == SkillType::MULTI_ATTACK)
    {
        if (target == nullptr)
        {
            logMessage(LogSeverity::ERROR, "Called with null target");
            return {false, 0, false, "Miss"};
        }

        Enemy* enemy = static_cast<Enemy*>(target);

        bool hit = BattleCalcs::hit(*user, *target, *skill);

        if (!hit)
        {
            return {false, 0, false, "Miss"};
        }

        u32 damage = BattleCalcs::attack(*user, *target, *skill);
        bool oneMore = false;

        BattleStats::Affinity affinity = enemy->battleStats.affinities[(u32)skill->element];
        if (affinity == BattleStats::Affinity::WEAK && !enemy->knockedDown)
        {
            oneMore = true;
            enemy->knockedDown = true;
        }

        return {true, -(s32)damage, oneMore, skill->name};
    }
    else if (skill->skillType == SkillType::HEAL || skill->skillType == SkillType::MULTI_HEAL)
    {
        u32 healed = BattleCalcs::healing(*user, *skill);
        return {true, (s32)healed, false, skill->name};
    }

    return {false, 0, false, skill->name};
}
