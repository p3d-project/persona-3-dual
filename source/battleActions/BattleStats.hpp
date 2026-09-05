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

    enum Affinity
    {
        Neutral,
        Weak,
        Resist,
        Null,
        Absorb,
        Repel
    };

    Affinity affinities[10] = {
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral // almighty
    };

    BattleStats* getBattleStats()
    {
        return this;
    };
};
