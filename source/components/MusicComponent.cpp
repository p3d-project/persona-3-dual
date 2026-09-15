#include "MusicComponent.hpp"

void MusicComponent::Destroy()
{
    stopMusic();
}

void MusicComponent::registerMusic(std::string path, ae::q20_12_t loopStartTime, ae::q20_12_t loopEndTime)
{
    am.registerAudio(path, loopStartTime, loopEndTime);
}

void MusicComponent::playMusic()
{
    am.playAudio();
}

void MusicComponent::pauseMusic()
{
    am.pauseAudio();
}

void MusicComponent::stopMusic()
{
    am.stopAudio();
}
