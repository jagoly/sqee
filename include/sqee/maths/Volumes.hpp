#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq {

struct Plane { Vec3F normal; float offset; };
struct Sphere { Vec3F origin; float radius; };
struct Frustum { Sphere sphere; Plane pN, pT, pB, pL, pR; Vec3F xy, xY, Xy, XY; };
struct BoundBox { Sphere sphere; Vec3F size, nX{1,0,0}, nY{0,1,0}, nZ{0,0,1}; };
struct OrthoFrus { Plane pT, pB, pL, pR; };

Sphere make_Sphere(const Mat4F& _matrix, Vec3F _origin, float _radius);
Frustum make_Frustum(const Mat4F& _matrix, Vec3F _pos, Vec3F _dir, float _rmin, float _rmax);
BoundBox make_BoundBox(const Mat4F& _matrix, Vec3F _origin, float _radius, Vec3F _size);
OrthoFrus make_OrthoFrus(const Mat4F& _matrix, Vec3F _centre);

Frustum reflect_Frustum(const Frustum& _frus, Vec3F _normal, Vec3F _trans);
Vec3F calc_frusCentre(const Frustum& _frus);

bool sphr_in_sphr(const Sphere& _A, const Sphere& _B);
bool bbox_in_sphr(const BoundBox& _A, const Sphere& _B);
bool frus_in_sphr(const Frustum& _A, const Sphere& _B);

bool sphr_in_frus(const Sphere& _A, const Frustum& _B);
bool bbox_in_frus(const BoundBox& _A, const Frustum& _B);
bool frus_in_frus(const Frustum& _A, const Frustum& _B);

bool sphr_in_orth(const Sphere& _A, const OrthoFrus& _B);
bool bbox_in_orth(const BoundBox& _A, const OrthoFrus& _B);

}
