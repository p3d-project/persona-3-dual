#include "DialogueComponent.hpp"
#include "core/globals.hpp"
#include "core/logger.hpp"
#include "types/UITypes.hpp"

namespace
{
// text area inside the dialogue box; text beyond TEXT_MAX_X wraps and beyond TEXT_MAX_Y moves to a new page
constexpr int TEXT_X = 8;
constexpr int TEXT_Y = 154;
constexpr int TEXT_MAX_X = 248;
constexpr int TEXT_MAX_Y = 186;

constexpr int OPTIONS_X = 128;
constexpr int OPTIONS_Y = 50;
} // namespace

void DialogueComponent::Init()
{
    isActive = false;
}

void DialogueComponent::Update(ae::q20_12_t)
{
    if (closing)
    {
        if (screen == nullptr || !screen->isAnimating())
        {
            closing = false;
            isActive = false;
        }
        return;
    }

    if (!isActive || current == nullptr)
    {
        isActive = false;
        return;
    }

    if (opening)
    {
        if (screen != nullptr && screen->isAnimating())
        {
            return;
        }
        opening = false;
        advanceTo(current);
        return;
    }

    // animation
    if (!text->appearTextDone())
    {
        /// swap the background exactly once per dialogue line, on the first
        /// frame, and only if the image has actually changed
        if (renderBust)
        {
            if (screen != nullptr)
            {
                screen->renderBust(current->spritePayload);
            }

            renderBust = false;
        }
        else
        {
            optionCount = (int)current->selections.size();

            if (optionCount > 0)
            {
                // render the full text + option list now that animation ended
                doRenderOptions = true;
            }
        }
    }
    else if (doRenderOptions)
    {
        renderOptions();
        doRenderOptions = false;
    }

    // input
    u32 pressed = systemKeysHeld & ~prevKeys;
    prevKeys = systemKeysHeld;

    if (pressed & KEY_START)
    {
        end();
        return;
    }

    if (optionCount > 0)
    {
        if (text->appearTextDone())
        {
            // switch palette to green
            if (screen != nullptr)
            {
                screen->triggerAction(UIAction::SwitchToPalette1);
            }

            // selection dialogue
            if (pressed & KEY_DOWN)
            {
                selectedOption = (selectedOption + 1) % optionCount;
                renderOptions();
            }
            else if (pressed & KEY_UP)
            {
                selectedOption = (selectedOption + optionCount - 1) % optionCount;
                renderOptions();
            }
        }
        if (pressed & KEY_A)
        {
            if (skipOrContinueText())
            {
                return;
            }
            Dialogue* next = current->selections[selectedOption].next;
            if (next == nullptr)
            {
                end();
                return;
            }

            // switch palette to blue
            if (screen != nullptr)
            {
                screen->triggerAction(UIAction::SwitchToPalette0);
            }

            advanceTo(next);
        }
    }
    else
    {
        // linear dialogue
        if (pressed & KEY_A)
        {
            if (skipOrContinueText())
            {
                return;
            }
            Dialogue* next = current->next;
            if (next == nullptr)
            {
                end();
                return;
            }
            advanceTo(next);
        }
        else if (pressed & KEY_B)
        {
            if (skipOrContinueText())
            {
                return;
            }
            Dialogue* next = current->prev;
            if (next == nullptr)
            {
                end();
                return;
            }
            advanceTo(next);
        }
    }
}

void DialogueComponent::Destroy()
{
    isActive = false;
}

void DialogueComponent::configureDialogue(const DialogueConfig& config)
{
    if (config.text == nullptr || config.textAlt == nullptr)
    {
        logMessage(LogSeverity::WARNING, "Called with null text component");
    }

    text = config.text;
    textAlt = config.textAlt;
    screen = config.screen;
    prevKeys = 0;

    // load busts into memory
    if (screen != nullptr)
    {
        screen->loadBusts(config.spritePayloads);
    }
}

void DialogueComponent::start(Dialogue* firstLine)
{
    if (firstLine == nullptr)
    {
        logMessage(LogSeverity::WARNING, "Called with null firstLine");
        return;
    }

    logMessage(LogSeverity::DEBUG, "Dialogue started: " + firstLine->name);

    current = firstLine;
    optionCount = 0;
    selectedOption = 0;
    doRenderOptions = false;
    text->clearScreen();
    opening = true;
    closing = false;
    if (screen != nullptr)
    {
        screen->showAnimated();
    }

    prevKeys = systemKeysHeld;
    isActive = true;
    renderBust = true;
}

void DialogueComponent::end()
{
    logMessage(LogSeverity::DEBUG, "Dialogue ended");

    if (text != nullptr)
    {
        text->clearScreen();
    }

    if (screen != nullptr)
    {
        // Keep the component active until hideAnimated() finishes; Update()
        // watches `closing` and clears isActive when !screen->isAnimating().
        screen->hideAnimated();
        closing = true;
        return;
    }
    isActive = false;
}

void DialogueComponent::advanceTo(Dialogue* next)
{
    if (next == nullptr)
    {
        logMessage(LogSeverity::WARNING, "Called with null Dialogue");
        end();
        return;
    }

    logMessage(LogSeverity::DEBUG, "Dialogue advanced to: " + next->name);

    current = next;
    doRenderOptions = false;
    optionCount = 0;
    selectedOption = 0;
    renderBust = true;
    if (text != nullptr)
    {
        text->clearScreen();
    }
    renderAnimFrame();
}

void DialogueComponent::renderAnimFrame()
{
    if (!text || !textAlt || !current)
    {
        logMessage(LogSeverity::WARNING, "text, textAlt, or current is null");
        return;
    }

    textAlt->drawText("\xFF\x02\x01" + current->name, 8, 128, TextColor::RichBlue);
    text->appearText(current->text, TEXT_X, TEXT_Y, TextColor::Black, TEXT_MAX_X, TEXT_MAX_Y);
}

bool DialogueComponent::skipOrContinueText()
{
    if (text->appearTextDone())
    {
        return false;
    }

    // a full page waits for input, otherwise finish the current page instantly
    if (text->appearTextPaused())
    {
        text->appearTextNextPage();
    }
    else
    {
        text->appearTextSkip();
    }
    return true;
}

void DialogueComponent::renderOptions()
{
    if (!text || !textAlt || !current)
    {
        logMessage(LogSeverity::WARNING, "text, textAlt, or current is null");
        return;
    }

    // the line itself stays on screen (it may span several pages), only the name and options are redrawn
    textAlt->drawText("\xFF\x02\x01" + current->name, 8, 128, TextColor::DualGreen);
    //TODO: The options are currently drawn outside the textbox, do we want to add an extra overlay for them similar to the actual game?
    int textSize = text->getFontSize();
    text->clearArea(OPTIONS_X, OPTIONS_Y, 256 - OPTIONS_X, textSize * optionCount + 4);
    for (int i = 0; i < optionCount; i++)
    {
        if (i == selectedOption)
        {
            text->drawText(current->selections[i].text, OPTIONS_X, OPTIONS_Y + textSize * i, TextColor::Blue);
        }
        else
        {
            text->drawText(current->selections[i].text, OPTIONS_X, OPTIONS_Y + textSize * i, TextColor::White);
        }
    }
}
