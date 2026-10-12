/**
 * @file SignContractScreen.hpp
 * @author Gregory Munro (ggmini)
 * @brief Handles the Rendering & Logic for the Sign Contract UI
 */

#pragma once

#include "components/GraphicsComponent.hpp"
#include "components/TextComponent.hpp"
#include "components/screens/UIScreen.hpp"
#include "core/keyboard.hpp"

#include <etl/vector.h>
#include <nds.h>

class SignContractScreen : public UIScreen
{
  public:
    static void create();
    static void destroy();
    static SignContractScreen* getInstance();

    void load();
    void unload();

    int update();

    /**
     * @brief Renders the background and status text to the screen
     */
    void renderSprites() override;

    /**
     * @brief Handles touch input. Will check if any of the keyboard keys were pressed and execute any necessary actions.
     * @param touch Touch input variable
     * @return int Return code to indicate if the game should proceed to the next view
     */
    int onTouch(touchPosition* touch) override;

  private:
    SignContractScreen() : UIScreen(false) {};
    ~SignContractScreen() {};
    static SignContractScreen* instance;

    /**
     * @brief Loads the name from the save data file into a local variable
     */
    void loadSaveDataName();

    /**
     * @brief Loads background assets into RAM
     * @note This does not push the backgrounds into VRAM. The background assets will remain in memory until they are unloaded.
     */
    void loadBackgrounds();

    /**
     * @brief Switches the rendered background between the default and shift/capslock backgrounds
     */
    void switchBackground();

    /**
     * @brief Pushes background data into VRAM so that it is rendered on screen
     * @param bgIndex The id of the background to render. 0 = default, 1 = shift/capslock
     */
    void renderBackground(int bgIndex = 0);

    /**
     * @brief Clears the current background from VRAM and unloads the background data from RAM
     */
    void unloadBackgrounds();

    /**
     * @brief Writes a character to the name buffer at the current index and increments the index
     * @param c The character to write to the name buffer
     * @note Will stop writing characters if the index is at the end of the name buffer (20 characters)
     */
    void writeCharacter(char c);

    /**
     * @brief Clears any old status text and writes a new status to the status text box
     * @param status String of the status to write
     */
    void updateStatus(std::string status);

    /**
     * @brief Clears any old name text and writes the current name buffer to the name input area
     */
    void renderName();

    /**
     * @brief Renders a small cursor at the current slot where the next character will be written in the name buffer
     * @note Will write "End" at the end behind the name input area if the index exceeds the array bounds of the name
     */
    void renderCursor();

    void clearCursor();

    int confirmAction();
    void moveCursorLeft();
    void moveCursorRight();

    ae::Entity* signContractUI = nullptr;
    GraphicsComponent* graphics = nullptr;
    TextComponent* text = nullptr;

    uint16_t* bgTextBufferSub = nullptr;

    OnScreenKeyboard keyboard;

    std::string FONT_NAME = "cosmetica";
    int FONT_SIZE = 10;
    int lineSpacing = 0;

    bool cursorRendered = false;

    // backgrounds
    const std::string bgPath = "graphics/SignContractView/backgrounds/";
    GraphicAsset bgUI[2] = {};

    bool isLastName = true;
    int index = 0;
    char firstName[10] = "         ";
    char lastName[10] = "         ";

    // TODO: move these variables into keyboard class once that gets a proper implementation
    // Currently we need them to handle the background switching
    bool isShift = false;
    bool isCapsLock = false;
};
