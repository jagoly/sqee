#include <initializer_list>

#include <sqee/maths/General.hpp>
#include <sqee/maths/Volumes.hpp>

using namespace sq;


Sphere sq::make_Sphere(const Mat4F& _matrix, Vec3F _origin, float _radius) {
    Sphere sphere;
    float scaleX = maths::length(_matrix[0]);
    float scaleY = maths::length(_matrix[1]);
    float scaleZ = maths::length(_matrix[2]);
    sphere.origin = Vec3F(_matrix * Vec4F(_origin, 1.f));
    sphere.radius = _radius * maths::max(scaleX, scaleY, scaleZ);
    return sphere;
}


Frustum sq::make_Frustum(const Mat4F& _matrix, Vec3F _pos, Vec3F _dir, float _rmin, float _rmax) {
    Frustum frus;
    Mat4F invMat = maths::inverse(_matrix); Vec4F tmp;
    tmp = invMat * Vec4F(-1.f, -1.f, 1.f, 1.f); frus.points[1] = Vec3F(tmp) / tmp.w;
    tmp = invMat * Vec4F(-1.f,  1.f, 1.f, 1.f); frus.points[3] = Vec3F(tmp) / tmp.w;
    tmp = invMat * Vec4F( 1.f, -1.f, 1.f, 1.f); frus.points[2] = Vec3F(tmp) / tmp.w;
    tmp = invMat * Vec4F( 1.f,  1.f, 1.f, 1.f); frus.points[4] = Vec3F(tmp) / tmp.w;
    frus.planes[1].normal = norm_from_tri(frus.points[3], frus.points[1], _pos);
    frus.planes[1].offset = maths::dot(-frus.planes[1].normal, _pos);
    frus.planes[2].normal = norm_from_tri(frus.points[2], frus.points[4], _pos);
    frus.planes[2].offset = maths::dot(-frus.planes[2].normal, _pos);
    frus.planes[3].normal = norm_from_tri(frus.points[1], frus.points[2], _pos);
    frus.planes[3].offset = maths::dot(-frus.planes[3].normal, _pos);
    frus.planes[4].normal = norm_from_tri(frus.points[4], frus.points[3], _pos);
    frus.planes[4].offset = maths::dot(-frus.planes[4].normal, _pos);
    frus.planes[0] = {-_dir, maths::dot(_dir, _pos + _dir * _rmin)};
    frus.points[0] = _pos; frus.radius = _rmax;
    return frus;
}


BoundBox sq::make_BoundBox(const Mat4F& _matrix, Vec3F _origin, float _radius, Vec3F _size) {
    BoundBox bbox;
    Mat3F normMat = make_normMat(_matrix);
    float scaleX = maths::length(_matrix[0]);
    float scaleY = maths::length(_matrix[1]);
    float scaleZ = maths::length(_matrix[2]);
    bbox.origin = Vec3F(_matrix * Vec4F(_origin, 1.f));
    bbox.radius = _radius * maths::max(scaleX, scaleY, scaleZ);
    bbox.normX = maths::normalize(normMat * Vec3F(1.f, 0.f, 0.f));
    bbox.normY = maths::normalize(normMat * Vec3F(0.f, 1.f, 0.f));
    bbox.normZ = maths::normalize(normMat * Vec3F(0.f, 0.f, 1.f));
    bbox.sizeX = _size.x * scaleX;
    bbox.sizeY = _size.y * scaleY;
    bbox.sizeZ = _size.z * scaleZ;
    return bbox;
}

OrthoFrus sq::make_OrthoFrus(const Mat4F& _matrix, Vec3F _centre) {
    OrthoFrus orth;
    Mat4F invMat = maths::inverse(_matrix); Vec4F tmp;
    Vec3F nX = maths::normalize(Mat3F(invMat) * Vec3F(1.f, 0.f, 0.f));
    Vec3F nY = maths::normalize(Mat3F(invMat) * Vec3F(0.f, 1.f, 0.f));
    tmp = invMat * Vec4F(-1.f,  0.f,  0.f, 1.f);
    orth.planes[0] = {-nX, maths::dot(nX, Vec3F(tmp) / tmp.w)};
    tmp = invMat * Vec4F( 1.f,  0.f,  0.f, 1.f);
    orth.planes[1] = {nX, maths::dot(-nX, Vec3F(tmp) / tmp.w)};
    tmp = invMat * Vec4F( 0.f, -1.f,  0.f, 1.f);
    orth.planes[2] = {-nY, maths::dot(nY, Vec3F(tmp) / tmp.w)};
    tmp = invMat * Vec4F( 0.f,  1.f,  0.f, 1.f);
    orth.planes[3] = {nY, maths::dot(-nY, Vec3F(tmp) / tmp.w)};
    return orth;
}

