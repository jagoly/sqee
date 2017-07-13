#pragma once

#include <ostream>

#include <sqee/dop/Classes.hpp>
#include <sqee/dop/Functions.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

#include <sqee/misc/StringCast.hpp>

//============================================================================//

namespace dop = sq::dop;
namespace maths = sq::maths;

static constexpr float PI = 3.14159265358979323846f;

//============================================================================//

template <class T> struct Approx
{
    explicit Approx(T value) : value(value) {}
    Approx(const Approx& other) : value(other.value) {}

    static_assert
    (
        std::is_same_v<T, float> ||
        std::is_same_v<T, Vec2F> ||
        std::is_same_v<T, Vec3F> ||
        std::is_same_v<T, Vec4F> ||
        std::is_same_v<T, Mat3F> ||
        std::is_same_v<T, Mat34F> ||
        std::is_same_v<T, Mat4F> ||
        std::is_same_v<T, QuatF>,

        "not implemented for T"
    );

    const T value;
};

//============================================================================//

template <class T> bool operator==(T a, Approx<T> b);
template <class T> bool operator!=(T a, Approx<T> b);
template <class T> bool operator==(Approx<T> a, T b);
template <class T> bool operator!=(Approx<T> a, T b);

//============================================================================//

// helper function for type-deduced construction /////
template <class T> Approx<T> approx(T arg) { return Approx<T>(arg); }

//============================================================================//

template <class T> inline
std::ostream& operator<<(std::ostream& os, const Approx<T>& arg)
{
    return os << "Approx(" << sq::chai_string(arg.value) << ")";
}

template <> inline
std::ostream& operator<<(std::ostream& os, const Approx<float>& arg)
{
    return os << "Approx(" << std::to_string(arg.value) << ")";
}
