#pragma once
#include "animation/Animation.h"
#include "animation/AnimationHandle.h"
#include <etl/vector.h>
#include <memory>
#include <type_traits>

namespace uiAnimation
{

class Animator;

class SequenceAnimation : public Animation
{
  public:
    static constexpr std::size_t MAX_STEPS = 8;
    static constexpr std::size_t MAX_ANIMATIONS_PER_STEP = 4;
    using Group = etl::vector<std::shared_ptr<Animation>, MAX_ANIMATIONS_PER_STEP>;

    void appendGroup(std::shared_ptr<Animation> anim)
    {
        if (steps.full())
        {
            return;
        }

        Group group;
        group.push_back(std::move(anim));
        steps.push_back(std::move(group));
    }

    void joinGroup(std::shared_ptr<Animation> anim)
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

  protected:
    bool onUpdate(float dt) override
    {
        if (currentStep >= static_cast<int>(steps.size()))
            return true;

        Group& group = steps[currentStep];
        bool allDone = true;
        for (auto& anim : group)
        {
            if (!anim->tick(dt))
                allDone = false;
        }
        if (allDone)
            ++currentStep;

        return currentStep >= static_cast<int>(steps.size());
    }

  private:
    etl::vector<Group, MAX_STEPS> steps;
    int currentStep = 0;
};

class SequenceBuilder
{
  public:
    explicit SequenceBuilder(Animator* owner = nullptr) : seq(std::make_shared<SequenceAnimation>()), owner(owner)
    {
    }

    template <typename Builder>
    auto append(Builder&& b)
        -> std::enable_if_t<!std::is_convertible_v<Builder, std::shared_ptr<Animation>>, SequenceBuilder&>
    {
        seq->appendGroup(std::forward<Builder>(b).build());
        return *this;
    }

    template <typename Builder>
    auto join(Builder&& b)
        -> std::enable_if_t<!std::is_convertible_v<Builder, std::shared_ptr<Animation>>, SequenceBuilder&>
    {
        seq->joinGroup(std::forward<Builder>(b).build());
        return *this;
    }

    SequenceBuilder& append(std::shared_ptr<Animation> anim)
    {
        seq->appendGroup(std::move(anim));
        return *this;
    }

    SequenceBuilder& join(std::shared_ptr<Animation> anim)
    {
        seq->joinGroup(std::move(anim));
        return *this;
    }

    std::shared_ptr<Animation> build()
    {
        return seq;
    }

    AnimationHandle start();

  private:
    std::shared_ptr<SequenceAnimation> seq;
    Animator* owner;
};

inline SequenceBuilder sequence()
{
    return SequenceBuilder(nullptr);
}

} // namespace uiAnimation
