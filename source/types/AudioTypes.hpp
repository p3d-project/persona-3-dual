#pragma once
#include <cstdint>
#include <cstdio>

#include "debug/qoa.h"

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

enum class SFX
{
    SFX_0 = 0,
    SFX_1,
    SFX_2,
};
