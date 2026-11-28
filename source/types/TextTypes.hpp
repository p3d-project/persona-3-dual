#pragma once

#include <cstdint>
#include <cstdlib>
#include <string>

/**
 * @brief Human Readable enum for text colors.
 */
enum class TextColor : uint8_t
{
    TRANSPARENT = 0, // 8bpp uses palette index 0 as transparent
    BLACK = 235,
    WHITE = 236,
    DUAL_GREEN = 237,
    DUAL_GREEN2 = 238,
    DARK_GREEN = 239,
    DARKER_GREEN = 240,
    DARKEST_GREEN = 241,
    LIGHT_BLUE = 242,
    RICH_BLUE = 243,
    DARK_BLUE = 244,
    NAVY_BLUE = 245,
    DARKEST_BLUE = 246,
    LIGHT_ORANGE = 247,
    LIGHT_PURPLE = 248,
    RED = 249,
    GREEN = 250,
    BLUE = 251,
    YELLOW = 252,
    MAGENTA = 253,
    CYAN = 254,
    GRAY = 255
};

/**
 * @brief Human readable enum for text instructions.
 */
enum class TextInstruction : uint8_t
{
    COLOR_CHANGE = 0x01,
    STYLE_CHANGE = 0x02,
    STYLE_BOLD = 0x01,
    STYLE_ITALIC = 0x02,
    STYLE_UNDERLINE = 0x04,
    RESET = 0xFF
};

inline constexpr unsigned char TEXT_INSTRUCTION_MARKER = 0xFF;

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
    Font() = default;

    /**
     * @brief Frees the regular and bold bitmaps.
     */
    ~Font()
    {
        free(bitmap);
        free(bitmapBold);
    }

    Font(const Font&) = delete;
    Font& operator=(const Font&) = delete;

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
    TextColor baseColor;
    TextColor activeColor;
    int counter;
    bool bold;
    bool italic;
    bool underline;
    int letterSpacing;
    int lineSpacing;
    int spaceWidth;

    /// Right edge (exclusive) used for word wrapping
    int maxX = 256;
    /// Bottom edge used for pagination. 0 disables pagination
    int maxY = 0;
    /// Set when the next glyph would not fit above maxY. Output pauses until the next page is requested
    bool pageBreak = false;
    bool hyphenate = false;
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
