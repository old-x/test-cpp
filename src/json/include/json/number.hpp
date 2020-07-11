#ifndef JSON_NUMBER_HPP
#define JSON_NUMBER_HPP

#include <charconv>
#include <cstdint>
#include <iomanip>
#include <limits>
#include <ostream>
#include <variant>

namespace json {

class number final {
public:
    constexpr number(int i): value{static_cast<std::int64_t>(i)} {}
    constexpr number(std::int64_t l): value{l} {}
    constexpr number(double d): value{d} {}

    constexpr std::int64_t to_long() const {
        return std::visit(long_getter{}, value);
    }

    constexpr double to_double() const {
        return std::visit(double_getter{}, value);
    }

    inline void print(std::ostream &out) const {
        std::visit(printer{out}, value);
    }

    inline void pretty_print(std::ostream &out, const std::size_t = 0) const {
        print(out);
    }

private:
    std::variant<std::int64_t, double> value;

    struct long_getter {
        constexpr std::int64_t operator()(std::int64_t value) const {
            return value;
        }

        constexpr std::int64_t operator()(double value) const {
            return static_cast<std::int64_t>(value);
        }
    };

    struct double_getter {
        constexpr double operator()(std::int64_t value) const {
            return static_cast<double>(value);
        }

        constexpr double operator()(double value) const {
            return value;
        }
    };

    struct printer {
        std::ostream &out;

        static constexpr int max_long_digits = std::numeric_limits<std::int64_t>::digits10 + 2;
        static constexpr int max_precision = std::numeric_limits<double>::digits10;

        void operator()(std::int64_t value) const {
            char str[max_long_digits];
            auto [p, ec] = std::to_chars(str, str + max_long_digits, value);
            if (ec == std::errc{}) {
                out.write(str, p - str);
            }
        }

        void operator()(double value) const {
            out << std::setprecision(max_precision) << value;
        }
    };
};

}

#endif
