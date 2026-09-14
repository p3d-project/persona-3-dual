#include "views/DebugView.hpp"
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
}

ViewState DebugView::update()
{
    return ViewState::KEEP_CURRENT;
}

void DebugView::cleanup()
{
    BaseView::cleanup();
}
