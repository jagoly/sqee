#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "sqee/app/logging.hpp"
#include "sqee/maths/general.hpp"

using namespace sq;

dvec3 sq::make_tangent(dvec3 _norm) {
    dvec3 t1(glm::cross(_norm, {1, 0, 0})), t2(glm::cross(_norm, {0, 1, 0}));
    return glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
}

dmat3 sq::make_normMat(dmat4 _viewModelMat) {
    return dmat3(glm::transpose(glm::inverse(_viewModelMat)));
}

dvec3 sq::norm_from_tri(dvec3 _t0, dvec3 _t1, dvec3 _t2) {
    dvec3 p0 = _t1 - _t0; dvec3 p1 = _t2 - _t0;
    return glm::normalize(glm::cross(p0, p1));
}
