#pragma once
#include <sqee/forward.hpp>

namespace sq {

Sphere make_Sphere(fmat4 _matrix, fvec3 _origin, float _radius);
Frustum make_Frustum(fmat4 _matrix, fvec3 _pos, fvec3 _dir, float _rmin, float _rmax);
BoundBox make_BoundBox(fmat4 _matrix, fvec3 _origin, float _radius, fvec3 _size);

Frustum reflect_Frustum(const Frustum& _frus, fvec3 _normal, fvec3 _trans);
fvec3 calc_frusCentre(const Frustum& _frus);

bool sphr_in_sphr(const Sphere& _A, const Sphere& _B);
bool bbox_in_sphr(const BoundBox& _A, const Sphere& _B);
bool frus_in_sphr(const Frustum& _A, const Sphere& _B);

bool sphr_in_frus(const Sphere& _A, const Frustum& _B);
bool bbox_in_frus(const BoundBox& _A, const Frustum& _B);
bool frus_in_frus(const Frustum& _A, const Frustum& _B);

}
