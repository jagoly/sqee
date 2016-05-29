#include <chaiscript/dispatchkit/dispatchkit.hpp>
#include <chaiscript/utility/utility.hpp>

#include "../wcoe/Camera.hpp"
#include "../wcoe/SkyBox.hpp"
#include "../wcoe/Ambient.hpp"
#include "../wcoe/SkyLight.hpp"
#include "../rndr/Renderer.hpp"
#include "../wcoe/World.hpp"

#include "../components/Animator.hpp"
#include "../components/DynamicBody.hpp"
#include "../components/Transform.hpp"
#include "../components/StaticBody.hpp"
#include "../components/Model.hpp"
#include "../components/Decal.hpp"
#include "../components/SpotLight.hpp"
#include "../components/PointLight.hpp"
#include "../components/Reflect.hpp"

#include "Scripting.hpp"

using std::bind; using std::ref;
using namespace std::placeholders;
using namespace sqt;

using chai::fun;
using chai::user_type;
using chai::base_class;
using chai::constructor;
using chai::type_conversion;
using chai::utility::add_class;
using chai::Boxed_Value;
using chai::boxed_cast;


void sqt::cs_setup_world(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<World>(*m, "World", {}, {
        {fun(&World::root),      "root"},
        {fun(&World::camera),    "camera"},
        {fun(&World::skybox),    "skybox"},
        {fun(&World::ambient),   "ambient"},
        {fun(&World::skylight),  "skylight"},
        {fun(&World::configure), "configure"},
        {fun(&World::mark_configure_all_components), "mark_configure_all_components"},
        {fun(&World::mark_refresh_all_components), "mark_refresh_all_components"}
    });

    _cs.add(m);
}

void sqt::cs_setup_renderer(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Renderer>(*m, "Renderer", {}, {});

    _cs.add(m);
}


template<class T, ecs::if_not_Configure<T>...> inline void setup_mark_configure(chai::Module& _m) {}
template<class T, ecs::if_Configure<T>...> inline void setup_mark_configure(chai::Module& _m) {
    _m.add(fun(&World::mark_configure_component<T>), "mark_configure_" + T::type() + "Component"); }

template<class T, ecs::if_not_Refresh<T>...> inline void setup_mark_refresh(chai::Module& _m) {}
template<class T, ecs::if_Refresh<T>...> inline void setup_mark_refresh(chai::Module& _m) {
    _m.add(fun(&World::mark_refresh_component<T>), "mark_refresh_" + T::type() + "Component"); }

template<class T> inline void setup_component(chai::Module& _m, World& _world, const vector<pair<chai::Proxy_Function, string>>& _funcs) {
    _m.add(fun<void(EntityRPG*)>(bind(&EntityRPG::remove_component<T>, _1, ref(_world))), "remove_" + T::type() + "Component");
    _m.add(fun<T*(EntityRPG*)>(bind(&EntityRPG::add_component<T>, _1, ref(_world))), "add_" + T::type() + "Component");
    _m.add(fun<T*(EntityRPG::*)()>(&EntityRPG::get_component<T>), "get_" + T::type() + "Component");
    _m.add(user_type<T>(), T::type() + "Component"); _m.add(base_class<ComponentRPG, T>());
    for (const auto& func : _funcs) _m.add(func.first, func.second);
    setup_mark_configure<T>(_m); setup_mark_refresh<T>(_m);
}

template<class Type, bool Norm> inline
void setup_PropAnim(chai::Module& _m, const string& _name) {
    add_class<PropAnim<Type, Norm>>(_m, _name, {}, {
        {fun(&PropAnim<Type, Norm>::repeat), "repeat"},
        {fun(&PropAnim<Type, Norm>::values), "values"},
        {fun(&PropAnim<Type, Norm>::times),  "times"}
    });
}


