#include <chaiscript/dispatchkit/dispatchkit.hpp>
#include <chaiscript/utility/utility.hpp>

#include "../wcoe/Cell.hpp"
#include "../wcoe/World.hpp"
#include "../wcoe/SkyBox.hpp"
#include "../wcoe/Ambient.hpp"
#include "../wcoe/SkyLight.hpp"
#include "../wcoe/objects/MetaObject.hpp"
#include "../wcoe/objects/RigBodyBasic.hpp"
#include "../wcoe/objects/RigBodyMulti.hpp"
#include "../wcoe/objects/ModelSimple.hpp"
#include "../wcoe/objects/ModelSkelly.hpp"
#include "../wcoe/objects/PointLight.hpp"
#include "../wcoe/objects/SpotLight.hpp"
#include "../wcoe/objects/Reflector.hpp"
#include "../wcoe/objects/Emitter.hpp"
#include "../wcoe/objects/Liquid.hpp"
#include "../wcoe/objects/Decal.hpp"
#include "../rndr/Renderer.hpp"
#include "Scripting.hpp"

#include "../components/Transform.hpp"
#include "../components/Model.hpp"
#include "../components/SpotLight.hpp"

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
        {fun(&World::add_cell),    "add_cell"},
        {fun(&World::get_cell),    "get_cell"},
        {fun(&World::reload_list), "reload_list"},
        {fun(&World::invalidate),  "invalidate"},
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

    add_class<Object>(*m, "Object", {}, {
       {fun(&Object::name),       "name"},
       {fun(&Object::cell),       "cell"},
       {fun(&Object::refresh),    "refresh"},
       //{fun(&Object::extra),      "extra"},
       {fun(&Object::invalidate), "invalidate"} });

    add_class<RigBodyBasic>(*m, "RigBodyBasic", {}, {
       {fun(&RigBodyBasic::PROP_scale),           "scale"},
       {fun(&RigBodyBasic::PROP_physobj),         "physobj"},
       {fun(&RigBodyBasic::FUNC_set_ModelSimple), "set_ModelSimple"} });

    add_class<ModelSimple>(*m, "ModelSimple", {}, {
       {fun(&ModelSimple::PROP_position), "position"},
       {fun(&ModelSimple::PROP_rotation), "rotation"},
       {fun(&ModelSimple::PROP_scale),    "scale"},
       {fun(&ModelSimple::PROP_render),   "render"},
       {fun(&ModelSimple::PROP_shadow),   "shadow"},
       {fun(&ModelSimple::PROP_mesh),     "mesh"},
       {fun(&ModelSimple::PROP_skin),     "skin"},
       {fun(&ModelSimple::ANIM_position), "animPosition"},
       {fun(&ModelSimple::ANIM_rotation), "animRotation"},
       {fun(&ModelSimple::ANIM_scale),    "animScale"} });

    add_class<ModelSkelly>(*m, "ModelSkelly", {}, {
       {fun(&ModelSkelly::PROP_position), "position"},
       {fun(&ModelSkelly::PROP_rotation), "rotation"},
       {fun(&ModelSkelly::PROP_scale),    "scale"},
       {fun(&ModelSkelly::PROP_shadow),   "shadow"},
       {fun(&ModelSkelly::PROP_arma),     "arma"},
       {fun(&ModelSkelly::PROP_mesh),     "mesh"},
       {fun(&ModelSkelly::PROP_skin),     "skin"},
       {fun(&ModelSkelly::PROP_pose),     "pose"},
       {fun(&ModelSkelly::PROP_anim),     "anim"},
       {fun(&ModelSkelly::FUNC_stop),     "stop"},
       {fun(&ModelSkelly::FUNC_loop),     "loop"},
       {fun(&ModelSkelly::FUNC_play),     "play"},
       {fun(&ModelSkelly::ANIM_position), "animPosition"},
       {fun(&ModelSkelly::ANIM_rotation), "animRotation"},
       {fun(&ModelSkelly::ANIM_scale),    "animScale"} });

    add_class<PointLight>(*m, "PointLight", {}, {
       {fun(&PointLight::PROP_position),  "position"},
       {fun(&PointLight::PROP_colour),    "colour"},
       {fun(&PointLight::PROP_intensity), "intensity"},
       {fun(&PointLight::PROP_texsize),   "texsize"},
       {fun(&PointLight::PROP_shadow),    "shadow"},
       {fun(&PointLight::PROP_specular),  "specular"},
       {fun(&PointLight::ANIM_position),  "animPosition"},
       {fun(&PointLight::ANIM_colour),    "animColour"},
       {fun(&PointLight::ANIM_intensity), "animIntensity"} });

    add_class<SpotLight>(*m, "SpotLight", {}, {
       {fun(&SpotLight::PROP_direction), "direction"},
       {fun(&SpotLight::PROP_position),  "position"},
       {fun(&SpotLight::PROP_colour),    "colour"},
       {fun(&SpotLight::PROP_intensity), "intensity"},
       {fun(&SpotLight::PROP_softness),  "softness"},
       {fun(&SpotLight::PROP_angle),     "angle"},
       {fun(&SpotLight::PROP_texsize),   "texsize"},
       {fun(&SpotLight::PROP_shadow),    "shadow"},
       {fun(&SpotLight::PROP_specular),  "specular"},
       {fun(&SpotLight::ANIM_direction), "animDirection"},
       {fun(&SpotLight::ANIM_position),  "animPosition"},
       {fun(&SpotLight::ANIM_colour),    "animColour"},
       {fun(&SpotLight::ANIM_intensity), "animIntensity"},
       {fun(&SpotLight::ANIM_softness),  "animSoftness"},
       {fun(&SpotLight::ANIM_angle),     "animAngle"} });

    add_class<Reflector>(*m, "Reflector", {}, {
       {fun(&Reflector::PROP_position), "position"},
       {fun(&Reflector::PROP_rotation), "rotation"},
       {fun(&Reflector::PROP_scale),    "scale"},
       {fun(&Reflector::PROP_factor),   "factor"},
       {fun(&Reflector::PROP_shadow),   "shadow"},
       {fun(&Reflector::PROP_mesh),     "mesh"},
       {fun(&Reflector::PROP_skin),     "skin"},
       {fun(&Reflector::ANIM_position), "animPosition"},
       {fun(&Reflector::ANIM_rotation), "animRotation"},
       {fun(&Reflector::ANIM_scale),    "animScale"},
       {fun(&Reflector::ANIM_factor),   "animFactor"} });

    add_class<Emitter>(*m, "Emitter", {}, {
       {fun(&Emitter::PROP_position), "position"},
       {fun(&Emitter::FUNC_emit_puff), "emit_puff"} });

    add_class<Decal>(*m, "Decal", {}, {
       {fun(&Decal::PROP_position), "position"},
       {fun(&Decal::PROP_rotation), "rotation"},
       {fun(&Decal::PROP_scale),    "scale"},
       {fun(&Decal::PROP_alpha),    "alpha"},
       {fun(&Decal::PROP_diff),     "diff"},
       {fun(&Decal::PROP_norm),     "norm"},
       {fun(&Decal::PROP_spec),     "spec"},
       {fun(&Decal::ANIM_position), "animPosition"},
       {fun(&Decal::ANIM_rotation), "animRotation"},
       {fun(&Decal::ANIM_scale),    "animScale"},
       {fun(&Decal::ANIM_alpha),    "animAlpha"} });

    add_class<MetaObject>(*m, "MetaObject", {}, {
        });

    add_class<Cell>(*m, "Cell", {}, {
        {fun(&Cell::PROP_enabled), "enabled"},
        {fun(&Cell::PROP_position), "position"},
        {fun(&Cell::load_from_file), "load_from_file"},
        {fun(&Cell::add_object<MetaObject>),   "add_MetaObject"},
        {fun(&Cell::get_object<MetaObject>),   "get_MetaObject"},
        {fun(&Cell::add_object<RigBodyBasic>), "add_RigBodyBasic"},
        {fun(&Cell::get_object<RigBodyBasic>), "get_RigBodyBasic"},
        {fun(&Cell::add_object<RigBodyMulti>), "add_RigBodyMulti"},
        {fun(&Cell::get_object<RigBodyMulti>), "get_RigBodyMulti"},
        {fun(&Cell::add_object<ModelSimple>),  "add_ModelSimple"},
        {fun(&Cell::get_object<ModelSimple>),  "get_ModelSimple"},
        {fun(&Cell::add_object<ModelSkelly>),  "add_ModelSkelly"},
        {fun(&Cell::get_object<ModelSkelly>),  "get_ModelSkelly"},
        {fun(&Cell::add_object<PointLight>),   "add_PointLight"},
        {fun(&Cell::get_object<PointLight>),   "get_PointLight"},
        {fun(&Cell::add_object<SpotLight>),    "add_SpotLight"},
        {fun(&Cell::get_object<SpotLight>),    "get_SpotLight"},
        {fun(&Cell::add_object<Reflector>),    "add_Reflector"},
        {fun(&Cell::get_object<Reflector>),    "get_Reflector"},
        {fun(&Cell::add_object<Emitter>),      "add_Emitter"},
        {fun(&Cell::get_object<Emitter>),      "get_Emitter"},
        {fun(&Cell::add_object<Liquid>),       "add_Liquid"},
        {fun(&Cell::get_object<Liquid>),       "get_Liquid"},
        {fun(&Cell::add_object<Decal>),        "add_Decal"},
        {fun(&Cell::get_object<Decal>),        "get_Decal"},
        {fun(&Cell::get_object<Object>),       "get_Object"},
        {fun(&Cell::invalidate), "invalidate"},
        {fun(&Cell::refresh), "refresh"} });

    m->add(base_class<Object, MetaObject>());
    m->add(base_class<Object, RigBodyBasic>());
    m->add(base_class<Object, RigBodyMulti>());
    m->add(base_class<Object, ModelSimple>());
    m->add(base_class<Object, ModelSkelly>());
    m->add(base_class<Object, PointLight>());
    m->add(base_class<Object, SpotLight>());
    m->add(base_class<Object, Reflector>());
    m->add(base_class<Object, Emitter>());
    m->add(base_class<Object, Liquid>());
    m->add(base_class<Object, Decal>());

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
        {fun(&ModelComponent::PROP_skin),   "skin"}
    });

    setup_component<SpotLightComponent>(*m, "SpotLightComponent", {
        {fun(&SpotLightComponent::PROP_texsize),   "texsize"},
        {fun(&SpotLightComponent::PROP_offset),    "offset"},
        {fun(&SpotLightComponent::PROP_direction), "direction"},
        {fun(&SpotLightComponent::PROP_colour),    "colour"},
        {fun(&SpotLightComponent::PROP_softness),  "softness"},
        {fun(&SpotLightComponent::PROP_angle),     "angle"},
    });

    _cs.add(m);
}


void sqt::cs_setup_rndr(chai::ChaiScript& _cs) {
    using namespace rndr;
    chai::ModulePtr m(new chai::Module());

    add_class<Renderer>(*m, "Renderer", {}, {
        {fun(&Renderer::reload_lists), "reload_lists"} });

    _cs.add(m);
}
