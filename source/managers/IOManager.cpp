#include "IOManager.hpp"

extern std::string fatBasePath;
void IOManager::Init()
{
    basePath = fatBasePath;
}

// TODO: update to not use malloc/free
void* IOManager::loadToRAM(const std::string& filePath, u32* outSize)
{
    // open file
    std::string path = basePath + filePath;
    FILE* file = fopen(filePath.c_str(), "rb");
    if (!file)
    {
        if (outSize)
        {
            *outSize = 0;
        }
        return NULL;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    u32 size = ftell(file);
    // return to beginning of file
    rewind(file);

    if (size == 0)
    {
        fclose(file);
        if (outSize)
        {
            *outSize = 0;
        }
        return NULL;
    }

    // allocate buffer
    void* buffer = malloc(size);
    if (buffer)
    {
        fread(buffer, 1, size, file);
    }

    fclose(file);

    if (outSize)
    {
        *outSize = size;
    }

    return buffer;
}

// TODO: update to not use malloc/free
void IOManager::unloadFromRAM(void* buffer)
{
    // free memory
    if (buffer)
    {
        free(buffer);
    }
}

std::string IOManager::getAssetFilePath(const std::string& path, const char* suffix)
{
    std::string directPath = basePath + path + suffix;

    FILE* file = fopen(directPath.c_str(), "rb");
    if (file)
    {
        fclose(file);
        return directPath;
    }

    size_t end = path.find_last_not_of('/');
    if (end == std::string::npos)
    {
        return directPath;
    }

    size_t slash = path.find_last_of('/', end);
    std::string leaf =
        path.substr(slash == std::string::npos ? 0 : slash + 1, end - (slash == std::string::npos ? 0 : slash + 1) + 1);
    return path.substr(0, end + 1) + "/" + leaf + suffix;
}

// TODO: update to not use malloc/free
void* IOManager::openFile(const std::string& filePath)
{
    std::string path = basePath + filePath;
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
        return nullptr;

    //get file size
    fseek(file, 0, SEEK_END);
    u32 size = ftell(file);
    rewind(file);

    if (size == 0)
    {
        fclose(file);
        return nullptr;
    }

    void* buffer = malloc(size);
    if (buffer)
        fread(buffer, 1, size, file);
    fclose(file);

    return buffer;
}

// TODO: update to not use malloc/free
void* IOManager::openFile(const std::string& filePath, u32& size)
{
    std::string path = basePath + filePath;
    FILE* file = fopen(path.c_str(), "rb");
    if (!file)
        return nullptr;

    fseek(file, 0, SEEK_END);
    size = ftell(file);
    rewind(file);

    if (size == 0)
    {
        fclose(file);
        return nullptr;
    }

    void* buffer = malloc(size);
    if (buffer)
        fread(buffer, 1, size, file);
    fclose(file);

    return buffer;
}
