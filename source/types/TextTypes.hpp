#pragma once

#include <cstdint>
#include <nds.h>
#include <string>

/**
 * @brief Human Readable enum for text colors.
 */
enum TextColor
{
    Transparent = 0, // 8bpp uses palette index 0 as transparent
    Black = 235,
    White = 236,
    DualGreen = 237,
    DualGreen2 = 238,
    DarkGreen = 239,
    DarkerGreen = 240,
    DarkestGreen = 241,
    LightBlue = 242,
    RichBlue = 243,
    DarkBlue = 244,
    NavyBlue = 245,
    DarkestBlue = 246,
    LightOrange = 247,
    LightPurple = 248,
    Red = 249,
    Green = 250,
    Blue = 251,
    Yellow = 252,
    Magenta = 253,
    Cyan = 254,
    Gray = 255
};

/**
 * @brief Human readable enum for text instructions.
 */
enum TextInstruction
{
    ColorChange = 0x01,
    StyleChange = 0x02,
    StyleBold = 0x01,
    StyleItalic = 0x02,
    StyleUnderline = 0x04,
    Reset = 0xFF
};

/**
 * @brief Stores data for a single glyph (character) in a font.
 */
struct Glyph
{
    int xPos;
    int yPos;
    int width = 0; /// Used to check if the glyph was read in correctly. Setting it to 0 here wipes any old data
    int height;
    int xOffset;
    int yOffset;
};

/**
 * @brief Stores data for a font.
 * @note Assumes that the regular and bold (if present) font bitmaps are the same size.
 */
struct Font
{
    std::uint8_t* bitmap = nullptr;
    std::uint8_t* bitmapBold = nullptr;
    int bitmapWidth = 256;
    int bitmapHeight = 256;
    int lineHeight = 32;
    Glyph glyphs[256];
    Glyph boldGlyphs[256];
    bool boldLoaded = false;
};

/**
 * @brief A struct that represents a block of text being rendered on the screen.
 */
struct Text
{
    int cursorX;
    int cursorY;
    int startX;
    int startY;
    std::string content;
    Font* font;
    uint16_t* videoBuffer;
    int cursorPos;
    int baseColor;
    int activeColor;
    int counter;
    bool bold;
    bool italic;
    bool underline;
    int letterSpacing;
    int lineSpacing;
    int spaceWidth;
};

/**
 * @brief A struct that holds initial config values for the TextComponent
 */
struct TextConfig
{
    uint16_t* videoBuffer = nullptr;

    /// for loadFont
    std::string* fontNamePath = nullptr;
    int fontSize;

    /// for loadFontBitmap
    std::string* fontBitmapPath = nullptr;

    /// for loadPalette
    std::string* fontPalettePath = nullptr;
    bool isSub;

    /// for loadFontMetadata
    std::string* fontMetadataPath = nullptr;
    bool isBoldBitmap;

    TextConfig() = default;

    /// load font (loads font, font bitmap, font metadata. NOT font palette)
    TextConfig(uint16_t* iVideoBuffer, std::string* iFontNamePath, int iFontSize)
        : videoBuffer(iVideoBuffer), fontNamePath(iFontNamePath), fontSize(iFontSize)
    {
    }

    /// load font palette
    TextConfig(uint16_t* iVideoBuffer, std::string* iFontPalettePath, bool iIsSub)
        : videoBuffer(iVideoBuffer), fontPalettePath(iFontPalettePath), isSub(iIsSub)
    {
    }
};
