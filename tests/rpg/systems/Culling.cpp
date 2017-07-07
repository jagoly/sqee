#include <sqee/dop/Functions.hpp>

#include <sqee/render/Mesh.hpp>
#include <sqee/render/Material.hpp>

#include "Culling.hpp"

namespace dop = sq::dop;
namespace maths = sq::maths;
using namespace sqt::sys;

//============================================================================//

namespace { // anonymous

//============================================================================//

inline bool sphere_outside_sphere(Vec3F origin, float radius, maths::Sphere sphere)
{
    return maths::distance(origin, sphere.origin) > radius + sphere.radius;
}

template <size_t N>
inline bool sphere_outside_planes(Vec3F origin, float radius, maths::Planes<N> planes)
{
    for (const auto& plane : planes)
        if (maths::dot(origin, plane.normal) + plane.offset > radius)
            return true;

    return false;
}

//============================================================================//

inline bool box_outside_sphere(Vec3F origin, Vec3F extents, QuatF basis, maths::Sphere sphere)
{
    float dist = 0.f;

    const Vec3F boxMin = maths::min(+extents, -extents);
    const Vec3F boxMax = maths::max(+extents, -extents);

    const Vec3F newOrigin = basis * (sphere.origin - origin) + origin;

    const Vec3F diffMin = (newOrigin - boxMin) * (newOrigin - boxMin);
    const Vec3F diffMax = (newOrigin - boxMax) * (newOrigin - boxMax);

    if      (origin.x < boxMin.x) dist = dist + diffMin.x;
    else if (origin.x > boxMax.x) dist = dist + diffMax.x;
    if      (origin.y < boxMin.y) dist = dist + diffMin.y;
    else if (origin.y > boxMax.y) dist = dist + diffMax.y;
    if      (origin.z < boxMin.z) dist = dist + diffMin.z;
    else if (origin.z > boxMax.z) dist = dist + diffMax.z;

    return dist > sphere.radius * sphere.radius;
}

template <size_t N>
inline bool box_outside_planes(Vec3F origin, Vec3F extents, QuatF basis, maths::Planes<N> planes)
{
    const Vec3F offsetX = basis * Vec3F(1.f, 0.f, 0.f) * extents.x;
    const Vec3F offsetY = basis * Vec3F(0.f, 1.f, 0.f) * extents.y;
    const Vec3F offsetZ = basis * Vec3F(0.f, 0.f, 1.f) * extents.z;

    const Vec3F pointA = origin - offsetX - offsetY - offsetZ;
    const Vec3F pointB = origin - offsetX - offsetY + offsetZ;
    const Vec3F pointC = origin - offsetX + offsetY - offsetZ;
    const Vec3F pointD = origin - offsetX + offsetY + offsetZ;
    const Vec3F pointE = origin + offsetX - offsetY - offsetZ;
    const Vec3F pointF = origin + offsetX - offsetY + offsetZ;
    const Vec3F pointG = origin + offsetX + offsetY - offsetZ;
    const Vec3F pointH = origin + offsetX + offsetY + offsetZ;

    for (const auto& plane : planes)
    {
        if (maths::dot(pointA, plane.normal) + plane.offset < 0.f) continue;
        if (maths::dot(pointB, plane.normal) + plane.offset < 0.f) continue;
        if (maths::dot(pointC, plane.normal) + plane.offset < 0.f) continue;
        if (maths::dot(pointD, plane.normal) + plane.offset < 0.f) continue;
        if (maths::dot(pointE, plane.normal) + plane.offset < 0.f) continue;
        if (maths::dot(pointF, plane.normal) + plane.offset < 0.f) continue;
        if (maths::dot(pointG, plane.normal) + plane.offset < 0.f) continue;
        if (maths::dot(pointH, plane.normal) + plane.offset < 0.f) continue;

        return true;
    }

    return false;
}

//============================================================================//

} // anonymous namespace

//============================================================================//

