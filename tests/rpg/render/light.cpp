#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "light.hpp"

using namespace sqt;

////////////////
/// SkyLight ///
////////////////

SkyLight::SkyLight() {
    ubo.reserve("colour", 4);
    ubo.reserve("direction", 4);
    ubo.reserve("matArr", 4*16);
    ubo.create();
}

void SkyLight::update() {
    vec3 t1(glm::cross(direction, {1, 0, 0})), t2(glm::cross(direction, {0, 1, 0}));
    vec3 tangent = glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
    for (int i = 0; i < 4; i++) {
        const auto& csm = camera->csmArr[i];
        vec3 frusCentre = sq::calc_frusCentre(csm.second);
        mat4 viewMat = glm::lookAt(frusCentre-direction, frusCentre, tangent);
        vec3 arr[8] {
            vec3(viewMat * vec4(csm.second.xyz, 1)),
            vec3(viewMat * vec4(csm.second.xyZ, 1)),
            vec3(viewMat * vec4(csm.second.xYz, 1)),
            vec3(viewMat * vec4(csm.second.xYZ, 1)),
            vec3(viewMat * vec4(csm.second.Xyz, 1)),
            vec3(viewMat * vec4(csm.second.XyZ, 1)),
            vec3(viewMat * vec4(csm.second.XYz, 1)),
            vec3(viewMat * vec4(csm.second.XYZ, 1))
        };

        vec3 minO = {INFINITY, INFINITY, INFINITY};
        vec3 maxO = {-INFINITY, -INFINITY, -INFINITY};
        for (auto& vec : arr) {
            minO = glm::min(minO, vec);
            maxO = glm::max(maxO, vec);
        }
        minO.z = glm::min(minO.z, glm::min(minO.x, minO.y));
        maxO.z = glm::max(maxO.z, glm::max(maxO.x, maxO.y));

        mat4 projMat = glm::ortho(minO.x, maxO.x, minO.y, maxO.y, minO.z, maxO.z);
        matArr[i] = projMat * viewMat;
    }
}

void SkyLight::bind_ubo() {
    ubo.bind(1);
    ubo.update("colour", &colour);
    ubo.update("direction", &direction);
    ubo.update("matArr", matArr.data());
}


/////////////////
/// SpotLight ///
/////////////////

SpotLight::SpotLight() {
    ubo.reserve("position", 3);
    ubo.reserve("angle", 1);
    ubo.reserve("direction", 3);
    ubo.reserve("intensity", 1);
    ubo.reserve("colour", 3);
    ubo.reserve("softness", 1);
    ubo.reserve("matrix", 16);
    ubo.create();
}

void SpotLight::update() {
    vec3 t1(glm::cross(direction, {1, 0, 0})), t2(glm::cross(direction, {0, 1, 0}));
    vec3 tangent = glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
    mat4 viewMat = glm::lookAt(position, position+direction, tangent);
    mat4 projMat = glm::perspective(angle * 2.f, 1.f, 1.5f, intensity);
    matrix = projMat * viewMat;
}

void SpotLight::bind_ubo() {
    ubo.bind(1);
    ubo.update("position", &position);
    ubo.update("angle", &angle);
    ubo.update("direction", &direction);
    ubo.update("intensity", &intensity);
    ubo.update("colour", &colour);
    ubo.update("softness", &softness);
    ubo.update("matrix", &matrix);
}
