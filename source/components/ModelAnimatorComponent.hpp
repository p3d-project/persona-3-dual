/**
 * @file ModelAnimatorComponent.hpp
 * @brief Contains animation data for an associated model component.
 * @author Gregory Munro (ggmini)
 */

#pragma once

#include "managers/RenderManager.hpp"
#include "types/aeTypes.hpp"

#include <aegis/component.hpp>
#include <memory>
#include <string>
#include <vector>

class ModelAnimatorComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::ModelAnimator);

    void Init() override
    {
    }

    void Destroy() override
    {
    }

    /**
     * @brief Increments the current frame counter.
     */
    void Update(ae::q20_12_t /*dt*/) override;

    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

    bool loadAnimations(FILE* file, uint32_t animCount);

    void applyTransformationForNode(int nodeIndex);

  protected:
    void SubmitToManager() override
    {
    }

  private:
    std::vector<Animation_N> animations;

    const NodeTrack* findNodeTrack(const Animation_N* anim, int32_t nodeIndex) const;

    uint32_t currentAnimationIndex = 0;
    uint32_t currentFrame = 0;

    RenderManager& render = RenderManager::GetInstance();
};
