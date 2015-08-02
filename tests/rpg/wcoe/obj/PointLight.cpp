#include <glm/gtc/matrix_transform.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/UniformBuffer.hpp>
#include <sqee/gl/FrameBuffer.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/maths/Culling.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "PointLight.hpp"

using namespace sqt::wcoe;

PointLight::PointLight(const string& _name, Cell* _cell)
    : Object(ObjType::PointLight, _name, _cell) {
    ubo.reset(new sq::UniformBuffer());
    ubo->reserve("position", 4);
    ubo->reserve("colour", 3);
    ubo->reserve("intensity", 1);
    ubo->reserve("matArr", 6*16);
    ubo->reserve("modelMat", 16);
    ubo->create();
}

void PointLight::load_from_spec(const ObjSpec& _spec) {
    assert_fvec3(_spec, name, "position");
    assert_fvec3(_spec, name, "colour");
    assert_float(_spec, name, "intensity");
    assert_uint(_spec, name, "texsize");

    PROP_shadow    = _spec.flagSet.count("shadow");
    PROP_specular  = _spec.flagSet.count("specular");
    PROP_position  = _spec.fvec3Map.at("position");
    PROP_colour    = _spec.fvec3Map.at("colour");
    PROP_intensity = _spec.floatMap.at("intensity");
    PROP_texsize   = _spec.uintMap.at("texsize");
}

void PointLight::refresh() {
    if (PROP_shadow == true) {
        tex.reset(new sq::TextureCube());
        tex->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
        tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        fboArr[0].reset(new sq::FrameBuffer()); fboArr[0]->attach(gl::DEPTH_ATTACHMENT, *tex, 0);
        fboArr[1].reset(new sq::FrameBuffer()); fboArr[1]->attach(gl::DEPTH_ATTACHMENT, *tex, 1);
        fboArr[2].reset(new sq::FrameBuffer()); fboArr[2]->attach(gl::DEPTH_ATTACHMENT, *tex, 2);
        fboArr[3].reset(new sq::FrameBuffer()); fboArr[3]->attach(gl::DEPTH_ATTACHMENT, *tex, 3);
        fboArr[4].reset(new sq::FrameBuffer()); fboArr[4]->attach(gl::DEPTH_ATTACHMENT, *tex, 4);
        fboArr[5].reset(new sq::FrameBuffer()); fboArr[5]->attach(gl::DEPTH_ATTACHMENT, *tex, 5);
    } else { tex.reset(); for (int i=0; i<6; i++) fboArr[i].reset(); }

    animate();
}

void PointLight::tick() {
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
    fvec3 position = PROP_position + cell->DAT_position;
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

    ubo->bind(1);
    ubo->update("position", &position);
    ubo->update("colour", &PROP_colour);
    ubo->update("intensity", &PROP_intensity);
    ubo->update("matArr", matArr.data());
    ubo->update("modelMat", &modelMat);
}
