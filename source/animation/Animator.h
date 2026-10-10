#pragma once
#include "animation/Animated.h"
#include "animation/Animation.h"
#include "animation/AnimationHandle.h"
#include "animation/CallbackAnimation.h"
#include "animation/DelayAnimation.h"
#include "animation/PropertyAnimation.h"
#include "animation/SequenceAnimation.h"
#include <etl/vector.h>
#include <functional>
#include <memory>

namespace uiAnimation
{

class Animator
{
  public:
    static constexpr std::size_t MAX_ACTIVE_ANIMATIONS = 8;

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
        if (anim == nullptr || active.full())
        {
            return AnimationHandle();
        }

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
    }

    void cancelAll()
    {
        for (auto& a : active)
            a->cancel();
        active.clear();
    }

  private:
    etl::vector<std::shared_ptr<Animation>, MAX_ACTIVE_ANIMATIONS> active;
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
