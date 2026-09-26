#include "VideoController.hpp"
#include "core/globals.hpp"
#include "managers/AudioManager.hpp"
#include <malloc.h>
#include <maxmod9.h>
#include <nds.h>
#include <new>
#include <p3d-qoa>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

constexpr uint8_t VID_AUDIO_NONE = 0;
constexpr uint8_t VID_AUDIO_QOA = 1;

namespace
{
constexpr uint32_t RING_SIZE = 32768; // compressed audio ring, must be a power of two
constexpr uint32_t RING_MASK = RING_SIZE - 1;
static_assert(AUDIO_CHUNK_MAX <= RING_SIZE, "audio ring must hold at least one audio chunk");
} // namespace

// ===========================================================================
// Internal QOA player: compressed ring -> QOA decode -> maxmod stream
// ===========================================================================
static mm_word videoAudioCallback(mm_word length, mm_addr dest, mm_stream_formats);
static VideoAudio* g_audio = nullptr;

struct VideoAudio
{
    uint8_t* ring = nullptr;
    uint32_t head = 0;
    uint32_t count = 0;

    short* pcm = nullptr; // one decoded QOA frame, interleaved
    uint32_t pcmPos = 0;
    uint32_t pcmLen = 0;

    uint8_t* scratch = nullptr; // for frames that wrap around the ring end
    uint32_t scratchSize = 0;

    qoa_desc desc;
    uint32_t rate = 0;
    uint32_t channels = 0;
    uint32_t streamSamples = 0;

    uint32_t contentSamples = 0; // real audio samples handed to maxmod
    uint32_t freeRunSamples = 0; // silence after the file ended (keeps the clock running)
    uint32_t silentRun = 0;      // consecutive fully-silent samples
    bool fileEnded = false;
    bool fault = false;
    bool streamOpen = false;

    bool open(uint32_t sampleRate, uint32_t numChannels)
    {
        rate = sampleRate;
        channels = numChannels;
        memset(&desc, 0, sizeof(desc));
        desc.channels = numChannels;
        desc.samplerate = sampleRate;

        scratchSize = qoa_max_frame_size(&desc);
        ring = (uint8_t*)malloc(RING_SIZE);
        pcm = (short*)malloc(QOA_FRAME_LEN * numChannels * sizeof(short));
        scratch = (uint8_t*)malloc(scratchSize);
        if (!ring || !pcm || !scratch)
        {
            close();
            return false;
        }

        uint32_t s = (rate * VIDEO_AUDIO_STREAM_MS / 1000) & ~15u;
        streamSamples = s < 512 ? 512 : (s > 8192 ? 8192 : s);
        return true;
    }

    bool start()
    {
        mm_stream stream;
        stream.sampling_rate = rate;
        stream.buffer_length = streamSamples;
        stream.callback = videoAudioCallback;
        stream.format = (channels == 1) ? MM_STREAM_16BIT_MONO : MM_STREAM_16BIT_STEREO;
        stream.timer = MM_TIMER0;
        stream.manual = true;

        g_audio = this;
        mmStreamOpen(&stream);
        streamOpen = true;
        mmStreamUpdate(); // prime the buffer
        mmResume();       // harmless if not paused; covers a leftover mmPause()
        return true;
    }

    void close()
    {
        if (streamOpen)
        {
            mmStreamClose();
            streamOpen = false;
        }
        g_audio = nullptr;
        free(ring);
        free(pcm);
        free(scratch);
        ring = nullptr;
        pcm = nullptr;
        scratch = nullptr;
    }

    void pump()
    {
        if (streamOpen)
        {
            mmStreamUpdate();
        }
    }

    uint32_t space() const
    {
        return RING_SIZE - count;
    }

    // Reads `size` bytes from f directly into the ring (handles wrap-around).
    bool readFrom(FILE* f, uint32_t size)
    {
        uint32_t tail = (head + count) & RING_MASK;
        uint32_t first = RING_SIZE - tail;
        if (first > size)
        {
            first = size;
        }
        if (fread(ring + tail, 1, first, f) != first)
        {
            return false;
        }
        if (size > first && fread(ring, 1, size - first, f) != size - first)
        {
            return false;
        }
        count += size;
        return true;
    }

