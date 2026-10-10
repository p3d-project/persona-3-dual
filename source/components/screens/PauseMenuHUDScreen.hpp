#pragma once
#include "components/GraphicsComponent.hpp"
#include "components/screens/UIScreen.hpp"

#include <etl/array.h>
#include <nds.h>

class PauseMenuHUDScreen : public UIScreen
{
  public:
    static void create();
    static void destroy();
    static PauseMenuHUDScreen* getInstance();

    void load();
    void unload();
    void renderSprites() override;
    int onTouch(touchPosition* touch) override;

  private:
    PauseMenuHUDScreen() : UIScreen(false) {};
    ~PauseMenuHUDScreen() {};
    static PauseMenuHUDScreen* instance;

    void loadBackground();
    void renderBackground();
    void unloadBackground();

    ae::Entity* pauseMenuHUD = nullptr;
    GraphicsComponent* graphics = nullptr;

    // background
    const std::string bgPath = "graphics/MenuHUD/backgrounds/";

    GraphicAsset bgHUD = {};

    // sprite setup
    const std::string spritePath = "graphics/PauseMenuHUD/sprites/";

    // status sprite
    Sprite statusSprite = {SpriteSize_32x32, SpriteColorFormat_16Color, 0};
    GraphicAsset statusGraphic = {};
    SpriteRenderState srs0 = {statusSprite, 202, -15, 1, 0, false, false, false, true};
    SpritePayload sp0 = {srs0, spritePath, statusGraphic, SpriteType::STATUS, (int)StatusSprite::STATUS_GREAT};

    // selected option sprites
    Sprite selectedOptionSprites[4] = {{SpriteSize_32x16, SpriteColorFormat_16Color, 0},
                                       {SpriteSize_32x16, SpriteColorFormat_16Color, 0},
                                       {SpriteSize_32x16, SpriteColorFormat_16Color, 0},
                                       {SpriteSize_32x16, SpriteColorFormat_16Color, 0}};
    GraphicAsset selectedOptionGraphics[4] = {};

    SpriteRenderState srs1 = {selectedOptionSprites[0], 165, -5, 1, 0, false, false, false, true};
    SpritePayload sp1 = {srs1,
                         spritePath,
                         selectedOptionGraphics[0],
                         SpriteType::SELECTED_OPTION,
                         (int)SelectedOptionSprite::SELECTED_OPTION_0};

    SpriteRenderState srs2 = {selectedOptionSprites[1], -11, 141, 1, 0, false, false, false, true};
    SpritePayload sp2 = {srs2,
                         spritePath,
                         selectedOptionGraphics[1],
                         SpriteType::SELECTED_OPTION,
                         (int)SelectedOptionSprite::SELECTED_OPTION_1};

    SpriteRenderState srs3 = {selectedOptionSprites[2], 15, 141, 1, 0, false, false, false, true};
    SpritePayload sp3 = {srs3,
                         spritePath,
                         selectedOptionGraphics[2],
                         SpriteType::SELECTED_OPTION,
                         (int)SelectedOptionSprite::SELECTED_OPTION_2};

    SpriteRenderState srs4 = {selectedOptionSprites[3], 54, 141, 1, 0, false, false, false, true};
    SpritePayload sp4 = {srs4,
                         spritePath,
                         selectedOptionGraphics[3],
                         SpriteType::SELECTED_OPTION,
                         (int)SelectedOptionSprite::SELECTED_OPTION_3};

    // data groups
    etl::array<SpritePayload, 5> spritePayloads = {sp0, sp1, sp2, sp3, sp4};

    etl::array<SpriteTransform, 0> spriteTransforms = {};
    // ---
};
