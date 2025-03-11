#include "mithril/logging.hpp"

#include <fstream>

logging::Level log_level = logging::Level::Info;
std::ofstream log_file {};

std::string logging::LevelName(const Level level) {
    switch (level) {
        case Level::Debug:
            return "Debug";
        case Level::Info:
            return "Info";
        case Level::Warning:
            return "Warning";
        case Level::Error:
            return "Error";
        case Level::Off:
            return "?";
    }
    return "?";
}

void logging::SetLogFile(const std::string &file_name) {
    log_file = std::ofstream {file_name};
    if (!log_file.good()) {
        Error("could not open log file");
    }
}

void logging::SetLevel(const Level level) { log_level = level; }

logging::Level logging::GetLevel() { return log_level; }

void logging::LogImpl(std::ostringstream &oss, const std::string &format, std::size_t pos) {
    oss << format.substr(pos);
}

std::size_t logging::CountPlaceholders(const std::string &fmt) {
    std::size_t count = 0;
    for (std::size_t i = 0; i < fmt.size() - 1; ++i) {
        if (fmt[i] == '{' && fmt[i + 1] == '}') {
            count++;
            i++;
        }
    }
    return count;
}
