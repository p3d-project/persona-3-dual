/**
 * @file UIAnimator.hpp
 * @brief Runs a fixed-capacity set of concurrent UI animations.
 * @author Oles Gedz (@olesgedz)
 */

#pragma once

#include "uiAnimation/UIAnimated.hpp"
#include "uiAnimation/UIAnimation.hpp"
#include "uiAnimation/UIAnimationHandle.hpp"
#include "uiAnimation/UICallbackAnimation.hpp"
#include "uiAnimation/UIDelayAnimation.hpp"
#include "uiAnimation/UIPropertyAnimation.hpp"
#include "uiAnimation/UISequenceAnimation.hpp"

#include <etl/vector.h>
#include <functional>
#include <memory>

/**
 * @brief Owns and ticks active UI animations. Safe against cancel-during-tick.
 */
class UIAnimator
{
  public:
    static constexpr std::size_t MAX_ACTIVE_ANIMATIONS = 8;

    template <typename T> UIPropertyAnimationBuilder<T> animate(T& target)
    {
        return UIPropertyAnimationBuilder<T>(target, this);
    }

    template <typename T> UIPropertyAnimationBuilder<T> animate(UIAnimated<T>& target)
    {
        return UIPropertyAnimationBuilder<T>(target.get(), this);
    }

    UISequenceBuilder sequence()
    {
        return UISequenceBuilder(this);
    }

    UIAnimationHandle play(std::shared_ptr<UIAnimation> anim)
    {
        if (anim == nullptr)
        {
            return UIAnimationHandle();
        }

        if (updating)
        {
            if (!pendingPlay.full())
            {
                pendingPlay.push_back(anim);
            }
            return UIAnimationHandle(std::move(anim));
        }

        if (active.full())
        {
            return UIAnimationHandle();
        }

        active.push_back(anim);
        return UIAnimationHandle(std::move(anim));
    }

    std::shared_ptr<UIAnimation> call(std::function<void()> fn)
    {
        return std::make_shared<UICallbackAnimation>(std::move(fn));
    }

    std::shared_ptr<UIAnimation> delay(int ms)
    {
        return std::make_shared<UIDelayAnimation>(ms);
    }

    /**
     * @brief Advances all active animations by @p dt seconds (Q20.12).
     */
    void update(ae::q20_12_t dt)
    {
        updating = true;

        for (auto it = active.begin(); it != active.end();)
        {
            if ((*it)->tick(dt))
            {
                it = active.erase(it);
            }
            else
            {
                ++it;
            }
        }

        updating = false;

        if (pendingCancelAll)
        {
            pendingCancelAll = false;
            for (auto& a : active)
            {
                a->cancel();
            }
            active.clear();
            pendingPlay.clear();
            return;
        }

        for (auto& anim : pendingPlay)
        {
            if (!active.full() && anim != nullptr && !anim->isCancelled() && !anim->isDone())
            {
                active.push_back(anim);
            }
        }
        pendingPlay.clear();
    }

    void cancelAll()
    {
        for (auto& a : active)
        {
            a->cancel();
        }

        if (updating)
        {
            pendingCancelAll = true;
            return;
        }

        active.clear();
        pendingPlay.clear();
    }

  private:
    etl::vector<std::shared_ptr<UIAnimation>, MAX_ACTIVE_ANIMATIONS> active;
    etl::vector<std::shared_ptr<UIAnimation>, MAX_ACTIVE_ANIMATIONS> pendingPlay;
    bool updating = false;
    bool pendingCancelAll = false;
};

template <typename T> UIAnimationHandle UIPropertyAnimationBuilder<T>::start()
{
    if (owner == nullptr)
    {
        return UIAnimationHandle();
    }
    return owner->play(build());
}

inline UIAnimationHandle UISequenceBuilder::start()
{
    if (owner == nullptr)
    {
        return UIAnimationHandle();
    }
    return owner->play(seq);
}
