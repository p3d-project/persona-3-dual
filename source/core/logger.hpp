/**
 * @file logger.hpp
 * @brief Util to handle error logging
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include <cstdio>
#include <nds.h>
#include <source_location>
#include <string_view>

enum class LogSeverity
{
    DEBUG,
    WARNING,
    ERROR,
    FATAL,
};

/**
 * @brief Logs a diagnostic message and halts only for fatal messages
 *
 * @warning Fatal severity will freeze the program when called
 *
 * @param severity the log severity level
 * @param message the message to print
 * @param location the calling code's location, captured automatically; don't pass it
 */
void logMessage(LogSeverity severity,
                std::string_view message,
                std::source_location location = std::source_location::current());

/**
 * @brief Clears the log file for a fresh game session
 */
void clearLog();
