/**
 * @file CameraEvents.hpp
 * @brief Events for CameraSystem
 * @author Oles Gedz (olesgedz)
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include "core/enums.h"
#include "core/geometry.h"
#include "core/structs.h"
#include <aegis/aegis.hpp>

namespace Event
{
/**
 * @brief Output of @ref CameraSystem::Update(ae::fixed_t), consumed by gluLookAt().
 */
struct CameraPosition : public etl::message<EventID::CameraPosition>
{
    Vec3<float> eye;    ///< Camera eye position.
    Vec3<float> target; ///< Look-at point.
    Vec3<float> up;     ///< Up vector (default 0,1,0).
};

/**
 * @brief All parameters needed to configure a @ref CameraSystem in one call.
 *
 * Set the relevant fields for the chosen mode. Fields irrelevant to the chosen
 * mode are ignored.
 */
struct ConfigureCamera : public etl::message<EventID::ConfigureCamera>
{
    CameraMode mode = CameraMode::Follow;

    // Static / CCTV — fixed eye position
    Vec3<float> eye = {};
    Vec3<float> target = {}; ///< Look-at point. Used by Static only.

    // Follow / Free
    float initialAngle = 0.0f;    ///< Starting orbit angle in radians.
    float distance = 1.5f;        ///< Distance from character to camera eye.
    float height = 0.6f;          ///< Eye height above the character origin.
    float lookAhead = 0.5f;       ///< Distance ahead of the character for the look-at point.
    float angleIncrement = 0.05f; ///< Radians rotated per frame on L/R input.
};

/**
 * @brief Set the CameraMode.
 */
struct SetCameraMode : public etl::message<EventID::SetCameraMode>
{
    CameraMode mode;
    SetCameraMode(CameraMode iMode) : mode(iMode)
    {
    }
};

/**
 * @brief Sets the pointer to a CameraPath.
 */
struct SetCameraPath : public etl::message<EventID::SetCameraPath>
{
    CameraPath* path;
    SetCameraPath(CameraPath* iPath) : path(iPath)
    {
    }
};

/**
 * @brief Activates the CameraSystem update loop
 */
struct StartCamera : public etl::message<EventID::StartCamera>
{
};

/**
 * @brief Deactivates the CameraSystem update loop
 */
struct StopCamera : public etl::message<EventID::StopCamera>
{
};
} // namespace Event
