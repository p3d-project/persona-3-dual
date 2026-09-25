#include "TextComponent.hpp"
#include <aegis/ndsTypes.hpp>
#include <aegis/types.hpp>
#include <algorithm>

void TextComponent::Update(ae::q20_12_t)
{
    if (appearingText != nullptr)
    {
        if (appearingText->cursorPos < (int)appearingText->content.size())
        {
            if (appearingText->counter <= 0)
            {
                ts.drawNextFromText(appearingText);
                appearingText->cursorPos++;
                appearingText->counter = ts.APPEAR_DELAY; // Reset the counter for the next character
            }
            else
                appearingText->counter--;
        }
        else //this text has finished appearing, so we can clear the storage
        {
            delete appearingText;
            appearingText = nullptr;
        }
    }
}

void TextComponent::Destroy()
{
    isActive = false;
    appearTextStop();
    tm.unloadFont(font);
    font = nullptr;
}

void TextComponent::configureText(const TextConfig& config, bool loadDefaultPalette)
{
    isActive = true;

    if (font != nullptr)
    {
        tm.unloadFont(font);
        font = nullptr;
    }

    // set video buffer
    if (config.videoBuffer != nullptr)
    {
        videoBuffer = config.videoBuffer;
    }

    /// @note Loading a font and loading a font palette are mutually exclusive
    // load font
    if (config.fontNamePath != nullptr)
    {
        font = tm.loadFont(config.fontNamePath, config.fontSize);
        fontSize = config.fontSize;

        /// Auto-derive spacing from fontSize, using the same ratios the old fixed
        /// defaults had at a 12px font (1/12 letter, 2/12 line, 2/12 space).
        /// Call the setters after configureText() to override any of these manually.
        letterSpacing = std::max(1, fontSize / 12);
        lineSpacing = std::max(1, fontSize / 6);
        spaceWidth = std::max(1, fontSize / 6);
    }
    // load font palette
    else if (config.fontPalettePath != nullptr)
    {
        tm.loadPalette(config.fontPalettePath, config.isSub);
    }

    // load default font palette
    if (loadDefaultPalette)
    {
        sassert(font != nullptr, "Cannot load a font palette if a font has not been loaded first!");
        tm.loadDefaultPalette();
    }
}

void TextComponent::drawText(const std::string& text, int x, int y, int color)
{
    ts.drawText(text, font, videoBuffer, x, y, color, letterSpacing, lineSpacing, spaceWidth);
}

void TextComponent::appearText(const std::string& text, int x, int y, int color)
{
    ts.appearText(appearingText, text, font, videoBuffer, x, y, color, letterSpacing, lineSpacing, spaceWidth);
}

void TextComponent::appearTextSkip()
{
    ts.appearTextSkip(appearingText);
}

void TextComponent::appearTextStop()
{
    if (appearingText != nullptr)
    {
        delete appearingText;
        appearingText = nullptr;
    }
}

bool TextComponent::appearTextDone()
{
    return ts.appearTextDone(appearingText);
}

void TextComponent::drawGlyph(const Glyph& glyph, int x, int y, int color, bool bold, bool italic, bool underline)
{
    ts.drawGlyph(glyph, font, videoBuffer, x, y, color, bold, italic, underline);
}

void TextComponent::clearArea(int x, int y, int width, int height)
{
    ts.clearArea(videoBuffer, x, y, width, height);
}

void TextComponent::clearScreen()
{
    tm.clearScreen(videoBuffer);
    appearTextStop();
}

int TextComponent::getFontSize()
{
    return fontSize;
}

int TextComponent::getLineSpacing()
{
    return lineSpacing;
}

int TextComponent::getLetterSpacing()
{
    return letterSpacing;
}

int TextComponent::getSpaceWidth()
{
    return spaceWidth;
}

void TextComponent::setLetterSpacing(int value)
{
    letterSpacing = value;
}

void TextComponent::setLineSpacing(int value)
{
    lineSpacing = value;
}

void TextComponent::setSpaceWidth(int value)
{
    spaceWidth = value;
}

void TextComponent::testBitmap()
{
    ts.testBitmap(font, videoBuffer);
}

void TextComponent::testPalette()
{
    ts.testPalette(videoBuffer);
}
