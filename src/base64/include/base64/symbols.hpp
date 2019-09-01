#ifndef BASE64_SYMBOLS_HPP
#define BASE64_SYMBOLS_HPP

#include <array>
#include <cstdint>

namespace base64 {

struct symbols {
    static constexpr std::uint8_t size = 64;
    static constexpr std::uint8_t value_bits = 6;
    static constexpr std::uint8_t value_empty_bits = 2;
    static constexpr std::uint8_t half_byte = 4;
    static constexpr std::uint8_t value_mask_hi = 0b11111100;
    static constexpr std::uint8_t value_mask_lo = 0b00111111;
    static constexpr std::uint8_t half_byte_mask_hi = 0b11110000;
    static constexpr std::uint8_t half_byte_mask_md = 0b00111100;
    static constexpr std::uint8_t half_byte_mask_lo = 0b00001111;
    static constexpr std::uint8_t two_bits_mask_hi = 0b11000000;
    static constexpr std::uint8_t two_bits_mask_md = 0b00110000;
    static constexpr std::uint8_t two_bits_mask_lo = 0b00000011;
    static constexpr char symbol62_default = '+';
    static constexpr char symbol63_default = '/';
    static constexpr char symbol62_url = '-';
    static constexpr char symbol63_url = '_';
    static constexpr char padding_symbol = '=';

    using table_t = std::array<char, size>;

    template <char c62, char c63>
    static constexpr table_t table = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
        'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
        'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
        'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
        'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
        'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
        'w', 'x', 'y', 'z', '0', '1', '2', '3',
        '4', '5', '6', '7', '8', '9', c62, c63
    };
};

}

#endif
