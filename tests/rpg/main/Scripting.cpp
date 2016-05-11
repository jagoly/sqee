#include <chaiscript/dispatchkit/dispatchkit.hpp>
#include <chaiscript/utility/utility.hpp>

#include "../wcoe/World.hpp"
#include "../wcoe/SkyBox.hpp"
#include "../wcoe/Ambient.hpp"
#include "../wcoe/SkyLight.hpp"
#include "../rndr/Renderer.hpp"
#include "Scripting.hpp"

#include "../components/Transform.hpp"
#include "../components/Model.hpp"
#include "../components/SpotLight.hpp"
#include "../components/PointLight.hpp"
#include "../components/Reflect.hpp"
#include "../components/Decal.hpp"

using chai::fun;
using chai::user_type;
using chai::base_class;
using chai::constructor;
using chai::type_conversion;
using chai::utility::add_class;
using chai::Boxed_Value;
using chai::boxed_cast;

using namespace sqt;


void sqt::cs_setup_main(chai::ChaiScript& _cs) {
    chai::ModulePtr m(new chai::Module());

    _cs.add(m);
}


template<class ComponentType>
void setup_component(chai::Module& _m, const string& _name, const vector<pair<chai::Proxy_Function, string>>& _funcs) {
    _m.add(user_type<ComponentType>(), _name); for (const auto& func : _funcs) _m.add(func.first, func.second);
    _m.add(fun<ComponentType*(sq::Entity::*)()>(&sq::Entity::get_component<ComponentType>), "get_" + _name);
    _m.add(fun(&sq::Entity::add_component<ComponentType>), "add_" + _name);
    _m.add(base_class<sq::Component, ComponentType>());
}


void sqt::cs_setup_wcoe(chai::ChaiScript& _cs) {
    using namespace wcoe;
    chai::ModulePtr m(new chai::Module());

    add_class<SkyBox>(*m, "SkyBox", {}, {
        {fun(&SkyBox::PROP_enabled),    "enabled"},
        {fun(&SkyBox::PROP_saturation), "saturation"},
        {fun(&SkyBox::PROP_brightness), "brightness"},
        {fun(&SkyBox::PROP_contrast),   "contrast"},
        {fun(&SkyBox::PROP_alpha),      "alpha"},
        {fun(&SkyBox::PROP_texture),    "texture"},
        {fun(&SkyBox::ANIM_saturation), "animSaturation"},
        {fun(&SkyBox::ANIM_brightness), "animBrightness"},
        {fun(&SkyBox::ANIM_contrast),   "animContrast"},
        {fun(&SkyBox::ANIM_alpha),      "animAlpha"},
        {fun(&SkyBox::refresh),         "refresh"} });

    add_class<Ambient>(*m, "Ambient", {}, {
        {fun(&Ambient::PROP_enabled), "enabled"},
        {fun(&Ambient::PROP_colour),  "colour"},
        {fun(&Ambient::ANIM_colour),  "animColour"},
        {fun(&Ambient::refresh),      "refresh"} });

    add_class<SkyLight>(*m, "SkyLight", {}, {
        {fun(&SkyLight::PROP_enabled),   "enabled"},
        {fun(&SkyLight::PROP_direction), "direction"},
        {fun(&SkyLight::PROP_colour),    "colour"},
        {fun(&SkyLight::PROP_density),   "density"},
        {fun(&SkyLight::ANIM_direction), "animDirection"},
        {fun(&SkyLight::ANIM_colour),    "animColour"},
        {fun(&SkyLight::ANIM_density),   "animDensity"},
        {fun(&SkyLight::refresh),        "refresh"} });

    add_class<World>(*m, "World", {}, {
        {fun(&World::root),        "root"},
        {fun(&World::skybox),      "skybox"},
        {fun(&World::ambient),     "ambient"},
        {fun(&World::skylight),    "skylight"},
        {fun(&World::refresh),     "refresh"} });

    add_class<AnimatorFloat>(*m, "AnimatorFloat", {}, {
       {fun(&AnimatorFloat::values), "values"},
       {fun(&AnimatorFloat::times),  "times"},
       {fun(&AnimatorFloat::start),  "start"} });

    add_class<AnimatorVec2F>(*m, "AnimatorVec2F", {}, {
       {fun(&AnimatorVec2F::values), "values"},
       {fun(&AnimatorVec2F::times),  "times"},
       {fun(&AnimatorVec2F::start),  "start"} });

    add_class<AnimatorVec3F>(*m, "AnimatorVec3F", {}, {
       {fun(&AnimatorVec3F::values), "values"},
       {fun(&AnimatorVec3F::times),  "times"},
       {fun(&AnimatorVec3F::start),  "start"} });

    add_class<AnimatorVec4F>(*m, "AnimatorVec4F", {}, {
       {fun(&AnimatorVec4F::values), "values"},
       {fun(&AnimatorVec4F::times),  "times"},
       {fun(&AnimatorVec4F::start),  "start"} });

    add_class<AnimatorNormF>(*m, "AnimatorNormF", {}, {
       {fun(&AnimatorNormF::values), "values"},
       {fun(&AnimatorNormF::times),  "times"},
       {fun(&AnimatorNormF  ::start),  "start"} });

    add_class<AnimatorQuatF>(*m, "AnimatorQuatF", {}, {
       {fun(&AnimatorQuatF::values), "values"},
       {fun(&AnimatorQuatF::times),  "times"},
       {fun(&AnimatorQuatF::start),  "start"} });
    m->add(type_conversion<unique_ptr<SkyBox>&, SkyBox*>([]
          (unique_ptr<SkyBox>& uptr) { return uptr.get(); }));
    m->add(type_conversion<unique_ptr<Ambient>&, Ambient*>([]
          (unique_ptr<Ambient>& uptr) { return uptr.get(); }));
    m->add(type_conversion<unique_ptr<SkyLight>&, SkyLight*>([]
          (unique_ptr<SkyLight>& uptr) { return uptr.get(); }));

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

    setup_component<DecalComponent>(*m, "DecalComponent", {
        {fun(&DecalComponent::PROP_scale), "scale"},
        {fun(&DecalComponent::PROP_alpha), "alpha"},
        {fun(&DecalComponent::PROP_diff),  "diff"},
        {fun(&DecalComponent::PROP_norm),  "norm"},
        {fun(&DecalComponent::PROP_spec),  "spec"}
    });

    _cs.add(m);
}


void sqt::cs_setup_rndr(chai::ChaiScript& _cs) {
    using namespace rndr;
    chai::ModulePtr m(new chai::Module());

    add_class<Renderer>(*m, "Renderer", {}, {});

    _cs.add(m);
}
