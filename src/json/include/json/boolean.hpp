#ifndef JSON_BOOLEAN_HPP
#define JSON_BOOLEAN_HPP

#include <ostream>

namespace json {

class boolean final {
    static constexpr const char *values[] = {"false", "true"};
public:
    constexpr boolean(bool b): value{b} {}

    constexpr operator bool() const {
        return value;
    }

    inline void print(std::ostream &out) const {
        out << values[static_cast<int>(value)];
    }

    inline void pretty_print(std::ostream &out, const std::size_t = 0) const {
        print(out);
    }

private:
    bool value;
};

}

#endif
