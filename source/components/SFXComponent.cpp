#include "SFXComponent.hpp"

void SFXComponent::Destroy()
{
    stopSFX();
}

void SFXComponent::registerSFX(SFX sfx)
{
    am.registerSFX(sfx);
}

void SFXComponent::playSFX(SFX sfx, int volume, int panning)
{
    stopSFX();
    am.playSFX(sfx, volume, panning);
}

void SFXComponent::stopSFX()
{
    am.stopSFX();
}
