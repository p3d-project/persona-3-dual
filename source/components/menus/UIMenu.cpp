#include "UIMenu.hpp"
#include "core/logger.hpp"

#include "managers/MathManager.hpp"
#include "uiAnimation/UIEase.hpp"

#include <algorithm>

void UIMenu::resetMenu()
{
    options = {};
    selectedOption = 0;
    startIndex = 0;
    isActive = false;
    nextViewState = ViewState::KEEP_CURRENT;
    selectionX = 0;
    animator.cancelAll();

    alertText.clear();
    alertActive = false;
    alertMessagePrinted = false;

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
    const ae::q20_12_t dt = MathManager::getInstance().div(ae::q20_12_t{1}, ae::q20_12_t{60});
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

void UIMenu::clearAlertArea()
{
    if (text == nullptr || alertText.empty())
    {
        return;
    }

    const int lineHeight = text->getFontSize() + text->getLineSpacing();
    int lineCount = 1;
    int lineWidth = 0;

    for (size_t i = 0; i < alertText.size(); ++i)
    {
        char c = alertText[i];
        if (c == '\n')
        {
            if (i + 1 < alertText.size())
            {
                ++lineCount;
                lineWidth = 0;
            }
        }
        else if (c == ' ')
        {
            lineWidth += text->getSpaceWidth();
        }
        else
        {
            Glyph* glyph = text->getGlyph(c);
            const int charWidth = (glyph != nullptr && glyph->width > 0) ? (glyph->width + text->getLetterSpacing())
                                                                         : text->getSpaceWidth();
            if (lineWidth + charWidth > 256)
            {
                ++lineCount;
                lineWidth = charWidth;
            }
            else
            {
                lineWidth += charWidth;
            }
        }
    }

    text->clearArea(0, 0, 256, std::min(192, lineCount * lineHeight));
}

void UIMenu::showAlert(std::string_view message, ae::q20_12_t durationSeconds)
{
    alertText = message;
    alertTimer.start(durationSeconds);
    alertActive = true;
    alertMessagePrinted = false;
    animator.cancelAll();
    selectionX = 0;
}

bool UIMenu::isAlertActive() const
{
    return alertActive && !alertTimer.isFinished();
}

bool UIMenu::isAlertDisplayed() const
{
    return isAlertActive() || alertMessagePrinted;
}

void UIMenu::clearAlert()
{
    clearAlertArea();
    alertActive = false;
    alertMessagePrinted = false;
    alertText.clear();
}
