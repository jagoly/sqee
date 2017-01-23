#pragma once

#include <sqee/builtins.hpp>

#include <sqee/maths/Volumes.hpp>

#include "WorldStuff.hpp"

namespace sqt { namespace sys {

//class CullingSystem : sq::NonCopyable
//{
//public:

//    using PlaneArray = array<sq::Plane, 4>;

//    dop::IdSet get_visible_entity_set(sq::Sphere sphere, PlaneArray planes);

//    dop::IdSet get_visible_entity_set(PlaneArray planes);

//    void system_refresh();

//private:

//    CullingSystem(); // private constructor
//    friend CullingSystem& static_CullingSystem();

//    WorldStuff& mStuff;
//};

//CullingSystem& static_CullingSystem();

void system_refresh_culling(WorldStuff& stuff);

//std::array<bool, 6> system_get_visible_pointlight_faces(WorldStuff& stuff, int32_t light, CullSphere sphere, CullFrustum frustum);

sq::dop::Group system_get_visible_objects_sphere(const WorldStuff& stuff, sq::maths::Sphere sphere);

sq::dop::Group system_get_visible_objects_frustum(const WorldStuff& stuff, sq::maths::Frustum frustum);

sq::dop::Group system_get_visible_objects_planes(const WorldStuff& stuff, sq::maths::Planes<4> planes);

//dop::IdSet system_get_visible_objects(const WorldStuff& stuff, sq::maths::Sphere sphere, sq::maths::Planes4 planes);

}} // namespace sqt::sys
