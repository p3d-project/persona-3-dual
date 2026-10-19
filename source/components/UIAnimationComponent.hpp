/**
 * @file UIAnimationComponent.hpp
 * @brief Per-entity property tweening, bridging UIAnimator into the aegis ECS.
 *
 * @author The P3D Project
 */

#pragma once

#include "types/aeTypes.hpp"
#include "uiAnimation/UIAnimator.hpp"

#include <aegis/component.hpp>

/**
 * @brief ECS component that owns a UIAnimator for entity-scoped UI tweens.
 */
class UIAnimationComponent : public ae::Component
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
     * @see UIAnimator::animate
     */
    template <typename T> UIPropertyAnimationBuilder<T> animate(T& target)
    {
        return animator.animate(target);
    }

    /**
     * @brief Starts building a multi-step (sequenced/parallel) animation.
     * @see UIAnimator::sequence
     */
    UISequenceBuilder sequence()
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
    UIAnimator animator;
};
