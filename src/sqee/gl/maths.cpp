#include <glm/gtc/matrix_transform.hpp>

#include <sqee/app/logging.hpp>

#include "sqee/gl/maths.hpp"

using namespace sq;

dvec3 sq::make_tangent(dvec3 _norm) {
    dvec3 t1(glm::cross(_norm, {1, 0, 0})), t2(glm::cross(_norm, {0, 1, 0}));
    return glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
}

dmat3 sq::make_normMat(dmat4 _viewModelMat) {
    return dmat3(glm::transpose(glm::inverse(_viewModelMat)));
}

inline double sign(dvec2 _t0, dvec2 _t1, dvec2 _t2) {
    return (_t0.x - _t2.x) * (_t1.y - _t2.y) - (_t1.x - _t2.x) * (_t0.y - _t2.y);
}

bool sq::point_in_tri(dvec2 _p, dvec2 _t0, dvec2 _t1, dvec2 _t2) {
    bool b0, b1, b2;
    b0 = sign(_p, _t0, _t1) < 0.0;
    b1 = sign(_p, _t1, _t2) < 0.0;
    b2 = sign(_p, _t2, _t0) < 0.0;
    return (b0 == b1) && (b1 == b2);
}

dvec3 sq::norm_from_tri(dvec3 _t0, dvec3 _t1, dvec3 _t2) {
    dvec3 p0 = _t1 - _t0; dvec3 p1 = _t2 - _t0;
    return glm::normalize(glm::cross(p0, p1));
}

double sq::dist_from_plane(dvec3 _point, dvec4 _plane) {
    return glm::dot(_point, dvec3(_plane)) + _plane.w;
}

bool sq::sphere_in_frus(dvec4 _sphere, dmat4 _mat, Frustum _frus) {
    dvec4 pT(norm_from_tri(_frus.xYz, _frus.XYz, _frus.xYZ), 0);
    dvec4 pB(norm_from_tri(_frus.XyZ, _frus.Xyz, _frus.xyZ), 0);
    dvec4 pL(norm_from_tri(_frus.xyz, _frus.xYz, _frus.xyZ), 0);
    dvec4 pR(norm_from_tri(_frus.XYZ, _frus.XYz, _frus.XyZ), 0);
    dvec4 pN(norm_from_tri(_frus.xyz, _frus.Xyz, _frus.xYz), 0);
    dvec4 pF(norm_from_tri(_frus.XYZ, _frus.XyZ, _frus.xYZ), 0);
    pT.w = glm::dot(-dvec3(pT), _frus.xYz);
    pB.w = glm::dot(-dvec3(pB), _frus.XyZ);
    pL.w = glm::dot(-dvec3(pL), _frus.xyz);
    pR.w = glm::dot(-dvec3(pR), _frus.XYZ);
    pN.w = glm::dot(-dvec3(pN), _frus.xyz);
    pF.w = glm::dot(-dvec3(pF), _frus.XYZ);

    double scaX(glm::length(_mat[0])), scaY(glm::length(_mat[1])), scaZ(glm::length(_mat[2]));
    dvec3 pos(_mat * dvec4(dvec3(_sphere), 1));
    double rad(_sphere.w * glm::max(scaX, glm::max(scaY, scaZ)));
    _sphere = dvec4(pos, rad);

    if (dist_from_plane(dvec3(_sphere), pT) > _sphere.w) return true;
    if (dist_from_plane(dvec3(_sphere), pR) > _sphere.w) return true;
    if (dist_from_plane(dvec3(_sphere), pB) > _sphere.w) return true;
    if (dist_from_plane(dvec3(_sphere), pL) > _sphere.w) return true;
    if (dist_from_plane(dvec3(_sphere), pN) > _sphere.w) return true;
    if (dist_from_plane(dvec3(_sphere), pF) > _sphere.w) return true;
    return false;
}

Frustum sq::make_Frustum(dmat4 _invProjViewMat) {
    Frustum fr; dvec4 temp;
    temp = _invProjViewMat * dvec4(-1, -1, -1, 1); fr.xyz = dvec3(temp) / temp.w;
    temp = _invProjViewMat * dvec4(-1, -1,  1, 1); fr.xyZ = dvec3(temp) / temp.w;
    temp = _invProjViewMat * dvec4(-1,  1, -1, 1); fr.xYz = dvec3(temp) / temp.w;
    temp = _invProjViewMat * dvec4(-1,  1,  1, 1); fr.xYZ = dvec3(temp) / temp.w;
    temp = _invProjViewMat * dvec4( 1, -1, -1, 1); fr.Xyz = dvec3(temp) / temp.w;
    temp = _invProjViewMat * dvec4( 1, -1,  1, 1); fr.XyZ = dvec3(temp) / temp.w;
    temp = _invProjViewMat * dvec4( 1,  1, -1, 1); fr.XYz = dvec3(temp) / temp.w;
    temp = _invProjViewMat * dvec4( 1,  1,  1, 1); fr.XYZ = dvec3(temp) / temp.w;
    return fr;
}

dvec3 sq::calc_frusCentre(Frustum _frus) {
    dvec3 min(INFINITY, INFINITY, INFINITY);
    dvec3 max(-INFINITY, -INFINITY, -INFINITY);
    for (const dvec3& vec : {_frus.xyz, _frus.xyZ, _frus.xYz, _frus.xYZ,
                             _frus.Xyz, _frus.XyZ, _frus.XYz, _frus.XYZ}) {
        min = glm::min(min, vec);
        max = glm::max(max, vec);
    }
    return (min + max) / 2.0;
}
