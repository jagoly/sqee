#pragma once

#include <sqee/core/Types.hpp>
#include <sqee/maths/Approx.hpp>

#include <ostream>

//============================================================================//

using namespace sq::coretypes;
using sq::maths::Approx;

namespace detail = sq::detail;
namespace maths = sq::maths;

constexpr const float PI = 3.14159265358979323846f;

//============================================================================//

// sqee no longer has overloads for operator<<, so we define them here

namespace sq::maths {

template <int S, class T>
inline std::ostream& operator<<(std::ostream& os, const Vector<S, T>& arg)
{ return os << fmt::to_string(arg); }

template <int H, int W, class T>
inline std::ostream& operator<<(std::ostream& os, const Matrix<H, W, T>& arg)
{ return os << fmt::to_string(arg); }

template <class T>
inline std::ostream& operator<<(std::ostream& os, const Quaternion<T>& arg)
{ return os << fmt::to_string(arg); }

template <class T>
inline std::ostream& operator<<(std::ostream& os, const Approx<T>& arg)
{ return os << fmt::to_string(arg); }

} // namespace sq::maths
