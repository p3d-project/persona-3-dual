/**
 * @file keyboard.hpp
 * @author Gregory Munro (ggmini)
 * @brief Acts as an onscreen keyboard and converts touch inputs into characters
 */

#pragma once

#include <etl/vector.h>
#include <nds.h>

/**
 * @brief Collection of keycodes for special keys on the keyboard
 */
enum KEYCODES
{
    /// Zero is reserved for not touching the keyboard
    CAPSLOCK = 1,
    SHIFT = 2,
    BACKSPACE = 3,
    LEFT = 4,
    RIGHT = 5,
    CONFIRM = 6
};

/**
 * @brief Represent a single key on the keyboard. Stores x bounadaries and the associated key
 * @note The key does not store Y values, check the corresponding Button Row for this.
 */
struct Key
{
    int topX;
    int bottomX;

    char key;

    bool keyHit(int x)
    {
        if (x >= topX && x <= bottomX)
        {
            return true;
        }
        return false;
    }
};

/**
 * @brief Stores a row of keys on a keyboard and their associated y boundaries.
 *
 */
struct KeyRow
{
    int topY;
    int bottomY;

    etl::vector<Key, 12> keys;
    void addKey(Key key)
    {
        keys.push_back(key);
    }

    char checkRow(int x)
    {
        for (Key& key : keys)
        {
            if (key.keyHit(x))
            {
                return key.key;
            }
        }
        return 0;
    }
};

// TODO: rename
/**
 * @brief On-Screen keyboard
 * @details This class represents an on-screen keyboard that converts touch inputs into characters.
 */
class Keyboard_N
{
  public:
    Keyboard_N() = default;
    ~Keyboard_N() = default;

    /**
     * @brief Checks a touch input against the keyboard positions and returns pressed charcater
     *
     * @param touch The touch position input to evaluate
     * @return int Character corresponding to the touch input, or 0 if no key was hit
     */
    int evaluateInput(touchPosition* touch);

  private:
    bool isCapsLock = false;
    bool isShift = false;

    KeyRow keyboardRows[6] = {{
                                  80,
                                  94,
                                  {{27, 41, '1'},
                                   {44, 58, '2'},
                                   {61, 75, '3'},
                                   {78, 92, '4'},
                                   {95, 109, '5'},
                                   {112, 126, '6'},
                                   {129, 143, '7'},
                                   {146, 160, '8'},
                                   {163, 177, '9'},
                                   {180, 194, '0'}},
                              },
                              {
                                  97,
                                  111,
                                  {{36, 50, 'q'},
                                   {53, 67, 'w'},
                                   {70, 84, 'e'},
                                   {87, 101, 'r'},
                                   {104, 118, 't'},
                                   {121, 135, 'y'},
                                   {138, 152, 'u'},
                                   {155, 169, 'i'},
                                   {172, 186, 'o'},
                                   {189, 203, 'p'},
                                   {206, 220, '['},
                                   {223, 237, ']'}},
                              },
                              {114,
                               128,
                               {
                                   {19, 41, static_cast<char>(KEYCODES::CAPSLOCK)},
                                   {44, 58, 'a'},
                                   {61, 75, 's'},
                                   {78, 92, 'd'},
                                   {95, 109, 'f'},
                                   {112, 126, 'g'},
                                   {129, 143, 'h'},
                                   {146, 160, 'j'},
                                   {163, 177, 'k'},
                                   {180, 194, 'l'},
                                   {197, 211, ';'},
                                   {214, 228, '\''},
                               }},
                              {131,
                               145,
                               {
                                   {19, 50, static_cast<char>(KEYCODES::SHIFT)},
                                   {53, 67, 'z'},
                                   {70, 84, 'x'},
                                   {87, 101, 'c'},
                                   {104, 118, 'v'},
                                   {121, 135, 'b'},
                                   {138, 152, 'n'},
                                   {155, 169, 'm'},
                                   {172, 186, ','},
                                   {189, 203, '.'},
                                   {206, 220, '/'},
                               }},
                              {148,
                               162,
                               {
                                   {19, 50, static_cast<char>(KEYCODES::LEFT)},
                                   {53, 84, static_cast<char>(KEYCODES::RIGHT)},
                                   {87, 186, ' '},
                                   {189, 228, static_cast<char>(KEYCODES::BACKSPACE)},
                               }},
                              {170, 184, {{100, 155, static_cast<char>(KEYCODES::CONFIRM)}}}};
};
