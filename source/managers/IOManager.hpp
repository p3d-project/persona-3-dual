/**
 * @file IOManager.hpp
 * @brief Manager for IO functions
 *
 * @author Taha Rashid (TheBossT910 / thebosst)
 * @author Gregory Munroo (ggmini)
 */

#pragma once
#include <aegis/manager.hpp>
#include <nds.h>

class IOManager : public ae::Manager, public ae::Singleton<IOManager>
{
  public:
    void Init() override;

    void Process() override
    {
    }

    void Shutdown() override
    {
    }

    /**
     * @brief Reads raw data of type T from a file into @p data.
     * @tparam T Trivially-copyable type to read into.
     * @param data Object to read into.
     * @param filePath Path relative to the base path.
     * @return true on success, false if the file could not be opened.
     */
    template <typename T> bool readFile(T* data, std::string filePath)
    {
        // open file
        std::string path = basePath + filePath;
        FILE* file = fopen(path.c_str(), "rb");
        if (!file)
        {
            return false;
        }

        // read data
        fread(data, sizeof(T), 1, file);

        // close file
        fclose(file);

        return true;
    }

    /**
     * @brief Writes raw data of type T to a file, atomically via a temp file.
     * @tparam T Trivially-copyable type to write.
     * @param data Object to write.
     * @param filePath Path relative to the base path.
     * @return true on success, false if the temp file could not be opened.
     */
    template <typename T> bool writeFile(T* data, std::string filePath)
    {
        // open file
        std::string tempPath = basePath + filePath + ".tmp";
        std::string path = basePath + filePath;

        FILE* file = fopen(tempPath.c_str(), "wb");
        if (!file)
        {
            return false;
        }

        // write data
        fwrite(data, sizeof(T), 1, file);

        // close file
        fclose(file);

        // replace old file with new file
        remove(path.c_str());
        rename(tempPath.c_str(), path.c_str());

        return true;
    }

    /**
     * @brief Loads an entire file into a buffer.
     * @param filePath Path relative to the base path.
     * @param outSize Optional. Receives the loaded size in bytes (0 on failure).
     * @return Pointer to the loaded data, or nullptr on failure.
     */
    void* loadToRAM(const std::string& filePath, u32* outSize);

    /**
     * @brief Releases a buffer previously returned by loadToRAM().
     * @param buffer Pointer to release. Safe to call with nullptr.
     */
    void unloadFromRAM(void* buffer);

    /**
     * @brief Open a file and return a pointer to its contents.
     * @param path The path to the file to open.
     * @return Pointer to the contents of the file, or nullptr if opening failed.
     */
    void* openFile(const std::string& path);

    /**
     * @brief Open a file and return a pointer to its contents, along with the size of the file.
     * @param path The path to the file to open.
     * @param size Reference to a variable to store the size of the file.
     * @return Pointer to the contents of the file, or nullptr if opening failed.
     * @note This function is useful when you need to know the size of the file being opened.
     */
    void* openFile(const std::string& path, u32& size);

    /**
     * @brief Resolves the on-disk path for an asset, allowing it to be stored either
     * as a flat file or grouped in its own subdirectory.
     *
     * Tries `basePath + path + suffix` first. If that file doesn't exist, falls back
     * to treating @p path as a directory and looking for `<path>/<leaf>` + suffix,
     * where `<leaf>` is the last path component (e.g. "textures/rock" ->
     * "textures/rock/rock" + suffix). This lets related asset files (e.g. multiple
     * suffixes for one logical asset) be grouped in a subfolder when needed.
     *
     * @note The fallback path is returned unconditionally, without checking it
     * actually exists.
     *
     * @param path   Relative asset path, e.g. "textures/rock".
     * @param suffix Suffix/extension to append, e.g. ".img.bin".
     * @return std::string Resolved file path.
     */
    std::string getAssetFilePath(const std::string& path, const char* suffix);

  private:
    friend class Singleton<IOManager>;
    IOManager() = default;

    std::string basePath;
};
