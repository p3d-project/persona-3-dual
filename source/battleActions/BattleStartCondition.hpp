#pragma once

/**
 * @brief Describes how the Battle was started eg. PartyAdvantage if Player surprised the enemy
 *
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
*/

enum class BattleStartCondition
{
    EVEN,
    PARTY_ADVANTAGE,
    ENEMY_ADVANTAGE
};
