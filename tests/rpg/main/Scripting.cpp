#include <chaiscript/dispatchkit/dispatchkit.hpp>
#include <chaiscript/utility/utility.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/SkyBox.hpp"
#include "../wcoe/Ambient.hpp"
#include "../wcoe/SkyLight.hpp"
#include "../rndr/Renderer.hpp"
#include "Scripting.hpp"

#include "../components/Animator.hpp"
#include "../components/Transform.hpp"
#include "../components/Model.hpp"
#include "../components/Decal.hpp"
#include "../components/SpotLight.hpp"
#include "../components/PointLight.hpp"
#include "../components/Reflect.hpp"

using chai::fun;
using chai::user_type;
using chai::base_class;
using chai::constructor;
using chai::type_conversion;
using chai::utility::add_class;
using chai::Boxed_Value;
using chai::boxed_cast;
using namespace sqt;


void sqt::cs_setup_world(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<World>(*m, "World", {}, {
        {fun(&World::root),      "root"},
        {fun(&World::skybox),    "skybox"},
        {fun(&World::ambient),   "ambient"},
        {fun(&World::skylight),  "skylight"},
        {fun(&World::configure), "configure"}
    });

    _cs.add(m);
}

void sqt::cs_setup_renderer(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<Renderer>(*m, "Renderer", {}, {});

    _cs.add(m);
}


template<class T, sq::ecs::if_not_Configure<T>...> inline void setup_mark_configure(chai::Module& _m, const string& _name) {}
template<class T, sq::ecs::if_Configure<T>...> inline void setup_mark_configure(chai::Module& _m, const string& _name) {
    _m.add(fun(&sq::Entity::mark_configure_component<T>), "mark_configure_" + _name); }

template<class T, sq::ecs::if_not_Refresh<T>...> inline void setup_mark_refresh(chai::Module& _m, const string& _name) {}
template<class T, sq::ecs::if_Refresh<T>...> inline void setup_mark_refresh(chai::Module& _m, const string& _name) {
    _m.add(fun(&sq::Entity::mark_refresh_component<T>), "mark_refresh_" + _name); }

template<class ComponentType> inline
void setup_component(chai::Module& _m, const string& _name, const vector<pair<chai::Proxy_Function, string>>& _funcs) {
    _m.add(user_type<ComponentType>(), _name); _m.add(base_class<sq::Component, ComponentType>());
    _m.add(fun<ComponentType*(sq::Entity::*)()>(&sq::Entity::add_component<ComponentType>), "add_" + _name);
    _m.add(fun<ComponentType*(sq::Entity::*)()>(&sq::Entity::get_component<ComponentType>), "get_" + _name);
    setup_mark_configure<ComponentType>(_m, _name); setup_mark_refresh<ComponentType>(_m, _name);
    for (const auto& f : _funcs) _m.add(f.first, f.second);
}

template<class Type, bool Norm> inline
void setup_PropAnim(chai::Module& _m, const string& _type) {
    add_class<PropAnim<Type, Norm>>(_m, "PropAnim" + _type, {}, {
        {fun(&PropAnim<Type, Norm>::repeat), "repeat"},
        {fun(&PropAnim<Type, Norm>::values), "values"},
        {fun(&PropAnim<Type, Norm>::times),  "times"}
    });

    _m.add(fun(&AnimatorComponent::FUNC_add_prop<Type, Norm>), "add_" + _type);
    _m.add(fun(&AnimatorComponent::FUNC_get_prop<Type, Norm>), "get_" + _type);
    _m.add(fun(&AnimatorComponent::FUNC_erase_prop<Type, Norm>), "erase_" + _type);
}


