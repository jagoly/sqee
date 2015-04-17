#include <glm/gtc/matrix_transform.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/render/pointlight.hpp>

using namespace sq;

PointLight::PointLight(bool _shadow, bool _uformbuf) : shadow(_shadow), uformbuf(_uformbuf) {
    if (uformbuf) {
        ubo.reset(new Uniformbuffer());
        ubo->reserve("position", 4);
        ubo->reserve("colour", 3);
        ubo->reserve("intensity", 1);
        if (shadow) ubo->reserve("matArr", 6*16);
        ubo->create();
    }
    if (shadow) {
        tex.reset(new TextureCube());
        tex->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
        tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        for (int face = 0; face < 6; face++) {
            fboArr[face].reset(new Framebuffer());
            fboArr[face]->attach(gl::DEPTH_ATTACHMENT, *tex, face);
        }
    }
}

void PointLight::update() {
    if (uformbuf) {
        ubo->bind(1);
        ubo->update("position", &position);
        ubo->update("colour", &colour);
        ubo->update("intensity", &intensity);
    }
    if (shadow) {
        mat4 projMat = glm::perspective(glm::radians(90.f), 1.f, 0.2f, intensity);
        for (int i = 0; i < 6; i++) {
            mat4 viewMat = glm::lookAt(position, position+sq::cubeNrms[i], sq::cubeTans[i]);
            matArr[i] = projMat * viewMat;
            frusArr[i] = sq::make_Frustum(glm::inverse(matArr[i]));
        }
        if (uformbuf) ubo->update("matArr", matArr.data());
    }
}

void PointLight::resize_texture(uint _power) {
    uint adjSize = texSize * std::pow(2, _power);
    tex->resize(adjSize);
}

void PointLight::filter_texture(bool _enable) {
    if (_enable) tex->set_preset(TextureCube::L_C());
    else tex->set_preset(TextureCube::N_C());
}
