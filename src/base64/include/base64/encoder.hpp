#ifndef BASE64_ENCODER_HPP
#define BASE64_ENCODER_HPP

#include "symbols.hpp"

namespace base64 {

template <char c62, char c63>
class encoder {
public:
    static encoder with_padding() {
        return encoder{true};
    }

    static encoder without_padding() {
        return encoder{false};
    }

    std::size_t get_size(const std::size_t size) const {
        const auto rem = size % 3;
        if (rem == 0) {
            return (size << 2) / 3;
        }
        const auto quot = size / 3;
        return _with_padding ? ((quot + 1) << 2) : ((quot << 2) + rem + 1);
    }

    template <class container>
    std::string operator()(const container &input) const {
        std::string result;
        result.resize(get_size(input.size()));
        operator()(input.begin(), input.end(), result.begin());
        return result;
    }

    template <class in_iterator, class out_iterator>
    out_iterator operator()(in_iterator begin, const in_iterator end, out_iterator out) const {
        converter conv{out, _with_padding};
        while (begin != end) {
            conv(*begin++);
        }
        return out;
    }

private:
    encoder(const bool with_padding):
    _with_padding{with_padding} {

    }

    static constexpr auto get_table_value(const std::uint8_t index) {
        return symbols::table<c62, c63>[index & symbols::value_mask_lo];
    }

    template <class output>
    class converter {
    public:
        converter(output &out, const bool with_padding):
        _out{out},
        _with_padding{with_padding},
        _stage{stage::first},
        _value{0} {

        }

        ~converter() {
            if (_stage == stage::first) {
                return;
            }
            process_first(_value);
            if (_with_padding) {
                *_out++ = symbols::padding_symbol;
                if (_stage == stage::second) {
                    *_out++ = symbols::padding_symbol;
                }
            }
        }

        void operator()(const std::uint8_t value) {
            _stage = process(value);
        }

    private:
        enum class stage: std::uint8_t {
            first, second, third
        };

        stage process(const std::uint8_t value) {
            switch (_stage) {
            case stage::first:
                return process_first(value);
            case stage::second:
                return process_second(value);
            case stage::third:
                return process_third(value);
            default:
                return stage::first;
            }
        }

        stage process_first(const std::uint8_t value) {
            const std::uint8_t index = (value & symbols::value_mask_hi) >> symbols::value_empty_bits;
            *_out++ = get_table_value(index);
            _value = (value << symbols::value_bits) & symbols::two_bits_mask_hi;
            return stage::second;
        }

        stage process_second(const std::uint8_t value) {
            const std::uint8_t index = get_index(
                value, symbols::half_byte_mask_hi, symbols::value_empty_bits
            );
            *_out++ = get_table_value(index);
            _value = (value << symbols::half_byte) & symbols::half_byte_mask_hi;
            return stage::third;
        }

        stage process_third(const std::uint8_t value) {
            const std::uint8_t index = get_index(
                value, symbols::two_bits_mask_hi, symbols::half_byte, symbols::value_empty_bits
            );
            *_out++ = get_table_value(index);
            *_out++ = get_table_value(value);
            return stage::first;
        }

        std::uint8_t get_index(const std::uint8_t value, const std::uint8_t mask, const std::uint8_t shift) {
            const std::uint8_t index = (value & mask) >> shift;
            return (index | _value) >> shift;
        }

        std::uint8_t get_index(const std::uint8_t value, const std::uint8_t mask,
                               const std::uint8_t shift, const std::uint8_t value_shift)
        {
            const std::uint8_t index = (value & mask) >> shift;
            return (index | _value) >> value_shift;
        }

        output &_out;
        bool _with_padding;
        stage _stage;
        std::uint8_t _value;
    };

    bool _with_padding;
};

using encoder_default = encoder<symbols::symbol62_default, symbols::symbol63_default>;
using encoder_url = encoder<symbols::symbol62_url, symbols::symbol63_url>;

}

#endif
