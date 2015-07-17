#include <glm/gtc/matrix_transform.hpp>

#include <sqee/maths/General.hpp>

using namespace sq;

fvec3 sq::make_tangent(fvec3 _norm) {
    fvec3 tX = glm::cross(_norm, {1.f, 0.f, 0.f});
    fvec3 tY = glm::cross(_norm, {0.f, 1.f, 0.f});
    fvec3 tZ = glm::cross(_norm, {0.f, 0.f, 1.f});
    fvec3 retTan = glm::length(tX) > glm::length(tY) ? tX : tY;
    retTan = glm::length(tZ) > glm::length(retTan) ? tZ : retTan;
    return glm::normalize(retTan);
}


fmat3 sq::make_normMat(fmat4 _viewModelMat) {
    return glm::transpose(glm::inverse(fmat3(_viewModelMat)));
}


fvec3 sq::norm_from_tri(fvec3 _t0, fvec3 _t1, fvec3 _t2) {
    fvec3 p0 = _t1 - _t0; fvec3 p1 = _t2 - _t0;
    return glm::normalize(glm::cross(p0, p1));
}