void sqt::cs_setup_components(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    add_class<SkyBox>(*m, "SkyBox", {}, {
        {fun(&SkyBox::PROP_enabled),    "enabled"},
        {fun(&SkyBox::PROP_saturation), "saturation"},
        {fun(&SkyBox::PROP_brightness), "brightness"},
        {fun(&SkyBox::PROP_contrast),   "contrast"},
        {fun(&SkyBox::PROP_alpha),      "alpha"},
        {fun(&SkyBox::PROP_texture),    "texture"},
        {fun(&SkyBox::configure),       "configure"}
   });

    add_class<Ambient>(*m, "Ambient", {}, {
        {fun(&Ambient::PROP_enabled), "enabled"},
        {fun(&Ambient::PROP_colour),  "colour"},
        {fun(&Ambient::configure),    "configure"} });

    add_class<SkyLight>(*m, "SkyLight", {}, {
        {fun(&SkyLight::PROP_enabled),   "enabled"},
        {fun(&SkyLight::PROP_direction), "direction"},
        {fun(&SkyLight::PROP_colour),    "colour"},
        {fun(&SkyLight::PROP_density),   "density"},
        {fun(&SkyLight::configure),      "configure"} });

    m->add(type_conversion<unique_ptr<SkyBox>&, SkyBox*>([]
          (unique_ptr<SkyBox>& uptr) { return uptr.get(); }));
    m->add(type_conversion<unique_ptr<Ambient>&, Ambient*>([]
          (unique_ptr<Ambient>& uptr) { return uptr.get(); }));
    m->add(type_conversion<unique_ptr<SkyLight>&, SkyLight*>([]
          (unique_ptr<SkyLight>& uptr) { return uptr.get(); }));

    setup_component<AnimatorComponent>(*m, "AnimatorComponent", {
    });

    setup_PropAnim<float, false>(*m, "Float");
    setup_PropAnim<Vec2F, false>(*m, "Vec2F");
    setup_PropAnim<Vec3F, false>(*m, "Vec3F");
    setup_PropAnim<Vec4F, false>(*m, "Vec4F");
    setup_PropAnim<Vec2F, true>(*m, "Norm2");
    setup_PropAnim<Vec3F, true>(*m, "Norm3");
    setup_PropAnim<Vec4F, true>(*m, "Norm4");
    setup_PropAnim<QuatF, true>(*m, "QuatF");

    setup_component<TransformComponent>(*m, "TransformComponent", {
        {fun(&TransformComponent::PROP_position), "position"},
        {fun(&TransformComponent::PROP_rotation), "rotation"},
        {fun(&TransformComponent::PROP_scale),    "scale"}
    });

    setup_component<ModelComponent>(*m, "ModelComponent", {
        {fun(&ModelComponent::PROP_scale),  "scale"},
        {fun(&ModelComponent::PROP_render), "render"},
        {fun(&ModelComponent::PROP_shadow), "shadow"},
        {fun(&ModelComponent::PROP_mesh),   "mesh"},
        {fun(&ModelComponent::PROP_skin),   "skin"},
        {fun(&ModelComponent::PROP_arma),   "arma"},
        {fun(&ModelComponent::PROP_pose),   "pose"}
    });

    setup_component<DecalComponent>(*m, "DecalComponent", {
        {fun(&DecalComponent::PROP_scale), "scale"},
        {fun(&DecalComponent::PROP_alpha), "alpha"},
        {fun(&DecalComponent::PROP_diff),  "diff"},
        {fun(&DecalComponent::PROP_norm),  "norm"},
        {fun(&DecalComponent::PROP_spec),  "spec"}
    });

    setup_component<SpotLightComponent>(*m, "SpotLightComponent", {
        {fun(&SpotLightComponent::PROP_texsize),  "texsize"},
        {fun(&SpotLightComponent::PROP_colour),   "colour"},
        {fun(&SpotLightComponent::PROP_softness), "softness"},
        {fun(&SpotLightComponent::PROP_angle),    "angle"}
    });

    setup_component<PointLightComponent>(*m, "PointLightComponent", {
        {fun(&PointLightComponent::PROP_texsize), "texsize"},
        {fun(&PointLightComponent::PROP_colour),  "colour"}
    });

    setup_component<ReflectComponent>(*m, "ReflectComponent", {
        {fun(&ReflectComponent::PROP_factor), "factor"}
    });

    _cs.add(m);
}
