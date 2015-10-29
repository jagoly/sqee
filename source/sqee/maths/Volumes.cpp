#include <initializer_list>

#include <sqee/maths/General.hpp>
#include <sqee/maths/Volumes.hpp>

using namespace sq;

inline Vec3F reflect(Vec3F _incident, Vec3F _normal, Vec3F _trans) {
    return maths::reflect(_incident - _trans, _normal) + _trans;
}

Sphere sq::make_Sphere(const Mat4F& _matrix, Vec3F _origin, float _radius) {
    Sphere sphere;
    float scaX = maths::length(_matrix[0]),
          scaY = maths::length(_matrix[1]),
          scaZ = maths::length(_matrix[2]);
    sphere.origin = Vec3F(_matrix * Vec4F(_origin, 1.f));
    sphere.radius = _radius * maths::max(scaX, scaY, scaZ);
    return sphere;
}

Frustum sq::make_Frustum(const Mat4F& _matrix, Vec3F _pos, Vec3F _dir, float _rmin, float _rmax) {
    Frustum fr; Vec4F tmp4; Vec3F tmp3;
    Mat4F invMat = maths::inverse(_matrix);
    tmp4 = invMat * Vec4F(-1.f, -1.f, 1.f, 1.f); fr.xy = Vec3F(tmp4) / tmp4.w;
    tmp4 = invMat * Vec4F(-1.f,  1.f, 1.f, 1.f); fr.xY = Vec3F(tmp4) / tmp4.w;
    tmp4 = invMat * Vec4F( 1.f, -1.f, 1.f, 1.f); fr.Xy = Vec3F(tmp4) / tmp4.w;
    tmp4 = invMat * Vec4F( 1.f,  1.f, 1.f, 1.f); fr.XY = Vec3F(tmp4) / tmp4.w;
    tmp3 = norm_from_tri(fr.XY, fr.xY, _pos); fr.pT = {tmp3, maths::dot(-tmp3, _pos)};
    tmp3 = norm_from_tri(fr.xy, fr.Xy, _pos); fr.pB = {tmp3, maths::dot(-tmp3, _pos)};
    tmp3 = norm_from_tri(fr.xY, fr.xy, _pos); fr.pL = {tmp3, maths::dot(-tmp3, _pos)};
    tmp3 = norm_from_tri(fr.Xy, fr.XY, _pos); fr.pR = {tmp3, maths::dot(-tmp3, _pos)};
    fr.pN = {-_dir, maths::dot(_dir, _pos + _dir * _rmin)};
    fr.sphere = {_pos, _rmax};
    return fr;
}

BoundBox sq::make_BoundBox(const Mat4F& _matrix, Vec3F _origin, float _radius, Vec3F _size) {
    BoundBox bb;
    float scaX = maths::length(_matrix[0]),
          scaY = maths::length(_matrix[1]),
          scaZ = maths::length(_matrix[2]);
    Mat3F normMat = make_normMat(_matrix);
    bb.sphere.origin = Vec3F(_matrix * Vec4F(_origin, 1.f));
    bb.sphere.radius = _radius * maths::max(scaX, scaY, scaZ);
    bb.nX = maths::normalize(normMat * Vec3F(1.f, 0.f, 0.f));
    bb.nY = maths::normalize(normMat * Vec3F(0.f, 1.f, 0.f));
    bb.nZ = maths::normalize(normMat * Vec3F(0.f, 0.f, 1.f));
    bb.size = _size * Vec3F(scaX, scaY, scaZ);
    return bb;
}

OrthoFrus sq::make_OrthoFrus(const Mat4F& _matrix, Vec3F _centre) {
    OrthoFrus ot; Vec4F tmp4;
    Mat4F invMat = maths::inverse(_matrix);
    Vec3F nX = maths::normalize(Mat3F(invMat) * Vec3F(1.f, 0.f, 0.f));
    Vec3F nY = maths::normalize(Mat3F(invMat) * Vec3F(0.f, 1.f, 0.f));
    tmp4 = invMat * Vec4F( 1.f,  0.f,  0.f, 1.f); Vec3F pXP = Vec3F(tmp4) / tmp4.w;
    tmp4 = invMat * Vec4F(-1.f,  0.f,  0.f, 1.f); Vec3F pXN = Vec3F(tmp4) / tmp4.w;
    tmp4 = invMat * Vec4F( 0.f,  1.f,  0.f, 1.f); Vec3F pYP = Vec3F(tmp4) / tmp4.w;
    tmp4 = invMat * Vec4F( 1.f, -1.f,  0.f, 1.f); Vec3F pYN = Vec3F(tmp4) / tmp4.w;
    ot.pT = {nY, maths::dot(-nY, pYP)}; ot.pB = {-nY, maths::dot(nY, pYN)};
    ot.pR = {nX, maths::dot(-nX, pXP)}; ot.pL = {-nX, maths::dot(nX, pXN)};
    return ot;
}

