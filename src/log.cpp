#include "mithril/log.hpp"

#include <iostream>

#define COLOR_RESET "\033[0m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED "\033[31m"

LogLevel log_level = LogLevel::Info;

std::string LogLevelName(LogLevel level) {
    switch (level) {
        case LogLevel::Debug:
            return "Debug";
        case LogLevel::Info:
            return "Info";
        case LogLevel::Warning:
            return COLOR_YELLOW "Warning";
        case LogLevel::Error:
            return COLOR_RED "Error";
        case LogLevel::Off:
            return "?";
    }
}

void log::Log(LogLevel level, const std::string &message) {
    if (level > log_level) {
        return;
    }

    const std::string out = "[" + LogLevelName(level) + "] " + message + "\n";
    std::cout << out;
}

void log::SetLevel(const LogLevel level) { log_level = level; }

LogLevel log::GetLevel(void) { return log_level; }
