#pragma once
#include "animation/Animation.h"
#include "animation/AnimationHandle.h"
#include <memory>
#include <type_traits>
#include <vector>

namespace uiAnimation
{

class Animator;

class SequenceAnimation : public Animation
{
  public:
    using Group = std::vector<std::shared_ptr<Animation>>;

    void appendGroup(std::shared_ptr<Animation> anim)
    {
        steps.push_back({std::move(anim)});
    }

    void joinGroup(std::shared_ptr<Animation> anim)
    {
        if (steps.empty())
        {
            appendGroup(std::move(anim));
            return;
        }
        steps.back().push_back(std::move(anim));
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
    std::vector<Group> steps;
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
