#ifndef JSON_UTILS_HPP
#define JSON_UTILS_HPP

#include <ostream>

namespace json::utils {

struct printer final {
    inline void print_empty_array(std::ostream &out) const {
        out << "[]";
    }

    inline void print_array_opening(std::ostream &out) const {
        out << '[';
    }

    inline void print_array_closing(std::ostream &out) const {
        out << ']';
    }

    inline void print_empty_object(std::ostream &out) const {
        out << "{}";
    }

    inline void print_object_opening(std::ostream &out) const {
        out << '{';
    }

    inline void print_object_closing(std::ostream &out) const {
        out << '}';
    }

    template <typename type>
    inline void print_value(std::ostream &out, const type &value) const {
        value.print(out);
    }

    template <typename key, typename value>
    inline void print_key_value(std::ostream &out, const key &k, const value &v) const {
        out << '"' << k << "\":";
        v.print(out);
    }

    inline void print_values_separator(std::ostream &out) const {
        out << ',';
    }
};

class pretty_printer final {
    static constexpr std::size_t default_indent = 4;
    static constexpr char eol = '\n';

public:
    pretty_printer(const std::size_t i): indent{i} {}

    inline void print_empty_array(std::ostream &out) const {
        print_array_opening(out);
        print_array_closing(out);
    }

    inline void print_array_opening(std::ostream &out) const {
        out << '[' << eol;
    }

    inline void print_array_closing(std::ostream &out) const {
        out << eol;
        print_indent(out);
        out << ']';
    }

    inline void print_empty_object(std::ostream &out) const {
        print_object_opening(out);
        print_object_closing(out);
    }

    inline void print_object_opening(std::ostream &out) const {
        out << '{' << eol;
    }

    inline void print_object_closing(std::ostream &out) const {
        out << eol;
        print_indent(out);
        out << '}';
    }

    template <typename type>
    inline void print_value(std::ostream &out, const type &value) const {
        print_value_indent(out);
        value.pretty_print(out, get_value_indent());
    }

    template <typename key, typename value>
    inline void print_key_value(std::ostream &out, const key &k, const value &v) const {
        print_value_indent(out);
        out << '"' << k << "\": ";
        v.pretty_print(out, get_value_indent());
    }

    inline void print_values_separator(std::ostream &out) const {
        out << ',' << eol;
    }

private:
    std::size_t indent;

    inline void print_indent(std::ostream &out) const {
        print_indent(out, indent);
    }

    inline void print_value_indent(std::ostream &out) const {
        print_indent(out, get_value_indent());
    }

    inline std::size_t get_value_indent() const {
        return indent + default_indent;
    }

    static inline void print_indent(std::ostream &out, const std::size_t indent) {
        for (std::size_t i = 0; i < indent; ++i) {
            out << ' ';
        }
    }
};

}

#endif
