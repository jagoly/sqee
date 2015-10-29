#include <sqee/maths/General.hpp>

using namespace sq;

Vec3F sq::make_tangent(const Vec3F& _norm) {
    Vec3F tX = maths::cross(_norm, Vec3F(1.f, 0.f, 0.f));
    Vec3F tY = maths::cross(_norm, Vec3F(0.f, 1.f, 0.f));
    Vec3F tZ = maths::cross(_norm, Vec3F(0.f, 0.f, 1.f));
    Vec3F tang = maths::length(tX) > maths::length(tY) ? tX : tY;
    tang = maths::length(tZ) > maths::length(tang) ? tZ : tang;
    return maths::normalize(tang);
}


Mat3F sq::make_normMat(const Mat4F& _viewModelMat) {
    return maths::transpose(maths::inverse(Mat3F(_viewModelMat)));
}


Vec3F sq::norm_from_tri(const Vec3F& _t0, const Vec3F& _t1, const Vec3F& _t2) {
    return maths::normalize(maths::cross(_t1-_t0, _t2-_t0));
}
