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
#include <cstring>
#include <filesystem>
#include <fstream>
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
    logging::Info("received signal: {}", SignalName(signal));
    const std::vector<std::string> frames = stacktrace::Stacktrace();
    logging::Info("found {} stack frames", frames.size());
    for (const std::string &frame : frames) {
        std::cout << frame + "\n";
    }
    std::exit(EXIT_FAILURE);
}

std::string addr2line(const std::string &program_name, void *address);
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
    //logging::Info("{}", addr2line(process_exe, addresses[0]));
    for (void *address : addresses) {
        std::stringstream command;
        command << "addr2line -e " << process_exe << " -fpC 0x" << std::hex
                << reinterpret_cast<uintptr_t>(address) - 1;
        FILE *process = popen(command.str().c_str(), "r");
        if (!process) {
            logging::Error("could not invoke addr2line");
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

// sections: .debug_info (most dies), .debug_abbrev, .debug_line, .debug_addr
std::string addr2line(const std::string &program_name, void *address) {
    // Open the ELF file in binary mode.
    std::ifstream elf {program_name, std::ios::binary};
    if (!elf.good()) {
        logging::Error("error opening file {}", program_name);
        return "";
    }

    // Read the ELF header.
    Elf64_Ehdr ehdr;
    elf.read(reinterpret_cast<char *>(&ehdr), sizeof(ehdr));
    if (std::memcmp(ehdr.e_ident, ELFMAG, SELFMAG) != 0) {
        logging::Error("not an elf file");
        return "";
    }

    // Seek to the section header table.
    elf.seekg(ehdr.e_shoff, std::ios::beg);
    Elf64_Shdr *sh_table = new Elf64_Shdr[ehdr.e_shnum];
    elf.read(reinterpret_cast<char *>(sh_table), ehdr.e_shnum * sizeof(Elf64_Shdr));

    // Locate the section header string table.
    Elf64_Shdr sh_str = sh_table[ehdr.e_shstrndx];
    char *sh_strtab = new char[sh_str.sh_size];
    elf.seekg(sh_str.sh_offset, std::ios::beg);
    elf.read(sh_strtab, sh_str.sh_size);

    // Iterate over the section headers and print section names.
    for (int i = 0; i < ehdr.e_shnum; i++) {
        const std::string sec_name = sh_strtab + sh_table[i].sh_name;
        logging::Info("Section {}: {}", i, sec_name);
    }

    // Clean up.
    delete[] sh_strtab;
    delete[] sh_table;
    elf.close();

    return "";
}
