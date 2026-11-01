#include "TextManager.hpp"
#include "core/logger.hpp"
#include <sstream>

// Helper constexpr to convert 8 bit RGB values to 5 bit RGB values for the Nintendo DS
constexpr uint16_t nds_rgb(uint8_t r, uint8_t g, uint8_t b)
{
    return static_cast<uint16_t>((r >> 3) | (((g) >> 3) << 5) | (((b) >> 3) << 10) | BIT(15));
}

static const uint16_t customPalette[21] = {
    ARGB16(1, 0, 0, 0),     // Black         235
    ARGB16(1, 31, 31, 31),  // White         236
    nds_rgb(0, 202, 105),   // Dual Green    237
    nds_rgb(18, 168, 88),   // Dual Green 2  238
    nds_rgb(28, 118, 55),   // Dark Green    239
    nds_rgb(0, 69, 40),     // Darker Green  240
    nds_rgb(4, 34, 18),     // Darkest Green 241
    nds_rgb(121, 206, 255), // Light Blue    242
    nds_rgb(9, 137, 253),   // Rich Blue     243
    nds_rgb(0, 104, 208),   // Dark Blue     244
    nds_rgb(0, 44, 208),    // Navy Blue     245
    nds_rgb(0, 0, 36),      // Darkest Blue  246
    nds_rgb(245, 198, 164), // Light Orange  247
    nds_rgb(198, 164, 245), // Light Purple  248
    //Defaults
    ARGB16(1, 31, 0, 0),   // Red           249
    ARGB16(1, 0, 31, 0),   // Green         250
    ARGB16(1, 0, 0, 31),   // Blue          251
    ARGB16(1, 31, 31, 0),  // Yellow        252
    ARGB16(1, 31, 0, 31),  // Magenta       253
    ARGB16(1, 0, 31, 31),  // Cyan          254
    ARGB16(1, 15, 15, 15), // Gray          255
};

Font* TextManager::loadFont(std::string* name, int size)
{
    if (name == nullptr)
    {
        logMessage(LogSeverity::FATAL, "name cannot be nullptr");
    }

    Font* font = new Font();
    std::string fullPath = "fonts/" + *name + "/size-" + std::to_string(size);

    std::string fontBitmapPath = fullPath + ".img.bin";
    font->bitmap = loadFontBitmap(&fontBitmapPath);
    if (!font->bitmap)
    {
        logMessage(LogSeverity::FATAL, "Failed to load font bitmap from \n" + fullPath + ".img.bin");
    }

    std::string fontMetadataPath = fullPath + ".fnt";
    if (!loadFontMetadata(&fontMetadataPath, font))
    {
        logMessage(LogSeverity::FATAL, "Failed to load font metadata from \n" + fullPath + ".fnt");
    }

    std::string fontBitmapBoldPath = fullPath + "-bold.img.bin";
    std::string fontMetadataBoldPath = fullPath + "-bold.fnt";
    font->bitmapBold = loadFontBitmap(&fontBitmapBoldPath);
    if (!font->bitmapBold || !loadFontMetadata(&fontMetadataBoldPath, font, true))
    {
        font->boldLoaded = false;
        free(font->bitmapBold);
        font->bitmapBold = nullptr;
    }
    else
    {
        font->boldLoaded = true;
    }

    return font;
}

void TextManager::unloadFont(Font* font)
{
    if (font == nullptr)
    {
        return;
    }

    free(font->bitmap);
    free(font->bitmapBold);
    delete font;
}

void TextManager::loadDefaultPalette()
{
    dmaCopy(customPalette, BG_PALETTE + 235, 21 * sizeof(uint16_t));
    dmaCopy(customPalette, BG_PALETTE_SUB + 235, 21 * sizeof(uint16_t));
}

