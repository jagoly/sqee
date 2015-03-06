#include <glm/gtc/matrix_transform.hpp>

#include "gl/maths.hpp"

using namespace sq;

Frustum sq::make_worldFrustum(const glm::mat4& _invProjView) {
    Frustum fr; glm::vec4 temp;

    temp = _invProjView * glm::vec4(-1, -1, -1, 1);
    fr.xyz = glm::vec3(temp) / temp.w;
    temp = _invProjView * glm::vec4(-1, -1,  1, 1);
    fr.xyZ = glm::vec3(temp) / temp.w;
    temp = _invProjView * glm::vec4(-1,  1, -1, 1);
    fr.xYz = glm::vec3(temp) / temp.w;
    temp = _invProjView * glm::vec4(-1,  1,  1, 1);
    fr.xYZ = glm::vec3(temp) / temp.w;
    temp = _invProjView * glm::vec4( 1, -1, -1, 1);
    fr.Xyz = glm::vec3(temp) / temp.w;
    temp = _invProjView * glm::vec4( 1, -1,  1, 1);
    fr.XyZ = glm::vec3(temp) / temp.w;
    temp = _invProjView * glm::vec4( 1,  1, -1, 1);
    fr.XYz = glm::vec3(temp) / temp.w;
    temp = _invProjView * glm::vec4( 1,  1,  1, 1);
    fr.XYZ = glm::vec3(temp) / temp.w;

    return fr;
}

glm::vec3 sq::calc_frusCentre(const Frustum& _frus) {
    glm::vec3 min(INFINITY, INFINITY, INFINITY);
    glm::vec3 max(-INFINITY, -INFINITY, -INFINITY);
    for (auto& vec : {_frus.xyz, _frus.xyZ, _frus.xYz, _frus.xYZ,
                      _frus.Xyz, _frus.XyZ, _frus.XYz, _frus.XYZ}) {
        min = glm::min(min, vec);
        max = glm::max(max, vec);
    }
    return (min + max) / 2.f;
}

glm::mat4 sq::make_viewMat(const glm::vec3& _pos, const glm::vec3& _dir) {
    // TODO: fix for up vector
    return glm::lookAt(_pos, _pos+_dir, {0.f, 0.f, 1.f});
}

glm::mat4 sq::make_persMat(const glm::vec2& _range, const glm::vec2& _size, float _fov) {
    return glm::perspective(_fov, _size.x / _size.y, _range.x, _range.y);
}

glm::mat4 sq::make_orthMat(const glm::vec3& _min, const glm::vec3& _max) {
    return glm::ortho(_min.x, _max.x, _min.y, _max.y, _min.z, _max.z);
}
