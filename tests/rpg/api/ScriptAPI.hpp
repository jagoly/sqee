#pragma once

#include "../systems/WorldStuff.hpp"

#include "other.hpp"
#include "editors.hpp"

namespace sqt {

//============================================================================//

class ScriptAPI final : sq::NonCopyable
{
public: //====================================================//

    using Int32 = std::int32_t;
    using StringVec = std::vector<std::string>;

    //--------------------------------------------------------//

    ScriptAPI(ResourceCaches& caches, WorldStuff& stuff);

    //--------------------------------------------------------//

    Int32 create_root_entity(const StringVec& components);

    Int32 create_entity(Int32 parent, const StringVec& components);

    //--------------------------------------------------------//

    void delete_entity(Int32 id);

    //--------------------------------------------------------//

    api::SoundEdit play_sound_global(const string& path, uint group);

    api::SoundEdit play_sound_position(const string& path, uint group, Vec3F position);

    api::SoundEdit play_sound_entity(const string& path, uint group, Int32 entity);

    void set_sound_group_volume(uint group, float volume);

    //--------------------------------------------------------//

    api::AnimationEdit begin_animation(Int32 id, const api::AnimationTimeline& timeline);

    //--------------------------------------------------------//

    api::EntityEdit edit_Entity(Int32 id);
    api::AnimationEdit edit_Animation(Int32 id);
    api::TransformEdit edit_Transform(Int32 id);
    api::ModelEdit edit_Model(Int32 id);
    api::SkeletonEdit edit_Skeleton(Int32 id);
    api::OrthoLightEdit edit_OrthoLight(Int32 id);
    api::PointLightEdit edit_PointLight(Int32 id);
    api::SpotLightEdit edit_SpotLight(Int32 id);
    api::SoundEdit edit_Sound(Int32 id);

    //--------------------------------------------------------//

    world::Skybox& get_skybox();
    world::Ambient& get_ambient();
    world::Skylight& get_skylight();

protected: //=================================================//

    ResourceCaches& caches;
    WorldStuff& world;

    //--------------------------------------------------------//

    friend struct api::EntityEdit;
    friend struct api::AnimationEdit;
    friend struct api::TransformEdit;
    friend struct api::ModelEdit;
    friend struct api::SkeletonEdit;
    friend struct api::OrthoLightEdit;
    friend struct api::PointLightEdit;
    friend struct api::SpotLightEdit;
    friend struct api::SoundEdit;

private: //===================================================//

    void impl_add_components(Int32 id, const StringVec& components);

    void impl_play_sound(const string& path, uint group);
};

//============================================================================//

} // namespace sqt
