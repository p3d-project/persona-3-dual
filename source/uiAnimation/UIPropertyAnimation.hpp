/**
 * @file UIPropertyAnimation.hpp
 * @brief Tweens a single target value over time with easing and optional repeat.
 * @author Oles Gedz (olesgedz)
 */

#pragma once

#include "core/geometry.hpp"
#include "managers/MathManager.hpp"
#include "uiAnimation/UIAnimated.hpp"
#include "uiAnimation/UIAnimation.hpp"
#include "uiAnimation/UIAnimationHandle.hpp"
#include "uiAnimation/UIEase.hpp"

#include <memory>
#include <type_traits>
#include <utility>

class UIAnimator;

/**
 * @brief Interpolates *target from @p fromVal to @p toVal over @p duration seconds.
 */
template <typename T> class UIPropertyAnimation : public UIAnimation
{
  public:
    UIPropertyAnimation(T* tgt, const T& fromVal, const T& toVal, ae::q20_12_t durationSec, UIEase easeMode)
        : target(tgt), from(fromVal), to(toVal), duration(durationSec), ease(easeMode)
    {
    }

    void reverse() override
    {
        std::swap(from, to);
        elapsed = ae::q20_12_t{0};
        done = false;
        cancelled = false;
    }

    void reset() override
    {
        UIAnimation::reset();
    }

  protected:
    bool onUpdate(ae::q20_12_t dt) override
    {
        MathManager& math = MathManager::GetInstance();
        elapsed += dt;
        ae::q20_12_t t = ae::q20_12_t{1};
        if (duration > ae::q20_12_t{0})
        {
            t = math.clamp(math.div(elapsed, duration), ae::q20_12_t{0}, ae::q20_12_t{1});
        }
        t = math.ease(ease, t);
        *target = lerpValue(from, to, t);
        return elapsed >= duration;
    }

  private:
    static T lerpValue(const T& a, const T& b, ae::q20_12_t t)
    {
        MathManager& math = MathManager::GetInstance();
        if constexpr (std::is_same_v<T, int>)
        {
            return math.lerp(a, b, t);
        }
        else if constexpr (std::is_same_v<T, ae::q20_12_t>)
        {
            return math.lerp(a, b, t);
        }
        else if constexpr (std::is_same_v<T, Point2D<int>>)
        {
            return Point2D<int>(math.lerp(a.x, b.x, t), math.lerp(a.z, b.z, t));
        }
        else if constexpr (std::is_same_v<T, Point2D<ae::q20_12_t>>)
        {
            return Point2D<ae::q20_12_t>(math.lerp(a.x, b.x, t), math.lerp(a.z, b.z, t));
        }
        else
        {
            static_assert(sizeof(T) == 0, "UIPropertyAnimation: unsupported type");
            return a;
        }
    }

    T* target;
    T from;
    T to;
    ae::q20_12_t duration{0};
    UIEase ease = UIEase::Linear;
};

/**
 * @brief Fluent builder for UIPropertyAnimation. Must be started via a UIAnimator owner.
 */
template <typename T> class UIPropertyAnimationBuilder
{
  public:
    explicit UIPropertyAnimationBuilder(T& tgt, UIAnimator* animOwner = nullptr) : ptr(&tgt), owner(animOwner)
    {
    }

    UIPropertyAnimationBuilder& from(const T& v)
    {
        fromVal = v;
        hasFrom = true;
        return *this;
    }

    UIPropertyAnimationBuilder& to(const T& v)
    {
        toVal = v;
        return *this;
    }

    /**
     * @brief Duration in milliseconds.
     */
    UIPropertyAnimationBuilder& duration(int ms)
    {
        durationMs = ms;
        return *this;
    }

    UIPropertyAnimationBuilder& ease(UIEase e)
    {
        easeType = e;
        return *this;
    }

    UIPropertyAnimationBuilder& repeat(UIRepeatMode mode, int count = -1)
    {
        repeatMode = mode;
        repeatCount = count;
        return *this;
    }

    UIPropertyAnimationBuilder& loop(int count = -1)
    {
        return repeat(UIRepeatMode::Loop, count);
    }

    UIPropertyAnimationBuilder& pingPong(int count = -1)
    {
        return repeat(UIRepeatMode::PingPong, count);
    }

    std::shared_ptr<UIAnimation> build() const
    {
        MathManager& math = MathManager::GetInstance();
        const ae::q20_12_t durationSec = math.div(ae::q20_12_t{durationMs}, ae::q20_12_t{1000});
        T f = hasFrom ? fromVal : *ptr;
        auto anim = std::make_shared<UIPropertyAnimation<T>>(ptr, f, toVal, durationSec, easeType);
        anim->setRepeat(repeatMode, repeatCount);
        return anim;
    }

    UIAnimationHandle start();

  private:
    T* ptr;
    T fromVal = T{};
    T toVal = T{};
    bool hasFrom = false;
    int durationMs = 200;
    UIEase easeType = UIEase::Linear;
    UIRepeatMode repeatMode = UIRepeatMode::Once;
    int repeatCount = -1;
    UIAnimator* owner = nullptr;
};
