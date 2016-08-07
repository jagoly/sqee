#include <sqee/maths/General.hpp>
#include <sqee/maths/Volumes.hpp>

using namespace sq;


Sphere sq::make_Sphere(const Mat4F& _matrix, Vec3F _origin, float _radius) {
    Vec3F scale = maths::extract_scale(Mat3F(_matrix));
    _origin = Vec3F(_matrix * Vec4F(_origin, 1.f));
    _radius *= maths::max(scale.x, scale.y, scale.z);
    return Sphere {_origin, _radius};
}

BoundBox sq::make_BoundBox(const Mat4F& _matrix, Vec3F _origin, float _radius, Vec3F _extents) {

    BoundBox bbox;

    const Vec3F scale = maths::extract_scale(Mat3F(_matrix));
    bbox.radius = _radius * maths::max(scale.x, scale.x, scale.x);
    bbox.origin = Vec3F(_matrix * Vec4F(_origin, 1.f));

    const Mat3F normMat = maths::transpose(maths::inverse(Mat3F(_matrix)));
    Vec3F offsetX = maths::normalize(normMat * Vec3F(1.f, 0.f, 0.f)) * _extents.x * scale.x;
    Vec3F offsetY = maths::normalize(normMat * Vec3F(0.f, 1.f, 0.f)) * _extents.y * scale.y;
    Vec3F offsetZ = maths::normalize(normMat * Vec3F(0.f, 0.f, 1.f)) * _extents.z * scale.z;

    bbox.points[0] = Vec3F(bbox.origin - offsetX - offsetY - offsetZ);
    bbox.points[1] = Vec3F(bbox.origin - offsetX - offsetY + offsetZ);
    bbox.points[2] = Vec3F(bbox.origin - offsetX + offsetY - offsetZ);
    bbox.points[3] = Vec3F(bbox.origin - offsetX + offsetY + offsetZ);
    bbox.points[4] = Vec3F(bbox.origin + offsetX - offsetY - offsetZ);
    bbox.points[5] = Vec3F(bbox.origin + offsetX - offsetY + offsetZ);
    bbox.points[6] = Vec3F(bbox.origin + offsetX + offsetY - offsetZ);
    bbox.points[7] = Vec3F(bbox.origin + offsetX + offsetY + offsetZ);

     return bbox;
}

Frustum sq::make_Frustum(const Mat4F& _matrix, Vec3F _origin, Vec3F _dir, float _near, float _far) {
    Frustum frus; const Mat4F invMat = maths::inverse(_matrix); Vec4F temp;
    temp = invMat * Vec4F(-1, -1, 1, 1); frus.points[1] = Vec3F(temp) / temp.w;
    temp = invMat * Vec4F(-1, +1, 1, 1); frus.points[3] = Vec3F(temp) / temp.w;
    temp = invMat * Vec4F(+1, -1, 1, 1); frus.points[2] = Vec3F(temp) / temp.w;
    temp = invMat * Vec4F(+1, +1, 1, 1); frus.points[4] = Vec3F(temp) / temp.w;
    frus.planes[1].normal = maths::norm_from_tri(frus.points[3], frus.points[1], _origin);
    frus.planes[2].normal = maths::norm_from_tri(frus.points[2], frus.points[4], _origin);
    frus.planes[3].normal = maths::norm_from_tri(frus.points[1], frus.points[2], _origin);
    frus.planes[4].normal = maths::norm_from_tri(frus.points[4], frus.points[3], _origin);
    frus.planes[1].offset = maths::dot(-frus.planes[1].normal, _origin);
    frus.planes[2].offset = maths::dot(-frus.planes[2].normal, _origin);
    frus.planes[3].offset = maths::dot(-frus.planes[3].normal, _origin);
    frus.planes[4].offset = maths::dot(-frus.planes[4].normal, _origin);
    frus.planes[0] = {-_dir, maths::dot(_dir, _origin + _dir * _near)};
    frus.points[0] = _origin; frus.radius = _far; return frus;
}

