#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <cstdlib>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "value.hpp"

namespace args {

class options final {
public:
    options(int argc, char *argv[]):
    options{argc, const_cast<const char**>(argv)} {

    }

    options(const int argc, const char *argv[]):
    _application_name{},
    _positional_values{},
    _named_values{} {
        init(argc, argv);
    }

    inline const std::string &get_application_name() const {
        return _application_name;
    }

    inline std::size_t get_positional_size() const {
        return _positional_values.size();
    }

    inline std::size_t get_named_size() const {
        return _named_values.size();
    }

    template <class type = value>
    std::optional<type> get(const std::size_t index) const {
        if (index < _positional_values.size()) {
            return get<type>(_positional_values[index]);
        }
        return {};
    }

    template <class type = value>
    std::optional<type> get(const std::string &name) const {
        if (auto it = _named_values.find(name); it != _named_values.end()) {
            return get<type>(it->second);
        }
        return {};
    }

    template <class type = value>
    std::optional<type> get_env(const std::string &name) const {
        if (const char *val = std::getenv(name.c_str()); val) {
            return get<type>(value{val});
        }
        return {};
    }

private:
    static constexpr char dash_symbol = '-';
    static constexpr char equals_symbol = '=';
    static constexpr char space_symbol = ' ';

    static constexpr std::size_t short_option_length = 1;
    static constexpr std::size_t long_option_length = 2;

    using positional_values_t = std::vector<value>;
    using named_values_t = std::unordered_map<std::string, value>;

    std::string _application_name;
    positional_values_t _positional_values;
    named_values_t _named_values;

    template <class type>
    static constexpr bool is_bool = std::is_same_v<type, bool>;

    static constexpr bool is_short_option(const std::string_view &value) {
        return !value.empty() && value[0] == dash_symbol;
    }

    static constexpr bool is_long_option(const std::string_view &value) {
        return value.size() > 1 && value[0] == dash_symbol && value[1] == dash_symbol;
    }

    template <class type>
    static std::optional<type> get(const value &val) {
        if constexpr (is_bool<type>) {
            return val ? val.get<type>() : true;
        } else {
            return val.get<type>();
        }
    }

    void init(const int argc, const char *argv[]) {
        if (argc < 1) {
            return;
        }
        if (const char *arg = argv[0]; arg) {
            _application_name.assign(arg);
        }
        args_parser parser{this};
        for (int i = 1; i < argc; ++i) {
            if (const char *arg = argv[i]; arg) {
                parser(arg);
            }
        }
    }

    class args_parser {
    public:
        args_parser(options *opts):
        _positional_values{&opts->_positional_values},
        _named_values{&opts->_named_values},
        _it{_named_values->end()} {

        }

        void operator()(const std::string_view &arg) {
            if (is_long_option(arg)) {
                parse_long_option(arg.substr(long_option_length));
            } else if (is_short_option(arg)) {
                parse_short_option(arg.substr(short_option_length));
            } else {
                parse_positional_option(arg);
            }
        }

    private:
        positional_values_t *_positional_values;
        named_values_t *_named_values;
        named_values_t::iterator _it;

        inline void add_named_option(const std::string_view &name, const std::string_view &val) {
            auto [it, ok] = _named_values->emplace(name, value{val.data(), val.size()});
            _it = it;
            if (!ok && !val.empty()) {
                _it->second.add(val);
            }
        }

        void parse_long_option(const std::string_view &arg) {
            if (arg.empty()) {
                _it = _named_values->end();
                return;
            }
            const auto pos = arg.find(equals_symbol);
            if (pos == std::string_view::npos) {
                add_named_option(arg, std::string_view{});
                return;
            }
            add_named_option(arg.substr(0, pos), arg.substr(pos + 1));
        }

        void parse_short_option(const std::string_view &arg) {
            if (arg.empty()) {
                _it = _named_values->end();
                return;
            }
            if (arg.size() == 1) {
                add_named_option(arg, std::string_view{});
                return;
            }
            const auto pos = arg.find_first_not_of(space_symbol, 1);
            add_named_option(arg.substr(0, 1), arg.substr(pos == std::string_view::npos ? 1 : pos));
        }

        void parse_positional_option(const std::string_view &arg) {
            if (_it == _named_values->end()) {
                _positional_values->push_back(value{arg.data(), arg.size()});
            } else {
                _it->second.add(arg);
            }
        }
    };
};

}

#endif
