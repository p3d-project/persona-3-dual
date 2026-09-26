#pragma once
#include "PersonaBase.hpp"

struct PersonaDb
{
    static PersonaBase orpheus;
    static PersonaBase forneus;
    static PersonaBase hermes;
    static PersonaBase io;

    static void Initialize();

  private:
    static Skill* orpheusSkills[2];
    static Skill* forneusSkills[2];
    static Skill* hermesSkills[1];
    static Skill* ioSkills[3];
};
