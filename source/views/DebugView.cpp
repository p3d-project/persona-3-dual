#include "views/DebugView.hpp"
#include "core/globals.hpp"
#include <aegis/types.hpp>
#include <nds.h>
#include <stdlib.h>
#include <string.h>

/**
 * TODO:
 * Rename qoaPlay functions to follow correct naming scheme
 * use ae types for values/numbers
 * Move into AudioSystem, AudioManager
 * Add doxygen documentation
 */

//audio
#define QOA_IMPLEMENTATION
#include "debug/qoa.h"
#include <maxmod9.h>

typedef struct
{
    qoa_desc info;
    FILE* file;
    uint32_t firstFramePos;
    uint32_t samplePos;

    uint32_t bufferLen;
    uint8_t* buffer;

    uint32_t sampleDataPos;
    uint32_t sampleDataLen;
    short* sampleData;
} qoaplay_desc;

// NOTE: these functions are not used
ae::q20_12_t getDuration();
ae::q20_12_t getTime();

int getFrame();
void seekFrame(int frame);
void setLoop(ae::q20_12_t startTime, ae::q20_12_t endTime);
void loop();
void rewind();
uint32_t decodeFrame();
uint32_t decode(short* sample_data, int num_samples);
void audioInit();
mm_word audioUpdate(mm_word length, mm_addr dest, mm_stream_formats format);
void audioCleanup();

qoaplay_desc* qp = nullptr;
bool isLoopingEnabled = false;
int startFrame = 0;
int endFrame = 0;

ae::q20_12_t getDuration()
{
    return ae::q20_12_t{qp->info.samples} / ae::q20_12_t{qp->info.samplerate};
}

ae::q20_12_t getTime()
{
    return ae::q20_12_t{qp->samplePos} / ae::q20_12_t{qp->info.samplerate};
}

int getFrame()
{
    return qp->samplePos / QOA_FRAME_LEN;
}

void seekFrame(int frame)
{
    if (frame < 0)
    {
        frame = 0;
    }
    if (frame > (int)qp->info.samples / QOA_FRAME_LEN)
    {
        frame = qp->info.samples / QOA_FRAME_LEN;
    }

    qp->samplePos = frame * QOA_FRAME_LEN;
    qp->sampleDataLen = 0;
    qp->sampleDataPos = 0;

    uint32_t offset = qp->firstFramePos + frame * qoa_max_frame_size(&qp->info);
    fseek(qp->file, offset, SEEK_SET);
}

void setLoop(ae::q20_12_t startTime, ae::q20_12_t endTime)
{
    isLoopingEnabled = true;

    // use 64-bit integers to prevent overflow
    // .raw_value() returns the number pre-multiplied by 2^12, so we shift right by 12
    int64_t startSample = ((int64_t)startTime.raw_value() * qp->info.samplerate) >> 12;
    startFrame = startSample / QOA_FRAME_LEN;

    if (endTime != ae::q20_12_t{-1})
    {
        int64_t endSample = ((int64_t)endTime.raw_value() * qp->info.samplerate) >> 12;
        endFrame = endSample / QOA_FRAME_LEN;
    }
    else
    {
        endFrame = qp->info.samples / QOA_FRAME_LEN;
    }
}

void loop()
{
    if (!isLoopingEnabled)
    {
        return;
    }

    if (getFrame() >= endFrame)
    {
        // rewind to start time
        seekFrame(startFrame);
    }
}

void rewind()
{
    fseek(qp->file, qp->firstFramePos, SEEK_SET);
    qp->samplePos = 0;
    qp->sampleDataLen = 0;
    qp->sampleDataPos = 0;
}

uint32_t decodeFrame()
{
    qp->bufferLen = fread(qp->buffer, 1, qoa_max_frame_size(&qp->info), qp->file);

    uint32_t frame_len;
    qoa_decode_frame(qp->buffer, qp->bufferLen, &qp->info, qp->sampleData, &frame_len);
    qp->sampleDataPos = 0;
    qp->sampleDataLen = frame_len;
    return frame_len;
}

