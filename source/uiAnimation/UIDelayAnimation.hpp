/**
 * @file UIDelayAnimation.hpp
 * @brief Animation that waits for a fixed duration before completing.
 * @author Oles Gedz (@olesgedz)
 */

#pragma once

#include "managers/MathManager.hpp"
#include "uiAnimation/UIAnimation.hpp"

/**
 * @brief Holds for @p durationMs milliseconds (converted to Q20.12 seconds).
 */
class UIDelayAnimation : public UIAnimation
{
  public:
    explicit UIDelayAnimation(int durationMs)
    {
        duration = MathManager::GetInstance().div(ae::q20_12_t{durationMs}, ae::q20_12_t{1000});
    }

    void reset() override
    {
        UIAnimation::reset();
    }

  protected:
    bool onUpdate(ae::q20_12_t dt) override
    {
        elapsed += dt;
        return elapsed >= duration;
    }

  private:
    ae::q20_12_t duration{0};
};
