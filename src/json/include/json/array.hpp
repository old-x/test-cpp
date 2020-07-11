#ifndef JSON_ARRAY_HPP
#define JSON_ARRAY_HPP

#include <ostream>
#include <vector>

#include "utils.hpp"

namespace json::tmp {

template <typename value>
class array final {
public:
    template <typename ...types>
    array(types &&...args): values{std::forward<types>(args)...} {}

    inline array &add(const value &&v) {
        values.push_back(v);
        return *this;
    }

    inline array &add(value &&v) {
        values.push_back(std::move(v));
        return *this;
    }

    inline auto size() const {
        return values.size();
    }

    inline const value &get(const std::size_t index) const {
        return values[index];
    }

    inline value &get(const std::size_t index) {
        return values[index];
    }

    inline auto begin() const {
        return values.begin();
    }

    inline auto begin() {
        return values.begin();
    }

    inline auto end() const {
        return values.end();
    }

    inline auto end() {
        return values.end();
    }

    inline void print(std::ostream &out) const {
        print(out, utils::printer{});
    }

    inline void pretty_print(std::ostream &out, const std::size_t indent = 0) const {
        print(out, utils::pretty_printer{indent});
    }

private:
    std::vector<value> values;

    template <typename printer>
    void print(std::ostream &out, const printer p) const {
        if (values.empty()) {
            p.print_empty_array(out);
        } else {
            p.print_array_opening(out);
            p.print_value(out, values.front());
            for (std::size_t i = 1, n = values.size(); i < n; ++i) {
                p.print_values_separator(out);
                p.print_value(out, values[i]);
            }
            p.print_array_closing(out);
        }
    }
};

}

#endif
