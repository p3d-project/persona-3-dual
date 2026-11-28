/**
 * @file EnemyProfileDb.cpp
 * @brief Holds / Populates the EnemyProfile's
 * @author Nolan Kolb (theMoonWalker8692 / TrueGiles)
 */

#include "EnemyProfileDb.hpp"
#include "../armours/ArmourDb.hpp"
#include "../shoes/ShoeDb.hpp"
#include "../skills/SkillDb.hpp"

EnemyProfile EnemyProfileDb::mercilessMaya;
EnemyProfile EnemyProfileDb::cowardlyMaya;

Skill* EnemyProfileDb::mercilessMayaSkills[1];
Skill* EnemyProfileDb::cowardlyMayaSkills[1];

void EnemyProfileDb::initialize()
{
    /*--------------MERCILESS MAYA--------------*/
    mercilessMaya.name = "Merciless Maya";
    mercilessMaya.maxHp = 83;
    mercilessMaya.hp = 83;
    mercilessMaya.maxSp = 15;
    mercilessMaya.sp = 15;
    mercilessMaya.lv = 2;
    mercilessMaya.battleStats.st = 2;
    mercilessMaya.battleStats.ma = 4;
    mercilessMaya.battleStats.en = 3;
    mercilessMaya.battleStats.ag = 2;
    mercilessMaya.battleStats.lu = 2;
    mercilessMaya.baseAttackAction = &SkillDb::strikeAttack;
    mercilessMaya.armour = ArmourDb::enemyArmour;
    mercilessMaya.shoe = ShoeDb::enemyShoe;

    mercilessMayaSkills[0] = &SkillDb::agi;
    mercilessMaya.skill = mercilessMayaSkills;
    mercilessMaya.skillCount = 1;

    mercilessMaya.battleStats.affinities[(u32)Element::SLASH] = BattleStats::Affinity::WEAK;
    mercilessMaya.battleStats.affinities[(u32)Element::STRIKE] = BattleStats::Affinity::NEUTRAL;
    mercilessMaya.battleStats.affinities[(u32)Element::PIERCE] = BattleStats::Affinity::NEUTRAL;
    mercilessMaya.battleStats.affinities[(u32)Element::FIRE] = BattleStats::Affinity::RESIST;
    mercilessMaya.battleStats.affinities[(u32)Element::ICE] = BattleStats::Affinity::WEAK;
    mercilessMaya.battleStats.affinities[(u32)Element::ELECTRICITY] = BattleStats::Affinity::NULLIFY;
    mercilessMaya.battleStats.affinities[(u32)Element::WIND] = BattleStats::Affinity::NEUTRAL;
    mercilessMaya.battleStats.affinities[(u32)Element::LIGHT] = BattleStats::Affinity::NEUTRAL;
    mercilessMaya.battleStats.affinities[(u32)Element::DARK] = BattleStats::Affinity::NEUTRAL;
    mercilessMaya.battleStats.affinities[(u32)Element::ALMIGHTY] = BattleStats::Affinity::NEUTRAL;

    /*--------------COWARDLY MAYA--------------*/
    cowardlyMaya.name = "Cowardly Maya";
    cowardlyMaya.maxHp = 83;
    cowardlyMaya.hp = 83;
    cowardlyMaya.maxSp = 20;
    cowardlyMaya.sp = 20;
    cowardlyMaya.lv = 2;
    cowardlyMaya.battleStats.st = 2;
    cowardlyMaya.battleStats.ma = 3;
    cowardlyMaya.battleStats.en = 3;
    cowardlyMaya.battleStats.ag = 2;
    cowardlyMaya.battleStats.lu = 2;
    cowardlyMaya.baseAttackAction = &SkillDb::strikeAttack;
    cowardlyMaya.armour = ArmourDb::enemyArmour;
    cowardlyMaya.shoe = ShoeDb::enemyShoe;

    cowardlyMayaSkills[0] = &SkillDb::bufu;
    cowardlyMaya.skill = cowardlyMayaSkills;
    cowardlyMaya.skillCount = 1;

    cowardlyMaya.battleStats.affinities[(u32)Element::SLASH] = BattleStats::Affinity::WEAK;
    cowardlyMaya.battleStats.affinities[(u32)Element::STRIKE] = BattleStats::Affinity::NEUTRAL;
    cowardlyMaya.battleStats.affinities[(u32)Element::PIERCE] = BattleStats::Affinity::NEUTRAL;
    cowardlyMaya.battleStats.affinities[(u32)Element::FIRE] = BattleStats::Affinity::WEAK;
    cowardlyMaya.battleStats.affinities[(u32)Element::ICE] = BattleStats::Affinity::NEUTRAL;
    cowardlyMaya.battleStats.affinities[(u32)Element::ELECTRICITY] = BattleStats::Affinity::NEUTRAL;
    cowardlyMaya.battleStats.affinities[(u32)Element::WIND] = BattleStats::Affinity::NEUTRAL;
    cowardlyMaya.battleStats.affinities[(u32)Element::LIGHT] = BattleStats::Affinity::NEUTRAL;
    cowardlyMaya.battleStats.affinities[(u32)Element::DARK] = BattleStats::Affinity::NEUTRAL;
    cowardlyMaya.battleStats.affinities[(u32)Element::ALMIGHTY] = BattleStats::Affinity::NEUTRAL;
}