Frustum sq::reflect_Frustum(const Frustum& _frus, Vec3F _normal, Vec3F _trans) {
    Frustum frus; Vec3F tmp;
    frus.points[0] = maths::reflect(_frus.points[0], _normal, _trans);
    frus.points[1] = maths::reflect(_frus.points[1], _normal, _trans);
    frus.points[2] = maths::reflect(_frus.points[2], _normal, _trans);
    frus.points[3] = maths::reflect(_frus.points[3], _normal, _trans);
    frus.points[4] = maths::reflect(_frus.points[4], _normal, _trans);
    frus.planes[0] = {-_normal, maths::dot(-_normal, _trans)};
    tmp = maths::reflect(_frus.planes[1].normal, _normal);
    frus.planes[1] = {tmp, maths::dot(-tmp, frus.points[0])};
    tmp = maths::reflect(_frus.planes[2].normal, _normal);
    frus.planes[2] = {tmp, maths::dot(-tmp, frus.points[0])};
    tmp = maths::reflect(_frus.planes[3].normal, _normal);
    frus.planes[3] = {tmp, maths::dot(-tmp, frus.points[0])};
    tmp = maths::reflect(_frus.planes[4].normal, _normal);
    frus.planes[4] = {tmp, maths::dot(-tmp, frus.points[0])};
    frus.radius = _frus.radius;
    return frus;
}


Vec3F sq::calc_frusCentre(const Frustum& _frus) {
    Vec3F min(INFINITY, INFINITY, INFINITY);
    Vec3F max(-INFINITY, -INFINITY, -INFINITY);
    for (const auto& point : _frus.points) {
        min = maths::min(min, point);
        max = maths::max(max, point);
    }
    return (min + max) / 2.f;
}


static inline
bool inside(Vec3F _point, Plane _plane, float _radius = 0.f) {
    return maths::dot(_point, _plane.normal) + _plane.offset <= _radius;
}

static inline
bool inside(Vec3F _pointA, Vec3F _pointB, float _radius) {
    return maths::distance(_pointA, _pointB) <= _radius;
}

template<class T> static inline
bool all_outside(T _points, Plane _plane) {
    for (Vec3F point : _points) if (inside(point, _plane)) return false;
    return true;
}

template<class T> static inline
bool all_outside(T _points, Vec3F _pointB, float _radius) {
    for (Vec3F point : _points) if (inside(point, _pointB, _radius)) return false;
    return true;
}


bool sq::sphr_in_sphr(const Sphere& _a, const Sphere& _b) {
    // Sphere <> Sphere
    return inside(_a.origin, _b.origin, _a.radius + _b.radius);
}


bool sq::bbox_in_sphr(const BoundBox& _a, const Sphere& _b) {
    Vec3F o[3] {
        _a.normX * _a.sizeX,
        _a.normY * _a.sizeY,
        _a.normZ * _a.sizeZ
    };

    std::array<Vec3F, 8> points {
        _a.origin -o[0] -o[1] -o[2],
        _a.origin -o[0] -o[1] +o[2],
        _a.origin -o[0] +o[1] -o[2],
        _a.origin -o[0] +o[1] +o[2],
        _a.origin +o[0] -o[1] -o[2],
        _a.origin +o[0] -o[1] +o[2],
        _a.origin +o[0] +o[1] -o[2],
        _a.origin +o[0] +o[1] +o[2]
    };

    // Sphere <> Sphere
    if (!inside(_a.origin, _b.origin, _a.radius + _b.radius)) return false;

    // BoundBox <> Sphere
    for (auto point : points) if (inside(point, _b.origin, _b.radius)) return true;

    return false;
}


bool sq::sphr_in_frus(const Sphere& _a, const Frustum& _b) {
    // Sphere <> Sphere
    if (!inside(_a.origin, _b.points[0], _a.radius + _b.radius)) return false;

    // Sphere <> Frustum
    for (auto plane : _b.planes) if (!inside(_a.origin, plane, _a.radius)) return false;

    return true;
}


