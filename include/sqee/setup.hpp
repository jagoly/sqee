#pragma once

#include <sqee/export.hpp> // IWYU pragma: export

#include <fmt/format.h> // IWYU pragma: export

#include <cctype> // IWYU pragma: export
#include <cfloat> // IWYU pragma: export
#include <climits> // IWYU pragma: export
#include <cmath> // IWYU pragma: export
#include <cstddef> // IWYU pragma: export
#include <cstdint> // IWYU pragma: export
#include <cstdio> // IWYU pragma: export
#include <cstdlib> // IWYU pragma: export
#include <cstring> // IWYU pragma: export

#include <algorithm> // IWYU pragma: export
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
#include <ranges> // IWYU pragma: export
#include <set> // IWYU pragma: export
#include <span> // IWYU pragma: export
#include <stdexcept> // IWYU pragma: export
#include <string> // IWYU pragma: export
#include <string_view> // IWYU pragma: export
#include <system_error> // IWYU pragma: export
#include <tuple> // IWYU pragma: export
#include <type_traits> // IWYU pragma: export
#include <unordered_map> // IWYU pragma: export
#include <unordered_set> // IWYU pragma: export
#include <utility> // IWYU pragma: export
#include <vector> // IWYU pragma: export

//==============================================================================

// without these, IWYU sometimes reccomends stddef.h
typedef std::ptrdiff_t ptrdiff_t;
typedef std::size_t size_t;

// sqee does not support 32bit platforms
static_assert(sizeof(int) == 4);
static_assert(sizeof(size_t) == 8);

// implementation defined, but true for all supported platforms
static_assert(sizeof(bool) == 1);

// these are not standard c++ so make sure they are defined
typedef unsigned char uchar;
typedef unsigned int uint;

// sometimes want to have this as a member variable
typedef unsigned int ImGuiID;

// mostly just to save on typing
#define SQEE_THROW(...) throw std::runtime_error(fmt::format(__VA_ARGS__))

// for reducing noise in class definitions
#define SQEE_COPY_DELETE(Class) Class(const Class&) = delete; Class& operator=(const Class&) = delete;
#define SQEE_MOVE_DELETE(Class) Class(Class&&) = delete; Class& operator=(Class&&) = delete;
#define SQEE_COPY_DEFAULT(Class) Class(const Class&) = default; Class& operator=(const Class&) = default;
#define SQEE_MOVE_DEFAULT(Class) Class(Class&&) = default; Class& operator=(Class&&) = default;

//==============================================================================

namespace sq {

/// Mathematical classes and functions.
namespace maths {}

/// A set of core types that get used everywhere.
namespace coretypes {}

namespace ranges { using namespace std::ranges; }
namespace views { using namespace std::ranges::views; }

} // namespace sq

//==============================================================================

/// @namespace sq
/// @brief The primary SQEE namespace.
