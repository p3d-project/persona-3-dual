#include "logger.hpp"

#include "core/globals.hpp"
#include <cstdint>
#include <string>

namespace
{
struct LogStats
{
    uint32_t total = 0;
    uint32_t debug = 0;
    uint32_t warning = 0;
    uint32_t error = 0;
    uint32_t fatal = 0;
};

LogStats stats;
bool isLogSessionStarted = false;

/**
 * @brief Get the string name of the LogSeverity
 *
 * @param severity LogSeverity
 * @return const char* the string name of the LogSeverity
 */
const char* severityName(LogSeverity severity)
{
    const char* severityStr = "UNKNOWN";
    switch (severity)
    {
    case LogSeverity::DEBUG:
    {
        severityStr = "DEBUG";
        break;
    }

    case LogSeverity::WARNING:
    {
        severityStr = "WARNING";
        break;
    }

    case LogSeverity::ERROR:
    {
        severityStr = "ERROR";
        break;
    }

    case LogSeverity::FATAL:
    {
        severityStr = "FATAL";
        break;
    }
    }

    return severityStr;
}

/**
 * @brief Helper to increment the number of LogSeverity stats
 *
 * @param severity LogSeverity
 */
void incrementStats(LogSeverity severity)
{
    stats.total++;
    switch (severity)
    {
    case LogSeverity::DEBUG:
    {
        stats.debug++;
        break;
    }

    case LogSeverity::WARNING:
    {
        stats.warning++;
        break;
    }

    case LogSeverity::ERROR:
    {
        stats.error++;
        break;
    }

    case LogSeverity::FATAL:
    {
        stats.fatal++;
        break;
    }
    }
}
} // namespace

void clearLog()
{
    const std::string logPath = fatBasePath.empty() ? "fat:/data/log.txt" : (fatBasePath + "log.txt");
    FILE* logFile = std::fopen(logPath.c_str(), "w");
    if (logFile != nullptr)
    {
        std::fflush(logFile);
        std::fclose(logFile);
    }
    stats = LogStats{};
    isLogSessionStarted = true;
}

void logMessageImpl(LogSeverity severity, std::string_view message, const char* file, int line, const char* func)
{
    incrementStats(severity);
    const char* name = severityName(severity);

    if (severity == LogSeverity::DEBUG)
    {
        /// print additional info to stderr
        std::fprintf(stderr,
                     "frame=%ld stats(total=%lu debug=%lu warning=%lu error=%lu fatal=%lu)\n"
                     "[%s] file: %s(%d) `%s`: %.*s\n",
                     static_cast<long>(frame),
                     static_cast<unsigned long>(stats.total),
                     static_cast<unsigned long>(stats.debug),
                     static_cast<unsigned long>(stats.warning),
                     static_cast<unsigned long>(stats.error),
                     static_cast<unsigned long>(stats.fatal),
                     name,
                     file,
                     line,
                     func,
                     static_cast<int>(message.size()),
                     message.data());
    }
    else if (severity != LogSeverity::FATAL)
    {
        /// log to stderr
        std::fprintf(stderr,
                     "[%s] file: %s(%d) `%s`: %.*s\n",
                     name,
                     file,
                     line,
                     func,
                     static_cast<int>(message.size()),
                     message.data());
    }

    /// log to file
    const std::string logPath = fatBasePath + "log.txt";
    const char* openMode = isLogSessionStarted ? "a" : "w";
    FILE* logFile = std::fopen(logPath.c_str(), openMode);

    if (logFile != nullptr)
    {
        isLogSessionStarted = true;
        std::fprintf(logFile,
                     "frame=%ld stats(total=%lu debug=%lu warning=%lu error=%lu fatal=%lu)\n"
                     "[%s] file: %s(%d) `%s`: %.*s\n",
                     static_cast<long>(frame),
                     static_cast<unsigned long>(stats.total),
                     static_cast<unsigned long>(stats.debug),
                     static_cast<unsigned long>(stats.warning),
                     static_cast<unsigned long>(stats.error),
                     static_cast<unsigned long>(stats.fatal),
                     name,
                     file,
                     line,
                     func,
                     static_cast<int>(message.size()),
                     message.data());
        std::fflush(logFile);
        std::fclose(logFile);
    }

    if (severity == LogSeverity::FATAL)
    {
        /// print additional info to stderr & freeze program
        consoleDemoInit();
        std::fprintf(stderr,
                     "frame=%ld stats(total=%lu debug=%lu warning=%lu error=%lu fatal=%lu)\n"
                     "[%s] file: %s(%d) `%s`: %.*s\n",
                     static_cast<long>(frame),
                     static_cast<unsigned long>(stats.total),
                     static_cast<unsigned long>(stats.debug),
                     static_cast<unsigned long>(stats.warning),
                     static_cast<unsigned long>(stats.error),
                     static_cast<unsigned long>(stats.fatal),
                     name,
                     file,
                     line,
                     func,
                     static_cast<int>(message.size()),
                     message.data());
        while (1)
        {
            swiWaitForVBlank();
        }
    }
}
