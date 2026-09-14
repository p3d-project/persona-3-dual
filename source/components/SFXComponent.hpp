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
    void registerSFX(SFX sfx);
    void playSFX(SFX sfx, int volume, int panning);
    void stopSFX();

  protected:
    void SubmitToManager() override
    {
    }

  private:
    AudioManager& am = AudioManager::GetInstance();
};
