#pragma once
#include "components/ui/UIScreen.h"
#include "core/enums.h"
#include "core/globals.h"
#include "core/structs.h"
#include <nds.h>

#include "components/GraphicsComponent.hpp"

class MenuHUDScreen : public UIScreen
{
  public:
    static void create();
    static void destroy();
    static MenuHUDScreen* getInstance();

    void load();
    void unload();
    void renderSprites() override;
    void removeSprites() override;
    int onTouch(touchPosition* touch) override;
    void tick();

  private:
    MenuHUDScreen() : UIScreen(false) {};
    ~MenuHUDScreen() {};
    static MenuHUDScreen* instance;

    // NOTE: we can have max:
    // 1 moon
    // 1 day of the week
    // 4 numbers
    // 4 times
    // 18 skill progress items (all same sprite)

    // sprites
    Sprite sprites[28]; // enough entries for moon, day, digits, times, and repeated skill markers
    GraphicAsset moonSprite;
    GraphicAsset dayOfWeekSprite;
    GraphicAsset numberSprites[4];
    GraphicAsset timeSprites[4];
    GraphicAsset skillSprites[18];
    GraphicAsset slashSprite;

    static constexpr int kAnimSlot = 12;
    static constexpr int kAnimAffine = 31;
    static constexpr int kAnimFrames = 6;
    u16* animGfx[kAnimFrames] = {};
    GraphicAsset animAsset[kAnimFrames] = {};
    int animX = 176;
    int animY = 80;
    bool animReady = false;

    bool bgLoaded;
    void loadBackground();

    ae::Entity* menuHUD = nullptr;
    GraphicsComponent* graphics = nullptr;
};
