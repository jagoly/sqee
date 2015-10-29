#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq {

Vec3F make_tangent(const Vec3F& _norm);

Mat3F make_normMat(const Mat4F& _viewModelMat);

Vec3F norm_from_tri(const Vec3F& _t0, const Vec3F& _t1, const Vec3F& _t2);

}
