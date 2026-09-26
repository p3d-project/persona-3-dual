#pragma once
#include "../enemies/Enemy.hpp"
#include "../party/PartyMember.hpp"
#include "ActionBase.hpp"

struct PersonaAction : ActionBase
{
    PersonaAction()
    {
        name = "Persona";
        possibleUsers = ParticipantType::Party;
    }

    TurnResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill) override;
};
