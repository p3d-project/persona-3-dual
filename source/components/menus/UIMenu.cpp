#include "UIMenu.hpp"

#include "managers/MathManager.hpp"
#include "uiAnimation/UIEase.hpp"

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
    const ae::q20_12_t dt = MathManager::GetInstance().div(ae::q20_12_t{1}, ae::q20_12_t{60});
    animator.update(dt);
}

int UIMenu::selectionOffset() const
{
    return selectionX;
}

void UIMenu::animateSelection()
{
    animator.cancelAll();
    selectionX = -8;
    animator.animate(selectionX).from(-8).to(0).duration(140).ease(UIEase::OutBack).start();
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
