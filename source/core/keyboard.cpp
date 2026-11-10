#include "keyboard.hpp"
#include <string>

int OnScreenKeyboard::evaluateInput(touchPosition* touch)
{
    if (touch == nullptr)
    {
        return 0;
    }

    for (KeyRow& row : keyboardRows)
    {
        if (touch->py >= row.topY && touch->py <= row.bottomY)
        {
            char c = row.checkRow(touch->px);
            if (c == static_cast<char>(KEYCODES::CAPSLOCK))
            {
                isCapsLock = !isCapsLock;
                return static_cast<char>(
                    KEYCODES::CAPSLOCK); // currently returning this so the screen image can be changed
            }
            else if (c == static_cast<char>(KEYCODES::SHIFT))
            {
                isShift = !isShift;
                return static_cast<int>(KEYCODES::SHIFT);
            }
            if (isShift)
            {
                c = toupper(c);
                isShift = false;
            }
            else if (isCapsLock)
            {
                c = toupper(c);
            }
            return c;
        }
    }
    return 0;
}

void OnScreenKeyboard::reset()
{
    isShift = false;
    isCapsLock = false;
}
