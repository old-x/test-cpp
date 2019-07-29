#ifndef DECODER_HPP
#define DECODER_HPP

#include <array>
#include <cstdint>
#include <cstring>
#include <istream>
#include <limits>
#include <type_traits>

#include "data.hpp"

namespace pack {

class decoder final {
    static constexpr std::uint8_t max_size = 8;
public:
    enum class state_t: std::uint8_t {
        unknown = 0,
        no_first_byte = 1,
        result_type_too_small = 2,
        not_enough_data = 3,
        read_error = 4,
        ok = 5
    };

    template <typename type>
    struct result_t {
        static_assert(std::is_integral_v<type>, "type must be integral");
        static_assert(sizeof(type) > 0 && sizeof(type) <= max_size, "type must have size [1..8]");

        const state_t state = state_t::unknown;
        const type value = 0;
        const std::uint8_t size = 0;
    };

    static constexpr std::uint8_t get_size(const std::uint8_t value) {
        return sizes[value];
    }

    template <typename type>
    static result_t<type> read(std::istream &in) {
        const auto first_byte = in.peek();
        if (first_byte == std::istream::traits_type::eof()) {
            return {state_t::no_first_byte};
        }
        const decoded_size decoded_size{first_byte};
        if (decoded_size.size > sizeof(type)) {
            return {state_t::result_type_too_small};
        }
        if (in.get() == std::istream::traits_type::eof()) {
            return {state_t::no_first_byte};
        }
        if (decoded_size.size == 0) {
            return read<type>(first_byte, 0, decoded_size);
        }
        std::make_unsigned_t<type> value = 0;
        in.read(reinterpret_cast<std::istream::char_type*>(&value), decoded_size.size);
        if (in.eof()) {
            return {state_t::not_enough_data};
        }
        if (in.fail()) {
            return {state_t::read_error};
        }
        if (decoded_size.is_max_size()) {
            return {state_t::ok, static_cast<type>(value), decoded_size.buffer_size};
        }
        return read<type>(static_cast<std::uint8_t>(first_byte), value, decoded_size);
    }

    template <typename type>
    static result_t<type> read(const char *buffer, const std::size_t buffer_size) {
        if (!buffer || buffer_size < 1) {
            return {state_t::no_first_byte};
        }
        const std::uint8_t first_byte = static_cast<std::uint8_t>(buffer[0]);
        const decoded_size decoded_size{first_byte};
        if (buffer_size < decoded_size.buffer_size) {
            return {state_t::not_enough_data};
        }
        if (decoded_size.size > sizeof(type)) {
            return {state_t::result_type_too_small};
        }
        if (decoded_size.size == 0) {
            return read<type>(first_byte, 0, decoded_size);
        }
        const char *buffer_next = buffer + 1;
        if (decoded_size.is_max_size()) {
            return {state_t::ok, *reinterpret_cast<const type*>(buffer_next), decoded_size.buffer_size};
        }
        using value_t = std::make_unsigned_t<type>;
        value_t value = 0;
        if (decoded_size.size == sizeof(type)) {
            value = *reinterpret_cast<const value_t*>(buffer_next);
        } else {
            std::memcpy(&value, buffer_next, decoded_size.size);
        }
        return read<type>(first_byte, value, decoded_size);
    }

private:
    template <class ...args> decoder(args...) = delete;

    template <typename type>
    class data_reader {
    public:
        static constexpr type read(const std::uint8_t first_byte,
            const std::make_unsigned_t<type> value, const std::uint8_t size)
        {
            reader_t reader = get_reader(size);
            return (*reader)(first_byte, &value);
        }

    private:
        template <class ...args> data_reader(args...) = delete;

        using reader_t = type(*)(const std::uint8_t first_byte, const void *source);
        using readers_t = std::array<reader_t, max_size>;

        template <class data>
        static constexpr type read(const std::uint8_t first_byte, const void *source) {
            using data_t = typename data::type_t;
            data_t value = *reinterpret_cast<const data_t*>(source);
            const std::uint8_t size = data::bytes();
            value <<= (max_size - size);
            value |= (first_byte & masks[size - 1]);
            return data::template read<type>(value);
        }

        static constexpr reader_t get_reader(const std::uint8_t size) {
            if constexpr (std::is_signed_v<type>) {
                return signed_readers[size];
            } else {
                return unsigned_readers[size];
            }
        }

        static constexpr readers_t signed_readers = {
            &read<int1_t>,
            &read<int2_t>,
            &read<int3_t>,
            &read<int4_t>,
            &read<int5_t>,
            &read<int6_t>,
            &read<int7_t>,
            &read<int8_t>
        };

        static constexpr readers_t unsigned_readers = {
            &read<uint1_t>,
            &read<uint2_t>,
            &read<uint3_t>,
            &read<uint4_t>,
            &read<uint5_t>,
            &read<uint6_t>,
            &read<uint7_t>,
            &read<uint8_t>
        };
    };

    struct decoded_size {
        template <typename type>
        explicit constexpr decoded_size(const type first_byte):
        size{get_size(static_cast<std::uint8_t>(first_byte))},
        buffer_size{static_cast<std::uint8_t>(size + 1)},
        data_size{std::min(max_size, buffer_size)} {

        }

        constexpr bool is_max_size() const {
            return size == max_size;
        }

        const std::uint8_t size;
        const std::uint8_t buffer_size;
        const std::uint8_t data_size;
    };

    template <typename type>
    static result_t<type> read(const std::uint8_t first_byte,
        const std::make_unsigned_t<type> value, const decoded_size &decoded_size)
    {
        const type result = data_reader<type>::read(first_byte, value, decoded_size.size);
        return {state_t::ok, result, decoded_size.buffer_size};
    }

    static constexpr std::size_t sizes_size = std::numeric_limits<std::uint8_t>::max() + 1;

    using sizes_t = std::array<std::uint8_t, sizes_size>;

    static constexpr auto fill = [](sizes_t &sizes, const std::size_t start,
                                    const std::size_t finish, const std::uint8_t value)
    {
        for (std::size_t i = start; i <= finish; ++i) {
            sizes[i] = value;
        }
    };

    static constexpr sizes_t sizes = []() constexpr {
        sizes_t sizes = {};
        fill(sizes, 0b00000000, 0b01111111, 0);
        fill(sizes, 0b10000000, 0b10111111, 1);
        fill(sizes, 0b11000000, 0b11011111, 2);
        fill(sizes, 0b11100000, 0b11101111, 3);
        fill(sizes, 0b11110000, 0b11110111, 4);
        fill(sizes, 0b11111000, 0b11111011, 5);
        fill(sizes, 0b11111100, 0b11111101, 6);
        fill(sizes, 0b11111110, 0b11111110, 7);
        fill(sizes, 0b11111111, 0b11111111, 8);
        return sizes;
    }();

    static constexpr sizes_t masks = {
        0b0111'1111,
        0b0011'1111,
        0b0001'1111,
        0b0000'1111,
        0b0000'0111,
        0b0000'0011,
        0b0000'0001,
        0b0000'0000,
        0b0000'0000
    };
};

}

#endif