    // Decodes the next whole QOA frame from the ring into pcm.
    bool decodeFrame()
    {
        pcmPos = 0;
        pcmLen = 0;
        if (count < 8)
        {
            return false;
        }

        uint8_t h[8];
        for (uint32_t i = 0; i < 8; i++)
        {
            h[i] = ring[(head + i) & RING_MASK];
        }
        uint32_t size = ((uint32_t)h[6] << 8) | h[7];
        if (h[0] != channels || size < 8 || size > scratchSize)
        {
            fault = true;
            return false;
        }
        if (count < size)
        {
            return false; // underrun
        }

        const uint8_t* src;
        uint32_t toEnd = RING_SIZE - head;
        if (size <= toEnd)
        {
            src = ring + head;
        }
        else
        {
            memcpy(scratch, ring + head, toEnd);
            memcpy(scratch + toEnd, ring, size - toEnd);
            src = scratch;
        }

        uint32_t len = 0;
        qoa_decode_frame(src, size, &desc, pcm, &len);
        head = (head + size) & RING_MASK;
        count -= size;

        if (len == 0 || len > QOA_FRAME_LEN)
        {
            fault = true;
            return false;
        }
        pcmLen = len;
        return true;
    }

    // Playback position in samples (what is audible, not just decoded).
    uint32_t clock() const
    {
        s32 latency = (s32)streamSamples + (VIDEO_SYNC_OFFSET_MS * (s32)rate) / 1000;
        s32 played = (s32)(contentSamples + freeRunSamples) - latency;
        return played > 0 ? (uint32_t)played : 0;
    }

    bool failed() const
    {
        return fault || (!fileEnded && silentRun > rate * 2);
    }
};

static mm_word videoAudioCallback(mm_word length, mm_addr dest, mm_stream_formats)
{
    VideoAudio* a = g_audio;
    if (!a)
    {
        return 0;
    }

    short* out = (short*)dest;
    const uint32_t ch = a->channels;
    uint32_t done = 0;

    while (done < length)
    {
        if (a->pcmPos >= a->pcmLen && !a->decodeFrame())
        {
            break;
        }
        uint32_t n = a->pcmLen - a->pcmPos;
        if (n > length - done)
        {
            n = length - done;
        }
        const size_t copySamples = static_cast<size_t>(n) * static_cast<size_t>(ch);
        const size_t copyBytes = copySamples * sizeof(short);
        memcpy(out + done * ch, a->pcm + a->pcmPos * ch, copyBytes);
        a->pcmPos += n;
        done += n;
    }

    a->contentSamples += done;

    if (done < length)
    {
        uint32_t pad = length - done;
        const size_t padSamples = static_cast<size_t>(pad) * static_cast<size_t>(ch);
        const size_t padBytes = padSamples * sizeof(short);
        memset(out + done * ch, 0, padBytes);
        if (a->fileEnded)
        {
            a->freeRunSamples += pad; // no more audio will come: let time pass
        }
        a->silentRun = done ? 0 : a->silentRun + pad;
    }
    else
    {
        a->silentRun = 0;
    }
    return length;
}

// ===========================================================================
// VideoController
// ===========================================================================
VideoController* VideoController::instance = nullptr;

void VideoController::create()
{
    if (instance == nullptr)
    {
        instance = new VideoController();
    }
}

void VideoController::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

VideoController* VideoController::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

void VideoController::pumpAudio()
{
    if (aud)
    {
        aud->pump();
    }
}

void VideoController::stopInternalAudio()
{
    if (aud)
    {
        aud->close();
        delete aud;
        aud = nullptr;
    }
}

void VideoController::setEOF()
{
    fileEOF = true;
    if (aud)
    {
        aud->fileEnded = true;
    }
}

int VideoController::clockFrame() const
{
    if (aud)
    {
        return (int)(((u64)aud->clock() * (uint32_t)fpsInt) / aud->rate);
    }
    return (int)(((u64)silentVblanks * (uint32_t)fpsInt) / 60);
}

