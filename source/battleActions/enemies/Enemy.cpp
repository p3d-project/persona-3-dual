#include "Enemy.hpp"
#include "../party/PartyMember.hpp"
#include "../skills/BattleCalcs.hpp"
#include <stdlib.h>

Enemy::Enemy(const EnemyProfile& iEnemyProfile) : enemyProfile(iEnemyProfile)
{
    participantType = ParticipantType::Enemy;

    name = enemyProfile.name;
    maxHp = enemyProfile.maxHp;
    hp = enemyProfile.hp;
    maxSp = enemyProfile.maxSp;
    sp = enemyProfile.sp;
    lv = enemyProfile.lv;

    battleStats = enemyProfile.battleStats;

    baseAttackAction = enemyProfile.baseAttackAction;
    skill = enemyProfile.skill;
    skillCount = enemyProfile.skillCount;

    armour = enemyProfile.armour;
    shoe = enemyProfile.shoe;
}

Skill* Enemy::pickSkill()
{
    u32 roll = rand() % (skillCount + 1);
    return (roll == 0) ? baseAttackAction : skill[roll - 1];
}

BattleParticipant* Enemy::pickTarget(etl::vector<BattleParticipant*, 13>& partyMembers)
{
    BattleParticipant* target = nullptr;
    do
    {
        target = partyMembers[rand() % partyMembers.size()];
    } while (target->hp <= 0);

    return target;
}

TurnResult Enemy::resolve(BattleParticipant* target, Skill* skill)
{
    PartyMember* party = static_cast<PartyMember*>(target);

    s32* resource;
    if (skill->skillRace == SkillRace::mag)
        resource = &sp;
    else
        resource = &hp;

    bool canAfford = *resource >= skill->cost;
    if (!canAfford)
        return {false, 0, false, skill->name};

    std::string targetLog = name + " targets " + target->name + "\n";

    *resource -= skill->cost;
    bool hit = BattleCalcs::hit(*this, *target, *skill);

    if (!hit)
        return {false, 0, false, targetLog + "Miss"};

    u32 damage = BattleCalcs::attack(*this, *target, *skill);

    if (party->guarding)
        damage = (u32)(damage * 0.4f);

    bool oneMoreResult = false;
    u32 affinity = party->curPersona->battleStats.affinities[(u32)skill->element];
    if (affinity == BattleStats::Affinity::Weak && !party->knockedDown && !party->guarding)
    {
        oneMoreResult = true;
        party->knockedDown = true;
    }

    return {true, -(s32)damage, oneMoreResult, targetLog + skill->name};
}

ae::q20_12_t Enemy::calculateBaseDamage(BattleParticipant& defender, Skill& skill)
{
    u32 atk = BattleCalcs::getAtk(battleStats, skill);
    ae::q20_12_t levelDifference = BattleCalcs::getLevelDifference(lv, defender.lv);
    ae::q20_12_t affinityMtp = BattleCalcs::getAffinityMtp(*defender.getBattleStats(), skill);

    if (skill.skillType == SkillType::RegularAttack)
        return (math.sqrt(math.div(ae::q20_12_t{skill.movePower * 6 * atk},
                                   ae::q20_12_t{8 * defender.getBattleStats()->en + defender.armour.defense})) *
                ae::q20_12_t{9} * levelDifference) *
               affinityMtp;
    else if (skill.skillType == SkillType::Attack || skill.skillType == SkillType::MultiAttack)
        return ((math.sqrt(math.div(ae::q20_12_t{skill.movePower * 6 * atk},
                                    ae::q20_12_t{8 * defender.getBattleStats()->en + defender.armour.defense})) *
                     ae::q20_12_t{9} * levelDifference -
                 ae::q20_12_t{10}) *
                affinityMtp);
    else
        return ae::q20_12_t{0};
}

ae::q20_12_t Enemy::getTeamMultiplier()
{
    return ae::q20_12_t{0.6};
}

BattlePhase Enemy::getInitalTurnPhase()
{
    return BattlePhase::EnemyTurn;
}

void Enemy::onDead(Event::BattleResult& battleResult)
{
}

void Enemy::setCurrentTurnOrderAgility(ae::q20_12_t boost)
{
    currentTurnOrderAgility = ae::q20_12_t{battleStats.ag};
}
