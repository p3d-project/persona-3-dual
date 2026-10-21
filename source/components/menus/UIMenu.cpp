#include "UIMenu.hpp"
#include "core/error.hpp"

void UIMenu::resetMenu()
{
    options = {};
    selectedOption = 0;
    startIndex = 0;
    isActive = false;
    nextViewState = ViewState::KEEP_CURRENT;

    if (text != nullptr)
    {
        text->clearScreen();
    }

    resetHook();

    while (!prevOptions.empty())
    {
        prevOptions.pop();
    }
}

ViewState UIMenu::changeMenu(etl::span<MenuOption> newOptions)
{
    if (prevOptions.full())
    {
        logMessage(LogSeverity::WARNING, "prevOptions stack is full");
        return ViewState::KEEP_CURRENT;
    }

    // add to prevOptions
    MenuState currentState = {options, selectedOption, startIndex};
    prevOptions.push(currentState);

    logMessage(LogSeverity::DEBUG, "Menu options changed, count: " + std::to_string(newOptions.size()));

    // set new options
    options = newOptions;
    selectedOption = 0;
    startIndex = 0;

    return ViewState::KEEP_CURRENT;
}

void UIMenu::prevOption()
{
    // if we're in a submenu, return to main menu
    if (!prevOptions.empty())
    {
        MenuState prevState = prevOptions.top();
        prevOptions.pop();

        options = prevState.options;
        selectedOption = prevState.selectedOption;
        startIndex = prevState.startIndex;
    }
    else
    {
        // otherwise, close the menu
        closeHook();
    }
}

ViewState UIMenu::updateHook()
{
    return ViewState::DEFAULT;
}
