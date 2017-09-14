#include <sqee/debug/Logging.hpp>
#include <sqee/misc/StringCast.hpp>

#include <sqee/render/Mesh.hpp>
#include <sqee/render/Material.hpp>
#include <sqee/render/Armature.hpp>

#include "ScriptAPI.hpp"
#include "editors.hpp"

//============================================================================//

using namespace sqt;
using namespace sqt::api;

//============================================================================//

EntityEdit::EntityEdit(ScriptAPI& api, int32_t id)
    : entry(id, &api.world.tables.entity.get(id)), api(api) {}

AnimationEdit::AnimationEdit(ScriptAPI& api, int32_t id)
    : entry(id, &api.world.tables.animation.get(id)), api(api) {}

TransformEdit::TransformEdit(ScriptAPI& api, int32_t id)
    : entry(id, &api.world.tables.transform.get(id)), api(api) {}

ModelEdit::ModelEdit(ScriptAPI& api, int32_t id)
    : entry(id, &api.world.tables.model.get(id)), api(api) {}

SkeletonEdit::SkeletonEdit(ScriptAPI& api, int32_t id)
    : entry(id, &api.world.tables.skeleton.get(id)), api(api) {}

OrthoLightEdit::OrthoLightEdit(ScriptAPI& api, int32_t id)
    : entry(id, &api.world.tables.ortholight.get(id)), api(api) {}

PointLightEdit::PointLightEdit(ScriptAPI& api, int32_t id)
    : entry(id, &api.world.tables.pointlight.get(id)), api(api) {}

SpotLightEdit::SpotLightEdit(ScriptAPI& api, int32_t id)
    : entry(id, &api.world.tables.spotlight.get(id)), api(api) {}

SoundEdit::SoundEdit(ScriptAPI& api, int32_t id)
    : entry(id, &api.world.soundTable.get(id)), api(api) {}

//============================================================================//

void EntityEdit::set_unique_name(const string& name)
{
    auto& entryName = entry->uniqueName;
    auto& nameMap = api.world.uniqueNames;

    // allow assigning same name
    if (entryName == name) return;

    // assert that the name is available
    SQASSERT(nameMap.count(name) == 0u, "");

    // erase the old name from the map
    if (entryName != "") nameMap.erase(entryName);

    // add the new name to the map
    if (name != "") nameMap.emplace(name, entry.id);

    entryName = name;
}

void EntityEdit::adopt_child(int32_t child)
{
    auto& table = api.world.tables.entity;
    auto& childData = table.get(child);

    if (childData.parent != -1)
    {
        // remove from old parent's children
        auto& oldParent = table.get(childData.parent);
        oldParent.children.erase(child);
    }

    entry->children.insert(child);
    childData.parent = entry.id;
}

//============================================================================//

void AnimationEdit::set_callback_on_end(std::function<void(int32_t)> func)
{
    entry->callback_on_end = func;
}

//============================================================================//

void TransformEdit::set_position(Vec3F position)
{
    entry->localPosition = position;
}

void TransformEdit::set_rotation(QuatF rotation)
{
    entry->localRotation = rotation;
}

void TransformEdit::set_scale(float scale)
{
    entry->localScale = scale;
}

//============================================================================//

void ModelEdit::set_stretch(Vec3F stretch)
{
    entry->stretch = stretch;
}

void ModelEdit::add_material(const string& path)
{
    entry->materials.push_back(api.caches.materials.acquire(path));
}

void ModelEdit::set_mesh(const string& path)
{
    entry->mesh = api.caches.meshes.acquire(path);
}

void ModelEdit::enable_render(bool enable)
{
    entry->enableRender = enable;
}

void ModelEdit::enable_shadow(bool enable)
{
    entry->enableShadow = enable;
}

void ModelEdit::enable_decals(bool enable)
{
    entry->enableDecals = enable;
}

//============================================================================//

void SkeletonEdit::set_armature(const string& path)
{
    //entry->armature = acquire_Armature(path);
}

//============================================================================//

void OrthoLightEdit::set_colour(Vec3F colour)
{
    entry->colour = colour;
}

void OrthoLightEdit::set_minimum(Vec3F minimum)
{
    entry->minimum = minimum;
}

void OrthoLightEdit::set_maximum(Vec3F maximum)
{
    entry->maximum = maximum;
}

void OrthoLightEdit::set_density(float density)
{
    entry->density = density;
}

void OrthoLightEdit::set_resolution(uint resolution)
{
    entry->resolution = resolution;
}

//============================================================================//

void PointLightEdit::set_colour(Vec3F colour)
{
    entry->colour = colour;
}

void PointLightEdit::set_resolution(uint resolution)
{
    entry->resolution = resolution;
}

//============================================================================//

void SpotLightEdit::set_colour(Vec3F colour)
{
    entry->colour = colour;
}

void SpotLightEdit::set_softness(float softness)
{
    entry->softness = softness;
}

void SpotLightEdit::set_angle(float angle)
{
    entry->angle = angle;
}

void SpotLightEdit::set_resolution(uint resolution)
{
    entry->resolution = resolution;
}

//============================================================================//

void SoundEdit::set_volume(float volume)
{
    const auto& groups = api.world.soundGroupVolumes;
    entry->volume = volume * groups[entry->group];
    entry->sound.set_volume(entry->volume);
}

void SoundEdit::enable_looping(bool enable)
{
    entry->sound.set_loop(enable);
}
