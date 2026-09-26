#pragma once

#include "data/spriteDb.hpp"
#include <nds.h>

/**
 * @brief Holds loaded graphics data
 */
struct GraphicAsset
{
    int id = -1;
    void* tiles = nullptr;
    u32 tilesLen = -1;
    void* pal = nullptr;
    u32 palLen = -1;
    void* map = nullptr;
    u32 mapLen = -1;
};

/**
 * @brief A simple sprite structure
 */
struct Sprite
{
    SpriteSize size;
    SpriteColorFormat format;
    int paletteAlpha;
    u16* gfx; // default = 0?
};

/**
 * @brief Data to render a Sprite
 */
struct SpriteRenderState
{
    // sprite data
    Sprite& sprite;

    // render data
    int x = 0;
    int y = 0;
    int priority = 1;
    int affineIndex = 0;
    bool hflip = false;
    bool vflip = false;
    // uncommon
    bool hide = false;
    bool sizeDouble = false;
    bool mosaic = false;
};

/**
 * @brief Data to perform a transformation on a Sprite
 */
struct SpriteTransform
{
    int angle;
    int sx;
    int sy;
};

/**
 * @brief Data to manage the lifecycle of a sprite
 */
struct SpritePayload
{
    SpriteRenderState& srs;
    // SpriteTransform spriteTransform; // TODO: remove? This is an operation *on* a sprite, not dealing with lifecyle of a sprite

    std::string& spritePath;
    GraphicAsset& ga;

    SpriteType spriteType;
    int spriteVariant;
};
