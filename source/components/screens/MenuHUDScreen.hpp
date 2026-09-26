#pragma once
#include "components/GraphicsComponent.hpp"
#include "components/screens/UIScreen.hpp"

#include <etl/array.h>
#include <nds.h>

class MenuHUDScreen : public UIScreen
{
  public:
    static void create();
    static void destroy();
    static MenuHUDScreen* getInstance();

    void load();
    void unload();
    void renderSprites() override;
    int onTouch(touchPosition* touch) override;

  private:
    MenuHUDScreen() : UIScreen(false) {};
    ~MenuHUDScreen() {};
    static MenuHUDScreen* instance;

    void loadBackground();
    void renderBackground();
    void unloadBackground();

    ae::Entity* menuHUD = nullptr;
    GraphicsComponent* graphics = nullptr;

    // background
    const std::string bgPath = "graphics/MenuHUD/backgrounds/";
    GraphicAsset bgHUD = {};

    // ---
    // sprite setup
    // TODO: make const!
    std::string spritePath = "graphics/MenuHUD/sprites/";

    // moon sprite
    Sprite moonSprite = {SpriteSize_32x32, SpriteColorFormat_256Color, 0};
    GraphicAsset moonGraphic = {};
    SpriteRenderState srs0 = {moonSprite, 202, -15, 1, 0, false, false, false, true};
    SpritePayload sp0 = {srs0, spritePath, moonGraphic, SpriteType::MOON, (int)MoonSprite::MOON_22};

    // day of week sprite
    Sprite dayOfWeekSprite = {SpriteSize_32x32, SpriteColorFormat_256Color, 1};
    GraphicAsset dayOfWeekGraphic = {};
    SpriteRenderState srs1 = {dayOfWeekSprite, 134, 143, 1, 0, false, false, false, true};
    SpritePayload sp1 = {srs1, spritePath, dayOfWeekGraphic, SpriteType::DAY_OF_WEEK, (int)DayOfWeekSprite::TUESDAY};

    // digit sprites
    Sprite digitSprites[3] = {{SpriteSize_32x32, SpriteColorFormat_256Color, 2},
                              {SpriteSize_32x32, SpriteColorFormat_256Color, 2},
                              {SpriteSize_32x32, SpriteColorFormat_256Color, 2}};
    GraphicAsset digitGraphics[3] = {};

    SpriteRenderState srs2 = {digitSprites[0], -11, 141, 1, 0, false, false, false, true};
    SpritePayload sp2 = {srs2, spritePath, digitGraphics[0], SpriteType::DIGIT, (int)DigitSprite::DIGIT_0};

    SpriteRenderState srs3 = {digitSprites[1], 15, 141, 1, 0, false, false, false, true};
    SpritePayload sp3 = {srs3, spritePath, digitGraphics[1], SpriteType::DIGIT, (int)DigitSprite::DIGIT_4};

    SpriteRenderState srs4 = {digitSprites[0], 54, 141, 1, 0, false, false, false, true};
    SpritePayload sp4 = {srs4, spritePath, digitGraphics[0], SpriteType::DIGIT, (int)DigitSprite::DIGIT_0};

    SpriteRenderState srs5 = {digitSprites[2], 80, 141, 1, 0, false, false, false, true};
    SpritePayload sp5 = {srs5, spritePath, digitGraphics[2], SpriteType::DIGIT, (int)DigitSprite::DIGIT_7};

    // time sprites
    Sprite timeSprites[4] = {{SpriteSize_64x32, SpriteColorFormat_256Color, 3},
                             {SpriteSize_64x32, SpriteColorFormat_256Color, 4},
                             {SpriteSize_64x32, SpriteColorFormat_256Color, 5},
                             {SpriteSize_64x32, SpriteColorFormat_256Color, 6}};
    GraphicAsset timeGraphics[4] = {};

    SpriteRenderState srs6 = {timeSprites[0], -27, -5, 1, 0, false, false, false, true};
    SpritePayload sp6 = {srs6, spritePath, timeGraphics[0], SpriteType::TIME, (int)TimeSprite::EARLY_MORNING_0_0};

    SpriteRenderState srs7 = {timeSprites[1], 37, -5, 1, 0, false, false, false, true};
    SpritePayload sp7 = {srs7, spritePath, timeGraphics[1], SpriteType::TIME, (int)TimeSprite::EARLY_MORNING_1_0};

    SpriteRenderState srs8 = {timeSprites[2], 101, -5, 1, 0, false, false, false, true};
    SpritePayload sp8 = {srs8, spritePath, timeGraphics[2], SpriteType::TIME, (int)TimeSprite::EARLY_MORNING_2_0};

    SpriteRenderState srs9 = {timeSprites[3], 165, -5, 1, 0, false, false, false, true};
    SpritePayload sp9 = {srs9, spritePath, timeGraphics[3], SpriteType::TIME, (int)TimeSprite::EARLY_MORNING_3_0};

    // skill sprite
    Sprite skillSprite = {SpriteSize_16x16, SpriteColorFormat_256Color, 7};
    GraphicAsset skillGraphic = {};
    SpriteRenderState srs10 = {skillSprite, 90, 77, 1, 0, false, false, false, true};
    SpritePayload sp10 = {srs10, spritePath, skillGraphic, SpriteType::SKILL_SPRITE, (int)SkillSprite::SKILLS_LEVEL};

    // slash sprite
    Sprite slashSprite = {SpriteSize_16x16, SpriteColorFormat_256Color, 2};
    GraphicAsset slashGraphic = {};
    SpriteRenderState srs11 = {slashSprite, 52, 157, 1, 0, false, false, false, true};
    SpritePayload sp11 = {srs11, spritePath, slashGraphic, SpriteType::DIGIT, (int)DigitSprite::SLASH};

    // data groups
    etl::array<GraphicAsset*, 8> spritePalettes = {&moonGraphic,
                                                   &dayOfWeekGraphic,
                                                   &digitGraphics[0],
                                                   &timeGraphics[0],
                                                   &timeGraphics[1],
                                                   &timeGraphics[2],
                                                   &timeGraphics[3],
                                                   &skillGraphic};

    etl::array<SpritePayload, 12> spritePayloads = {sp0, sp1, sp2, sp3, sp4, sp5, sp6, sp7, sp8, sp9, sp10, sp11};

    etl::array<SpriteTransform, 0> spriteTransforms = {};
    // ---
};
