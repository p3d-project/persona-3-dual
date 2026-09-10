#pragma once

#include <aegis/ndsTypes.hpp>
#include <aegis/types.hpp>
#include <fpm/fixed.hpp>

#include <nds.h>

struct View3DConfig
{
    int settings;
    uint32_t polyParams = POLY_ALPHA(31) | POLY_CULL_BACK;

    int clearColorRed = 0;
    int clearColorGreen = 0;
    int clearColorBlue = 0;
    int clearColorAlpha = 31;

    int clearDepth = 0x7FFF;
    int clearPolyID = 0;

    int fov = 55;
    double aspect = 256.0 / 192.0;
    /// @brief Defines how close the cmaera can see.
    double nearPlane = 0.1;
    /// @brief Defines how far the camera can see.
    int farPlane = 40;

    /// @brief Outline color in RGB15 format. If -1, outline will not be enabled.
    rgb outlineColor = -1;

    /// @brief Red component of fog color (0-31). If -1, fog will not be enabled.
    int8_t fogRed = -1;
    /// @brief Green component of fog color (0-31).
    int8_t fogGreen;
    /// @brief Blue component of fog color (0-31).
    int8_t fogBlue;
    /// @brief Alpha component of fog color (0-31).
    int8_t fogAlpha;
    uint8_t shift = 1;
    /// @brief How thick (translucent) the fog is
    uint8_t mass = 1;
    /// @brief How far the fog is (0x0000 to 0x8000)
    uint16_t depth = 0x6000;
};
