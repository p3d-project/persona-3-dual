#include "RenderManager.hpp"
#include <nds.h>

void RenderManager::showBg(int bgId)
{
    bgShow(bgId);
}

void RenderManager::hideBg(int bgId)
{
    bgHide(bgId);
}
