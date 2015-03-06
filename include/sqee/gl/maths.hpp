#pragma once
#include "forward.hpp"

#include <glm/vector_relational.hpp>

namespace sq {

struct Frustum { glm::vec3 xyz, xyZ, xYz, xYZ, Xyz, XyZ, XYz, XYZ; };

template<class T>
bool within_box(const T& _val, const T& _min, const T& _max) {
    return glm::all(glm::greaterThanEqual(_val, _min) + glm::lessThan(_val, _max));
}

Frustum make_worldFrustum(const glm::mat4& _invProjView);
glm::vec3 calc_frusCentre(const Frustum& _frus);
glm::mat4 make_viewMat(const glm::vec3& _pos, const glm::vec3& _dir);
glm::mat4 make_persMat(const glm::vec2& _range, const glm::vec2& _size, float _fov);
glm::mat4 make_orthMat(const glm::vec3& _min, const glm::vec3& _max);

}
