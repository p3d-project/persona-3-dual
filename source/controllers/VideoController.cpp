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

#define VID_AUDIO_NONE 0
#define VID_AUDIO_QOA 1

namespace
{
constexpr u32 RING_SIZE = 32768; // compressed audio ring, must be a power of two
constexpr u32 RING_MASK = RING_SIZE - 1;
static_assert(AUDIO_CHUNK_MAX <= RING_SIZE, "audio ring must hold at least one audio chunk");
} // namespace

// ===========================================================================
// Internal QOA player: compressed ring -> QOA decode -> maxmod stream
// ===========================================================================
static mm_word videoAudioCallback(mm_word length, mm_addr dest, mm_stream_formats);
static VideoAudio* g_audio = nullptr;

struct VideoAudio
{
    u8* ring = nullptr;
    u32 head = 0;
    u32 count = 0;

    short* pcm = nullptr; // one decoded QOA frame, interleaved
    u32 pcmPos = 0;
    u32 pcmLen = 0;

    u8* scratch = nullptr; // for frames that wrap around the ring end
    u32 scratchSize = 0;

    qoa_desc desc;
    u32 rate = 0;
    u32 channels = 0;
    u32 streamSamples = 0;

    u32 contentSamples = 0; // real audio samples handed to maxmod
    u32 freeRunSamples = 0; // silence after the file ended (keeps the clock running)
    u32 silentRun = 0;      // consecutive fully-silent samples
    bool fileEnded = false;
    bool fault = false;
    bool streamOpen = false;

    bool open(u32 sampleRate, u32 numChannels)
    {
        rate = sampleRate;
        channels = numChannels;
        memset(&desc, 0, sizeof(desc));
        desc.channels = numChannels;
        desc.samplerate = sampleRate;

        scratchSize = qoa_max_frame_size(&desc);
        ring = (u8*)malloc(RING_SIZE);
        pcm = (short*)malloc(QOA_FRAME_LEN * numChannels * sizeof(short));
        scratch = (u8*)malloc(scratchSize);
        if (!ring || !pcm || !scratch)
        {
            close();
            return false;
        }

        u32 s = (rate * VIDEO_AUDIO_STREAM_MS / 1000) & ~15u;
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

    u32 space() const
    {
        return RING_SIZE - count;
    }

    // Reads `size` bytes from f directly into the ring (handles wrap-around).
    bool readFrom(FILE* f, u32 size)
    {
        u32 tail = (head + count) & RING_MASK;
        u32 first = RING_SIZE - tail;
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

        u8 h[8];
        for (u32 i = 0; i < 8; i++)
        {
            h[i] = ring[(head + i) & RING_MASK];
        }
        u32 size = ((u32)h[6] << 8) | h[7];
        if (h[0] != channels || size < 8 || size > scratchSize)
        {
            fault = true;
            return false;
        }
        if (count < size)
        {
            return false; // underrun
        }

        const u8* src;
        u32 toEnd = RING_SIZE - head;
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

        u32 len = 0;
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
    u32 clock() const
    {
        s32 latency = (s32)streamSamples + (VIDEO_SYNC_OFFSET_MS * (s32)rate) / 1000;
        s32 played = (s32)(contentSamples + freeRunSamples) - latency;
        return played > 0 ? (u32)played : 0;
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
    const u32 ch = a->channels;
    u32 done = 0;

    while (done < length)
    {
        if (a->pcmPos >= a->pcmLen && !a->decodeFrame())
        {
            break;
        }
        u32 n = a->pcmLen - a->pcmPos;
        if (n > length - done)
        {
            n = length - done;
        }
        memcpy(out + done * ch, a->pcm + a->pcmPos * ch, n * ch * sizeof(short));
        a->pcmPos += n;
        done += n;
    }

    a->contentSamples += done;

    if (done < length)
    {
        u32 pad = length - done;
        memset(out + done * ch, 0, pad * ch * sizeof(short));
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
        return (int)(((u64)aud->clock() * (u32)fpsInt) / aud->rate);
    }
    return (int)(((u64)silentVblanks * (u32)fpsInt) / 60);
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
    u8 header[16];
    size_t hRead = fread(header, 1, 16, videoFile);
    u8 audioCodec = VID_AUDIO_NONE;
    u32 audioRate = 0;
    u32 audioChannels = 0;

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

    frameSize = (u32)frameW * frameH * bpp;
    bufferSize = frameSize * FRAMES_TO_BUFFER;

    // Select the background format from the parsed bit depth.
    if (bpp == 1)
    {
        bg = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

        u16 palette[256];
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

    ramBuffer = (u8*)memalign(32, bufferSize);
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

    // Each frame starts with a u32 audio size.
    if (!haveChunkHeader)
    {
        u32 size = 0;
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
    u8* dest = &ramBuffer[writeIndex * frameSize];
    u32 got = 0;
    while (got < frameSize)
    {
        u32 n = frameSize - got;
        if (n > VIDEO_READ_SLICE)
        {
            n = VIDEO_READ_SLICE;
        }
        size_t r = fread(dest + got, 1, n, videoFile);
        got += (u32)r;
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
        silentVblanks = (u32)(((u64)currentFrame * 60) / (u32)fpsInt);
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
