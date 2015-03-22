#include <glm/gtc/matrix_transform.hpp>

#include "gl/maths.hpp"

using namespace sq;

vec3 sq::make_tangent(vec3 _norm) {
    vec3 t1(glm::cross(_norm, {1, 0, 0})), t2(glm::cross(_norm, {0, 1, 0}));
    return glm::normalize(glm::length(t1) > glm::length(t2) ? t1 : t2);
}

inline float sign(vec2 _t0, vec2 _t1, vec2 _t2) {
    return (_t0.x - _t2.x) * (_t1.y - _t2.y) - (_t1.x - _t2.x) * (_t0.y - _t2.y);
}

bool sq::point_in_tri(vec2 _p, vec2 _t0, vec2 _t1, vec2 _t2) {
    bool b0, b1, b2;
    b0 = sign(_p, _t0, _t1) < 0.f;
    b1 = sign(_p, _t1, _t2) < 0.f;
    b2 = sign(_p, _t2, _t0) < 0.f;
    return (b0 == b1) && (b1 == b2);
}

Frustum sq::make_Frustum(const mat4& _invProjView) {
    Frustum fr; vec4 temp;

    temp = _invProjView * vec4(-1, -1, -1, 1); fr.xyz = vec3(temp) / temp.w;
    temp = _invProjView * vec4(-1, -1,  1, 1); fr.xyZ = vec3(temp) / temp.w;
    temp = _invProjView * vec4(-1,  1, -1, 1); fr.xYz = vec3(temp) / temp.w;
    temp = _invProjView * vec4(-1,  1,  1, 1); fr.xYZ = vec3(temp) / temp.w;
    temp = _invProjView * vec4( 1, -1, -1, 1); fr.Xyz = vec3(temp) / temp.w;
    temp = _invProjView * vec4( 1, -1,  1, 1); fr.XyZ = vec3(temp) / temp.w;
    temp = _invProjView * vec4( 1,  1, -1, 1); fr.XYz = vec3(temp) / temp.w;
    temp = _invProjView * vec4( 1,  1,  1, 1); fr.XYZ = vec3(temp) / temp.w;

    return fr;
}

vec3 sq::calc_frusCentre(const Frustum& _frus) {
    vec3 min(INFINITY, INFINITY, INFINITY);
    vec3 max(-INFINITY, -INFINITY, -INFINITY);
    for (auto& vec : {_frus.xyz, _frus.xyZ, _frus.xYz, _frus.xYZ,
                      _frus.Xyz, _frus.XyZ, _frus.XYz, _frus.XYZ}) {
        min = glm::min(min, vec);
        max = glm::max(max, vec);
    }
    return (min + max) / 2.f;
}

Frustum sq::make_boxFrustum(const mat4& _invProjView) {
    Frustum frus = make_Frustum(_invProjView);

    vec3 min(INFINITY, INFINITY, INFINITY);
    vec3 max(-INFINITY, -INFINITY, -INFINITY);
    for (auto& vec : {frus.xyz, frus.xyZ, frus.xYz, frus.xYZ,
                      frus.Xyz, frus.XyZ, frus.XYz, frus.XYZ}) {
        min = glm::min(min, vec);
        max = glm::max(max, vec);
    }

    frus.xyz = {min.x, min.y, min.z};
    frus.xyZ = {min.x, min.y, max.z};
    frus.xYz = {min.x, max.y, min.z};
    frus.xYZ = {min.x, max.y, max.z};
    frus.Xyz = {max.x, min.y, min.z};
    frus.XyZ = {max.x, min.y, max.z};
    frus.XYz = {max.x, max.y, min.z};
    frus.XYZ = {max.x, max.y, max.z};

    return frus;
}