Frustum sq::reflect_Frustum(const Frustum& _frus, Vec3F _normal, Vec3F _trans) {
    Frustum fr = _frus; Vec3F tmp;
    fr.pN = {-_normal, maths::dot(-_normal, _trans)};
    fr.sphere.origin = maths::reflect(_frus.sphere.origin - _trans, _normal) + _trans;
    tmp = maths::reflect(_frus.pT.normal, _normal); fr.pT = {tmp, maths::dot(-tmp, fr.sphere.origin)};
    tmp = maths::reflect(_frus.pB.normal, _normal); fr.pB = {tmp, maths::dot(-tmp, fr.sphere.origin)};
    tmp = maths::reflect(_frus.pL.normal, _normal); fr.pL = {tmp, maths::dot(-tmp, fr.sphere.origin)};
    tmp = maths::reflect(_frus.pR.normal, _normal); fr.pR = {tmp, maths::dot(-tmp, fr.sphere.origin)};
    fr.xy = maths::reflect(_frus.xy - _trans, _normal) + _trans;
    fr.xY = maths::reflect(_frus.xY - _trans, _normal) + _trans;
    fr.Xy = maths::reflect(_frus.Xy - _trans, _normal) + _trans;
    fr.XY = maths::reflect(_frus.XY - _trans, _normal) + _trans;
    return fr;
}

Vec3F sq::calc_frusCentre(const Frustum& _frus) {
    Vec3F min(INFINITY, INFINITY, INFINITY), max(-INFINITY, -INFINITY, -INFINITY);
    for (const Vec3F& vec : {_frus.sphere.origin, _frus.xy, _frus.xY, _frus.Xy, _frus.XY})
        min = maths::min(min, vec), max = maths::max(max, vec);
    return (min + max) / 2.f;
}


bool sq::sphr_in_sphr(const Sphere& _A, const Sphere& _B) {
    // Sphere <> Sphere
    if (maths::distance(_A.origin, _B.origin) >= _A.radius + _B.radius) return false;

    return true;
}

bool sq::bbox_in_sphr(const BoundBox& _A, const Sphere& _B) {
    Vec3F oX = _A.nX * _A.size.x, oY = _A.nY * _A.size.y, oZ = _A.nZ * _A.size.z;
    Vec3F points[8] { _A.sphere.origin -oX -oY -oZ, _A.sphere.origin -oX -oY +oZ,
                      _A.sphere.origin -oX +oY -oZ, _A.sphere.origin -oX +oY +oZ,
                      _A.sphere.origin +oX -oY -oZ, _A.sphere.origin +oX -oY +oZ,
                      _A.sphere.origin +oX +oY -oZ, _A.sphere.origin +oX +oY +oZ};

    // Sphere <> Sphere
    if (maths::distance(_A.sphere.origin, _B.origin) >= _A.sphere.radius + _B.radius) return false;
    //float sphereDistance = maths::distance(_A.sphere.origin, _B.origin);
    //if (sphereDistance >= _A.sphere.radius + _B.radius) return false;
    //if (_A.sphere.radius < _B.radius - sphereDistance) return true;

    // BoundBox <> Sphere
    for (const auto& point : points) if (maths::distance(point, _B.origin) <= _B.radius) return true;

    return false;
}

bool sq::sphr_in_frus(const Sphere& _A, const Frustum& _B) {
    // Sphere <> Near Plane
    if (maths::dot(_A.origin, _B.pN.normal) + _B.pN.offset >= _A.radius) return false;

    // Sphere <> Sphere
    if (maths::distance(_A.origin, _B.sphere.origin) >= _A.radius+_B.sphere.radius) return false;

    // Sphere <> Frustum
    for (const auto& plane : {_B.pT, _B.pB, _B.pL, _B.pR})
        if (maths::dot(_A.origin, plane.normal) + plane.offset >= _A.radius) return false;

    return true;
}

