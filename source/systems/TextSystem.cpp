#include "TextSystem.hpp"

constexpr char INSTRUCTION_BIT = 0xFF; /// Special value used to indicate that the next byte is an instruction

void TextSystem::drawText(const std::string& text,
                          Font* font,
                          uint16_t* videoBuffer,
                          int startX,
                          int startY,
                          int color,
                          int letterSpacing,
                          int lineSpacing,
                          int spaceWidth)
{
    Text* textObj = createText(text, font, videoBuffer, startX, startY, color, letterSpacing, lineSpacing, spaceWidth);

    while (textObj->cursorPos < (int)textObj->content.size())
    {
        drawNextFromText(textObj);
        textObj->cursorPos++;
    }
    delete textObj;
}

void TextSystem::appearText(Text*& appearingText,
                            const std::string& content,
                            Font* font,
                            uint16_t* videoBuffer,
                            int startX,
                            int startY,
                            int color,
                            int letterSpacing,
                            int lineSpacing,
                            int spaceWidth)
{
    if (appearingText != nullptr)
        delete appearingText;
    appearingText =
        createText(content, font, videoBuffer, startX, startY, color, letterSpacing, lineSpacing, spaceWidth);
}

void TextSystem::appearTextSkip(Text*& appearingText)
{
    if (appearingText != nullptr)
    {
        while (appearingText->cursorPos < (int)appearingText->content.size())
        {
            drawNextFromText(appearingText);
            appearingText->cursorPos++;
        }
    }
}

bool TextSystem::appearTextDone(Text*& appearingText)
{
    if (appearingText == nullptr)
        return true;
    if (appearingText->cursorPos >=
        (int)appearingText->content.size()) // not really sure if this is needed but it should be safe to check anyway
        return true;
    return false;
}

void TextSystem::drawGlyph(const Glyph& glyph,
                           Font* font,
                           uint16_t* videoBuffer,
                           int cursorX,
                           int cursorY,
                           int color,
                           bool bold,
                           bool italic,
                           bool underline)
{
    for (int y = 0; y < glyph.height; y++)
    {
        int distY = italic ? glyph.height - 1 - y : 0; /// Distance from baseline
        int italicOffset =
            italic ? (distY * SLANT_FACTOR) >> 8 : 0; /// Integer math equivalent of distY * (SLANT_FACTOR/256)
        for (int x = 0; x < glyph.width; x++)
        {
            int bitmapX = glyph.xPos + x;
            int bitmapY = glyph.yPos + y;
            int bitmapIndex = bitmapY * font->bitmapWidth + bitmapX;

            sassert(bitmapIndex < font->bitmapWidth * font->bitmapHeight, "Bitmap index out of bounds");

            int pixelValue = bold ? font->bitmapBold[bitmapIndex] : font->bitmap[bitmapIndex];
            if (pixelValue > 0)
            {
                int screenX = cursorX + x + italicOffset;
                int screenY = cursorY + glyph.yOffset + y;
                if (screenX >= 0 && screenX < 256 && screenY >= 0 && screenY < 192)
                    drawPixel(videoBuffer, screenX, screenY, color);
            }
        }
    }
    if (underline)
    {
        int underlineY = cursorY + font->lineHeight - 2; /// Position the underline just below the glyph
        for (int x = 0; x < glyph.width; x++)
        {
            int screenX = cursorX + x;
            if (screenX >= 0 && screenX < 256 && underlineY >= 0 && underlineY < 192)
                drawPixel(videoBuffer, screenX, underlineY, color);
        }
    }
}

void TextSystem::clearArea(uint16_t* videoBuffer, int x, int y, int width, int height)
{
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            int pixelX = x + col;
            int pixelY = y + row;
            if (pixelX >= 0 && pixelX < 256 && pixelY >= 0 && pixelY < 192)
            {
                drawPixel(videoBuffer, pixelX, pixelY, TextColor::Transparent);
            }
        }
    }
}

