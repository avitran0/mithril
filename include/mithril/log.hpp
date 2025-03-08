#pragma once

#include <string>

enum class LogLevel {
    Debug,
    Info,
    Warning,
    Error,
    Off,
};

namespace log {
    void Log(LogLevel level, const std::string &message);

    void SetLevel(LogLevel level);
    LogLevel GetLevel(void);
}  // namespace log
