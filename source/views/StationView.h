#pragma once

#include "views/EnvironmentView.h"

// data
#include "data/environmentDb.h"
// maps
#include "maps/station.h"

class StationView : public EnvironmentView
{
  public:
    StationView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[3];
    }
    void setMovementConfig() override;
    void setMusic() override;
    ViewState onTileCheck(TileType tile, u32 pressed) override;
    void setTextConfig() override;
    void setupUI() override;

  private:
    // movement and camera
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.02f;
    const float angleIncrement = 0.05f;
    const float distance = 0.7f;
    const float lookAhead = 0.3f;

    // character position
    const Point2D<float> characterTranslate = Point2D<float>(-0.0175f, 1.3216f);
    const float height = 0.0f;
    const float angle = 1.5708f * 2; // 180 degrees (rad)
    const float characterFacingAngle = 180.0f;
};
