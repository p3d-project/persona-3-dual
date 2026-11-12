#include "battleActions/skills/BattleCalcsCoreTestExample.hpp"

#include <gtest/gtest.h>

TEST(BattleCalcsCore, LevelDifferenceUsesCenterValueAtParity)
{
    EXPECT_EQ(battle::calcs::levelDifferenceMultiplier(40, 40), ae::q20_12_t{1.0});
}

TEST(BattleCalcsCore, LevelDifferenceClampsLowAndHighBounds)
{
    EXPECT_EQ(battle::calcs::levelDifferenceMultiplier(1, 99), ae::q20_12_t{0.5});
    EXPECT_EQ(battle::calcs::levelDifferenceMultiplier(99, 1), ae::q20_12_t{1.5});
}

TEST(BattleCalcsCore, LevelDifferenceUsesKnownTablePoint)
{
    EXPECT_EQ(battle::calcs::levelDifferenceMultiplier(20, 25), ae::q20_12_t{0.97});
}

TEST(BattleCalcsCore, AffinityMultiplierMatchesBattleAffinityMapping)
{
    EXPECT_EQ(battle::calcs::affinityMultiplier(0), ae::q20_12_t{1.0});
    EXPECT_EQ(battle::calcs::affinityMultiplier(1), ae::q20_12_t{1.25});
    EXPECT_EQ(battle::calcs::affinityMultiplier(2), ae::q20_12_t{0.5});
    EXPECT_EQ(battle::calcs::affinityMultiplier(3), ae::q20_12_t{0.0});
    EXPECT_EQ(battle::calcs::affinityMultiplier(4), ae::q20_12_t{-1.0});
    EXPECT_EQ(battle::calcs::affinityMultiplier(5), ae::q20_12_t{-2.0});
}

TEST(BattleCalcsCore, AffinityMultiplierDefaultsToNeutralForUnknownValues)
{
    EXPECT_EQ(battle::calcs::affinityMultiplier(999), ae::q20_12_t{1.0});
}
