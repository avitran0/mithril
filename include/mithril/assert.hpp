#pragma once

#include <string>
#include <type_traits>

#include "mithril/log.hpp"
#include "mithril/types.hpp"

namespace source_location {
    struct source_location {
        const char *file;
        const char *function;
        u32 line;

        constexpr source_location(const char *file, const char *function, u32 line) noexcept
            : file(file), function(function), line(line) {}

        constexpr source_location() noexcept : file(""), function(""), line(0) {}

        std::string to_string() {
            return std::string(file) + ":" + std::to_string(line) + " (" + function + ")";
        }
    };

#define CURRENT_LOCATION \
    source_location::source_location(__builtin_FILE(), __builtin_FUNCTION(), __builtin_LINE())

}  // namespace source_location

namespace assert {
    template <typename, typename = void>
    struct has_to_string : std::false_type {};

    template <typename T>
    struct has_to_string<T, std::void_t<decltype(std::to_string(std::declval<T>()))>>
        : std::true_type {};

    template <typename, typename = void>
    struct has_equal : std::false_type {};

    template <typename T>
    struct has_equal<T, std::void_t<decltype(std::declval<T>() == std::declval<T>())>>
        : std::integral_constant<
              bool, std::is_convertible_v<decltype(std::declval<T>() == std::declval<T>()), bool>> {
    };

    template <
        typename T, typename = std::enable_if_t<has_to_string<T>::value>,
        typename = std::enable_if_t<has_equal<T>::value>>
    void AssertEqual(T left, T right, source_location::source_location loc = CURRENT_LOCATION) {
        if (left != right) {
            log::Log(
                LogLevel::Error,
                std::to_string(left) + " != " + std::to_string(right) + " at " + loc.to_string());
        }
    }

    template <
        typename T, typename = std::enable_if_t<has_to_string<T>::value>,
        typename = std::enable_if_t<has_equal<T>::value>>
    void AssertNotEqual(T left, T right, source_location::source_location log = CURRENT_LOCATION) {
        if (left == right) {
            log::Log(
                LogLevel::Error,
                std::to_string(left) + " == " + std::to_string(right) + " at " + log.to_string());
        }
    }
}  // namespace assert
