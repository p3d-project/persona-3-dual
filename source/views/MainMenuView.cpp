#include "MainMenuView.hpp"
#include "core/globals.hpp"
#include "systems/UISystem.hpp"

#include <nds.h>
#include <stdio.h>
#include <string>

void MainMenuView::init()
{
    if (mainMenu == nullptr)
    {
        mainMenu = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        textMenu = engine.CreateComponent<TextComponent>();
        musicCmpt = engine.CreateComponent<MusicComponent>();
        sfxCmpt = engine.CreateComponent<SFXComponent>();

        mainMenu->AddComponent(graphics);
        mainMenu->AddComponent(textMenu);
        mainMenu->AddComponent(musicCmpt);
        mainMenu->AddComponent(sfxCmpt);

        UISystem::GetInstance().SetSFXComponent(sfxCmpt);
    }

    // setup music
    musicCmpt->registerMusic(
        (fatBasePath + "music/menus/velvetRoom/aria_of_the_soul.qoa").c_str(), ae::q20_12_t{0}, ae::q20_12_t{164.940});

    // set video mode for 2 text layers and 2 extended rotation layer
    videoSetMode(MODE_5_2D);
    // set sub video mode for 4 text layers
    videoSetModeSub(MODE_3_2D | DISPLAY_BG3_ACTIVE);

    // map vram bank A to main engine background (slot 0)
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankD(VRAM_D_MAIN_BG_0x06020000);
    // map vram bank B to main engine sprites (slot 0)
    vramSetBankB(VRAM_B_MAIN_SPRITE);
    // map vram to sub screen
    vramSetBankC(VRAM_C_SUB_BG);

    // enable extended palettes
    bgExtPaletteEnable();

    // setup text engine
    int bgTextSub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 4, 0);
    uint16_t* textVideoBufferSub = bgGetGfxPtr(bgTextSub);
    bgSetPriority(bgTextSub, 0);
    textMenu->configureText(TextConfig(textVideoBufferSub, &FONT_NAME, FONT_SIZE));

    // setup main menu
    mainMenuCmpt = MainMenu::getInstance();
    std::array<UIMenu*, 10> menus = {mainMenuCmpt};
    ae::BroadcastEvent(Event::ConfigureUIMenu{textMenu, menus});
    ae::BroadcastEvent(Event::ShowMenu{mainMenuCmpt});

    // initialize backgrounds
    // check https://mtheall.com/vram.html to ensure bg fit in vram
    bg[0] = bgInit(0, BgType_Text8bpp, BgSize_T_512x512, 4, 0);    // silhouette
    bg[1] = bgInit(1, BgType_Text8bpp, BgSize_T_256x256, 9, 2);    // door
    bg[2] = bgInit(2, BgType_ExRotation, BgSize_ER_256x256, 8, 5); // fog

    // need to set priority to properly display
    // 0 is highest, 3 is lowest
    bgSetPriority(bg[0], 0); // silhouette
    bgSetPriority(bg[2], 1); // fog
    bgSetPriority(bg[1], 2); // door

    // reset background vram
    dmaFillHalfWords(0, bgGetMapPtr(bg[0]), 8192);
    dmaFillHalfWords(1, bgGetMapPtr(bg[1]), 2048);
    dmaFillHalfWords(2, bgGetMapPtr(bg[2]), 2048);

    // load graphics
    std::string bgPath = "graphics/MainMenuView/backgrounds/";
    GraphicAsset silhouetteBg = graphics->loadGraphic(bgPath + "menuSilhouetteBackground/menuSilhouetteBackground");
    GraphicAsset doorBg = graphics->loadGraphic(bgPath + "doorBackground/doorBackground");
    GraphicAsset fogBg = graphics->loadGraphic(bgPath + "fogBackground/fogBackground");

    dmaCopy(silhouetteBg.tiles, bgGetGfxPtr(bg[0]), silhouetteBg.tilesLen);
    dmaCopy(doorBg.tiles, bgGetGfxPtr(bg[1]), doorBg.tilesLen);
    dmaCopy(fogBg.tiles, bgGetGfxPtr(bg[2]), fogBg.tilesLen);

    dmaCopy(silhouetteBg.map, bgGetMapPtr(bg[0]), silhouetteBg.mapLen);
    dmaCopy(doorBg.map, bgGetMapPtr(bg[1]), doorBg.mapLen);
    dmaCopy(fogBg.map, bgGetMapPtr(bg[2]), fogBg.mapLen);

    vramSetBankE(VRAM_E_LCD);
    dmaCopy(silhouetteBg.pal, &VRAM_E_EXT_PALETTE[0][0], silhouetteBg.palLen);
    dmaCopy(doorBg.pal, &VRAM_E_EXT_PALETTE[1][0], doorBg.palLen);
    dmaCopy(fogBg.pal, &VRAM_E_EXT_PALETTE[2][0], fogBg.palLen);
    vramSetBankE(VRAM_E_BG_EXT_PALETTE);

    graphics->unloadGraphic(silhouetteBg);
    graphics->unloadGraphic(doorBg);
    graphics->unloadGraphic(fogBg);

    ui.hideBg(bg[2]);
    bgSetCenter(bg[2], 128, 96); // pivot point on the screen
    bgSetScroll(bg[2], 128, 96); // pivot point on the image

    // for slide in animation
    bgSetScroll(bg[0], -silhouetteX, -silhouetteY);
    bgUpdate();

    setBrightness(3, -16);

    // Setup door blend initially
    REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG1 | BLEND_DST_BACKDROP;
    REG_BLDALPHA = 0 | (16 << 8);

    fadeTimer.start(ae::q20_12_t{0.8});
    transitionPhase = TransitionPhase::FADING_IN;
}

