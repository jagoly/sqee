// Copyright(c) 2018 James Gangur
// Part of https://github.com/jagoly/sqee

#include <sqee/maths/Volumes.hpp>

namespace sq::maths {

//============================================================================//

/// Check for intersection between a @ref Sphere and a @ref Sphere.
///
/// @param first the sphere to check
/// @param second the sphere to check against
///
/// @result -1 if outside, 0 if intersects, +1 if inside

inline int8_t intersect_sphere_sphere(Sphere first, Sphere second)
{
    const float dist = maths::distance_squared(first.origin, second.origin);

    const float radiusSum = first.radius + second.radius;
    const float radiusDiff = first.radius - second.radius;

    // check if fully outside
    if (dist > radiusSum * radiusSum) return -1;

    // check if fully inside
    if (dist < radiusDiff * radiusDiff) return +1;

    return 0; // intersects
}

//============================================================================//

/// Check for intersection between a @ref Sphere and a @ref Capsule.
///
/// @param sphere the sphere to check
/// @param capsule the capsule to check against
///
/// @result -1 if outside, 0 if intersects, +1 if inside

inline int8_t intersect_sphere_capsule(Sphere sphere, Capsule capsule)
{
    const Vec3F lineVec = capsule.originB - capsule.originA;
    const float distOnLine = maths::dot(sphere.origin - capsule.originA, lineVec);
    const Vec3F closest = capsule.originA + lineVec * maths::clamp(distOnLine, 0.f, 1.f);

    const float dist = maths::distance_squared(sphere.origin, closest);

    const float radiusSum = sphere.radius + capsule.radius;
    const float radiusDiff = sphere.radius - capsule.radius;

    // check if fully outside
    if (dist > radiusSum * radiusSum) return -1;

    // check if fully inside
    if (dist < radiusDiff * radiusDiff) return +1;

    return 0; // intersects
}

//============================================================================//

/// Check for intersection between a @ref Sphere and an @ref Ortho2D.
///
/// @param sphere the sphere to check
/// @param ortho the ortho to check against
///
/// @result -1 if outside, 0 if intersects, +1 if inside

inline int8_t intersect_sphere_ortho(Sphere sphere, Ortho2D ortho)
{
    const float dotX = maths::dot(sphere.origin, ortho.normalX);
    const float dotY = maths::dot(sphere.origin, ortho.normalY);

    const float offsetX0 = +dotX + ortho.offsetsX[0];
    const float offsetX1 = -dotX + ortho.offsetsX[1];
    const float offsetY0 = +dotY + ortho.offsetsY[0];
    const float offsetY1 = -dotY + ortho.offsetsY[1];

    const float dotZ = maths::dot(sphere.origin, ortho.planeZ.normal);
    const float offsetZ = dotZ + ortho.planeZ.offset;

    // check if fully outside
    if (offsetX0 >= sphere.radius) return -1;
    if (offsetX1 >= sphere.radius) return -1;
    if (offsetY0 >= sphere.radius) return -1;
    if (offsetY1 >= sphere.radius) return -1;
    if (offsetZ >= sphere.radius) return -1;

    // check if intersecting
    if (offsetX0 > -sphere.radius) return 0;
    if (offsetX1 > -sphere.radius) return 0;
    if (offsetY0 > -sphere.radius) return 0;
    if (offsetY1 > -sphere.radius) return 0;
    if (offsetZ > -sphere.radius) return 0;

    return +1; // fully inside
}

//============================================================================//

/// Check if a @ref Sphere and a @ref Sphere do not intersect.
///
/// @param first the sphere to check
/// @param second the sphere to check against
///
/// @result true if object can be culled

inline bool cull_sphere_sphere(Sphere first, Sphere second)
{
    const float dist = maths::distance_squared(first.origin, second.origin);
    const float radiusSum = first.radius + second.radius;

    return dist > radiusSum * radiusSum;
}

//============================================================================//

/// Check if a @ref Box and a @ref Sphere do not intersect.
///
/// @param box the box to check
/// @param sphere the sphere to check against
///
/// @result true if object can be culled

inline bool cull_box_sphere(Box box, Sphere sphere)
{
    const int8_t sphereTest = intersect_sphere_sphere({box.origin, box.radius}, sphere);
    if (sphereTest != 0) return sphereTest < 0; // early exit if fully out or in

    const float boxOffsetX = maths::dot(box.basis[0], box.origin + (box.basis[0] * box.extents.x));
    const float boxOffsetY = maths::dot(box.basis[1], box.origin + (box.basis[1] * box.extents.y));
    const float boxOffsetZ = maths::dot(box.basis[2], box.origin + (box.basis[2] * box.extents.z));

    const float offsetX = maths::dot(sphere.origin, box.basis[0]) + boxOffsetX;
    const float offsetY = maths::dot(sphere.origin, box.basis[1]) + boxOffsetY;
    const float offsetZ = maths::dot(sphere.origin, box.basis[2]) + boxOffsetZ;

    if (offsetX < sphere.radius || offsetX > box.extents.x + box.extents.x + sphere.radius) return true;
    if (offsetY < sphere.radius || offsetY > box.extents.y + box.extents.y + sphere.radius) return true;
    if (offsetZ < sphere.radius || offsetZ > box.extents.z + box.extents.z + sphere.radius) return true;\

    return false;
}

//============================================================================//

/// Check if a @ref Sphere and an @ref Ortho2D do not intersect.
///
/// @param sphere the sphere to check
/// @param ortho the ortho to check against
///
/// @result true if object can be culled

inline bool cull_sphere_ortho(Sphere sphere, Ortho2D ortho)
{
    const float dotX = maths::dot(sphere.origin, ortho.normalX);
    const float dotY = maths::dot(sphere.origin, ortho.normalY);

    const float offsetZ = maths::dot(sphere.origin, ortho.planeZ.normal) + ortho.planeZ.offset;

    if (+dotX + ortho.offsetsX[0] >= sphere.radius) return true;
    if (-dotX + ortho.offsetsX[1] >= sphere.radius) return true;
    if (+dotY + ortho.offsetsY[0] >= sphere.radius) return true;
    if (-dotY + ortho.offsetsY[1] >= sphere.radius) return true;

    if (offsetZ >= sphere.radius) return true;

    return false;
}

//============================================================================//

/// Check if a @ref Box and an @ref Ortho2D do not intersect.
///
/// @param box the box to check
/// @param ortho the ortho to check against
///
/// @result true if object can be culled

inline bool cull_box_ortho(Box box, Ortho2D ortho)
{
    const int8_t sphereTest = intersect_sphere_ortho({box.origin, box.radius}, ortho);
    if (sphereTest != 0) return sphereTest < 0; // early exit if fully out or in

    //--------------------------------------------------------//

    const Vec3F sideOffsetX = box.basis[0] * box.extents.x;
    const Vec3F sideOffsetY = box.basis[1] * box.extents.y;
    const Vec3F sideOffsetZ = box.basis[2] * box.extents.z;

    const Vec3F points[8] =
    {
        box.origin - sideOffsetX - sideOffsetY - sideOffsetZ,
        box.origin - sideOffsetX - sideOffsetY + sideOffsetZ,
        box.origin - sideOffsetX + sideOffsetY - sideOffsetZ,
        box.origin - sideOffsetX + sideOffsetY + sideOffsetZ,
        box.origin + sideOffsetX - sideOffsetY - sideOffsetZ,
        box.origin + sideOffsetX - sideOffsetY + sideOffsetZ,
        box.origin + sideOffsetX + sideOffsetY - sideOffsetZ,
        box.origin + sideOffsetX + sideOffsetY + sideOffsetZ
    };

    //--------------------------------------------------------//

    uint8_t bitsPosX = 0u, bitsNegX = 0u;

    for (uint8_t i = 0u; i < 8u; ++i)
    {
        float pointOffset = maths::dot(points[i], ortho.normalX);
        bitsPosX |= (+pointOffset + ortho.offsetsX[0] < 0.f) << i;
        bitsNegX |= (-pointOffset + ortho.offsetsX[1] < 0.f) << i;
    }

    if (bitsPosX == 0u || bitsNegX == 0u) return true;

    //--------------------------------------------------------//

    uint8_t bitsPosY = 0u, bitsNegY = 0u;

    for (uint8_t i = 0u; i < 8u; ++i)
    {
        float pointOffset = maths::dot(points[i], ortho.normalY);
        bitsPosY |= (+pointOffset + ortho.offsetsY[0] < 0.f) << i;
        bitsNegY |= (-pointOffset + ortho.offsetsY[1] < 0.f) << i;
    }

    if (bitsPosX == 0u || bitsNegX == 0u) return true;

    //--------------------------------------------------------//

    uint8_t bitsFar = 0u;

    for (uint8_t i = 0u; i < 8u; ++i)
    {
        float pointOffset = maths::dot(points[i], ortho.planeZ.normal);
        bitsFar |= (pointOffset + ortho.planeZ.offset < 0.f) << i;
    }

    if (bitsFar == 0u) return true;

    //--------------------------------------------------------//

    return false;

    /* the rest of this is pointless, but I wrote it because I planned my time poorly
     * it's still got some useful snippets so I'm not deleting it yet

    //--------------------------------------------------------//

    const uint8_t andBits = bitsPosX & bitsNegX & bitsPosY & bitsPosY;

    // check if any points were inside of all planes
    if (andBits & 1<<0 || andBits & 1<<1 || andBits & 1<<2 || andBits & 1<<3) return false;
    if (andBits & 1<<4 || andBits & 1<<5 || andBits & 1<<6 || andBits & 1<<7) return false;

    //--------------------------------------------------------//

    // The rest of this function tests the ortho corner rays against
    // the box. This should only be reached for very large objects.

    const Mat3F invBasis = maths::transpose(box.basism);
    const Vec3F orthoNormalZ = maths::cross(ortho.normalX, ortho.normalY);
    const Vec3F invRayDirection = Vec3F(1.f) / (invBasis * orthoNormalZ);

    //--------------------------------------------------------//

    const auto ray_aabb_test = [&](Vec3F origin) -> bool
    {
        const Vec3F valuesA = (-box.extents - origin) * invRayDirection;
        const Vec3F valuesB = (+box.extents - origin) * invRayDirection;

        // the INFINITYs are needed to correctly handle edge cases with NANs
        const Vec3F minV = maths::min(maths::min(valuesA, valuesB), Vec3F(+INFINITY));
        const Vec3F maxV = maths::max(maths::max(valuesA, valuesB), Vec3F(-INFINITY));

        return maths::min(maxV.x, maxV.y, maxV.z) > maths::max(minV.x, minV.y, minV.z);
    };

    //--------------------------------------------------------//

    const Vec3F crossYZ = maths::cross(ortho.normalY, orthoNormalZ);
    const Vec3F crossZX = maths::cross(orthoNormalZ, ortho.normalX);

    Vec3F originPP = -(box.origin);
    originPP += crossYZ * (ortho.offset.x);
    originPP += crossZX * (ortho.offset.y);
    originPP = invBasis * originPP;

    if (ray_aabb_test(originPP) == true) return false;

    Vec3F originNN = -(box.origin);
    originNN -= crossYZ * (ortho.size.x - ortho.offset.x);
    originNN -= crossZX * (ortho.size.y - ortho.offset.y);
    originNN = invBasis * originNN;

    if (ray_aabb_test(originNN) == true) return false;

    //--------------------------------------------------------//

    const Vec3F crossZY = maths::cross(orthoNormalZ, ortho.normalY);
    const Vec3F crossXZ = maths::cross(ortho.normalX, orthoNormalZ);

    Vec3F originPN = -(box.origin);
    originPN += crossZY * (ortho.size.x - ortho.offset.x);
    originPN -= crossXZ * (ortho.offset.y);
    originPN = invBasis * originPN;

    if (ray_aabb_test(originPN) == true) return false;

    Vec3F originNP = -(box.origin);
    originNP -= crossZY * (ortho.offset.x);
    originNP += crossXZ * (ortho.size.y - ortho.offset.y);
    originNP = invBasis * originNP;

    if (ray_aabb_test(originNP) == true) return false;

    //--------------------------------------------------------//

    return true;
    */
}

//============================================================================//

/// Check if a @ref Sphere and a @ref Frustum do not intersect.
///
/// @param sphere the sphere to check
/// @param frustum the frustum to check against
///
/// @result true if object can be culled

inline bool cull_sphere_frustum(Sphere sphere, Frustum frustum)
{
    if (cull_sphere_sphere(sphere, frustum.sphere)) return true;

    const float offset0 = maths::dot(sphere.origin, frustum.planes[0].normal);
    if (offset0 + frustum.planes[0].offset >= sphere.radius) return true;

    const float offset1 = maths::dot(sphere.origin, frustum.planes[1].normal);
    const float offset2 = maths::dot(sphere.origin, frustum.planes[2].normal);
    const float offset3 = maths::dot(sphere.origin, frustum.planes[3].normal);
    const float offset4 = maths::dot(sphere.origin, frustum.planes[4].normal);

    if (offset1 + frustum.planes[1].offset >= sphere.radius) return true;
    if (offset2 + frustum.planes[2].offset >= sphere.radius) return true;
    if (offset3 + frustum.planes[3].offset >= sphere.radius) return true;
    if (offset4 + frustum.planes[4].offset >= sphere.radius) return true;

    return false;
}

//============================================================================//

/// Check if a @ref Box and a @ref Frustum do not intersect.
///
/// @param box the box to check
/// @param frustum the frustum to check against
///
/// @result true if object can be culled

inline bool cull_box_frustum(Box box, Frustum frustum)
{
    if (cull_sphere_frustum({box.origin, box.radius}, frustum)) return true;

    //--------------------------------------------------------//

    const Vec3F offsetX = box.basis[0] * box.extents.x;
    const Vec3F offsetY = box.basis[1] * box.extents.y;
    const Vec3F offsetZ = box.basis[2] * box.extents.z;

    const Vec3F pointA = box.origin - offsetX - offsetY - offsetZ;
    const Vec3F pointB = box.origin - offsetX - offsetY + offsetZ;
    const Vec3F pointC = box.origin - offsetX + offsetY - offsetZ;
    const Vec3F pointD = box.origin - offsetX + offsetY + offsetZ;
    const Vec3F pointE = box.origin + offsetX - offsetY - offsetZ;
    const Vec3F pointF = box.origin + offsetX - offsetY + offsetZ;
    const Vec3F pointG = box.origin + offsetX + offsetY - offsetZ;
    const Vec3F pointH = box.origin + offsetX + offsetY + offsetZ;

    //--------------------------------------------------------//

    const auto test_points_plane = [&](Plane plane) -> uint8_t
    {
        uint8_t result = 0u;

        result |= (maths::dot(pointA, plane.normal) + plane.offset < 0.f) << 0;
        result |= (maths::dot(pointB, plane.normal) + plane.offset < 0.f) << 1;
        result |= (maths::dot(pointC, plane.normal) + plane.offset < 0.f) << 2;
        result |= (maths::dot(pointD, plane.normal) + plane.offset < 0.f) << 3;
        result |= (maths::dot(pointE, plane.normal) + plane.offset < 0.f) << 4;
        result |= (maths::dot(pointF, plane.normal) + plane.offset < 0.f) << 5;
        result |= (maths::dot(pointG, plane.normal) + plane.offset < 0.f) << 6;
        result |= (maths::dot(pointH, plane.normal) + plane.offset < 0.f) << 7;

        return result;
    };

    //--------------------------------------------------------//

    if (test_points_plane(frustum.planes[0]) == 0u) return true;
    if (test_points_plane(frustum.planes[1]) == 0u) return true;
    if (test_points_plane(frustum.planes[2]) == 0u) return true;
    if (test_points_plane(frustum.planes[3]) == 0u) return true;
    if (test_points_plane(frustum.planes[4]) == 0u) return true;

    return false;
}

//============================================================================//

} // namespace sq::maths
