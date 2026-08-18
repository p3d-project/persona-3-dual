/**
 * @file GenericEvents.hpp
 * @brief Generic events used across multiple systems
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include "core/structs.h"
#include <aegis/aegis.hpp>

namespace Event
{
/**
 * @brief Event payload to set the character position data.
 */
struct SetCharacterPosition : public etl::message<EventID::SetCharacterPosition>
{
    CharacterPosition charPos;
    SetCharacterPosition(CharacterPosition iCharPos) : charPos(iCharPos)
    {
    }
};

/**
 * @brief Event payload to switch the ViewState.
 */
struct SwitchView : public etl::message<EventID::SwitchView>
{
    ViewState view;
    SwitchView(ViewState iView) : view(iView)
    {
    }
};
} // namespace Event
