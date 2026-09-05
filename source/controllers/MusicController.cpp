#include "MusicController.hpp"
#include "managers/MathManager.hpp"
#include <malloc.h>
#include <nds.h>
#include <stdint.h>
#include <string.h>
#include <string>

// Save and restore the IRQ mask around ring-buffer operations.
static inline int enterCritical()
{
    int oldIME = REG_IME;
    REG_IME = 0;
    return oldIME;
}
static inline void exitCritical(int oldIME)
{
    REG_IME = oldIME;
}

// Small stream copies avoid contending with video DMA transfers.
static inline void fastCopy(void* dst, const void* src, size_t size)
{
    memcpy(dst, src, size);
}

// Ring buffer shared by the main loop and the stream IRQ callback.
class SafeRingBuffer
{
  public:
    void init(u32 capacity)
    {
        free(buffer);
        buffer = (u8*)malloc(capacity);
        size = capacity;
        readPos = 0;
        writePos = 0;
        available = 0;
    }

    void destroy()
    {
        if (buffer)
        {
            free(buffer);
        }

        buffer = nullptr;
        size = 0;
        readPos = 0;
        writePos = 0;
        available = 0;
    }

    void reset()
    {
        int oldIME = enterCritical();
        readPos = 0;
        writePos = 0;
        available = 0;
        exitCritical(oldIME);
    }

    bool isValid() const
    {
        return buffer != nullptr;
    }

    u32 freeSpace() const
    {
        int oldIME = enterCritical();
        u32 result = size - available;
        exitCritical(oldIME);
        return result;
    }

    u32 usedSpace() const
    {
        int oldIME = enterCritical();
        u32 result = available;
        exitCritical(oldIME);
        return result;
    }

    // Write as much as the available capacity allows.
    u32 write(const u8* data, u32 bytes)
    {
        if (!buffer)
        {
            return 0;
        }

        int oldIME = enterCritical();

        u32 space = size - available;
        if (bytes > space)
        {
            bytes = space;
        }

        if (bytes > 0)
        {
            u32 firstPart = size - writePos;
            if (bytes <= firstPart)
            {
                fastCopy(&buffer[writePos], data, bytes);
                writePos = (writePos + bytes) % size;
            }
            else
            {
                fastCopy(&buffer[writePos], data, firstPart);
                u32 secondPart = bytes - firstPart;
                fastCopy(buffer, data + firstPart, secondPart);
                writePos = secondPart;
            }
            available += bytes;
        }

        exitCritical(oldIME);
        return bytes;
    }

    // Read as much as the available data allows.
    u32 read(u8* dest, u32 bytes)
    {
        if (!buffer)
        {
            return 0;
        }

        int oldIME = enterCritical();

        if (bytes > available)
        {
            bytes = available;
        }

        if (bytes > 0)
        {
            u32 firstPart = size - readPos;
            if (bytes <= firstPart)
            {
                fastCopy(dest, &buffer[readPos], bytes);
                readPos = (readPos + bytes) % size;
            }
            else
            {
                fastCopy(dest, &buffer[readPos], firstPart);
                u32 secondPart = bytes - firstPart;
                fastCopy(dest + firstPart, buffer, secondPart);
                readPos = secondPart;
            }
            available -= bytes;
        }

        exitCritical(oldIME);
        return bytes;
    }

  private:
    u8* buffer = nullptr;
    u32 size = 0;
    volatile u32 readPos = 0;
    volatile u32 writePos = 0;
    volatile u32 available = 0;
};

static FILE* s_audioFile = nullptr;
static bool s_isPaused = false;
static bool s_streamOpen = false;
static std::string s_currentFilePath = "";
static u32 s_elapsedSamples = 0;
static u32 s_loopStartSamples = 0;
static u32 s_loopEndSamples = 0;
static long s_loopStartOffset = 0;
static bool s_loopAtEOF = false;

static bool s_isVideoAudio = false;

// Two seconds of buffering at 32 kHz, 16-bit stereo.
static const u32 RING_BUFFER_SIZE = 256 * 1024;
static SafeRingBuffer s_videoRing;
static SafeRingBuffer s_musicRing;

static bool s_videoPrefillDone = false;
static bool s_musicPrefillDone = false;
static const u32 PREFILL_FRACTION_DIVISOR = 4; // start once 25% full

