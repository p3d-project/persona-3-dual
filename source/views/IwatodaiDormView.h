#pragma once

#include "animation/Animator.h"
#include "views/EnvironmentView.h"

// data
#include "data/environmentDb.h"
// maps
#include "maps/iwatodai_dorm_floor_1.h"
// dialogue
#include "dialogue/demo_dialogue.h"

class IwatodaiDormView : public EnvironmentView
{
  public:
    // TODO: dont forget to clear in future
    IwatodaiDormView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[0];
    }
    void setMovementConfig() override;
    void setMusic() override;
    ViewState onTileCheck(TileType tile, u32 pressed) override;
    void setDialogueConfig() override;
    void setCameraConfig() override;
    void onEnvironmentUpdate() override;
    void cleanup() override;
    void setTextConfig() override;
    void setupUI() override;

  private:
    // movement and camera
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.03f;

    // character position
    const Point2D<float> characterTranslate = Point2D<float>(0.4f, 2.8f);
    const float height = 0.0f;
    const float characterFacingAngle = 180.0f;

    // test animation
    uiAnimation::Animator animator;
    float triSize = 10.0f;
    uiAnimation::AnimationHandle circleHandle;
};
