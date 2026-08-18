/**
 * @file MathManger.hpp
 * @brief Manager for hardware specific math functions
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include <aegis/manager.hpp>
#include <math.h>
#include <nds/arm9/trig_lut.h>

class MathManager : public ae::Manager, public ae::Singleton<MathManager>
{
  public:
    void Init() override
    {
    }

    void Process() override
    {
    }

    void Shutdown() override
    {
    }

    /**
     * @brief Computes the sine of an angle using a hardware-optimized lookup table.
     *
     * @param r The angle in radians.
     * @return The sine of the angle, ranging from -1.0f to 1.0f.
     */
    float sin(float r);

    /**
     * @brief Computes the cosine of an angle using a hardware-optimized lookup table.
     *
     * @param r The angle in radians.
     * @return The cosine of the angle, ranging from -1.0f to 1.0f.
     */
    float cos(float r);

    /**
    * @brief Computes the tangent from an angles using an approximation.
    *
    * @details Function courtesy of https://yal.cc/fast-atan2/.
    *
    * @param x The angle in radians.
    * @return The tangent of the angle.
    * @todo Replace with tanLerp?
    *
    * @author Vadym Diachenko (yellowafterlife)
    */
    float tan(float r);

    // TODO: replace with native atan2 solution in upcoming BlocksDS update
    /**
     * @brief Computes the arctangent from two angles using an approximation.
     *
     * @details Function courtesy of https://yal.cc/fast-atan2/.
     * @note Does not handle infinities or NaNs. Has a limited input range (±45deg).
     *
     * @param y Point 2.
     * @param x Point 1.
     * @return The arctangent angle of the two points.
     *
     * @author Vadym Diachenko (yellowafterlife)
     */
    float atan2(float y, float x);

  private:
    friend class Singleton<MathManager>;
    MathManager() = default;

    const float RAD_TO_LIBNDS = 32768.0f / (2.0f * 3.14159265f);
};
