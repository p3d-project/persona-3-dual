/**
 * @file error.hpp
 * @brief Util to handle error logging
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#include <cstdio>
#include <nds.h>
#include <string_view>

/**
 * @brief Logs message to the screen
 *
 * @warning This function will freeze the program when called
 *
 * @note This codebase currently targets C++17. While C++20 introduces
 *       std::source_location to replace macros, this implementation is
 *       custom-built and adapted from the example found on
 *       https://en.cppreference.com/cpp/utility/source_location
 *
 * @param message the message to print
 * @param file the file of the calling code
 * @param line the line of the calling code
 * @param column the column of the calling code
 * @param func the function signature of the calling code
 */
void logMessageImpl(std::string_view message, const char* file, int line, const char* func)
{
    consoleDemoInit();
    std::fprintf(
        stderr, "file: %s(%d) `%s`: %.*s\n", file, line, func, static_cast<int>(message.size()), message.data());
    while (1)
    {
        swiWaitForVBlank();
    }
}

#define logMessage(message) logMessageImpl(message, __FILE__, __LINE__, __func__)
