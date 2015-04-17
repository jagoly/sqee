#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include "sqee/app/logging.hpp"
#include "sqee/gl/maths.hpp"

using namespace sq;

dvec3 sq::make_tangent(dvec3 _norm) {
    dvec3 t1(glm::cross(_norm, {1, 0, 0})), t2(glm::cross(_norm, {0, 1, 0}));
    return glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
}

dmat3 sq::make_normMat(dmat4 _viewModelMat) {
    return dmat3(glm::transpose(glm::inverse(_viewModelMat)));
}

bool sq::point_in_tri(dvec2 _p, dvec2 _t0, dvec2 _t1, dvec2 _t2) {
    bool b0 = (_p.x-_t1.x)*(_t0.y-_t1.y)-(_t0.x-_t1.x)*(_p.y-_t1.y) < 0.0;
    bool b1 = (_p.x-_t2.x)*(_t1.y-_t2.y)-(_t1.x-_t2.x)*(_p.y-_t2.y) < 0.0;
    bool b2 = (_p.x-_t0.x)*(_t2.y-_t0.y)-(_t2.x-_t0.x)*(_p.y-_t0.y) < 0.0;
    return (b0 == b1) && (b1 == b2);
}

dvec3 sq::norm_from_tri(dvec3 _t0, dvec3 _t1, dvec3 _t2) {
    dvec3 p0 = _t1 - _t0; dvec3 p1 = _t2 - _t0;
    return glm::normalize(glm::cross(p0, p1));
}

BoundBox sq::bbox_by_model(BoundBox _bbox, dmat4 _modelMat) {
    BoundBox retBox;
    dmat3 normMat = make_normMat(_modelMat);
    double scaX(glm::length(_modelMat[0])),
           scaY(glm::length(_modelMat[1])),
           scaZ(glm::length(_modelMat[2]));
    retBox.origin = dvec3(_modelMat * dvec4(_bbox.origin, 1));
    retBox.radius = _bbox.radius * glm::max(scaX, glm::max(scaY, scaZ));
    retBox.size = _bbox.size * dvec3(scaX, scaY, scaZ);
    retBox.nX = glm::normalize(normMat * _bbox.nX);
    retBox.nY = glm::normalize(normMat * _bbox.nY);
    retBox.nZ = glm::normalize(normMat * _bbox.nZ);
    return retBox;
}

bool sq::bbox_in_frus(BoundBox _bbox, Frustum _frus) {
    const auto& planes = {_frus.pT, _frus.pB, _frus.pL, _frus.pR, _frus.pN, _frus.pF};

    // Sphere Test
    bool intersect = false;
    for (const auto& plane : planes) {
        double dist = glm::dot(_bbox.origin, dvec3(plane)) + plane.w;
        if (dist >= _bbox.radius) return true;
        if (dist < -_bbox.radius) intersect = true;
    } if (!intersect) return false;

    // OBB Test
    dvec3 oX = _bbox.size.x*_bbox.nX,
          oY = _bbox.size.y*_bbox.nY,
          oZ = _bbox.size.z*_bbox.nZ;
    array<dvec3, 8> points = {
        _bbox.origin -oX -oY -oZ, _bbox.origin -oX -oY +oZ,
        _bbox.origin -oX +oY -oZ, _bbox.origin -oX +oY +oZ,
        _bbox.origin +oX -oY -oZ, _bbox.origin +oX -oY +oZ,
        _bbox.origin +oX +oY -oZ, _bbox.origin +oX +oY +oZ
    };
    for (const auto& plane : planes) {
        bool less = false, more = false;
        for (const auto& point : points) {
            double dist = glm::dot(point, dvec3(plane)) + plane.w;
            if (dist >= 0.0) more = true;
            if (dist <= 0.0) less = true;
        } if (more && !less) return true;
    } return false;
}

