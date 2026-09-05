#pragma once
#include "../BattleParticipant.hpp"
#include "../BattleStats.hpp"
#include "../enemies/Enemy.hpp"
#include "../party/Player.hpp"
#include "../shoes/Shoe.hpp"
#include "../skills/SkillDb.hpp"
#include "Skill.hpp"
#include <algorithm>
#include <cmath>
#include <nds.h>
#include <string>

/**
 * @brief various calculations used in Battle
 *
 * @details
 * These are battle calculations to decide things like damage, healing, hitrate etc.
 * These forumlas are directly from Reload, since those are the only one well documented.
 *
 * Private functions are helpers.
 *
 * For a complete breakdown, visit https://steamcommunity.com/sharedfiles/filedetails/?id=3230774091
 * Credits for this document goes to CTOBN on Steam.
 *
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
 */

struct BattleCalcs
{
    /**
     * @brief Calculates damage of an attack.
     *
     * @param attacker BattleParticipant using the skill.
     * @param defender BattleParticipant the skill is targetet at.
     * @param skill skill used.
     * @return HP difference.
     */
    static u32 attack(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill);

    /**
     * @brief Calculates if a skill hit the target.
     *
     * @param attacker BattleParticipant using the skill.
     * @param defender BattleParticipant the skill is targetet at.
     * @param skill The skill used.
     * @return If it hit the target.
     */
    static bool hit(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill);

    /**
     * @brief Calculates heal of a skill.
     *
     * @param user BattleParticipant using the skill.
     * @param skill The skill used.
     * @return HP difference.
     */
    static u32 healing(BattleParticipant& user, Skill& skill);

    /**
     * @brief Calculates damage of an All out attack.
     *
     * Needs to be called for each BattleParticipant that gets attacked seperatly.

     * @param attacker Protagonist, his stats are the main part of the calculation.
     * @param defender BattleParticipant the attack is targetet at.
     * @param participantCount amount of BattleParticipant's executing the attack.
     * @return HP difference.
     */
    static u32 allOutAttack(Player& attacker, BattleParticipant& defender, u32 participantCount);

    /**
     * @brief Gets correct stat for damage calculations based on skill type.
     *
     * @param attackerStats Stats of the attacker, to return magic or strength.
     * @param skill Needed to determine the type of the skill.
     * @return Magic or strength stat, depending on skill type
     */
    static u32 getAtk(BattleStats& attackerStats, Skill& skill);

    /**
     * @brief Gets a multiplier used for damage calcs based on an arbitrary hardcoded table.
     *
     * @param attackerLevel Level of the attacker.
     * @param defenderLevel Level of the attacked defenderLevel BattleParticipant.
     * @return Multiplier for damage.
     */
    static ae::q20_12_t getLevelDifference(u32 attackerLevel, u32 defenderLevel);

    /**
     * @brief Calculate damage multiplier based on resistance & skill affinity.
     *
     * @param battleStats Stats to check resistance to the skills affinity.
     * @param skill Skil to get affinity from.
     * @return Multiplier for damage.
     */
    static ae::q20_12_t getAffinityMtp(BattleStats& battleStats, Skill& skill);

  private:
    /**
     * @brief Calculate healing bonus based on an arbitrary hardcoded table.
     *
     * @param magic Value to calculate the boost from.
     * @return Boost
     */
    static uint8_t getMagicBoostHeal(uint8_t& magic);

    //Attack
    static const ae::q20_12_t levelMultipliers[24];

    //Heal
    static const uint8_t magicBoostTableHeal[20];
};
