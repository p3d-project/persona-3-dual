#pragma once

#include <aegis/ndsTypes.hpp>
#include <aegis/types.hpp>
#include <nds.h>

struct EnvironmentTexture
{
    const char* name;
    int width;
    int height;

    const unsigned int* bitmap;
};

struct BillboardData
{
    const char* name;
    ae::q4_12_t x, y, z;
    ae::q4_12_t halfWidth;
    ae::q4_12_t halfHeight;

    int texSlot;

    ae::q12_4_t u0, v0;
    ae::q12_4_t u1, v1;
};

struct EnvironmentDbEntry
{
    // Name/debugging
    const char* name;

    // Binary display list file
    const char* binaryFile;

    // World bounds
    ae::q20_12_t worldOffsetX;
    ae::q20_12_t worldOffsetZ;
    ae::q20_12_t worldWidth;
    ae::q20_12_t worldDepth;

    // Texture information
    int textureCount;
    const EnvironmentTexture* textures;

    // Billboards
    int billboardCount;
    const BillboardData* billboards;
};
