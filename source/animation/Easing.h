#pragma once

namespace uiAnimation
{

enum class Ease
{
    Linear,
    InQuad,
    OutQuad,
    InOutQuad,
    InCubic,
    OutCubic,
    InBack,
    OutBack,
};

inline float applyEase(Ease e, float t)
{
    switch (e)
    {
    case Ease::InQuad:
        return t * t;
    case Ease::OutQuad:
        return t * (2.0f - t);
    case Ease::InOutQuad:
        return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
    case Ease::InCubic:
        return t * t * t;
    case Ease::OutCubic:
    {
        float f = t - 1.0f;
        return f * f * f + 1.0f;
    }
    case Ease::InBack:
    {
        const float s = 1.70158f;
        return t * t * ((s + 1.0f) * t - s);
    }
    case Ease::OutBack:
    {
        const float s = 1.70158f;
        float f = t - 1.0f;
        return f * f * ((s + 1.0f) * f + s) + 1.0f;
    }
    default:
        return t;
    }
}

} // namespace uiAnimation
