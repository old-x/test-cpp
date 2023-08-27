#ifndef JSON_EXCEPTION_HPP
#define JSON_EXCEPTION_HPP

#include <cstring>
#include <exception>
#include <string_view>

namespace json {

class exception: std::exception {
public:
    exception(const std::string_view s,
              const std::string_view::size_type i): std::exception{}
    {
        if (i < s.size()) {
            std::memcpy(str, prefix.data(), prefix.size());
            std::size_t begin = i < context_length ? 0 : i - context_length;
            std::size_t end = std::min(begin + context_length, s.size());
            std::size_t size = end - begin;
            std::size_t total_size = size;
            std::memcpy(str + prefix.size(), s.data() + begin, size);
            str[prefix.size() + size] = '*';
            begin = end;
            end = std::min(begin + context_length, s.size());
            size = end - begin;
            std::memcpy(str + prefix.size() + total_size, s.data() + begin, size);
            total_size += size;
            str[prefix.size() + total_size] = '\0';
        } else {
            std::memcpy(str, prefix.data(), prefix.size() - 1);
            str[prefix.size() - 1] = '\0';
        }
    }

    const char *what() const noexcept override {
        return str;
    }
private:
    static constexpr std::string_view prefix = "json::exception ";
    static constexpr std::size_t context_length = 30;
    static constexpr std::size_t str_size = prefix.size() + context_length * 2 + 2;

    char str[str_size];
};

}

#endif
