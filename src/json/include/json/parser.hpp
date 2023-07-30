#ifndef JSON_PARSER_HPP
#define JSON_PARSER_HPP

#include <string_view>

#include "value.hpp"

namespace json {

value parse(std::string_view s) {

}

value parse(const char *s) {
    return parse(std::string_view{s});
}

value parse(const std::string &s) {
    return parse(std::string_view{s});
}

}

#endif
