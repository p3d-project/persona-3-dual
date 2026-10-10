#include "AnimationComponent.hpp"

void AnimationComponent::Update(ae::q20_12_t dt)
{
    // uiAnimation durations are authored in 60fps frames; the engine ticks on a fixed
    // 1/60s timestep, so convert dt (seconds) back into an equivalent frame count.
    animator.update(static_cast<float>(dt) * 60.0f);
}

void AnimationComponent::Destroy()
{
    animator.cancelAll();
    isActive = false;
}
