#include "BaseView.h"
#include "core/globals.h"

// TODO: recieve and handle etl message Event::SwitchView

void BaseView::cleanup()
{
    // clear screen
    setBrightness(3, 0);

    // global components
    Globals::enableBillboards = true;
    Globals::enableCharacterAnim = true;
    Globals::enableDebugPrint = false;
    Globals::isPauseMenuActive = false;

    // disable blending globally to prevent ghosting across scenes
    REG_BLDCNT = 0;
    REG_BLDCNT_SUB = 0;
    REG_BLDALPHA = 0;
    REG_BLDALPHA_SUB = 0;

    // reset sprites
    oamClear(&oamMain, 0, 0);
    oamClear(&oamSub, 0, 0);

    // reset vram
    vramSetPrimaryBanks(VRAM_A_LCD, VRAM_B_LCD, VRAM_C_LCD, VRAM_D_LCD);
    vramSetBanks_EFG(VRAM_E_LCD, VRAM_F_LCD, VRAM_G_LCD);
    vramSetBankH(VRAM_H_LCD);
    vramSetBankI(VRAM_I_LCD);

    dmaFillHalfWords(0, VRAM_A, 0x20000); // 128 KB
    dmaFillHalfWords(0, VRAM_B, 0x20000); // 128 KB
    dmaFillHalfWords(0, VRAM_C, 0x20000); // 128 KB
    dmaFillHalfWords(0, VRAM_D, 0x20000); // 128 KB
    dmaFillHalfWords(0, VRAM_E, 0x10000); // 64 KB
    dmaFillHalfWords(0, VRAM_H, 0x08000); // 32 KB
    dmaFillHalfWords(0, VRAM_I, 0x04000); // 16 KB
}
