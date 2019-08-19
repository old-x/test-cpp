#ifndef ARGS_PROPERTIES_HPP
#define ARGS_PROPERTIES_HPP

#include <array>
#include <fstream>
#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include "value.hpp"

namespace args {

class properties final {
public:
    properties() = default;

    explicit properties(const std::string &file_name):
    _values{} {
        load(file_name);
    }

    explicit properties(std::istream &in):
    _values{} {
        load(in);
    }

    inline std::size_t size() const {
        return _values.size();
    }

    template <class type = value>
    std::optional<type> get(const std::string &name) {
        if (auto it = _values.find(name); it != _values.end()) {
            return it->second.get<type>();
        }
        return {};
    }

    void set(const std::string &name, const std::string &val) {
        if (auto [it, ok] = _values.insert(std::make_pair(name, val)); !ok) {
            it->second.set(val);
        }
    }

    bool load(const std::string &file_name) {
        std::ifstream in{file_name};
        return load(in);
    }

    bool load(std::istream &in) {
        _values.clear();
        if (!in) {
            return false;
        }
        buffer_t buffer;
        buffer.reserve(init_buffer_length);
        while (!in.eof()) {
            buffer.clear();
            auto name = read_name(in, buffer);
            if (!name) {
                continue;
            }
            if (!skip_separator(in)) {
                continue;
            }
            buffer.clear();
            auto val = read_value(in, buffer);
            if (!val) {
                continue;
            }
            _values.emplace(std::move(name.value()), std::move(val.value()));
        }
        return true;
    }

    bool save(const std::string &file_name) const {
        std::ofstream out{file_name};
        return save(out);
    }

    bool save(std::ostream &out) const {
        if (!out) {
            return false;
        }
        for (const auto &[k, v] : _values) {
            auto finish = find_space(k);
            if (finish == std::string::npos) {
                out << k << equals_symbol << v << line_feed_symbol;
            } else {
                decltype(finish) start = 0;
                do {
                    write(out, k.data() + start, finish - start) << backslash_symbol;
                    start = finish;
                } while ((finish = find_space(k, finish + 1)) != std::string::npos);
                write(out, k.data() + start, k.size() - start) << equals_symbol << v << line_feed_symbol;
            }
        }
        return out.good();
    }

private:
    static constexpr char equals_symbol = '=';
    static constexpr char colon_symbol = ':';
    static constexpr char line_feed_symbol = '\n';
    static constexpr char carriage_return_symbol = '\r';
    static constexpr char backslash_symbol = '\\';
    static constexpr char hash_symbol = '#';
    static constexpr char exclamation_mark_symbol = '!';
    static constexpr char space_symbol = ' ';
    static constexpr char hex_symbol = 'x';
    static constexpr std::size_t init_buffer_length = 128;

    using buffer_t = std::vector<std::string::value_type>;

    std::unordered_map<std::string, value> _values;

    static constexpr char hex_to_dec(const char hex) noexcept {
        if (hex >= '0' && hex <= '9') {
            return hex - '0';
        }
        if (hex >= 'a' && hex <= 'f') {
            return 10 + hex - 'a';
        }
        if (hex >= 'A' && hex <= 'F') {
            return 10 + hex - 'A';
        }
        return 0;
    }

    static constexpr bool is_eof(const std::istream::int_type symbol) noexcept {
        return symbol == std::istream::traits_type::eof();
    }

    static constexpr bool is_endl(const std::istream::int_type symbol) noexcept {
        return symbol == line_feed_symbol || symbol == carriage_return_symbol;
    }

    static constexpr bool not_is_endl(const std::istream::int_type symbol) noexcept {
        return !is_endl(symbol);
    }

    static constexpr bool is_separator(const std::istream::int_type symbol) noexcept {
        return symbol == equals_symbol || symbol == colon_symbol;
    }

    static constexpr bool is_comment(const std::istream::int_type symbol) noexcept {
        return symbol == hash_symbol || symbol == exclamation_mark_symbol;
    }

    static inline bool is_space_or_endl(const std::istream::int_type symbol) noexcept {
        return std::isspace(static_cast<unsigned char>(symbol)) != 0;
    }

    static inline bool not_is_space_or_endl(const std::istream::int_type symbol) noexcept {
        return !is_space_or_endl(symbol);
    }

    static inline bool is_space(const std::istream::int_type symbol) noexcept {
        return is_space_or_endl(symbol) && not_is_endl(symbol);
    }

    static inline bool not_is_space(const std::istream::int_type symbol) noexcept {
        return !is_space(symbol);
    }

