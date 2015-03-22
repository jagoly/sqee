#pragma once
#include "forward.hpp"

#include <glm/vector_relational.hpp>

namespace sq {

struct AABbox {

};

vec3 make_tangent(vec3 _norm);

struct Frustum { vec3 xyz, xyZ, xYz, xYZ, Xyz, XyZ, XYz, XYZ; };

bool point_in_tri(vec2 _p, vec2 _t0, vec2 _t1, vec2 _t2);

template<class T>
bool within_box(const T& _val, const T& _min, const T& _max) {
    return glm::all(glm::greaterThanEqual(_val, _min) + glm::lessThan(_val, _max));
}

Frustum make_Frustum(const mat4& _invProjView);
vec3 calc_frusCentre(const Frustum& _frus);

Frustum make_boxFrustum(const mat4& _invProjView);

}
