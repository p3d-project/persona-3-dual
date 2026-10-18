/**
 * @file MenuHUDScreen.hpp
 * @brief Handles the rendering & logic for the menu HUD UI
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

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

    /**
     * @brief Renders the background and sprites text to the screen
     */
    void renderSprites() override;

    /**
     * @brief Handles touch input
     * @param touch Touch input variable
     * @return int Touch input return code indicating what area of the screen was touched
     */
    int onTouch(touchPosition* touch) override;

    // TODO: replace with ae dependency injection
    /**
     * @brief Set the Text Component object
     * @param text The TextComponent pointer to set
     * @warning This will be removed in the future in favour of a native dependency injection solution in Aegis Engine
     */
    void setTextComponent(TextComponent* text);

    // TODO: replace with ae dependency injection
    /**
     * @brief Set the Text Content object
     *
     * @param textContent The text to print
     * @param textColor The text colour to render the textContent in
     *
     * @warning This will be removed in the future in favour of a native dependency injection solution in Aegis Engine
     */
    void setTextContent(std::string textContent, TextColor textColor);

  private:
    MenuHUDScreen() : UIScreen(false) {};
    ~MenuHUDScreen() {};
    static MenuHUDScreen* instance;

    /**
     * @brief Pushes background data into VRAM so that it is rendered on screen
     */
    void renderBackground();

    /**
     * @brief Prints the text to render on screen
     */
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
    const std::string spritePath = "graphics/MenuHUD/sprites/";

    // moon sprite
    Sprite moonSprite = {SpriteSize_32x32, SpriteColorFormat_16Color, 0};
    GraphicAsset moonGraphic = {};
    SpriteRenderState srs0 = {moonSprite, 206, 20, 1, 0, false, false, false, true};
    SpritePayload sp0 = {srs0, spritePath, moonGraphic, SpriteType::MOON, (int)MoonSprite::MOON_0};

    // time sprites
    Sprite timeSprites[2] = {{SpriteSize_64x32, SpriteColorFormat_16Color, 0},
                             {SpriteSize_64x32, SpriteColorFormat_16Color, 0}};
    GraphicAsset timeGraphics[2] = {};

    SpriteRenderState srs6 = {timeSprites[0], 112, -5, 1, 0, false, false, false, true};
    SpritePayload sp6 = {srs6, spritePath, timeGraphics[0], SpriteType::TIME, (int)TimeSprite::EARLY_MORNING_0_0};

    SpriteRenderState srs7 = {timeSprites[1], 176, -5, 1, 0, false, false, false, true};
    SpritePayload sp7 = {srs7, spritePath, timeGraphics[1], SpriteType::TIME, (int)TimeSprite::EARLY_MORNING_1_0};

    // skill sprite
    Sprite skillSprite = {SpriteSize_16x16, SpriteColorFormat_16Color, 0};
    GraphicAsset skillGraphic = {};
    SpriteRenderState srs8 = {skillSprite, 25, 130, 1, 0, false, false, false, true};
    SpritePayload sp8 = {srs8, spritePath, skillGraphic, SpriteType::SKILL_SPRITE, (int)SkillSprite::SKILLS_LEVEL};

    // data groups
    etl::array<SpritePayload, 4> spritePayloads = {sp0, sp6, sp7, sp8};

    etl::array<SpriteTransform, 0> spriteTransforms = {};
    // ---
};
