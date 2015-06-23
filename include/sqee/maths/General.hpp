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

fvec3 make_tangent(fvec3 _norm);

fmat3 make_normMat(fmat4 _viewModelMat);

fvec3 norm_from_tri(fvec3 _t0, fvec3 _t1, fvec3 _t2);

}
