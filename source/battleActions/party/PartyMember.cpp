#include "PartyMember.hpp"
#include "../skills/BattleCalcs.hpp"
#include <fpm/math.hpp>

PartyMember::PartyMember(const CharacterProfile& iCharacterProfile) : characterProfile(iCharacterProfile)

{
    name = characterProfile.name;
    maxHp = characterProfile.maxHp;
    hp = characterProfile.hp;
    maxSp = characterProfile.maxSp;
    sp = characterProfile.sp;
    lv = characterProfile.lv;

    baseAttackAction = characterProfile.baseAttackAction;
    participantType = characterProfile.participantType;

    armourType = characterProfile.armourType;
    armour = characterProfile.armour;
    shoe = characterProfile.shoe;
    weaponType = characterProfile.weaponType;
    weapon = characterProfile.weapon;
    personas = characterProfile.personas;
    curPersona = characterProfile.curPersona;
}

ae::q20_12_t PartyMember::calculateBaseDamage(BattleParticipant& defender, Skill& skill)
{
    u32 atk = BattleCalcs::getAtk(curPersona->battleStats, skill);
    ae::q20_12_t levelDifference = BattleCalcs::getLevelDifference(lv, defender.lv);
    ae::q20_12_t affinityMtp = BattleCalcs::getAffinityMtp(*defender.getBattleStats(), skill);
    u32 movePower = (skill.skillType == SkillType::RegularAttack) ? (weapon.weaponPower / 2) : skill.movePower;

    return fpm::floor(
        math.sqrt(math.div(ae::q20_12_t{movePower * 15 * atk}, ae::q20_12_t{defender.getBattleStats()->en})) *
        ae::q20_12_t{2} * levelDifference * affinityMtp);
}

ae::q20_12_t PartyMember::getTeamMultiplier()
{
    return ae::q20_12_t{1.0};
}

void PartyMember::setCurrentTurnOrderAgility(ae::q20_12_t boost)
{
    currentTurnOrderAgility = ae::q20_12_t{curPersona->battleStats.ag} * boost;
}

BattlePhase PartyMember::getInitalTurnPhase()
{
    return BattlePhase::ChooseAction;
}

void PartyMember::onDead(Event::BattleResult& battleResult)
{
    //TODO: clear ailments, buffs etc in the future as they get wiped when revived
}

bool PartyMember::canParticipateInAllOutAttack()
{
    if (hp <= 0 || knockedDown)
    {
        return false;
    }

    return true;
}

bool PartyMember::actorCanUse(ActionBase* action)
{
    return action->possibleUsers == ParticipantType::Party;
}