void TextSystem::drawNextFromText(Text*& text)
{
    unsigned char c = text->content[text->cursorPos];

    ///Handle Newline
    if (c == '\n')
    {
        text->cursorX = text->startX;
        text->cursorY += text->font->lineHeight + text->lineSpacing;
    }
    else if (c == ' ')
    {
        if (text->cursorX == text->startX)
        {
            return; /// Don't add a space at the beginning of a line
        }
        std::string nextWord = getNextWord(text->content.substr(text->cursorPos + 1));
        if (checkWordWrap(nextWord, text->font, text->cursorX, text->bold, text->letterSpacing))
        {
            text->cursorX = text->startX;
            text->cursorY += text->font->lineHeight + text->lineSpacing;
        }
        else
        {
            if (text->underline)
            {
                underlineGap(text->cursorX,
                             text->cursorY + text->font->lineHeight - 2,
                             text->spaceWidth,
                             text->videoBuffer,
                             text->activeColor);
            }
            text->cursorX += text->spaceWidth;
        }
    }
    else if (c == INSTRUCTION_BIT) /// Handle special instructions for text formatting
    {
        c = getNextChar(text);
        if (c == TextInstruction::ColorChange) /// Color change
        {
            c = getNextChar(text);
            if (c == TextInstruction::Reset) /// Reset to base color
                text->activeColor = text->baseColor;
            else if (c < 256) /// bg palette only has 256 colors
                text->activeColor = static_cast<int>(c);
        }
        else if (c == TextInstruction::StyleChange)
        {
            c = getNextChar(text);
            if (c == TextInstruction::Reset) /// Reset all styles
            {
                text->bold = false;
                text->italic = false;
                text->underline = false;
            }
            else
            {
                /// Extract style flags from indivudual bits
                text->bold = (c & TextInstruction::StyleBold) != 0 &&
                             text->font->boldLoaded; /// Only apply bold if the font has a bold bitmap
                text->italic = (c & TextInstruction::StyleItalic) != 0;
                text->underline = (c & TextInstruction::StyleUnderline) != 0;
            }
        }
    }
    else if (text->font->glyphs[c].width == 0)
        text->cursorX += text->spaceWidth; /// If the glyph width is 0, skip it (char has not been defined in the font)
    else
    {
        Glyph g = text->bold ? text->font->boldGlyphs[c] : text->font->glyphs[c];
        drawGlyph(g,
                  text->font,
                  text->videoBuffer,
                  text->cursorX,
                  text->cursorY,
                  text->activeColor,
                  text->bold,
                  text->italic,
                  text->underline);
        if (text->underline)
        {
            underlineGap(text->cursorX + g.width,
                         text->cursorY + text->font->lineHeight - 2,
                         text->letterSpacing,
                         text->videoBuffer,
                         text->activeColor);
        }
        text->cursorX += g.width + text->letterSpacing;
    }
}

Text* TextSystem::createText(const std::string& text,
                             Font* font,
                             uint16_t* videoBuffer,
                             int startX,
                             int startY,
                             int color,
                             int letterSpacing,
                             int lineSpacing,
                             int spaceWidth)
{
    Text* newText = new Text();
    newText->cursorX = startX;
    newText->cursorY = startY;
    newText->startX = startX;
    newText->startY = startY;
    newText->content = text;
    newText->baseColor = color;
    newText->activeColor = color;
    newText->font = font;
    newText->videoBuffer = videoBuffer;
    newText->cursorPos = 0; // Start at the beginning of the text
    newText->bold = false;
    newText->italic = false;
    newText->underline = false;
    newText->letterSpacing = letterSpacing;
    newText->lineSpacing = lineSpacing;
    newText->spaceWidth = spaceWidth;
    return newText;
}

char TextSystem::getNextChar(Text* text)
{
    if (text->cursorPos + 1 < (int)text->content.size())
        return text->content[++text->cursorPos];
    return 0xFE; /// Return a special value indicating no more characters
}

void TextSystem::drawPixel(uint16_t* videoBuffer, int x, int y, int paletteValue)
{
    int wordIndex = (y * 256 + x) / 2;
    u16 currentWord = videoBuffer[wordIndex];
    if (x % 2 == 0) //Clear the lower 8 bits, then inject our 8-bit color index
        videoBuffer[wordIndex] = (currentWord & 0xFF00) | (paletteValue & 0xFF);
    else //Clear the upper 8 bits, then inject our 8-bit color index shifted up
        videoBuffer[wordIndex] = (currentWord & 0x00FF) | ((paletteValue & 0xFF) << 8);
}

std::string TextSystem::getNextWord(const std::string& text)
{
    std::string nextWord = "";
    int i = 0;
    while (i < (int)text.size() && text[i] != ' ' && text[i] != '\n')
    {
        nextWord += text[i];
        i++;
    }
    return nextWord;
}

bool TextSystem::checkWordWrap(const std::string& text, Font* font, int startX, bool bold, int letterSpacing)
{
    int cursorX = startX;
    for (char c : text)
    {
        Glyph g = bold ? font->boldGlyphs[static_cast<unsigned char>(c)] : font->glyphs[static_cast<unsigned char>(c)];
        cursorX += g.width + letterSpacing;
    }
    if (cursorX > 256)
        return true; // Word exceeds screen width
    return false;
}

void TextSystem::underlineGap(int startX, int y, int width, uint16_t* videoBuffer, int color)
{
    for (int x = 0; x < width; x++)
    {
        int screenX = startX + x;
        if (screenX >= 0 && screenX < 256 && y >= 0 && y < 192)
            drawPixel(videoBuffer, screenX, y, color);
    }
}

void TextSystem::testBitmap(Font* font, uint16_t* videoBuffer)
{
    for (int y = 0; y < 256; y++)
    {
        for (int x = 0; x < 256; x++)
        {
            int index = font->bitmap[y * font->bitmapWidth + x];
            int pixelValue = font->bitmap[index];
            if (pixelValue > 0)
                drawPixel(videoBuffer, x, y, DualGreen);
        }
    }
}

void TextSystem::testPalette(uint16_t* videoBuffer)
{
    for (int i = 0; i < 128; i += 2)
    {
        for (int y = 0; y < 50; y++)
        {
            drawPixel(videoBuffer, i, y, i);
            drawPixel(videoBuffer, i + 1, y, i);
        }
    }
    for (int i = 0; i < 128; i += 2)
    {
        for (int y = 0; y < 50; y++)
        {
            drawPixel(videoBuffer, i, y + 50, i + 128);
            drawPixel(videoBuffer, i + 1, y + 50, i + 128);
        }
    }
}
