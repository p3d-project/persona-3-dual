#pragma once
#include "../ParticipantType.hpp"
#include "../TurnResult.hpp"
#include <nds.h>
#include <string>

// Forward declarations
struct BattleParticipant;
struct PartyMember;
struct Skill;

struct ActionBase
{
    std::string name;
    ParticipantType possibleUsers;

    virtual TurnResult resolve(PartyMember* user, BattleParticipant* target, Skill* skill = nullptr) = 0;

    virtual ~ActionBase() = default;
};
