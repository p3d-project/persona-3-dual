#include "UIManager.hpp"
#include <nds.h>

void UIManager::showBg(int bgId)
{
    bgShow(bgId);
}

void UIManager::hideBg(int bgId)
{
    // set bg color to black
    setBackdropColor(RGB15(0, 0, 0));
    setBackdropColorSub(RGB15(0, 0, 0));

    bgHide(bgId);
}
