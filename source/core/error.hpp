/**
 * @file error.hpp
 * @brief Util to handle error logging
 * @author Taha Rashid (TheBossT910 / thebosst)
 */

#pragma once
#include <cstdio>
#include <nds.h>
#include <string_view>

enum class LogSeverity
{
    DEBUG,
    WARNING,
    ERROR,
    FATAL,
};

/* @brief Logs a diagnostic message and halts only for fatal messages
 *
 * @warning This function will freeze the program when called
 *
 * @note This codebase currently targets C++17. While C++20 introduces
 *       std::source_location to replace macros, this implementation is
 *       custom-built and adapted from the example found on
 *       https://en.cppreference.com/cpp/utility/source_location
 * @brief
 *
 * @param message the message to print
 * @param file the file of the calling code
 * @param line the line of the calling code
 * @param column the column of the calling code
 * @param func the function signature of the calling code
**/
void logMessageImpl(LogSeverity severity, std::string_view message, const char* file, int line, const char* func);

/**
 * @brief Clears the log file for a fresh game session
 */
void clearLog();

#define logMessage(severity, message) logMessageImpl(severity, message, __FILE__, __LINE__, __func__)
