// Copyright(c) 2020 James Gangur
// Part of https://github.com/jagoly/sqee

#pragma once

#include <sqee/export.hpp> // IWYU pragma: export

#include <cctype> // IWYU pragma: export
#include <cfloat> // IWYU pragma: export
#include <climits> // IWYU pragma: export
#include <cmath> // IWYU pragma: export
#include <cstddef> // IWYU pragma: export
#include <cstdint> // IWYU pragma: export
#include <cstdio> // IWYU pragma: export
#include <cstdlib> // IWYU pragma: export
#include <cstring> // IWYU pragma: export

#include <array> // IWYU pragma: export
#include <exception> // IWYU pragma: export
#include <functional> // IWYU pragma: export
#include <initializer_list> // IWYU pragma: export
#include <iterator> // IWYU pragma: export
#include <limits> // IWYU pragma: export
#include <map> // IWYU pragma: export
#include <memory> // IWYU pragma: export
#include <numeric> // IWYU pragma: export
#include <optional> // IWYU pragma: export
#include <set> // IWYU pragma: export
#include <stdexcept> // IWYU pragma: export
#include <tuple> // IWYU pragma: export
#include <type_traits> // IWYU pragma: export
#include <typeindex> // IWYU pragma: export
#include <unordered_map> // IWYU pragma: export
#include <unordered_set> // IWYU pragma: export
#include <utility> // IWYU pragma: export
#include <variant> // IWYU pragma: export
#include <vector> // IWYU pragma: export

//============================================================================//

// without these, IWYU sometimes reccomends stddef.h
typedef std::ptrdiff_t ptrdiff_t;
typedef std::size_t size_t;

// sqee shouldn't make these assumptions, but right now it does
static_assert(sizeof(int)    == 4);
static_assert(sizeof(size_t) == 8);

// these are not standard c++ so make sure they are defined
typedef unsigned char uchar;
typedef unsigned int  uint;

// sometimes want to have this as a member variable
typedef unsigned int ImGuiID;

//============================================================================//

namespace sq {

//============================================================================//

/// Base for objects that can't be copied or moved.
struct NonCopyable
{
    NonCopyable() = default;

    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;

    NonCopyable(NonCopyable&&) = delete;
    NonCopyable& operator=(NonCopyable&&) = delete;
};

//----------------------------------------------------------------------------//

/// Base for objects that can be moved but not copied.
struct MoveOnly
{
    MoveOnly() = default;

    MoveOnly(const MoveOnly&) = delete;
    MoveOnly& operator=(const MoveOnly&) = delete;

    MoveOnly(MoveOnly&&) = default;
    MoveOnly& operator=(MoveOnly&&) = default;
};

//----------------------------------------------------------------------------//


/// Some very basic generic algorithms.
namespace algo {}

/// Utilities for data oriented programming.
namespace dop {}

/// Mathematical classes and functions.
namespace maths {}

/// OpenGL Core 4.5 constants and functions.
namespace gl {}

/// A set of core types that get used everywhere.
namespace coretypes {}

//============================================================================//

} // namespace sq

//============================================================================//

/// @namespace sq
/// @brief The primary SQEE namespace.
