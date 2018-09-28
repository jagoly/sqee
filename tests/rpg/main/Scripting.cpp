#include <sqee/scripts/Helpers.hpp>

#include "../world/Camera.hpp"
#include "../world/Skybox.hpp"
#include "../world/Ambient.hpp"
#include "../world/Skylight.hpp"

#include "../api/ScriptAPI.hpp"

#include "Scripting.hpp"

//============================================================================//

void sqt::chaiscript_setup_world(sq::ChaiEngine& engine)
{
    auto m = std::make_shared<chai::Module>();

    //--------------------------------------------------------//

    m->add(user_type<world::Camera>(), "Camera");

    m->add(fun(&world::Camera::set_position),  "set_position");
    m->add(fun(&world::Camera::set_direction), "set_direction");

    m->add(fun(&world::Camera::get_position),  "get_position");
    m->add(fun(&world::Camera::get_direction), "get_direction");

    //--------------------------------------------------------//

    m->add(user_type<world::Skybox>(), "Skybox");

    m->add(fun(&world::Skybox::set_texture),    "set_texture");
    m->add(fun(&world::Skybox::set_saturation), "set_saturation");
    m->add(fun(&world::Skybox::set_brightness), "set_brightness");
    m->add(fun(&world::Skybox::set_contrast),   "set_contrast");
    m->add(fun(&world::Skybox::set_opacity),    "set_opacity");

    m->add(fun(&world::Skybox::get_texture),    "get_texture");
    m->add(fun(&world::Skybox::get_saturation), "get_saturation");
    m->add(fun(&world::Skybox::get_brightness), "get_brightness");
    m->add(fun(&world::Skybox::get_contrast),   "get_contrast");
    m->add(fun(&world::Skybox::get_opacity),    "get_opacity");

    //--------------------------------------------------------//

    m->add(user_type<world::Ambient>(), "Ambient");

    m->add(fun(&world::Ambient::set_colour), "set_colour");

    m->add(fun(&world::Ambient::get_colour), "get_colour");

    //--------------------------------------------------------//

    m->add(user_type<world::Skylight>(), "Skylight");

    m->add(fun(&world::Skylight::set_rotation),   "set_rotation");
    m->add(fun(&world::Skylight::set_colour),     "set_colour");
    m->add(fun(&world::Skylight::set_density),    "set_density");
    m->add(fun(&world::Skylight::set_resolution), "set_resolution");
    m->add(fun(&world::Skylight::set_cascades),   "set_cascades");

    m->add(fun(&world::Skylight::get_rotation),   "get_rotation");
    m->add(fun(&world::Skylight::get_colour),     "get_colour");
    m->add(fun(&world::Skylight::get_density),    "get_density");
    m->add(fun(&world::Skylight::get_resolution), "get_resolution");
    m->add(fun(&world::Skylight::get_cascades),   "get_cascades");

    //--------------------------------------------------------//

    engine.add(m);
}

//============================================================================//

