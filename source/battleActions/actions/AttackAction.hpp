#pragma once
#include "../enemies/Enemy.hpp"
#include "../party/PartyMember.hpp"
#include "ActionBase.hpp"

struct AttackAction : ActionBase
{
    AttackAction()
    {
        name = "Attack";
        possibleUsers = ParticipantType::Party;
    }

    TurnResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill = nullptr) override;
};
