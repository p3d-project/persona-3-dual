#pragma once
#include "views/EnvironmentView.hpp"

#include "data/environmentDb.hpp"
#include "maps/station.hpp"

class StationView : public EnvironmentView
{
  public:
    StationView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[3];
    }
    void setupMovement() override;
    void setupMusic() override;
    ViewState onTileCheck(TileType tile, u32 pressed) override;
    void setupText() override;
    void setupUI() override;
    void setupCamera() override;

  private:
    // movement and camera
    const Point2D<ae::q20_12_t> characterSize{ae::q20_12_t{0.1}, ae::q20_12_t{0.1}};
    const ae::q20_12_t speed{0.02};
    const ae::q20_12_t angleIncrement{0.05};
    const ae::q20_12_t distance{0.7};
    const ae::q20_12_t lookAhead{0.3};

    // character position
    const Point2D<ae::q20_12_t> characterTranslate{ae::q20_12_t{-0.0175}, ae::q20_12_t{1.3216}};
    const ae::q20_12_t height{0};
    const ae::q20_12_t angle{1.5708 * 2}; // 180 degrees (rad)
    const ae::q20_12_t characterFacingAngle{180};
};
