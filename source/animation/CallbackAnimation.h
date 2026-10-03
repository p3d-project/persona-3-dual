#pragma once
#include "animation/Animation.h"
#include <functional>

namespace uiAnimation
{

class CallbackAnimation : public Animation
{
  public:
    explicit CallbackAnimation(std::function<void()> f) : fn(std::move(f))
    {
    }

  protected:
    bool onUpdate(float) override
    {
        fn();
        return true;
    }

  private:
    std::function<void()> fn;
};

} // namespace uiAnimation
