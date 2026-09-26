/**
 * @file CameraSystem.hpp
 * @brief Manages the camera for a 3D environment view.
 *
 * Owns all camera state (position, angle, path playback). Call configure()
 * once on room load, then update() every frame to get the gluLookAt arguments.
 *
 * @author Oles Gedz (olesgedz)
 */

#pragma once

#include "core/geometry.hpp"
#include "core/routerIDs.hpp"
#include "events/CameraEvents.hpp"
#include "events/GenericEvents.hpp"
#include "managers/MathManager.hpp"
#include "types/CameraTypes.hpp"

#include <aegis/system.hpp>
#include <etl/vector.h>

// !Todo replace floats with fixed point math for camera position and target position.

class CameraSystem : public ae::SystemRouter<CameraSystem,
                                             Event::ConfigureCamera,
                                             Event::SetCameraMode,
                                             Event::SetCameraPath,
                                             Event::SetCharacterPosition,
                                             Event::StartCamera,
                                             Event::StopCamera>,
                     public ae::Singleton<CameraSystem>
{
  public:
    void Init() override;

    void Shutdown() override;

    /**
     * @brief Core update loop that advances camera state and broadcasts the
     * resulting gluLookAt arguments.
     *
     * @param dt Fixed-point delta time passed from the aegis engine loop (currently unused).
     */
    void Update(ae::q20_12_t /*dt*/) override;

    /**
     * @brief ETL message handler to configure the camera settings.
     *
     * @details Resets all tuning parameters and mode from @p config.
     * Call this after setupCamera() sets up the room-specific
     * @ref CameraConfig.
     *
     * Required to call in order to enable the CameraSystem
     *
     * @param config The event payload containing the camera configuration to apply.
     */
    void on_receive(const Event::ConfigureCamera& config);

    /**
     * @brief ETL message handler to set the active camera mode at
     * runtime.
     *
     * @param msg The event payload containing the camera mode to apply.
     */
    void on_receive(const Event::SetCameraMode& msg);

    /**
     * @brief ETL message handler to set the path used by @ref CameraMode::Path
     * and rewinds playback
     *
     * Call this before switching mode to Path. The pointer must remain valid
     * for the lifetime of the playback.
     *
     * @param msg The event payload containing the pointer to the CameraPath to
     * play. Must not be null
     */
    void on_receive(const Event::SetCameraPath& msg);

    /**
     * @brief ETL message handler to set the character position
     *
     * @param msg The event payload containing the pointer to CharacterPosition
     */
    void on_receive(const Event::SetCharacterPosition& msg);

    /**
     * @brief ETL message handler to make the Camera active
     *
     * @param msg The event payload (unused)
     */
    void on_receive(const Event::StartCamera& msg);

    /**
     * @brief ETL message handler to disable the Camera
     *
     * @param msg The event payload (unused)
     */
    void on_receive(const Event::StopCamera& msg);

    /**
     * @brief Fallback handler for unhandled ETL messages.
     *
     * @details Required by the ETL message router interface. Safely ignores
     * any messages routed to the CameraSystem that do not have a specific handler.
     *
     * @param msg The unhandled incoming message (unused).
     */
    void on_receive_unknown(const etl::imessage& msg)
    {
    }

    /** @brief Returns the current camera mode. */
    CameraMode getMode() const
    {
        return mode;
    }

    /** @brief Returns the current orbit angle in radians. */
    ae::q20_12_t getAngle() const
    {
        return angle;
    }

    /** @brief Returns true once the Path playback has reached its last keyframe. */
    bool isPathComplete() const
    {
        return pathDone;
    }

    /**
     * @brief Returns the angle that maps the UP key to "move away from camera".
     *
     * For Follow/Free returns the current orbit angle. For CCTV/Static
     * computes atan2 from the fixed eye position toward the character, so
     * movement direction stays correct regardless of where the camera is mounted.
     *
     * @return Angle in radians.
     */
    ae::q20_12_t getMovementAngle() const;

    /**
     * @brief Returns the camera position.
     *
     * camPos gets updated in the CameraSystem:Update(ae::q20_12_t) loop. This is a temporary
     * helper function that should get removed while certain parts of the
     * game still do not comply with aegis-engine.
     *
     * @todo In practice, code that needs to recieve the CameraPosition should capture
     * CameraPosition as a Event, which automatically gets broadcasted during the
     * CameraSystem::Update(ae::q20_12_t) loop.
     *
     * @return The latest camera position.
     */
    Event::CameraPosition getCameraPosition() const
    {
        return camPos;
    }

  private:
    friend class Singleton<CameraSystem>;
    CameraSystem() : SystemRouter(kCameraSystemRouterID)
    {
    }

    MathManager& math = MathManager::GetInstance();

    CameraMode mode = CameraMode::Follow;

    Vec3<ae::q20_12_t> currentPos = {};
    Vec3<ae::q20_12_t> targetPos = {};

    CharacterPosition charPos;
    // TODO: remove camPos in favour of Event pub/sub?
    Event::CameraPosition camPos = {};
    ae::q20_12_t angle{0};
    ae::q20_12_t distance{1.5};
    ae::q20_12_t height{0.6};
    ae::q20_12_t lookAhead{0.5};
    ae::q20_12_t angleIncrement{0.05};
    ae::q20_12_t freeCameraSpeed{0.02};
    bool isRotationLocked = false;

    // Path playback state
    const CameraPath* path = nullptr;
    int pathFrame = 0;
    int pathKeyIndex = 0;
    bool pathDone = false;

    // Free mode state
    bool freeInitialised = false;
};
