#ifndef STORAGE_HPP
#define STORAGE_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

namespace storage {

namespace details {

enum class storage_type {
    heap,
    stack
};

template <typename type, storage_type>
class storage_impl;

template <typename type>
class storage_impl<type, storage_type::heap> {
public:
    template <typename ...args>
    constexpr storage_impl(args &&...values):
    _value{new type{std::forward<args>(values)...}} {

    }

    storage_impl(const storage_impl &rhs):
    _value{new type{*rhs._value}} {

    }

    storage_impl(storage_impl &&rhs) noexcept:
    _value{rhs._value} {
        rhs._value = nullptr;
    }

    ~storage_impl() {
        delete _value;
    }

    storage_impl &operator=(const storage_impl &rhs) {
        *_value = *rhs._value;
        return *this;
    }

    storage_impl &operator=(storage_impl &&rhs) noexcept {
        storage_impl tmp = std::move(rhs);
        std::swap(_value, tmp._value);
        return *this;
    }

    type &get() {
        return *_value;
    }

    const type &get() const {
        return *_value;
    }

private:
    type *_value;
};

template <typename type>
class storage_impl<type, storage_type::stack> {
public:
    template <typename ...args>
    constexpr storage_impl(args &&...values):
    _value{std::forward<args>(values)...} {

    }

    type &get() {
        return _value;
    }

    const type &get() const {
        return _value;
    }

private:
    type _value;
};

}

template <typename type, std::size_t size = 64>
class storage {
    static constexpr details::storage_type storage_type = sizeof(type) > size
        ? details::storage_type::heap
        : details::storage_type::stack;
public:
    template <typename ...args>
    constexpr storage(args &&...values):
    _storage_impl{std::forward<args>(values)...} {

    }

    storage(storage &rhs):
    storage{const_cast<const storage&>(rhs)} {

    }

    storage &operator=(storage &rhs) {
        return *this = const_cast<const storage&>(rhs);
    }

    storage(const storage&) = default;
    storage(storage&&) = default;
    storage &operator=(const storage&) = default;
    storage &operator=(storage&&) = default;

    type &get() {
        return _storage_impl.get();
    }

    const type &get() const {
        return _storage_impl.get();
    }

private:
    details::storage_impl<type, storage_type> _storage_impl;
};

}

#endif
