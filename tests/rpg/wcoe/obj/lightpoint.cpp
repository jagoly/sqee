#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/lightpoint.hpp>

#include "../cell.hpp"
#include "lightpoint.hpp"

using namespace sqt::wcoe;

LightPoint::LightPoint(const string& _name, const Cell* _cell)
    : Object(ObjType::LightPoint, _name, _cell) {}

void LightPoint::load_from_spec(const ObjSpec& _spec) {
    DATA.shadow = _spec.flags.count("shadow");
    DATA.diffuse = _spec.flags.count("diffuse");
    DATA.specular = _spec.flags.count("specular");

    DATA.position  = glm::make_vec3(_spec.fMap.at("position").data());
    DATA.colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    DATA.intensity = _spec.fMap.at("intensity")[0];
    if (DATA.shadow) DATA.texsize = _spec.iMap.at("texsize")[0];
}

void LightPoint::update_from_data() {
    sphere.origin = DATA.position;
    sphere.radius = DATA.intensity;

    if (DATA.shadow) {
        sPoint.reset(new sq::ShadowPoint()); lPoint.reset();
        sPoint->position = DATA.position + cell->position;
        sPoint->colour = DATA.colour;
        sPoint->intensity = DATA.intensity;
        sPoint->texSize = DATA.texsize;
        sPoint->update(); for (int i = 0; i < 6; i++)
            frusArr[i] = sq::make_Frustum(sPoint->matArr[i]);
    } else {
        lPoint.reset(new sq::LightPoint()); sPoint.reset();
        lPoint->position = DATA.position + cell->position;
        lPoint->colour = DATA.colour;
        lPoint->intensity = DATA.intensity;
        lPoint->update(); for (int i = 0; i < 6; i++)
            frusArr[i] = sq::make_Frustum(lPoint->matArr[i]);
    }
}
