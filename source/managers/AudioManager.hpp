/**
 * @file AudioManager.hpp
 * @brief Manager for hardware specific audio functions
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include "types/AudioTypes.hpp"
#include <aegis/manager.hpp>
#include <maxmod9.h>
#include <string>

class AudioManager : public ae::Manager, public ae::Singleton<AudioManager>
{
  public:
    void Init() override;

    void Process() override;

    void Shutdown() override;

    // TODO: add doxygen
    void registerAudio(std::string path, ae::q20_12_t loopStartTime, ae::q20_12_t loopEndTime);
    void playAudio();
    void pauseAudio();
    void stopAudio();

    void registerSFX(SFX sfx);
    void playSFX(SFX sfx, int volume, int panning);
    void stopSFX();

  private:
    friend class Singleton<AudioManager>;
    AudioManager() = default;

    qoaplay_desc* qp = nullptr;
    bool isLoopingEnabled = false;
    int startFrame = 0;
    int endFrame = 0;

    // TODO: add doxygen to all functions
    ae::q20_12_t getDuration(); // unused
    ae::q20_12_t getTime();     // unused
    int getFrame();
    void seekFrame(int frame);
    void setLoop(ae::q20_12_t startTime, ae::q20_12_t endTime);
    void loop();
    void rewind();
    uint32_t decodeFrame();
    uint32_t decode(short* sample_data, int num_samples);

    // audio stream processing
    static mm_word audioCallback(mm_word length, mm_addr dest, mm_stream_formats format);
    mm_word processStream(mm_word length, mm_addr dest, mm_stream_formats format);

    // sfx helper
    int fetchSFXSampleId(SFX sfx);
};
