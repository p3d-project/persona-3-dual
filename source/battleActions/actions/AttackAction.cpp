#include "AttackAction.hpp"
#include "battleActions/skills/BattleCalcs.hpp"
#include "core/error.hpp"
#include <stdlib.h>

//ignore skill here
TurnResult AttackAction::resolve(PartyMember* user, BattleParticipant* target, Skill* skill)
{
    if (user == nullptr || target == nullptr || user->baseAttackAction == nullptr)
    {
        logMessage(LogSeverity::ERROR, "Called with null user, target, or attack action");
        return {false, 0, false, "Miss"};
    }

    logMessage(LogSeverity::DEBUG, "Attack action resolving: " + user->name + " attacking " + target->name);

    Skill* atk = user->baseAttackAction;
    Enemy* enemy = static_cast<Enemy*>(target);

    bool hit = BattleCalcs::hit(*user, *target, *atk);

    if (!hit)
    {
        return {false, 0, false, "Miss"};
    }

    u32 damage = BattleCalcs::attack(*user, *target, *atk);
    bool oneMore = false;

    u32 affinity = enemy->battleStats.affinities[(u32)atk->element];
    if (affinity == BattleStats::Affinity::Weak && !enemy->knockedDown)
    {
        oneMore = true;
        enemy->knockedDown = true;
    }

    return {true, -(s32)damage, oneMore, atk->name};
}
