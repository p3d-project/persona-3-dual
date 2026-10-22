#include "MusicComponent.hpp"
#include "core/globals.hpp"
#include "core/logger.hpp"
#include "types/aeTypes.hpp"

void MusicComponent::Destroy()
{
    stopMusic();
}

void MusicComponent::registerMusic(AudioType type, int audioId)
{
    std::string filename = getAudioFilepath(type, audioId);
    if (filename.empty())
    {
        logMessage(LogSeverity::WARNING, "Empty filename for audio type/id");
        return;
    }
    std::string path = fatBasePath + "music/" + filename + ".qoa";
    ae::q20_12_t loopStartTime = getLoopStart(type, audioId);
    ae::q20_12_t loopEndTime = getLoopEnd(type, audioId);
    logMessage(LogSeverity::DEBUG, "Registering music: " + filename);
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
