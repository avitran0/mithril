#pragma once

#include <string>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Off,
};

namespace logging {
    void Log(LogLevel level, const std::string &message);
    void SetLogFile(const std::string &file_name);

    void SetLevel(LogLevel level);
    LogLevel GetLevel();

    std::string LevelName(LogLevel level);
}  // namespace log
