#include "Common.hpp"

//============================================================================//

static constexpr float EPSILON = std::numeric_limits<float>::epsilon() * 8.f;

//============================================================================//

template<> bool operator==(float a, Approx<float> b)
{
    const float maxLength = maths::max(std::abs(a), std::abs(b.value));
    return std::abs(a - b.value) < EPSILON * (1.f + maxLength);
}

template<> bool operator==(Vec2F a, Approx<Vec2F> b)
{
    const float maxLength = maths::max(maths::length(a), maths::length(b.value));
    return maths::distance(a, b.value) < EPSILON * (1.f + maxLength);
}

template<> bool operator==(Vec3F a, Approx<Vec3F> b)
{
    const float maxLength = maths::max(maths::length(a), maths::length(b.value));
    return maths::distance(a, b.value) < EPSILON * (1.f + maxLength);
}

template<> bool operator==(Vec4F a, Approx<Vec4F> b)
{
    const float maxLength = maths::max(maths::length(a), maths::length(b.value));
    return maths::distance(a, b.value) < EPSILON * (1.f + maxLength);
}

template<> bool operator==(QuatF a, Approx<QuatF> b)
{
    Vec4F vecA(a.x, a.y, a.z, a.w), vecB(b.value.x, b.value.y, b.value.z, b.value.w);
    const float maxLength = maths::max(maths::length(a), maths::length(b.value));
    const float dist = maths::distance(vecA, vecB);
    return (std::abs(dist - 2.0f) < EPSILON) || (dist <= EPSILON * (1.f + maxLength));
}

template<> bool operator==(Mat3F a, Approx<Mat3F> b)
{
    return a[0] == Approx(b.value[0]) && a[1] == Approx(b.value[1]) && a[2] == Approx(b.value[2]);
}

template<> bool operator==(Mat34F a, Approx<Mat34F> b)
{
    return a[0] == Approx(b.value[0]) && a[1] == Approx(b.value[1]) && a[2] == Approx(b.value[2]);
}

template<> bool operator==(Mat4F a, Approx<Mat4F> b)
{
    return a[0] == Approx(b.value[0]) && a[1] == Approx(b.value[1]) && a[2] == Approx(b.value[2]) && a[3] == Approx(b.value[3]);
}

//============================================================================//

template<> bool operator==(Approx<Vec2F> a, Vec2F b) { return operator==(b, a); }
template<> bool operator!=(Vec2F a, Approx<Vec2F> b) { return !operator==(a, b); }
template<> bool operator!=(Approx<Vec2F> a, Vec2F b) { return !operator==(b, a); }

template<> bool operator==(Approx<Vec3F> a, Vec3F b) { return operator==(b, a); }
template<> bool operator!=(Vec3F a, Approx<Vec3F> b) { return !operator==(a, b); }
template<> bool operator!=(Approx<Vec3F> a, Vec3F b) { return !operator==(b, a); }

template<> bool operator==(Approx<Vec4F> a, Vec4F b) { return operator==(b, a); }
template<> bool operator!=(Vec4F a, Approx<Vec4F> b) { return !operator==(a, b); }
template<> bool operator!=(Approx<Vec4F> a, Vec4F b) { return !operator==(b, a); }

template<> bool operator==(Approx<QuatF> a, QuatF b) { return operator==(b, a); }
template<> bool operator!=(QuatF a, Approx<QuatF> b) { return !operator==(a, b); }
template<> bool operator!=(Approx<QuatF> a, QuatF b) { return !operator==(b, a); }

template<> bool operator==(Approx<Mat3F> a, Mat3F b) { return operator==(b, a); }
template<> bool operator!=(Mat3F a, Approx<Mat3F> b) { return !operator==(a, b); }
template<> bool operator!=(Approx<Mat3F> a, Mat3F b) { return !operator==(b, a); }

template<> bool operator==(Approx<Mat34F> a, Mat34F b) { return operator==(b, a); }
template<> bool operator!=(Mat34F a, Approx<Mat34F> b) { return !operator==(a, b); }
template<> bool operator!=(Approx<Mat34F> a, Mat34F b) { return !operator==(b, a); }

template<> bool operator==(Approx<Mat4F> a, Mat4F b) { return operator==(b, a); }
template<> bool operator!=(Mat4F a, Approx<Mat4F> b) { return !operator==(a, b); }
template<> bool operator!=(Approx<Mat4F> a, Mat4F b) { return !operator==(b, a); }
