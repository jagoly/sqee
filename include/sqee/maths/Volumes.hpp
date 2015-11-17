#pragma once

#include <array>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq {

struct Plane { Vec3F normal; float offset; };

struct Sphere { Vec3F origin; float radius; };

struct BoundBox {
    Vec3F origin, normX, normY, normZ;
    float radius, sizeX, sizeY, sizeZ;
};

struct Frustum {
    std::array<Vec3F, 5> points; // origin, LB, RB, LT, RT
    std::array<Plane, 5> planes; // N, L, R, B, T
    float radius;
};

struct OrthoFrus {
    std::array<Plane, 4> planes; // L, R, B, T
};

Sphere make_Sphere(const Mat4F& _matrix, Vec3F _origin, float _radius);
Frustum make_Frustum(const Mat4F& _matrix, Vec3F _pos, Vec3F _dir, float _rmin, float _rmax);
BoundBox make_BoundBox(const Mat4F& _matrix, Vec3F _origin, float _radius, Vec3F _size);
OrthoFrus make_OrthoFrus(const Mat4F& _matrix, Vec3F _centre);

Frustum reflect_Frustum(const Frustum& _frus, Vec3F _normal, Vec3F _trans);
Vec3F calc_frusCentre(const Frustum& _frus);

bool sphr_in_sphr(const Sphere& _a, const Sphere& _bS);
bool bbox_in_sphr(const BoundBox& _aB, const Sphere& _bS);
bool frus_in_sphr(const Frustum& _aF, const Sphere& _bS);

bool sphr_in_frus(const Sphere& _a, const Frustum& _bF);
bool bbox_in_frus(const BoundBox& _aB, const Frustum& _bF);
bool frus_in_frus(const Frustum& _aF, const Frustum& _bF);

bool sphr_in_orth(const Sphere& _a, const OrthoFrus& _bO);
bool bbox_in_orth(const BoundBox& _aB, const OrthoFrus& _bO);
bool frus_in_orth(const Frustum& _aF, const OrthoFrus& _bO);

}
