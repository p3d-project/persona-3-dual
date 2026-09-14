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

    // TODO: add doxygen
    void Destroy() override;

    void Update(ae::q20_12_t /*dt*/) override
    {
    }

    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

    // TODO: add doxygen
    void registerMusic(std::string path, ae::q20_12_t loopStartTime, ae::q20_12_t loopEndTime);
    void playMusic();
    void pauseMusic();
    void stopMusic();

  protected:
    void SubmitToManager() override
    {
    }

  private:
    AudioManager& am = AudioManager::GetInstance();
};
