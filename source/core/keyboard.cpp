#include "keyboard.hpp"
#include <string>

int Keyboard_N::evaluateInput(touchPosition* touch)
{
    for (KeyRow& row : keyboardRows)
    {
        if (touch->py >= row.topY && touch->py <= row.bottomY)
        {
            char c = row.checkRow(touch->px);
            if (c == KEYCODES::CAPSLOCK)
            {
                isCapsLock = !isCapsLock;
                return KEYCODES::CAPSLOCK; // currently returning this so the screen image can be changed
            }
            else if (c == KEYCODES::SHIFT)
            {
                isShift = !isShift;
                return KEYCODES::SHIFT;
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
