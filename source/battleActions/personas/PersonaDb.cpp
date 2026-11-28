#include "PersonaDb.hpp"
#include "../skills/SkillDb.hpp"

PersonaBase PersonaDb::orpheus;
PersonaBase PersonaDb::forneus;
PersonaBase PersonaDb::hermes;
PersonaBase PersonaDb::io;

Skill* PersonaDb::orpheusSkills[2];
Skill* PersonaDb::forneusSkills[2];
Skill* PersonaDb::hermesSkills[1];
Skill* PersonaDb::ioSkills[3];

void PersonaDb::initialize()
{
    /*--------------ORPHEUS--------------*/
    orpheus.name = "Orpheus";
    orpheus.lv = 1;
    orpheus.battleStats.st = 2;
    orpheus.battleStats.ma = 2;
    orpheus.battleStats.en = 2;
    orpheus.battleStats.ag = 3;
    orpheus.battleStats.lu = 2;

    orpheusSkills[0] = &SkillDb::agi;
    orpheusSkills[1] = &SkillDb::bash;
    orpheus.skills = orpheusSkills;
    orpheus.skillCount = 2;

    orpheus.battleStats.affinities[(u32)Element::SLASH] = BattleStats::Affinity::NEUTRAL;
    orpheus.battleStats.affinities[(u32)Element::STRIKE] = BattleStats::Affinity::NEUTRAL;
    orpheus.battleStats.affinities[(u32)Element::PIERCE] = BattleStats::Affinity::NEUTRAL;
    orpheus.battleStats.affinities[(u32)Element::FIRE] = BattleStats::Affinity::NEUTRAL;
    orpheus.battleStats.affinities[(u32)Element::ICE] = BattleStats::Affinity::NEUTRAL;
    orpheus.battleStats.affinities[(u32)Element::ELECTRICITY] = BattleStats::Affinity::WEAK;
    orpheus.battleStats.affinities[(u32)Element::WIND] = BattleStats::Affinity::NEUTRAL;
    orpheus.battleStats.affinities[(u32)Element::LIGHT] = BattleStats::Affinity::NEUTRAL;
    orpheus.battleStats.affinities[(u32)Element::DARK] = BattleStats::Affinity::WEAK;
    orpheus.battleStats.affinities[(u32)Element::ALMIGHTY] = BattleStats::Affinity::NEUTRAL;

    /*--------------FORNEUS--------------*/
    forneus.name = "Forneus";
    forneus.lv = 7;
    forneus.battleStats.st = 5;
    forneus.battleStats.ma = 6;
    forneus.battleStats.en = 7;
    forneus.battleStats.ag = 6;
    forneus.battleStats.lu = 4;

    forneusSkills[0] = &SkillDb::bash;
    forneusSkills[1] = &SkillDb::bufu;
    forneus.skills = forneusSkills;
    forneus.skillCount = 2;

    forneus.battleStats.affinities[(u32)Element::SLASH] = BattleStats::Affinity::NEUTRAL;
    forneus.battleStats.affinities[(u32)Element::STRIKE] = BattleStats::Affinity::NEUTRAL;
    forneus.battleStats.affinities[(u32)Element::PIERCE] = BattleStats::Affinity::NEUTRAL;
    forneus.battleStats.affinities[(u32)Element::FIRE] = BattleStats::Affinity::WEAK;
    forneus.battleStats.affinities[(u32)Element::ICE] = BattleStats::Affinity::RESIST;
    forneus.battleStats.affinities[(u32)Element::ELECTRICITY] = BattleStats::Affinity::NEUTRAL;
    forneus.battleStats.affinities[(u32)Element::WIND] = BattleStats::Affinity::NEUTRAL;
    forneus.battleStats.affinities[(u32)Element::LIGHT] = BattleStats::Affinity::NEUTRAL;
    forneus.battleStats.affinities[(u32)Element::DARK] = BattleStats::Affinity::NEUTRAL;
    forneus.battleStats.affinities[(u32)Element::ALMIGHTY] = BattleStats::Affinity::NEUTRAL;

    /*--------------HERMES--------------*/
    hermes.name = "Hermes";
    hermes.lv = 1;
    hermes.battleStats.st = 3;
    hermes.battleStats.ma = 1;
    hermes.battleStats.en = 2;
    hermes.battleStats.ag = 3;
    hermes.battleStats.lu = 1;

    hermesSkills[0] = &SkillDb::powerSlash;
    hermes.skills = hermesSkills;
    hermes.skillCount = 1;

    hermes.battleStats.affinities[(u32)Element::SLASH] = BattleStats::Affinity::NEUTRAL;
    hermes.battleStats.affinities[(u32)Element::STRIKE] = BattleStats::Affinity::NEUTRAL;
    hermes.battleStats.affinities[(u32)Element::PIERCE] = BattleStats::Affinity::NEUTRAL;
    hermes.battleStats.affinities[(u32)Element::FIRE] = BattleStats::Affinity::RESIST;
    hermes.battleStats.affinities[(u32)Element::ICE] = BattleStats::Affinity::NEUTRAL;
    hermes.battleStats.affinities[(u32)Element::ELECTRICITY] = BattleStats::Affinity::NEUTRAL;
    hermes.battleStats.affinities[(u32)Element::WIND] = BattleStats::Affinity::WEAK;
    hermes.battleStats.affinities[(u32)Element::LIGHT] = BattleStats::Affinity::NEUTRAL;
    hermes.battleStats.affinities[(u32)Element::DARK] = BattleStats::Affinity::NEUTRAL;
    hermes.battleStats.affinities[(u32)Element::ALMIGHTY] = BattleStats::Affinity::NEUTRAL;

    /*--------------IO--------------*/
    io.name = "Io";
    io.lv = 1;
    io.battleStats.st = 2;
    io.battleStats.ma = 3;
    io.battleStats.en = 1;
    io.battleStats.ag = 2;
    io.battleStats.lu = 2;

    ioSkills[0] = &SkillDb::garu;
    ioSkills[1] = &SkillDb::dia;
    ioSkills[2] = &SkillDb::magaru;
    io.skills = ioSkills;
    io.skillCount = 3;

    io.battleStats.affinities[(u32)Element::SLASH] = BattleStats::Affinity::NEUTRAL;
    io.battleStats.affinities[(u32)Element::STRIKE] = BattleStats::Affinity::NEUTRAL;
    io.battleStats.affinities[(u32)Element::PIERCE] = BattleStats::Affinity::NEUTRAL;
    io.battleStats.affinities[(u32)Element::FIRE] = BattleStats::Affinity::NEUTRAL;
    io.battleStats.affinities[(u32)Element::ICE] = BattleStats::Affinity::NEUTRAL;
    io.battleStats.affinities[(u32)Element::ELECTRICITY] = BattleStats::Affinity::WEAK;
    io.battleStats.affinities[(u32)Element::WIND] = BattleStats::Affinity::RESIST;
    io.battleStats.affinities[(u32)Element::LIGHT] = BattleStats::Affinity::NEUTRAL;
    io.battleStats.affinities[(u32)Element::DARK] = BattleStats::Affinity::NEUTRAL;
    io.battleStats.affinities[(u32)Element::ALMIGHTY] = BattleStats::Affinity::NEUTRAL;
}
