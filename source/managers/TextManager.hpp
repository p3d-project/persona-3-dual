/**
 * @file TextManager.hpp
 * @brief Manager for text loading.
 * @author Gregory Munroo (ggmini)
 */

#pragma once
#include "core/structs.h"
#include <aegis/manager.hpp>

#include "managers/IOManager.hpp"

class TextManager : public ae::Manager, public ae::Singleton<TextManager>
{
  public:
    void Init() override
    {
    }

    void Process() override
    {
    }

    void Shutdown() override
    {
    }

    /**
     * @brief Load a font from a file.
     * @param name Name of the font.
     * @param size Size of the font.
     * @return Pointer to the loaded font, or nullptr if loading failed.
     * @note This function assumes that the font files are located in the "fonts" directory and follow the naming convention "<name>/size-<size>".
     * @warning This function will halt the program if the regular font fails to load. If the bold font fails to load, it will simply set the boldLoaded flag to false and continue.
     */
    Font* loadFont(std::string* name, int size);

    /**
     * @brief Loads the predefined default palette.
     */
    void loadDefaultPalette();

    /**
     * @brief Load a palette from a file.
     * @param paletteFilePath The path to the palette file.
     * @param sub Whether to load the palette for the sub screen.
     * @return true if the palette was loaded successfully, false otherwise.
     * @note This palette will apply for all text that is being drawn.
     */
    bool loadPalette(std::string* paletteFilePath, bool sub = false);

    /**
     * @brief Unload the current palettes.
     * @note This function unloads the palettes from both the main and sub screens.
     */
    void unloadPalette();

    /**
     * @brief Load the font bitmap from a file.
     * @param path The path to the font bitmap file.
     * @return Pointer to the loaded font bitmap, or nullptr if loading failed.
     */
    std::uint8_t* loadFontBitmap(std::string* path);

    /**
     * @brief Load the font metadata from a file.
     * @param path The path to the font metadata file.
     * @param font Pointer to the font to populate with metadata.
     * @param forBoldBitmap Whether the metadata being loaded is for the bold version of the font.
     * @return true if loading was successful, false otherwise.
     */
    bool loadFontMetadata(std::string* path, Font* font, bool forBoldBitmap = false);

    /**
     * @brief Clear the text layer by filling the video buffer with black.
     * @param videoBuffer Pointer to the video buffer to clear.
     */
    void clearScreen(uint16_t* videoBuffer);

  private:
    friend class Singleton<TextManager>;
    TextManager() = default;

    IOManager& io = IOManager::GetInstance();

    /**
     * @brief Extract an integer value from a line of text based on a specified key.
     * @param line The line of text to extract the value from.
     * @param key The key that preceedes the integer value in the line.
     * @return The extracted integer value, or 0 if the key is not found.
     * @note This function assumes that the line takes the form "key=value ".
     */
    int extractIntValue(const std::string& line, const std::string& key);

    /**
     * @brief Halts the program and displays an error message.
     *
     * @param errorMessage The error message to be displayed.
     * @note This function will enter an infinite loop after displaying the error message, effectively halting the program.
     * It is intended for use in critical error situations where continuing execution could lead to undefined behavior or further errors.
     */
    void haltOnError(const std::string& errorMessage);
};