void VideoController::init(std::string iFileName, ae::q20_12_t iFps, ViewState iNextState)
{
    // Make repeated initialization safe.
    stopInternalAudio();
    if (ramBuffer != nullptr)
    {
        free(ramBuffer);
        ramBuffer = nullptr;
    }
    if (videoFile != nullptr)
    {
        fclose(videoFile);
        videoFile = nullptr;
    }

    fileEOF = false;
    framesAvailable = 0;
    readIndex = 0;
    writeIndex = 0;
    currentFrame = 0;
    silentVblanks = 0;
    haveChunkHeader = false;
    pendingAudioSize = 0;

    nextState = iNextState;
    fpsInt = (int)(iFps.raw_value() >> 12); // fallback if no header exists

    std::string videoPath = fatBasePath + "video/" + iFileName;

    videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankD(VRAM_D_MAIN_BG_0x06020000);
    vramSetBankC(VRAM_C_SUB_BG);

    // maxmod supports one stream at a time: release any music stream first.
    AudioManager::GetInstance().stopAudio();

    videoFile = fopen(videoPath.c_str(), "rb");
    if (!videoFile)
    {
        consoleDemoInit();
        printf("ERR: %s", videoPath.c_str());
        while (1)
        {
            swiWaitForVBlank();
        }
    }

    // Read the optional dynamic video header.
    uint8_t header[16];
    size_t hRead = fread(header, 1, 16, videoFile);
    uint8_t audioCodec = VID_AUDIO_NONE;
    uint32_t audioRate = 0;
    uint32_t audioChannels = 0;

    if (hRead == 16 && memcmp(header, "VID\0", 4) == 0)
    {
        // byte-wise reads avoid unaligned access crashes on the ARM9
        fpsInt = header[4] | (header[5] << 8);
        bpp = header[6];
        audioCodec = header[7];
        frameW = header[8] | (header[9] << 8);
        frameH = header[10] | (header[11] << 8);
        audioRate = header[12] | (header[13] << 8);
        audioChannels = header[14];
    }
    else
    {
        // Support legacy raw 16-bit 256x192 video files.
        fseek(videoFile, 0, SEEK_SET);
        bpp = 2;
        frameW = 256;
        frameH = 192;
    }

    if (fpsInt <= 0)
    {
        fpsInt = 24;
    }
    if ((bpp != 1 && bpp != 2) || frameW == 0 || frameH == 0)
    {
        consoleDemoInit();
        printf("ERR: bad video header");
        while (1)
        {
            swiWaitForVBlank();
        }
    }

    frameSize = (uint32_t)frameW * frameH * bpp;
    bufferSize = frameSize * FRAMES_TO_BUFFER;

    // Select the background format from the parsed bit depth.
    if (bpp == 1)
    {
        bg = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

        uint16_t palette[256];
        size_t palRead = fread(palette, 2, 256, videoFile);
        if (palRead != 256)
        {
            consoleDemoInit();
            printf("ERR: palette read failed");
            while (1)
            {
                swiWaitForVBlank();
            }
        }

        for (int i = 0; i < 256; i++)
        {
            BG_PALETTE[i] = palette[i];
        }
    }
    else
    {
        bgExtPaletteEnable();
        bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    }

    // Clear the first frame target before playback.
    dmaFillWords(0, bgGetGfxPtr(bg), frameSize);

    ramBuffer = (uint8_t*)memalign(32, bufferSize);
    if (!ramBuffer)
    {
        consoleDemoInit();
        printf("ERR: malloc failed");
        while (1)
        {
            swiWaitForVBlank();
        }
    }

    // Set up the internal audio player (falls back to silent playback on failure).
    if (audioCodec == VID_AUDIO_QOA && audioRate > 0 && (audioChannels == 1 || audioChannels == 2))
    {
        aud = new (std::nothrow) VideoAudio();
        if (aud && !aud->open(audioRate, audioChannels))
        {
            delete aud;
            aud = nullptr;
        }
    }

    // Prefill video (and the first audio chunks) before the stream starts.
    for (int i = 0; i < 3; i++)
    {
        if (!refillBuffer())
        {
            break;
        }
    }

    if (aud)
    {
        aud->start();
    }
}

