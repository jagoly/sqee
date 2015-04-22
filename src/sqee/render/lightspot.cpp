#include <glm/gtc/matrix_transform.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/render/lightspot.hpp>

using namespace sq;

LightSpot::LightSpot(bool _shadow) : shadow(_shadow) {
    ubo.reset(new Uniformbuffer());
    ubo->reserve("position", 3);
    ubo->reserve("angle", 1);
    ubo->reserve("direction", 3);
    ubo->reserve("intensity", 1);
    ubo->reserve("colour", 3);
    ubo->reserve("softness", 1);
    if (shadow) {
        tex.reset(new Texture2D());
        tex->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
        tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        fbo.reset(new Framebuffer());
        fbo->attach(gl::DEPTH_ATTACHMENT, *tex);
        ubo->reserve("matrix", 16);
    } ubo->create();
}

void LightSpot::update() {
    ubo->bind(1);
    ubo->update("position", &position);
    ubo->update("angle", &angle);
    ubo->update("direction", &direction);
    ubo->update("intensity", &intensity);
    ubo->update("colour", &colour);
    ubo->update("softness", &softness);
    if (shadow) {
        vec3 tangent = make_tangent(direction);
        mat4 viewMat = glm::lookAt(position, position+direction, tangent);
        mat4 projMat = glm::perspective(angle * 2.f, 1.f, 0.2f, intensity);
        matrix = projMat * viewMat;
        frus = make_Frustum(glm::inverse(matrix));
        ubo->update("matrix", &matrix);
    }
}

void LightSpot::resize_texture(uint _power) {
    uint adjSize = texSize * std::pow(2, _power);
    tex->resize(uvec2(adjSize, adjSize));
}

void LightSpot::filter_texture(bool _enable) {
    if (_enable) tex->set_preset(Texture2D::L_C());
    else tex->set_preset(Texture2D::N_C());
}
