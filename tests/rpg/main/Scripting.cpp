#include <sqee/scripts/Helpers.hpp>

#include "../world/World.hpp"

#include "../world/objects/Camera.hpp"
#include "../world/objects/SkyBox.hpp"
#include "../world/objects/Ambient.hpp"
#include "../world/objects/SkyLight.hpp"

#include "../systems/Entity.hpp"
#include "../systems/Animation.hpp"
#include "../systems/Transform.hpp"
#include "../systems/Lighting.hpp"
#include "../systems/Culling.hpp"
#include "../systems/Sound.hpp"

#include "../messages.hpp"

#include "../api/editors.hpp"
#include "../api/functions.hpp"
#include "../api/other.hpp"

#include "Scripting.hpp"

//============================================================================//

void sqt::chaiscript_setup_world(sq::ChaiEngine& engine)
{
    using namespace sqt;
    using namespace sqt::world;

    auto m = std::make_shared<chai::Module>();

    const auto get_skybox = [](World* world) -> SkyBoxObject& {
        SQASSERT(world->skybox, "skybox not enabled");
        return *world->skybox; };

    const auto get_ambient = [](World* world) -> AmbientObject& {
        SQASSERT(world->ambient, "ambient not enabled");
        return *world->ambient; };

    const auto get_skylight = [](World* world) -> SkyLightObject& {
        SQASSERT(world->skylight, "skylight not enabled");
        return *world->skylight; };

    add_class<World>(*m, "World", {}, {
        //{fun<Camera&,     World>(&World::get_Camera),     "get_Camera"},
        {fun(&World::update_options),  "update_options"},
        {fun(get_skybox), "skybox"},
        {fun(get_ambient), "ambient"},
        {fun(get_skylight), "skylight"}
    });

    add_class<CameraObject>(*m, "CameraObject", {}, {
        {fun(&CameraObject::PROP_position), "position"},
        {fun(&CameraObject::PROP_direction), "direction"},
        {fun(&CameraObject::PROP_fov), "fov"}
    });

    add_class<SkyBoxObject>(*m, "SkyBoxObject", {}, {
        {fun(&SkyBoxObject::PROP_saturation), "saturation"},
        {fun(&SkyBoxObject::PROP_brightness), "brightness"},
        {fun(&SkyBoxObject::PROP_contrast),   "contrast"},
        {fun(&SkyBoxObject::PROP_opacity),    "opacity"},
        {fun(&SkyBoxObject::PROP_texture),    "texture"}
    });

    add_class<AmbientObject>(*m, "AmbientObject", {}, {
        {fun(&AmbientObject::PROP_colour),  "colour"}
    });

    add_class<SkyLightObject>(*m, "SkyLightObject", {}, {
        {fun(&SkyLightObject::PROP_rotation),   "rotation"},
        {fun(&SkyLightObject::PROP_colour),     "colour"},
        {fun(&SkyLightObject::PROP_density),    "density"},
        {fun(&SkyLightObject::PROP_resolution), "resolution"},
        {fun(&SkyLightObject::PROP_cascades),   "cascades"}
    });

    engine.add(m);
}


void sqt::chaiscript_setup_api(sq::ChaiEngine& engine)
{
    auto m = std::make_shared<chai::Module>();

    {
        api::Stuff& stuff = sys::static_WorldStuff();

        //========================================================//

        m->add(user_type<api::EntityEdit>(),    "EntityEdit");
        m->add(user_type<api::AnimationEdit>(), "AnimationEdit");
        m->add(user_type<api::TransformEdit>(), "TransformEdit");
        m->add(user_type<api::ModelEdit>(),     "ModelEdit");
        m->add(user_type<api::SkeletonEdit>(),  "SkeletonEdit");
        m->add(user_type<api::SoundEdit>(),     "SoundEdit");

        //========================================================//

        m->add(fun([&stuff](int32_t id) { return api::EntityEdit(stuff, id);    }), "sq_edit_Entity");
        m->add(fun([&stuff](int32_t id) { return api::AnimationEdit(stuff, id); }), "sq_edit_Animation");
        m->add(fun([&stuff](int32_t id) { return api::TransformEdit(stuff, id); }), "sq_edit_Transform");
        m->add(fun([&stuff](int32_t id) { return api::ModelEdit(stuff, id);     }), "sq_edit_Model");
        m->add(fun([&stuff](int32_t id) { return api::SkeletonEdit(stuff, id);  }), "sq_edit_Skeleton");
        m->add(fun([&stuff](int32_t id) { return api::SoundEdit(stuff, id);     }), "sq_edit_Sound");

        //========================================================//

        m->add(fun([](api::EntityEdit& edit)    { return edit.entry.id; }), "get_id");
        m->add(fun([](api::AnimationEdit& edit) { return edit.entry.id; }), "get_id");
        m->add(fun([](api::TransformEdit& edit) { return edit.entry.id; }), "get_id");
        m->add(fun([](api::ModelEdit& edit)     { return edit.entry.id; }), "get_id");
        m->add(fun([](api::SkeletonEdit& edit)  { return edit.entry.id; }), "get_id");
        m->add(fun([](api::SoundEdit& edit)     { return edit.entry.id; }), "get_id");

        //========================================================//

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

        m->add(fun(&api::SoundEdit::set_volume),     "set_volume");
        m->add(fun(&api::SoundEdit::enable_looping), "enable_looping");

        //========================================================//

        m->add(fun(&api::create_root_entity, std::ref(stuff)), "sq_create_root_entity");
        m->add(fun(&api::create_entity,      std::ref(stuff)), "sq_create_entity");

        m->add(fun(&api::begin_animation, std::ref(stuff)), "sq_begin_animation");

        m->add(fun(&api::play_sound_global,   std::ref(stuff)), "sq_play_sound");
        m->add(fun(&api::play_sound_position, std::ref(stuff)), "sq_play_sound");
        m->add(fun(&api::play_sound_entity,   std::ref(stuff)), "sq_play_sound");

        //========================================================//

        m->add(constructor<api::AnimationTimeline()>(), "AnimationTimeline");

        m->add(fun(&api::AnimationTimeline::set_times),           "set_times");
        m->add(fun(&api::AnimationTimeline::add_Transform),       "add_Transform");
        m->add(fun(&api::AnimationTimeline::debug_assert_sanity), "debug_assert_sanity");
    }

    engine.add(m);
}


void sqt::chaiscript_setup_systems(sq::ChaiEngine& engine)
{
    auto m = std::make_shared<chai::Module>();

    engine.add(m);
}


void sqt::chaiscript_setup_messages(sq::ChaiEngine& engine)
{
    auto m = std::make_shared<chai::Module>();

    sq::chai_add_message_type<msg::Enable_SkyBox>(*m, "Enable_SkyBox");
    sq::chai_add_message_type<msg::Enable_Ambient>(*m, "Enable_Ambient");
    sq::chai_add_message_type<msg::Enable_SkyLight>(*m, "Enable_SkyLight");

    sq::chai_add_message_type<msg::Disable_SkyBox>(*m, "Disable_SkyBox");
    sq::chai_add_message_type<msg::Disable_Ambient>(*m, "Disable_Ambient");
    sq::chai_add_message_type<msg::Disable_SkyLight>(*m, "Disable_SkyLight");

    engine.add(m);
}
