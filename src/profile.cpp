#include "mithril/profile.hpp"

#include <chrono>
#include <vector>

#include "mithril/logging.hpp"
#include "mithril/types.hpp"

struct Profile {
    std::chrono::steady_clock::time_point time;
    std::string name {};
};

std::vector<Profile> profiles {64};

void profile::Start(const std::string &name) {
    for (const auto &profile : profiles) {
        if (profile.name == name) {
            logging::Log(LogLevel::Error, "profile with name " + name + " is already running");
            return;
        }
    }

    Profile profile;
    profile.name = name;
    profile.time = std::chrono::steady_clock::now();
    profiles.push_back(profile);
}

void profile::Stop(const std::string &name) {
    if (profiles.empty()) {
        logging::Log(LogLevel::Error, "no profiling in progress");
        return;
    }

    for (u64 i = 0; i < profiles.size(); i++) {
        const Profile &profile = profiles[i];
        if (profile.name == name) {
            const auto now = std::chrono::steady_clock::now();
            const auto elapsed = now - profile.time;

            const u64 us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            if (us < 1000) {
                logging::Log(LogLevel::Info, name + " took " + std::to_string(us) + " µs");
            } else if (us < 1000000) {
                const u64 ms =
                    std::chrono::duration_cast<std::chrono::milliseconds>(elapsed).count();
                logging::Log(LogLevel::Info, name + " took " + std::to_string(ms) + " ms");
            } else {
                const u64 sec = std::chrono::duration_cast<std::chrono::seconds>(elapsed).count();
                logging::Log(LogLevel::Info, name + " took " + std::to_string(sec) + " sec");
            }

            profiles.erase(profiles.begin() + i);

            return;
        }
    }

    logging::Log(LogLevel::Error, "profile " + name + " not found");
}
