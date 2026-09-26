#pragma once
#include "../party/PartyMember.hpp"
#include "ActionBase.hpp"

struct Guard : ActionBase
{
    Guard()
    {
        name = "Guard";
        possibleUsers = ParticipantType::Party;
    }

    TurnResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill = nullptr) override;
};
