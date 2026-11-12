#include "BattleCalcsCoreTestExample.hpp"

#include <algorithm>

namespace
{
constexpr ae::q20_12_t kLevelMultipliers[24] = {
    ae::q20_12_t{0.5},  ae::q20_12_t{0.51}, ae::q20_12_t{0.53}, ae::q20_12_t{0.59}, ae::q20_12_t{0.66},
    ae::q20_12_t{0.75}, ae::q20_12_t{0.84}, ae::q20_12_t{0.91}, ae::q20_12_t{0.97}, ae::q20_12_t{0.99},
    ae::q20_12_t{1.0},  ae::q20_12_t{1.0},  ae::q20_12_t{1.0},  ae::q20_12_t{1.0},  ae::q20_12_t{1.01},
    ae::q20_12_t{1.03}, ae::q20_12_t{1.09}, ae::q20_12_t{1.16}, ae::q20_12_t{1.25}, ae::q20_12_t{1.34},
    ae::q20_12_t{1.41}, ae::q20_12_t{1.47}, ae::q20_12_t{1.49}, ae::q20_12_t{1.5},
};
}

ae::q20_12_t battle::calcs::levelDifferenceMultiplier(std::uint32_t attackerLevel, std::uint32_t defenderLevel)
{
    std::int32_t diff = static_cast<std::int32_t>(attackerLevel) - static_cast<std::int32_t>(defenderLevel);
    diff = std::clamp(diff, static_cast<std::int32_t>(-13), static_cast<std::int32_t>(10));

    return kLevelMultipliers[diff + 13];
}

ae::q20_12_t battle::calcs::affinityMultiplier(std::uint32_t affinity)
{
    switch (affinity)
    {
    case 1:
        return ae::q20_12_t{1.25};
    case 2:
        return ae::q20_12_t{0.5};
    case 3:
        return ae::q20_12_t{0.0};
    case 4:
        return ae::q20_12_t{-1.0};
    case 5:
        return ae::q20_12_t{-2.0};
    default:
        return ae::q20_12_t{1.0};
    }
}
