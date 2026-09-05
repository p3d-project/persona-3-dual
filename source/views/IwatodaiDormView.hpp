#pragma once
#include "views/EnvironmentView.hpp"

class IwatodaiDormView : public EnvironmentView
{
  public:
    IwatodaiDormView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[0];
    }
    void setupMovement() override;
    void setupMusic() override;
    ViewState onTileCheck(TileType tile, u32 pressed) override;
    void setupDialogue() override;
    void setupCamera() override;
    void setupText() override;
    void setupUI() override;

  private:
    // movement and camera
    const Point2D<ae::q20_12_t> characterSize{ae::q20_12_t{0.1}, ae::q20_12_t{0.1}};
    const ae::q20_12_t speed{0.03};

    // character position
    const Point2D<ae::q20_12_t> characterTranslate{ae::q20_12_t{0.4}, ae::q20_12_t{2.8}};
    const ae::q20_12_t height{0};
    const ae::q20_12_t characterFacingAngle{180};

    std::string fontNameAlt = "noto-sans-jp-black";
    int fontSizeAlt = 16;
};
