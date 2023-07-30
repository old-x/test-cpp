#ifndef TEST_JSON_HPP
#define TEST_JSON_HPP

#include <cassert>
#include <sstream>

#include "json/json.hpp"

template <typename type>
static std::string print(const type &value) {
    std::ostringstream out;
    value.print(out);
    return out.str();
}

template <typename type>
static std::string pretty_print(const type &value) {
    std::ostringstream out;
    value.pretty_print(out);
    return out.str();
}

void test_array() {
    json::array a{1, 2.5, "hello", nullptr, false};
    a.add(json::object{"name1", 123, "name2", "value2"})
        .add(json::array{5, "world"});
    assert(print(a) == R"([1,2.5,"hello",null,false,{"name2":"value2","name1":123},[5,"world"]])");
    assert(pretty_print(a) ==
R"([
    1,
    2.5,
    "hello",
    null,
    false,
    {
        "name2": "value2",
        "name1": 123
    },
    [
        5,
        "world"
    ]
])");
    assert(a.size() == 7);
    assert(!a.get(4).as_boolean());

    std::ostringstream out;
    for (const auto &e : a) {
        e.print(out << '|');
    }
    assert(out.str() == R"(|1|2.5|"hello"|null|false|{"name2":"value2","name1":123}|[5,"world"])");
    assert(print(json::array{}) == "[]");
    assert(pretty_print(json::array{}) == "[\n\n]");
}

void test_boolean() {
    json::boolean t = true;
    json::boolean f = false;

    assert(t);
    assert(!f);
    assert(print(t) == "true");
    assert(pretty_print(t) == "true");
    assert(print(f) == "false");
    assert(pretty_print(f) == "false");
}

void test_null() {
    json::null n;
    assert(print(n) == "null");
    assert(pretty_print(n) == "null");
}

void test_number() {
    {
        json::number n = -123;
        assert(n.to_long() == -123L);
        assert(n.to_double() == -123.0);
        assert(print(n) == "-123");
        assert(pretty_print(n) == "-123");
    }

    {
        json::number n = 123456789.12345;
        assert(n.to_long() == 123456789L);
        assert(n.to_double() == 123456789.12345);
        assert(print(n) == "123456789.12345");
        assert(pretty_print(n) == "123456789.12345");
    }
}

void test_object() {
    json::object o{"qwert", nullptr, "asdfg", "hello"};
    o.put("object", json::object{"name1", json::array{}, "name2", "value2"})
        .put("array", json::array{1, true, "name"});
    assert(print(o) == R"({"array":[1,true,"name"],"object":{"name2":"value2","name1":[]},"qwert":null,"asdfg":"hello"})");
    assert(pretty_print(o) ==
R"({
    "array": [
        1,
        true,
        "name"
    ],
    "object": {
        "name2": "value2",
        "name1": [

        ]
    },
    "qwert": null,
    "asdfg": "hello"
})");
    assert(o.get("object")->as_object().get("name2")->as_string().get_value() == "value2");

    std::ostringstream out;
    for (const auto &p : o) {
        out << p.first << '|';
    }
    assert(out.str() == "array|object|qwert|asdfg|");
    assert(print(json::object{}) == "{}");
    assert(pretty_print(json::object{}) == "{\n\n}");
}

void test_parser() {
    std::puts(__func__);
}

void test_string() {
    json::string s = "Hello, world!";
    assert(print(s) == R"("Hello, world!")");
    assert(pretty_print(s) == R"("Hello, world!")");
}

void test_value() {
    json::value v = "Test";
    assert(print(v) == R"("Test")");
    assert(pretty_print(v) == R"("Test")");
    assert(!v.is_array());
    assert(!v.is_boolean());
    assert(!v.is_null());
    assert(!v.is_number());
    assert(!v.is_object());
    assert(v.is_string());
    assert(v.as_string().get_value() == "Test");

    v = 777;
    assert(print(v) == "777");
    assert(pretty_print(v) == "777");
    assert(!v.is_array());
    assert(!v.is_boolean());
    assert(!v.is_null());
    assert(v.is_number());
    assert(!v.is_object());
    assert(!v.is_string());
    assert(v.as_number().to_long() == 777);
}

#endif
