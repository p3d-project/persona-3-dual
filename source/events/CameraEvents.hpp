/**
 * @file CameraEvents.hpp
 * @brief Events for CameraSystem
 * @author Oles Gedz (olesgedz)
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once

#include "core/geometry.hpp"
#include "types/CameraTypes.hpp"
#include "types/aeTypes.hpp"

#include <aegis/aegis.hpp>

namespace Event
{
/**
 * @brief Output of @ref CameraSystem::Update(ae::q20_12_t), consumed by gluLookAt().
 */
struct CameraPosition : public etl::message<EventID::CameraPosition>
{
    Vec3<ae::q20_12_t> eye;    ///< Camera eye position.
    Vec3<ae::q20_12_t> target; ///< Look-at point.
    Vec3<ae::q20_12_t> up;     ///< Up vector (default 0,1,0).
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
    Vec3<ae::q20_12_t> eye = {};
    Vec3<ae::q20_12_t> target = {}; ///< Look-at point. Used by Static only.

    // Follow / Free
    ae::q20_12_t initialAngle{0};      ///< Starting orbit angle in radians.
    ae::q20_12_t distance{1.5};        ///< Distance from character to camera eye.
    ae::q20_12_t height{0.6};          ///< Eye height above the character origin.
    ae::q20_12_t lookAhead{0.5};       ///< Distance ahead of the character for the look-at point.
    ae::q20_12_t angleIncrement{0.05}; ///< Radians rotated per frame on L/R input.
    bool isRotationLocked = false;     ///< Disable camera angle rotation on L/R input.
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
