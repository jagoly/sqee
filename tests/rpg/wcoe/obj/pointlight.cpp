#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/maths/culling.hpp>
#include <sqee/maths/general.hpp>

#include "../cell.hpp"
#include "pointlight.hpp"

using namespace sqt::wcoe;

PointLight::PointLight(const string& _name, Cell* _cell)
    : Object(ObjType::PointLight, _name, _cell) {
    ubo.reset(new sq::Uniformbuffer());
    ubo->reserve("position", 4);
    ubo->reserve("colour", 3);
    ubo->reserve("intensity", 1);
    ubo->reserve("matArr", 6*16);
    ubo->reserve("modelMat", 16);
    ubo->create();
}

void PointLight::load_from_spec(const ObjSpec& _spec) {
    SPEC_ASSERT_FLOAT("position", 3);
    SPEC_ASSERT_FLOAT("colour", 3);
    SPEC_ASSERT_FLOAT("intensity", 1);

    PROP_shadow    = SPEC_HAS_FLAG("shadow");
    PROP_specular  = SPEC_HAS_FLAG("specular");
    PROP_position  = glm::make_vec3(_spec.fMap.at("position").data());
    PROP_colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    PROP_intensity = _spec.fMap.at("intensity")[0];
    if (PROP_shadow) PROP_texsize = _spec.iMap.at("texsize")[0];
}

void PointLight::refresh() {
    if (PROP_shadow == true) {
        tex.reset(new sq::TextureCube());
        tex->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
        tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        fboArr[0].reset(new sq::Framebuffer()); fboArr[0]->attach(gl::DEPTH_ATTACHMENT, *tex, 0);
        fboArr[1].reset(new sq::Framebuffer()); fboArr[1]->attach(gl::DEPTH_ATTACHMENT, *tex, 1);
        fboArr[2].reset(new sq::Framebuffer()); fboArr[2]->attach(gl::DEPTH_ATTACHMENT, *tex, 2);
        fboArr[3].reset(new sq::Framebuffer()); fboArr[3]->attach(gl::DEPTH_ATTACHMENT, *tex, 3);
        fboArr[4].reset(new sq::Framebuffer()); fboArr[4]->attach(gl::DEPTH_ATTACHMENT, *tex, 4);
        fboArr[5].reset(new sq::Framebuffer()); fboArr[5]->attach(gl::DEPTH_ATTACHMENT, *tex, 5);
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
    matArr[0] = projMat * glm::lookAt(position, position+sq::cubeNrms[0], sq::cubeTans[0]);
    matArr[1] = projMat * glm::lookAt(position, position+sq::cubeNrms[1], sq::cubeTans[1]);
    matArr[2] = projMat * glm::lookAt(position, position+sq::cubeNrms[2], sq::cubeTans[2]);
    matArr[3] = projMat * glm::lookAt(position, position+sq::cubeNrms[3], sq::cubeTans[3]);
    matArr[4] = projMat * glm::lookAt(position, position+sq::cubeNrms[4], sq::cubeTans[4]);
    matArr[5] = projMat * glm::lookAt(position, position+sq::cubeNrms[5], sq::cubeTans[5]);
    frusArr[0] = sq::make_Frustum(matArr[0], position, sq::cubeNrms[0], 0.1, PROP_intensity);
    frusArr[1] = sq::make_Frustum(matArr[1], position, sq::cubeNrms[1], 0.1, PROP_intensity);
    frusArr[2] = sq::make_Frustum(matArr[2], position, sq::cubeNrms[2], 0.1, PROP_intensity);
    frusArr[3] = sq::make_Frustum(matArr[3], position, sq::cubeNrms[3], 0.1, PROP_intensity);
    frusArr[4] = sq::make_Frustum(matArr[4], position, sq::cubeNrms[4], 0.1, PROP_intensity);
    frusArr[5] = sq::make_Frustum(matArr[5], position, sq::cubeNrms[5], 0.1, PROP_intensity);
    modelMat = glm::scale(glm::translate(fmat4(), position), fvec3(PROP_intensity*2.f));
    sphere.origin = position; sphere.radius = PROP_intensity;

    ubo->bind(1);
    ubo->update("position", &position);
    ubo->update("colour", &PROP_colour);
    ubo->update("intensity", &PROP_intensity);
    ubo->update("matArr", matArr.data());
    ubo->update("modelMat", &modelMat);
}
