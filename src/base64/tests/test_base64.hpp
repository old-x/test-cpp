#ifndef TEST_BASE64_HPP
#define TEST_BASE64_HPP

#include <cassert>

#include <algorithm>
#include <string>
#include <string_view>

#include "base64/base64.hpp"

namespace base64::test {
static constexpr std::string_view decoded = std::string_view{
    "\x00\x01\x02\x03\x04\x05\x06\x07\x08\x09\x0A\x0B\x0C\x0D\x0E\x0F"
    "\x10\x11\x12\x13\x14\x15\x16\x17\x18\x19\x1A\x1B\x1C\x1D\x1E\x1F"
    "\x20\x21\x22\x23\x24\x25\x26\x27\x28\x29\x2A\x2B\x2C\x2D\x2E\x2F"
    "\x30\x31\x32\x33\x34\x35\x36\x37\x38\x39\x3A\x3B\x3C\x3D\x3E\x3F"
    "\x40\x41\x42\x43\x44\x45\x46\x47\x48\x49\x4A\x4B\x4C\x4D\x4E\x4F"
    "\x50\x51\x52\x53\x54\x55\x56\x57\x58\x59\x5A\x5B\x5C\x5D\x5E\x5F"
    "\x60\x61\x62\x63\x64\x65\x66\x67\x68\x69\x6A\x6B\x6C\x6D\x6E\x6F"
    "\x70\x71\x72\x73\x74\x75\x76\x77\x78\x79\x7A\x7B\x7C\x7D\x7E\x7F"
    "\x80\x81\x82\x83\x84\x85\x86\x87\x88\x89\x8A\x8B\x8C\x8D\x8E\x8F"
    "\x90\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9A\x9B\x9C\x9D\x9E\x9F"
    "\xA0\xA1\xA2\xA3\xA4\xA5\xA6\xA7\xA8\xA9\xAA\xAB\xAC\xAD\xAE\xAF"
    "\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xB8\xB9\xBA\xBB\xBC\xBD\xBE\xBF"
    "\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC7\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF"
    "\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF"
    "\xE0\xE1\xE2\xE3\xE4\xE5\xE6\xE7\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF"
    "\xF0\xF1\xF2\xF3\xF4\xF5\xF6\xF7\xF8\xF9\xFA\xFB\xFC\xFD\xFE\xFF", 256
};

static constexpr std::string_view encoded_with_padding =
    "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v"
    "MDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5f"
    "YGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P"
    "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/"
    "wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v"
    "8PHy8/T19vf4+fr7/P3+/w==";

static constexpr std::string_view encoded_without_padding =
    "AAECAwQFBgcICQoLDA0ODxAREhMUFRYXGBkaGxwdHh8gISIjJCUmJygpKissLS4v"
    "MDEyMzQ1Njc4OTo7PD0+P0BBQkNERUZHSElKS0xNTk9QUVJTVFVWV1hZWltcXV5f"
    "YGFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6e3x9fn+AgYKDhIWGh4iJiouMjY6P"
    "kJGSk5SVlpeYmZqbnJ2en6ChoqOkpaanqKmqq6ytrq+wsbKztLW2t7i5uru8vb6/"
    "wMHCw8TFxsfIycrLzM3Oz9DR0tPU1dbX2Nna29zd3t/g4eLj5OXm5+jp6uvs7e7v"
    "8PHy8/T19vf4+fr7/P3+/w";

static constexpr char default_to_url_char(const char c) {
    switch (c) {
    case '+':
        return '-';
    case '/':
        return '_';
    default:
        return c;
    }
}

static std::string default_to_url(const std::string_view &value) {
    std::string result;
    std::transform(
        value.begin(),
        value.end(),
        std::back_insert_iterator{result},
        default_to_url_char
    );
    return result;
}

}

void test_decoder() {
    assert(base64::decoder_default::get_size(0) == 0);
    assert(base64::decoder_default::get_size(1) == 1);
    assert(base64::decoder_default::get_size(2) == 1);
    assert(base64::decoder_default::get_size(3) == 2);
    assert(base64::decoder_default::get_size(4) == 3);
    assert(base64::decoder_default::get_size(5) == 4);
    assert(base64::decoder_default::get_size(6) == 4);
    assert(base64::decoder_default::get_size(7) == 5);
    assert(base64::decoder_default::get_size(8) == 6);
    assert(base64::decoder_default::get_size(9) == 7);

    base64::decoder_default decoder_default;

    {
        std::string output;
        decoder_default(base64::test::encoded_with_padding, output);
        assert(output == base64::test::decoded);
    }

    {
        std::string output;
        decoder_default(base64::test::encoded_without_padding, output);
        assert(output == base64::test::decoded);
    }
}

void test_encoder() {
    assert(base64::encoder_default::with_padding().get_size(0) == 0);
    assert(base64::encoder_default::with_padding().get_size(1) == 4);
    assert(base64::encoder_default::with_padding().get_size(2) == 4);
    assert(base64::encoder_default::with_padding().get_size(3) == 4);
    assert(base64::encoder_default::with_padding().get_size(4) == 8);
    assert(base64::encoder_default::with_padding().get_size(5) == 8);
    assert(base64::encoder_default::with_padding().get_size(6) == 8);
    assert(base64::encoder_default::with_padding().get_size(7) == 12);
    assert(base64::encoder_default::with_padding().get_size(8) == 12);
    assert(base64::encoder_default::with_padding().get_size(9) == 12);

    assert(base64::encoder_default::without_padding().get_size(0) == 0);
    assert(base64::encoder_default::without_padding().get_size(1) == 2);
    assert(base64::encoder_default::without_padding().get_size(2) == 3);
    assert(base64::encoder_default::without_padding().get_size(3) == 4);
    assert(base64::encoder_default::without_padding().get_size(4) == 6);
    assert(base64::encoder_default::without_padding().get_size(5) == 7);
    assert(base64::encoder_default::without_padding().get_size(6) == 8);
    assert(base64::encoder_default::without_padding().get_size(7) == 10);
    assert(base64::encoder_default::without_padding().get_size(8) == 11);
    assert(base64::encoder_default::without_padding().get_size(9) == 12);

    assert(
        base64::encoder_default::with_padding()(base64::test::decoded) ==
        base64::test::encoded_with_padding
    );

    assert(
        base64::encoder_default::without_padding()(base64::test::decoded) ==
        base64::test::encoded_without_padding
    );

    assert(
        base64::encoder_url::with_padding()(base64::test::decoded) ==
        base64::test::default_to_url(base64::test::encoded_with_padding)
    );

    assert(
        base64::encoder_url::without_padding()(base64::test::decoded) ==
        base64::test::default_to_url(base64::test::encoded_without_padding)
    );
}

#endif
