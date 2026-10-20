/**
 * @file UISequenceAnimation.hpp
 * @brief Ordered groups of UI animations (sequential steps, parallel joins).
 * @author Oles Gedz (olesgedz)
 */

#pragma once

#include "uiAnimation/UIAnimation.hpp"
#include "uiAnimation/UIAnimationHandle.hpp"

#include <etl/vector.h>
#include <memory>
#include <type_traits>

class UIAnimator;

/**
 * @brief Plays groups of animations: each step waits until all in the group finish.
 */
class UISequenceAnimation : public UIAnimation
{
  public:
    static constexpr std::size_t MAX_STEPS = 8;
    static constexpr std::size_t MAX_ANIMATIONS_PER_STEP = 4;
    using Group = etl::vector<std::shared_ptr<UIAnimation>, MAX_ANIMATIONS_PER_STEP>;

    void appendGroup(std::shared_ptr<UIAnimation> anim)
    {
        if (steps.full())
        {
            return;
        }

        Group group;
        group.push_back(std::move(anim));
        steps.push_back(std::move(group));
    }

    void joinGroup(std::shared_ptr<UIAnimation> anim)
    {
        if (steps.empty())
        {
            appendGroup(std::move(anim));
            return;
        }

        if (!steps.back().full())
        {
            steps.back().push_back(std::move(anim));
        }
    }

    void reverse() override
    {
        // Not reversing whole sequences; individual property steps handle reverse.
    }

    void reset() override
    {
        UIAnimation::reset();
        currentStep = 0;
        for (auto& group : steps)
        {
            for (auto& anim : group)
            {
                if (anim)
                {
                    anim->reset();
                }
            }
        }
    }

  protected:
    bool onUpdate(ae::q20_12_t dt) override
    {
        if (currentStep >= static_cast<int>(steps.size()))
        {
            return true;
        }

        Group& group = steps[currentStep];
        bool allDone = true;
        for (auto& anim : group)
        {
            if (anim && !anim->tick(dt))
            {
                allDone = false;
            }
        }
        if (allDone)
        {
            ++currentStep;
        }

        return currentStep >= static_cast<int>(steps.size());
    }

  private:
    etl::vector<Group, MAX_STEPS> steps;
    int currentStep = 0;
};

/**
 * @brief Fluent builder for UISequenceAnimation. Requires a UIAnimator to start.
 */
class UISequenceBuilder
{
  public:
    explicit UISequenceBuilder(UIAnimator* animOwner = nullptr)
        : seq(std::make_shared<UISequenceAnimation>()), owner(animOwner)
    {
    }

    template <typename Builder>
    auto append(Builder&& b)
        -> std::enable_if_t<!std::is_convertible_v<Builder, std::shared_ptr<UIAnimation>>, UISequenceBuilder&>
    {
        seq->appendGroup(std::forward<Builder>(b).build());
        return *this;
    }

    template <typename Builder>
    auto join(Builder&& b)
        -> std::enable_if_t<!std::is_convertible_v<Builder, std::shared_ptr<UIAnimation>>, UISequenceBuilder&>
    {
        seq->joinGroup(std::forward<Builder>(b).build());
        return *this;
    }

    UISequenceBuilder& append(std::shared_ptr<UIAnimation> anim)
    {
        seq->appendGroup(std::move(anim));
        return *this;
    }

    UISequenceBuilder& join(std::shared_ptr<UIAnimation> anim)
    {
        seq->joinGroup(std::move(anim));
        return *this;
    }

    UISequenceBuilder& repeat(UIRepeatMode mode, int count = -1)
    {
        seq->setRepeat(mode, count);
        return *this;
    }

    std::shared_ptr<UIAnimation> build()
    {
        return seq;
    }

    UIAnimationHandle start();

  private:
    std::shared_ptr<UISequenceAnimation> seq;
    UIAnimator* owner;
};
