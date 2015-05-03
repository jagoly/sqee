#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/render/cameras.hpp>
#include <sqee/render/lightsky.hpp>

#include "../../main/camera.hpp"
#include "../cell.hpp"
#include "../world.hpp"
#include "lightsky.hpp"

using namespace sqt::wcoe;

LightSky::LightSky(const string& _name, const Cell* _cell)
    : Object(ObjType::LightSky, _name, _cell) {}

void LightSky::load_from_spec(const ObjSpec& _spec) {
    DATA.shadow = _spec.flags.count("shadow");
    DATA.diffuse = _spec.flags.count("diffuse");
    DATA.specular = _spec.flags.count("specular");

    DATA.direction = glm::make_vec3(_spec.fMap.at("direction").data());
    DATA.colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    if (DATA.shadow) DATA.texsize = _spec.iMap.at("texsize")[0];
}

void LightSky::update_from_data() {
    if (DATA.shadow) {
        sSky.reset(new sq::ShadowSky()); lSky.reset();
        sSky->direction = DATA.direction;
        sSky->colour = DATA.colour;
        sSky->camera = cell->world->camera;
        sSky->texSize = DATA.texsize;
        sSky->update();
    } else {
        lSky.reset(new sq::LightSky()); sSky.reset();
        lSky->direction = DATA.direction;
        lSky->colour = DATA.colour;
        lSky->update();
    }
}

void LightSky::tick() {
    if (DATA.shadow) sSky->update();
}