dop::Group sqt::sys::system_get_visible_objects_sphere(const WorldStuff& stuff, maths::Sphere sphere)
{
    SQASSERT(stuff.culling.mainBoxes.mIsSorted, "");
    SQASSERT(stuff.culling.mainSpheres.mIsSorted, "");

    //--------------------------------------------------------//

    dop::Group result;
    result.reserve(256u);

    //--------------------------------------------------------//

    for (const auto& [id, data] : dop::joined(stuff.culling.mainBoxes))
    {
        if (sphere_outside_sphere(data.origin, data.radius, sphere)) continue;
        if (box_outside_sphere(data.origin, data.extents, data.basis, sphere)) continue;
        result.mIds.push_back(id);
    }

    for (const auto& [id, data] : dop::joined(stuff.culling.mainSpheres))
    {
        if (sphere_outside_sphere(data.origin, data.radius, sphere)) continue;
        result.mIds.push_back(id);
    }

    //--------------------------------------------------------//

    return result;
}

//============================================================================//

dop::Group sqt::sys::system_get_visible_objects_frustum(const WorldStuff& stuff, maths::Frustum frustum)
{
    SQASSERT(stuff.culling.mainBoxes.mIsSorted, "");
    SQASSERT(stuff.culling.mainSpheres.mIsSorted, "");

    //--------------------------------------------------------//

    dop::Group result;
    result.reserve(256u);

    //--------------------------------------------------------//

    for (const auto& [id, data] : dop::joined(stuff.culling.mainBoxes))
    {
        if (sphere_outside_sphere(data.origin, data.radius, frustum.sphere)) continue;
        if (sphere_outside_planes(data.origin, data.radius, frustum.planes)) continue;
        if (box_outside_sphere(data.origin, data.extents, data.basis, frustum.sphere)) continue;
        if (box_outside_planes(data.origin, data.extents, data.basis, frustum.planes)) continue;
        result.mIds.push_back(id);
    }

    for (const auto& [id, data] : dop::joined(stuff.culling.mainSpheres))
    {
        if (sphere_outside_sphere(data.origin, data.radius, frustum.sphere)) continue;
        if (sphere_outside_planes(data.origin, data.radius, frustum.planes)) continue;
        result.mIds.push_back(id);
    }

    //--------------------------------------------------------//

    return result;
}

//============================================================================//

dop::Group sqt::sys::system_get_visible_objects_planes(const WorldStuff& stuff, maths::Planes<4> planes)
{
    SQASSERT(stuff.culling.mainBoxes.mIsSorted, "");
    SQASSERT(stuff.culling.mainSpheres.mIsSorted, "");

    //--------------------------------------------------------//

    dop::Group result;
    result.reserve(256u);

    //--------------------------------------------------------//

    for (const auto& [id, data] : dop::joined(stuff.culling.mainBoxes))
    {
        if (sphere_outside_planes(data.origin, data.radius, planes)) continue;
        if (box_outside_planes(data.origin, data.extents, data.basis, planes)) continue;
        result.mIds.push_back(id);
    }

    for (const auto& [id, data] : dop::joined(stuff.culling.mainSpheres))
    {
        if (sphere_outside_planes(data.origin, data.radius, planes)) continue;
        result.mIds.push_back(id);
    }

    //--------------------------------------------------------//

    return result;
}

//============================================================================//

void sqt::sys::system_refresh_culling(WorldStuff& stuff)
{
    stuff.culling.mainBoxes.clear();

    //--------------------------------------------------------//

    for (const auto& [id, transform, model] : dop::joined(stuff.tables.transform, stuff.tables.model))
    {
        const Vec3F scale = maths::abs(model.stretch * transform.worldScale);
        const float maxScale = maths::max(scale.x, scale.y, scale.z);

        stuff.culling.mainBoxes.insert(id, maths::Box());
        auto& box = stuff.culling.mainBoxes.mData.back();

        box.origin = model.mesh->get_origin() * model.stretch;
        box.origin = Vec3F(transform.worldMatrix * Vec4F(box.origin, 1.f));
        box.radius = model.mesh->get_radius() * maxScale;

        box.extents = model.mesh->get_extents() * scale;
        box.basis = transform.worldRotation;
    }
}
