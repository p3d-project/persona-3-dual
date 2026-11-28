#pragma once

#include "types/StateTypes.hpp"
#include <etl/span.h>

class UIMenu;

struct MenuOption
{
    const char* name;
    int bgIndex;
    ViewState (UIMenu::*onSelect)();
};

struct MenuState
{
    etl::span<MenuOption> options;
    int selectedOption;
    int startIndex;
};

enum class UIAction
{
    SWITCH_TO_PALETTE0 = 0,
    SWITCH_TO_PALETTE1,
};
