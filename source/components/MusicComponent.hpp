/**
 * @file MusicComponent.hpp
 * @brief Orchestrates music
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include "data/audioDb.hpp"
#include "managers/AudioManager.hpp"
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
     * @brief Wrapper to set up the music to play
     *
     * @note Can only register one music track
     *
     * @param type the audio type
     * @param audioId the specific audio to play
     */
    template <typename AudioID> void registerMusic(AudioType type, AudioID audioId)
    {
        registerMusic(type, static_cast<int>(audioId));
    }

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

    /**
     * @brief Implementation to set up the music to play
     *
     * @note Can only register one music track
     *
     * @param type the audio type
     * @param audioId the specific audio to play
     */
    void registerMusic(AudioType type, int audioId);
};
