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

LightSky::LightSky(const string& _name, const Cell& _cell)
    : Object(ObjType::LightSky, _name, _cell) {}

void LightSky::load_from_spec(const ObjSpec& _spec) {
    shadow = _spec.flags.count("shadow");
    diffuse = _spec.flags.count("diffuse");
    specular = _spec.flags.count("specular");

    sky.reset(new sq::LightSky(shadow));
    sky->direction = glm::make_vec3(_spec.fMap.at("direction").data());
    sky->colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    if (shadow) sky->texSize = _spec.iMap.at("texsize")[0],
                sky->camera = static_cast<sq::Camera*>(cell.world.camera);
}

void LightSky::tick() {
    sky->update();
}
