#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/maths.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/gl/uniformbuffers.hpp>

#include "../cell.hpp"
#include "pointlight.hpp"

using namespace sqt::wcoe;

PointLight::PointLight(const string& _name, const Cell* _cell)
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
    DAT_shadow = _spec.flags.count("shadow");
    DAT_specular = _spec.flags.count("specular");

    DAT_position  = glm::make_vec3(_spec.fMap.at("position").data());
    DAT_colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    DAT_intensity = _spec.fMap.at("intensity")[0];
    if (DAT_shadow) DAT_texsize = _spec.iMap.at("texsize")[0];
}

void PointLight::update_from_data() {
    vec3 position = DAT_position + cell->position;

    ubo->bind(1);
    ubo->update("position", &position);
    ubo->update("colour", &DAT_colour);
    ubo->update("intensity", &DAT_intensity);

    mat4 projMat = glm::perspective(glm::radians(90.f), 1.f, 0.2f, DAT_intensity);
    matArr[0] = projMat * glm::lookAt(position, position+sq::cubeNrms[0], sq::cubeTans[0]);
    matArr[1] = projMat * glm::lookAt(position, position+sq::cubeNrms[1], sq::cubeTans[1]);
    matArr[2] = projMat * glm::lookAt(position, position+sq::cubeNrms[2], sq::cubeTans[2]);
    matArr[3] = projMat * glm::lookAt(position, position+sq::cubeNrms[3], sq::cubeTans[3]);
    matArr[4] = projMat * glm::lookAt(position, position+sq::cubeNrms[4], sq::cubeTans[4]);
    matArr[5] = projMat * glm::lookAt(position, position+sq::cubeNrms[5], sq::cubeTans[5]);
    for (int i=0; i<6; i++) frusArr[i] = sq::make_Frustum(matArr[i]);

    sphere.origin = position; sphere.radius = DAT_intensity;
    modelMat = glm::scale(glm::translate(mat4(), position), vec3(DAT_intensity*2.f));

    ubo->update("matArr", matArr.data());
    ubo->update("modelMat", &modelMat);

    if (DAT_shadow == true) {
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
}