bool sq::bbox_in_frus(const BoundBox& _A, const Frustum& _B) {
    Vec3F oX = _A.nX * _A.size.x, oY = _A.nY * _A.size.y, oZ = _A.nZ * _A.size.z;
    const auto& points = {_A.sphere.origin -oX -oY -oZ, _A.sphere.origin -oX -oY +oZ,
                   _A.sphere.origin -oX +oY -oZ, _A.sphere.origin -oX +oY +oZ,
                   _A.sphere.origin +oX -oY -oZ, _A.sphere.origin +oX -oY +oZ,
                   _A.sphere.origin +oX +oY -oZ, _A.sphere.origin +oX +oY +oZ};

    // Sphere <> Near Plane
    if (maths::dot(_A.sphere.origin, _B.pN.normal) + _B.pN.offset >= _A.sphere.radius) return false;

    // Sphere <> Sphere
    if (maths::distance(_A.sphere.origin, _B.sphere.origin) >= _A.sphere.radius+_B.sphere.radius) return false;

    // Sphere <> Frustum
    for (const auto& plane : {_B.pT, _B.pB, _B.pL, _B.pR})
        if (maths::dot(_A.sphere.origin, plane.normal) + plane.offset >= _A.sphere.radius) return false;

    // BoundBox <> NearPlane
    bool less = false, more = false;
    for (const auto& point : points) {
        float dist = maths::dot(point, _B.pN.normal) + _B.pN.offset;
        if (dist >= 0.f) more=true; if (dist <= 0.f) less=true;
    } if (more && !less) return false;

    // BoundBox <> Sphere
    bool hit = false;
    for (const auto& point : points)
        if (maths::distance(point, _B.sphere.origin)
            <= _B.sphere.radius) { hit = true; break; }
    if (hit == false) return false;

    // BoundBox <> Frustum
    for (const auto& plane : {_B.pT, _B.pB, _B.pL, _B.pR}) {
        bool less = false, more = false;
        for (const auto& point : points) {
            float dist = maths::dot(point, plane.normal) + plane.offset;
            if (dist >= 0.f) more=true; if (dist <= 0.f) less=true;
        } if (more && !less) return false;
    }

    return true;
}

bool sq::frus_in_frus(const Frustum& _A, const Frustum& _B) {
    const auto& points = {_A.sphere.origin, _A.xy, _A.xY, _A.Xy, _A.XY};

    // Frustum <> Near Plane
    bool hitA = false;
    for (const auto& point : points)
        if (maths::dot(point, _B.pN.normal) + _B.pN.offset
            <= 0.f) { hitA = true; break; }
    if (hitA == false) return false;

    // Frustum <> Sphere
    bool hitB = false;
    for (const auto& point : points)
        if (maths::distance(point, _B.sphere.origin)
            <= _B.sphere.radius) { hitB = true; break; }
    if (hitB == false) return false;

    // Frustum <> Frustum
    for (const auto& plane : {_B.pT, _B.pB, _B.pL, _B.pR}) {
        bool less = false, more = false;
        for (const auto& point : points) {
            float dist = maths::dot(point, plane.normal) + plane.offset;
            if (dist >= 0.f) more=true; if (dist <= 0.f) less=true;
        } if (more && !less) return false;
    }

    return true;
}

bool sq::sphr_in_orth(const Sphere& _A, const OrthoFrus& _B) {
    // Sphere <> OrthoFrus
    for (const auto& plane : {_B.pT, _B.pB, _B.pL, _B.pR})
        if (maths::dot(_A.origin, plane.normal) + plane.offset >= _A.radius) return false;

    return true;
}

bool sq::bbox_in_orth(const BoundBox& _A, const OrthoFrus& _B) {
    Vec3F oX = _A.nX * _A.size.x, oY = _A.nY * _A.size.y, oZ = _A.nZ * _A.size.z;
    const auto& points {_A.sphere.origin -oX -oY -oZ, _A.sphere.origin -oX -oY +oZ,
                        _A.sphere.origin -oX +oY -oZ, _A.sphere.origin -oX +oY +oZ,
                        _A.sphere.origin +oX -oY -oZ, _A.sphere.origin +oX -oY +oZ,
                        _A.sphere.origin +oX +oY -oZ, _A.sphere.origin +oX +oY +oZ};

    // Sphere <> OrthoFrus
    for (const auto& plane : {_B.pT, _B.pB, _B.pL, _B.pR})
        if (maths::dot(_A.sphere.origin, plane.normal) + plane.offset
            >= _A.sphere.radius) return false;

    // BoundBox <> OrthoFrus
    for (const auto& plane : {_B.pT, _B.pB, _B.pL, _B.pR}) {
        bool less = false, more = false;
        for (const auto& point : points) {
            float dist = maths::dot(point, plane.normal) + plane.offset;
            if (dist >= 0.f) more=true; if (dist <= 0.f) less=true;
        } if (more && !less) return false;
    }

    return true;
}
