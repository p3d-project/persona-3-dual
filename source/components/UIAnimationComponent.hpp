/**
 * @file UIAnimationComponent.hpp
 * @brief Per-entity property tweening, bridging UIAnimator into the aegis ECS.
 *
 * @author Oles Gedz (olesgedz)
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

    void init() override
    {
        active = true;
    }

    void update(ae::q20_12_t dt) override;

    void destroy() override;

    ae::ComponentTypeID getType() const override
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
    void submitToManager() override
    {
    }

  private:
    UIAnimator animator;
};
