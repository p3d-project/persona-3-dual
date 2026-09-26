/**
 * @file SaveEvents.hpp
 * @brief Events for SaveSystem
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once

#include "types/aeTypes.hpp"

#include <aegis/aegis.hpp>

namespace Event
{
/**
 * @brief Event payload to trigger a save write
 */
struct WriteSave : public etl::message<EventID::WriteSave>
{
};

/**
 * @brief Event payload to trigger a save read
 */
struct ReadSave : public etl::message<EventID::ReadSave>
{
};
} // namespace Event
