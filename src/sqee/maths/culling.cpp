#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_normalized_axis.hpp>
#include <glm/gtx/rotate_vector.hpp>

#include <sqee/app/logging.hpp>
#include <glm/gtx/string_cast.hpp>

#include "sqee/maths/general.hpp"
#include "sqee/maths/culling.hpp"

using namespace sq;

inline dvec3 reflect(dvec3 _val, dvec3 _normal, dvec3 _trans) {
    return glm::reflect(_val - _trans, _normal) + _trans;
}

Frustum sq::make_Frustum(dmat4 _matrix, dvec3 _pos, dvec3 _dir, dvec2 _range) {
    Frustum fr; dvec4 tmp4; dvec3 tmp3;
    dmat4 invMat = glm::inverse(_matrix);
    tmp4 = invMat * dvec4(-1, -1,  1, 1); fr.xy = dvec3(tmp4) / tmp4.w;
    tmp4 = invMat * dvec4(-1,  1,  1, 1); fr.xY = dvec3(tmp4) / tmp4.w;
    tmp4 = invMat * dvec4( 1, -1,  1, 1); fr.Xy = dvec3(tmp4) / tmp4.w;
    tmp4 = invMat * dvec4( 1,  1,  1, 1); fr.XY = dvec3(tmp4) / tmp4.w;
    tmp3 = norm_from_tri(fr.XY, fr.xY, _pos); fr.pT = {tmp3, glm::dot(-tmp3, _pos)};
    tmp3 = norm_from_tri(fr.xy, fr.Xy, _pos); fr.pB = {tmp3, glm::dot(-tmp3, _pos)};
    tmp3 = norm_from_tri(fr.xY, fr.xy, _pos); fr.pL = {tmp3, glm::dot(-tmp3, _pos)};
    tmp3 = norm_from_tri(fr.Xy, fr.XY, _pos); fr.pR = {tmp3, glm::dot(-tmp3, _pos)};
    fr.pN = {-_dir, glm::dot(_dir, _pos+_dir*_range.x)};
    fr.sphere = {_pos, _range.y};
    return fr;
}

BoundBox sq::make_BoundBox(dmat4 _matrix, dvec3 _origin, dvec3 _size, double _radius) {
    BoundBox bb;
    double scaX = glm::length(_matrix[0]),
           scaY = glm::length(_matrix[1]),
           scaZ = glm::length(_matrix[2]);
    dmat3 normMat = make_normMat(_matrix);
    bb.sphere.origin = dvec3(_matrix * dvec4(_origin, 1.0));
    bb.sphere.radius = _radius * glm::max(scaX, glm::max(scaY, scaZ));
    bb.size = _size * dvec3(scaX, scaY, scaZ);
    bb.nX = glm::normalize(normMat * dvec3(1.0, 0.0, 0.0));
    bb.nY = glm::normalize(normMat * dvec3(0.0, 1.0, 0.0));
    bb.nZ = glm::normalize(normMat * dvec3(0.0, 0.0, 1.0));
    return bb;
}

Frustum sq::reflect_Frustum(const Frustum& _frus, dvec3 _normal, dvec3 _trans) {
    Frustum fr = _frus; dvec3 tmp;
    fr.pN = {-_normal, glm::dot(-_normal, _trans)};
    fr.sphere.origin = glm::reflect(_frus.sphere.origin - _trans, _normal) + _trans;
    tmp = glm::reflect(_frus.pT.normal, _normal); fr.pT = {tmp, glm::dot(-tmp, fr.sphere.origin)};
    tmp = glm::reflect(_frus.pB.normal, _normal); fr.pB = {tmp, glm::dot(-tmp, fr.sphere.origin)};
    tmp = glm::reflect(_frus.pL.normal, _normal); fr.pL = {tmp, glm::dot(-tmp, fr.sphere.origin)};
    tmp = glm::reflect(_frus.pR.normal, _normal); fr.pR = {tmp, glm::dot(-tmp, fr.sphere.origin)};
    fr.xy = glm::reflect(_frus.xy - _trans, _normal) + _trans;
    fr.xY = glm::reflect(_frus.xY - _trans, _normal) + _trans;
    fr.Xy = glm::reflect(_frus.Xy - _trans, _normal) + _trans;
    fr.XY = glm::reflect(_frus.XY - _trans, _normal) + _trans;
    return fr;
}