bool sq::frus_in_frus(Frustum _frusA, Frustum _frusB) {
    // NEEDS IMPROVEMENT
    const auto& points = {_frusA.xyz, _frusA.xyZ, _frusA.xYz, _frusA.xYZ,
                          _frusA.Xyz, _frusA.XyZ, _frusA.XYz, _frusA.XYZ};
    const auto& planes = {_frusB.pT, _frusB.pB, _frusB.pL, _frusB.pR, _frusB.pN, _frusB.pF};
    for (const auto& plane : planes) {
        bool less = false, more = false;
        for (const auto& point : points) {
            double dist = glm::dot(point, dvec3(plane)) + plane.w;
            if (dist >= 0.0) more = true;
            if (dist <= 0.0) less = true;
        } if (more && !less) return true;
    } return false;
}

bool sq::sphere_in_frus(dvec4 _sphere, Frustum _frus) {
    const auto& planes = {_frus.pT, _frus.pB, _frus.pL, _frus.pR, _frus.pN, _frus.pF};
    for (const auto& plane : planes) {
        double dist = glm::dot(dvec3(_sphere), dvec3(plane)) + plane.w;
        if (dist >= _sphere.w) return true;
    } return false;
}

Frustum sq::make_Frustum(dmat4 _invProjViewMat) {
    Frustum fr; dvec4 tmp4; dvec3 tmp3;
    tmp4 = _invProjViewMat * dvec4(-1, -1, -1, 1); fr.xyz = dvec3(tmp4) / tmp4.w;
    tmp4 = _invProjViewMat * dvec4(-1, -1,  1, 1); fr.xyZ = dvec3(tmp4) / tmp4.w;
    tmp4 = _invProjViewMat * dvec4(-1,  1, -1, 1); fr.xYz = dvec3(tmp4) / tmp4.w;
    tmp4 = _invProjViewMat * dvec4(-1,  1,  1, 1); fr.xYZ = dvec3(tmp4) / tmp4.w;
    tmp4 = _invProjViewMat * dvec4( 1, -1, -1, 1); fr.Xyz = dvec3(tmp4) / tmp4.w;
    tmp4 = _invProjViewMat * dvec4( 1, -1,  1, 1); fr.XyZ = dvec3(tmp4) / tmp4.w;
    tmp4 = _invProjViewMat * dvec4( 1,  1, -1, 1); fr.XYz = dvec3(tmp4) / tmp4.w;
    tmp4 = _invProjViewMat * dvec4( 1,  1,  1, 1); fr.XYZ = dvec3(tmp4) / tmp4.w;
    tmp3 = norm_from_tri(fr.xYz, fr.XYz, fr.xYZ); fr.pT = dvec4(tmp3, glm::dot(-tmp3, fr.xYz));
    tmp3 = norm_from_tri(fr.XyZ, fr.Xyz, fr.xyZ); fr.pB = dvec4(tmp3, glm::dot(-tmp3, fr.XyZ));
    tmp3 = norm_from_tri(fr.xyz, fr.xYz, fr.xyZ); fr.pL = dvec4(tmp3, glm::dot(-tmp3, fr.xyz));
    tmp3 = norm_from_tri(fr.XYZ, fr.XYz, fr.XyZ); fr.pR = dvec4(tmp3, glm::dot(-tmp3, fr.XYZ));
    tmp3 = norm_from_tri(fr.xyz, fr.Xyz, fr.xYz); fr.pN = dvec4(tmp3, glm::dot(-tmp3, fr.xyz));
    tmp3 = norm_from_tri(fr.XYZ, fr.XyZ, fr.xYZ); fr.pF = dvec4(tmp3, glm::dot(-tmp3, fr.XYZ));
    return fr;
}

dvec3 sq::calc_frusCentre(Frustum _frus) {
    dvec3 min(INFINITY, INFINITY, INFINITY);
    dvec3 max(-INFINITY, -INFINITY, -INFINITY);
    for (const dvec3& vec : {_frus.xyz, _frus.xyZ, _frus.xYz, _frus.xYZ,
                             _frus.Xyz, _frus.XyZ, _frus.XYz, _frus.XYZ}) {
        min = glm::min(min, vec);
        max = glm::max(max, vec);
    }
    return (min + max) / 2.0;
}
