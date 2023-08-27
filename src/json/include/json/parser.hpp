#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include <string_view>

#include "exception.hpp"
#include "value.hpp"


#include <iostream>

namespace json::tmp {

enum class type: std::uint8_t {
    array = 0,
    boolean,
    null,
    number,
    object,
    string,
    unknown
};

static constexpr const char *type_names[] = {
    "array",
    "boolean",
    "null",
    "number",
    "object",
    "string",
    "unknown"
};

static constexpr const char *to_string(const type t) {
    const std::uint8_t i = static_cast<std::uint8_t>(t);
    return type_names[i];
}

static constexpr bool is_space(const char c) {
    // space || line feed || carriage return || horizontal tab
    return c == ' ' || c == '\n' || c == '\r' || c == '\t';
}

static constexpr bool is_digit(const char c) {
    return c >= '0' && c <= '9';
}

static constexpr std::string_view::size_type skip_spaces(const std::string_view s,
                                                         std::string_view::size_type i)
{
    for (std::string_view::size_type n = s.size(); i < n; ++i) {
        if (!is_space(s[i])) {
            return i;
        }
    }
    return std::string_view::npos;
}

static constexpr type get_type(const char c) {
    switch (c) {
    case '[': return type::array;
    case 't':
    case 'f': return type::boolean;
    case 'n': return type::null;
    case '-': return type::number;
    case '{': return type::object;
    case '"': return type::string;
    default: return is_digit(c) ? type::number : type::unknown;
    }
}

static std::string_view::size_type parse(const std::string_view s,
                                         const std::string_view::size_type i);

static std::string_view::size_type parse_array(const std::string_view s,
                                               const std::string_view::size_type i)
{
    std::cout << "array" << std::endl;
    const std::string_view::size_type n = s.size();
    std::string_view::size_type j = skip_spaces(s, i);
    if (j < n && s[j] == ']') {
        return j + 1;
    }
    while (j < n) {
        j = parse(s, j);
        j = skip_spaces(s, j);
        if (j < n) {
            const char c = s[j];
            if (c == ']') {
                return j + 1;
            }
            if (c == ',') {
                ++j;
                continue;
            }
        }
        break;
    }
    throw exception{s, i};
}

static std::string_view::size_type parse_boolean(const std::string_view s,
                                                 const std::string_view::size_type i)
{
    std::cout << "boolean" << std::endl;
    switch (s[i]) {
    case 'r':
        if ((i + 2) < s.size() && s[i + 1] == 'u' && s[i + 2] == 'e') {
            return i + 3;
        }
        break;
    case 'a':
        if ((i + 2) < s.size() && s[i + 1] == 'l' && s[i + 2] == 's' && s[i + 3] == 'e') {
            return i + 4;
        }
        break;
    }
    throw exception{s, i};
}

static std::string_view::size_type parse_null(const std::string_view s,
                                              const std::string_view::size_type i)
{
    std::cout << "null" << std::endl;
    if ((i + 2) < s.size() && s[i] == 'u' && s[i + 1] == 'l' && s[i + 2] == 'l') {
        return i + 3;
    }
    throw exception{s, i};
}

static std::string_view::size_type parse_digits(const std::string_view s,
                                                const std::string_view::size_type i)
{
    std::cout << "digits" << std::endl;
    bool digit = false;
    std::string_view::size_type j = i;
    for (std::string_view::size_type n = s.size(); j < n; ++j) {
        const char c = s[j];
        if (is_digit(c)) {
            digit = true;
        } else if (digit) {
            std::cout << "    " << s.substr(i, j - i) << std::endl;
            return j;
        } else {
            throw exception(s, j);
        }
    }
    std::cout << "    " << s.substr(i, j - i) << std::endl;
    return j;
}

static std::string_view::size_type parse_exponent(const std::string_view s,
                                                  const std::string_view::size_type i)
{
    std::cout << "exponent" << std::endl;
    bool sign = false;
    std::string_view::size_type j = i;
    for (std::string_view::size_type n = s.size(); j < n; ++j) {
        const char c = s[j];
        if (is_digit(c)) {
            return parse_digits(s, j);
        } else if (c == '-' || c == '+') {
            if (sign) {
                throw exception(s, j);
            }
            sign = true;
        } else {
            throw exception(s, j);
        }
    }
    throw exception(s, i);
}

static std::string_view::size_type parse_number(const std::string_view s,
                                                const std::string_view::size_type i)
{
    std::cout << "number" << std::endl;
    const std::string_view::size_type n = s.size();
    std::string_view::size_type j = i;
    bool neg = false;
    bool digit = false;
    bool dot = false;
    while (j < n) {
        const char c = s[j];
        switch (c) {
        case '-':
            if (neg || digit) {
                throw exception(s, j);
            }
            neg = true;
            ++j;
            break;
        case '.':
            if (!digit || dot) {
                throw exception(s, j);
            }
            dot = true;
            j = parse_digits(s, j + 1);
            break;
        case 'e':
        case 'E':
            j = parse_exponent(s, j + 1);
            break;
        default:
            if (is_digit(c)) {
                digit = true;
                j = parse_digits(s, j);
            } else {
                return j;
            }
        }
    }
    return j;
}

static std::string_view::size_type parse_string(const std::string_view s,
                                                const std::string_view::size_type i)
{
    std::cout << "string" << std::endl;
    bool skip = false;
    for (std::string_view::size_type j = i, n = s.size(); j < n; ++j) {
        switch (s[j]) {
        case '\\':
            skip = !skip;
            break;
        case '"':
            if (!skip) {
                std::cout << "    " << s.substr(i, j - i) << std::endl;
                return j + 1;
            }
            skip = false;
            break;
        default:
            skip = false;
            break;
        }
    }
    throw exception{s, i};
}

static std::string_view::size_type parse_object(const std::string_view s,
                                                const std::string_view::size_type i)
{
    std::cout << "object" << std::endl;
    const std::string_view::size_type n = s.size();
    std::string_view::size_type j = skip_spaces(s, i);
    if (j < n && s[j] == '}') {
        return j + 1;
    }
    while (j < n) {
        if (s[j] != '"') {
            break;
        }
        j = parse_string(s, j + 1);
        j = skip_spaces(s, j);
        if (j >= n || s[j] != ':') {
            break;
        }
        j = parse(s, j + 1);
        j = skip_spaces(s, j);
        if (j < n) {
            const char c = s[j];
            if (c == '}') {
                return j + 1;
            }
            if (c == ',') {
                j = skip_spaces(s, j + 1);
                continue;
            }
        }
        break;
    }
    throw exception{s, i};
}

static std::string_view::size_type parse(const std::string_view s,
                                         const std::string_view::size_type i)
{
    const std::string_view::size_type pos = skip_spaces(s, i);
    if (pos >= s.size()) {
        throw exception{s, i};
    }
    switch (get_type(s[pos])) {
    case type::array: return parse_array(s, pos + 1);
    case type::boolean: return parse_boolean(s, pos + 1);
    case type::null: return parse_null(s, pos + 1);
    case type::number: return parse_number(s, pos);
    case type::object: return parse_object(s, pos + 1);
    case type::string: return parse_string(s, pos + 1);
    default: throw exception{s, pos};
    }
}

}

namespace json {

value parse(const std::string_view s) {
    tmp::parse(s, 0);
    return value{true};
}

value parse(const char *s) {
    return parse(std::string_view{s});
}

value parse(const std::string &s) {
    return parse(std::string_view{s});
}

}

#endif
