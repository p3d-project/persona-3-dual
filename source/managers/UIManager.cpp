#include "UIManager.hpp"
#include <nds.h>

void UIManager::showBg(int bgId)
{
    bgShow(bgId);
}

void UIManager::hideBg(int bgId)
{
    bgHide(bgId);
}
