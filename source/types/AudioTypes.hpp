#pragma once
#include "debug/qoa.h"
#include <cstdint>
#include <cstdio>

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