static u32 s_musicReadSamples = 0; // samples read from disk so far

static const u32 MUSIC_READ_CHUNK = 4096;
static const u32 MUSIC_FILL_BUDGET_PER_UPDATE = 16 * 1024;

// The callback only drains a ring buffer; file I/O stays in update().
static mm_word audio_stream_callback(mm_word length, mm_addr dest, mm_stream_formats format)
{
    size_t bytesReq = length * BYTES_PER_FRAME;
    u8* out = (u8*)dest;

    SafeRingBuffer* ring = s_isVideoAudio ? &s_videoRing : &s_musicRing;
    bool* prefillDone = s_isVideoAudio ? &s_videoPrefillDone : &s_musicPrefillDone;

    if (!ring->isValid() || s_isPaused)
    {
        memset(out, 0, bytesReq);
        return length;
    }

    if (!*prefillDone)
    {
        if (ring->usedSpace() >= RING_BUFFER_SIZE / PREFILL_FRACTION_DIVISOR)
        {
            *prefillDone = true;
        }
        else
        {
            memset(out, 0, bytesReq);
            if (s_isVideoAudio)
            {
                s_elapsedSamples += (bytesReq / BYTES_PER_FRAME);
            }

            return length;
        }
    }

    u32 bytesRead = ring->read(out, bytesReq);
    s_elapsedSamples += (bytesRead / BYTES_PER_FRAME);

    if (bytesRead < bytesReq)
    {
        memset(out + bytesRead, 0, bytesReq - bytesRead);
        *prefillDone = false; // rebuild safety margin before draining again
    }

    return length;
}

static void fillMusicBuffer()
{
    if (!s_audioFile || s_isVideoAudio || !s_musicRing.isValid())
    {
        return;
    }

    u32 budgetRemaining = MUSIC_FILL_BUDGET_PER_UPDATE;
    u8 scratch[MUSIC_READ_CHUNK];

    while (budgetRemaining > 0)
    {
        u32 space = s_musicRing.freeSpace();
        if (space < MUSIC_READ_CHUNK)
        {
            break;
        }

        size_t bytesRead = fread(scratch, 1, MUSIC_READ_CHUNK, s_audioFile);
        s_musicReadSamples += (bytesRead / BYTES_PER_FRAME);

        bool hitLoopPoint = (s_loopEndSamples > 0 && s_musicReadSamples >= s_loopEndSamples);
        bool hitEOF = (bytesRead < MUSIC_READ_CHUNK);

        if (bytesRead > 0)
        {
            s_musicRing.write(scratch, bytesRead);
        }

        if (hitLoopPoint || (hitEOF && s_loopAtEOF))
        {
            fseek(s_audioFile, s_loopStartOffset, SEEK_SET);
            s_musicReadSamples = s_loopStartSamples;
        }
        else if (hitEOF && !s_loopAtEOF)
        {
            break;
        }

        budgetRemaining -= (budgetRemaining >= MUSIC_READ_CHUNK) ? MUSIC_READ_CHUNK : budgetRemaining;
    }
}

MusicController* MusicController::instance = nullptr;

void MusicController::create()
{
    if (!instance)
    {
        instance = new MusicController();
    }
}

void MusicController::destroy()
{
    if (instance)
    {
        delete instance;
    }
    instance = nullptr;
}

MusicController* MusicController::getInstance()
{
    if (!instance)
    {
        create();
    }

    return instance;
}

