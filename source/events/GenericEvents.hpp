/**
 * @file GenericEvents.hpp
 * @brief Generic events used across multiple systems
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once

#include "types/MovementTypes.hpp"
#include "types/StateTypes.hpp"
#include "types/aeTypes.hpp"

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

/**
 * @brief Event payload to release resources owned by the current view.
 */
struct ResetUIResources : public etl::message<EventID::ResetUIResources>
{
};
} // namespace Event
