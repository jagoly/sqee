#include <glm/gtc/matrix_transform.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/render/lightpoint.hpp>

using namespace sq;

LightPoint::LightPoint() {
    ubo.reset(new Uniformbuffer());
    ubo->reserve("position", 4);
    ubo->reserve("colour", 3);
    ubo->reserve("intensity", 1);
    ubo->reserve("matArr", 6*16);
    ubo->create();
}

void LightPoint::update() {
    ubo->bind(1);
    ubo->update("position", &position);
    ubo->update("colour", &colour);
    ubo->update("intensity", &intensity);

    mat4 projMat = glm::perspective(glm::radians(90.f), 1.f, 0.2f, intensity);
    matArr[0] = projMat * glm::lookAt(position, position+sq::cubeNrms[0], sq::cubeTans[0]);
    matArr[1] = projMat * glm::lookAt(position, position+sq::cubeNrms[1], sq::cubeTans[1]);
    matArr[2] = projMat * glm::lookAt(position, position+sq::cubeNrms[2], sq::cubeTans[2]);
    matArr[3] = projMat * glm::lookAt(position, position+sq::cubeNrms[3], sq::cubeTans[3]);
    matArr[4] = projMat * glm::lookAt(position, position+sq::cubeNrms[4], sq::cubeTans[4]);
    matArr[5] = projMat * glm::lookAt(position, position+sq::cubeNrms[5], sq::cubeTans[5]);
    ubo->update("matArr", matArr.data());
}


ShadowPoint::ShadowPoint() {
    ubo.reset(new Uniformbuffer());
    ubo->reserve("position", 4);
    ubo->reserve("colour", 3);
    ubo->reserve("intensity", 1);
    ubo->reserve("matArr", 6*16);
    ubo->create();

    tex.reset(new TextureCube());
    tex->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
    tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
    fboArr[0].reset(new Framebuffer()); fboArr[0]->attach(gl::DEPTH_ATTACHMENT, *tex, 0);
    fboArr[1].reset(new Framebuffer()); fboArr[1]->attach(gl::DEPTH_ATTACHMENT, *tex, 1);
    fboArr[2].reset(new Framebuffer()); fboArr[2]->attach(gl::DEPTH_ATTACHMENT, *tex, 2);
    fboArr[3].reset(new Framebuffer()); fboArr[3]->attach(gl::DEPTH_ATTACHMENT, *tex, 3);
    fboArr[4].reset(new Framebuffer()); fboArr[4]->attach(gl::DEPTH_ATTACHMENT, *tex, 4);
    fboArr[5].reset(new Framebuffer()); fboArr[5]->attach(gl::DEPTH_ATTACHMENT, *tex, 5);
}

void ShadowPoint::update() {
    ubo->bind(1);
    ubo->update("position", &position);
    ubo->update("colour", &colour);
    ubo->update("intensity", &intensity);

    mat4 projMat = glm::perspective(glm::radians(90.f), 1.f, 0.2f, intensity);
    matArr[0] = projMat * glm::lookAt(position, position+sq::cubeNrms[0], sq::cubeTans[0]);
    matArr[1] = projMat * glm::lookAt(position, position+sq::cubeNrms[1], sq::cubeTans[1]);
    matArr[2] = projMat * glm::lookAt(position, position+sq::cubeNrms[2], sq::cubeTans[2]);
    matArr[3] = projMat * glm::lookAt(position, position+sq::cubeNrms[3], sq::cubeTans[3]);
    matArr[4] = projMat * glm::lookAt(position, position+sq::cubeNrms[4], sq::cubeTans[4]);
    matArr[5] = projMat * glm::lookAt(position, position+sq::cubeNrms[5], sq::cubeTans[5]);
    ubo->update("matArr", matArr.data());
}

void ShadowPoint::resize_texture(uint _power) {
    uint adjSize = texSize * std::pow(2, _power);
    tex->resize(adjSize);
}

void ShadowPoint::filter_texture(bool _enable) {
    if (_enable) tex->set_preset(TextureCube::L_C());
    else tex->set_preset(TextureCube::N_C());
}
