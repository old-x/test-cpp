#ifndef ENCODER_HPP
#define ENCODER_HPP

#include <array>
#include <cstdint>
#include <cstring>
#include <ostream>
#include <type_traits>

#include "data.hpp"

namespace pack {

class encoder final {
    static constexpr std::uint8_t under_max_size = 7;
    static constexpr std::uint8_t max_size = 8;

public:
    enum class state_t: std::uint8_t {
        unknown = 0,
        not_enough_data = 1,
        buffer_too_small = 2,
        write_error = 3,
        ok = 4
    };

    struct result_t {
        const state_t state = state_t::unknown;
        const std::uint8_t size = 0;
    };

    template <typename type>
    static std::uint8_t get_size(const type value) {
        static_assert(std::is_integral_v<type>, "type must be integral");
        static_assert(sizeof(type) > 0 && sizeof(type) <= max_size, "type must have size [1..8]");

        constexpr std::uint8_t index = get_size_getter_index<type>();
        size_getter_t size_getter = get_size_getter<type>(index);
        return (*size_getter)(&value, index);
    }

    template <typename type>
    static result_t write(type value, std::ostream &out) {
        const encoded_size encoded_size{value};
        if (sizeof(type) < encoded_size.size) {
            return {state_t::not_enough_data};
        }
        const auto value_ptr = reinterpret_cast<const std::ostream::char_type*>(&value);
        if (encoded_size.is_max_size() || encoded_size.is_under_max_size()) {
            const bool error = out.put(first_bytes[encoded_size.size]).fail() ||
                out.write(value_ptr, encoded_size.data_size).fail();
            if (error) {
                return {state_t::write_error};
            }
            return {state_t::ok, encoded_size.buffer_size};
        }
        std::ostream::char_type first_byte = value & 0xFF;
        combine(first_byte, value, encoded_size);
        if (out.put(first_byte).fail()) {
            return {state_t::write_error};
        }
        if (encoded_size.size > 0 &&
            out.write(value_ptr, encoded_size.size).fail())
        {
            return {state_t::write_error};
        }
        return {state_t::ok, encoded_size.buffer_size};
    }

    template <typename type>
    static result_t write(type value, char *buffer, const std::size_t buffer_size) {
        const encoded_size encoded_size{value};
        if (sizeof(type) < encoded_size.size) {
            return {state_t::not_enough_data};
        }
        if (buffer_size < encoded_size.buffer_size) {
            return {state_t::buffer_too_small};
        }
        if (encoded_size.is_max_size() || encoded_size.is_under_max_size()) {
            buffer[0] = first_bytes[encoded_size.size];
            char *next = buffer + 1;
            if (encoded_size.data_size == sizeof(type)) {
                *reinterpret_cast<type*>(next) = value;
            } else {
                std::memcpy(next, &value, encoded_size.data_size);
            }
            return {state_t::ok, encoded_size.buffer_size};
        }
        char first_byte = value & 0xFF;
        combine(first_byte, value, encoded_size);
        buffer[0] = first_byte;
        if (encoded_size.size > 0) {
            char *next = buffer + 1;
            if (encoded_size.size == sizeof(type)) {
                *reinterpret_cast<type*>(next) = value;
            } else {
                std::memcpy(next, &value, encoded_size.size);
            }
        }
        return {state_t::ok, encoded_size.buffer_size};
    }

private:
    template <class ...args> encoder(args...) = delete;

    using size_getter_t = std::uint8_t(*)(const void *vptr, const std::uint8_t index);
    using size_getters_t = std::array<size_getter_t, max_size>;
    using first_bytes_t = std::array<char, max_size + 1>;

    struct encoded_size {
        template <typename type>
        explicit constexpr encoded_size(const type value):
        size{get_size<type>(value)},
        buffer_size{static_cast<std::uint8_t>(size + 1)},
        data_size{std::min(max_size, buffer_size)} {

        }

        constexpr bool is_under_max_size() const {
            return size == under_max_size;
        }

        constexpr bool is_max_size() const {
            return size == max_size;
        }

        const std::uint8_t size;
        const std::uint8_t buffer_size;
        const std::uint8_t data_size;
    };

    template <typename type>
    static constexpr std::uint8_t get_size_getter_index() {
        return sizeof(type) - 1;
    }

    template <typename type>
    static constexpr size_getter_t get_size_getter(const std::uint8_t index) {
        if constexpr (std::is_signed_v<type>) {
            return signed_size_getters[index];
        } else {
            return unsigned_size_getters[index];
        }
    }

    template <typename type>
    static constexpr std::uint8_t get_size(const void *vptr, const std::uint8_t index) {
        using type_t = typename type::type_t;
        const type_t value = *reinterpret_cast<const type_t*>(vptr);

        if (value > type::max_value() || value < type::min_value()) {
            return index + 1;
        }
        if (index == 0) {
            return index;
        }
        if (value > type::prev_max_value() || value < type::prev_min_value()) {
            return index;
        }

        size_getter_t size_getter = get_size_getter<type_t>(index - 1);
        return (*size_getter)(vptr, index - 1);
    }

    template <typename first_byte_t, typename value_t>
    static inline void combine(first_byte_t &first_byte, value_t &value, const encoded_size &encoded_size) {
        first_byte &= first_byte_masks[encoded_size.size];
        first_byte |= first_bytes[encoded_size.size];
        value >>= (max_size - encoded_size.data_size);
    }

    static constexpr size_getters_t signed_size_getters = {
        &get_size<int1_t>,
        &get_size<int2_t>,
        &get_size<int3_t>,
        &get_size<int4_t>,
        &get_size<int5_t>,
        &get_size<int6_t>,
        &get_size<int7_t>,
        &get_size<int8_t>
    };

    static constexpr size_getters_t unsigned_size_getters = {
        &get_size<uint1_t>,
        &get_size<uint2_t>,
        &get_size<uint3_t>,
        &get_size<uint4_t>,
        &get_size<uint5_t>,
        &get_size<uint6_t>,
        &get_size<uint7_t>,
        &get_size<uint8_t>
    };

    static constexpr first_bytes_t first_bytes = {
        static_cast<char>(0b0000'0000),
        static_cast<char>(0b1000'0000),
        static_cast<char>(0b1100'0000),
        static_cast<char>(0b1110'0000),
        static_cast<char>(0b1111'0000),
        static_cast<char>(0b1111'1000),
        static_cast<char>(0b1111'1100),
        static_cast<char>(0b1111'1110),
        static_cast<char>(0b1111'1111)
    };

    static constexpr first_bytes_t first_byte_masks = {
        static_cast<char>(0b0111'1111),
        static_cast<char>(0b0011'1111),
        static_cast<char>(0b0001'1111),
        static_cast<char>(0b0000'1111),
        static_cast<char>(0b0000'0111),
        static_cast<char>(0b0000'0011),
        static_cast<char>(0b0000'0001),
        static_cast<char>(0b0000'0000),
        static_cast<char>(0b0000'0000)
    };
};

template std::uint8_t encoder::get_size<int1_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<int2_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<int3_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<int4_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<int5_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<int6_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<int7_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<int8_t>(const void *vptr, const std::uint8_t index);

template std::uint8_t encoder::get_size<uint1_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<uint2_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<uint3_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<uint4_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<uint5_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<uint6_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<uint7_t>(const void *vptr, const std::uint8_t index);
template std::uint8_t encoder::get_size<uint8_t>(const void *vptr, const std::uint8_t index);

}

#endif
