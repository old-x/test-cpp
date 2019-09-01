#ifndef BASE64_DECODER_HPP
#define BASE64_DECODER_HPP

#include <iomanip>
#include <iterator>
#include <sstream>
#include <stdexcept>

#include "symbols.hpp"

namespace base64 {

template <char c62, char c63>
class decoder {
public:
    static constexpr std::size_t get_size(const std::size_t size) {
        const auto rem = size % 4;
        switch (rem) {
        case 0:
            return (size >> 2) * 3;
        case 1:
            return (size >> 2) * 3 + 1;
        default:
            return (size >> 2) * 3 + rem - 1;
        }
    }

    template <class input, class output>
    void operator()(const input &in, output &out) const {
        auto end = in.end();
        skip_padding(in.begin(), end);
        const std::size_t size = in.size() - std::distance(end, in.end());
        out.resize(get_size(size));
        operator()(in.begin(), end, out.begin());
    }

    template <class in_iterator, class out_iterator>
    out_iterator operator()(in_iterator begin, in_iterator end, out_iterator out) const {
        skip_padding(begin, end);
        const in_iterator start = begin;
        converter conv{out};
        while (begin != end) {
            if (!conv(*begin)) {
                throw create_error(*begin, std::distance(start, begin));
            }
            ++begin;
        }
        return out;
    }

private:
    static constexpr std::size_t table_size = 256;
    static constexpr std::uint8_t invalid_value = 255;

    using table_t = std::array<std::uint8_t, table_size>;

    static constexpr table_t table = []() constexpr {
        table_t result = {};
        for (std::uint8_t &v : result) {
            v = invalid_value;
        }
        for (std::size_t i = 0; i < symbols::size; ++i) {
            result[symbols::table<c62, c63>[i]] = i;
        }
        return result;
    }();

    template <class iterator>
    static constexpr void skip_padding(const iterator begin, iterator &end) {
        while (begin != end) {
            const auto prev = std::prev(end);
            if (*prev == symbols::padding_symbol) {
                end = prev;
            } else {
                break;
            }
        }
    }

    template <typename symbol>
    static std::invalid_argument create_error(const symbol value, const std::size_t position) {
        std::ostringstream err;
        err << "Invalid symbol: 0x" << std::hex << std::setw(2) << std::setfill('0');
        err << static_cast<int>(value) << " [" << value << "] at position " << position;
        return std::invalid_argument{err.str()};
    }

    template <class output>
    class converter {
    public:
        converter(output &out):
        _out{out},
        _stage{stage::first},
        _value{0} {

        }

        ~converter() {
            if (_stage == stage::second) {
                *_out++ = (_value << symbols::value_empty_bits) & symbols::value_mask_hi;
            }
        }

        template <typename symbol>
        bool operator()(const symbol s) {
            const std::uint8_t value = table[s];
            if (value == invalid_value) {
                return false;
            }
            _stage = process(value);
            return true;
        }

    private:
        enum class stage: std::uint8_t {
            first, second, third, fourth
        };

        static constexpr std::uint8_t get_value(const std::uint8_t value, const std::uint8_t value_mask,
                                                const std::uint8_t shift, const std::uint8_t mask)
        {
            return ((value & value_mask) >> shift) & mask;
        }

        stage process(const std::uint8_t value) {
            switch (_stage) {
            case stage::first:
                return process_first(value);
            case stage::second:
                return process_second(value);
            case stage::third:
                return process_third(value);
            case stage::fourth:
                return process_fourth(value);
            default:
                return stage::first;
            }
        }

        stage process_first(const std::uint8_t value) {
            _value = value;
            return stage::second;
        }

        stage process_second(const std::uint8_t value) {
            const std::uint8_t symbol =
                get_value(symbols::value_empty_bits, symbols::value_mask_hi) |
                get_value(
                    value, symbols::two_bits_mask_md, symbols::half_byte, symbols::two_bits_mask_lo
                );
            *_out++ = symbol;
            _value = value & symbols::half_byte_mask_lo;
            return stage::third;
        }

        stage process_third(const std::uint8_t value) {
            const std::uint8_t symbol =
                get_value(symbols::half_byte, symbols::half_byte_mask_hi) |
                get_value(
                    value, symbols::half_byte_mask_md, symbols::value_empty_bits, symbols::half_byte_mask_lo
                );
            *_out++ = symbol;
            _value = value & symbols::two_bits_mask_lo;
            return stage::fourth;
        }

        stage process_fourth(const std::uint8_t value) {
            const std::uint8_t symbol = get_value(symbols::value_bits, symbols::two_bits_mask_hi) | value;
            *_out++ = symbol;
            return stage::first;
        }

        std::uint8_t get_value(const std::uint8_t shift, const std::uint8_t mask) const {
            return (_value << shift) & mask;
        }

        output &_out;
        stage _stage;
        std::uint8_t _value;
    };
};

using decoder_default = decoder<symbols::symbol62_default, symbols::symbol63_default>;
using decoder_url = decoder<symbols::symbol62_url, symbols::symbol63_url>;

}

#endif
