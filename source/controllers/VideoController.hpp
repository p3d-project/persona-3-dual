#pragma once

#include "types/StateTypes.hpp"
#include <aegis/ndsTypes.hpp>
#include <aegis/types.hpp>

#include <nds.h>
#include <stdio.h>
#include <string>

/** @brief Number of decoded video frames held in the RAM ring buffer. */
constexpr uint8_t FRAMES_TO_BUFFER = 15;

/** @brief Maximum frame reads per update(). Reads stop early if a frame is already due. */
constexpr uint8_t READS_PER_UPDATE = 3;

/** @brief Maximum audio payload (bytes) in one per-frame chunk. Must match MAX_AUDIO_CHUNK in video2vid.py. */
constexpr uint16_t AUDIO_CHUNK_MAX = 16384;

/**
 * @brief Size (bytes) of each SD card read slice when loading a video frame.
 *
 * The audio stream is fed between slices so a long read cannot underrun it.
 */
constexpr uint16_t VIDEO_READ_SLICE = 16384;

/** @brief Length of the maxmod audio stream buffer in ms. Larger tolerates slower SD reads. */
constexpr uint8_t VIDEO_AUDIO_STREAM_MS = 100;

/**
 * @brief Fine A/V sync offset in ms.
 *
 * Positive delays the picture (use if the picture leads the sound),
 * negative advances it (use if the picture lags the sound).
 */
constexpr int8_t VIDEO_SYNC_OFFSET_MS = 0;

/** @brief Internal QOA/maxmod audio player, defined in VideoController.cpp. */
struct VideoAudio;

/**
 * @brief Streams video frames and multiplexed QOA audio from a single .vid file.
 *
 * Audio is decoded and streamed by a private QOA/maxmod player inside this
 * class (AudioManager is not used for playback). The audio clock drives frame
 * pacing; files without audio use a vblank clock instead.
 *
 * The controller is a singleton and owns its file handle, frame buffer and
 * audio player.
 */
class VideoController
{
  public:
    /** @brief Creates the singleton instance if it does not exist. */
    static void create();

    /** @brief Destroys the singleton instance and releases all video resources. */
    static void destroy();

    /** @return The process-wide video controller instance (created on demand). */
    static VideoController* getInstance();

    /**
     * @brief Opens and prepares a video stream.
     *
     * Reads the .vid header, configures the display background, allocates the
     * frame buffer, starts the internal audio player (if the file has audio)
     * and prefills the buffers. Any previous playback is released first.
     *
     * @param iFileName Video filename relative to the data video directory.
     * @param iFps Fallback frame rate when the file has no header.
     * @param iNextState View state returned by update() after playback completes.
     */
    void init(std::string iFileName, ae::q20_12_t iFps, ViewState iNextState);

    /**
     * @brief Advances playback: pumps audio, displays the next due frame and reads ahead.
     * @return ViewState::KEEP_CURRENT while playing, or the state passed to init() once finished.
     */
    ViewState update();

    /** @brief Stops playback and releases the file, audio stream and frame buffer. */
    void cleanup();

  private:
    VideoController() = default;
    ~VideoController()
    {
        cleanup();
    }
    static VideoController* instance; ///< Singleton instance.

    ViewState nextState = ViewState::DEFAULT; ///< State returned when playback ends.
    uint8_t fpsInt = 24;                      ///< Frame rate in whole fps (from the header or fallback).

    FILE* videoFile = nullptr; ///< Open .vid file handle.
    bool fileEOF = false;      ///< True once the file (or a read) is exhausted.
    int64_t currentFrame = 0;  ///< Index of the next frame to display.
    int8_t bg = -1;            ///< Background id returned by bgInit().

    uint8_t* ramBuffer = nullptr; ///< Ring buffer holding FRAMES_TO_BUFFER decoded frames.
    uint16_t readIndex = 0;       ///< Ring slot of the next frame to display.
    uint16_t writeIndex = 0;      ///< Ring slot the next frame is read into.
    uint16_t framesAvailable = 0; ///< Number of frames currently buffered.

    // dynamic video variables
    uint16_t frameW = 0;     ///< Frame width in pixels.
    uint16_t frameH = 0;     ///< Frame height in pixels.
    uint8_t bpp = 0;         ///< Bytes per pixel (1 = 8-bit paletted, 2 = 16-bit).
    uint32_t frameSize = 0;  ///< Size of one frame in bytes.
    uint32_t bufferSize = 0; ///< Total size of the frame ring buffer in bytes.

    // multiplexed audio
    VideoAudio* aud = nullptr;     ///< Internal audio player; non-null while audio is active.
    bool haveChunkHeader = false;  ///< True if the current chunk's audio size has been read.
    uint32_t pendingAudioSize = 0; ///< Audio bytes still to be read for the current chunk.
    uint32_t silentVblanks = 0;    ///< Pacing clock (vblanks) used when there is no audio.
    bool waitedThisUpdate = false; ///< True if update() already waited for vblank.

    /**
     * @brief Reads one audio chunk (into the audio ring) and one video frame.
     *
     * Stops without reading if the frame buffer or audio ring is full.
     *
     * @return true if a video frame was read.
     */
    bool refillBuffer();

    /** @brief Feeds the maxmod stream; a no-op when no audio is active. */
    void pumpAudio();

    /** @brief Marks end of file and tells the audio player no more data will arrive. */
    void setEOF();

    /**
     * @brief Computes the frame that should be on screen now.
     * @return Expected frame index from the audio clock, or from the vblank clock if there is no audio.
     */
    int clockFrame() const;

    /** @brief Shuts down and frees the internal audio player. */
    void stopInternalAudio();
};
