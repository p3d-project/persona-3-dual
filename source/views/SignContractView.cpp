#include "SignContractView.h"
#include "core/enums.h"
#include "core/globals.h"
#include "events/SaveEvents.hpp"
#include <nds.h>
#include <stdio.h>

// sfx
#include "soundbank.h"

void SignContractView::cancelSFX()
{
    musicCtrl->stopSFX(sfxMenuHandle);
    musicCtrl->stopSFX(sfxSelectHandle);
    musicCtrl->stopSFX(sfxCancelHandle);
}

void SignContractView::init()
{
    if (signContract == nullptr)
    {
        signContract = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        signContract->AddComponent(graphics);
    }

    // set both screens to black
    setBrightness(3, -16);

    // setup music
    musicCtrl->loadSFX(SFX_MENU);
    musicCtrl->loadSFX(SFX_SELECT);
    musicCtrl->loadSFX(SFX_CANCEL);
    musicCtrl->init((fatBasePath + "music/menus/contract/mistic.pcm").c_str(), 1.998f, 49.959f);

    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_0_2D);

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
    if (contractBg.tiles)
        dmaCopy(contractBg.tiles, bgGetGfxPtr(bg[0]), contractBg.tilesLen);
    if (contractBg.map)
        dmaCopy(contractBg.map, bgGetMapPtr(bg[0]), contractBg.mapLen);

    vramSetBankE(VRAM_E_LCD);
    if (contractBg.pal)
        dmaCopy(contractBg.pal, &VRAM_E_EXT_PALETTE[0][0], contractBg.palLen);
    vramSetBankE(VRAM_E_BG_EXT_PALETTE);

    graphics->unloadGraphic(contractBg);

    // setup console
    consoleInit(&animatedConsole, 0, BgType_Text4bpp, BgSize_T_256x256, 5, 3, false, true);
    consoleInit(&console, 1, BgType_Text4bpp, BgSize_T_256x256, 2, 0, false, true);

    keyboardInit(keyboardGetDefault(), 2, BgType_Text4bpp, BgSize_T_256x512, 3, 1, false, true);

    bgSetPriority(animatedConsole.bgId, 0);
    bgSetPriority(console.bgId, 1);
    bgSetPriority(keyboardGetDefault()->background, 2);

    keyboardShow();

    consoleSelect(&animatedConsole);
    printf("\x1b[11;6HEnter your last name");
    printf("\x1b[0;0H%s", saveData.lastName);
    consoleSelect(&console);

    // setup animated text
    REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG0 | BLEND_DST_BACKDROP;
    REG_BLDALPHA_SUB = textAlpha | ((16 - textAlpha) << 8);

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
                printf("%c", key);
            }
        }
        else if (!isLastName && !isNameConfirmed)
        {
            if (saveData.firstName[0] == '\0')
            {
                isLastName = true;
                consoleSelect(&console);
                consoleClear();

                consoleSelect(&animatedConsole);
                consoleClear();
                printf("\x1b[11;6HEnter your last name");

                consoleSelect(&console);
                printf("\x1b[0;0H%s", saveData.lastName);
            }
            else
            {
                saveData.firstName[firstNameIndex - 1] = '\0';
                firstNameIndex--;
                printf("%c", key);
            }
        }
        else
        {
            isNameConfirmed = false;
            consoleSelect(&console);
            consoleClear();

            consoleSelect(&animatedConsole);
            consoleClear();
            printf("\x1b[11;6HEnter your first name");

            consoleSelect(&console);
            printf("\x1b[0;0H%s", saveData.firstName);
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
            consoleSelect(&console);
            consoleClear();

            consoleSelect(&animatedConsole);
            consoleClear();
            printf("\x1b[11;5HEnter your first name");

            consoleSelect(&console);
            printf("\x1b[0;0H%s", saveData.firstName);
        }
        else if (!isNameConfirmed)
        {
            isNameConfirmed = true;
            consoleSelect(&console);
            consoleClear();

            consoleSelect(&animatedConsole);
            consoleClear();
            printf("\x1b[11;7HConfirm your name?");

            consoleSelect(&console);

            printf("\x1b[0;0H%s,", saveData.lastName);
            printf("\x1b[1;0H%s", saveData.firstName);
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
        }
        else if (!isLastName && !isNameConfirmed && (firstNameIndex < 31))
        {
            saveData.firstName[firstNameIndex] = key;
            saveData.firstName[firstNameIndex + 1] = '\0';
            firstNameIndex++;
        }

        printf("%c", key);
    }

    // animate text
    durationCounter++;
    if (durationCounter >= duration)
    {
        durationCounter = 0;
        textAlpha += textAlphaDirection;

        // start fading out
        if (textAlpha >= 16)
        {
            textAlpha = 16;
            textAlphaDirection = -1;
        }
        // start fading in
        else if (textAlpha <= 0)
        {
            textAlpha = 0;
            textAlphaDirection = 1;
        }

        REG_BLDCNT_SUB = BLEND_ALPHA | BLEND_SRC_BG0 | BLEND_DST_BACKDROP;
        REG_BLDALPHA_SUB = textAlpha | ((16 - textAlpha) << 8);
    }

    musicCtrl->update();
    return ViewState::KEEP_CURRENT;
}

void SignContractView::cleanup()
{
    if (graphics != nullptr)
    {
        graphics->unloadAll();
    }

    if (signContract != nullptr)
    {
        signContract->RemoveComponent<GraphicsComponent>();
        engine.DestroyEntity(signContract);

        signContract = nullptr;
        graphics = nullptr;
    }

    // update save data (names)
    ae::BroadcastEvent(Event::WriteSave{});
    musicCtrl->cleanup();
    BaseView::cleanup();
}
