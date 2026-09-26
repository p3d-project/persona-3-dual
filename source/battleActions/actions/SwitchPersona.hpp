#pragma once
#include "ActionBase.hpp"

struct SwitchPersona : ActionBase
{
    SwitchPersona()
    {
        name = "SwitchPersona";
        possibleUsers = ParticipantType::Player;
    }

    TurnResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill = nullptr) override;
};
