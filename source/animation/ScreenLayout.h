#pragma once
#include "core/geometry.h"

namespace uiAnimation
{

struct ScreenLayout
{
    static constexpr int width()
    {
        return 256;
    }
    static constexpr int height()
    {
        return 192;
    }
    static constexpr int centerX()
    {
        return 128;
    }
    static constexpr int centerY()
    {
        return 96;
    }

    static constexpr int rightEdge(int w)
    {
        return 256 - w;
    }
    static constexpr int bottomEdge(int h)
    {
        return 192 - h;
    }

    static Point2D<int> topLeft()
    {
        return {0, 0};
    }
    static Point2D<int> topRight()
    {
        return {256, 0};
    }
    static Point2D<int> bottomLeft()
    {
        return {0, 192};
    }
    static Point2D<int> bottomRight()
    {
        return {256, 192};
    }
    static Point2D<int> center()
    {
        return {128, 96};
    }
};

} // namespace uiAnimation
