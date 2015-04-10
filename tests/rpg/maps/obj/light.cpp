#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/maths.hpp>

#include "../cell.hpp"
#include "light.hpp"

using namespace sqt;

Light::Light(const ObjSpec& _spec) : Object(_spec) {
    pos       = glm::make_vec3(_spec.fMap.at("pos").data()) + cell.pos;
    dir       = glm::make_vec3(_spec.fMap.at("dir").data());
    colour    = glm::make_vec3(_spec.fMap.at("colour").data());
    angle     = glm::radians(_spec.fMap.at("angIntSft")[0]) / 2.f;
    intensity = _spec.fMap.at("angIntSft")[1];
    softness  = _spec.fMap.at("angIntSft")[2];
    texSize   = _spec.iMap.at("texSize")[0];

    vec3 tangent = sq::make_tangent(dir);
    mat4 viewMat = glm::lookAt(pos, pos+dir, tangent);
    mat4 projMat = glm::perspective(angle * 2.f, 1.f, 0.1f, intensity);

    shadMat = projMat * viewMat;
}
