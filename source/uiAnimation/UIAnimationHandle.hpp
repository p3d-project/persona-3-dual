/**
 * @file UIAnimationHandle.hpp
 * @brief Weak handle used by callers to cancel/pause/query a running UI animation.
 * @author Oles Gedz (@olesgedz)
 */

#pragma once

#include "uiAnimation/UIAnimation.hpp"

#include <memory>

/**
 * @brief Non-owning control handle for a UIAnimation managed by UIAnimator.
 */
class UIAnimationHandle
{
  public:
    UIAnimationHandle() = default;

    explicit UIAnimationHandle(std::shared_ptr<UIAnimation> ptr) : anim(ptr)
    {
    }

    void cancel()
    {
        if (auto a = anim.lock())
        {
            a->cancel();
        }
    }

    void pause()
    {
        if (auto a = anim.lock())
        {
            a->pause();
        }
    }

    void resume()
    {
        if (auto a = anim.lock())
        {
            a->resume();
        }
    }

    void reverse()
    {
        if (auto a = anim.lock())
        {
            a->reverse();
        }
    }

    bool isValid() const
    {
        return !anim.expired();
    }

    bool isPlaying() const
    {
        auto a = anim.lock();
        return a && !a->isCancelled() && !a->isPaused() && !a->isDone();
    }

  private:
    std::weak_ptr<UIAnimation> anim;
};
