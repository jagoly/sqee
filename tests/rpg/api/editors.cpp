#include <sqee/debug/Logging.hpp>
#include <sqee/misc/StringCast.hpp>

#include <sqee/render/Mesh.hpp>
#include <sqee/render/Material.hpp>
#include <sqee/render/Armature.hpp>

#include "editors.hpp"

//============================================================================//

using namespace sqt::api;

//============================================================================//

EntityEdit::EntityEdit(sys::WorldStuff& stuff, int32_t id)
    : entry(id, &stuff.tables.entity.get(id)), stuff(stuff) {}

AnimationEdit::AnimationEdit(sys::WorldStuff& stuff, int32_t id)
    : entry(id, &stuff.tables.animation.get(id)), stuff(stuff) {}

TransformEdit::TransformEdit(sys::WorldStuff& stuff, int32_t id)
    : entry(id, &stuff.tables.transform.get(id)), stuff(stuff) {}

ModelEdit::ModelEdit(sys::WorldStuff& stuff, int32_t id)
    : entry(id, &stuff.tables.model.get(id)), stuff(stuff) {}

SkeletonEdit::SkeletonEdit(sys::WorldStuff& stuff, int32_t id)
    : entry(id, &stuff.tables.skeleton.get(id)), stuff(stuff) {}

SoundEdit::SoundEdit(sys::WorldStuff& stuff, int32_t id)
    : entry(id, &stuff.sound.table.get(id)), stuff(stuff) {}

//============================================================================//

void EntityEdit::set_unique_name(const string& name)
{
    auto& entryName = entry->uniqueName;
    auto& nameMap = stuff.uniqueNames;

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
    auto& table = stuff.tables.entity;
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
    entry->materials.push_back(acquire_Material(path));
}

void ModelEdit::set_mesh(const string& path)
{
    entry->mesh = acquire_Mesh(path);
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

void SoundEdit::set_volume(float volume)
{
    const auto& groups = stuff.sound.groupVolumes;
    entry->volume = volume * groups[entry->group];
    entry->sound.set_volume(entry->volume);
}

void SoundEdit::enable_looping(bool enable)
{
    entry->sound.set_loop(enable);
}
