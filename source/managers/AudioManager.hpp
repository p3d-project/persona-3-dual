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

    /**
     * @brief Sets up the audio track to be played
     *
     * @param path the file path to the audio track
     * @param loopStartTime the loop start time in seconds
     * @param loopEndTime the loop end time in seconds
     */
    void registerAudio(std::string path, ae::q20_12_t loopStartTime, ae::q20_12_t loopEndTime);

    /**
     * @brief Play the currently registered audio, if paused
     */
    void playAudio();

    /**
     * @brief Pause the currently registered audio, if playing
     */
    void pauseAudio();

    /**
     * @brief Deregister the currently registered audio
     */
    void stopAudio();

    /**
     * @brief Register the SFX to be played
     *
     * @param sfx the SFX to register
     */
    void registerSFX(SFX sfx);

    /**
     * @brief Play the specified registered SFX
     *
     * @param sfx the SFX to play
     * @param volume the volume of the SFX. Range is from 0-127
     * @param panning the direction to play the sound effect (left to right). The range is 0-127 (64 plays both left and right equally)
     */
    void playSFX(SFX sfx, int volume, int panning);

    /**
     * @brief Stop all currently playing SFXs
     */
    void stopSFX();

  private:
    friend class Singleton<AudioManager>;
    AudioManager() = default;

    qoaplay_desc* qp = nullptr;
    bool isLoopingEnabled = false;
    int startFrame = 0;
    int endFrame = 0;

    /**
     * @brief Get the total duration of the audio
     *
     * @return ae::q20_12_t the duration in seconds
     */
    ae::q20_12_t getDuration(); // unused

    /**
     * @brief Get the current time of the audio
     *
     * @return ae::q20_12_t the time in seconds
     */
    ae::q20_12_t getTime();     // unused

    /**
     * @brief Get the current audio frame
     *
     * @return int the audio frame
     */
    int getFrame();

    /**
     * @brief Move to the specified audio frame
     *
     * @param frame the audio frame to move to
     */
    void seekFrame(int frame);

    /**
     * @brief Set the audio looping points
     *
     * @param startTime the loop start time in seconds
     * @param endTime the loop end time in seconds
     */
    void setLoop(ae::q20_12_t startTime, ae::q20_12_t endTime);

    /**
     * @brief Looping logic
     */
    void loop();

    /**
     * @brief Move to the start of the audio track
     */
    void rewind();

    /**
     * @brief Decode the current audio frame
     *
     * @return uint32_t the number of samples
     */
    uint32_t decodeFrame();

    /**
     * @brief Decode the given audio sample based on the current frame
     *
     * @param sample_data the sample data
     * @param num_samples the number of samples
     * @return uint32_t the frame length
     */
    uint32_t decode(short* sample_data, int num_samples);

    /**
     * @brief Callback for maxmod audio streaming, acting as a wrapper for processStream
     *
     * @param length the audio word length
     * @param dest the audio destination address
     * @param format the audio stream format
     * @return mm_word the audio data to play
     */
    static mm_word audioCallback(mm_word length, mm_addr dest, mm_stream_formats format);

    /**
     * @brief Processes the next audio data to play
     *
     * @param length the audio word length
     * @param dest the audio destination address
     * @param format the audio stream format
     * @return mm_word the decoded audio data
     */
    mm_word processStream(mm_word length, mm_addr dest, mm_stream_formats format);

    /**
     * @brief Fetch the SFX id based on the enum value
     *
     * @param sfx the SFX enum
     * @return int the SFX id
     */
    int fetchSFXSampleId(SFX sfx);
};
