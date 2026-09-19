#pragma once

#include "types/StateTypes.hpp"
#include <aegis/ndsTypes.hpp>
#include <aegis/types.hpp>

#include <nds.h>
#include <stdio.h>
#include <string>

#define FRAMES_TO_BUFFER 15
#define READS_PER_UPDATE 3 // max frame reads per update() (reads stop early if a frame is due)

// Max audio payload in one frame chunk. Must match MAX_AUDIO_CHUNK in video2vid.py.
#define AUDIO_CHUNK_MAX 16384

// Video frames are read from the SD card in slices of this size; the audio
// stream is fed between slices so a long read can't underrun it.
#define VIDEO_READ_SLICE 16384

// Length of the maxmod stream buffer in ms. Bigger = more tolerance for slow reads.
#define VIDEO_AUDIO_STREAM_MS 100

// Fine sync tuning in ms. Positive delays the picture (use if picture leads sound),
// negative advances it (use if picture lags sound).
#define VIDEO_SYNC_OFFSET_MS 0

struct VideoAudio; // internal QOA player, defined in VideoController.cpp

/**
 * @brief Streams video frames and multiplexed QOA audio from a single .vid file.
 *
 * Audio is decoded and streamed by a private QOA/maxmod player inside this
 * class (AudioManager is not used for playback). The audio clock drives frame
 * pacing; files without audio use a vblank clock instead.
 */
class VideoController
{
  public:
    /** @brief Creates the singleton instance if it does not exist. */
    static void create();
    /** @brief Destroys the singleton instance and releases video resources. */
    static void destroy();
    /** @return The process-wide video controller instance. */
    static VideoController* getInstance();

    /**
     * @brief Opens and prepares a video stream.
     * @param iFileName Video filename relative to the data video directory.
     * @param iFps Fallback frame rate when the file has no header.
     * @param iNextState View state returned after playback completes.
     */
    void init(std::string iFileName, ae::q20_12_t iFps, ViewState iNextState);

    /** @brief Advances playback and renders the next available frame. */
    ViewState update();

    /** @brief Stops playback and releases the file, audio stream and frame buffer. */
    void cleanup();

  private:
    VideoController() = default;
    ~VideoController()
    {
        cleanup();
    }
    static VideoController* instance;

    ViewState nextState = ViewState::DEFAULT;
    int fpsInt = 24;

    FILE* videoFile = nullptr;
    bool fileEOF = false;
    int currentFrame = 0;
    int bg = -1;

    u8* ramBuffer = nullptr;
    int readIndex = 0;
    int writeIndex = 0;
    int framesAvailable = 0;

    // dynamic video variables
    u16 frameW = 0;
    u16 frameH = 0;
    u8 bpp = 0;
    u32 frameSize = 0;
    u32 bufferSize = 0;

    // multiplexed audio
    VideoAudio* aud = nullptr; // non-null while the internal audio player is active
    bool haveChunkHeader = false;
    u32 pendingAudioSize = 0;
    u32 silentVblanks = 0; // pacing clock when there is no audio
    bool waitedThisUpdate = false;

    /** @brief Reads one audio chunk (into the audio ring) and one video frame. @return true if a frame was read. */
    bool refillBuffer();
    void pumpAudio();
    void setEOF();
    int clockFrame() const;
    void stopInternalAudio();
};
