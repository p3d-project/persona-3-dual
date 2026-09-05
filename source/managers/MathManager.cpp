#include "MathManager.hpp"

float MathManager::tanDepreciated(float x)
{
    return M_PI_4 * x - x * (fabs(x) - 1) * (0.2447 + 0.0663 * fabs(x));
}

ae::q20_12_t MathManager::div(ae::q20_12_t num, ae::q20_12_t den)
{
    return ae::q20_12_t::from_raw_value(divf32(num.raw_value(), den.raw_value()));
}

ae::q20_12_t MathManager::sqrt(ae::q20_12_t v)
{
    uint32_t raw = sqrtf32(static_cast<uint32_t>(v.raw_value()));
    return ae::q20_12_t::from_raw_value(static_cast<int32_t>(raw));
}

ae::q20_12_t MathManager::mod(ae::q20_12_t num, ae::q20_12_t den)
{
    return ae::q20_12_t::from_raw_value(mod32(num.raw_value(), den.raw_value()));
}

ae::angle16_t MathManager::radiansToAngle(ae::q20_12_t radians)
{
    return static_cast<ae::angle16_t>((static_cast<int32_t>(radians.raw_value()) * 5215) / 4096);
}

ae::q20_12_t MathManager::angleToRadians(ae::angle16_t angle)
{
    int32_t raw_rad = (static_cast<int32_t>(angle) * 51472) >> 16;
    return ae::q20_12_t::from_raw_value(raw_rad);
}

ae::q4_12_t MathManager::sin(ae::q20_12_t radians)
{
    return ae::q4_12_t::from_raw_value(sinLerp(radiansToAngle(radians)));
}

ae::q4_12_t MathManager::sin(ae::angle16_t angle)
{
    return ae::q4_12_t::from_raw_value(sinLerp(angle));
}

ae::q4_12_t MathManager::cos(ae::q20_12_t radians)
{
    return ae::q4_12_t::from_raw_value(cosLerp(radiansToAngle(radians)));
}

ae::q4_12_t MathManager::cos(ae::angle16_t angle)
{
    return ae::q4_12_t::from_raw_value(cosLerp(angle));
}

ae::q20_12_t MathManager::tan(ae::q20_12_t radians)
{
    return ae::q20_12_t::from_raw_value(tanLerp(radiansToAngle(radians)));
}

ae::angle16_t MathManager::asin(ae::q4_12_t ratio)
{
    return asinLerp(ratio.raw_value());
}

ae::angle16_t MathManager::acos(ae::q4_12_t ratio)
{
    return acosLerp(ratio.raw_value());
}

ae::q20_12_t MathManager::dot(
    ae::q20_12_t x1, ae::q20_12_t y1, ae::q20_12_t z1, ae::q20_12_t x2, ae::q20_12_t y2, ae::q20_12_t z2)
{
    // libnds dotf32 handles the 64-bit accumulation internally and returns Q20.12
    s32 vec1[3] = {x1.raw_value(), y1.raw_value(), z1.raw_value()};
    s32 vec2[3] = {x2.raw_value(), y2.raw_value(), z2.raw_value()};
    return ae::q20_12_t::from_raw_value(dotf32(vec1, vec2));
}

ae::q20_12_t MathManager::lengthSq(ae::q20_12_t x, ae::q20_12_t y, ae::q20_12_t z)
{
    return dot(x, y, z, x, y, z);
}

ae::q20_12_t MathManager::length(ae::q20_12_t x, ae::q20_12_t y, ae::q20_12_t z)
{
    return ae::q20_12_t::from_raw_value(sqrtf32(lengthSq(x, y, z).raw_value()));
}

void MathManager::normalize(ae::q20_12_t& x, ae::q20_12_t& y, ae::q20_12_t& z)
{
    s32 vec[3] = {x.raw_value(), y.raw_value(), z.raw_value()};

    normalizef32(vec);

    x = ae::q20_12_t::from_raw_value(vec[0]);
    y = ae::q20_12_t::from_raw_value(vec[1]);
    z = ae::q20_12_t::from_raw_value(vec[2]);
}

ae::q20_12_t MathManager::randFrac()
{
    uint32_t r = static_cast<uint32_t>(rand()) % 4096;
    return ae::q20_12_t::from_raw_value(static_cast<int32_t>(r));
}

float MathManager::atan2(float y, float x)
{
    if (x >= 0)
    { // -pi/2 .. pi/2
        if (y >= 0)
        { // 0 .. pi/2
            if (y < x)
            { // 0 .. pi/4
                return tanDepreciated(y / x);
            }
            else
            { // pi/4 .. pi/2
                return M_PI_2 - tanDepreciated(x / y);
            }
        }
        else
        {
            if (-y < x)
            { // -pi/4 .. 0
                return tanDepreciated(y / x);
            }
            else
            { // -pi/2 .. -pi/4
                return -M_PI_2 - tanDepreciated(x / y);
            }
        }
    }
    else
    { // -pi..-pi/2, pi/2..pi
        if (y >= 0)
        { // pi/2 .. pi
            if (y < -x)
            { // pi*3/4 .. pi
                return tanDepreciated(y / x) + M_PI;
            }
            else
            { // pi/2 .. pi*3/4
                return M_PI_2 - tanDepreciated(x / y);
            }
        }
        else
        { // -pi .. -pi/2
            if (-y < -x)
            { // -pi .. -pi*3/4
                return tanDepreciated(y / x) - M_PI;
            }
            else
            { // -pi*3/4 .. -pi/2
                return -M_PI_2 - tanDepreciated(x / y);
            }
        }
    }
}

/// TODO: uses depreciated solution internally, needs to be updated next blocksds release
ae::q20_12_t MathManager::atan2(ae::q20_12_t y, ae::q20_12_t x)
{
    float result = atan2(static_cast<float>(y), static_cast<float>(x));
    return ae::q20_12_t{result};
}

u32 MathManager::secondsToSamples(ae::q20_12_t seconds, u32 sampleRate)
{
    u32 wholeSeconds = static_cast<u32>(seconds);
    ae::q20_12_t fractionalSeconds = seconds - ae::q20_12_t{wholeSeconds};
    u32 fractionalSamples = static_cast<u32>(fractionalSeconds * sampleRate);
    return (wholeSeconds * sampleRate) + fractionalSamples;
}
