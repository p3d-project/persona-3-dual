#include "error.hpp"

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
