#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <cstdlib>
#include <optional>
#include <ostream>
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
    _named_values{},
    _option_descriptions{} {
        init(argc, argv);
    }

    options &add_option(const std::string_view &name, const std::string_view &desc, const bool required) {
        const auto pos = name.find(comma_symbol);
        if (pos == std::string_view::npos) {
            _option_descriptions.emplace_back(name, desc, required);
        } else {
            _option_descriptions.emplace_back(name.substr(0, pos), name.substr(pos + 1), desc, required);
        }
        return *this;
    }

    void show_options(std::ostream &out) const {
        out << _application_name;
        if (_option_descriptions.empty()) {
            out << " [args...]" << endl_symbol;
            return;
        }
        for (const option_description &opt : _option_descriptions) {
            out << space_symbol;
            opt.show(out);
        }
        out << endl_symbol;
        for (const option_description &opt : _option_descriptions) {
            opt.show_full(out);
        }
    }

    bool validate(std::ostream &out) const {
        bool result = true;
        for (const option_description &opt : _option_descriptions) {
            if (!opt.validate(*this, out)) {
                result = false;
            }
        }
        return result;
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
    static constexpr char comma_symbol = ',';
    static constexpr char angle_bracket_open_symbol = '<';
    static constexpr char angle_bracket_close_symbol = '>';
    static constexpr char square_bracket_open_symbol = '[';
    static constexpr char square_bracket_close_symbol = ']';
    static constexpr char endl_symbol = '\n';

    static constexpr std::size_t short_option_length = 1;
    static constexpr std::size_t long_option_length = 2;

    using positional_values_t = std::vector<value>;
    using named_values_t = std::unordered_map<std::string, value>;

    class option_description {
    public:
        option_description(const std::string_view &name, const std::string_view &desc,
                           const bool required):
        option_description{name.size() > 1 ? name : "", name.size() == 1 ? name : "", desc, required} {

        }

        option_description(const std::string_view &long_name, const std::string_view &short_name,
                            const std::string_view &desc, const bool required):
        _long_name{long_name.data(), long_name.size()},
        _short_name{short_name.data(), short_name.size()},
        _description{desc.data(), desc.size()},
        _required{required} {

        }

        void show(std::ostream &out) const {
            show_name(out, true);
        }

        void show_full(std::ostream &out) const {
            show_name(out, false) << space_symbol << _description << space_symbol;
            if (_required) {
                out << angle_bracket_open_symbol << "required" << angle_bracket_close_symbol;
            } else {
                out << square_bracket_open_symbol << "optional" << square_bracket_close_symbol;
            }
            out << endl_symbol;
        }

        bool validate(const options &opts, std::ostream &out) const {
            if (!_required) {
                return true;
            }
            if (!_long_name.empty() && opts.get(_long_name)) {
                return true;
            }
            if (!_short_name.empty() && opts.get(_short_name)) {
                return true;
            }
            out << "Required: ";
            show_name(out, false);
            out << space_symbol << _description << endl_symbol;
            return false;
        }

    private:
        std::string _long_name;
        std::string _short_name;
        std::string _description;
        bool _required;

        std::ostream &show_name(std::ostream &out, const bool with_brackets) const {
            const bool is_short = !_short_name.empty();
            if (!_long_name.empty()) {
                out << std::string(long_option_length, dash_symbol);
                if (with_brackets) {
                    out << (_required ? angle_bracket_open_symbol : square_bracket_open_symbol);
                }
                out << _long_name;
                if (is_short) {
                    out << comma_symbol << _short_name;
                }
                if (with_brackets) {
                    out << (_required ? angle_bracket_close_symbol : square_bracket_close_symbol);
                }
            } else if (is_short) {
                out << std::string(short_option_length, dash_symbol);
                if (with_brackets) {
                    out << (_required ? angle_bracket_open_symbol : square_bracket_open_symbol);
                }
                out << _short_name;
                if (with_brackets) {
                    out << (_required ? angle_bracket_close_symbol : square_bracket_close_symbol);
                }
            }
            return out;
        }
    };

    std::string _application_name;
    positional_values_t _positional_values;
    named_values_t _named_values;
    std::vector<option_description> _option_descriptions;

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
