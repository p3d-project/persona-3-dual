#pragma once
#include <aegis/types.hpp>

#include "views/EnvironmentView.hpp"

#include "data/environmentDb.hpp"
#include "maps/paulownia_mall.hpp"

class PaulowniaMallView : public EnvironmentView
{
  public:
    PaulowniaMallView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[2];
    }
    ae::q20_12_t getCameraYOffset() const override
    {
        return ae::q20_12_t{0.3};
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
    const ae::q20_12_t speed{0.03};

    // character position
    const Point2D<ae::q20_12_t> characterTranslate{ae::q20_12_t{0.0122}, ae::q20_12_t{2.3355}};
    const ae::q20_12_t height{0.2};
    const ae::q20_12_t characterFacingAngle{180};
};
