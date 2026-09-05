#pragma once
#include <aegis/ndsTypes.hpp>
#include <aegis/types.hpp>
#include <maxmod9.h>
#include <nds.h>
#include <stdio.h>

#define AUDIO_SAMPLE_RATE 32000
#define AUDIO_CHANNELS 2
#define BYTES_PER_SAMPLE 2
#define BYTES_PER_FRAME (AUDIO_CHANNELS * BYTES_PER_SAMPLE)

/**
 * @brief Streams music, video audio, and sound effects through Maxmod.
 *
 * Music and video audio are buffered outside the stream callback so file I/O
 * remains in the main update loop. The controller is a process-wide singleton.
 */
class MusicController
{
  public:
    /** @brief Creates the singleton instance if it does not exist. */
    static void create();
    /** @brief Destroys the singleton instance and releases active audio state. */
    static void destroy();
    /** @return The process-wide music controller instance. */
    static MusicController* getInstance();

    /**
     * @brief Starts streaming a music file.
     * @param filePath Path to the PCM stream.
     * @param loopStartSeconds Loop start position in seconds.
     * @param loopEndSeconds Loop end position in seconds, or -1 to loop at EOF.
     */
    void init(const char* filePath,
              ae::q20_12_t loopStartSeconds = ae::q20_12_t{0},
              ae::q20_12_t loopEndSeconds = ae::q20_12_t{-1.0});
    /** @brief Fills audio buffers and services the Maxmod stream. */
    void update();
    /** @brief Pauses the active stream. */
    void pause();
    /** @brief Resumes the active stream. */
    void resume();

    /** @brief Starts the audio stream used by video playback. */
    void initVideoAudio();

    /**
     * @brief Queues decoded video audio for playback.
     * @param data Interleaved stereo 16-bit PCM data.
     * @param size Number of bytes in @p data.
     */
    void pushVideoAudio(const u8* data, size_t size);

    /** @return Elapsed video audio time in seconds. */
    ae::q20_12_t getVideoTime();

    /** @brief Loads a sound effect into Maxmod. */
    void loadSFX(mm_word effectID);
    /**
     * @brief Starts a sound effect.
     * @param effectID Maxmod sound effect identifier.
     * @param volume Playback volume from 0 to 255.
     * @param panning Stereo panning from 0 to 255.
     * @return Maxmod's handle for the playing effect.
     */
    mm_sfxhand playSFX(mm_word effectID, int volume = 255, int panning = 128);
    /** @brief Stops a previously started sound effect. */
    void stopSFX(mm_sfxhand handle);

    /** @brief Stops audio, closes files, and releases stream buffers. */
    void cleanup();

  private:
    MusicController() = default;
    /** @brief Cleans up active audio state before destruction. */
    ~MusicController()
    {
        cleanup();
    }
    static MusicController* instance;

    /** @return The byte offset of the first audio sample in a stream. */
    long getAudioStartOffset(FILE* file);
};
