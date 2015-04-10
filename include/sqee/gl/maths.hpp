#pragma once
#include <sqee/forward.hpp>

namespace sq {

dvec3 make_tangent(dvec3 _norm);

dmat3 make_normMat(dmat4 _viewModelMat);

struct Frustum { dvec3 xyz, xyZ, xYz, xYZ, Xyz, XyZ, XYz, XYZ; };

bool point_in_tri(dvec2 _p, dvec2 _t0, dvec2 _t1, dvec2 _t2);

dvec3 norm_from_tri(dvec3 _t0, dvec3 _t1, dvec3 _t2);

double dist_from_plane(dvec3 _point, dvec4 _plane);

bool sphere_in_frus(dvec4 _sphere, dmat4 _mat, Frustum _frus);

Frustum make_Frustum(dmat4 _invProjViewMat);

dvec3 calc_frusCentre(Frustum _frus);

}
