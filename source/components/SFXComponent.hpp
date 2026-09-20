/**
 * @file SFXComponent.hpp
 * @brief Orchestrates sfx
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include "managers/AudioManager.hpp"
#include "types/AudioTypes.hpp"
#include "types/aeTypes.hpp"
#include <aegis/component.hpp>
#include <string>

class SFXComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::SFX);
    void Init() override
    {
    }

    /**
     * @brief Automatically stops all SFX audio from playing
     */
    void Destroy() override;

    void Update(ae::q20_12_t /*dt*/) override
    {
    }

    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

    /**
     * @brief Register the SFX to use
     *
     * @note Multiple SFXs can be registered at once
     *
     * @param sfx the SFX to register
     */
    void registerSFX(SFX sfx);

    /**
     * @brief Play the specified SFX, if registered
     *
     * @param sfx the SFX to play
     * @param volume the volume of the SFX. Range is from 0-127
     * @param panning the direction to play the sound effect (left to right). The range is 0-127 (64 plays both left and right equally)
     */
    void playSFX(SFX sfx, int volume = 127, int panning = 64);

    /**
     * @brief Stops all SFX audio from playing
     */
    void stopSFX();

  protected:
    void SubmitToManager() override
    {
    }

  private:
    AudioManager& am = AudioManager::GetInstance();
};