bool VideoController::refillBuffer()
{
    if (fileEOF || framesAvailable >= FRAMES_TO_BUFFER)
    {
        return false;
    }

    // Each frame starts with a uint32_t audio size.
    if (!haveChunkHeader)
    {
        uint32_t size = 0;
        if (fread(&size, 4, 1, videoFile) != 1)
        {
            setEOF();
            return false;
        }
        pendingAudioSize = size;
        haveChunkHeader = true;
    }

    if (pendingAudioSize > 0)
    {
        if (aud && pendingAudioSize <= AUDIO_CHUNK_MAX)
        {
            if (aud->space() < pendingAudioSize)
            {
                return false; // backpressure: wait for the audio to drain
            }
            if (!aud->readFrom(videoFile, pendingAudioSize))
            {
                setEOF();
                return false;
            }
        }
        else if (fseek(videoFile, (long)pendingAudioSize, SEEK_CUR) != 0)
        {
            // silent/legacy file (or oversized chunk): skip the payload
            setEOF();
            return false;
        }
        pendingAudioSize = 0;
    }
    haveChunkHeader = false;

    // Read the video frame in slices, feeding the audio stream in between.
    uint8_t* dest = &ramBuffer[writeIndex * frameSize];
    uint32_t got = 0;
    while (got < frameSize)
    {
        uint32_t n = frameSize - got;
        if (n > VIDEO_READ_SLICE)
        {
            n = VIDEO_READ_SLICE;
        }
        size_t r = fread(dest + got, 1, n, videoFile);
        got += (uint32_t)r;
        if (r != n)
        {
            break;
        }
        pumpAudio();
    }

    if (got != frameSize)
    {
        setEOF();
        return false;
    }

    DC_FlushRange(dest, frameSize);
    writeIndex = (writeIndex + 1) % FRAMES_TO_BUFFER;
    framesAvailable++;
    return true;
}

ViewState VideoController::update()
{
    pumpAudio();
    waitedThisUpdate = false;

    // Audio faulted or went silent before the file ended: continue on the vblank clock.
    if (aud && aud->failed())
    {
        stopInternalAudio();
        silentVblanks = (uint32_t)(((u64)currentFrame * 60) / (uint32_t)fpsInt);
    }

    if (!aud)
    {
        swiWaitForVBlank();
        waitedThisUpdate = true;
        silentVblanks++;
    }

    int expected = clockFrame();

    // Drop late frames (bounded so we never stall on a long catch-up).
    int dropBudget = 3;
    while (currentFrame < expected - 1 && framesAvailable > 0 && dropBudget-- > 0)
    {
        readIndex = (readIndex + 1) % FRAMES_TO_BUFFER;
        framesAvailable--;
        currentFrame++;
    }

    if (fileEOF && framesAvailable == 0)
    {
        return nextState;
    }

    bool worked = false;

    // Display first, so SD reads never delay a due frame.
    if (framesAvailable > 0 && currentFrame <= expected)
    {
        if (!waitedThisUpdate)
        {
            swiWaitForVBlank();
            waitedThisUpdate = true;
        }
        pumpAudio();
        dmaCopy(&ramBuffer[readIndex * frameSize], bgGetGfxPtr(bg), frameSize);
        readIndex = (readIndex + 1) % FRAMES_TO_BUFFER;
        framesAvailable--;
        currentFrame++;
        worked = true;
    }

    // Use the remaining time to read ahead; stop early if the next frame is already due.
    for (int r = 0; r < READS_PER_UPDATE; r++)
    {
        if (r > 0 && framesAvailable > 0 && currentFrame <= clockFrame())
        {
            break;
        }
        if (!refillBuffer())
        {
            break;
        }
        worked = true;
        pumpAudio();
    }

    // Nothing to do (waiting for the clock or a full buffer): don't spin.
    if (!worked && !waitedThisUpdate)
    {
        swiWaitForVBlank();
        pumpAudio();
    }

    return ViewState::KEEP_CURRENT;
}

void VideoController::cleanup()
{
    stopInternalAudio();

    if (ramBuffer != nullptr)
    {
        if (bg >= 0)
        {
            dmaFillWords(0, bgGetGfxPtr(bg), frameSize);
        }
        free(ramBuffer);
        ramBuffer = nullptr;
    }

    bg = -1;

    if (videoFile != nullptr)
    {
        fclose(videoFile);
        videoFile = nullptr;
    }

    haveChunkHeader = false;
    pendingAudioSize = 0;
    silentVblanks = 0;

    fileEOF = false;
    framesAvailable = 0;
    readIndex = 0;
    writeIndex = 0;
    currentFrame = 0;
}
