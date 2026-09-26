#pragma once

#include <aegis/ndsTypes.hpp>
#include <aegis/types.hpp>
#include <core/geometry.hpp>
#include <nds.h>

enum class TileType
{
    NO_COLLISION = 0,
    COLLISION = 1,
    SAVE = 2,

    // scenes / events
    SCENE_0 = 400,
    SCENE_1 = 401,
    SCENE_2 = 402,
    SCENE_3 = 403,
    SCENE_4 = 404,
    SCENE_5 = 405,
    SCENE_6 = 406,
    SCENE_7 = 407,
    SCENE_8 = 408,
    SCENE_9 = 409,

    // characters (party)
    C_MC = 600,
    C_YU = 601,
    C_JU = 602,
    C_AK = 603,
    C_MI = 604,
    C_FU = 605,
    C_AE = 606,
    C_KO = 607,
    C_KE = 608,
    C_SH = 609,

    // enemies (shadows)
    // w = weak, m = medium, s = tring
    SHD_W = 800,
    SHD_M = 801,
    SHD_S = 802,
};

struct CharacterPosition
{
    ae::q20_12_t x;
    ae::q20_12_t z;
    ae::q20_12_t y; // height
    ae::q20_12_t facingAngle;
};

/**
 * @brief A struct that holds initial config values for the MovementComponent
 */
struct MovementConfig
{
    // 3D environment
    int mapWidth;
    int mapHeight;
    uint16_t* collisionMap;

    // world
    ae::q20_12_t tileSize;
    ae::q20_12_t worldOffsetX;
    ae::q20_12_t worldOffsetZ;
    Point2D<ae::q20_12_t> characterSize;

    // translation
    ae::q20_12_t speed;
    ae::q20_12_t height;
    Point2D<ae::q20_12_t> characterTranslate;
    ae::q20_12_t characterFacingAngle;

    MovementConfig() = default;

    MovementConfig(int iMapWidth,
                   int iMapHeight,
                   uint16_t* iCollisionMap,
                   ae::q20_12_t iTileSize,
                   ae::q20_12_t iWorldOffsetX,
                   ae::q20_12_t iWorldOffsetZ,
                   Point2D<ae::q20_12_t> iCharacterSize,
                   ae::q20_12_t iSpeed,
                   ae::q20_12_t iHeight,
                   Point2D<ae::q20_12_t> iCharacterTranslate,
                   ae::q20_12_t iCharacterFacingAngle)
        : mapWidth(iMapWidth), mapHeight(iMapHeight), collisionMap(iCollisionMap), tileSize(iTileSize),
          worldOffsetX(iWorldOffsetX), worldOffsetZ(iWorldOffsetZ), characterSize(iCharacterSize), speed(iSpeed)
    {
        height = iHeight;
        characterTranslate = iCharacterTranslate;
        characterFacingAngle = iCharacterFacingAngle;
    };
};
