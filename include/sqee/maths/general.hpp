#pragma once
#include <sqee/forward.hpp>

namespace sq {

const array<vec3, 6> cubeNrms = {
    vec3(1.f, 0.f, 0.f), vec3(-1.f, 0.f, 0.f),
    vec3(0.f, 1.f, 0.f), vec3(0.f, -1.f, 0.f),
    vec3(0.f, 0.f, 1.f), vec3(0.f, 0.f, -1.f)
};
const array<vec3, 6> cubeTans = {
    vec3(0.f, -1.f, 0.f), vec3(0.f, -1.f, 0.f),
    vec3(0.f, 0.f, 1.f), vec3(0.f, 0.f, -1.f),
    vec3(0.f, -1.f, 0.f), vec3(0.f, -1.f, 0.f)
};

dvec3 make_tangent(dvec3 _norm);

dmat3 make_normMat(dmat4 _viewModelMat);

dvec3 norm_from_tri(dvec3 _t0, dvec3 _t1, dvec3 _t2);

}
