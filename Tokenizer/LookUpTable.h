
#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H
#include <array>
#include <string_view>

#endif //LOOKUPTABLE_H

namespace lut {
    static constexpr auto MakeTable(const std::string_view acceptedCharacters) {
        std::array<bool, 256> lut{0};
        for (const auto s : acceptedCharacters) {
            lut.at(uint8_t(s));
        }
    }
}