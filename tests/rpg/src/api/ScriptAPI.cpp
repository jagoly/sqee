#include <sqee/debug/Logging.hpp>

#include "ScriptAPI.hpp"

namespace dop = sq::dop;
namespace algo = sq::algo;
using namespace sqt;

//============================================================================//

using Int32 = ScriptAPI::Int32;
using StringVec = ScriptAPI::StringVec;

//============================================================================//

ScriptAPI::ScriptAPI(ResourceCaches& caches, WorldStuff& world)
    : caches(caches), world(world) {}

//============================================================================//

void ScriptAPI::impl_add_components(Int32 id, const StringVec& components)
{
    for (const std::string& name : components)
    {
        if      (name == "Transform")  world.tables.transform  .insert(id, {});
        else if (name == "Model")      world.tables.model      .insert(id, {});
        else if (name == "Skeleton")   world.tables.skeleton   .insert(id, {});
        else if (name == "OrthoLight") world.tables.ortholight .insert(id, {});
        else if (name == "PointLight") world.tables.pointlight .insert(id, {});
        else if (name == "SpotLight")  world.tables.spotlight  .insert(id, {});
        else sq::log_error("invalid component type '{}'", name);
    }
}

Int32 ScriptAPI::create_root_entity(const StringVec& components)
{
    const auto id = ++world.entityCounter;
    world.tables.entity.insert(id, sys::EntityData());

    impl_add_components(id, components);
    return id;
}

Int32 ScriptAPI::create_entity(Int32 parent, const StringVec& components)
{
    const auto id = ++world.entityCounter;
    world.tables.entity.insert(id, sys::EntityData());

    world.tables.entity.get(parent).children.insert(id);
    world.tables.entity.back()->parent = parent;

    impl_add_components(id, components);
    return id;
}

//============================================================================//

void ScriptAPI::delete_entity(Int32 id)
{
    SQASSERT(world.tables.entity.exists(id), "invalid entity id");

    world.deletedEntities.insert(id);

    //--------------------------------------------------------//

    const uint index = world.tables.entity.find(id);
    auto& entity = world.tables.entity.mData[index];

    for (Int32 child : entity.children.mIds)
        delete_entity(child);

    if (entity.uniqueName.empty() == false)
        world.uniqueNames.erase(entity.uniqueName);

    auto& parent = world.tables.entity.get(entity.parent);
    parent.children.erase(id);

    //--------------------------------------------------------//

    world.tables.animation.try_erase(id);
    world.tables.transform.try_erase(id);
    world.tables.model.try_erase(id);
    world.tables.skeleton.try_erase(id);
    world.tables.ortholight.try_erase(id);
    world.tables.pointlight.try_erase(id);
    world.tables.spotlight.try_erase(id);

    world.tables.entity.erase_at(index);
}

//============================================================================//

void ScriptAPI::impl_play_sound(const std::string& path, uint group)
{
    sys::SoundData data { sq::Sound(), uint8_t(group), -1, 100.f };
    data.sound.set_volume(100.f * world.soundGroupVolumes[group]);
    data.sound.set_wave(caches.sounds.acquire(path));

    const auto id = ++world.soundCounter;
    world.soundTable.insert(id, std::move(data));
    world.soundTable.back()->sound.play();
}

api::SoundEdit ScriptAPI::play_sound_global(const std::string& path, uint group)
{
    sq::log_info("playing global sound {}", path);

    impl_play_sound(path, group);

    auto entry = world.soundTable.back();
    entry->sound.set_relative(true);

    return api::SoundEdit(*this, entry.id);
}

api::SoundEdit ScriptAPI::play_sound_position(const std::string& path, uint group, Vec3F position)
{
    sq::log_info("playing local sound {}", path);

    impl_play_sound(path, group);

    auto entry = world.soundTable.back();
    entry->sound.set_relative(false);
    entry->sound.set_position(position);

    return api::SoundEdit(*this, entry.id);
}

api::SoundEdit ScriptAPI::play_sound_entity(const std::string& path, uint group, Int32 entity)
{
    sq::log_info("playing entity sound {}", path);

    impl_play_sound(path, group);

    auto& transformData = world.tables.transform.get(entity);
    auto& position = transformData.worldPosition;

    auto entry = world.soundTable.back();
    entry->sound.set_relative(false);
    entry->sound.set_position(position);

    return api::SoundEdit(*this, entry.id);
}

void ScriptAPI::set_sound_group_volume(uint group, float volume)
{
    world.soundGroupVolumes[group] = volume;

    for (auto& item : world.soundTable.mData)
        if (item.group == group)
            item.sound.set_volume(item.volume * volume);
}

//============================================================================//

api::AnimationEdit ScriptAPI::begin_animation(Int32 id, const api::AnimationTimeline& timeline)
{
    world.tables.animation.insert(id, sys::AnimationData{timeline.mTimes, 0u, 0u, {}});

    if (timeline.mTransform.empty() == false)
    {
        world.animation.transform.insert(id, timeline.mTransform);
    }

    return api::AnimationEdit(*this, id);
}

//============================================================================//

api::EntityEdit ScriptAPI::edit_Entity(Int32 id) { return api::EntityEdit(*this, id); }

api::AnimationEdit ScriptAPI::edit_Animation(Int32 id) { return api::AnimationEdit(*this, id); }

api::TransformEdit ScriptAPI::edit_Transform(Int32 id) { return api::TransformEdit(*this, id); }

api::ModelEdit ScriptAPI::edit_Model(Int32 id) { return api::ModelEdit(*this, id); }

api::SkeletonEdit ScriptAPI::edit_Skeleton(Int32 id) { return api::SkeletonEdit(*this, id); }

api::OrthoLightEdit ScriptAPI::edit_OrthoLight(Int32 id) { return api::OrthoLightEdit(*this, id); }

api::PointLightEdit ScriptAPI::edit_PointLight(Int32 id) { return api::PointLightEdit(*this, id); }

api::SpotLightEdit ScriptAPI::edit_SpotLight(Int32 id) { return api::SpotLightEdit(*this, id); }

api::SoundEdit ScriptAPI::edit_Sound(Int32 id) { return api::SoundEdit(*this, id); }

//============================================================================//

world::Skybox& ScriptAPI::get_skybox() { return *world.skybox; }

world::Ambient& ScriptAPI::get_ambient() { return *world.ambient; }

world::Skylight& ScriptAPI::get_skylight() { return *world.skylight; }
