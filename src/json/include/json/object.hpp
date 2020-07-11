#ifndef JSON_OBJECT_HPP
#define JSON_OBJECT_HPP

#include <ostream>
#include <string>
#include <unordered_map>
#include <utility>

#include "utils.hpp"

namespace json::tmp {

template <typename value>
class object final {
public:
    object(): pairs{new pairs_t{}} {}

    object(const object &o): object{} {
        *pairs = *o.pairs;
    }

    object(object &&o): pairs{std::exchange(o.pairs, nullptr)} {}

    template <typename ...types>
    object(types &&...args): object{} {
        pairs->reserve(sizeof...(types) >> 1);
        add(std::forward<types>(args)...);
    }

    ~object() {
        delete pairs;
    }

    object &operator=(const object &o) {
        object tmp{o};
        std::swap(pairs, tmp.pairs);
        return *this;
    }

    object &operator=(object &&o) {
        object tmp{std::move(o)};
        std::swap(pairs, tmp.pairs);
        return *this;
    }

    inline auto size() const {
        return pairs->size();
    }

    inline const value *get(const std::string name) const {
        auto it = pairs->find(name);
        return it != pairs->end() ? &it->second : nullptr;
    }

    inline value *get(const std::string name) {
        auto it = pairs->find(name);
        return it != pairs->end() ? &it->second : nullptr;
    }

    inline auto begin() const {
        return pairs->begin();
    }

    inline auto begin() {
        return pairs->begin();
    }

    inline auto end() const {
        return pairs->end();
    }

    inline auto end() {
        return pairs->end();
    }

    inline object &put(std::string name, const value &&v) {
        pairs->insert_or_assign(std::move(name), v);
        return *this;
    }

    inline object &put(std::string name, value &&v) {
        pairs->insert_or_assign(std::move(name), std::move(v));
        return *this;
    }

    inline void print(std::ostream &out) const {
        print(out, utils::printer{});
    }

    inline void pretty_print(std::ostream &out, const std::size_t indent = 0) const {
        print(out, utils::pretty_printer{indent});
    }

private:
    using pairs_t = std::unordered_map<std::string, value>;
    pairs_t *pairs;

    template <typename name, typename val, typename ...types>
    void add(name &&n, val &&v, types &&...args) {
        pairs->insert_or_assign(std::forward<name>(n), std::forward<val>(v));
        add(std::forward<types>(args)...);
    }

    void add() {
    }

    template <typename printer>
    void print(std::ostream &out, const printer p) const {
        if (pairs->empty()) {
            p.print_empty_object(out);
        } else {
            auto it = pairs->begin();
            p.print_object_opening(out);
            p.print_key_value(out, it->first, it->second);
            ++it;
            for (auto end = pairs->end(); it != end; ++it) {
                p.print_values_separator(out);
                p.print_key_value(out, it->first, it->second);
            }
            p.print_object_closing(out);
        }
    }
};

}

#endif
