/**
 * @file PauseMenuScreen.hpp
 * @brief Pause menu screen
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
 */

#pragma once
#include "components/GraphicsComponent.hpp"
#include "components/TextComponent.hpp"
#include "components/menus/UIMenu.hpp"
#include "components/screens/UIScreen.hpp"
#include "uiAnimation/UIAnimator.hpp"

#include <etl/array.h>
#include <nds.h>

class PauseMenuScreen : public UIScreen
{
  public:
    static void create();
    static void destroy();
    static PauseMenuScreen* getInstance();

    void load();
    void unload();
    void renderSprites() override;
    int onTouch(touchPosition* touch) override;

    /**
     * @brief Starts the jiggle animation on the selected option sprite
     */
    void animateSelection();

    /**
     * @brief Updates active UI animations
     */
    void updateAnimation();

    /**
     * @brief Updates active UI animations with a given delta time
     * @param dt Delta time in seconds (Q20.12)
     */
    void updateAnimation(ae::q20_12_t dt);

    /**
     * @brief Checks if the selection sprite is currently animating
     * @return true if currently animating, false otherwise
     */
    bool isAnimating() const;

    /**
     * @brief Cancels all animations
     */
    void cancelAnimation();

    /**
     * @brief Set the Text Component object
     * @param text The TextComponent pointer to set
     * @warning This will be removed in the future in favour of a native dependency injection solution in Aegis Engine
     */
    void setTextComponent(TextComponent* text);

    /**
     * @brief Set the Text Component used to draw the menu options
     * @param optionsText The TextComponent pointer to set
     * @warning This will be removed in the future in favour of a native dependency injection solution in Aegis Engine
     */
    void setOptionsTextComponent(TextComponent* optionsText);

    /**
     * @brief Set the Yen Text object (info panel)
     *
     * @param yen The yen amount string
     * @param color The text colour to render the yen amount in
     */
    void setYenText(const std::string& yen, TextColor color);

    /**
     * @brief Set the Persona Text object (info panel)
     *
     * @param persona The active persona name string
     * @param color The text colour to render the persona name in
     */
    void setPersonaText(const std::string& persona, TextColor color);

    /**
     * @brief Set the Arcana Text object (info panel)
     *
     * @param arcana The active persona arcana string
     * @param color The text colour to render the arcana in
     */
    void setArcanaText(const std::string& arcana, TextColor color);

    /**
     * @brief Set the Level Text object (info panel)
     *
     * @param level The active persona level string
     * @param color The text colour to render the level in
     */
    void setLevelText(const std::string& level, TextColor color);

    /**
     * @brief Set the Description Text object (top bar)
     *
     * @param description The description string
     * @param color The text colour to render the description in
     */
    void setDescriptionText(const std::string& description, TextColor color);

    /**
     * @brief Draws the text
     * @note Must be re-called after anything clears the shared text component (e.g. menu navigation)
     */
    void renderText();

    // TODO: replace with ae dependency injection
    /**
     * @brief Set the menu whose options are drawn on this screen
     * @param menu The UIMenu pointer to set
     * @warning This will be removed in the future in favour of a native dependency injection solution in Aegis Engine
     */
    void setMenu(const UIMenu* menu);

    /**
     * @brief Gets how many option rows the screen shows
     *
     * @return int the number of visible options
     */
    int getVisibleOptions();

  private:
    PauseMenuScreen() : UIScreen(false) {};
    ~PauseMenuScreen() {};
    static PauseMenuScreen* instance;

    void loadBackground();
    void renderBackground();
    void unloadBackground();

    /**
     * @brief Draws the visible options of the active menu and moves the selection bar to the selected one
     */
    void renderOptions();

    /**
     * @brief Updates the positions of the selected option bar sprites (including jiggle offset)
     */
    void updateSelectionSpritePosition();

    ae::Entity* pauseMenu = nullptr;
    GraphicsComponent* graphics = nullptr;
    TextComponent* text = nullptr;
    TextComponent* optionsText = nullptr;
    const UIMenu* menu = nullptr;

    int selectedBarBaseY = 0;
    int jiggleOffsetY = 0;
    UIAnimator animator;
    UIAnimationHandle selectionAnimation;

    // menu options
    const int visibleOptions = 7;
    const int optionsStartX = 26;
    const int optionsStartY = 52;
    const int optionsRowHeight = 19;
    TextColor optionColor = TextColor::RichBlue;
    TextColor selectedOptionColor = TextColor::DarkestBlue;

    //Menu texts
    std::string yenText = "";
    TextColor yenColor = TextColor::DarkestBlue;

    std::string personaText = "";
    TextColor personaColor = TextColor::DarkestBlue;

    std::string arcanaText = "";
    TextColor arcanaColor = TextColor::DarkestBlue;

    std::string levelText = "";
    TextColor levelColor = TextColor::DarkestBlue;

    std::string descriptionText = "";
    TextColor descriptionColor = TextColor::White;

    // background
    const std::string bgPath = "graphics/PauseMenu/backgrounds/";

    GraphicAsset bgPauseMenu = {};

    // sprite setup
    const std::string spritePath = "graphics/PauseMenu/sprites/";

    // status sprite
    Sprite statusSprite = {SpriteSize_32x32, SpriteColorFormat_16Color, 0};
    GraphicAsset statusGraphic = {};
    SpriteRenderState srs0 = {statusSprite, 222, 138, 1, 0, false, false, false, false};
    SpritePayload sp0 = {srs0, spritePath, statusGraphic, SpriteType::STATUS, (int)StatusSprite::STATUS_GREAT};

    // selected option sprites
    Sprite selectedOptionSprites[4] = {{SpriteSize_32x16, SpriteColorFormat_16Color, 0},
                                       {SpriteSize_32x16, SpriteColorFormat_16Color, 0},
                                       {SpriteSize_32x16, SpriteColorFormat_16Color, 0},
                                       {SpriteSize_32x16, SpriteColorFormat_16Color, 0}};
    GraphicAsset selectedOptionGraphics[4] = {};

    const int selectedBarPieceWidth = 32;
    const int selectedBarYOffset = 7;

    SpriteRenderState srs1 = {selectedOptionSprites[0], 0, 0, 1, -1, false, false, false, false};
    SpritePayload sp1 = {srs1,
                         spritePath,
                         selectedOptionGraphics[0],
                         SpriteType::SELECTED_OPTION,
                         (int)SelectedOptionSprite::SELECTED_OPTION_0};

    SpriteRenderState srs2 = {selectedOptionSprites[1], selectedBarPieceWidth, 0, 1, -1, false, false, false, false};
    SpritePayload sp2 = {srs2,
                         spritePath,
                         selectedOptionGraphics[1],
                         SpriteType::SELECTED_OPTION,
                         (int)SelectedOptionSprite::SELECTED_OPTION_1};

    SpriteRenderState srs3 = {
        selectedOptionSprites[2], selectedBarPieceWidth * 2, 0, 1, -1, false, false, false, false};
    SpritePayload sp3 = {srs3,
                         spritePath,
                         selectedOptionGraphics[2],
                         SpriteType::SELECTED_OPTION,
                         (int)SelectedOptionSprite::SELECTED_OPTION_2};

    SpriteRenderState srs4 = {
        selectedOptionSprites[3], selectedBarPieceWidth * 3, 0, 1, -1, false, false, false, false};
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
