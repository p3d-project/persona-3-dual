#pragma once
#include "../BattleParticipant.hpp"
#include "../TurnResult.hpp"
#include "EnemyProfile.hpp"
#include <etl/vector.h>
#include <nds.h>

struct Enemy : BattleParticipant
{
    Skill** skill;
    u32 skillCount;
    BattleStats battleStats;

    EnemyProfile enemyProfile;

    Enemy(const EnemyProfile& iEnemyProfile);

    Skill* pickSkill();
    BattleParticipant* pickTarget(etl::vector<BattleParticipant*, 13>& partyMembers);
    TurnResult resolve(BattleParticipant* target, Skill* skill);

    BattleStats* getBattleStats() override
    {
        return &battleStats;
    }

    ae::q20_12_t calculateBaseDamage(BattleParticipant& defender, Skill& skill) override;
    ae::q20_12_t getTeamMultiplier() override;
    BattlePhase getInitalTurnPhase() override;
    void onDead(Event::BattleResult& battleResult) override;
    void setCurrentTurnOrderAgility(ae::q20_12_t boost);

    virtual ~Enemy() = default;

  private:
    MathManager& math = MathManager::GetInstance();
};
