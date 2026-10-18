#pragma once
#include "components/GraphicsComponent.hpp"
#include "components/TextComponent.hpp"
#include "components/menus/UIMenu.hpp"
#include "components/screens/UIScreen.hpp"

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

    // TODO: replace with ae dependency injection
    /**
     * @brief Set the Text Component object
     * @param text The TextComponent pointer to set
     * @warning This will be removed in the future in favour of a native dependency injection solution in Aegis Engine
     */
    void setTextComponent(TextComponent* text);

    // TODO: replace with ae dependency injection
    /**
     * @brief Set the Info Panel Text object
     *
     * @param yen The yen amount string
     * @param persona The persona name string
     * @param arcana The arcana name string
     * @param level The level string
     * @param color The text colour to render the panel in
     *
     * @warning This will be removed in the future in favour of a native dependency injection solution in Aegis Engine
     */
    void setInfoPanelText(const std::string& yen,
                          const std::string& persona,
                          const std::string& arcana,
                          const std::string& level,
                          TextColor color);

    // TODO: replace with ae dependency injection
    /**
     * @brief Set the Description Text object (top bar)
     *
     * @param description The description string
     * @param color The text colour to render the description in
     *
     * @warning This will be removed in the future in favour of a native dependency injection solution in Aegis Engine
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

    ae::Entity* pauseMenu = nullptr;
    GraphicsComponent* graphics = nullptr;
    TextComponent* text = nullptr;
    const UIMenu* menu = nullptr;

    // menu options
    const int visibleOptions = 7;
    const int optionsStartX = 26;
    const int optionsStartY = 60;
    const int optionsRowHeight = 19;
    TextColor optionColor = TextColor::RichBlue;
    TextColor selectedOptionColor = TextColor::DarkestBlue;

    //Menu texts
    std::string yenText = "";
    std::string personaText = "";
    std::string arcanaText = "";
    std::string levelText = "";
    TextColor textColor = TextColor::DarkestBlue;

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
    const int selectedBarYOffset = -2;

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
