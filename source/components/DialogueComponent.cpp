#include "DialogueComponent.hpp"
#include "core/error.hpp"
#include "core/globals.hpp"
#include "types/UITypes.hpp"

void DialogueComponent::Init()
{
    isActive = false;
}

void DialogueComponent::Update(ae::q20_12_t)
{
    if (!isActive || current == nullptr)
    {
        isActive = false;
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
            if (!text->appearTextDone())
            {
                text->appearTextSkip();
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
            if (!text->appearTextDone())
            {
                text->appearTextSkip();
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
            if (!text->appearTextDone())
            {
                text->appearTextSkip();
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

    // point to first line
    advanceTo(firstLine);

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
    text->appearText(current->text, 8, 154, TextColor::Black);
}

void DialogueComponent::renderOptions()
{
    if (!text || !textAlt || !current)
    {
        logMessage(LogSeverity::WARNING, "text, textAlt, or current is null");
        return;
    }

    /// reprint the complete line then list choices below it
    text->clearScreen();
    textAlt->drawText("\xFF\x02\x01" + current->name, 8, 128, TextColor::DualGreen);
    text->drawText(current->text, 8, 154, TextColor::Black);
    //TODO: The options are currently drawn outside the textbox, do we want to add an extra overlay for them similar to the actual game?
    int textSize = text->getFontSize();
    for (int i = 0; i < optionCount; i++)
    {
        if (i == selectedOption)
        {
            text->drawText(current->selections[i].text, 128, 50 + textSize * i, TextColor::Blue);
        }
        else
        {
            text->drawText(current->selections[i].text, 128, 50 + textSize * i, TextColor::White);
        }
    }
}
