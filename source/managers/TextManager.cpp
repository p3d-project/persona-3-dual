#include "TextManager.hpp"
#include <sstream>

// Helper macro to convert 8 bit RGB values to 5 bit RGB values for the Nintendo DS
#define NDS_RGB(r, g, b) (uint16_t)((r) >> 3) | (((g) >> 3) << 5) | (((b) >> 3) << 10) | BIT(15)

static const uint16_t customPalette[256] = {
    ARGB16(0, 0, 0, 0),     // Transparent   0
    ARGB16(1, 0, 0, 0),     // Black         1
    ARGB16(1, 31, 31, 31),  // White         2
    NDS_RGB(0, 202, 105),   // Dual Green    3
    NDS_RGB(18, 168, 88),   // Dual Green 2  4
    NDS_RGB(28, 118, 55),   // Dark Green    5
    NDS_RGB(0, 69, 40),     // Darker Green  6
    NDS_RGB(4, 34, 18),     // Darkest Green 7
    NDS_RGB(121, 206, 255), // Light Blue    8
    NDS_RGB(9, 137, 253),   // Rich Blue     9
    NDS_RGB(0, 104, 208),   // Dark Blue     10
    NDS_RGB(0, 44, 208),    // Navy Blue     11
    NDS_RGB(0, 0, 36),      // Darkest Blue  12
    NDS_RGB(245, 198, 164), // Light Orange  13
    NDS_RGB(198, 164, 245), // Light Purple  14
    //Defaults
    ARGB16(1, 31, 0, 0),   // Red           15
    ARGB16(1, 0, 31, 0),   // Green         16
    ARGB16(1, 0, 0, 31),   // Blue          17
    ARGB16(1, 31, 31, 0),  // Yellow        18
    ARGB16(1, 31, 0, 31),  // Magenta       19
    ARGB16(1, 0, 31, 31),  // Cyan          20
    ARGB16(1, 15, 15, 15), // Gray          21
};

Font* TextManager::loadFont(std::string* name, int size)
{
    if (name == nullptr)
    {
        haltOnError("TextManager::loadFont(std::string* name, int size) : name cannot be nullptr");
    }

    Font* font = new Font();
    std::string fullPath = "fonts/" + *name + "/size-" + std::to_string(size);

    std::string fontBitmapPath = fullPath + ".img.bin";
    font->bitmap = loadFontBitmap(&fontBitmapPath);
    if (!font->bitmap)
    {
        haltOnError("Failed to load font bitmap from \n" + fullPath + ".img.bin");
    }

    std::string fontMetadataPath = fullPath + ".fnt";
    if (!loadFontMetadata(&fontMetadataPath, font))
    {
        haltOnError("Failed to load font metadata from \n" + fullPath + ".fnt");
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
        return;

    free(font->bitmap);
    free(font->bitmapBold);
    delete font;
}

void TextManager::loadDefaultPalette()
{
    dmaCopy(customPalette, BG_PALETTE, 256 * sizeof(uint16_t));
    dmaCopy(customPalette, BG_PALETTE_SUB, 256 * sizeof(uint16_t));
}

bool TextManager::loadPalette(std::string* path, bool sub)
{
    if (path == nullptr)
    {
        haltOnError("TextManager::loadPalette(std::string* path, bool sub) : path cannot be nullptr");
    }

    FileBuffer buffer = io.openFileBuffer(*path);
    if (buffer.get() == nullptr)
    {
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
        haltOnError("TextManager::loadFontBitmap(std::string* path) : path cannot be nullptr");
    }

    FileBuffer buffer = io.openFileBuffer(*path);
    if (buffer.get() == nullptr)
    {
        return nullptr;
    }

    return reinterpret_cast<std::uint8_t*>(buffer.release());
}

bool TextManager::loadFontMetadata(std::string* path, Font* font, bool forBoldBitmap)
{
    if (path == nullptr)
    {
        haltOnError("TextManager::loadFontMetadata(std::string* path, Font* font, bool forBoldBitmap) : path cannot be "
                    "nullptr");
    }

    FileBuffer buffer = io.openFileBuffer(*path);
    if (buffer.get() == nullptr)
    {
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
    std::size_t dataEnd = dataStart;
    while (dataEnd < line.size() && line[dataEnd] != ' ')
    {
        dataEnd++;
    }

    return std::stoi(line.substr(dataStart, dataEnd - dataStart));
}

void TextManager::haltOnError(const std::string& errorMessage)
{
    consoleDemoInit();
    printf("\n\n");
    printf("Error; %s", errorMessage.c_str());
    while (1)
    {
        swiWaitForVBlank();
    }
}
