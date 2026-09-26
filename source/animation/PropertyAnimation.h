#pragma once
#include "animation/Animated.h"
#include "animation/Animation.h"
#include "animation/AnimationHandle.h"
#include "animation/Easing.h"
#include "core/geometry.hpp"
#include <algorithm>
#include <memory>

namespace uiAnimation
{

inline float lerp(float a, float b, float t)
{
    return a + (b - a) * t;
}
inline int lerp(int a, int b, float t)
{
    return a + static_cast<int>((b - a) * t);
}

template <typename T> inline Point2D<T> lerp(const Point2D<T>& a, const Point2D<T>& b, float t)
{
    return Point2D<T>(lerp(a.x, b.x, t), lerp(a.z, b.z, t));
}

template <typename T> class PropertyAnimation : public Animation
{
  public:
    PropertyAnimation(T* tgt, const T& from, const T& to, float durationFrames, Ease ease)
        : target(tgt), from(from), to(to), durationFrames(durationFrames), ease(ease)
    {
    }

    void reverse() override
    {
        std::swap(from, to);
        elapsed = 0.0f;
        done = false;
        cancelled = false;
    }

  protected:
    bool onUpdate(float dt) override
    {
        elapsed += dt;
        float t = durationFrames > 0.0f ? std::clamp(elapsed / durationFrames, 0.0f, 1.0f) : 1.0f;
        *target = lerp(from, to, applyEase(ease, t));
        return elapsed >= durationFrames;
    }

  private:
    T* target;
    T from;
    T to;
    float durationFrames;
    float elapsed = 0.0f;
    Ease ease;
};

class Animator;

template <typename T> class PropertyAnimationBuilder
{
  public:
    explicit PropertyAnimationBuilder(T& tgt, Animator* owner = nullptr) : ptr(&tgt), owner(owner)
    {
    }

    PropertyAnimationBuilder& from(const T& v)
    {
        fromVal = v;
        hasFrom = true;
        return *this;
    }
    PropertyAnimationBuilder& to(const T& v)
    {
        toVal = v;
        return *this;
    }

    PropertyAnimationBuilder& duration(int ms)
    {
        durationFrames = ms * 60.0f / 1000.0f;
        return *this;
    }

    PropertyAnimationBuilder& ease(Ease e)
    {
        easeType = e;
        return *this;
    }

    std::shared_ptr<Animation> build() const
    {
        T f = hasFrom ? fromVal : *ptr;
        return std::make_shared<PropertyAnimation<T>>(ptr, f, toVal, durationFrames, easeType);
    }

    AnimationHandle start();

  private:
    T* ptr;
    T fromVal = T{};
    T toVal = T{};
    bool hasFrom = false;
    float durationFrames = 12.0f;
    Ease easeType = Ease::Linear;
    Animator* owner = nullptr;
};

template <typename T> inline PropertyAnimationBuilder<T> Animate(T& target)
{
    return PropertyAnimationBuilder<T>(target);
}

template <typename T> inline PropertyAnimationBuilder<T> Animate(Animated<T>& target)
{
    return PropertyAnimationBuilder<T>(target.get());
}

} // namespace uiAnimation
