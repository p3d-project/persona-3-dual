#pragma once
#include "Element.hpp"
#include "armours/Armour.hpp"
#include <nds.h>

/**
 * @brief Basic stats / affinities. Used by Personas or directly by Enemies.
 *
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
*/

struct BattleStats
{
    uint8_t st;
    uint8_t ma;
    uint8_t en;
    uint8_t ag;
    uint8_t lu;

    enum class Affinity : uint8_t
    {
        NEUTRAL,
        WEAK,
        RESIST,
        NULLIFY,
        ABSORB,
        REPEL
    };

    Affinity affinities[11] = {
        Affinity::NEUTRAL,
        Affinity::NEUTRAL,
        Affinity::NEUTRAL,
        Affinity::NEUTRAL,
        Affinity::NEUTRAL,
        Affinity::NEUTRAL,
        Affinity::NEUTRAL,
        Affinity::NEUTRAL,
        Affinity::NEUTRAL,
        Affinity::NEUTRAL,
        Affinity::NEUTRAL //Almighty
    };

    BattleStats* getBattleStats()
    {
        return this;
    };
};
