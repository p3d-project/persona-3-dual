/**
 * @file MathManger.hpp
 * @brief Manager for hardware specific math functions
 * @author Taha Rashid (TheBossT910 / thebosst)
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
 */

#pragma once
#include <aegis/manager.hpp>
#include <aegis/ndsTypes.hpp>
#include <aegis/types.hpp>
#include <math.h>
#include <nds/arm9/math.h>
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
     * @brief Computes the angle (in radians) between the positive x-axis and (x, y).
     *
     * @details Calls BlocksDS's native fixed-point atan2_f32, which operates
     * directly on 20.12 fixed-point values with no float conversion.
     *
     * @param y Y Point 2.
     * @param x X Point 1.
     * @return The angle in radians, as Q20.12.
     */
    ae::q20_12_t atan2(ae::q20_12_t y, ae::q20_12_t x);

    /**
     * @brief Computes the arctangent of a single ratio.
     *
     * @details Implemented as atan2(y, 1), per BlocksDS's documented
     * approach for deriving atan from atan2_f32.
     *
     * @param y Ratio in Q20.12.
     * @return The angle in radians, as Q20.12.
     */
    ae::q20_12_t atan(ae::q20_12_t y);

    /**
     * @brief Divides two Q20.12 values.
     *
     * @param num Numerator.
     * @param den Denominator.
     * @return The quotient in Q20.12.
     */
    ae::q20_12_t div(ae::q20_12_t num, ae::q20_12_t den);

    /**
     * @brief Computes the square root of a Q20.12 value.
     *
     * @param v Value to root.
     * @return The square root in Q20.12.
     */
    ae::q20_12_t sqrt(ae::q20_12_t v);

    /**
     * @brief Computes the modulo of two Q20.12 values.
     *
     * @param num Numerator.
     * @param den Denominator.
     * @return The remainder in Q20.12.
     */
    ae::q20_12_t mod(ae::q20_12_t num, ae::q20_12_t den);

    /**
     * @brief Converts an angle in radians (Q20.12)
     *
     * @param radians Angle in radians, as a Q20.12 fixed-point value.
     * @return The equivalent cyclic angle (full turn = 1<<15).
     */
    ae::angle16_t radiansToAngle(ae::q20_12_t radians);

    /**
     * @brief Converts a cyclic angle to radians (Q20.12).
     *
     * @param angle Cyclic angle (full turn = 1<<15).
     * @return The equivalent angle in radians, as a Q20.12 fixed-point value.
     */
    ae::q20_12_t angleToRadians(ae::angle16_t angle);

    /**
     * @brief Sine of a cyclic angle.
     *
     * @param angle Cyclic angle (full turn = 1<<15).
     * @return The sine in Q4.12.
     */
    ae::q4_12_t sin(ae::angle16_t angle);

    /**
     * @brief Sine, accepting radians (Q20.12)
     *
     * @param radians Angle in radians, as a Q20.12 fixed-point value.
     * @return The sine in Q4.12.
     */
    ae::q4_12_t sin(ae::q20_12_t radians);

    /**
     * @brief Cosine of a cyclic angle.
     *
     * @param angle Cyclic angle (full turn = 1<<15).
     * @return The cosine in Q4.12.
     */
    ae::q4_12_t cos(ae::angle16_t angle);

    /**
     * @brief Cosine, accepting radians (Q20.12)
     *
     * @param radians Angle in radians, as a Q20.12 fixed-point value.
     * @return The cosine in Q4.12.
     */
    ae::q4_12_t cos(ae::q20_12_t radians);

    /**
     * @brief Tangent, accepting radians (Q20.12)
     *
     * @param radians Angle in radians, as a Q20.12 fixed-point value.
     * @return The tangent in Q20.12.
     */
    ae::q20_12_t tan(ae::q20_12_t radians);

    /**
     * @brief Computes the arcsine of a ratio.
     *
     * @param ratio Ratio in Q4.12.
     * @return The cyclic angle whose sine is ratio.
     */
    ae::angle16_t asin(ae::q4_12_t ratio);

    /**
     * @brief Computes the arccosine of a ratio.
     *
     * @param ratio Ratio in Q4.12.
     * @return The cyclic angle whose cosine is ratio.
     */
    ae::angle16_t acos(ae::q4_12_t ratio);

    /**
     * @brief Computes the dot product of two Q20.12 3D vectors.
     *
     * @param x1 X component of the first vector.
     * @param y1 Y component of the first vector.
     * @param z1 Z component of the first vector.
     * @param x2 X component of the second vector.
     * @param y2 Y component of the second vector.
     * @param z2 Z component of the second vector.
     * @return The dot product in Q20.12.
     */
    ae::q20_12_t dot(
        ae::q20_12_t x1, ae::q20_12_t y1, ae::q20_12_t z1, ae::q20_12_t x2, ae::q20_12_t y2, ae::q20_12_t z2);

    /**
     * @brief Computes the squared length of a Q20.12 3D vector.
     *
     * @param x X component.
     * @param y Y component.
     * @param z Z component.
     * @return The squared length in Q20.12.
     */
    ae::q20_12_t lengthSq(ae::q20_12_t x, ae::q20_12_t y, ae::q20_12_t z);

    /**
     * @brief Computes the length of a Q20.12 3D vector.
     *
     * @param x X component.
     * @param y Y component.
     * @param z Z component.
     * @return The length in Q20.12.
     */
    ae::q20_12_t length(ae::q20_12_t x, ae::q20_12_t y, ae::q20_12_t z);

    /**
     * @brief Normalizes a Q20.12 3D vector in place.
     *
     * @param x X component, updated in place.
     * @param y Y component, updated in place.
     * @param z Z component, updated in place.
     * @return Sets x, y, z to 0,0,0 if length is zero (via out parameters).
     */
    void normalize(ae::q20_12_t& x, ae::q20_12_t& y, ae::q20_12_t& z);

    /**
     * @brief Converts a Q20.12 seconds value to a sample count, avoiding
     * overflow when seconds * sampleRate exceeds Q20.12's integer range.
     *
     * @param seconds Time value in seconds, as Q20.12.
     * @param sampleRate Samples per second.
     * @return The equivalent sample count.
     */
    u32 secondsToSamples(ae::q20_12_t seconds, u32 sampleRate);

    /**
     * @brief Returns a uniformly distributed random fraction from 0 to 1.
     *
     * @return A random value in Q20.12, from 0 to 1.
     */
    ae::q20_12_t randFrac();

  private:
    friend class Singleton<MathManager>;
    MathManager() = default;
};