dvec3 sq::calc_frusCentre(const Frustum& _frus) {
    dvec3 min(INFINITY, INFINITY, INFINITY), max(-INFINITY, -INFINITY, -INFINITY);
    for (const dvec3& vec : {_frus.sphere.origin, _frus.xy, _frus.xY, _frus.Xy, _frus.XY})
        min = glm::min(min, vec), max = glm::max(max, vec);
    return (min + max) / 2.0;
}

bool sq::sphr_in_frus(const Sphere& _A, const Frustum& _B) {
    const auto& planes = {_B.pT, _B.pB, _B.pL, _B.pR};

    // Sphere <> Near Plane
    if (glm::dot(_A.origin, _B.pN.normal) + _B.pN.offset
        >= _A.radius) return true;

    // Sphere <> Sphere
    if (glm::distance(_A.origin, _B.sphere.origin)
        >= _A.radius+_B.sphere.radius) return true;

    // Sphere <> Frustum
    for (const auto& plane : planes)
        if (glm::dot(_A.origin, plane.normal) + plane.offset
            >= _A.radius) return true;

    return false;
}

bool sq::bbox_in_frus(const BoundBox& _A, const Frustum& _B) {
    dvec3 oX = _A.size.x*_A.nX, oY = _A.size.y*_A.nY, oZ = _A.size.z*_A.nZ;
    array<dvec3, 8> points {
        _A.sphere.origin -oX -oY -oZ, _A.sphere.origin -oX -oY +oZ,
        _A.sphere.origin -oX +oY -oZ, _A.sphere.origin -oX +oY +oZ,
        _A.sphere.origin +oX -oY -oZ, _A.sphere.origin +oX -oY +oZ,
        _A.sphere.origin +oX +oY -oZ, _A.sphere.origin +oX +oY +oZ
    }; const auto& planes = {_B.pT, _B.pB, _B.pL, _B.pR};

    // Sphere <> Near Plane
    if (glm::dot(_A.sphere.origin, _B.pN.normal) + _B.pN.offset
        >= _A.sphere.radius) return true;

    // Sphere <> Sphere
    if (glm::distance(_A.sphere.origin, _B.sphere.origin)
        >= _A.sphere.radius+_B.sphere.radius) return true;

    // Sphere <> Frustum
    for (const auto& plane : planes)
        if (glm::dot(_A.sphere.origin, plane.normal) + plane.offset
            >= _A.sphere.radius) return true;

    // BoundBox <> NearPlane
    bool less = false, more = false;
    for (const auto& point : points) {
        double dist = glm::dot(point, _B.pN.normal) + _B.pN.offset;
        if (dist >= 0.0) more=true; if (dist <= 0.0) less=true;
    } if (more && !less) return true;

    // BoundBox <> Sphere
    bool hit = false;
    for (const auto& point : points)
        if (glm::distance(point, _B.sphere.origin)
            <= _B.sphere.radius) { hit = true; break; }
    if (hit == false) return true;

    // BoundBox <> Frustum
    for (const auto& plane : planes) {
        bool less = false, more = false;
        for (const auto& point : points) {
            double dist = glm::dot(point, plane.normal) + plane.offset;
            if (dist >= 0.0) more=true; if (dist <= 0.0) less=true;
        } if (more && !less) return true;
    }

    return false;
}

bool sq::frus_in_frus(const Frustum& _A, const Frustum& _B) {
    const auto& planes = {_B.pT, _B.pB, _B.pL, _B.pR};
    const auto& points = {_A.sphere.origin, _A.xy, _A.xY, _A.Xy, _A.XY};

    // Frustum <> Near Plane
    bool hitA = false;
    for (const auto& point : points)
        if (glm::dot(point, _B.pN.normal) + _B.pN.offset
            <= 0.0) { hitA = true; break; }
    if (hitA == false) return true;

    // Frustum <> Sphere
    bool hitB = false;
    for (const auto& point : points)
        if (glm::distance(point, _B.sphere.origin)
            <= _B.sphere.radius) { hitB = true; break; }
    if (hitB == false) return true;

    // Frustum <> Frustum
    for (const auto& plane : planes) {
        bool less = false, more = false;
        for (const auto& point : points) {
            double dist = glm::dot(point, plane.normal) + plane.offset;
            if (dist >= 0.0) more=true; if (dist <= 0.0) less=true;
        } if (more && !less) return true;
    }

    return false;
}
