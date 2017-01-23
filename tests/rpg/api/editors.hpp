#pragma once

#include "../systems/WorldStuff.hpp"

//============================================================================//

namespace sqt { namespace api {

//============================================================================//

struct EntityEdit
{
    EntityEdit(sys::WorldStuff& stuff, int32_t id);

    sq::dop::Entry<sys::EntityData> entry;
    sys::WorldStuff& stuff;

    //========================================================//

    void set_unique_name(const string& name);
    void adopt_child(int32_t child);
};

//============================================================================//

struct AnimationEdit
{
    AnimationEdit(sys::WorldStuff& stuff, int32_t id);

    sq::dop::Entry<sys::AnimationData> entry;
    sys::WorldStuff& stuff;

    //========================================================//

    void set_callback_on_end(std::function<void(int32_t id)> func);
};

//============================================================================//

struct TransformEdit
{
    TransformEdit(sys::WorldStuff& stuff, int32_t id);

    sq::dop::Entry<sys::TransformData> entry;
    sys::WorldStuff& stuff;

    //========================================================//

    void set_position(Vec3F position);
    void set_rotation(QuatF rotation);
    void set_scale(float scale);
};

//============================================================================//

struct ModelEdit
{
    ModelEdit(sys::WorldStuff& stuff, int32_t id);

    sq::dop::Entry<sys::ModelData> entry;
    sys::WorldStuff& stuff;

    //========================================================//

    void set_stretch(Vec3F stretch);

    void add_material(const string& path);
    void set_mesh(const string& path);

    void enable_render(bool enable);
    void enable_shadow(bool enable);
    void enable_decals(bool enable);
};

//============================================================================//

struct SkeletonEdit
{
    SkeletonEdit(sys::WorldStuff& stuff, int32_t id);

    sq::dop::Entry<sys::SkeletonData> entry;
    sys::WorldStuff& stuff;

    //========================================================//

    void set_armature(const string& path);
};

//============================================================================//

struct SoundEdit
{
    SoundEdit(sys::WorldStuff& stuff, int32_t id);

    sq::dop::Entry<sys::SoundData> entry;
    sys::WorldStuff& stuff;

    //========================================================//

    void set_volume(float volume);
    void enable_looping(bool enable);
};

}} // namespace sqt::api