Ortho sq::make_Ortho(const Mat4F& _matrix) {
    Ortho orth; const Mat4F invMat = maths::inverse(_matrix); Vec4F temp;
    const Vec3F normX = maths::normalize(Mat3F(invMat) * Vec3F(1, 0, 0));
    const Vec3F normY = maths::normalize(Mat3F(invMat) * Vec3F(0, 1, 0));
    orth.planes[0].normal = -normX; temp = invMat * Vec4F(-1, 0, 0, 1);
    orth.planes[0].offset = maths::dot(+normX, Vec3F(temp) / temp.w);
    orth.planes[1].normal = +normX; temp = invMat * Vec4F(+1, 0, 0, 1);
    orth.planes[1].offset = maths::dot(-normX, Vec3F(temp) / temp.w);
    orth.planes[2].normal = -normY; temp = invMat * Vec4F(0, -1, 0, 1);
    orth.planes[2].offset = maths::dot(+normY, Vec3F(temp) / temp.w);
    orth.planes[3].normal = +normY; temp = invMat * Vec4F(0, +1, 0, 1);
    orth.planes[3].offset = maths::dot(-normY, Vec3F(temp) / temp.w);
    return orth;
}

Frustum sq::reflect_Frustum(const Frustum& _frus, Vec3F _normal, Vec3F _trans) {
    // todo: this all looks wrong :P
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


/////////////////////////////////////

bool sq::intersects(const Sphere& a, const Sphere& b) {

    // check for intersection between the two spheres
    return maths::distance(a.origin, b.origin) < a.radius + b.radius;
}

bool sq::intersects(const Sphere& a, const Frustum& b) {

    // check if the sphere is behind the frustum's near plane
    if (maths::dot(a.origin, b.planes[0].normal) + b.planes[0].offset > a.radius) return false;

    // check if the sphere is outside the frustum's sphere
    if (maths::distance(a.origin, b.points[0]) > a.radius + b.radius) return false;

    // check if the sphere is outside any of the frustum's side planes
    if (maths::dot(a.origin, b.planes[1].normal) + b.planes[1].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[2].normal) + b.planes[2].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[3].normal) + b.planes[3].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[4].normal) + b.planes[4].offset > a.radius) return false;

    return true;
}

bool sq::intersects(const Sphere& a, const Ortho& b) {

    // check if the sphere is outside of any of the ortho's planes
    if (maths::dot(a.origin, b.planes[0].normal) + b.planes[0].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[1].normal) + b.planes[1].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[2].normal) + b.planes[2].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[3].normal) + b.planes[3].offset > a.radius) return false;

    return true;
}

/////////////////////////////////////

bool sq::intersects(const BoundBox& a, const Sphere& b) {

    // check if the box's sphere is completely outside of the sphere
    if (maths::distance(a.origin, b.origin) > a.radius + b.radius) return false;

    // check if any of the box's corners are inside of the sphere
    if (maths::distance(a.points[0], b.origin) < b.radius) return true;
    if (maths::distance(a.points[1], b.origin) < b.radius) return true;
    if (maths::distance(a.points[2], b.origin) < b.radius) return true;
    if (maths::distance(a.points[3], b.origin) < b.radius) return true;
    if (maths::distance(a.points[4], b.origin) < b.radius) return true;
    if (maths::distance(a.points[5], b.origin) < b.radius) return true;
    if (maths::distance(a.points[6], b.origin) < b.radius) return true;
    if (maths::distance(a.points[7], b.origin) < b.radius) return true;

    return false;
}

bool sq::intersects(const BoundBox& a, const Frustum& b) {

    // check if the box's sphere is behind the frustum's near plane
    if (maths::dot(a.origin, b.planes[0].normal) + b.planes[0].offset > a.radius) return false;

    // check if the box's sphere is outside the frustum's sphere
    if (maths::distance(a.origin, b.points[0]) > a.radius + b.radius) return false;

    // check if the box's sphere is outside any of the frustum's side planes
    if (maths::dot(a.origin, b.planes[1].normal) + b.planes[1].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[2].normal) + b.planes[2].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[3].normal) + b.planes[3].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[4].normal) + b.planes[4].offset > a.radius) return false;

    // check if eight points in an array are outside of a plane
    const auto points_outside_plane = [](auto points, Plane plane) {
        return maths::dot(points[0], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[1], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[2], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[3], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[4], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[5], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[6], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[7], plane.normal) + plane.offset > 0.f; };

    // check if the box is outside of any of the frustum's planes
    if (points_outside_plane(a.points, b.planes[0]) == true) return false;
    if (points_outside_plane(a.points, b.planes[1]) == true) return false;
    if (points_outside_plane(a.points, b.planes[2]) == true) return false;
    if (points_outside_plane(a.points, b.planes[3]) == true) return false;
    if (points_outside_plane(a.points, b.planes[4]) == true) return false;

    return true;
}

