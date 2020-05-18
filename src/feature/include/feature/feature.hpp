#ifndef FEATURE_HPP
#define FEATURE_HPP

#include "detector.hpp"
#include "global_detector.hpp"

#define declare_feature_method(name, method) \
template <typename type, typename ...args> \
using name = decltype(std::declval<type>().method(std::declval<args>()...))

#define declare_feature_field(name, field) \
template <typename type, typename...> \
using name = decltype(type::field)

#define declare_feature_method_global(name, method) \
template <typename ...types> \
using name = decltype(method(std::declval<types>()...))

#endif