bool sq::bbox_in_frus(const BoundBox& _a, const Frustum& _b) {
    const Vec3F offs[3] {
        Vec3F{_a.normX * _a.sizeX},
        Vec3F{_a.normY * _a.sizeY},
        Vec3F{_a.normZ * _a.sizeZ}
    };

    const auto points = {
        Vec3F{_a.origin -offs[0] -offs[1] -offs[2]},
        Vec3F{_a.origin -offs[0] -offs[1] +offs[2]},
        Vec3F{_a.origin -offs[0] +offs[1] -offs[2]},
        Vec3F{_a.origin -offs[0] +offs[1] +offs[2]},
        Vec3F{_a.origin +offs[0] -offs[1] -offs[2]},
        Vec3F{_a.origin +offs[0] -offs[1] +offs[2]},
        Vec3F{_a.origin +offs[0] +offs[1] -offs[2]},
        Vec3F{_a.origin +offs[0] +offs[1] +offs[2]}
    };

    const auto planes = {
        Plane{ _a.normX, maths::dot(-_a.normX, _a.origin + offs[0])},
        Plane{-_a.normX, maths::dot( _a.normX, _a.origin - offs[0])},
        Plane{ _a.normY, maths::dot(-_a.normY, _a.origin + offs[1])},
        Plane{-_a.normY, maths::dot( _a.normY, _a.origin - offs[1])},
        Plane{ _a.normZ, maths::dot(-_a.normZ, _a.origin + offs[2])},
        Plane{-_a.normZ, maths::dot( _a.normZ, _a.origin - offs[2])}
    };

    // Sphere <> Sphere
    if (!inside(_a.origin, _b.points[0], _a.radius + _b.radius)) return false;

    // Sphere <> Frustum   TODO: test performance
    for (auto plane : _b.planes) if (!inside(_a.origin, plane, _a.radius)) return false;

    // BoundBox <> Sphere
    if (all_outside(points, _b.points[0], _b.radius)) return false;

    // BoundBox <> Frustum
    for (auto plane : _b.planes) if (all_outside(points, plane)) return false;
    for (auto plane : planes) if (all_outside(_b.points, plane)) return false;

    return true;
}

bool sq::frus_in_frus(const Frustum& _a, const Frustum& _b) {
    // Frustum <> Sphere
    if (all_outside(_a.points, _b.points[0], _b.radius)) return false;

    // Frustum <> Frustum
    for (auto plane : _b.planes) if (all_outside(_a.points, plane)) return false;
    for (auto plane : _a.planes) if (all_outside(_b.points, plane)) return false;

    return true;
}

bool sq::sphr_in_orth(const Sphere& _a, const OrthoFrus& _b) {
    // Sphere <> OrthoFrus
    for (auto plane : _b.planes) if (!inside(_a.origin, plane, _a.radius)) return false;

    return true;
}

bool sq::bbox_in_orth(const BoundBox& _a, const OrthoFrus& _b) {
    const Vec3F offs[3] {
        Vec3F{_a.normX * _a.sizeX},
        Vec3F{_a.normY * _a.sizeY},
        Vec3F{_a.normZ * _a.sizeZ}
    };

    const auto points = {
        Vec3F{_a.origin -offs[0] -offs[1] -offs[2]},
        Vec3F{_a.origin -offs[0] -offs[1] +offs[2]},
        Vec3F{_a.origin -offs[0] +offs[1] -offs[2]},
        Vec3F{_a.origin -offs[0] +offs[1] +offs[2]},
        Vec3F{_a.origin +offs[0] -offs[1] -offs[2]},
        Vec3F{_a.origin +offs[0] -offs[1] +offs[2]},
        Vec3F{_a.origin +offs[0] +offs[1] -offs[2]},
        Vec3F{_a.origin +offs[0] +offs[1] +offs[2]}
    };

    // Sphere <> OrthoFrus
    for (auto plane : _b.planes) if (!inside(_a.origin, plane, _a.radius)) return false;

    // BoundBox <> OrthoFrus
    for (auto plane : _b.planes) if (all_outside(points, plane)) return false;

    // TODO: Add test with OrthoFrus points

    return true;
}
