#include "UIScreen.hpp"

void UIScreen::triggerAction(UIAction action)
{
}

void UIScreen::renderSprites()
{
}

void UIScreen::removeSprites()
{
    if (oam == nullptr)
    {
        return;
    }
    oamClear(oam, 0, 0);
}

int UIScreen::onTouch(touchPosition* touch)
{
    return -1;
}

void UIScreen::moveSprite(int spriteId, int x, int y)
{
    if (oam == nullptr)
    {
        return;
    }
    oamSetXY(oam, spriteId, x, y);
}

void UIScreen::showSprite(int spriteId)
{
    if (oam == nullptr)
    {
        return;
    }
    oamSetHidden(oam, spriteId, false);
}

void UIScreen::hideSprite(int spriteId)
{
    if (oam == nullptr)
    {
        return;
    }
    oamSetHidden(oam, spriteId, true);
}
