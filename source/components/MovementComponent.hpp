/**
 * @file MovementComponent.hpp
 * @brief Orchestrates the movement & view logic for a 3D environment view.
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once

#include "core/enums.h"
#include "core/structs.h"

#include "core/routerIDs.hpp"
#include <aegis/component.hpp>

#include "controllers/AnimationController.h"
#include "managers/MathManager.hpp"
#include "systems/CameraSystem.hpp"

class MovementComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::Movement);
    void Init() override;

    void Destroy() override;

    /**
     * @brief Core update loop that calculates the character & camera movement
     *
     * @param dt Fixed-point delta time passed from the aegis engine loop (currently unused).
     */
    void Update(ae::fixed_t /*dt*/) override;

    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

    /**
     * @brief Configure the movement system
     *
     * Required to call before calling start().
     *
     * @param config The struct containing the movement configuration to apply.
     */
    void configureMovement(const MovementConfig& config);

    /**
     * @brief Start the Update(ae::fixed_t) loop by setting isActive to true
     *
     * Movement must be set by configureMovement() before start() can be called
     */
    void start();

    /**
     * @brief Stop the Update(ae::fixed_t) loop by setting isActive to false
     */
    void stop();

    /**
     * @brief Get the current character position.
     *
     * @return the character position & facing angle in 3D space
     */
    CharacterPosition isCharacterAt();

    /**
     * @brief Get the tile value at the current character position.
     *
     * @details Wrapper function for private isTileAt(int tileX, int tileY)
     * function. Tiles are invisible triggers in 3D space.
     *
     * @return the TileType at the current character position in 3D space
     */
    TileType isTileAt();

  protected:
    void SubmitToManager() override
    {
    }

  private:
    MathManager& math = MathManager::GetInstance();
    AnimationController* animationCtrl = AnimationController::getInstance();

    MovementConfig config;
    CameraMode cameraMode = CameraMode::Static;
    int walkAnim;
    int idleAnim;

    /**
     * @brief Get the tile value at the given position.
     *
     * @details If a TileType is not defined for the given position,
     * TileType defaults to TileType::NO_COLLISION.
     *
     * @return the TileType at the given position in 3D space
     */
    TileType isTileAt(int tileX, int tileY);

    /**
     * @brief Checks for collision at the given position
     *
     * @details Collision is TileType::COLLISION
     *
     * @return true if there is collision at the given position in 3D space, otherwise false
     */
    bool isTileWalkable(float worldX, float worldZ);
};
