/**
 * @file BattleEvents.hpp
 * @brief Events for BattleSystem
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once

#include "types/aeTypes.hpp"

#include "battleActions/BattleStartCondition.hpp"
#include "battleActions/enemies/EnemyProfileDb.hpp"
#include "battleActions/party/CharacterProfileDb.hpp"

#include <aegis/system.hpp>
#include <etl/vector.h>

namespace Event
{
/**
 * @brief Event payload to start the BattleSystem.
 *
 * @details Called before every new battle. Takes in profiles of BattleParticipants
 * so the battle can create and manage participants itself.
 *
 * Proceeding this event, the system sets music, initializes variables, and performs
 * various cleanup. Finally, turn order is calculated with the battleStartCondition
 * and the battle gets started.
 */
struct ExecuteBattle : public etl::message<EventID::ExecuteBattle>
{
    /// The main player character. We need to specifically know them for some things all the time.
    CharacterProfile& player;

    /// All party members currently on the field.
    etl::vector<CharacterProfile, 4>& characterProfiles;

    /// All enemies currently on the field.
    etl::vector<EnemyProfile, 8>& enemyProfiles;

    /// Condition like player advantage, enemy advantage, or even. Used to decide turn order.
    BattleStartCondition battleStartCondition;

    /**
     * @brief Constructs the ExecuteBattle event.
     *
     * @param iPlayer The main player profile.
     * @param iCharacterProfiles List of all player profiles in the party.
     * @param iEnemyProfiles List of all enemy profiles in the encounter.
     * @param iBattleStartCondition The advantage state for the encounter.
     */
    ExecuteBattle(CharacterProfile& iPlayer,
                  etl::vector<CharacterProfile, 4>& iCharacterProfiles,
                  etl::vector<EnemyProfile, 8>& iEnemyProfiles,
                  BattleStartCondition iBattleStartCondition)
        : player(iPlayer), characterProfiles(iCharacterProfiles), enemyProfiles(iEnemyProfiles),
          battleStartCondition(iBattleStartCondition)
    {
    }
};

/**
 * @brief Holds data on how the battle concluded.
 *
 * @details This event can be used in future for things like game over
 * screens, shuffle time, etc.
*/
struct BattleResult : public etl::message<EventID::BattleResult>
{
    /// A flag to check if the player died in battle
    bool playerDied = false;
};
} // namespace Event
