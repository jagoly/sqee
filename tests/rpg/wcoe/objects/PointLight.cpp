#include <glm/gtc/matrix_transform.hpp>

#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>
#include <sqee/app/Settings.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "PointLight.hpp"

using namespace sqt::wcoe;

PointLight::PointLight(const string& _name, Cell* _cell) : Object(_name, _cell),
    tex(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, sq::Texture::ShadowMap()) {

    ubo.reserve("position", 4u);
    ubo.reserve("colour", 3u);
    ubo.reserve("intensity", 1u);
    ubo.reserve("matArr", 96u);
    ubo.reserve("modelMat", 16u);
    ubo.allocate_storage();
}

void PointLight::load_from_spec(const ObjSpec& _spec) {
    _spec.set_if("shadow", PROP_shadow);
    _spec.set_if("specular", PROP_specular);
    _spec.set_if("probes", PROP_probes);
    _spec.set_if("position", PROP_position);
    _spec.set_if("colour", PROP_colour);
    _spec.set_if("intensity", PROP_intensity);
    _spec.set_if("texsize", PROP_texsize);
}

void PointLight::refresh() {
    if (revalidate() == true) {
        if (PROP_shadow == true) {
            uint shadMult = settings.get<bool>("rpg_shadlarge") + 1u;
            tex.allocate_storage(PROP_texsize * shadMult, false);
        } else tex.delete_object();
    }

    animate();
}

void PointLight::update() {
    bool doFinish = false;
    if (ANIM_position.active())  if (ANIM_position.tick())  doFinish = true;
    if (ANIM_colour.active())    if (ANIM_colour.tick())    doFinish = true;
    if (ANIM_intensity.active()) if (ANIM_intensity.tick()) doFinish = true;
    if (doFinish == true) animate();
}

void PointLight::calc(double _accum) {
    bool doAnim = false;
    if (ANIM_position.active())  { ANIM_position.calc(_accum);  doAnim = true; }
    if (ANIM_colour.active())    { ANIM_colour.calc(_accum);    doAnim = true; }
    if (ANIM_intensity.active()) { ANIM_intensity.calc(_accum); doAnim = true; }
    if (doAnim == true) animate();
}

void PointLight::animate() {
    fvec3 position = PROP_position + cell->PROP_position;
    fmat4 projMat = glm::perspective(glm::radians(90.f), 1.f, 0.1f, PROP_intensity);
    matArr[0] = projMat * glm::lookAt(position, position+fvec3(+1.f, 0.f, 0.f), fvec3(0.f, -1.f, 0.f));
    matArr[1] = projMat * glm::lookAt(position, position+fvec3(-1.f, 0.f, 0.f), fvec3(0.f, -1.f, 0.f));
    matArr[2] = projMat * glm::lookAt(position, position+fvec3(0.f, +1.f, 0.f), fvec3(0.f, 0.f, +1.f));
    matArr[3] = projMat * glm::lookAt(position, position+fvec3(0.f, -1.f, 0.f), fvec3(0.f, 0.f, -1.f));
    matArr[4] = projMat * glm::lookAt(position, position+fvec3(0.f, 0.f, +1.f), fvec3(0.f, -1.f, 0.f));
    matArr[5] = projMat * glm::lookAt(position, position+fvec3(0.f, 0.f, -1.f), fvec3(0.f, -1.f, 0.f));
    frusArr[0] = sq::make_Frustum(matArr[0], position, fvec3(+1.f, 0.f, 0.f), 0.1f, PROP_intensity);
    frusArr[1] = sq::make_Frustum(matArr[1], position, fvec3(-1.f, 0.f, 0.f), 0.1f, PROP_intensity);
    frusArr[2] = sq::make_Frustum(matArr[2], position, fvec3(0.f, +1.f, 0.f), 0.1f, PROP_intensity);
    frusArr[3] = sq::make_Frustum(matArr[3], position, fvec3(0.f, -1.f, 0.f), 0.1f, PROP_intensity);
    frusArr[4] = sq::make_Frustum(matArr[4], position, fvec3(0.f, 0.f, +1.f), 0.1f, PROP_intensity);
    frusArr[5] = sq::make_Frustum(matArr[5], position, fvec3(0.f, 0.f, -1.f), 0.1f, PROP_intensity);
    modelMat = glm::scale(glm::translate(fmat4(), position), fvec3(PROP_intensity*2.f));
    sphere.origin = position; sphere.radius = PROP_intensity;

    ubo.update("position", &position);
    ubo.update("colour", &PROP_colour);
    ubo.update("intensity", &PROP_intensity);
    ubo.update("matArr", matArr.data());
    ubo.update("modelMat", &modelMat);
}
