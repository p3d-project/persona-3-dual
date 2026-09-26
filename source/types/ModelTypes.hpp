#pragma once

#include <etl/vector.h>
#include <nds.h>

/**
 * @brief Represents a texture used in a 3D model.
 */
struct MDL3Texture
{
    int textureID = -1;
    uint16_t width = 0;
    uint16_t height = 0;
    bool isRGBA = false;
    char name[65] = {0};

    MDL3Texture() = default;
    ~MDL3Texture() = default;
};

// TODO: figure out a way to make the vector sizes dynamic or compiletimed

/**
 * @brief Represents a position keyframe in an animation.
 */
struct PositionKey
{
    int16_t frame;
    int32_t x, y, z;
};

/**
 * @brief Represents a quaternion keyframe in an animation.
 */
struct QuaternionKey
{
    int16_t frame;
    int16_t x, y, z, w;
};

/**
 * @brief Represents a track for a node in an animation.
 */
struct NodeTrack
{
    int32_t nodeIndex = -1;
    etl::vector<PositionKey, 64> translations;
    etl::vector<QuaternionKey, 64> rotations;
};

/**
 * @brief Represents an animation in the 3D model.
 */
struct Animation_N
{
    char name[32] = {0};
    uint32_t numFrames = 0;
    int16_t fps;
    etl::vector<NodeTrack, 64> tracks;
};

/**
 * @brief Represents a sublist of a node in the 3D model.
 */
struct SubList_N
{
    int32_t texSlot = -1;
    int32_t dlSize = 0;
    const uint32_t* displayList = nullptr;
};

/**
 * @brief Represents a node in the 3D model.
 */
struct Node
{
    int32_t pid = -1;
    etl::vector<SubList_N, 64> subLists;
};

/**
 * @brief Represents a 3D model with nodes, textures, and animations.
 */
struct MDL3Model
{
    uint32_t nodeCount = 0;
    uint32_t texCount = 0;
    etl::vector<MDL3Texture, 8> textures;
    etl::vector<Node, 64> nodes;

    MDL3Model() = default;
    ~MDL3Model() = default;
};

#pragma pack(push, 1)
/**
 * @brief Represents the header of a raw texture file.
 * @note This struct is to be used for reading the texture header from a file.
 */
struct RawTextureHeader
{
    char name[64];
    uint16_t width;
    uint16_t height;
    uint8_t isRGBA;
    uint8_t pad[3];
};

/**
 * @brief Represents the header of a raw model file.
 * @note This struct is to be used for reading the model header from a file.
 */
struct RawModelHeader
{
    char magic[4];
    uint32_t nodeCount = 0;
    uint32_t animCount = 0;
    uint32_t texCount = 0;
};

/**
 * @brief Represents the header of a raw node file.
 * @note This struct is to be used for reading the node header from a file.
 */
struct RawNodeHeader
{
    int32_t pid = -1;
    uint32_t subListCount = 0;
};
#pragma pack(pop)
