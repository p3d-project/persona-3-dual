#include "SignContractView.hpp"

#include "core/globals.hpp"
#include "systems/UISystem.hpp"

#include <cstring>
#include <nds.h>
#include <stdio.h>

void SignContractView::cancelSFX()
{
    sfxCmpt->stopSFX();
}

void SignContractView::init()
{
    if (signContract == nullptr)
    {
        signContract = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        musicCmpt = engine.CreateComponent<MusicComponent>();
        sfxCmpt = engine.CreateComponent<SFXComponent>();

        signContract->AddComponent(graphics);
        signContract->AddComponent(musicCmpt);
        signContract->AddComponent(sfxCmpt);
    }

    // set both screens to black
    setBrightness(3, -16);

    // setup music
    sfxCmpt->registerSFX(SFX::SFX_1);
    sfxCmpt->registerSFX(SFX::SFX_2);
    sfxCmpt->registerSFX(SFX::SFX_0);
    musicCmpt->registerMusic(
        (fatBasePath + "music/menus/contract/mistic.qoa").c_str(), ae::q20_12_t{1.998}, ae::q20_12_t{49.959});

    videoSetMode(MODE_5_2D);
    videoSetModeSub(MODE_3_2D | DISPLAY_BG3_ACTIVE);

    // map vram banks to main engine background
    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankD(VRAM_D_MAIN_BG_0x06020000);
    // map vram to sub screen
    vramSetBankC(VRAM_C_SUB_BG);

    // enable extended palettes
    bgExtPaletteEnableSub();

    // initialize backgrounds
    int bgSubId = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 8, 0);

    signContractScreen = SignContractScreen::getInstance();
    bgSub[0] = bgSubId;
    ae::BroadcastEvent(Event::ConfigureUIScreen{bgSub, bgMain, &oamSub, &oamMain, {signContractScreen}});
    ae::BroadcastEvent(Event::ShowScreen{signContractScreen});

    // transition both screens from black
    for (int i = -16; i < 0; i++)
    {
        setBrightness(3, i);

        // wait a few frames
        for (int duration = 0; duration <= 2; duration++)
        {
            swiWaitForVBlank();
        }
    }
}

ViewState SignContractView::update()
{
    if (systemKeysDown & KEY_TOUCH)
    {
        touchRead(&touch);

        if (signContractScreen->onTouch(&touch) == 1)
        {
            return ViewState::CUTSCENE_2;
        }
    }

    return ViewState::KEEP_CURRENT;
}

void SignContractView::cleanup()
{
    if (signContract != nullptr)
    {
        engine.DestroyEntity(signContract);

        signContract = nullptr;
        graphics = nullptr;
        signContractScreen = nullptr;
        musicCmpt = nullptr;
        sfxCmpt = nullptr;
    }

    BaseView::cleanup();
}
