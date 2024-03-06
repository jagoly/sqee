#pragma once

#include <sqee/setup.hpp>

#include <sqee/core/TypeAliases.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq::maths { //########################################################

struct Plane
{
    Vec3F normal; // normal of the plane
    float offset; // distance from origin
};

struct Ray
{
    Vec3F direction; // direction of the ray
    Vec3F origin;    // origin point of the ray
};

struct Sphere
{
    Vec3F origin; // centre of the sphere
    float radius; // size of the sphere
};

struct Capsule
{
    Vec3F originA; // first origin point
    Vec3F originB; // second origin point
    float radius;  // radius of cylinder
};

struct Cone
{
    Vec3F origin;    // apex of the cone
    float length;    // length of the cone
    Vec3F direction; // from apex to base
    float radius;    // radius of cone base
};

struct Box
{
    Vec3F origin;  // centre of the shapes
    float radius;  // size of the sphere
    Vec3F extents; // size of the box
    Mat3F basis;   // orientation matrix
};

struct Ortho2D
{
    Vec3F normalX;  // x axis normal
    Vec3F normalY;  // y axis normal
    Vec2F offsetsX; // x axis offsets
    Vec2F offsetsY; // y axis offsets
    Plane planeZ;   // z far plane
};

struct Frustum
{
    Sphere sphere;   // origin, radius
    Plane planes[5]; // near, L, R, B, T
};

//==============================================================================

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

//==============================================================================

/// Check if a point is inside of a @ref Sphere.
///
/// @param point the point to check
/// @param sphere the sphere to check against
/// @param scale multiply sphere radius
///
/// @result true if point is inside of sphere

inline bool point_in_sphere(Vec3F point, Sphere sphere, float scale = 1.f)
{
    const float dist = maths::distance(point, sphere.origin);
    const float radius = sphere.radius * scale;

    return dist < radius;
}

//==============================================================================

/// Check if a point is inside of a @ref Cone.
///
/// @param point the point to check
/// @param cone the cone to check against
/// @param scale multiply cone radius
///
/// @result true if point is inside of cone

inline bool point_in_cone(Vec3F point, Cone cone, float scale = 1.f)
{
    const float axisDot = maths::dot(point - cone.origin, cone.direction);

    if (axisDot < 0.f || axisDot > cone.length) return false;

    const float circleDist = maths::length((point - cone.origin) - cone.direction * axisDot);
    const float circleRadius = (axisDot / cone.length) * cone.radius * scale;

    return circleDist < circleRadius;
}

//==============================================================================

/// Check if a point is inside of a @ref Plane.
///
/// @param point the point to check
/// @param plane the plane to check against
///
/// @result true if point is inside of plane

inline bool point_in_plane(Vec3F point, Plane plane)
{
    return maths::dot(point, plane.normal) < plane.offset;
}

//==============================================================================

/// Check if a point is inside of an @ref Ortho2D.
///
/// @param point the point to check
/// @param ortho the ortho to check against
///
/// @result true if point is inside of ortho

inline bool point_in_ortho(Vec3F point, Ortho2D ortho)
{
    const float dotX = maths::dot(point, ortho.normalX);
    const float dotY = maths::dot(point, ortho.normalY);

    if (+dotX >= ortho.offsetsX[0] || -dotX >= ortho.offsetsX[1]) return false;
    if (+dotY >= ortho.offsetsY[0] || -dotY >= ortho.offsetsY[1]) return false;

    return true;
}

} // namespace sq::maths #######################################################
