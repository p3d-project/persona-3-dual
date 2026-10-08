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
    void renderSprites() override;
    int onTouch(touchPosition* touch) override;

  private:
    SignContractScreen() : UIScreen(false) {};
    ~SignContractScreen() {};
    static SignContractScreen* instance;

    void loadSaveDataName();
    void loadBackgrounds();
    void renderBackground();
    void unloadBackgrounds();

    int evaluateInput(char c);
    void writeCharacter(char c);

    void updateStatus(std::string status);
    void renderName();

    ae::Entity* signContractUI = nullptr;
    GraphicsComponent* graphics = nullptr;
    TextComponent* text = nullptr;

    Keyboard_N keyboard;

    std::string FONT_NAME = "cosmetica";
    int FONT_SIZE = 10;
    int lineSpacing = 0;

    // backgrounds
    const std::string bgPath = "graphics/SignContractView/backgrounds/";
    GraphicAsset bgUI[2] = {};

    bool isLastName = true;
    bool isNameConfirmed = false;
    int index = 0;
    char firstName[10] = "         ";
    char lastName[10] = "         ";
};
