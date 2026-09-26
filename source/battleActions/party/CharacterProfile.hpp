#pragma once
#include "../ParticipantType.hpp"
#include "../ProfileBase.hpp"
#include "../personas/PersonaBase.hpp"
#include "../shoes/Shoe.hpp"
#include "../skills/Skill.hpp"
#include "../weapons/Weapon.hpp"
#include <etl/vector.h>
#include <nds.h>

/**
 * @brief Holds character data which a battleParticipant is created from
 *
 * @details
 * This datatype holds the current information of a character and
 * should in the future be synced after each battle so we can re-create the
 * characters correctly in a new batle
 *
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
 */
struct CharacterProfile : ProfileBase
{
    ParticipantType participantType;

    ArmourType armourType;

    WeaponType weaponType;
    Weapon weapon;

    etl::vector<PersonaBase*, 13> personas;
    PersonaBase* curPersona;
};
