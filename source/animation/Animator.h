#pragma once
#include "animation/Animated.h"
#include "animation/Animation.h"
#include "animation/AnimationHandle.h"
#include "animation/CallbackAnimation.h"
#include "animation/DelayAnimation.h"
#include "animation/PropertyAnimation.h"
#include "animation/SequenceAnimation.h"
#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace uiAnimation
{

class Animator
{
  public:
    template <typename T> PropertyAnimationBuilder<T> animate(T& target)
    {
        return PropertyAnimationBuilder<T>(target, this);
    }

    template <typename T> PropertyAnimationBuilder<T> animate(Animated<T>& target)
    {
        return PropertyAnimationBuilder<T>(target.get(), this);
    }

    SequenceBuilder sequence()
    {
        return SequenceBuilder(this);
    }

    AnimationHandle play(std::shared_ptr<Animation> anim)
    {
        active.push_back(anim);
        return AnimationHandle(std::move(anim));
    }

    std::shared_ptr<Animation> call(std::function<void()> fn)
    {
        return std::make_shared<CallbackAnimation>(std::move(fn));
    }

    std::shared_ptr<Animation> delay(int ms)
    {
        return std::make_shared<DelayAnimation>(ms);
    }

    void update(float dt)
    {
        active.erase(std::remove_if(active.begin(),
                                    active.end(),
                                    [dt](const std::shared_ptr<Animation>& a) { return a->tick(dt); }),
                     active.end());
    }

    void cancelAll()
    {
        for (auto& a : active)
            a->cancel();
        active.clear();
    }

  private:
    std::vector<std::shared_ptr<Animation>> active;
};

template <typename T> AnimationHandle PropertyAnimationBuilder<T>::start()
{
    return owner->play(build());
}

inline AnimationHandle SequenceBuilder::start()
{
    return owner->play(seq);
}

} // namespace uiAnimation
