#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#include <ostream>
#include <variant>

#include "array.hpp"
#include "boolean.hpp"
#include "null.hpp"
#include "number.hpp"
#include "object.hpp"
#include "string.hpp"

namespace json {

class value final {
public:
    using array = tmp::array<value>;
    using object = tmp::object<value>;

    value(const array &a): content{a} {}
    value(array &&a): content{std::move(a)} {}
    value(boolean b): content{b} {}
    value(bool b): content{boolean{b}} {}
    value(null n): content{n} {}
    value(std::nullptr_t): content{null{}} {}
    value(const number &n): content{n} {}
    value(number &&n): content{std::move(n)} {}
    value(int i): content{number{i}} {}
    value(long l): content{number{l}} {}
    value(double d): content{number{d}} {}
    value(const object &o): content{o} {}
    value(object &&o): content{std::move(o)} {}
    value(const string &s): content{s} {}
    value(string &&s): content{std::move(s)} {}
    value(const char *s): content{string{s}} {}

    inline bool is_array() const {
        return content.index() == 0;
    }

    inline bool is_boolean() const {
        return content.index() == 1;
    }

    inline bool is_null() const {
        return content.index() == 2;
    }

    inline bool is_number() const {
        return content.index() == 3;
    }

    inline bool is_object() const {
        return content.index() == 4;
    }

    inline bool is_string() const {
        return content.index() == 5;
    }

    inline array &as_array() {
        return std::get<array>(content);
    }

    inline boolean &as_boolean() {
        return std::get<boolean>(content);
    }

    inline null &as_null() {
        return std::get<null>(content);
    }

    inline number &as_number() {
        return std::get<number>(content);
    }

    inline object &as_object() {
        return std::get<object>(content);
    }

    inline string &as_string() {
        return std::get<string>(content);
    }

    inline void print(std::ostream &out) const {
        std::visit([&out](auto &&v) {
            v.print(out);
        }, content);
    }

    inline void pretty_print(std::ostream &out, const std::size_t indent = 0) const {
        std::visit([&out, indent](auto &&v) {
            v.pretty_print(out, indent);
        }, content);
    }

private:
    std::variant<array, boolean, null, number, object, string> content;
};

using array = value::array;
using object = value::object;

}

#endif
