#pragma once

#include "../systems/WorldStuff.hpp"

//============================================================================//

namespace sqt { class ScriptAPI; } // Forward Declaration

//============================================================================//

namespace sqt::api {

//============================================================================//

struct EntityEdit
{
    EntityEdit(ScriptAPI& api, int32_t id);

    sq::dop::Entry<sys::EntityData> entry;
    ScriptAPI& api;

    //--------------------------------------------------------//

    void set_unique_name(const String& name);
    void adopt_child(int32_t child);
};

//============================================================================//

struct AnimationEdit
{
    AnimationEdit(ScriptAPI& api, int32_t id);

    sq::dop::Entry<sys::AnimationData> entry;
    ScriptAPI& api;

    //--------------------------------------------------------//

    void set_callback_on_end(std::function<void(int32_t id)> func);
};

//============================================================================//

struct TransformEdit
{
    TransformEdit(ScriptAPI& api, int32_t id);

    sq::dop::Entry<sys::TransformData> entry;
    ScriptAPI& api;

    //--------------------------------------------------------//

    void set_position(Vec3F position);
    void set_rotation(QuatF rotation);
    void set_scale(float scale);
};

//============================================================================//

struct ModelEdit
{
    ModelEdit(ScriptAPI& api, int32_t id);

    sq::dop::Entry<sys::ModelData> entry;
    ScriptAPI& api;

    //--------------------------------------------------------//

    void set_stretch(Vec3F stretch);

    void add_material(const String& path);
    void set_mesh(const String& path);

    void enable_render(bool enable);
    void enable_shadow(bool enable);
    void enable_decals(bool enable);
};

//============================================================================//

struct SkeletonEdit
{
    SkeletonEdit(ScriptAPI& api, int32_t id);

    sq::dop::Entry<sys::SkeletonData> entry;
    ScriptAPI& api;

    //--------------------------------------------------------//

    void set_armature(const String& path);
};

//============================================================================//

struct OrthoLightEdit
{
    OrthoLightEdit(ScriptAPI& api, int32_t id);

    sq::dop::Entry<sys::OrthoLightData> entry;
    ScriptAPI& api;

    //--------------------------------------------------------//

    void set_colour(Vec3F colour);
    void set_minimum(Vec3F minimum);
    void set_maximum(Vec3F maximum);
    void set_density(float density);
    void set_resolution(uint resolution);
};

//============================================================================//

struct PointLightEdit
{
    PointLightEdit(ScriptAPI& api, int32_t id);

    sq::dop::Entry<sys::PointLightData> entry;
    ScriptAPI& api;

    //--------------------------------------------------------//

    void set_colour(Vec3F colour);
    void set_resolution(uint resolution);
};

//============================================================================//

struct SpotLightEdit
{
    SpotLightEdit(ScriptAPI& api, int32_t id);

    sq::dop::Entry<sys::SpotLightData> entry;
    ScriptAPI& api;

    //--------------------------------------------------------//

    void set_colour(Vec3F colour);
    void set_softness(float softness);
    void set_angle(float angle);
    void set_resolution(uint resolution);
};

//============================================================================//

struct SoundEdit
{
    SoundEdit(ScriptAPI& api, int32_t id);

    sq::dop::Entry<sys::SoundData> entry;
    ScriptAPI& api;

    //--------------------------------------------------------//

    void set_volume(float volume);
    void enable_looping(bool enable);
};

} // namespace sqt::api
