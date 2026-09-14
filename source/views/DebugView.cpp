#include "views/DebugView.hpp"
#include "core/globals.hpp"
#include "managers/AudioManager.hpp"
#include <nds.h>

void DebugView::init()
{
    videoSetMode(MODE_3_2D);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankD(VRAM_D_MAIN_BG_0x06020000);
    vramSetBankC(VRAM_C_SUB_BG);

    // set brightness on bottom screen to completely dark (no visible image)
    setBrightness(2, -16);

    if (debug == nullptr)
    {
        debug = engine.CreateEntity();
        music = engine.CreateComponent<MusicComponent>();
        debug->AddComponent(music);
    }

    std::string filePath = fatBasePath + "music/menus/title/tightrope.qoa";
    music->registerMusic(filePath, ae::q20_12_t{17.962}, ae::q20_12_t{66.082});
    music->playMusic();
}

ViewState DebugView::update()
{
    return ViewState::KEEP_CURRENT;
}

void DebugView::cleanup()
{
    if (debug != nullptr)
    {
        engine.DestroyEntity(debug);

        debug = nullptr;
        music = nullptr;
    }
    BaseView::cleanup();
}
