#pragma once

#include "managers/RenderManager.hpp"

#include <etl/vector.h>
#include <nds.h>

struct MDL3Texture
{
    int textureID = -1;
    uint16_t width = 0;
    uint16_t height = 0;
    bool isRGBA = false;
    char name[65] = {0};

    MDL3Texture() = default;
    ~MDL3Texture()
    {
        if (textureID != -1)
        {
            RenderManager::GetInstance().deleteTexture(textureID);
        }
    }
};

// TODO: figure out a way to make the vector sizes dynamic or compiletimed

struct PositionKey
{
    int16_t frame;
    int32_t x, y, z;
};

struct QuaternionKey
{
    int16_t frame;
    int16_t x, y, z, w;
};

struct NodeTrack
{
    int32_t nodeIndex = -1;
    etl::vector<PositionKey, 64> translations;
    etl::vector<QuaternionKey, 64> rotations;
};

struct Animation
{
    char name[32] = {0};
    uint32_t numFrames = 0;
    int16_t fps;
    etl::vector<NodeTrack, 64> tracks;
};

struct SubList
{
    int32_t texSlot = -1;
    int32_t dlSize = 0;
    etl::vector<uint32_t, 64> displayList;
};

struct Node
{
    int32_t pid = -1;
    int32_t px = 0, py = 0, pz = 0;
    etl::vector<SubList, 64> subLists;
};

struct MDL3Model
{
    uint32_t nodeCount = 0;
    uint32_t texCount = 0;
    etl::vector<MDL3Texture, 64> textures;
    etl::vector<Node, 64> nodes;
    etl::vector<Animation, 32> animations;

    MDL3Model() = default;
    ~MDL3Model() = default;
};

#pragma pack(push, 1)
struct RawTextureHeader
{
    char name[65];
    uint16_t width;
    uint16_t height;
    bool isRGBA;
    uint8_t pad[3];
};
#pragma pack(pop)

struct RawModelHeader
{
    char magic[4];
    uint32_t nodeCount = 0;
    uint32_t animCount = 0;
    uint32_t texCount = 0;
};

struct RawNodeHeader
{
    int32_t pid = -1;
    int32_t posX = 0;
    int32_t posY = 0;
    int32_t posZ = 0;
    uint32_t subListCount = 0;
};
