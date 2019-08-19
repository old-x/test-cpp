#ifndef ARGS_VALUE_HPP
#define ARGS_VALUE_HPP

#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

namespace args {

class value final {
public:
    value() = default;

    explicit value(std::string &&val):
    _value{std::move(val)} {

    }

    value(const std::string::value_type *data, const std::string::size_type size):
    _value{data, size} {

    }

    operator bool() const {
        return !_value.empty();
    }

    template <class type = value>
    std::optional<type> get() const {
        if constexpr (is_value<type>) {
            return *this;
        } else {
            return get<type>(_value);
        }
    }

    template <class type = value>
    std::optional<std::vector<type>> get_values() const {
        std::size_t start = 0;
        std::vector<type> values;
        for (std::size_t i = 0, n = _value.size(); i < n; ++i) {
            if (_value[i] == delimiter_symbol) {
                add_value<type>(values, start, i - start);
                start = i + 1;
            }
        }
        add_value<type>(values, start, _value.size() - start);
        return values;
    }

    value &set(const std::string_view &val) {
        return set(val.data(), val.size());
    }

    value &set(const std::string::value_type *data, const std::string::size_type size) {
        _value.assign(data, size);
        return *this;
    }

    value &add(const std::string_view &val) {
        return add(val.data(), val.size());
    }

    value &add(const std::string::value_type *data, const std::string::size_type size) {
        if (_value.empty()) {
            _value.assign(data, size);
        } else {
            _value.push_back(delimiter_symbol);
            _value.append(data, size);
        }
        return *this;
    }

private:
    std::string _value;

    static constexpr char delimiter_symbol = '\x00';

    template <class type, typename = decltype(std::declval<std::istream>().operator>>(std::declval<type&>()))>
    static constexpr bool is_readable_by_member_function(std::nullptr_t) {
        return true;
    }

    template <class>
    static constexpr bool is_readable_by_member_function(...) {
        return false;
    }

    template <class type, typename = decltype(operator>>(std::declval<std::istream&>(), std::declval<type&>()))>
    static constexpr bool is_readable_by_free_function(std::nullptr_t) {
        return true;
    }

    template <class>
    static constexpr bool is_readable_by_free_function(...) {
        return false;
    }

    template <class type>
    static constexpr bool is_string = std::is_same_v<type, std::string>;

    template <class type>
    static constexpr bool is_bool = std::is_same_v<type, bool>;

    template <class type>
    static constexpr bool is_value = std::is_same_v<type, value>;

    template <class type>
    static constexpr bool is_constructible_from_string = std::is_constructible_v<type, std::string>;

    template <class type>
    static constexpr bool is_constructible_from_stream = std::is_constructible_v<type, std::istream&>;

    template <class type>
    static constexpr bool is_readable_from_stream = std::is_default_constructible_v<type> &&
        (is_readable_by_member_function<type>(nullptr) || is_readable_by_free_function<type>(nullptr));

    template <class type>
    static std::optional<type> get(std::string &&val) {
        return get_impl<type>(std::move(val));
    }

    template <class type>
    static std::optional<type> get(const std::string &val) {
        return get_impl<type>(val);
    }

    template <class type, class string>
    static std::optional<type> get_impl(string &&val) {
        static_assert(
            is_string<type> ||
            is_bool<type> ||
            is_constructible_from_string<type> ||
            is_constructible_from_stream<type> ||
            is_readable_from_stream<type>,
            "type cannot be constructed"
        );

        if constexpr (is_string<type>) {
            return std::forward<string>(val);
        }

        if constexpr (is_bool<type>) {
            switch (val.size()) {
            case 1:
                return val[0] == '1';
            case 4:
                return (val[0] == 't' || val[0] == 'T') &&
                    (val[1] == 'r' || val[1] == 'R') &&
                    (val[2] == 'u' || val[2] == 'U') &&
                    (val[3] == 'e' || val[3] == 'E');
            }
            return false;
        }

        if constexpr (is_constructible_from_string<type>) {
            return type{std::forward<string>(val)};
        }

        if  constexpr (is_constructible_from_stream<type>) {
            std::istringstream in{std::forward<string>(val)};
            return type{in};
        }

        if constexpr (is_readable_from_stream<type>) {
            type result;
            std::istringstream in{std::forward<string>(val)};
            in >> result;
            if (in.eof() && !in.fail()) {
                return std::move(result);
            }
        }
        return {};
    }

    template <class type>
    void add_value(std::vector<type> &values, const std::size_t start, const std::size_t length) const {
        if constexpr (is_value<type>) {
            values.push_back(value{_value.substr(start, length)});
        } else if (auto opt = get<type>(_value.substr(start, length)); opt) {
            values.push_back(std::move(opt).value());
        }
    }

    friend std::ostream &operator<<(std::ostream &out, const value &val);
};

std::ostream &operator<<(std::ostream &out, const value &val) {
    return out << val._value;
}

}

#endif
