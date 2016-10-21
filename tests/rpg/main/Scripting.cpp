#include <sqee/scripts/Helpers.hpp>

#include "../world/World.hpp"

#include "../world/objects/Camera.hpp"
#include "../world/objects/SkyBox.hpp"
#include "../world/objects/Ambient.hpp"
#include "../world/objects/SkyLight.hpp"

//#include "../components/Animator.hpp"
#include "../components/Transform.hpp"
//#include "../components/DynamicBody.hpp"
//#include "../components/StaticBody.hpp"
#include "../components/Model.hpp"
#include "../components/Skeleton.hpp"
#include "../components/Decal.hpp"
#include "../components/LightOrtho.hpp"
#include "../components/LightPoint.hpp"
#include "../components/LightSpot.hpp"
//#include "../components/Reflect.hpp"

#include "../systems/Sound.hpp"

#include "../messages.hpp"

#include "Scripting.hpp"

using namespace sqt;


void sqt::chaiscript_setup_world(sq::ChaiEngine& _engine) {
    chai::ModulePtr m(new chai::Module());

    using namespace sqt::world;

    const auto get_skybox = [](World* _world) -> SkyBoxObject& {
        SQASSERT(_world->skybox, "skybox not enabled"); return *_world->skybox; };
    const auto get_ambient = [](World* _world) -> AmbientObject& {
        SQASSERT(_world->ambient, "ambient not enabled"); return *_world->ambient; };
    const auto get_skylight = [](World* _world) -> SkyLightObject& {
        SQASSERT(_world->skylight, "skylight not enabled"); return *_world->skylight; };

    add_class<World>(*m, "World", {}, {
        {fun<sq::EntityManager&, World>(&World::get_EntityManager), "get_EntityManager"},
        {fun<SoundSystem&, World>(&World::get_SoundSystem), "get_SoundSystem"},
        //{fun<Camera&,     World>(&World::get_Camera),     "get_Camera"},
        {fun(&World::update_options),  "update_options"},
        {fun(&World::add_StaticCell), "add_StaticCell"},
        {fun(&World::get_StaticCell), "get_StaticCell"},
        //{fun(&World::get_StaticCellMap), "get_StaticCellMap"},
        {fun(get_skybox), "skybox"},
        {fun(get_ambient), "ambient"},
        {fun(get_skylight), "skylight"}
    });

    add_class<world::StaticCell>(*m, "StaticCell", {}, {
        {fun(&world::StaticCell::add_Model), "add_Model"},
        {fun(&world::StaticCell::add_Decal), "add_Decal"},
        {fun(&world::StaticCell::add_Body),  "add_Body"},
    });

    add_class<world::StaticModel>(*m, "StaticModel", {}, {
        {fun(&world::StaticModel::PROP_position), "position"},
        {fun(&world::StaticModel::PROP_rotation), "rotation"},
        {fun(&world::StaticModel::PROP_scale),    "scale"},
        {fun(&world::StaticModel::PROP_mtrls),    "mtrls"},
        {fun(&world::StaticModel::PROP_mesh),     "mesh"}
    });

    add_class<world::StaticDecal>(*m, "StaticDecal", {}, {
        {fun(&world::StaticDecal::PROP_position), "position"},
        {fun(&world::StaticDecal::PROP_rotation), "rotation"},
        {fun(&world::StaticDecal::PROP_scale),    "scale"},
        {fun(&world::StaticDecal::PROP_material), "material"},
        {fun(&world::StaticDecal::PROP_alpha),    "alpha"}
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

    _engine.add(m);
}


template<class T> inline void setup_component(chai::Module& _m, const vector<pair<chai::Proxy_Function, string>>& _funcs) {
    using Entity = sq::Entity;

    _m.add(fun(&Entity::enable_component<T>), "enable_component_" + T::type());
    _m.add(fun(&Entity::disable_component<T>), "disable_component_" + T::type());
    _m.add(fun<T*, Entity>(&Entity::get_component<T>), "c" + T::type());
    _m.add(user_type<T>(), T::type() + "Component");
    _m.add(base_class<sq::Component, T>());

    for (const auto& func : _funcs) _m.add(func.first, func.second);
}

template<class Type, bool Norm> inline
void setup_PropAnim(chai::Module& _m, const string& _name) {
//    add_class<PropAnim<Type, Norm>>(_m, _name, {}, {
//        {fun(&PropAnim<Type, Norm>::repeat), "repeat"},
//        {fun(&PropAnim<Type, Norm>::values), "values"},
//        {fun(&PropAnim<Type, Norm>::times),  "times"}
//    });
}


void sqt::chaiscript_setup_components(sq::ChaiEngine& _engine) {
    chai::ModulePtr m(new chai::Module());

//    setup_PropAnim<float, false>(*m, "PropAnimFloat");
//    setup_PropAnim<Vec2F, false>(*m, "PropAnimVec2F");
//    setup_PropAnim<Vec3F, false>(*m, "PropAnimVec3F");
//    setup_PropAnim<Vec4F, false>(*m, "PropAnimVec4F");
//    setup_PropAnim<Vec2F, true>(*m, "PropAnimNorm2");
//    setup_PropAnim<Vec3F, true>(*m, "PropAnimNorm3");
//    setup_PropAnim<Vec4F, true>(*m, "PropAnimNorm4");
//    setup_PropAnim<QuatF, true>(*m, "PropAnimQuatF");

//    setup_component<AnimatorComponent>(*m, _world, {
//        {fun(&AnimatorComponent::FUNC_add_Float), "add_Float"},
//        {fun(&AnimatorComponent::FUNC_add_Vec2F), "add_Vec2F"},
//        {fun(&AnimatorComponent::FUNC_add_Vec3F), "add_Vec3F"},
//        {fun(&AnimatorComponent::FUNC_add_Vec4F), "add_Vec4F"},
//        {fun(&AnimatorComponent::FUNC_add_Norm2), "add_Norm2"},
//        {fun(&AnimatorComponent::FUNC_add_Norm3), "add_Norm3"},
//        {fun(&AnimatorComponent::FUNC_add_Norm4), "add_Norm4"},
//        {fun(&AnimatorComponent::FUNC_add_QuatF), "add_QuatF"},
//        {fun(&AnimatorComponent::FUNC_get_Float), "get_Float"},
//        {fun(&AnimatorComponent::FUNC_get_Vec2F), "get_Vec2F"},
//        {fun(&AnimatorComponent::FUNC_get_Vec3F), "get_Vec3F"},
//        {fun(&AnimatorComponent::FUNC_get_Vec4F), "get_Vec4F"},
//        {fun(&AnimatorComponent::FUNC_get_Norm2), "get_Norm2"},
//        {fun(&AnimatorComponent::FUNC_get_Norm3), "get_Norm3"},
//        {fun(&AnimatorComponent::FUNC_get_Norm4), "get_Norm4"},
//        {fun(&AnimatorComponent::FUNC_get_QuatF), "get_QuatF"},
//        {fun(&AnimatorComponent::FUNC_del_Float), "del_Float"},
//        {fun(&AnimatorComponent::FUNC_del_Vec2F), "del_Vec2F"},
//        {fun(&AnimatorComponent::FUNC_del_Vec3F), "del_Vec3F"},
//        {fun(&AnimatorComponent::FUNC_del_Vec4F), "del_Vec4F"},
//        {fun(&AnimatorComponent::FUNC_del_Norm2), "del_Norm2"},
//        {fun(&AnimatorComponent::FUNC_del_Norm3), "del_Norm3"},
//        {fun(&AnimatorComponent::FUNC_del_Norm4), "del_Norm4"},
//        {fun(&AnimatorComponent::FUNC_del_QuatF), "del_QuatF"},
//    });

//    setup_component<DynamicBodyComponent>(*m, _world, {});

//    setup_component<StaticBodyComponent>(*m, _world, {});

    setup_component<TransformComponent>(*m, {
        {fun(&TransformComponent::PROP_position), "position"},
        {fun(&TransformComponent::PROP_rotation), "rotation"},
        {fun(&TransformComponent::PROP_scale),    "scale"}
    });

    setup_component<SkeletonComponent>(*m, {
        {fun(&SkeletonComponent::PROP_armature), "armature"},
        {fun(&SkeletonComponent::PROP_pose),     "pose"}
    });

    setup_component<ModelComponent>(*m, {
        {fun(&ModelComponent::PROP_stretch), "stretch"},
        {fun(&ModelComponent::PROP_render),  "render"},
        {fun(&ModelComponent::PROP_shadow),  "shadow"},
        {fun(&ModelComponent::PROP_decals),  "decals"},
        {fun(&ModelComponent::PROP_mtrls),   "mtrls"},
        {fun(&ModelComponent::PROP_mesh),    "mesh"}
    });

    setup_component<DecalComponent>(*m, {
        {fun(&DecalComponent::PROP_stretch),  "stretch"},
        {fun(&DecalComponent::PROP_alpha),    "alpha"},
        {fun(&DecalComponent::PROP_material), "material"}
    });

    setup_component<LightOrthoComponent>(*m, {
        {fun(&LightOrthoComponent::PROP_colour),     "colour"},
        {fun(&LightOrthoComponent::PROP_minimum),    "minimum"},
        {fun(&LightOrthoComponent::PROP_maximum),    "maximum"},
        {fun(&LightOrthoComponent::PROP_density),    "density"},
        {fun(&LightOrthoComponent::PROP_resolution), "resolution"}
    });

    setup_component<LightPointComponent>(*m, {
        {fun(&LightPointComponent::PROP_colour),     "colour"},
        {fun(&LightPointComponent::PROP_resolution), "resolution"}
    });

    setup_component<LightSpotComponent>(*m, {
        {fun(&LightSpotComponent::PROP_colour),     "colour"},
        {fun(&LightSpotComponent::PROP_softness),   "softness"},
        {fun(&LightSpotComponent::PROP_angle),      "angle"},
        {fun(&LightSpotComponent::PROP_resolution), "resolution"}
    });

//    setup_component<ReflectComponent>(*m, _world, {
//        {fun(&ReflectComponent::PROP_factor), "factor"}
//    });

    _engine.add(m);
}


void sqt::chaiscript_setup_functions(sq::ChaiEngine& _engine) {
    chai::ModulePtr m(new chai::Module());

    _engine.add(m);
}


void sqt::chaiscript_setup_systems(sq::ChaiEngine& engine)
{
    auto m = std::make_shared<chai::Module>();

    add_class<SoundSystem>(*m, "SoundSystem", {}, {
        {fun<int32_t, SoundSystem, const string&, uint8_t>(&SoundSystem::play), "play"},
        {fun<int32_t, SoundSystem, const string&, uint8_t, Vec3F>(&SoundSystem::play), "play"},
        {fun<int32_t, SoundSystem, const string&, uint8_t, const sq::Entity*>(&SoundSystem::play), "play"},
        {fun(&SoundSystem::set_sound_volume), "set_sound_volume"},
        {fun(&SoundSystem::set_sound_loop), "set_sound_loop"}
    });

    engine.add(m);
}


void sqt::chaiscript_setup_messages(sq::ChaiEngine& _engine) {
    chai::ModulePtr m(new chai::Module());

    sq::chai_add_message_type<msg::Enable_SkyBox>(*m, "Enable_SkyBox");
    sq::chai_add_message_type<msg::Enable_Ambient>(*m, "Enable_Ambient");
    sq::chai_add_message_type<msg::Enable_SkyLight>(*m, "Enable_SkyLight");

    sq::chai_add_message_type<msg::Disable_SkyBox>(*m, "Disable_SkyBox");
    sq::chai_add_message_type<msg::Disable_Ambient>(*m, "Disable_Ambient");
    sq::chai_add_message_type<msg::Disable_SkyLight>(*m, "Disable_SkyLight");

    _engine.add(m);
}
