/**
 * @file TextComponent.hpp
 * @brief Orchestrates rendering text from a bitmap.
 *
 * @author Gregory Munroo (ggmini)
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include "managers/TextManager.hpp"
#include "systems/TextSystem.hpp"
#include "types/TextTypes.hpp"
#include "types/aeTypes.hpp"

#include <aegis/component.hpp>

class TextComponent : public ae::Component
{
  public:
    static constexpr ae::ComponentTypeID TYPE_ID = static_cast<ae::ComponentTypeID>(ComponentType::Text);
    void Init() override
    {
    }

    /**
     * @brief Sets isActive to false on component destruction
     */
    void Destroy() override;

    void Update(ae::q20_12_t /*dt*/) override;

    ae::ComponentTypeID GetType() const override
    {
        return TYPE_ID;
    }

    /**
     * @brief Configure the text system
     *
     * Required to call before calling start(). Sets the font, font bitmap,
     * font palette, and font metadata information
     *
     * @param config The struct containing the text configuration to apply.
     * @param loadDefaultPalette Flag to load the default font palette if true
     */
    void configureText(const TextConfig& config, bool loadDefaultPalette = true);

    /**
     * @brief A wrapper for drawText in TextSystem. Draws text to the screen.
     * @param text The text to draw.
     * @param x The x-coordinate to start drawing the text.
     * @param y The y-coordinate to start drawing the text.
     * @param color The color to use for the text (default is white).
     * @see drawText
     */
    void drawText(const std::string& text, int x, int y, int color = TextColor::White);

    /**
     * @brief A wrapper for appearText in TextSystem. Simulates text typing effect.
     * @param text The text to render.
     * @param x The x-coordinate to start drawing the text.
     * @param y The y-coordinate to start drawing the text.
     * @param color The color to use for the text (default is white).
     * @see appearText
     */
    void appearText(const std::string& text, int x, int y, int color = TextColor::White);

    /**
     * @brief A wrapper for appearTextSkip in TextSystem. If a text is currently being rendered with appearText, this function will immediately render the rest of the text without delay.
     * @see appearTextSkip
     */
    void appearTextSkip();

    /**
     * @brief Stops the appearText animation immediately and clears the state.
     */
    void appearTextStop();

    /**
     * @brief A wrapper for appearTextDone in TextSystem. Check if the text being rendered with appearText has finished appearing.
     * @return true if the text has finished appearing, false otherwise.
     * @see appearTextDone
     */
    bool appearTextDone();

    /**
     * @brief A wrapper for drawGlyph in TextSystem. Draw a single glyph to the screen.
     * @param glyph The glyph to draw.
     * @param x The x-coordinate to start drawing the glyph.
     * @param y The y-coordinate to start drawing the glyph.
     * @param color The color to use for the glyph.
     * @param bold Whether to use the bold version of the bitmap.
     * @param italic Whether the glyph should be sheared to simulate italic text.
     * @param underline Whether the glyph should be underlined.
     * @see drawGlyph
     */
    void drawGlyph(
        const Glyph& glyph, int x, int y, int color, bool bold = false, bool italic = false, bool underline = false);

    /**
      * @brief A wrapper for clearArea in TextSystem. Clear a rectangular area of the text video buffer.
      * @param x The x-coordinate of the top-left corner of the area to clear.
      * @param y The y-coordinate of the top-left corner of the area to clear.
      * @param width The width of the area to clear.
      * @param height The height of the area to clear.
      * @see clearArea
      */
    void clearArea(int x, int y, int width, int height);

    /**
     * @brief A wrapper for clearScreen in TextManager. Clear the text layer by filling the video buffer with black.
     * @see clearScreen
     */
    void clearScreen();

    /**
     * @brief A getter to return the font size
     * @return The font size
     */
    int getFontSize();

    /**
     * @brief A getter to return the line spacing
     * @return The line spacing
     */
    int getLineSpacing();

    /**
     * @brief A getter to return the letter spacing
     * @return The letter spacing
     */
    int getLetterSpacing();

    /**
     * @brief A getter to return the space width (width of a ' ' character)
     * @return The space width
     */
    int getSpaceWidth();

    /**
     * @brief Manually override the auto-calculated letter spacing.
     * @param value Pixels of horizontal space to add after each glyph.
     */
    void setLetterSpacing(int value);

    /**
     * @brief Manually override the auto-calculated line spacing.
     * @param value Pixels of vertical space to add after each line, on top of the font's line height.
     */
    void setLineSpacing(int value);

    /**
     * @brief Manually override the auto-calculated space width.
     * @param value Pixels of horizontal space a ' ' character takes up.
     */
    void setSpaceWidth(int value);

  protected:
    void SubmitToManager() override
    {
    }

  private:
    TextSystem& ts = TextSystem::GetInstance();
    TextManager& tm = TextManager::GetInstance();

    Text* appearingText = nullptr;
    Font* font = nullptr;
    uint16_t* videoBuffer = nullptr;
    int fontSize = 0;

    /// Auto-derived from fontSize in configureText(); can be overridden manually via the setters above.
    int letterSpacing = 1;
    int lineSpacing = 2;
    int spaceWidth = 2;

    /**
     * @brief A wrapper for testBitmap in TextSystem
     * @see testBitmap
     */
    void testBitmap();

    /**
     * @brief A wrapper for testPalette in TextSystem
     * @see testPalette
     */
    void testPalette();
};
