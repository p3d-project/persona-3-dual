#pragma once
#include "animation/Animation.h"
#include <memory>

namespace uiAnimation
{

class AnimationHandle
{
  public:
    AnimationHandle() = default;
    explicit AnimationHandle(std::shared_ptr<Animation> ptr) : anim(ptr)
    {
    }

    void cancel()
    {
        if (auto a = anim.lock())
            a->cancel();
    }
    void pause()
    {
        if (auto a = anim.lock())
            a->pause();
    }
    void resume()
    {
        if (auto a = anim.lock())
            a->resume();
    }
    void reverse()
    {
        if (auto a = anim.lock())
            a->reverse();
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
    std::weak_ptr<Animation> anim;
};

} // namespace uiAnimation
