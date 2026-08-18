/**
 * @file TextSystem.hpp
 * @brief Manages rendering text from a bitmap.
 * @author Gregory Munroo (ggmini)
 */

#pragma once

#include "core/routerIDs.hpp"
#include "core/structs.h"
#include <aegis/system.hpp>

class TextSystem : public ae::System, public ae::Singleton<TextSystem>
{
  public:
    void Init() override
    {
    }

    void Shutdown() override
    {
    }

    void Update(ae::fixed_t /*dt*/) override
    {
    }

    /**
     * @brief Draw text to the screen.
     * @param text The text to draw.
     * @param font Pointer to the font to use for rendering.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @param x The x-coordinate to start drawing the text.
     * @param y The y-coordinate to start drawing the text.
     * @param color The color to use for the text.
     */
    void drawText(const std::string& text, Font* font, uint16_t* videoBuffer, int x, int y, int color);

    /**
     * @brief Create a Text object and renders each character with a delay to simulate typing effect.
     * @param appearingText The current apperance of the text
     * @param text The text to render.
     * @param font Pointer to the font to use for rendering.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @param x The x-coordinate to start drawing the text.
     * @param y The y-coordinate to start drawing the text.
     * @param color The color to use for the text.
     */
    void appearText(
        Text*& appearingText, const std::string& text, Font* font, uint16_t* videoBuffer, int x, int y, int color);

    /**
     * @brief If a text is currently being rendered with appearText, this function will immediately render the rest of the text without delay.
     * @param appearingText The current apperance of the text
     */
    void appearTextSkip(Text*& appearingText);

    /**
     * @brief Check if the text being rendered with appearText has finished appearing.
     * @param appearingText The current apperance of the text
     * @return true if the text has finished appearing, false otherwise.
     */
    bool appearTextDone(Text*& appearingText);

    /**
     * @brief Draw a single glyph to the screen.
     * @param glyph The glyph to draw.
     * @param font Pointer to the font that contains the glyph.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @param x The x-coordinate to start drawing the glyph.
     * @param y The y-coordinate to start drawing the glyph.
     * @param color The color to use for the glyph.
     * @param bold Whether to use the bold version of the bitmap.
     * @param italic Whether the glyph should be sheared to simulate italic text.
     * @param underline Whether the glyph should be underlined.
     */
    void drawGlyph(const Glyph& glyph,
                   Font* font,
                   uint16_t* videoBuffer,
                   int x,
                   int y,
                   int color,
                   bool bold,
                   bool italic,
                   bool underline);

    /**
      * @brief Clear a rectangular area of the text video buffer.
      * @param videoBuffer Pointer to the video buffer to clear.
      * @param x The x-coordinate of the top-left corner of the area to clear.
      * @param y The y-coordinate of the top-left corner of the area to clear.
      * @param width The width of the area to clear.
      * @param height The height of the area to clear.
      */
    void clearArea(uint16_t* videoBuffer, int x, int y, int width, int height);

  private:
    friend class Singleton<TextSystem>;
    friend class TextComponent;

    int APPEAR_DELAY = 2;
    int LETTER_SPACING = 1;
    int LINE_SPACING = 2;
    int SPACE_WIDTH = 2;
    /// 128 = 1 pixel shift every 2 rows, 64 = 1 pixel shift every 4 rows
    int SLANT_FACTOR = 64;

    /**
     * @brief Draw the next character from the text struct to the screen.
     * @note Used to draw text which appears character by character.
     */
    void drawNextFromText(Text*& text);

    /**
     * @brief Create a Text object and initialize its properties.
     * @param text The text content for the Text object.
     * @param font Pointer to the font to use for rendering the text.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @param startX The x-coordinate to start drawing the text.
     * @param startY The y-coordinate to start drawing the text.
     * @param color The color to use for the text.
     * @return Pointer to the newly created Text object.
     */
    Text* createText(const std::string& text, Font* font, uint16_t* videoBuffer, int startX, int startY, int color);

    /**
     * @brief Get the next character from a given Text object.
     * @param text Pointer to the Text object to extract the next character from.
     * @return The next character in the Text object's content, or a nullptr if there are no more characters.
     * @note This function automatically increments the cursor position in the Text object.
     */
    char getNextChar(Text* text);

    /**
     * @brief Draw a single pixel to the video buffer.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @param x The x-coordinate of the pixel to draw.
     * @param y The y-coordinate of the pixel to draw.
     * @param paletteValue The color index in the palette to use for the pixel.
     */
    void drawPixel(uint16_t* videoBuffer, int x, int y, int paletteValue);

    /**
     * @brief Get the next word from a given text string.
     * @param text The text string to extract the next word from.
     * @return The next word in the text string, or an empty string if there are no more words.
     */
    std::string getNextWord(const std::string& text);

    /**
     * @brief Check if a given text string will exceed the screen width when rendered with the specified font.
     * @param text The text string to check.
     * @param font Pointer to the font to use for rendering.
     * @param startX The starting x-coordinate for rendering the text.
     * @param bold Whether the bitmap is using bold text
     * @return true if the text will exceed the screen width, false otherwise.
     */
    bool checkWordWrap(const std::string& text, Font* font, int startX, bool bold);

    /**
     * @brief Underline a specificed area.
     * @param startX The starting x-coordinate of the area to underline.
     * @param y The y-coordinate of the area to underline.
     * @param width The width of the area to underline.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @param color The color to use for the underline.
     * @details This function draws a horizontal line of the specified width at the specified y-coordinate, starting from the specified x-coordinate.
     * It should be used to underline gaps in the text, that are not handled by the regular text rendering, i.e. the gaps between letters or spaces.
     */
    void underlineGap(int startX, int y, int width, uint16_t* videoBuffer, int color);

    /**
     * @brief Test function to draw the font bitmap to the screen.
     * @param font Pointer to the font to test.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @note This function is intended for testing purposes.
     */
    void testBitmap(Font* font, uint16_t* videoBuffer);

    /**
     * @brief Test function to draw the currently loaded palette to the screen.
     * @param videoBuffer Pointer to the video buffer to draw to.
     * @note This function is intended for testing purposes.
     */
    void testPalette(uint16_t* videoBuffer);
    // ---
};
