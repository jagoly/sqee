#pragma once

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>

namespace sq {

struct Plane {        /// 3D Plane
    Vec3F normal;     // normal of the plane
    float offset;     // distance from origin
};

struct Sphere {       /// Bounding Sphere
    Vec3F origin;     // centre of the sphere
    float radius;     // size of the sphere
};

struct BoundBox {     /// Oriented Bounding Box
    Vec3F origin;     // centre of the shapes
    float radius;     // size of the sphere
    Vec3F points[8];  // corners of the box
};

struct Frustum {      /// Perspective Frustum
    Vec3F points[5];  // origin, LB, RB, LT, RT
    Plane planes[5];  // near, L, R, B, T
    float radius;     // = far - near
};

struct Ortho {        /// Orthogonal Frustum
    Plane planes[4];  // L, R, B, T
};

Sphere   make_Sphere   (const Mat4F& _matrix, Vec3F _origin, float _radius);
BoundBox make_BoundBox (const Mat4F& _matrix, Vec3F _origin, float _radius, Vec3F _extents);
Frustum  make_Frustum  (const Mat4F& _matrix, Vec3F _origin, Vec3F _dir, float _near, float _far);
Ortho    make_Ortho    (const Mat4F& _matrix);

Frustum reflect_Frustum(const Frustum& _frus, Vec3F _normal, Vec3F _trans);
Vec3F calc_frusCentre(const Frustum& _frus);

bool intersects(const Sphere& a, const Sphere& b);
bool intersects(const Sphere& a, const Frustum& b);
bool intersects(const Sphere& a, const Ortho& b);

bool intersects(const BoundBox& a, const Sphere& b);
bool intersects(const BoundBox& a, const Frustum& b);
bool intersects(const BoundBox& a, const Ortho& b);

bool intersects(const Frustum& a, const Frustum& b);

bool point_in_sphere_volume(Vec3F _point, const Sphere& _sphere);
bool point_in_cone_volume(Vec3F _point, const Frustum& _frustum);

}