void sqt::chaiscript_setup_api(sq::ChaiEngine& engine)
{
    auto m = std::make_shared<chai::Module>();

    //--------------------------------------------------------//

    m->add(vector_conversion<Vector<String>>());

    m->add(user_type<sqt::ScriptAPI>(), "ScriptAPI");

    //--------------------------------------------------------//

    using API = sqt::ScriptAPI;

    m->add(fun(&API::create_root_entity), "create_root_entity");
    m->add(fun(&API::create_entity),      "create_entity");
    m->add(fun(&API::delete_entity),      "delete_entity");

    m->add(fun(&API::play_sound_global),   "play_sound");
    m->add(fun(&API::play_sound_position), "play_sound");
    m->add(fun(&API::play_sound_entity),   "play_sound");

    m->add(fun(&API::begin_animation), "begin_animation");

    m->add(fun(&API::edit_Entity),     "edit_Entity");
    m->add(fun(&API::edit_Animation),  "edit_Animation");
    m->add(fun(&API::edit_Transform),  "edit_Transform");
    m->add(fun(&API::edit_Model),      "edit_Model");
    m->add(fun(&API::edit_Skeleton),   "edit_Skeleton");
    m->add(fun(&API::edit_OrthoLight), "edit_OrthoLight");
    m->add(fun(&API::edit_PointLight), "edit_PointLight");
    m->add(fun(&API::edit_SpotLight),  "edit_SpotLight");
    m->add(fun(&API::edit_Sound),      "edit_Sound");

    m->add(fun(&API::get_skybox),   "skybox");
    m->add(fun(&API::get_ambient),  "ambient");
    m->add(fun(&API::get_skylight), "skylight");

    //--------------------------------------------------------//

    m->add(user_type<api::EntityEdit>(),     "EntityEdit");
    m->add(user_type<api::AnimationEdit>(),  "AnimationEdit");
    m->add(user_type<api::TransformEdit>(),  "TransformEdit");
    m->add(user_type<api::ModelEdit>(),      "ModelEdit");
    m->add(user_type<api::SkeletonEdit>(),   "SkeletonEdit");
    m->add(user_type<api::OrthoLightEdit>(), "OrthoLightEdit");
    m->add(user_type<api::PointLightEdit>(), "PointLightEdit");
    m->add(user_type<api::SpotLightEdit>(),  "SpotLightEdit");
    m->add(user_type<api::SoundEdit>(),      "SoundEdit");

    //--------------------------------------------------------//

    m->add(fun([](const api::EntityEdit& edit)     { return edit.entry.id; }), "get_id");
    m->add(fun([](const api::AnimationEdit& edit)  { return edit.entry.id; }), "get_id");
    m->add(fun([](const api::TransformEdit& edit)  { return edit.entry.id; }), "get_id");
    m->add(fun([](const api::ModelEdit& edit)      { return edit.entry.id; }), "get_id");
    m->add(fun([](const api::SkeletonEdit& edit)   { return edit.entry.id; }), "get_id");
    m->add(fun([](const api::OrthoLightEdit& edit) { return edit.entry.id; }), "get_id");
    m->add(fun([](const api::PointLightEdit& edit) { return edit.entry.id; }), "get_id");
    m->add(fun([](const api::SpotLightEdit& edit)  { return edit.entry.id; }), "get_id");
    m->add(fun([](const api::SoundEdit& edit)      { return edit.entry.id; }), "get_id");

    //--------------------------------------------------------//

    m->add(fun(&api::EntityEdit::set_unique_name), "set_unique_name");
    m->add(fun(&api::EntityEdit::adopt_child),     "adopt_child");

    m->add(fun(&api::AnimationEdit::set_callback_on_end), "set_callback_on_end");

    m->add(fun(&api::TransformEdit::set_position), "set_position");
    m->add(fun(&api::TransformEdit::set_rotation), "set_rotation");
    m->add(fun(&api::TransformEdit::set_scale),    "set_scale");

    m->add(fun(&api::ModelEdit::set_stretch),   "set_stretch");
    m->add(fun(&api::ModelEdit::add_material),  "add_material");
    m->add(fun(&api::ModelEdit::set_mesh),      "set_mesh");
    m->add(fun(&api::ModelEdit::enable_render), "enable_render");
    m->add(fun(&api::ModelEdit::enable_shadow), "enable_shadow");
    m->add(fun(&api::ModelEdit::enable_decals), "enable_decals");

    m->add(fun(&api::SkeletonEdit::set_armature), "set_armature");

    m->add(fun(&api::OrthoLightEdit::set_colour),     "set_colour");
    m->add(fun(&api::OrthoLightEdit::set_minimum),    "set_minimum");
    m->add(fun(&api::OrthoLightEdit::set_maximum),    "set_maximum");
    m->add(fun(&api::OrthoLightEdit::set_density),    "set_density");
    m->add(fun(&api::OrthoLightEdit::set_resolution), "set_resolution");

    m->add(fun(&api::PointLightEdit::set_colour),     "set_colour");
    m->add(fun(&api::PointLightEdit::set_resolution), "set_resolution");

    m->add(fun(&api::SpotLightEdit::set_colour),     "set_colour");
    m->add(fun(&api::SpotLightEdit::set_softness),   "set_softness");
    m->add(fun(&api::SpotLightEdit::set_angle),      "set_angle");
    m->add(fun(&api::SpotLightEdit::set_resolution), "set_resolution");

    m->add(fun(&api::SoundEdit::set_volume),     "set_volume");
    m->add(fun(&api::SoundEdit::enable_looping), "enable_looping");

    //--------------------------------------------------------//

    m->add(constructor<api::AnimationTimeline()>(), "AnimationTimeline");

    m->add(fun(&api::AnimationTimeline::set_times),           "set_times");
    m->add(fun(&api::AnimationTimeline::add_Transform),       "add_Transform");
    m->add(fun(&api::AnimationTimeline::debug_assert_sanity), "debug_assert_sanity");

    //--------------------------------------------------------//

    engine.add(m);
}
