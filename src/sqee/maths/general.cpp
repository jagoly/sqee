#include <glm/gtc/matrix_transform.hpp>

#include <sqee/maths/general.hpp>

using namespace sq;

fvec3 sq::make_tangent(fvec3 _norm) {
    fvec3 t1(glm::cross(_norm, {1, 0, 0})), t2(glm::cross(_norm, {0, 1, 0}));
    return glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
}

fmat3 sq::make_normMat(fmat4 _viewModelMat) {
    return glm::transpose(glm::inverse(fmat3(_viewModelMat)));
}

fvec3 sq::norm_from_tri(fvec3 _t0, fvec3 _t1, fvec3 _t2) {
    fvec3 p0 = _t1 - _t0; fvec3 p1 = _t2 - _t0;
    return glm::normalize(glm::cross(p0, p1));
}
