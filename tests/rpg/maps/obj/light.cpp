#include "../../resbank.hpp"
#include "light.hpp"

using namespace sqt;

Light::Light(const ObjectSpec& _spec) : Object(_spec) {
    pos       = glm::make_vec3(_spec.fMap.at("pos").data()) + cellPos;
    dir       = glm::make_vec3(_spec.fMap.at("dir").data());
    colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    angle     = glm::radians(_spec.fMap.at("angIntSft")[0]) / 2.f;
    intensity = _spec.fMap.at("angIntSft")[1];
    softness  = _spec.fMap.at("angIntSft")[2];
    texSize   = _spec.iMap.at("texSize")[0];

    glm::vec3 t1(glm::cross(dir, {0, 1, 0})), t2(glm::cross(dir, {0, 0, 1}));
    glm::vec3 tangent = glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
    glm::mat4 viewMat = glm::lookAt(pos, pos+dir, tangent);
    glm::mat4 projMat = glm::perspective(angle * 2.f, 1.f, 0.1f, intensity);

    shadMat = projMat * viewMat;
}
