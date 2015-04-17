#include <glm/gtc/matrix_transform.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/render/skylight.hpp>

using namespace sq;

SkyLight::SkyLight(bool _shadow, bool _uformbuf) : shadow(_shadow), uformbuf(_uformbuf) {
    if (uformbuf) {
        ubo.reset(new Uniformbuffer());
        ubo->reserve("colour", 4);
        ubo->reserve("direction", 4);
        if (shadow) ubo->reserve("matArr", 4*16);
        ubo->create();
    }
    if (shadow) {
        tex.reset(new Texture2DArray());
        tex->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
        tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        for (int csm = 0; csm < 4; csm++) {
            fboArr[csm].reset(new Framebuffer());
            fboArr[csm]->attach(gl::DEPTH_ATTACHMENT, *tex, csm);
        }
    }
}

void SkyLight::update() {
    if (uformbuf) {
        ubo->bind(1);
        ubo->update("colour", &colour);
        ubo->update("direction", &direction);
    }
    if (shadow) {
        vec3 tangent = sq::make_tangent(direction);
        for (int i = 0; i < 4; i++) {
            const auto& csm = (*csmArr)[i];
            vec3 frusCentre = sq::calc_frusCentre(csm.second);
            mat4 viewMat = glm::lookAt(frusCentre-direction, frusCentre, tangent);
            mat4 projMat = glm::ortho(-csm.first, csm.first, -csm.first, csm.first, -csm.first, csm.first);
            matArr[i] = projMat * viewMat;
            frusArr[i] = sq::make_Frustum(glm::inverse(matArr[i]));
        }
        if (uformbuf) ubo->update("matArr", matArr.data());
    }
}

void SkyLight::resize_texture(uint _power) {
    uint adjSize = texSize * std::pow(2, _power);
    tex->resize(uvec3(adjSize, adjSize, 4));
}

void SkyLight::filter_texture(bool _enable) {
    if (_enable) tex->set_preset(Texture2DArray::L_C());
    else tex->set_preset(Texture2DArray::N_C());
}
