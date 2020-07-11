#ifndef JSON_NULL_HPP
#define JSON_NULL_HPP

#include <ostream>

namespace json {

struct null final {
    constexpr null() = default;

    inline void print(std::ostream &out) const {
        out << "null";
    }

    inline void pretty_print(std::ostream &out, const std::size_t = 0) const {
        print(out);
    }
};

}

#endif
