#include "UIMenu.hpp"

#include "animation/Easing.h"

void UIMenu::resetMenu()
{
    options = {};
    selectedOption = 0;
    startIndex = 0;
    isActive = false;
    nextViewState = ViewState::KEEP_CURRENT;
    selectionX = 0;
    animator.cancelAll();

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

void UIMenu::updateAnimation()
{
    animator.update(1.0f);
}

int UIMenu::selectionOffset() const
{
    return selectionX;
}

void UIMenu::animateSelection()
{
    animator.cancelAll();
    selectionX = -8;
    animator.animate(selectionX).from(-8).to(0).duration(140).ease(uiAnimation::Ease::OutBack).start();
}

ViewState UIMenu::changeMenu(etl::span<MenuOption> newOptions)
{
    // add to prevOptions
    MenuState currentState = {options, selectedOption, startIndex};
    prevOptions.push(currentState);

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
