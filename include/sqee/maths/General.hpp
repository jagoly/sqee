#pragma once
#include <sqee/forward.hpp>

namespace sq {

fvec3 make_tangent(fvec3 _norm);

fmat3 make_normMat(fmat4 _viewModelMat);

fvec3 norm_from_tri(fvec3 _t0, fvec3 _t1, fvec3 _t2);

}
