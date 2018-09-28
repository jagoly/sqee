// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/setup.hpp>

#include <array>
#include <vector>
#include <optional>
#include <memory>
#include <string>
#include <string_view>

namespace sq {

template <class T, size_t N> using Array = std::array<T, N>;

template <class T> using Vector = std::vector<T>;
template <class T> using Optional  = std::optional<T>;
template <class T> using UniquePtr = std::unique_ptr<T>;

using String = std::string;
using StringView = std::string_view;

namespace builtins {

using sq::Array, sq::Vector, sq::Optional, sq::UniquePtr, sq::String, sq::StringView;

}} // namespace sq::builtins

#ifdef SQEE_GLOBAL_BUILTINS
using namespace sq::builtins;
#endif
