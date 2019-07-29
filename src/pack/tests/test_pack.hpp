#ifndef TEST_PACK_HPP
#define TEST_PACK_HPP

#include <cassert>
#include <limits>
#include <sstream>

#include "pack/pack.hpp"

void test_data() {
    assert(pack::int1_t::bytes() == 1);
    assert(pack::int1_t::bites() == 7);
    assert(pack::int1_t::min_value() == -64);
    assert(pack::int1_t::max_value() == 63);
    assert(pack::int1_t::prev_min_value() == 0);
    assert(pack::int1_t::prev_max_value() == 0);

    assert(pack::uint1_t::bytes() == 1);
    assert(pack::uint1_t::bites() == 7);
    assert(pack::uint1_t::min_value() == 0);
    assert(pack::uint1_t::max_value() == 127);
    assert(pack::uint1_t::prev_min_value() == 0);
    assert(pack::uint1_t::prev_max_value() == 0);

    assert(pack::int2_t::bytes() == 2);
    assert(pack::int2_t::bites() == 14);
    assert(pack::int2_t::min_value() == -8192);
    assert(pack::int2_t::max_value() == 8191);
    assert(pack::int2_t::prev_min_value() == -64);
    assert(pack::int2_t::prev_max_value() == 63);

    assert(pack::uint2_t::bytes() == 2);
    assert(pack::uint2_t::bites() == 14);
    assert(pack::uint2_t::min_value() == 0);
    assert(pack::uint2_t::max_value() == 16383);
    assert(pack::uint2_t::prev_min_value() == 0);
    assert(pack::uint2_t::prev_max_value() == 127);

    assert(pack::int3_t::bytes() == 3);
    assert(pack::int3_t::bites() == 21);
    assert(pack::int3_t::min_value() == -1048576);
    assert(pack::int3_t::max_value() == 1048575);
    assert(pack::int3_t::prev_min_value() == -8192);
    assert(pack::int3_t::prev_max_value() == 8191);

    assert(pack::uint3_t::bytes() == 3);
    assert(pack::uint3_t::bites() == 21);
    assert(pack::uint3_t::min_value() == 0);
    assert(pack::uint3_t::max_value() == 2097151);
    assert(pack::uint3_t::prev_min_value() == 0);
    assert(pack::uint3_t::prev_max_value() == 16383);

    assert(pack::int4_t::bytes() == 4);
    assert(pack::int4_t::bites() == 28);
    assert(pack::int4_t::min_value() == -134217728);
    assert(pack::int4_t::max_value() == 134217727);
    assert(pack::int4_t::prev_min_value() == -1048576);
    assert(pack::int4_t::prev_max_value() == 1048575);

    assert(pack::uint4_t::bytes() == 4);
    assert(pack::uint4_t::bites() == 28);
    assert(pack::uint4_t::min_value() == 0);
    assert(pack::uint4_t::max_value() == 268435455);
    assert(pack::uint4_t::prev_min_value() == 0);
    assert(pack::uint4_t::prev_max_value() == 2097151);

    assert(pack::int5_t::bytes() == 5);
    assert(pack::int5_t::bites() == 35);
    assert(pack::int5_t::min_value() == -17179869184);
    assert(pack::int5_t::max_value() == 17179869183);
    assert(pack::int5_t::prev_min_value() == -134217728);
    assert(pack::int5_t::prev_max_value() == 134217727);

    assert(pack::uint5_t::bytes() == 5);
    assert(pack::uint5_t::bites() == 35);
    assert(pack::uint5_t::min_value() == 0);
    assert(pack::uint5_t::max_value() == 34359738367);
    assert(pack::uint5_t::prev_min_value() == 0);
    assert(pack::uint5_t::prev_max_value() == 268435455);

    assert(pack::int6_t::bytes() == 6);
    assert(pack::int6_t::bites() == 42);
    assert(pack::int6_t::min_value() == -2199023255552);
    assert(pack::int6_t::max_value() == 2199023255551);
    assert(pack::int6_t::prev_min_value() == -17179869184);
    assert(pack::int6_t::prev_max_value() == 17179869183);

    assert(pack::uint6_t::bytes() == 6);
    assert(pack::uint6_t::bites() == 42);
    assert(pack::uint6_t::min_value() == 0);
    assert(pack::uint6_t::max_value() == 4398046511103);
    assert(pack::uint6_t::prev_min_value() == 0);
    assert(pack::uint6_t::prev_max_value() == 34359738367);

    assert(pack::int7_t::bytes() == 7);
    assert(pack::int7_t::bites() == 49);
    assert(pack::int7_t::min_value() == -281474976710656);
    assert(pack::int7_t::max_value() == 281474976710655);
    assert(pack::int7_t::prev_min_value() == -2199023255552);
    assert(pack::int7_t::prev_max_value() == 2199023255551);

    assert(pack::uint7_t::bytes() == 7);
    assert(pack::uint7_t::bites() == 49);
    assert(pack::uint7_t::min_value() == 0);
    assert(pack::uint7_t::max_value() == 562949953421311);
    assert(pack::uint7_t::prev_min_value() == 0);
    assert(pack::uint7_t::prev_max_value() == 4398046511103);

    assert(pack::int8_t::bytes() == 8);
    assert(pack::int8_t::bites() == 56);
    assert(pack::int8_t::min_value() == -36028797018963968);
    assert(pack::int8_t::max_value() == 36028797018963967);
    assert(pack::int8_t::prev_min_value() == -281474976710656);
    assert(pack::int8_t::prev_max_value() == 281474976710655);

    assert(pack::uint8_t::bytes() == 8);
    assert(pack::uint8_t::bites() == 56);
    assert(pack::uint8_t::min_value() == 0);
    assert(pack::uint8_t::max_value() == 72057594037927935);
    assert(pack::uint8_t::prev_min_value() == 0);
    assert(pack::uint8_t::prev_max_value() == 562949953421311);
}