void MusicController::init(const char* filePath, ae::q20_12_t loopStartSeconds, ae::q20_12_t loopEndSeconds)
{
    if (s_streamOpen && !s_isVideoAudio && s_currentFilePath == filePath)
    {
        if (s_isPaused)
        {
            resume();
        }

        return;
    }

    cleanup();

    s_audioFile = fopen(filePath, "rb");
    if (!s_audioFile)
    {
        printf("MusicController: failed to open %s\n", filePath);
        return;
    }

    static char s_fileIOBuffer[32 * 1024];
    setvbuf(s_audioFile, s_fileIOBuffer, _IOFBF, sizeof(s_fileIOBuffer));

    s_elapsedSamples = 0;
    s_musicReadSamples = 0;
    s_isPaused = false;
    s_isVideoAudio = false;
    s_currentFilePath = filePath;

    s_loopStartSamples = MathManager::GetInstance().secondsToSamples(loopStartSeconds, AUDIO_SAMPLE_RATE);
    s_loopStartOffset = s_loopStartSamples * BYTES_PER_FRAME;

    if (loopEndSeconds == aegis::q20_12_t{-1.0})
    {
        s_loopAtEOF = true;
        s_loopEndSamples = 0;
    }
    else if (loopEndSeconds > aegis::q20_12_t{0})
    {
        s_loopEndSamples = MathManager::GetInstance().secondsToSamples(loopEndSeconds, AUDIO_SAMPLE_RATE);
    }
    else
    {
        s_loopEndSamples = 0;
    }

    s_musicRing.init(RING_BUFFER_SIZE);
    s_musicPrefillDone = false;

    // Prefill enough data to keep the stream callback from outputting silence.
    const u32 prefillTarget = RING_BUFFER_SIZE / PREFILL_FRACTION_DIVISOR;
    while (s_musicRing.usedSpace() < prefillTarget)
    {
        u32 before = s_musicRing.usedSpace();
        fillMusicBuffer();
        if (s_musicRing.usedSpace() == before)
        {
            break; // EOF on a short/non-looping track - stop spinning
        }
    }

    mm_stream stream;
    stream.timer = MM_TIMER0;
    stream.sampling_rate = AUDIO_SAMPLE_RATE;
    stream.buffer_length = 2048;
    stream.callback = audio_stream_callback;
    stream.format = MM_STREAM_16BIT_STEREO;
    stream.manual = true;
    mmStreamOpen(&stream);
    s_streamOpen = true;

    mmStreamUpdate();
}

void MusicController::initVideoAudio()
{
    cleanup();

    s_videoRing.init(RING_BUFFER_SIZE);
    s_videoPrefillDone = false;

    s_elapsedSamples = 0;
    s_isPaused = false;
    s_isVideoAudio = true;

    mm_stream stream;
    stream.timer = MM_TIMER0;
    stream.sampling_rate = AUDIO_SAMPLE_RATE;
    stream.buffer_length = 2048;
    stream.callback = audio_stream_callback;
    stream.format = MM_STREAM_16BIT_STEREO;
    stream.manual = true;
    mmStreamOpen(&stream);
    s_streamOpen = true;

    mmStreamUpdate();
}

void MusicController::pushVideoAudio(const u8* data, size_t size)
{
    if (!s_isVideoAudio || !s_videoRing.isValid())
    {
        return;
    }

    s_videoRing.write(data, (u32)size);
}

ae::q20_12_t MusicController::getVideoTime()
{
    return ae::q20_12_t{s_elapsedSamples} / ae::q20_12_t{AUDIO_SAMPLE_RATE};
}

void MusicController::update()
{
    if (!s_isVideoAudio)
    {
        fillMusicBuffer();
    }

    if (s_streamOpen)
    {
        mmStreamUpdate();
    }
}

void MusicController::pause()
{
    s_isPaused = true;
}

void MusicController::resume()
{
    s_isPaused = false;
}

void MusicController::loadSFX(mm_word effectID)
{
    mmLoadEffect(effectID);
}

mm_sfxhand MusicController::playSFX(mm_word effectID, int volume, int panning)
{
    mm_sound_effect effect;
    effect.id = effectID;
    effect.rate = (int)(1.0f * (1 << 10));
    effect.handle = 0;
    effect.volume = volume;
    effect.panning = panning;
    return mmEffectEx(&effect);
}

void MusicController::stopSFX(mm_sfxhand handle)
{
    if (handle != 0)
    {
        mmEffectCancel(handle);
    }
}

void MusicController::cleanup()
{
    if (s_streamOpen)
    {
        s_isVideoAudio = false;
        mmStreamClose();
        s_streamOpen = false;
    }
    if (s_audioFile)
    {
        fclose(s_audioFile);
        s_audioFile = nullptr;
    }
    s_currentFilePath = "";

    s_videoRing.destroy();
    s_musicRing.destroy();
    s_videoPrefillDone = false;
    s_musicPrefillDone = false;
}
