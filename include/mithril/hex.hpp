#pragma once

#include <sstream>
#include <string>

namespace hex {
    template <typename T>
    std::string HexString(T value) {
        static_assert(std::is_integral<T>::value, "T must be an integral type");
        std::stringstream ss;
        ss << std::hex << "0x" << value;
        return ss.str();
    }
}  // namespace hex
