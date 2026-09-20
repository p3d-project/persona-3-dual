#pragma once
#include <cstdint>
#include <cstdio>
#include <p3d-qoa>

extern "C"
{
#include "soundbank.h"
}

/**
 * @brief QOA audio stream data
 */
struct qoaplay_desc
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
};

/**
 * @brief SFX wrapper enums
 */
enum class SFX
{
    SFX_0 = SFX_CANCEL,
    SFX_1 = SFX_MENU,
    SFX_2 = SFX_SELECT
};
