#pragma once
#include <cstdint>
#include <cstdio>
#include <p3d-qoa>

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
    SFX_0 = 0,
    SFX_1,
    SFX_2,
};
