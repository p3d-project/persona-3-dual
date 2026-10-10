#pragma once
#include "components/GraphicsComponent.hpp"
#include "components/TextComponent.hpp"
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

    // TODO: replace with ae dependancy injection
    void setTextComponent(TextComponent* text);
    void setTextContent(std::string textContent, TextColor textColor);

  private:
    MenuHUDScreen() : UIScreen(false) {};
    ~MenuHUDScreen() {};
    static MenuHUDScreen* instance;

    void loadBackground();
    void renderBackground();
    void unloadBackground();

    void renderText();

    ae::Entity* menuHUD = nullptr;
    GraphicsComponent* graphics = nullptr;
    TextComponent* text = nullptr;
    std::string textContent = "";
    TextColor textColor = TextColor::White;

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
    SpriteRenderState srs0 = {moonSprite, 206, 20, 1, 0, false, false, false, true};
    SpritePayload sp0 = {srs0, spritePath, moonGraphic, SpriteType::MOON, (int)MoonSprite::MOON_0};

    // time sprites
    Sprite timeSprites[2] = {{SpriteSize_64x32, SpriteColorFormat_256Color, 1},
                             {SpriteSize_64x32, SpriteColorFormat_256Color, 2}};
    GraphicAsset timeGraphics[2] = {};

    SpriteRenderState srs6 = {timeSprites[0], 112, -5, 1, 0, false, false, false, true};
    SpritePayload sp6 = {srs6, spritePath, timeGraphics[0], SpriteType::TIME, (int)TimeSprite::EARLY_MORNING_0_0};

    SpriteRenderState srs7 = {timeSprites[1], 176, -5, 1, 0, false, false, false, true};
    SpritePayload sp7 = {srs7, spritePath, timeGraphics[1], SpriteType::TIME, (int)TimeSprite::EARLY_MORNING_1_0};

    // skill sprite
    Sprite skillSprite = {SpriteSize_16x16, SpriteColorFormat_256Color, 3};
    GraphicAsset skillGraphic = {};
    SpriteRenderState srs8 = {skillSprite, 25, 130, 1, 0, false, false, false, true};
    SpritePayload sp8 = {srs8, spritePath, skillGraphic, SpriteType::SKILL_SPRITE, (int)SkillSprite::SKILLS_LEVEL};

    // data groups
    etl::array<GraphicAsset*, 4> spritePalettes = {&moonGraphic, &timeGraphics[0], &timeGraphics[1], &skillGraphic};

    etl::array<SpritePayload, 4> spritePayloads = {sp0, sp6, sp7, sp8};

    etl::array<SpriteTransform, 0> spriteTransforms = {};
    // ---
};