bool sq::intersects(const BoundBox& a, const Ortho& b) {

    // check if the box's sphere is outside of any of the ortho's planes
    if (maths::dot(a.origin, b.planes[0].normal) + b.planes[0].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[1].normal) + b.planes[1].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[2].normal) + b.planes[2].offset > a.radius) return false;
    if (maths::dot(a.origin, b.planes[3].normal) + b.planes[3].offset > a.radius) return false;

    // check if eight points in an array are outside of a plane
    const auto points_outside_plane = [](auto points, Plane plane) {
        return maths::dot(points[0], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[1], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[2], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[3], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[4], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[5], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[6], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[7], plane.normal) + plane.offset > 0.f; };

    // check if the box is outside of any of the ortho's planes
    if (points_outside_plane(a.points, b.planes[0]) == true) return false;
    if (points_outside_plane(a.points, b.planes[1]) == true) return false;
    if (points_outside_plane(a.points, b.planes[2]) == true) return false;
    if (points_outside_plane(a.points, b.planes[3]) == true) return false;

    return true;
}

/////////////////////////////////////

bool sq::intersects(const Frustum& a, const Frustum& b) {

    // check if either frustum's sphere is behind the other's near plane
    if (maths::dot(a.points[0], b.planes[0].normal) + b.planes[0].offset > a.radius) return false;
    if (maths::dot(b.points[0], a.planes[0].normal) + a.planes[0].offset > b.radius) return false;

    // check for intersection between the two frustum's spheres
    return maths::distance(a.points[0], b.points[0]) < a.radius + b.radius;

    // check if six points in an array are outside of a plane
    const auto points_outside_plane = [](auto points, Plane plane) {
        return maths::dot(points[0], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[1], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[2], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[3], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[4], plane.normal) + plane.offset > 0.f &&
               maths::dot(points[5], plane.normal) + plane.offset > 0.f; };

    // check if either frustum is outside of any of the other's planes
    if (points_outside_plane(a.points, b.planes[0]) == true) return false;
    if (points_outside_plane(b.points, a.planes[0]) == true) return false;
    if (points_outside_plane(a.points, b.planes[1]) == true) return false;
    if (points_outside_plane(b.points, a.planes[1]) == true) return false;
    if (points_outside_plane(a.points, b.planes[2]) == true) return false;
    if (points_outside_plane(b.points, a.planes[2]) == true) return false;
    if (points_outside_plane(a.points, b.planes[3]) == true) return false;
    if (points_outside_plane(b.points, a.planes[3]) == true) return false;
    if (points_outside_plane(a.points, b.planes[4]) == true) return false;
    if (points_outside_plane(b.points, a.planes[4]) == true) return false;

    return true;
}

/////////////////////////////////////

bool sq::point_in_sphere_volume(Vec3F _point, const Sphere& _sphere) {
    return maths::distance(_point, _sphere.origin) < _sphere.radius * 1.16f;
}

bool sq::point_in_cone_volume(Vec3F _point, const Frustum& _frustum) {
    if (maths::distance(_point, _frustum.points[0]) > _frustum.radius * 1.13f) return false;
    if (maths::dot(_point, _frustum.planes[0].normal) + _frustum.planes[0].offset > 0.13f) return false;
    if (maths::dot(_point, _frustum.planes[1].normal) + _frustum.planes[1].offset > 0.13f) return false;
    if (maths::dot(_point, _frustum.planes[2].normal) + _frustum.planes[2].offset > 0.13f) return false;
    if (maths::dot(_point, _frustum.planes[3].normal) + _frustum.planes[3].offset > 0.13f) return false;
    if (maths::dot(_point, _frustum.planes[4].normal) + _frustum.planes[4].offset > 0.13f) return false;
    return true;
}
