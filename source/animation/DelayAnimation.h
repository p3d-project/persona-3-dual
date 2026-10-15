#pragma once
#include "animation/Animation.h"

namespace uiAnimation
{

class DelayAnimation : public Animation
{
  public:
    explicit DelayAnimation(int ms) : durationFrames(ms * 60.0f / 1000.0f)
    {
    }

  protected:
    bool onUpdate(float dt) override
    {
        elapsed += dt;
        return elapsed >= durationFrames;
    }

  private:
    float durationFrames;
    float elapsed = 0.0f;
};

} // namespace uiAnimation
