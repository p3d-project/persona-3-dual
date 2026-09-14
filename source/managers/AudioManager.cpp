#include "AudioManager.hpp"
#include "core/globals.hpp"
#include <nds.h>
#include <string>

#define QOA_IMPLEMENTATION
#include "debug/qoa.h"
#include "soundbank_bin.h"

/**
 * TODO:
 * Move into AudioSystem or component
 * Add doxygen documentation
 * Create fork of qoa & apply changes to the fork, & pull in qoa as a submodule
 */

void AudioManager::Init()
{
    // initialize maxmod (for audio)
    mm_ds_system sys;
    sys.mod_count = 0;
    sys.samp_count = 0;
    sys.mem_bank = 0;
    mmInit(&sys);

    // initialize maxmod (for sfx)
    mmInitDefaultMem((mm_addr)soundbank_bin);

    // debug
    audioInit();
}

void AudioManager::Process()
{
    // TODO: somehow here, have the audioInit fn run
    // have the audiosystem submit a payload (string path) to get an audio track
    // boolean. If no audio yet, don't update mmStreamUpdate()

    mmStreamUpdate();
}

void AudioManager::Shutdown()
{
    qp = nullptr;
    isLoopingEnabled = false;
    startFrame = 0;
    endFrame = 0;

    fclose(qp->file);
    free(qp);
}

ae::q20_12_t AudioManager::getDuration()
{
    return ae::q20_12_t{qp->info.samples} / ae::q20_12_t{qp->info.samplerate};
}

ae::q20_12_t AudioManager::getTime()
{
    return ae::q20_12_t{qp->samplePos} / ae::q20_12_t{qp->info.samplerate};
}

int AudioManager::getFrame()
{
    return qp->samplePos / QOA_FRAME_LEN;
}

void AudioManager::seekFrame(int frame)
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

void AudioManager::setLoop(ae::q20_12_t startTime, ae::q20_12_t endTime)
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

void AudioManager::loop()
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

void AudioManager::rewind()
{
    fseek(qp->file, qp->firstFramePos, SEEK_SET);
    qp->samplePos = 0;
    qp->sampleDataLen = 0;
    qp->sampleDataPos = 0;
}

uint32_t AudioManager::decodeFrame()
{
    qp->bufferLen = fread(qp->buffer, 1, qoa_max_frame_size(&qp->info), qp->file);

    uint32_t frame_len;
    qoa_decode_frame(qp->buffer, qp->bufferLen, &qp->info, qp->sampleData, &frame_len);
    qp->sampleDataPos = 0;
    qp->sampleDataLen = frame_len;
    return frame_len;
}

uint32_t AudioManager::decode(short* sample_data, int num_samples)
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

void AudioManager::audioInit()
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
    stream.callback = audioCallback;
    stream.format = (qoa.channels == 1) ? MM_STREAM_16BIT_MONO : MM_STREAM_16BIT_STEREO;
    stream.timer = MM_TIMER0;
    stream.manual = true;

    mmStreamOpen(&stream);
}

mm_word AudioManager::audioCallback(mm_word length, mm_addr dest, mm_stream_formats format)
{
    return AudioManager::GetInstance().processStream(length, dest, format);
}

mm_word AudioManager::processStream(mm_word length, mm_addr dest, mm_stream_formats format)
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
