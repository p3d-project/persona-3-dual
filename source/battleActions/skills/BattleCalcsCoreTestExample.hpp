#pragma once

#include <aegis/types.hpp>

#include <cstdint>

namespace battle::calcs
{
ae::q20_12_t levelDifferenceMultiplier(std::uint32_t attackerLevel, std::uint32_t defenderLevel);
ae::q20_12_t affinityMultiplier(std::uint32_t affinity);
} // namespace battle::calcs
