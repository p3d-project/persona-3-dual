#pragma once

#include <aegis/ndsTypes.hpp>
#include <aegis/types.hpp>
#include <fpm/fixed.hpp>

#include <nds.h>

/**
 * @brief Data structure to store RGBA values as 8-bit signed integers.
 */
struct RGBA
{
    int8_t red;
    int8_t green;
    int8_t blue;
    int8_t alpha;

    /**
     * @brief Constructor to initialize RGBA data structure .
     *
     * @param r Red component (0-31)
     * @param g Green component (0-31)
     * @param b Blue component (0-31)
     * @param a Alpha component (0-31)
     */
    RGBA(int8_t r, int8_t g, int8_t b, int8_t a) : red(r), green(g), blue(b), alpha(a)
    {
    }
};

/**
 * @brief Data structure to store 3D view configuration parameters.
 */
struct View3DConfig
{
    /// @brief This is a bitmask of various settings, which will be passed to `glEnable()`.
    int settings;
    /// @brief This is a bitmask of various polygon parameters, which will be passed to `glPolyFmt()`.
    uint32_t polyParams = POLY_ALPHA(31) | POLY_CULL_BACK;

    RGBA clearColor = RGBA(0, 0, 0, 31);
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

    /// @brief RGBA values for the foh color. If the r value is -1, fog will not be enabled.
    RGBA fogColor = RGBA(-1, 0, 0, 0);
    uint8_t shift = 1;
    /// @brief How thick (translucent) the fog is
    uint8_t mass = 1;
    /// @brief How far the fog is (0x0000 to 0x8000)
    uint16_t depth = 0x6000;
};
