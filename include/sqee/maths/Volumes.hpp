#pragma once

#include <array>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>

namespace sq { namespace maths {

//============================================================================//

struct Sphere
{
    Vec3F origin; // centre of the sphere
    float radius; // size of the sphere
};

struct Box
{
    Vec3F origin;  // centre of the shapes
    float radius;  // size of the sphere
    Vec3F extents; // size of the box
    QuatF basis;   // rotation of the box
};

struct Plane
{
    Vec3F normal; // normal of the plane
    float offset; // distance from origin
};

//============================================================================//

template <size_t N> using Planes = std::array<Plane, N>;

//============================================================================//

struct Frustum
{
    Sphere sphere;    // origin, radius
    Planes<5> planes; // near, L, R, B, T
};

//============================================================================//

inline Frustum make_frustum(Mat4F matrix, Vec3F origin, Vec3F direction, float radius)
{
    Frustum result;

    const Mat4F invMatrix = maths::inverse(matrix);
    const auto divide_by_w = [](Vec4F v) { return Vec3F(v) / v.w; };

    const Vec3F pointA = divide_by_w(invMatrix * Vec4F(-1.f, -1.f, 1.f, 1.f)); // LB
    const Vec3F pointB = divide_by_w(invMatrix * Vec4F(-1.f, +1.f, 1.f, 1.f)); // LT
    const Vec3F pointC = divide_by_w(invMatrix * Vec4F(+1.f, -1.f, 1.f, 1.f)); // RB
    const Vec3F pointD = divide_by_w(invMatrix * Vec4F(+1.f, +1.f, 1.f, 1.f)); // RT

    const Vec3F normalA = maths::normalize(maths::cross(origin - pointB, origin - pointA));
    const Vec3F normalB = maths::normalize(maths::cross(origin - pointC, origin - pointD));
    const Vec3F normalC = maths::normalize(maths::cross(origin - pointA, origin - pointC));
    const Vec3F normalD = maths::normalize(maths::cross(origin - pointD, origin - pointB));

    result.planes[1] = Plane { normalA, maths::dot(-normalA, origin) }; // L
    result.planes[2] = Plane { normalB, maths::dot(-normalB, origin) }; // R
    result.planes[3] = Plane { normalC, maths::dot(-normalC, origin) }; // B
    result.planes[4] = Plane { normalD, maths::dot(-normalD, origin) }; // T

    result.planes[0] = Plane { -direction, maths::dot(direction, origin) };

    result.sphere = Sphere { origin, radius };

    return result;
}

//============================================================================//

inline Planes<4> make_ortho_xy(Mat4F matrix)
{
    Planes<4> result;

    const Mat4F invMatrix = maths::inverse(matrix);
    const auto divide_by_w = [](Vec4F v) { return Vec3F(v) / v.w; };

    const Vec3F pointA = divide_by_w(invMatrix * Vec4F(-1.f,  0.f, 0.f, 1.f));
    const Vec3F pointB = divide_by_w(invMatrix * Vec4F(+1.f,  0.f, 0.f, 1.f));
    const Vec3F pointC = divide_by_w(invMatrix * Vec4F( 0.f, -1.f, 0.f, 1.f));
    const Vec3F pointD = divide_by_w(invMatrix * Vec4F( 0.f, +1.f, 0.f, 1.f));

    const Vec3F normalX = maths::normalize(Mat3F(invMatrix) * Vec3F(1.f, 0.f, 0.f));
    const Vec3F normalY = maths::normalize(Mat3F(invMatrix) * Vec3F(0.f, 1.f, 0.f));

    result[0] = Plane { -normalX, maths::dot(+normalX, pointA) }; // L
    result[1] = Plane { +normalX, maths::dot(-normalX, pointB) }; // R
    result[2] = Plane { -normalY, maths::dot(+normalY, pointC) }; // B
    result[3] = Plane { +normalY, maths::dot(-normalY, pointD) }; // T

    return result;
}

}} // namespace sq::maths
