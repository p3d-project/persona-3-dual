#include "MusicComponent.hpp"
#include "core/globals.hpp"
#include "types/aeTypes.hpp"

void MusicComponent::Destroy()
{
    stopMusic();
}

void MusicComponent::registerMusic(AudioType type, int audioId)
{
    std::string path = fatBasePath + "music/" + getAudioFilepath(type, audioId) + ".qoa";
    ae::q20_12_t loopStartTime = getLoopStart(type, audioId);
    ae::q20_12_t loopEndTime = getLoopEnd(type, audioId);
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
