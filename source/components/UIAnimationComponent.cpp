#include "UIAnimationComponent.hpp"

void UIAnimationComponent::Update(ae::q20_12_t dt)
{
    animator.update(dt);
}

void UIAnimationComponent::Destroy()
{
    animator.cancelAll();
    isActive = false;
}
