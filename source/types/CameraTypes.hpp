#pragma once

#include "core/geometry.hpp"
#include <aegis/ndsTypes.hpp>
#include <aegis/types.hpp>
#include <etl/vector.h>

/**
 * @brief Controls how the camera behaves each frame.
 *
 * - Free   : first-person fly cam, d-pad moves, L/R rotates.
 * - Static : fixed eye and target, ignores all input.
 * - CCTV   : fixed eye position, target tracks the character.
 * - Follow : orbits behind the character, L/R adjusts orbit angle.
 * - Path   : plays back a @ref CameraPath keyframe sequence, then
 *            automatically returns to Follow when complete.
 */
enum class CameraMode
{
    Free,
    Static,
    CCTV,
    Follow,
    Path
};

/**
 * @brief A single keyframe in a camera path.
 *
 * @see See CameraPath
 */
struct CameraKeyframe
{
    int time;                  ///< Frame index at which this keyframe is reached.
    Vec3<ae::q20_12_t> eye;    ///< Camera eye position.
    Vec3<ae::q20_12_t> target; ///< Look-at position.
};

/**
 * @brief An ordered list of keyframes defining a camera animation.
 *
 * The camera interpolates linearly between consecutive keyframes.
 * On completion the @ref CameraSystem switches to Follow mode.
 */
struct CameraPath
{
    etl::vector<CameraKeyframe, 100> keyframes;
};
