#pragma once
#include <array>
#include <nds.h>
#include <string>

/**
 * @brief Result of a singular turn. Note that currently multi target skills do use a
 * seperate turn result for each target.
 *
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
*/

struct TurnResult
{
    bool hit = false;
    s32 hpDelta = 0;
    bool oneMore = false;
    std::string log;
};
