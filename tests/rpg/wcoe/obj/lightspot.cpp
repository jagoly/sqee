#include <glm/gtc/type_ptr.hpp>

#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/lightspot.hpp>

#include "../cell.hpp"
#include "../world.hpp"
#include "lightspot.hpp"

using namespace sqt::wcoe;

LightSpot::LightSpot(const string& _name, const Cell* _cell)
    : Object(ObjType::LightSpot, _name, _cell) {}

void LightSpot::load_from_spec(const ObjSpec& _spec) {
    DATA.shadow = _spec.flags.count("shadow");
    DATA.diffuse = _spec.flags.count("diffuse");
    DATA.specular = _spec.flags.count("specular");

    DATA.position  = glm::make_vec3(_spec.fMap.at("position").data());
    DATA.direction = glm::make_vec3(_spec.fMap.at("direction").data());
    DATA.colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    DATA.angle     = _spec.fMap.at("angle")[0];
    DATA.intensity = _spec.fMap.at("intensity")[0];
    DATA.softness  = _spec.fMap.at("softness")[0];
    if (DATA.shadow) DATA.texsize = _spec.iMap.at("texsize")[0];
}

void LightSpot::update_from_data() {
    sphere.origin = DATA.position;
    sphere.radius = DATA.intensity;

    if (DATA.shadow) {
        sSpot.reset(new sq::ShadowSpot()); lSpot.reset();
        sSpot->position = DATA.position + cell->position;
        sSpot->direction = DATA.direction;
        sSpot->colour = DATA.colour;
        sSpot->angle = glm::radians(DATA.angle);
        sSpot->intensity = DATA.intensity;
        sSpot->softness = DATA.softness;
        sSpot->texSize = DATA.texsize;
        sSpot->update(); frus = sq::make_Frustum(sSpot->matrix);
    } else {
        lSpot.reset(new sq::LightSpot()); sSpot.reset();
        lSpot->position = DATA.position + cell->position;
        lSpot->direction = DATA.direction;
        lSpot->colour = DATA.colour;
        lSpot->angle = glm::radians(DATA.angle);
        lSpot->intensity = DATA.intensity;
        lSpot->softness = DATA.softness;
        lSpot->update(); frus = sq::make_Frustum(lSpot->matrix);
    }
}