    static inline bool is_hex_digit(const std::istream::int_type symbol) noexcept {
        return std::isxdigit(static_cast<unsigned char>(symbol)) != 0;
    }

    static inline std::ostream &write(std::ostream &out, const char *data, const std::string::size_type size) {
        return out.write(data, static_cast<std::streamsize>(size));
    }

    static std::string::size_type find_space(const std::string &val, const std::string::size_type pos = 0) {
        for (std::string::size_type i = pos, n = val.size(); i < n; ++i) {
            if (is_space(val[i])) {
                return i;
            }
        }
        return std::string::npos;
    }

    static bool skip_spaces(std::istream &in) {
        return skip(in, not_is_space, true);
    }

    static bool skip_spaces_and_endl(std::istream &in) {
        return skip(in, not_is_space_or_endl, true);
    }

    static bool skip_endl(std::istream &in) {
        return skip(in, not_is_endl, true);
    }

    static bool skip_line(std::istream &in) {
        if (skip(in, is_endl, false)) {
            return skip_endl(in);
        }
        return false;
    }

    static bool skip_separator(std::istream &in) {
        return skip(in, is_separator, false);
    }

    template <typename predicate>
    static bool skip(std::istream &in, predicate &&pred, const bool peek) {
        while (!in.eof()) {
            const auto symbol = peek ? in.peek() : in.get();
            if (is_eof(symbol)) {
                return false;
            }
            if (pred(symbol)) {
                return true;
            }
            if (peek) {
                in.get();
            }
        }
        return false;
    }

    static inline void put_symbol(std::istream &in, buffer_t &buffer) {
        char symbol = 0;
        in.get(symbol);
        buffer.push_back(symbol);
    }

    static std::optional<std::string> read_name(std::istream &in, buffer_t &buffer) {
        if (!skip_spaces(in)) {
            return {};
        }
        while (!in.eof()) {
            const auto symbol = in.peek();
            if (is_eof(symbol)) {
                break;
            }
            if (is_comment(symbol)) {
                skip_line(in);
                return {};
            }
            if (symbol == backslash_symbol) {
                in.get();
                const auto next = in.peek();
                if (is_eof(next)) {
                    break;
                }
                if (is_endl(next)) {
                    skip_spaces_and_endl(in);
                } else if (next == hex_symbol) {
                    auto hex = read_hex(in);
                    if (hex) {
                        buffer.push_back(hex.value());
                    } else {
                        buffer.push_back(backslash_symbol);
                    }
                } else {
                    if (next != space_symbol) {
                        buffer.push_back(backslash_symbol);
                    }
                    put_symbol(in, buffer);
                }
                continue;
            }
            if (is_space_or_endl(symbol) || is_separator(symbol)) {
                break;
            }
            put_symbol(in, buffer);
        }
        return std::string{buffer.data(), buffer.size()};
    }

    static std::optional<std::string> read_value(std::istream &in, buffer_t &buffer) {
        if (!skip_spaces(in)) {
            return {};
        }
        while (!in.eof()) {
            char symbol = 0;
            in.get(symbol);
            if (is_endl(symbol)) {
                skip_spaces_and_endl(in);
                break;
            }
            if (symbol == backslash_symbol) {
                const auto next = in.peek();
                if (is_endl(next)) {
                    skip_spaces_and_endl(in);
                    continue;
                }
                if (next == hex_symbol) {
                    auto hex = read_hex(in);
                    if (hex) {
                        buffer.push_back(hex.value());
                        continue;
                    }
                }
            }
            buffer.push_back(symbol);
        }
        return std::string{buffer.data(), buffer.size()};
    }

    static std::optional<char> read_hex(std::istream &in) {
        std::array<char, 3> buf;
        if (!read_chars(in, buf)) {
            return {};
        }
        if (buf[0] != hex_symbol || !is_hex_digit(buf[1]) || !is_hex_digit(buf[2])) {
            for (std::size_t i = 0, n = buf.size(); i < n; ++i) {
                in.unget();
            };
            return {};
        }
        return ((hex_to_dec(buf[1]) << 4) & 0xF0) | (hex_to_dec(buf[2]) & 0x0F);
    }

    template <std::size_t n>
    static bool read_chars(std::istream &in, std::array<char, n> &buf) {
        for (std::size_t i = 0; i < n; ++i) {
            if (in.get(buf[i]).eof()) {
                for (std::size_t j = 0; j < i; ++j) {
                    in.unget();
                }
                return false;
            }
        }
        return true;
    }
};

}

#endif
