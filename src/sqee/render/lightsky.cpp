#include <glm/gtc/matrix_transform.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/uniformbuffers.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/gl/framebuffers.hpp>
#include <sqee/render/cameras.hpp>
#include <sqee/render/lightsky.hpp>

using namespace sq;

LightSky::LightSky(bool _shadow) : shadow(_shadow) {
    ubo.reset(new Uniformbuffer());
    ubo->reserve("colour", 4);
    ubo->reserve("direction", 4);
    if (shadow) {
        tex.reset(new Texture2DArray());
        tex->create(gl::DEPTH_COMPONENT, gl::DEPTH_COMPONENT16, 1);
        tex->set_param(gl::TEXTURE_COMPARE_MODE, gl::COMPARE_REF_TO_TEXTURE);
        for (int csm = 0; csm < 4; csm++) {
            fboArr[csm].reset(new Framebuffer());
            fboArr[csm]->attach(gl::DEPTH_ATTACHMENT, *tex, csm);
        } ubo->reserve("splits", 4); ubo->reserve("matArr", 4*16);
    } ubo->create();
}

void LightSky::update() {
    ubo->bind(1);
    ubo->update("colour", &colour);
    ubo->update("direction", &direction);
    if (shadow) {
        const float weight = 0.666f;
        float prevSplit = camera->range.x;
        for (uint sliceInd = 0; sliceInd < 4; sliceInd++) {
            float f = float(sliceInd+1) / 4.f;
            float splitUni = camera->range.x + (camera->range.y - camera->range.x) * f;
            float splitLog = camera->range.x * glm::pow(camera->range.y / camera->range.x, f);
            float splitMix = glm::mix(splitUni, splitLog, weight);
            mat4 proj = glm::perspective(camera->fov, camera->size.x/camera->size.y, prevSplit, splitMix);
            splitArr[sliceInd] = splitMix;
            csmfrArr[sliceInd] = sq::make_Frustum(glm::inverse(proj * camera->viewMat));
            prevSplit = splitMix;
        }

        vec3 tangent = sq::make_tangent(direction);
        for (int i = 0; i < 4; i++) {
            const auto& split = splitArr[i]; const auto& csmfr = csmfrArr[i];
            vec3 frusCentre = sq::calc_frusCentre(csmfr);
            mat4 viewMat = glm::lookAt(frusCentre-direction, frusCentre, tangent);
            mat4 projMat = glm::ortho(-split, split, -split, split, -split, split);
            matArr[i] = projMat * viewMat;
            frusArr[i] = sq::make_Frustum(glm::inverse(matArr[i]));
        } ubo->update("splits", splitArr.data()); ubo->update("matArr", matArr.data());
    }
}

void LightSky::resize_texture(uint _power) {
    uint adjSize = texSize * std::pow(2, _power);
    tex->resize(uvec3(adjSize, adjSize, 4));
}

void LightSky::filter_texture(bool _enable) {
    if (_enable) tex->set_preset(Texture2DArray::L_C());
    else tex->set_preset(Texture2DArray::N_C());
}
