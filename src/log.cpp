#include "mithril/log.hpp"

#include <iostream>

#define COLOR_RESET "\033[0m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED "\033[31m"

auto log_level = LogLevel::Info;

std::string LogLevelName(const LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return "Debug";
        case LogLevel::Info:
            return "Info";
        case LogLevel::Warning:
            return "Warning";
        case LogLevel::Error:
            return "Error";
        case LogLevel::Off:
            return "?";
    }
    return "?";
}

std::string LogLevelColor(const LogLevel level) {
    switch (level) {
        case LogLevel::Error:
            return COLOR_RED;
        case LogLevel::Warning:
            return COLOR_YELLOW;
        default:
            return COLOR_RESET;
    }
}

void log::Log(const LogLevel level, const std::string &message) {
    if (level < log_level) {
        return;
    }

    const std::string out =
        LogLevelColor(level) + "[" + LogLevelName(level) + "] " + message + "\n" COLOR_RESET;
    std::cout << out;
}

void log::SetLevel(const LogLevel level) { log_level = level; }

LogLevel log::GetLevel() { return log_level; }
