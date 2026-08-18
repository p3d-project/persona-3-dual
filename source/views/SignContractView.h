#pragma once
#include "controllers/MusicController.h"
#include "views/BaseView.h"
#include <maxmod9.h>
#include <nds/arm9/console.h>
#include <nds/arm9/keyboard.h>

#include "components/GraphicsComponent.hpp"

class SignContractView : public BaseView
{
  private:
    int bg[3];
    bool isLastName = true;
    bool isNameConfirmed = false;
    int lastNameIndex = 0;
    int firstNameIndex = 0;

    // console
    PrintConsole console;
    PrintConsole animatedConsole;

    // text animation
    int duration = 4;
    int durationCounter = 0;
    int textAlpha = 0;
    int textAlphaDirection = 0;

    // sfx
    mm_sfxhand sfxMenuHandle;
    mm_sfxhand sfxSelectHandle;
    mm_sfxhand sfxCancelHandle;

    void cancelSFX();

    ae::Entity* signContract = nullptr;
    GraphicsComponent* graphics = nullptr;

    MusicController* musicCtrl = MusicController::getInstance();

  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
