#pragma once
#include <sqee/forward.hpp>

namespace sq {

const array<fvec3, 6> cubeNrms = {
    fvec3(1.f, 0.f, 0.f), fvec3(-1.f, 0.f, 0.f),
    fvec3(0.f, 1.f, 0.f), fvec3(0.f, -1.f, 0.f),
    fvec3(0.f, 0.f, 1.f), fvec3(0.f, 0.f, -1.f)
};
const array<fvec3, 6> cubeTans = {
    fvec3(0.f, -1.f, 0.f), fvec3(0.f, -1.f, 0.f),
    fvec3(0.f,  0.f, 1.f), fvec3(0.f, 0.f, -1.f),
    fvec3(0.f, -1.f, 0.f), fvec3(0.f, -1.f, 0.f)
};

dvec3 make_tangent(dvec3 _norm);

dmat3 make_normMat(dmat4 _viewModelMat);

dmat3 make_normMat(dmat4 _viewMat, dmat4 _modelMat);

dvec3 norm_from_tri(dvec3 _t0, dvec3 _t1, dvec3 _t2);

}
