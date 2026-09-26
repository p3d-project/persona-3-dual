#include "UIScreen.hpp"

void UIScreen::triggerAction(UIAction action)
{
}

void UIScreen::renderSprites()
{
}

void UIScreen::removeSprites()
{
    oamClear(oam, 0, 0);
}

int UIScreen::onTouch(touchPosition* touch)
{
    return -1;
}

void UIScreen::moveSprite(int spriteId, int x, int y)
{
    oamSetXY(oam, spriteId, x, y);
}

void UIScreen::showSprite(int spriteId)
{
    oamSetHidden(oam, spriteId, false);
}

void UIScreen::hideSprite(int spriteId)
{
    oamSetHidden(oam, spriteId, true);
}
