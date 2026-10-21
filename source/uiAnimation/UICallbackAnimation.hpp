/**
 * @file UICallbackAnimation.hpp
 * @brief One-shot animation that invokes a callback on its first tick.
 * @author Oles Gedz (olesgedz)
 */

#pragma once

#include "uiAnimation/UIAnimation.hpp"

#include <functional>

/**
 * @brief Runs a void callback once, then completes.
 */
class UICallbackAnimation : public UIAnimation
{
  public:
    explicit UICallbackAnimation(std::function<void()> f) : fn(std::move(f))
    {
    }

  protected:
    bool onUpdate(ae::q20_12_t) override
    {
        if (fn)
        {
            fn();
        }
        return true;
    }

  private:
    std::function<void()> fn;
};
