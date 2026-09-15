/**
 * @file MusicComponent.hpp
 * @brief Orchestrates music
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include "managers/AudioManager.hpp"
#include "types/aeTypes.hpp"
#include <aegis/component.hpp>
#include <string>

class MusicComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::Music);
    void Init() override
    {
    }

    /**
     * @brief Automatically deregisters the currently registered music, if any
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
     * @brief Set up the music to play
     *
     * @note Can only register one music track
     *
     * @param path the music file path
     * @param loopStartTime the loop start time in seconds
     * @param loopEndTimethe loop end time in seconds
     */
    void registerMusic(std::string path, ae::q20_12_t loopStartTime, ae::q20_12_t loopEndTime);

    /**
     * @brief Play the registered song if currently paused
     */
    void playMusic();

    /**
     * @brief Pause the registered song if currently playing
     */
    void pauseMusic();

    /**
     * @brief Deregister the currently registered music
     */
    void stopMusic();

  protected:
    void SubmitToManager() override
    {
    }

  private:
    AudioManager& am = AudioManager::GetInstance();
};
