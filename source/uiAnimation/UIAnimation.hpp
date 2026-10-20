/**
 * @file UIAnimation.hpp
 * @brief Base class for one UI animation step (property, delay, callback, sequence).
 * @author Oles Gedz (olesgedz)
 */

#pragma once

#include <aegis/types.hpp>

/**
 * @brief How an animation behaves after reaching its end.
 */
enum class UIRepeatMode
{
    Once,    ///< Play once, then complete.
    Loop,    ///< Restart from the beginning.
    PingPong ///< Reverse direction each cycle.
};

/**
 * @brief Abstract UI animation advanced once per frame by UIAnimator.
 */
class UIAnimation
{
  public:
    virtual ~UIAnimation() = default;

    UIAnimation(const UIAnimation&) = delete;
    UIAnimation& operator=(const UIAnimation&) = delete;
    UIAnimation(UIAnimation&&) = delete;
    UIAnimation& operator=(UIAnimation&&) = delete;

    /**
     * @brief Advances the animation by @p dt seconds (Q20.12).
     * @return true if the animation is finished and may be removed.
     */
    bool tick(ae::q20_12_t dt)
    {
        if (done || cancelled)
        {
            return true;
        }
        if (!paused)
        {
            done = onUpdate(dt);
            if (done && applyRepeat())
            {
                done = false;
            }
        }
        return done || cancelled;
    }

    virtual void reverse()
    {
    }

    /**
     * @brief Resets runtime state so the animation can play again (used by Loop).
     */
    virtual void reset()
    {
        elapsed = ae::q20_12_t{0};
        done = false;
        cancelled = false;
        paused = false;
    }

    void pause()
    {
        paused = true;
    }

    void resume()
    {
        paused = false;
    }

    void cancel()
    {
        cancelled = true;
    }

    bool isCancelled() const
    {
        return cancelled;
    }

    bool isPaused() const
    {
        return paused;
    }

    bool isDone() const
    {
        return done;
    }

    void setRepeat(UIRepeatMode mode, int count = -1)
    {
        repeatMode = mode;
        remainingRepeats = count;
    }

  protected:
    UIAnimation() = default;

    /**
     * @brief Per-frame update.
     * @return true when this cycle has completed.
     */
    virtual bool onUpdate(ae::q20_12_t dt) = 0;

    /**
     * @brief Applies repeat policy after a completed cycle.
     * @return true if the animation should continue playing.
     */
    bool applyRepeat()
    {
        if (repeatMode == UIRepeatMode::Once)
        {
            return false;
        }

        if (remainingRepeats == 0)
        {
            return false;
        }
        if (remainingRepeats > 0)
        {
            --remainingRepeats;
        }

        if (repeatMode == UIRepeatMode::PingPong)
        {
            reverse();
            elapsed = ae::q20_12_t{0};
            cancelled = false;
            done = false;
            return true;
        }

        // Loop
        reset();
        return true;
    }

    bool paused = false;
    bool cancelled = false;
    bool done = false;
    ae::q20_12_t elapsed{0};
    UIRepeatMode repeatMode = UIRepeatMode::Once;
    int remainingRepeats = -1; ///< -1 = infinite when mode is not Once.
};
