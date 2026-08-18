#pragma once

#include "views/EnvironmentView.h"

// data
#include "data/environmentDb.h"
// maps
#include "maps/paulownia_mall.h"

class PaulowniaMallView : public EnvironmentView
{
  public:
    PaulowniaMallView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[2];
    }
    float getCameraYOffset() const override
    {
        return 0.3f;
    }
    void setMovementConfig() override;
    void setMusic() override;
    ViewState onTileCheck(TileType tile, u32 pressed) override;
    void setTextConfig() override;
    void setupUI() override;

  private:
    // movement and camera
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.03f;

    // character position
    const Point2D<float> characterTranslate = Point2D<float>(0.0122f, 2.3355f);
    const float height = 0.2f;
    const float characterFacingAngle = 180.0f;
};