void test_decoder() {
    assert(pack::decoder::get_size(0b00000000) == 0);
    assert(pack::decoder::get_size(0b01111111) == 0);
    assert(pack::decoder::get_size(0b10000000) == 1);
    assert(pack::decoder::get_size(0b10111111) == 1);
    assert(pack::decoder::get_size(0b11000000) == 2);
    assert(pack::decoder::get_size(0b11011111) == 2);
    assert(pack::decoder::get_size(0b11100000) == 3);
    assert(pack::decoder::get_size(0b11101111) == 3);
    assert(pack::decoder::get_size(0b11110000) == 4);
    assert(pack::decoder::get_size(0b11110111) == 4);
    assert(pack::decoder::get_size(0b11111000) == 5);
    assert(pack::decoder::get_size(0b11111011) == 5);
    assert(pack::decoder::get_size(0b11111100) == 6);
    assert(pack::decoder::get_size(0b11111101) == 6);
    assert(pack::decoder::get_size(0b11111110) == 7);
    assert(pack::decoder::get_size(0b11111111) == 8);
}

void test_encoder() {
    assert(pack::encoder::get_size(std::int8_t{-64}) == 0);
    assert(pack::encoder::get_size(std::int8_t{63}) == 0);
    assert(pack::encoder::get_size(std::uint8_t{127}) == 0);

    assert(pack::encoder::get_size(std::int16_t{-8192}) == 1);
    assert(pack::encoder::get_size(std::int16_t{8191}) == 1);
    assert(pack::encoder::get_size(std::uint16_t{16383}) == 1);

    assert(pack::encoder::get_size(std::int32_t{-1048576}) == 2);
    assert(pack::encoder::get_size(std::int32_t{1048575}) == 2);
    assert(pack::encoder::get_size(std::uint32_t{2097151}) == 2);

    assert(pack::encoder::get_size(std::int32_t{-134217728}) == 3);
    assert(pack::encoder::get_size(std::int32_t{134217727}) == 3);
    assert(pack::encoder::get_size(std::uint32_t{268435455}) == 3);

    assert(pack::encoder::get_size(std::int64_t{-17179869184}) == 4);
    assert(pack::encoder::get_size(std::int64_t{17179869183}) == 4);
    assert(pack::encoder::get_size(std::uint64_t{34359738367}) == 4);

    assert(pack::encoder::get_size(std::int64_t{-2199023255552}) == 5);
    assert(pack::encoder::get_size(std::int64_t{2199023255551}) == 5);
    assert(pack::encoder::get_size(std::uint64_t{4398046511103}) == 5);

    assert(pack::encoder::get_size(std::int64_t{-281474976710656}) == 6);
    assert(pack::encoder::get_size(std::int64_t{281474976710655}) == 6);
    assert(pack::encoder::get_size(std::uint64_t{562949953421311}) == 6);

    assert(pack::encoder::get_size(std::int64_t{-36028797018963968}) == 7);
    assert(pack::encoder::get_size(std::int64_t{36028797018963967}) == 7);
    assert(pack::encoder::get_size(std::uint64_t{72057594037927935}) == 7);

    assert(pack::encoder::get_size(std::numeric_limits<std::int8_t>::min()) == 1);
    assert(pack::encoder::get_size(std::numeric_limits<std::int8_t>::max()) == 1);

    assert(pack::encoder::get_size(std::numeric_limits<std::uint8_t>::min()) == 0);
    assert(pack::encoder::get_size(std::numeric_limits<std::uint8_t>::max()) == 1);

    assert(pack::encoder::get_size(std::numeric_limits<std::int16_t>::min()) == 2);
    assert(pack::encoder::get_size(std::numeric_limits<std::int16_t>::max()) == 2);

    assert(pack::encoder::get_size(std::numeric_limits<std::uint16_t>::min()) == 0);
    assert(pack::encoder::get_size(std::numeric_limits<std::uint16_t>::max()) == 2);

    assert(pack::encoder::get_size(std::numeric_limits<std::int32_t>::min()) == 4);
    assert(pack::encoder::get_size(std::numeric_limits<std::int32_t>::max()) == 4);

    assert(pack::encoder::get_size(std::numeric_limits<std::uint32_t>::min()) == 0);
    assert(pack::encoder::get_size(std::numeric_limits<std::uint32_t>::max()) == 4);

    assert(pack::encoder::get_size(std::numeric_limits<std::int64_t>::min()) == 8);
    assert(pack::encoder::get_size(std::numeric_limits<std::int64_t>::max()) == 8);

    assert(pack::encoder::get_size(std::numeric_limits<std::uint64_t>::min()) == 0);
    assert(pack::encoder::get_size(std::numeric_limits<std::uint64_t>::max()) == 8);
}

