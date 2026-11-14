/**
 * @file EnemyProfileDb.h
 * @brief Static db of EnemyProfiles
 * @author Nolan Kolb (themoonwalker8692 / TrueGiles)
 */

#pragma once
#include "EnemyProfile.hpp"

/**
 * @details
 * Db of enemy Profiles. Call initialize() once at startup before any
 * profile here is used to construct an Enemy.
 */
struct EnemyProfileDb
{
    static EnemyProfile mercilessMaya;
    static EnemyProfile cowardlyMaya;

    /// @brief Initialize all static EnemyProfiles with their stats, call once before first use
    static void initialize();

  private:
    static Skill* mercilessMayaSkills[1];
    static Skill* cowardlyMayaSkills[1];
};
