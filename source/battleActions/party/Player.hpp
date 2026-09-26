#pragma once
#include "PartyMember.hpp"

struct Player : PartyMember
{
    using PartyMember::PartyMember;

    bool actorCanUse(ActionBase* action) override;
    void onDead(Event::BattleResult& battleResult) override;
};
