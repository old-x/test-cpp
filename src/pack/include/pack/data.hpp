#ifndef PACK_DATA_HPP
#define PACK_DATA_HPP

#include <cstdint>
#include <type_traits>

namespace pack {

template <typename type, std::uint8_t size, class prev>
class data {
    static_assert(std::is_integral_v<type>, "type must be integral");
    static_assert([]() constexpr {
        switch (sizeof(type)) {
        case 1:
            return size == 1;
        case 2:
            return size == 2;
        case 4:
            return size > 2 && size <= 4;
        case 8:
            return size > 4 && size <= 8;
        default:
            return false;
        }
    }(), "type and size must match");

public:
    using type_t = type;
    using prev_t = prev;

    static constexpr std::uint8_t bytes() {
        return size;
    }

    static constexpr std::uint8_t bites() {
        return (size << 3) - size;
    }

    static constexpr type min_value() {
        if constexpr (std::is_signed_v<type>) {
            return -(type{1} << (bites() - 1));
        } else {
            return 0;
        }
    }

    static constexpr type max_value() {
        if constexpr (std::is_signed_v<type>) {
            return (type{1} << (bites() - 1)) - 1;
        } else {
            return (type{1} << bites()) - 1;
        }
    }

    static constexpr auto prev_min_value() {
        if constexpr (std::is_same_v<prev, void>) {
            return 0;
        } else {
            return prev::min_value();
        }
    }

    static constexpr auto prev_max_value() {
        if constexpr (std::is_same_v<prev, void>) {
            return 0;
        } else {
            return prev::max_value();
        }
    }

    template <typename result>
    static constexpr result read(const type value) {
        return static_cast<result>(data{value}._value);
    }

private:
    data(const type value):
    _value{value} {

    }

    type _value: bites();
};

using int1_t = data<std::int8_t, 1, void>;
using int2_t = data<std::int16_t, 2, int1_t>;
using int3_t = data<std::int32_t, 3, int2_t>;
using int4_t = data<std::int32_t, 4, int3_t>;
using int5_t = data<std::int64_t, 5, int4_t>;
using int6_t = data<std::int64_t, 6, int5_t>;
using int7_t = data<std::int64_t, 7, int6_t>;
using int8_t = data<std::int64_t, 8, int7_t>;

using uint1_t = data<std::uint8_t, 1, void>;
using uint2_t = data<std::uint16_t, 2, uint1_t>;
using uint3_t = data<std::uint32_t, 3, uint2_t>;
using uint4_t = data<std::uint32_t, 4, uint3_t>;
using uint5_t = data<std::uint64_t, 5, uint4_t>;
using uint6_t = data<std::uint64_t, 6, uint5_t>;
using uint7_t = data<std::uint64_t, 7, uint6_t>;
using uint8_t = data<std::uint64_t, 8, uint7_t>;

}

#endif
