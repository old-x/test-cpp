#ifndef JSON_STRING_HPP
#define JSON_STRING_HPP

#include <ostream>
#include <string>

namespace json {

class string final {
public:
    string(const char *s): value{s} {}
    string(const std::string &s): value{s} {}
    string(std::string &&s): value{std::move(s)} {}

    inline const std::string &get_value() const {
        return value;
    }

    inline void print(std::ostream &out) const {
        out << '"' << value << '"';
    }

    inline void pretty_print(std::ostream &out, const std::size_t = 0) const {
        print(out);
    }

private:
    std::string value;
};

}

#endif
