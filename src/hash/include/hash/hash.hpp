#ifndef HASH_HPP
#define HASH_HPP

#include <functional>
#include <limits>
#include <type_traits>
#include <utility>

namespace hash {

template <typename T>
int calculate(T &&value) {
    using value_t = std::remove_cvref_t<T>;

    if constexpr (std::is_integral_v<value_t> && sizeof(value_t) <= sizeof(int)) {
        return static_cast<int>(value);
    } else {
        std::size_t hash = std::hash<value_t>{}(std::forward<T>(value));
        hash = hash ^ (hash >> std::numeric_limits<unsigned int>::digits);
        return static_cast<int>(hash);
    }
}

template <typename T, typename U>
long affinity(T &&key0, U &&key1) {
    int hash0 = calculate(std::forward<T>(key0));
    int hash1 = calculate(std::forward<U>(key1));
    unsigned long mask = ~0U;
    unsigned long key = (hash0 & mask) | ((hash1 & mask) << std::numeric_limits<unsigned int>::digits);

    key = (~key) + (key << 21); // key = (key << 21) - key - 1;
    key ^= (key >> 24);
    key += (key << 3) + (key << 8); // key * 265
    key ^= (key >> 14);
    key += (key << 2) + (key << 4); // key * 21
    key ^= (key >> 28);
    key += (key << 31);

    return static_cast<long>(key);
}

}

#endif