uint32_t decode(short* sample_data, int num_samples)
{
    int src_index = qp->sampleDataPos * qp->info.channels;
    int dst_index = 0;
    for (int i = 0; i < num_samples; i++)
    {
        // do we have to decode more samples?
        if (qp->sampleDataLen - qp->sampleDataPos == 0)
        {
            // loop audio section
            loop();

            // decode audio
            if (!decodeFrame())
            {
                // loop to the beginning if audio is finished
                rewind();
                decodeFrame();
            }
            src_index = 0;
        }

        // write raw 16-bit PCM samples in interleaved channel order
        for (int c = 0; c < (int)qp->info.channels; c++)
        {
            sample_data[dst_index++] = qp->sampleData[src_index++];
        }
        qp->sampleDataPos++;
        qp->samplePos++;
    }
    return num_samples;
}

void audioInit()
{
    std::string filePath = fatBasePath + "music/menus/title/tightrope.qoa";

    // open header
    FILE* file = fopen(filePath.c_str(), "rb");
    if (!file)
    {
        return;
    }

    // read header
    uint8_t header[QOA_MIN_FILESIZE];
    int read = fread(header, QOA_MIN_FILESIZE, 1, file);
    if (!read)
    {
        return;
    }

    // decode header
    qoa_desc qoa;
    uint32_t firstFramePos = qoa_decode_header(header, QOA_MIN_FILESIZE, &qoa);
    if (!firstFramePos)
    {
        return;
    }

    // rewind file back to beginning of the first frame
    fseek(file, firstFramePos, SEEK_SET);

    // allocate one chunk of memory for the qoaplay_desc struct, the sample data
    // for one frame and a buffer to hold one frame of encoded data.
    uint32_t bufferSize = qoa_max_frame_size(&qoa);
    uint32_t sampleDataSize = qoa.channels * QOA_FRAME_LEN * sizeof(short) * 2;

    qp = (qoaplay_desc*)malloc(sizeof(qoaplay_desc) + bufferSize + sampleDataSize);
    memset(qp, 0, sizeof(qoaplay_desc));

    // set qoaplay_desc values
    qp->firstFramePos = firstFramePos;
    qp->file = file;
    qp->buffer = ((uint8_t*)qp) + sizeof(qoaplay_desc);
    qp->sampleData = (short*)(((uint8_t*)qp) + sizeof(qoaplay_desc) + bufferSize);

    qp->info.channels = qoa.channels;
    qp->info.samplerate = qoa.samplerate;
    qp->info.samples = qoa.samples;

    // set loop
    setLoop(ae::q20_12_t{17.962}, ae::q20_12_t{66.082});

    // setup maxmod audio
    mm_stream stream;
    stream.sampling_rate = qoa.samplerate;
    stream.buffer_length = bufferSize;
    stream.callback = audioUpdate;
    stream.format = (qoa.channels == 1) ? MM_STREAM_16BIT_MONO : MM_STREAM_16BIT_STEREO;
    stream.timer = MM_TIMER0;
    stream.manual = true;

    mmStreamOpen(&stream);
}

mm_word audioUpdate(mm_word length, mm_addr dest, mm_stream_formats format)
{
    if (((qp->info.channels == 1) && (format != MM_STREAM_16BIT_MONO)) ||
        ((qp->info.channels == 2) && (format != MM_STREAM_16BIT_STEREO)))
    {
        consoleDemoInit();
        printf("Audio format from the current audio track does not match the original audio format\n");
        while (1)
        {
            swiWaitForVBlank();
        }
    }

    uint32_t decoded = decode((short*)dest, length);
    return decoded;
}

void audioCleanup()
{
    qp = nullptr;
    isLoopingEnabled = false;
    startFrame = 0;
    endFrame = 0;

    fclose(qp->file);
    free(qp);
}

void DebugView::init()
{
    videoSetMode(MODE_3_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankD(VRAM_D_MAIN_BG_0x06020000);
    vramSetBankC(VRAM_C_SUB_BG);

    // set brightness on bottom screen to completely dark (no visible image)
    setBrightness(2, -16);

    // set audio
    audioInit();
}

ViewState DebugView::update()
{
    mmStreamUpdate();
    return ViewState::KEEP_CURRENT;
}

void DebugView::cleanup()
{
    audioCleanup();
    BaseView::cleanup();
}
