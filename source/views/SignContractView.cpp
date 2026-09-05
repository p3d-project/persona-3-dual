#include "SignContractView.hpp"

#include "core/globals.hpp"
#include "events/SaveEvents.hpp"

#include <cstring>
#include <nds.h>
#include <nds/arm9/keyboard.h>
#include <stdio.h>

// sfx
#include "soundbank.h"

void SignContractView::cancelSFX()
{
    musicCtrl->stopSFX(sfxMenuHandle);
    musicCtrl->stopSFX(sfxSelectHandle);
    musicCtrl->stopSFX(sfxCancelHandle);
    sfxMenuHandle = 0;
    sfxSelectHandle = 0;
    sfxCancelHandle = 0;
}

void SignContractView::init()
{
    if (signContract == nullptr)
    {
        signContract = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        text = engine.CreateComponent<TextComponent>();

        signContract->AddComponent(graphics);
        signContract->AddComponent(text);
    }

    // set both screens to black
    setBrightness(3, -16);

    // setup music
    musicCtrl->loadSFX(SFX_MENU);
    musicCtrl->loadSFX(SFX_SELECT);
    musicCtrl->loadSFX(SFX_CANCEL);
    musicCtrl->init(
        (fatBasePath + "music/menus/contract/mistic.pcm").c_str(), ae::q20_12_t{1.998}, ae::q20_12_t{49.959});

    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_3_2D | DISPLAY_BG3_ACTIVE);

    // map vram banks to main engine background
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankD(VRAM_D_MAIN_BG_0x06020000);
    // map vram to sub screen
    vramSetBankC(VRAM_C_SUB_BG);

    // enable extended palettes
    bgExtPaletteEnable();

    // initialize backgrounds
    bg[0] = bgInit(0, BgType_Text8bpp, BgSize_T_256x256, 9, 2);
    bgSetPriority(bg[0], 0);

    // load contract background from runtime assets
    GraphicAsset contractBg = graphics->loadGraphic("graphics/SignContractView/backgrounds/contract/contract");

    dmaFillHalfWords(0, bgGetMapPtr(bg[0]), 8192);
    dmaCopy(contractBg.tiles, bgGetGfxPtr(bg[0]), contractBg.tilesLen);
    dmaCopy(contractBg.map, bgGetMapPtr(bg[0]), contractBg.mapLen);

    vramSetBankE(VRAM_E_LCD);
    dmaCopy(contractBg.pal, &VRAM_E_EXT_PALETTE[0][0], contractBg.palLen);
    vramSetBankE(VRAM_E_BG_EXT_PALETTE);

    graphics->unloadGraphic(contractBg);

    // setup text
    int bgTextSub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 4, 0);
    uint16_t* textVideoBufferSub = bgGetGfxPtr(bgTextSub);
    text->configureText(TextConfig(textVideoBufferSub, &FONT_NAME, FONT_SIZE));

    keyboardInit(keyboardGetDefault(), 2, BgType_Text4bpp, BgSize_T_256x512, 3, 1, false, true);

    bgSetPriority(bgTextSub, 0);
    bgSetPriority(keyboardGetDefault()->background, 2);

    keyboardShow();

    animText = "Enter your last name";
    displayText = saveData.lastName;

    firstNameIndex = std::strlen(saveData.firstName);
    lastNameIndex = std::strlen(saveData.lastName);

    text->drawText(displayText, 0, 0);
    text->drawText(animText, 0, 96);

    // transition both screens from black
    for (int i = -16; i < 0; i++)
    {
        setBrightness(3, i);

        // wait a few frames
        for (int duration = 0; duration <= 2; duration++)
        {
            swiWaitForVBlank();
            musicCtrl->update();
        }
    }
}

ViewState SignContractView::update()
{
    int key = keyboardUpdate();

    // Bksp (8) or "B"
    if ((key == 8) || (systemKeysDown & KEY_B))
    {
        key = 8;
        cancelSFX();
        sfxCancelHandle = musicCtrl->playSFX(SFX_CANCEL, 255, 128);

        if (isLastName)
        {
            if (saveData.lastName[0] != '\0')
            {
                saveData.lastName[lastNameIndex - 1] = '\0';
                lastNameIndex--;

                text->clearArea(0, 0, 256, FONT_SIZE);
                displayText = saveData.lastName;
            }
        }
        else if (!isLastName && !isNameConfirmed)
        {
            if (saveData.firstName[0] == '\0')
            {
                isLastName = true;
                animText = "Enter your last name";
                displayText = saveData.lastName;
            }
            else
            {
                saveData.firstName[firstNameIndex - 1] = '\0';
                firstNameIndex--;
                text->clearArea(0, 0, 256, FONT_SIZE);
                displayText = saveData.firstName;
            }
        }
        else
        {
            isNameConfirmed = false;
            text->clearScreen();

            animText = "Enter your first name";
            displayText = saveData.firstName;
        }
    }
    // Return (10) or "A"
    else if ((key == 10) || (systemKeysDown & KEY_A))
    {
        key = 10;
        cancelSFX();
        sfxSelectHandle = musicCtrl->playSFX(SFX_SELECT, 255, 128);

        if (isLastName)
        {
            isLastName = false;
            text->clearScreen();
            animText = "Enter your first name";
            displayText = saveData.firstName;
        }
        else if (!isNameConfirmed)
        {
            isNameConfirmed = true;
            text->clearScreen();
            animText = "Confirm your name?";

            std::string op = "\n";
            displayText = saveData.lastName + op + saveData.firstName;
        }
        else
        {
            cancelSFX();
            // transition both screens to black
            for (int i = 0; i <= 16; i++)
            {
                setBrightness(3, -i);

                // wait a few frames
                for (int duration = 0; duration <= 2; duration++)
                {
                    swiWaitForVBlank();
                    musicCtrl->update();
                }
            }
            musicCtrl->pause();

            return ViewState::CUTSCENE_2;
        }
    }
    // on any other keyboard entry
    else if (key > 0)
    {
        cancelSFX();
        sfxMenuHandle = musicCtrl->playSFX(SFX_MENU, 255, 128);

        if (isLastName && (lastNameIndex < 31))
        {
            saveData.lastName[lastNameIndex] = key;
            saveData.lastName[lastNameIndex + 1] = '\0';
            lastNameIndex++;
            displayText = saveData.lastName;
        }
        else if (!isLastName && !isNameConfirmed && (firstNameIndex < 31))
        {
            saveData.firstName[firstNameIndex] = key;
            saveData.firstName[firstNameIndex + 1] = '\0';
            firstNameIndex++;
            displayText = saveData.firstName;
        }
    }

    // draw text
    text->drawText(displayText, 0, 0);

    // blink text
    if (animText.length() != 0)
    {
        if (frame % 120 < 60)
        {
            text->drawText(animText, 0, 96);
        }
        else
        {
            text->clearArea(0, 96, 256, FONT_SIZE + text->getLineSpacing());
        }
    }

    musicCtrl->update();
    return ViewState::KEEP_CURRENT;
}

void SignContractView::cleanup()
{
    if (signContract != nullptr)
    {
        engine.DestroyEntity(signContract);

        signContract = nullptr;
        graphics = nullptr;
        text = nullptr;
    }

    // update save data (names)
    ae::BroadcastEvent(Event::WriteSave{});
    keyboardHide();
    musicCtrl->cleanup();
    BaseView::cleanup();
}
