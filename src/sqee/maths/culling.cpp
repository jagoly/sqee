#include <glm/gtc/matrix_transform.hpp>

#include <sqee/maths/general.hpp>
#include <sqee/maths/culling.hpp>

using namespace sq;

inline fvec3 reflect(fvec3 _val, fvec3 _normal, fvec3 _trans) {
    return glm::reflect(_val - _trans, _normal) + _trans;
}

Frustum sq::make_Frustum(fmat4 _matrix, fvec3 _pos, fvec3 _dir, float _rmin, float _rmax) {
    Frustum fr; fvec4 tmp4; fvec3 tmp3;
    fmat4 invMat = glm::inverse(_matrix);
    tmp4 = invMat * fvec4(-1.f, -1.f, 1.f, 1.f); fr.xy = fvec3(tmp4) / tmp4.w;
    tmp4 = invMat * fvec4(-1.f,  1.f, 1.f, 1.f); fr.xY = fvec3(tmp4) / tmp4.w;
    tmp4 = invMat * fvec4( 1.f, -1.f, 1.f, 1.f); fr.Xy = fvec3(tmp4) / tmp4.w;
    tmp4 = invMat * fvec4( 1.f,  1.f, 1.f, 1.f); fr.XY = fvec3(tmp4) / tmp4.w;
    tmp3 = norm_from_tri(fr.XY, fr.xY, _pos); fr.pT = {tmp3, glm::dot(-tmp3, _pos)};
    tmp3 = norm_from_tri(fr.xy, fr.Xy, _pos); fr.pB = {tmp3, glm::dot(-tmp3, _pos)};
    tmp3 = norm_from_tri(fr.xY, fr.xy, _pos); fr.pL = {tmp3, glm::dot(-tmp3, _pos)};
    tmp3 = norm_from_tri(fr.Xy, fr.XY, _pos); fr.pR = {tmp3, glm::dot(-tmp3, _pos)};
    fr.pN = {-_dir, glm::dot(_dir, _pos + _dir * _rmin)};
    fr.sphere = {_pos, _rmax};
    return fr;
}

BoundBox sq::make_BoundBox(fmat4 _matrix, fvec3 _origin, fvec3 _size, float _radius) {
    BoundBox bb;
    float scaX = glm::length(_matrix[0]),
          scaY = glm::length(_matrix[1]),
          scaZ = glm::length(_matrix[2]);
    fmat3 normMat = make_normMat(_matrix);
    bb.sphere.origin = fvec3(_matrix * fvec4(_origin, 1.f));
    bb.sphere.radius = _radius * glm::max(scaX, glm::max(scaY, scaZ));
    bb.size = _size * fvec3(scaX, scaY, scaZ);
    bb.nX = glm::normalize(normMat * fvec3(1.f, 0.f, 0.f));
    bb.nY = glm::normalize(normMat * fvec3(0.f, 1.f, 0.f));
    bb.nZ = glm::normalize(normMat * fvec3(0.f, 0.f, 1.f));
    return bb;
}

Frustum sq::reflect_Frustum(const Frustum& _frus, fvec3 _normal, fvec3 _trans) {
    Frustum fr = _frus; fvec3 tmp;
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

fvec3 sq::calc_frusCentre(const Frustum& _frus) {
    fvec3 min(INFINITY, INFINITY, INFINITY), max(-INFINITY, -INFINITY, -INFINITY);
    for (const fvec3& vec : {_frus.sphere.origin, _frus.xy, _frus.xY, _frus.Xy, _frus.XY})
        min = glm::min(min, vec), max = glm::max(max, vec);
    return (min + max) / 2.f;
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
    fvec3 oX = _A.size.x*_A.nX, oY = _A.size.y*_A.nY, oZ = _A.size.z*_A.nZ;
    array<fvec3, 8> points {
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
        float dist = glm::dot(point, _B.pN.normal) + _B.pN.offset;
        if (dist >= 0.f) more=true; if (dist <= 0.f) less=true;
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
            float dist = glm::dot(point, plane.normal) + plane.offset;
            if (dist >= 0.f) more=true; if (dist <= 0.f) less=true;
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
            <= 0.f) { hitA = true; break; }
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
            float dist = glm::dot(point, plane.normal) + plane.offset;
            if (dist >= 0.f) more=true; if (dist <= 0.f) less=true;
        } if (more && !less) return true;
    }

    return false;
}
