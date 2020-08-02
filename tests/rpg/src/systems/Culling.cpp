#include "Culling.hpp"

#include <sqee/maths/Culling.hpp>
#include <sqee/misc/DopFunctions.hpp>
#include <sqee/objects/Material.hpp>
#include <sqee/objects/Mesh.hpp>

namespace dop = sq::dop;
namespace maths = sq::maths;
using namespace sqt::sys;

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
        if (maths::cull_box_sphere(data, sphere)) continue;
        result.mIds.push_back(id);
    }

    for (const auto& [id, data] : dop::joined(stuff.culling.mainSpheres))
    {
        if (maths::cull_sphere_sphere(data, sphere)) continue;
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
        if (maths::cull_box_frustum(data, frustum)) continue;
        result.mIds.push_back(id);
    }

    for (const auto& [id, data] : dop::joined(stuff.culling.mainSpheres))
    {
        if (maths::cull_sphere_frustum(data, frustum)) continue;
        result.mIds.push_back(id);
    }

    //--------------------------------------------------------//

    return result;
}

//============================================================================//

dop::Group sqt::sys::system_get_visible_objects_ortho(const WorldStuff& stuff, maths::Ortho2D ortho)
{
    SQASSERT(stuff.culling.mainBoxes.mIsSorted, "");
    SQASSERT(stuff.culling.mainSpheres.mIsSorted, "");

    //--------------------------------------------------------//

    dop::Group result;
    result.reserve(256u);

    //--------------------------------------------------------//

    for (const auto& [id, data] : dop::joined(stuff.culling.mainBoxes))
    {
        if (maths::cull_box_ortho(data, ortho)) continue;
        result.mIds.push_back(id);
    }

    for (const auto& [id, data] : dop::joined(stuff.culling.mainSpheres))
    {
        if (maths::cull_sphere_ortho(data, ortho)) continue;\
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
        box.basis = Mat3F(transform.worldRotation);
    }
}
