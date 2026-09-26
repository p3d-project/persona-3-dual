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

void SkillDb::Initialize()
{
    /*---------------ALL OUT ATTACK--------------*/
    //Most of the stuff is not used, only element is actually important
    //this skill only exists because it would be even uglier to solve it otherwise
    allOutAttack.movePower = 15;
    allOutAttack.element = Element::Almighty;
    allOutAttack.cost = 0;
    allOutAttack.name = "All_Out_Attack";
    allOutAttack.hitRate = 90;
    allOutAttack.skillRace = SkillRace::mag;
    allOutAttack.skillTarget = SkillTarget::OppositionTeam;
    allOutAttack.skillType = SkillType::AllOutAttack;

    /*--------------REGULAR ATTACKS--------------*/
    slashAttack.movePower = 15;
    slashAttack.element = Element::Slash;
    slashAttack.cost = 0;
    slashAttack.name = "Slash_Attack";
    slashAttack.hitRate = 90;
    slashAttack.skillRace = SkillRace::phys;
    slashAttack.skillTarget = SkillTarget::OppositionTeam;
    slashAttack.skillType = SkillType::RegularAttack;

    strikeAttack.movePower = 15;
    strikeAttack.element = Element::Strike;
    strikeAttack.cost = 0;
    strikeAttack.name = "Strike_Attack";
    strikeAttack.hitRate = 90;
    strikeAttack.skillRace = SkillRace::phys;
    strikeAttack.skillTarget = SkillTarget::OppositionTeam;
    strikeAttack.skillType = SkillType::RegularAttack;

    pierceAttack.movePower = 15;
    pierceAttack.element = Element::Pierce;
    pierceAttack.cost = 0;
    pierceAttack.name = "Pierce_Attack";
    pierceAttack.hitRate = 90;
    pierceAttack.skillRace = SkillRace::phys;
    pierceAttack.skillTarget = SkillTarget::OppositionTeam;
    pierceAttack.skillType = SkillType::RegularAttack;

    /*--------------PHYSICAL--------------*/
    bash.movePower = 30;
    bash.element = Element::Strike;
    bash.cost = 10;
    bash.name = "Bash";
    bash.hitRate = 90;
    bash.skillRace = SkillRace::phys;
    bash.skillTarget = SkillTarget::OppositionTeam;
    bash.skillType = SkillType::Attack;

    powerSlash.movePower = 50;
    powerSlash.element = Element::Slash;
    powerSlash.cost = 7;
    powerSlash.name = "Power_Slash";
    powerSlash.hitRate = 92;
    powerSlash.skillRace = SkillRace::phys;
    powerSlash.skillTarget = SkillTarget::OppositionTeam;
    powerSlash.skillType = SkillType::Attack;

    /*--------------MAGIC--------------*/
    agi.movePower = 40;
    agi.element = Element::Fire;
    agi.cost = 3;
    agi.name = "Agi";
    agi.hitRate = 99;
    agi.skillRace = SkillRace::mag;
    agi.skillTarget = SkillTarget::OppositionTeam;
    agi.skillType = SkillType::Attack;

    bufu.movePower = 40;
    bufu.element = Element::Ice;
    bufu.cost = 3;
    bufu.name = "Bufu";
    bufu.hitRate = 99;
    bufu.skillRace = SkillRace::mag;
    bufu.skillTarget = SkillTarget::OppositionTeam;
    bufu.skillType = SkillType::Attack;

    garu.movePower = 40;
    garu.element = Element::Wind;
    garu.cost = 3;
    garu.name = "Garu";
    garu.hitRate = 99;
    garu.skillRace = SkillRace::mag;
    garu.skillTarget = SkillTarget::OppositionTeam;
    garu.skillType = SkillType::Attack;

    magaru.movePower = 40;
    magaru.element = Element::Wind;
    magaru.cost = 8;
    magaru.name = "Magaru";
    magaru.hitRate = 95;
    magaru.skillRace = SkillRace::mag;
    magaru.skillTarget = SkillTarget::OppositionTeam;
    magaru.skillType = SkillType::MultiAttack;

    /*--------------HEALING--------------*/
    dia.movePower = 50;
    dia.element = Element::Heal;
    dia.cost = 3;
    dia.name = "Dia";
    dia.hitRate = 100;
    dia.skillRace = SkillRace::mag;
    dia.skillTarget = SkillTarget::OwnTeam;
    dia.skillType = SkillType::Heal;
}
