#include "UIAnimationComponent.hpp"

void UIAnimationComponent::update(ae::q20_12_t dt)
{
    animator.update(dt);
}

void UIAnimationComponent::destroy()
{
    animator.cancelAll();
    active = false;
}
