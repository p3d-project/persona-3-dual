#pragma once
#include "BattlePhase.hpp"
#include "BattleStats.hpp"
#include "ParticipantType.hpp"
#include "armours/Armour.hpp"
#include "events/BattleEvents.hpp"
#include "managers/MathManager.hpp"
#include "shoes/Shoe.hpp"
#include "skills/Skill.hpp"
#include <aegis/ndsTypes.hpp>
#include <nds.h>
#include <vector>

/**
 * @brief Participant of battle
 *
 * can be an enemy, partyMember or player.
 *
 * @details
 * Parent of PartyMember.cpp (which is the Player's parent) and Enemy.cpp
 * since a lot of things are shared between them, and even for some things
 * where you might not expect it like enemies have values for Armour
 * with an actuall usecase. Otherwise the goal is to really on virtual
 * functions to generalize the code as much as possible to make it
 * scalable.
 *
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
 */
struct BattleParticipant
{
    std::string name;
    s32 maxHp;
    s32 hp;
    s32 maxSp;
    s32 sp;
    u32 lv;
    // TODO: dont think so that all bosses have a basattack, possibly move in the future
    Skill* baseAttackAction;
    ParticipantType participantType;

    //Enemies have default values
    Armour armour;
    Shoe shoe;

    ae::q20_12_t currentTurnOrderAgility;
    bool oneMore = false;
    bool knockedDown = false;

    virtual BattleStats* getBattleStats() = 0;
    virtual ae::q20_12_t calculateBaseDamage(BattleParticipant& defender, Skill& skill) = 0;
    virtual ae::q20_12_t getTeamMultiplier() = 0;
    virtual void setCurrentTurnOrderAgility(ae::q20_12_t boost) = 0;
    virtual BattlePhase getInitalTurnPhase() = 0;
    virtual void onDead(Event::BattleResult& battleResult) = 0;

    virtual ~BattleParticipant() = default;
};
