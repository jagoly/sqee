#include <sqee/debug/Logging.hpp>

#include <sqee/render/Mesh.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Material.hpp>

#include "../resources/Manager.hpp"

#include "functions.hpp"

namespace dop = sq::dop;

using namespace sqt;
using namespace sqt::api;

//============================================================================//

namespace { // anonymous

//============================================================================//

void impl_add_components(Stuff& stuff, Int32 id, const StringVec& components)
{
    for (const auto& name : components)
    {
        if (name == "Transform")
        {
            stuff.tables.transform.insert(id, sys::TransformData());
        }

        else if (name == "Model")
        {
            stuff.tables.model.insert(id, sys::ModelData());
        }

        else if (name == "Skeleton")
        {
            stuff.tables.skeleton.insert(id, sys::SkeletonData());
        }

        else if (name == "SpotLight")
        {
            stuff.tables.spotLight.insert(id, sys::SpotLightData());
        }

        else sq::log_error("invalid component type '%s'", name);
    }
}

//============================================================================//

void impl_play_sound(Stuff& stuff, const string& path, uint group)
{
    sys::SoundData data {sq::Sound(), uint8_t(group), -1, 100.f};
    data.sound.set_volume(100.f * stuff.sound.groupVolumes[group]);
    data.sound.set_wave(acquire_SoundWave(path));

    const auto id = ++stuff.soundCounter;
    stuff.sound.table.insert(id, std::move(data));
    stuff.sound.table.back()->sound.play();
}

//============================================================================//

} // anonymous namespace

//============================================================================//

Int32 api::create_root_entity(Stuff& stuff, const StringVec& components)
{
    const auto id = ++stuff.entityCounter;
    stuff.tables.entity.insert(id, sys::EntityData());

    impl_add_components(stuff, id, components);
    return id;
}

Int32 api::create_entity(Stuff& stuff, Int32 parent, const StringVec& components)
{
    const auto id = ++stuff.entityCounter;
    stuff.tables.entity.insert(id, sys::EntityData());

    stuff.tables.entity.get(parent).children.insert(id);
    stuff.tables.entity.back()->parent = parent;

    impl_add_components(stuff, id, components);
    return id;
}

//============================================================================//

AnimationEdit api::begin_animation(Stuff& stuff, Int32 id, const AnimationTimeline& timeline)
{
    stuff.tables.animation.insert(id, sys::AnimationData{timeline.mTimes, 0u, 0u, {}});

    if (timeline.mTransform.empty() == false)
    {
        stuff.animation.transform.insert(id, timeline.mTransform);
    }

    return AnimationEdit(stuff, id);
}

//============================================================================//

SoundEdit api::play_sound_global(Stuff& stuff, const string& path, uint group)
{
    sq::log_info("playing global sound %s", path);

    impl_play_sound(stuff, path, group);

    auto entry = stuff.sound.table.back();
    entry->sound.set_relative(true);

    return SoundEdit(stuff, entry.id);
}

SoundEdit api::play_sound_position(Stuff& stuff, const string& path, uint group, Vec3F position)
{
    sq::log_info("playing local sound %s", path);

    impl_play_sound(stuff, path, group);

    auto entry = stuff.sound.table.back();
    entry->sound.set_relative(false);
    entry->sound.set_position(position);

    return SoundEdit(stuff, entry.id);
}

SoundEdit api::play_sound_entity(Stuff& stuff, const string& path, uint group, Int32 entity)
{
    sq::log_info("playing entity sound %s", path);

    impl_play_sound(stuff, path, group);

    auto& transformData = stuff.tables.transform.get(entity);
    auto& position = transformData.worldPosition;

    auto entry = stuff.sound.table.back();
    entry->sound.set_relative(false);
    entry->sound.set_position(position);

    return SoundEdit(stuff, entry.id);
}

void api::set_sound_group_volume(Stuff& stuff, uint group, float volume)
{
    stuff.sound.groupVolumes[group] = volume;

    for (auto& item : stuff.sound.table.mData)
        if (item.group == group)
            item.sound.set_volume(item.volume * volume);
}
