#include "views/DebugView.hpp"
#include "core/globals.hpp"
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

    unsigned int firstFramePos;
    unsigned int samplePos;

    unsigned int bufferLen;
    unsigned char* buffer;

    unsigned int sampleDataPos;
    unsigned int sampleDataLen;
    short* sampleData;
} qoaplay_desc;

// TODO: remove static keyword?
static void audioInit();
static mm_word audioUpdate(mm_word length, mm_addr dest, mm_stream_formats format);
qoaplay_desc* qp = nullptr;
bool isLoopingEnabled = false;
int startFrame = 0;
int endFrame = 0;

double qoaplay_get_duration()
{
    return (double)qp->info.samples / (double)qp->info.samplerate;
}

double qoaplay_get_time()
{
    return (double)qp->samplePos / (double)qp->info.samplerate;
}

int qoaplay_get_frame()
{
    return qp->samplePos / QOA_FRAME_LEN;
}

void qoaplay_seek_frame(int frame)
{
    if (frame < 0)
    {
        frame = 0;
    }
    if (frame > qp->info.samples / QOA_FRAME_LEN)
    {
        frame = qp->info.samples / QOA_FRAME_LEN;
    }

    qp->samplePos = frame * QOA_FRAME_LEN;
    qp->sampleDataLen = 0;
    qp->sampleDataPos = 0;

    unsigned int offset = qp->firstFramePos + frame * qoa_max_frame_size(&qp->info);
    fseek(qp->file, offset, SEEK_SET);
}

void setLoop(float startTime, float endTime)
{
    isLoopingEnabled = true;
    startFrame = ceil((startTime * qp->info.samplerate) / QOA_FRAME_LEN);
    endFrame =
        endTime != -1 ? ceil((endTime * qp->info.samplerate) / QOA_FRAME_LEN) : ceil(qp->info.samples / QOA_FRAME_LEN);
}

void loop()
{
    if (!isLoopingEnabled)
    {
        return;
    }

    if (qoaplay_get_frame() >= endFrame)
    {
        // rewind to start time
        qoaplay_seek_frame(startFrame);
    }
}

void qoaplay_rewind()
{
    fseek(qp->file, qp->firstFramePos, SEEK_SET);
    qp->samplePos = 0;
    qp->sampleDataLen = 0;
    qp->sampleDataPos = 0;
}

unsigned int qoaplay_decode_frame()
{
    qp->bufferLen = fread(qp->buffer, 1, qoa_max_frame_size(&qp->info), qp->file);

    unsigned int frame_len;
    qoa_decode_frame(qp->buffer, qp->bufferLen, &qp->info, qp->sampleData, &frame_len);
    qp->sampleDataPos = 0;
    qp->sampleDataLen = frame_len;
    return frame_len;
}

unsigned int qoaplay_decode(short* sample_data, int num_samples)
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
            if (!qoaplay_decode_frame())
            {
                // loop to the beginning if audio is finished
                qoaplay_rewind();
                qoaplay_decode_frame();
            }
            src_index = 0;
        }

        // write raw 16-bit PCM samples in interleaved channel order
        for (int c = 0; c < qp->info.channels; c++)
        {
            sample_data[dst_index++] = qp->sampleData[src_index++];
        }
        qp->sampleDataPos++;
        qp->samplePos++;
    }
    return num_samples;
}

static void audioInit()
{
    std::string filePath = fatBasePath + "music/menus/title/tightrope.qoa";

    // open header
    FILE* file = fopen(filePath.c_str(), "rb");
    if (!file)
    {
        return;
    }

    // read header
    unsigned char header[QOA_MIN_FILESIZE];
    int read = fread(header, QOA_MIN_FILESIZE, 1, file);
    if (!read)
    {
        return;
    }

    // decode header
    qoa_desc qoa;
    unsigned int firstFramePos = qoa_decode_header(header, QOA_MIN_FILESIZE, &qoa);
    if (!firstFramePos)
    {
        return;
    }

    // rewind file back to beginning of the first frame
    fseek(file, firstFramePos, SEEK_SET);

    // allocate one chunk of memory for the qoaplay_desc struct, the sample data
    // for one frame and a buffer to hold one frame of encoded data.
    unsigned int bufferSize = qoa_max_frame_size(&qoa);
    unsigned int sampleDataSize = qoa.channels * QOA_FRAME_LEN * sizeof(short) * 2;

    qp = (qoaplay_desc*)malloc(sizeof(qoaplay_desc) + bufferSize + sampleDataSize);
    memset(qp, 0, sizeof(qoaplay_desc));

    // set qoaplay_desc values
    qp->firstFramePos = firstFramePos;
    qp->file = file;
    qp->buffer = ((unsigned char*)qp) + sizeof(qoaplay_desc);
    qp->sampleData = (short*)(((unsigned char*)qp) + sizeof(qoaplay_desc) + bufferSize);

    qp->info.channels = qoa.channels;
    qp->info.samplerate = qoa.samplerate;
    qp->info.samples = qoa.samples;

    // set loop
    setLoop(17.962, 66.082);

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
        // TODO: display error message
        // audio channels do not match...
    }

    unsigned int decoded = qoaplay_decode((short*)dest, length);
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
