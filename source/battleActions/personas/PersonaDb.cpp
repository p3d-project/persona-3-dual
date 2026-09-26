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

void PersonaDb::Initialize()
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

    orpheus.battleStats.affinities[(u32)Element::Slash] = BattleStats::Neutral;
    orpheus.battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
    orpheus.battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
    orpheus.battleStats.affinities[(u32)Element::Fire] = BattleStats::Neutral;
    orpheus.battleStats.affinities[(u32)Element::Ice] = BattleStats::Neutral;
    orpheus.battleStats.affinities[(u32)Element::Electricity] = BattleStats::Weak;
    orpheus.battleStats.affinities[(u32)Element::Wind] = BattleStats::Neutral;
    orpheus.battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
    orpheus.battleStats.affinities[(u32)Element::Dark] = BattleStats::Weak;
    orpheus.battleStats.affinities[(u32)Element::Almighty] = BattleStats::Neutral;

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

    forneus.battleStats.affinities[(u32)Element::Slash] = BattleStats::Neutral;
    forneus.battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
    forneus.battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
    forneus.battleStats.affinities[(u32)Element::Fire] = BattleStats::Weak;
    forneus.battleStats.affinities[(u32)Element::Ice] = BattleStats::Resist;
    forneus.battleStats.affinities[(u32)Element::Electricity] = BattleStats::Neutral;
    forneus.battleStats.affinities[(u32)Element::Wind] = BattleStats::Neutral;
    forneus.battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
    forneus.battleStats.affinities[(u32)Element::Dark] = BattleStats::Neutral;
    forneus.battleStats.affinities[(u32)Element::Almighty] = BattleStats::Neutral;

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

    hermes.battleStats.affinities[(u32)Element::Slash] = BattleStats::Neutral;
    hermes.battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
    hermes.battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
    hermes.battleStats.affinities[(u32)Element::Fire] = BattleStats::Resist;
    hermes.battleStats.affinities[(u32)Element::Ice] = BattleStats::Neutral;
    hermes.battleStats.affinities[(u32)Element::Electricity] = BattleStats::Neutral;
    hermes.battleStats.affinities[(u32)Element::Wind] = BattleStats::Weak;
    hermes.battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
    hermes.battleStats.affinities[(u32)Element::Dark] = BattleStats::Neutral;
    hermes.battleStats.affinities[(u32)Element::Almighty] = BattleStats::Neutral;

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

    io.battleStats.affinities[(u32)Element::Slash] = BattleStats::Neutral;
    io.battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
    io.battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
    io.battleStats.affinities[(u32)Element::Fire] = BattleStats::Neutral;
    io.battleStats.affinities[(u32)Element::Ice] = BattleStats::Neutral;
    io.battleStats.affinities[(u32)Element::Electricity] = BattleStats::Weak;
    io.battleStats.affinities[(u32)Element::Wind] = BattleStats::Resist;
    io.battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
    io.battleStats.affinities[(u32)Element::Dark] = BattleStats::Neutral;
    io.battleStats.affinities[(u32)Element::Almighty] = BattleStats::Neutral;
}
