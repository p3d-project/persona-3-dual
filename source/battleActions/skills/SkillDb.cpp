#include "SkillDb.hpp"

Skill SkillDb::agi;
Skill SkillDb::bash;
Skill SkillDb::bufu;
Skill SkillDb::dia;
Skill SkillDb::garu;
Skill SkillDb::magaru;
Skill SkillDb::pierceAttack;
Skill SkillDb::powerSlash;
Skill SkillDb::slashAttack;
Skill SkillDb::strikeAttack;
Skill SkillDb::allOutAttack;

void SkillDb::initialize()
{
    /*---------------ALL OUT ATTACK--------------*/
    //Most of the stuff is not used, only element is actually important
    //this skill only exists because it would be even uglier to solve it otherwise
    allOutAttack.movePower = 15;
    allOutAttack.element = Element::ALMIGHTY;
    allOutAttack.cost = 0;
    allOutAttack.name = "All_Out_Attack";
    allOutAttack.hitRate = 90;
    allOutAttack.skillRace = SkillRace::MAG;
    allOutAttack.skillTarget = SkillTarget::OPPOSITION_TEAM;
    allOutAttack.skillType = SkillType::ALL_OUT_ATTACK;

    /*--------------REGULAR ATTACKS--------------*/
    slashAttack.movePower = 15;
    slashAttack.element = Element::SLASH;
    slashAttack.cost = 0;
    slashAttack.name = "Slash_Attack";
    slashAttack.hitRate = 90;
    slashAttack.skillRace = SkillRace::PHYS;
    slashAttack.skillTarget = SkillTarget::OPPOSITION_TEAM;
    slashAttack.skillType = SkillType::REGULAR_ATTACK;

    strikeAttack.movePower = 15;
    strikeAttack.element = Element::STRIKE;
    strikeAttack.cost = 0;
    strikeAttack.name = "Strike_Attack";
    strikeAttack.hitRate = 90;
    strikeAttack.skillRace = SkillRace::PHYS;
    strikeAttack.skillTarget = SkillTarget::OPPOSITION_TEAM;
    strikeAttack.skillType = SkillType::REGULAR_ATTACK;

    pierceAttack.movePower = 15;
    pierceAttack.element = Element::PIERCE;
    pierceAttack.cost = 0;
    pierceAttack.name = "Pierce_Attack";
    pierceAttack.hitRate = 90;
    pierceAttack.skillRace = SkillRace::PHYS;
    pierceAttack.skillTarget = SkillTarget::OPPOSITION_TEAM;
    pierceAttack.skillType = SkillType::REGULAR_ATTACK;

    /*--------------PHYSICAL--------------*/
    bash.movePower = 30;
    bash.element = Element::STRIKE;
    bash.cost = 10;
    bash.name = "Bash";
    bash.hitRate = 90;
    bash.skillRace = SkillRace::PHYS;
    bash.skillTarget = SkillTarget::OPPOSITION_TEAM;
    bash.skillType = SkillType::ATTACK;

    powerSlash.movePower = 50;
    powerSlash.element = Element::SLASH;
    powerSlash.cost = 7;
    powerSlash.name = "Power_Slash";
    powerSlash.hitRate = 92;
    powerSlash.skillRace = SkillRace::PHYS;
    powerSlash.skillTarget = SkillTarget::OPPOSITION_TEAM;
    powerSlash.skillType = SkillType::ATTACK;

    /*--------------MAGIC--------------*/
    agi.movePower = 40;
    agi.element = Element::FIRE;
    agi.cost = 3;
    agi.name = "Agi";
    agi.hitRate = 99;
    agi.skillRace = SkillRace::MAG;
    agi.skillTarget = SkillTarget::OPPOSITION_TEAM;
    agi.skillType = SkillType::ATTACK;

    bufu.movePower = 40;
    bufu.element = Element::ICE;
    bufu.cost = 3;
    bufu.name = "Bufu";
    bufu.hitRate = 99;
    bufu.skillRace = SkillRace::MAG;
    bufu.skillTarget = SkillTarget::OPPOSITION_TEAM;
    bufu.skillType = SkillType::ATTACK;

    garu.movePower = 40;
    garu.element = Element::WIND;
    garu.cost = 3;
    garu.name = "Garu";
    garu.hitRate = 99;
    garu.skillRace = SkillRace::MAG;
    garu.skillTarget = SkillTarget::OPPOSITION_TEAM;
    garu.skillType = SkillType::ATTACK;

    magaru.movePower = 40;
    magaru.element = Element::WIND;
    magaru.cost = 8;
    magaru.name = "Magaru";
    magaru.hitRate = 95;
    magaru.skillRace = SkillRace::MAG;
    magaru.skillTarget = SkillTarget::OPPOSITION_TEAM;
    magaru.skillType = SkillType::MULTI_ATTACK;

    /*--------------HEALING--------------*/
    dia.movePower = 50;
    dia.element = Element::HEAL;
    dia.cost = 3;
    dia.name = "Dia";
    dia.hitRate = 100;
    dia.skillRace = SkillRace::MAG;
    dia.skillTarget = SkillTarget::OWN_TEAM;
    dia.skillType = SkillType::HEAL;
}
