
#ifndef LOOKUPTABLE_H
#define LOOKUPTABLE_H
#include <array>
#include <string_view>
#include <cstdint>

#endif //LOOKUPTABLE_H

namespace lut {
    static constexpr auto MakeTable(const std::string_view acceptedCharacters) {
        std::array<bool, 256> lut{0};
        for (const auto s : acceptedCharacters) {
            lut.at(static_cast<uint8_t>(s)) = true;
        }

        return lut;
    }

    constexpr auto WhitespaceDigits = MakeTable(" \t\n\r\v\f");
    constexpr auto NumericDigits = MakeTable("0123456789");
    constexpr auto RealNumericDigits = MakeTable(".0123456789");
    constexpr auto KeyDigits = MakeTable("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_0123456789");
    constexpr auto KeyValidStart = MakeTable("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_");
    constexpr auto StringDigits = MakeTable("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    constexpr auto TrueBooleanDigits = MakeTable("true");
    constexpr auto FalseBooleanDigits = MakeTable("false");
}