ViewState MainMenuView::update()
{
    switch (transitionPhase)
    {
    case TransitionPhase::FADING_IN:
    {
        if (fadeTimer.isFinished())
        {
            transitionPhase = TransitionPhase::FADING_IN_DOOR;
            fadeTimer.start(ae::q20_12_t{2.0});
        }
        else
        {
            int brightness = -16 + (fadeTimer.getProgress().raw_value() >> 8);
            setBrightness(1, brightness);
        }
        break;
    }

    case TransitionPhase::FADING_IN_DOOR:
    {
        if (fadeTimer.isFinished())
        {
            transitionPhase = TransitionPhase::IDLE;
            REG_BLDALPHA = 16 | (0 << 8); // Ensure door is fully visible
        }
        else
        {
            int fadeVal = fadeTimer.getProgress().raw_value() >> 8; // 0 to 16
            REG_BLDALPHA = fadeVal | ((16 - fadeVal) << 8);
        }
        break;
    }

    case TransitionPhase::IDLE:
    {
        if (isSilhouetteStillMoving)
        {
            // skip the animation if the user skipped it
            if (systemKeysDown != 0)
            {
                silhouetteX = 0;
                silhouetteY = 0;
                isSilhouetteStillMoving = false;
            }

            // animate X (moving right towards 0)
            if (silhouetteX < 0 && frame % 5 == 0)
            {
                silhouetteX += (-silhouetteX) / 6 + 1;
            }

            // animate Y (moving up towards 0)
            if (silhouetteY > 0 && frame % 5 == 0)
            {
                silhouetteY -= (silhouetteY / 6) + 1;
            }

            if (silhouetteX >= 0)
            {
                isSilhouetteStillMoving = false;
                silhouetteX = 0;
            }
            if (silhouetteY <= 0)
            {
                isSilhouetteStillMoving = false;
                silhouetteY = 0;
                brightness = 0;
            }
            bgSetScroll(bg[0], -silhouetteX, -silhouetteY);
            break;
        }

        // fade in bottom screen text
        if (brightness < 16 && frame % 4 == 0)
        {
            brightness++;
            setBrightness(2, brightness - 16);
        }

        // setup blending for fog
        if (!displayFog)
        {
            displayFog = true;
            REG_BLDCNT = BLEND_ALPHA | BLEND_SRC_BG2 | BLEND_DST_BACKDROP | BLEND_DST_BG1;
            ui.showBg(bg[2]);
        }

        // fade in fog
        if (displayFog && fogOpacity < 6 && frame % 4 == 0)
        {
            fogOpacity++;
            REG_BLDALPHA = fogOpacity | ((16 - fogOpacity) << 8);
        }

        // rotate fog
        if (displayFog && frame % 4 == 0)
        {
            waveAngle += 50;
            int angle = MathManager::GetInstance().sin(static_cast<ae::angle16_t>(waveAngle)).raw_value();

            int rotationSpeed = baseSpeed + ((angle * fluctuation) >> 12);
            currentRotation += rotationSpeed;
            bgSetRotateScale(bg[2], currentRotation, 256, 256);
        }

        break;
    }

    case TransitionPhase::FADING_OUT:
    {
        if (fadeTimer.isFinished())
        {
            setBrightness(3, -16);
            return nextViewState;
        }
        else
        {
            int fadeVal = fadeTimer.getProgress().raw_value() >> 8;
            setBrightness(3, -fadeVal);
        }
        break;
    }
    }

    return ViewState::KEEP_CURRENT;
}

void MainMenuView::cleanup()
{
    if (mainMenu != nullptr)
    {
        engine.DestroyEntity(mainMenu);

        mainMenu = nullptr;
        graphics = nullptr;
        textMenu = nullptr;
        musicCmpt = nullptr;
        sfxCmpt = nullptr;
    }
    BaseView::cleanup();
}
