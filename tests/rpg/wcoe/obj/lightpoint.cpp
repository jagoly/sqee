#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/lightpoint.hpp>

#include "../cell.hpp"
#include "lightpoint.hpp"

using namespace sqt::wcoe;

LightPoint::LightPoint(const string& _name, const Cell& _cell)
    : Object(ObjType::LightPoint, _name, _cell) {}

void LightPoint::load_from_spec(const ObjSpec& _spec) {
    shadow = _spec.flags.count("shadow");
    diffuse = _spec.flags.count("diffuse");
    specular = _spec.flags.count("specular");

    point.reset(new sq::LightPoint(shadow));
    point->position  = glm::make_vec3(_spec.fMap.at("position").data());
    point->colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    point->intensity = _spec.fMap.at("intensity")[0];
    if (shadow) point->texSize = _spec.iMap.at("texsize")[0];

    point->position += cell.position;
}

void LightPoint::tick() {
    point->update();
}
