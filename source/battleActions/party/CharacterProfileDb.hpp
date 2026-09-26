#pragma once
#include "CharacterProfile.hpp"

struct CharacterProfileDb
{
    static CharacterProfile player;
    static CharacterProfile yukari;
    static CharacterProfile junpei;

    static void Initialize();
};
