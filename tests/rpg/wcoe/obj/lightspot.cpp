#include <glm/gtc/type_ptr.hpp>

#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/lightspot.hpp>

#include "../cell.hpp"
#include "../world.hpp"
#include "lightspot.hpp"

using namespace sqt::wcoe;

LightSpot::LightSpot(const string& _name, const Cell& _cell)
    : Object(ObjType::LightSpot, _name, _cell) {}

void LightSpot::load_from_spec(const ObjSpec& _spec) {
    shadow = _spec.flags.count("shadow");
    diffuse = _spec.flags.count("diffuse");
    specular = _spec.flags.count("specular");

    spot.reset(new sq::LightSpot(shadow));
    spot->position  = glm::make_vec3(_spec.fMap.at("position").data());
    spot->direction = glm::make_vec3(_spec.fMap.at("direction").data());
    spot->colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    spot->angle     = glm::radians(_spec.fMap.at("angintsft")[0]) / 2.f;
    spot->intensity = _spec.fMap.at("angintsft")[1];
    spot->softness  = _spec.fMap.at("angintsft")[2];
    if (shadow) spot->texSize = _spec.iMap.at("texsize")[0];

    spot->position += cell.position;
}

void LightSpot::tick() {
    spot->update();
}
