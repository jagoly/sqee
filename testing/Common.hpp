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
        std::is_same<T, float>::value ||

        std::is_same<T, Vec2F>::value ||
        std::is_same<T, Vec3F>::value ||
        std::is_same<T, Vec4F>::value ||

        std::is_same<T, Mat2F>::value ||
        std::is_same<T, Mat23F>::value ||
        std::is_same<T, Mat3F>::value ||
        std::is_same<T, Mat34F>::value ||
        std::is_same<T, Mat4F>::value ||

        std::is_same<T, QuatF>::value,

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

//============================================================================//

namespace sq {

inline std::ostream& operator<<(std::ostream& os, const Vec2F& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec3F& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Vec4F& arg) { return os << sq::chai_string(arg); }

inline std::ostream& operator<<(std::ostream& os, const Mat2F& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Mat23F& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Mat3F& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Mat34F& arg) { return os << sq::chai_string(arg); }
inline std::ostream& operator<<(std::ostream& os, const Mat4F& arg) { return os << sq::chai_string(arg); }

inline std::ostream& operator<<(std::ostream& os, const QuatF& arg) { return os << sq::chai_string(arg); }

}
