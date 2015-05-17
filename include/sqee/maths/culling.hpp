#pragma once
#include <sqee/forward.hpp>

namespace sq {

Frustum make_Frustum(dmat4 _matrix, dvec3 _pos, dvec3 _dir, dvec2 _range);
BoundBox make_BoundBox(dmat4 _matrix, dvec3 _origin, dvec3 _size, double _radius);

Frustum reflect_Frustum(const Frustum& _frus, dvec3 _normal, dvec3 _trans);
dvec3 calc_frusCentre(const Frustum& _frus);

bool sphr_in_sphr(const Sphere& _A, const Sphere& _B);
bool bbox_in_sphr(const BoundBox& _A, const Sphere& _B);
bool frus_in_sphr(const Frustum& _A, const Sphere& _B);

bool sphr_in_frus(const Sphere& _A, const Frustum& _B);
bool bbox_in_frus(const BoundBox& _A, const Frustum& _B);
bool frus_in_frus(const Frustum& _A, const Frustum& _B);

}
