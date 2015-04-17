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

struct Frustum { dvec3 xyz, xyZ, xYz, xYZ, Xyz, XyZ, XYz, XYZ; dvec4 pT, pB, pL, pR, pN, pF; };

struct BoundBox { dvec3 origin, size; double radius; dvec3 nX{1,0,0}, nY{0,1,0}, nZ{0,0,1}; };

bool point_in_tri(dvec2 _p, dvec2 _t0, dvec2 _t1, dvec2 _t2);

dvec3 norm_from_tri(dvec3 _t0, dvec3 _t1, dvec3 _t2);

BoundBox bbox_by_model(BoundBox _bbox, dmat4 _modelMat);

bool bbox_in_frus(BoundBox _bbox, Frustum _frus);

bool frus_in_frus(Frustum _frusA, Frustum _frusB);

bool sphere_in_frus(dvec4 _sphere, Frustum _frus);

Frustum make_Frustum(dmat4 _invProjViewMat);

dvec3 calc_frusCentre(Frustum _frus);

}
