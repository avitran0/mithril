#include "mithril/logging.hpp"

#include <fstream>
#include <iostream>

#define COLOR_RESET "\033[0m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_RED "\033[31m"

auto log_level = LogLevel::Info;
std::ofstream log_file {};

std::string logging::LevelName(const LogLevel level) {
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

void logging::Log(const LogLevel level, const std::string &message) {
    if (level < log_level) {
        return;
    }

    const std::string out = "[" + LevelName(level) + "] " + message + "\n";
    std::cout << LogLevelColor(level) << out << COLOR_RESET;
    if (log_file.good()) {
        log_file << out;
    }
}

void logging::SetLogFile(const std::string &file_name) {
    log_file = std::ofstream {file_name};
    if (!log_file.good()) {
        Log(LogLevel::Error, "could not open log file");
    }
}

void logging::SetLevel(const LogLevel level) { log_level = level; }

LogLevel logging::GetLevel() { return log_level; }