bool TextManager::loadPalette(std::string* path, bool sub)
{
    if (path == nullptr)
    {
        logMessage(LogSeverity::FATAL, "path cannot be nullptr");
    }

    FileBuffer buffer = io.openFileBuffer(*path);
    if (buffer.get() == nullptr)
    {
        logMessage(LogSeverity::WARNING, "Failed to load palette from: " + *path);
        return false;
    }

    std::uint16_t* fontPalette = reinterpret_cast<std::uint16_t*>(buffer.get());
    if (sub)
    {
        dmaCopy(fontPalette, BG_PALETTE_SUB, 256 * sizeof(uint16_t));
    }
    else
    {
        dmaCopy(fontPalette, BG_PALETTE, 256 * sizeof(uint16_t));
    }

    return true;
}

void TextManager::unloadPalette()
{
    dmaFillHalfWords(0, BG_PALETTE, 256 * sizeof(uint16_t));
    dmaFillHalfWords(0, BG_PALETTE_SUB, 256 * sizeof(uint16_t));
}

std::uint8_t* TextManager::loadFontBitmap(std::string* path)
{
    if (path == nullptr)
    {
        logMessage(LogSeverity::FATAL, "path cannot be nullptr");
    }

    FileBuffer buffer = io.openFileBuffer(*path);
    if (buffer.get() == nullptr)
    {
        logMessage(LogSeverity::WARNING, "Failed to load font bitmap from: " + *path);
        return nullptr;
    }

    return reinterpret_cast<std::uint8_t*>(buffer.release());
}

bool TextManager::loadFontMetadata(std::string* path, Font* font, bool forBoldBitmap)
{
    if (path == nullptr)
    {
        logMessage(LogSeverity::FATAL, "path cannot be nullptr");
    }

    if (font == nullptr)
    {
        logMessage(LogSeverity::ERROR, "Font cannot be nullptr");
        return false;
    }

    FileBuffer buffer = io.openFileBuffer(*path);
    if (buffer.get() == nullptr)
    {
        logMessage(LogSeverity::WARNING, "Failed to load font metadata from: " + *path);
        return false;
    }

    std::string content(reinterpret_cast<char*>(buffer.get()), buffer.length());

    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line))
    {
        if (line.rfind("common ", 0) == 0 && !forBoldBitmap)
        {
            font->lineHeight = extractIntValue(line, "lineHeight=");
            font->bitmapWidth = extractIntValue(line, "scaleW=");
            font->bitmapHeight = extractIntValue(line, "scaleH=");
            continue;
        }

        //if there's no character in this line, move to the next
        if (line.rfind("char ", 0) != 0)
        {
            continue;
        }

        const int charID = extractIntValue(line, "id=");
        if (charID < 0 || charID >= 256)
        {
            continue;
        }

        Glyph glyph{};
        glyph.xPos = extractIntValue(line, "x=");
        glyph.yPos = extractIntValue(line, "y=");
        glyph.width = extractIntValue(line, "width=");
        glyph.height = extractIntValue(line, "height=");
        glyph.xOffset = extractIntValue(line, "xoffset=");
        glyph.yOffset = extractIntValue(line, "yoffset=");
        if (forBoldBitmap)
        {
            font->boldGlyphs[charID] = glyph;
        }
        else
        {
            font->glyphs[charID] = glyph;
        }
    }

    return true;
}

void TextManager::clearScreen(uint16_t* videoBuffer)
{
    if (videoBuffer == nullptr)
    {
        logMessage(LogSeverity::WARNING, "Called with null videoBuffer");
        return;
    }
    dmaFillHalfWords(0, videoBuffer, 256 * 256 * sizeof(uint8_t));
}

int TextManager::extractIntValue(const std::string& line, const std::string& key)
{
    std::size_t keyPos = line.find(key);
    if (keyPos == std::string::npos)
    {
        return 0;
    }

    const std::size_t dataStart = keyPos + key.size();
    if (dataStart >= line.size())
    {
        return 0;
    }

    char* endPtr = nullptr;
    long val = std::strtol(line.c_str() + dataStart, &endPtr, 10);
    if (endPtr == line.c_str() + dataStart)
    {
        logMessage(LogSeverity::WARNING, "Failed to parse int value for key: " + key);
        return 0;
    }

    return static_cast<int>(val);
}
