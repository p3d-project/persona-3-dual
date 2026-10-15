/**
 * @file AnimationComponent.hpp
 * @brief Per-entity property tweening, bridging `uiAnimation::Animator` into the aegis ECS.
 *
 * @author The P3D Project
 */

#pragma once
#include "animation/Animator.h"
#include "types/aeTypes.hpp"

#include <aegis/component.hpp>

class AnimationComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::Animation);

    void Init() override
    {
        isActive = true;
    }

    void Update(ae::q20_12_t dt) override;

    void Destroy() override;

    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

    /**
     * @brief Starts building a tween for a single value owned by the caller.
     * @see uiAnimation::Animator::animate
     */
    template <typename T> uiAnimation::PropertyAnimationBuilder<T> animate(T& target)
    {
        return animator.animate(target);
    }

    /**
     * @brief Starts building a multi-step (sequenced/parallel) animation.
     * @see uiAnimation::Animator::sequence
     */
    uiAnimation::SequenceBuilder sequence()
    {
        return animator.sequence();
    }

    /**
     * @brief Cancels every animation currently running on this component.
     */
    void cancelAll()
    {
        animator.cancelAll();
    }

  protected:
    void SubmitToManager() override
    {
    }

  private:
    uiAnimation::Animator animator;
};
