#include "mithril/stacktrace.hpp"

#include <cxxabi.h>
#include <dlfcn.h>
#include <elf.h>
#include <execinfo.h>

#include <csignal>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <ios>
#include <iostream>

#include "mithril/logging.hpp"

i32 skipped_frames = 0;

struct StackFrame {
    void *address;
    std::string function;
    std::string filename;
};

void stacktrace::SkipFrames(const i32 frames) { skipped_frames = frames; }

std::string SignalName(const i32 signal) {
    switch (signal) {
        case SIGILL:
            return "illegal instruction";
        case SIGABRT:
            return "abnormal termination";
        case SIGFPE:
            return "floating point error";
        case SIGSEGV:
            return "segmentation fault";
        default:
            return "?";
    }
}

void stacktrace::SignalHandler(const i32 signal) {
    logging::Log(LogLevel::Info, "received signal: " + SignalName(signal));
    const std::vector<std::string> frames = stacktrace::Stacktrace();
    logging::Log(LogLevel::Info, "found " + std::to_string(frames.size()) + " stack frames");
    for (const std::string &frame : frames) {
        std::cout << frame + "\n";
    }
    std::exit(EXIT_FAILURE);
}

std::vector<std::string> stacktrace::Stacktrace() {
    std::vector<void *> addresses(64 + skipped_frames);
    const i32 num_frames = backtrace(addresses.data(), 64 + skipped_frames);
    addresses.resize(num_frames);

    if (skipped_frames >= num_frames) {
        exit(EXIT_FAILURE);
    }

    addresses.erase(addresses.begin(), addresses.begin() + skipped_frames);

    const std::string process_exe = std::filesystem::read_symlink("/proc/self/exe");
    std::vector<std::string> frames;
    for (void *address : addresses) {
        std::stringstream command;
        command << "addr2line -e " << process_exe << " -fpC 0x" << std::hex
                << reinterpret_cast<uintptr_t>(address) - 1;
        FILE *process = popen(command.str().c_str(), "r");
        if (!process) {
            logging::Log(LogLevel::Error, "could not invoke addr2line");
        }

        char buffer[256];
        if (fgets(buffer, sizeof(buffer), process) != nullptr) {
            // remove trailing newline
            std::string frame {buffer};
            if (!frame.empty() && frame.back() == '\n') {
                frame.pop_back();
            }
            frames.push_back(frame);
        } else {
            frames.emplace_back("???");
        }

        pclose(process);
    }

    for (size_t i = 0; i < frames.size(); i++) {
        frames[i] = "#" + std::to_string(i) + " " + frames[i];
    }
    return frames;
}
