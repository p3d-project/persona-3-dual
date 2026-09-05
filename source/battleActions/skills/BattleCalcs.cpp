#include "BattleCalcs.hpp"
#include <aegis/ndsTypes.hpp>
#include <fpm/math.hpp>

u32 BattleCalcs::attack(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill)
{
    ae::q20_12_t base = attacker.calculateBaseDamage(defender, skill);
    ae::q20_12_t range = ae::q20_12_t{95} + ae::q20_12_t{(rand() % 11)};
    return std::clamp(
        static_cast<u32>(fpm::trunc(MathManager::GetInstance().div(base * ae::q20_12_t{range}, ae::q20_12_t{100}))),
        (u32)1,
        (u32)99999);
}

bool BattleCalcs::hit(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill)
{
    MathManager& math = MathManager::GetInstance();

    BattleStats& attackerStats = *attacker.getBattleStats();
    BattleStats& defenderStats = *defender.getBattleStats();

    if (skill.hitRate == 100)
        return true;

    ae::q20_12_t baseAccuracy = math.div(ae::q20_12_t{attackerStats.ag + 200}, ae::q20_12_t{defenderStats.ag + 200});
    ae::q20_12_t multipliedAccuracy;
    if (attacker.participantType == ParticipantType::Enemy)
    {
        ae::q20_12_t shoeMultiplier =
            math.div(ae::q20_12_t{attackerStats.ag + 200},
                     math.div(ae::q20_12_t{defender.shoe.evasion}, ae::q20_12_t{2}) + ae::q20_12_t{200});
        multipliedAccuracy = baseAccuracy * ae::q20_12_t{skill.hitRate} * shoeMultiplier;
    }
    else
    {
        multipliedAccuracy = baseAccuracy * ae::q20_12_t{skill.hitRate};
    }

    multipliedAccuracy = std::clamp(multipliedAccuracy, ae::q20_12_t{50}, ae::q20_12_t{99});
    return multipliedAccuracy > ae::q20_12_t{rand() % 100};
}

u32 BattleCalcs::healing(BattleParticipant& user, Skill& skill)
{
    BattleStats* battleStats = user.getBattleStats();
    ae::q20_12_t teamMultiplier = user.getTeamMultiplier();

    uint8_t magicBoost = BattleCalcs::getMagicBoostHeal(battleStats->ma);
    ae::q20_12_t base = fpm::floor(ae::q20_12_t{skill.movePower + magicBoost} * teamMultiplier);
    ae::q20_12_t range = ae::q20_12_t{95} + ae::q20_12_t{(rand() % 11)};

    return static_cast<u32>(fpm::floor(MathManager::GetInstance().div(base * range, ae::q20_12_t{100})));
}

u32 BattleCalcs::allOutAttack(Player& attacker, BattleParticipant& defender, u32 participantCount)
{
    MathManager& math = MathManager::GetInstance();

    ae::q20_12_t levelDifference = BattleCalcs::getLevelDifference(attacker.lv, defender.lv);
    BattleStats& attackerStats = *attacker.getBattleStats();
    BattleStats& defenderStats = *defender.getBattleStats();

    ae::q20_12_t affinityMtp = BattleCalcs::getAffinityMtp(*defender.getBattleStats(), SkillDb::allOutAttack);

    ae::q20_12_t base = fpm::trunc(
        math.sqrt(math.div(ae::q20_12_t{(attacker.weapon.weaponPower / 2) * 15 * attackerStats.st},
                           ae::q20_12_t{defenderStats.en})) *
        ae::q20_12_t{1.6} * (levelDifference * levelDifference) * affinityMtp * ae::q20_12_t{participantCount});

    ae::q20_12_t range = ae::q20_12_t{95} + ae::q20_12_t{(rand() % 11)};
    return static_cast<u32>(fpm::trunc(math.div(base * range, ae::q20_12_t{100})));
}

const ae::q20_12_t BattleCalcs::levelMultipliers[24] = {
    ae::q20_12_t{0.5},  ae::q20_12_t{0.51}, ae::q20_12_t{0.53}, ae::q20_12_t{0.59}, ae::q20_12_t{0.66},
    ae::q20_12_t{0.75}, ae::q20_12_t{0.84}, ae::q20_12_t{0.91}, ae::q20_12_t{0.97}, ae::q20_12_t{0.99},
    ae::q20_12_t{1.0},  ae::q20_12_t{1.0},  ae::q20_12_t{1.0},  ae::q20_12_t{1.0},  ae::q20_12_t{1.01},
    ae::q20_12_t{1.03}, ae::q20_12_t{1.09}, ae::q20_12_t{1.16}, ae::q20_12_t{1.25}, ae::q20_12_t{1.34},
    ae::q20_12_t{1.41}, ae::q20_12_t{1.47}, ae::q20_12_t{1.49}, ae::q20_12_t{1.5}};

const uint8_t BattleCalcs::magicBoostTableHeal[20] = {
    0,   // 1-5
    6,   // 6-10
    12,  // 11-15
    17,  // 16-20
    22,  // 21-25
    27,  // 26-30
    34,  // 31-35
    44,  // 36-40
    54,  // 41-45
    65,  // 46-50
    75,  // 51-55
    85,  // 56-60
    93,  // 61-65
    100, // 66-70
    105, // 71-75
    110, // 76-80
    115, // 81-85
    120, // 86-90
    125, // 91-95
    130  // 96-99
};

u32 BattleCalcs::getAtk(BattleStats& attackerStats, Skill& skill)
{
    return skill.skillRace == SkillRace::phys ? attackerStats.st : attackerStats.ma;
}

ae::q20_12_t BattleCalcs::getLevelDifference(u32 attackerLevel, u32 defenderLevel)
{
    s32 diff = attackerLevel - defenderLevel;
    diff = std::clamp(diff, (s32)-13, (s32)10);

    // offset so -13 is 0
    return levelMultipliers[diff + 13];
}

ae::q20_12_t BattleCalcs::getAffinityMtp(BattleStats& defenderStats, Skill& skill)
{
    u32 affinity = defenderStats.affinities[(u32)skill.element];

    switch (affinity)
    {
    case BattleStats::Affinity::Weak:
        return ae::q20_12_t{1.25};
    case BattleStats::Affinity::Resist:
        return ae::q20_12_t{0.5};
    case BattleStats::Affinity::Null:
        return ae::q20_12_t{0.0};
    case BattleStats::Affinity::Absorb:
        return ae::q20_12_t{-1.0};
    case BattleStats::Affinity::Repel:
        return ae::q20_12_t{-2.0}; // TODO: add repel logic
    case BattleStats::Affinity::Neutral:
    default:
        return ae::q20_12_t{1.0};
    }
}

uint8_t BattleCalcs::getMagicBoostHeal(uint8_t& magic)
{
    uint8_t index = (magic - 1) / 5;
    index = std::clamp(index, (uint8_t)0, (uint8_t)19);
    return magicBoostTableHeal[index];
}
