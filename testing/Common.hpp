#pragma once

#include <sqee/core/Types.hpp>
#include <sqee/misc/DopClasses.hpp>

//============================================================================//

using namespace sq::coretypes;

namespace detail = sq::detail;
namespace dop = sq::dop;
namespace maths = sq::maths;

constexpr const float PI = 3.14159265358979323846f;

//============================================================================//

// Simpler but templated version of a Catch's Approx class
template <class T> struct Approx
{
    explicit Approx(T value) : value(value) {}
    Approx(const Approx& other) : value(other.value) {}

    static_assert (
        std::is_same_v<T, float> || std::is_same_v<T, Vec2F> || std::is_same_v<T, Vec3F> || std::is_same_v<T, Vec4F> ||
        std::is_same_v<T, Mat3F> || std::is_same_v<T, Mat34F> || std::is_same_v<T, Mat4F> || std::is_same_v<T, QuatF>,
        "Approx not implemented for T"
    );

    const T value;
};

// deduction guide so we don't need an approx(arg) helper
template<class T> Approx(T) -> Approx<T>;

// this operator is defined per type inside of Common.cpp
template <class T> bool operator==(T a, Approx<T> b);

// these can all be defined in terms of the same operator
template <class T> inline bool operator==(Approx<T> a, T b) { return operator==(b, a); }
template <class T> inline bool operator!=(T a, Approx<T> b) { return !operator==(a, b); }
template <class T> inline bool operator!=(Approx<T> a, T b) { return !operator==(b, a); }

//============================================================================//

template <class Type>
struct fmt::formatter<Approx<Type>> : fmt::formatter<Type>
{
    template <class FormatContext>
    auto format(const Approx<Type>& approx, FormatContext& ctx)
    {
        fmt::format_to(ctx.out(), "Approx(");
        formatter<Type>::format(approx.value, ctx);
        return fmt::format_to(ctx.out(), ")");
    }
};

//============================================================================//

#include <ostream>

// sqee no longer has overloads for operator<<, so we define them here

template <int S, class T>
inline std::ostream& operator<<(std::ostream& os, const maths::Vector<S, T>& arg)
{ return os << fmt::to_string(arg); }

template <int H, int W, class T>
inline std::ostream& operator<<(std::ostream& os, const maths::Matrix<H, W, T>& arg)
{ return os << fmt::to_string(arg); }

template <class T>
inline std::ostream& operator<<(std::ostream& os, const maths::Quaternion<T>& arg)
{ return os << fmt::to_string(arg); }

template <class T>
inline std::ostream& operator<<(std::ostream& os, const Approx<T>& arg)
{ return os << fmt::to_string(arg); }