void sqt::cs_setup_components(chai::ChaiScript& _cs, World& _world) {
    chai::ModulePtr m(new chai::Module());

    add_class<EntityRPG>(*m, "Entity", {}, {
        {fun<EntityRPG* (EntityRPG::*)()>(&EntityRPG::get_parent), "get_parent"},
        {fun<vector<unique_ptr<EntityRPG>>& (EntityRPG::*)()>(&EntityRPG::get_children), "get_children"},
        {fun(&EntityRPG::add_child), "add_child"}, {fun(&EntityRPG::remove_child), "remove_child"},
        {fun<EntityRPG* (EntityRPG::*)(const string&)>(&EntityRPG::get_child), "get_child"},

        //{fun(&EntityRPG::mark_configure_all), "mark_configure_all"},
        //{fun(&EntityRPG::mark_refresh_all), "mark_refresh_all"},
        //{fun(&EntityRPG::mark_configure_recursive_all), "mark_configure_recursive_all"},
        //{fun(&EntityRPG::mark_refresh_recursive_all), "mark_refresh_recursive_all"},

        {fun(&sq::EntityBase::get_attr), "get"}, {fun(&sq::EntityBase::del_attr), "del"},
        {fun<decltype(&sq::EntityBase::operator[])>(&sq::EntityBase::operator[]), "[]"}
    });

    add_class<Camera>(*m, "Camera", {}, {
        {fun(&Camera::PROP_position), "position"},
        {fun(&Camera::PROP_direction), "direction"}
    });

    add_class<SkyBox>(*m, "SkyBox", {}, {
        {fun(&SkyBox::PROP_saturation), "saturation"},
        {fun(&SkyBox::PROP_brightness), "brightness"},
        {fun(&SkyBox::PROP_contrast),   "contrast"},
        {fun(&SkyBox::PROP_alpha),      "alpha"},
        {fun(&SkyBox::PROP_texture),    "texture"},
        {fun(&SkyBox::configure),       "configure"}
    });

    add_class<Ambient>(*m, "Ambient", {}, {
        {fun(&Ambient::PROP_colour),  "colour"},
        {fun(&Ambient::configure),    "configure"}
    });

    add_class<SkyLight>(*m, "SkyLight", {}, {
        {fun(&SkyLight::PROP_direction), "direction"},
        {fun(&SkyLight::PROP_colour),    "colour"},
        {fun(&SkyLight::PROP_density),   "density"},
        {fun(&SkyLight::configure),      "configure"}
    });

    m->add(type_conversion<unique_ptr<SkyBox>&, SkyBox*>([]
          (unique_ptr<SkyBox>& uptr) { return uptr.get(); }));
    m->add(type_conversion<unique_ptr<Ambient>&, Ambient*>([]
          (unique_ptr<Ambient>& uptr) { return uptr.get(); }));
    m->add(type_conversion<unique_ptr<SkyLight>&, SkyLight*>([]
          (unique_ptr<SkyLight>& uptr) { return uptr.get(); }));

    setup_PropAnim<float, false>(*m, "PropAnimFloat");
    setup_PropAnim<Vec2F, false>(*m, "PropAnimVec2F");
    setup_PropAnim<Vec3F, false>(*m, "PropAnimVec3F");
    setup_PropAnim<Vec4F, false>(*m, "PropAnimVec4F");
    setup_PropAnim<Vec2F, true>(*m, "PropAnimNorm2");
    setup_PropAnim<Vec3F, true>(*m, "PropAnimNorm3");
    setup_PropAnim<Vec4F, true>(*m, "PropAnimNorm4");
    setup_PropAnim<QuatF, true>(*m, "PropAnimQuatF");

    setup_component<AnimatorComponent>(*m, _world, {
        {fun(&AnimatorComponent::FUNC_add_Float), "add_Float"},
        {fun(&AnimatorComponent::FUNC_add_Vec2F), "add_Vec2F"},
        {fun(&AnimatorComponent::FUNC_add_Vec3F), "add_Vec3F"},
        {fun(&AnimatorComponent::FUNC_add_Vec4F), "add_Vec4F"},
        {fun(&AnimatorComponent::FUNC_add_Norm2), "add_Norm2"},
        {fun(&AnimatorComponent::FUNC_add_Norm3), "add_Norm3"},
        {fun(&AnimatorComponent::FUNC_add_Norm4), "add_Norm4"},
        {fun(&AnimatorComponent::FUNC_add_QuatF), "add_QuatF"},
        {fun(&AnimatorComponent::FUNC_get_Float), "get_Float"},
        {fun(&AnimatorComponent::FUNC_get_Vec2F), "get_Vec2F"},
        {fun(&AnimatorComponent::FUNC_get_Vec3F), "get_Vec3F"},
        {fun(&AnimatorComponent::FUNC_get_Vec4F), "get_Vec4F"},
        {fun(&AnimatorComponent::FUNC_get_Norm2), "get_Norm2"},
        {fun(&AnimatorComponent::FUNC_get_Norm3), "get_Norm3"},
        {fun(&AnimatorComponent::FUNC_get_Norm4), "get_Norm4"},
        {fun(&AnimatorComponent::FUNC_get_QuatF), "get_QuatF"},
        {fun(&AnimatorComponent::FUNC_del_Float), "del_Float"},
        {fun(&AnimatorComponent::FUNC_del_Vec2F), "del_Vec2F"},
        {fun(&AnimatorComponent::FUNC_del_Vec3F), "del_Vec3F"},
        {fun(&AnimatorComponent::FUNC_del_Vec4F), "del_Vec4F"},
        {fun(&AnimatorComponent::FUNC_del_Norm2), "del_Norm2"},
        {fun(&AnimatorComponent::FUNC_del_Norm3), "del_Norm3"},
        {fun(&AnimatorComponent::FUNC_del_Norm4), "del_Norm4"},
        {fun(&AnimatorComponent::FUNC_del_QuatF), "del_QuatF"},
    });

    setup_component<DynamicBodyComponent>(*m, _world, {});

    setup_component<TransformComponent>(*m, _world, {
        {fun(&TransformComponent::PROP_position), "position"},
        {fun(&TransformComponent::PROP_rotation), "rotation"},
        {fun(&TransformComponent::PROP_scale),    "scale"}
    });

    setup_component<StaticBodyComponent>(*m, _world, {});

    setup_component<ModelComponent>(*m, _world, {
        {fun(&ModelComponent::PROP_scale),  "scale"},
        {fun(&ModelComponent::PROP_render), "render"},
        {fun(&ModelComponent::PROP_shadow), "shadow"},
        {fun(&ModelComponent::PROP_decals), "decals"},
        {fun(&ModelComponent::PROP_mesh),   "mesh"},
        {fun(&ModelComponent::PROP_skin),   "skin"},
        {fun(&ModelComponent::PROP_arma),   "arma"},
        {fun(&ModelComponent::PROP_pose),   "pose"}
    });

    setup_component<DecalComponent>(*m, _world, {
        {fun(&DecalComponent::PROP_scale), "scale"},
        {fun(&DecalComponent::PROP_alpha), "alpha"},
        {fun(&DecalComponent::PROP_diff),  "diff"},
        {fun(&DecalComponent::PROP_norm),  "norm"},
        {fun(&DecalComponent::PROP_spec),  "spec"}
    });

    setup_component<SpotLightComponent>(*m, _world, {
        {fun(&SpotLightComponent::PROP_texsize),  "texsize"},
        {fun(&SpotLightComponent::PROP_colour),   "colour"},
        {fun(&SpotLightComponent::PROP_softness), "softness"},
        {fun(&SpotLightComponent::PROP_angle),    "angle"}
    });

    setup_component<PointLightComponent>(*m, _world, {
        {fun(&PointLightComponent::PROP_texsize), "texsize"},
        {fun(&PointLightComponent::PROP_colour),  "colour"}
    });

    setup_component<ReflectComponent>(*m, _world, {
        {fun(&ReflectComponent::PROP_factor), "factor"}
    });

    _cs.add(m);
}


void sqt::cs_setup_functions(chai::ChaiScript& _cs, World& _world) {
    chai::ModulePtr m(new chai::Module());

    _cs.add(m);
}
