#include "Guard.hpp"
#include "core/error.hpp"

TurnResult Guard::resolve(PartyMember* user, BattleParticipant* /*target*/, Skill* /*skill*/)
{
    if (user == nullptr)
    {
        logMessage(LogSeverity::ERROR, "Called with null user");
        return {false, 0, false, "Guard"};
    }

    logMessage(LogSeverity::DEBUG, "Guard action resolving for: " + user->name);

    user->guarding = true;
    return {true, 0, false, "Guard"};
}