template <typename type>
static void _test_pack_unpack_buffer(const type value) {
    char buffer[100] = {};
    const auto [enc_state, enc_size] = pack::encoder::write(value, buffer, sizeof(buffer));
    assert(enc_state == pack::encoder::state_t::ok);
    const auto [dec_state, dec_value, dec_size] = pack::decoder::read<type>(buffer, sizeof(buffer));
    assert(dec_state == pack::decoder::state_t::ok);
    assert(dec_value == value);
    assert(dec_size == enc_size);
}

template <typename type>
static void _test_pack_unpack_stream(const type value) {
    std::stringstream stream;
    const auto [enc_state, enc_size] = pack::encoder::write(value, stream);
    assert(enc_state == pack::encoder::state_t::ok);
    const auto [dec_state, dec_value, dec_size] = pack::decoder::read<type>(stream);
    assert(dec_state == pack::decoder::state_t::ok);
    assert(dec_value == value);
    assert(dec_size == enc_size);
}

template <typename type>
static void _test_pack_unpack() {
    type value = 0;
    const bool is_signed = std::is_signed_v<type>;
    for (std::uint8_t i = 0, n = sizeof(type) * 8 - (is_signed ? 1 : 0); i < n; ++i) {
        _test_pack_unpack_buffer<type>(value);
        _test_pack_unpack_stream<type>(value);
        if (is_signed) {
            _test_pack_unpack_buffer<type>(-value);
            _test_pack_unpack_stream<type>(-value);
        }
        value |= type{1} << i;
    }
}

void test_pack_unpack() {
    _test_pack_unpack<std::int8_t>();
    _test_pack_unpack<std::uint8_t>();
    _test_pack_unpack<std::int16_t>();
    _test_pack_unpack<std::uint16_t>();
    _test_pack_unpack<std::int32_t>();
    _test_pack_unpack<std::uint32_t>();
    _test_pack_unpack<std::int64_t>();
    _test_pack_unpack<std::uint64_t>();
}

#endif
