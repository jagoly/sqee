#pragma once
#include <sqee/forward.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <rp3d/mathematics/Vector2.hpp>
#include <rp3d/mathematics/Vector3.hpp>
#include <rp3d/mathematics/Matrix2x2.hpp>
#include <rp3d/mathematics/Matrix3x3.hpp>
#include <rp3d/mathematics/Transform.hpp>
#include <rp3d/mathematics/Quaternion.hpp>

namespace reactphysics3d {
class DynamicsWorld;
class RigidBody;
}

namespace sq {

inline fvec2 glm_cast(rp3d::Vector2 _vec2) {
    return fvec2(_vec2.x, _vec2.y);
}

inline fvec3 glm_cast(rp3d::Vector3 _vec3) {
    return fvec3(_vec3.x, _vec3.y, _vec3.z);
}

inline fmat2 glm_cast(rp3d::Matrix2x2 _mat2) {
    return fmat2(_mat2.getRow(0).x, _mat2.getRow(0).y,
                 _mat2.getRow(1).x, _mat2.getRow(1).y);
}

inline fmat3 glm_cast(rp3d::Matrix3x3 _mat3) {
    return fmat3(_mat3.getRow(0).x, _mat3.getRow(0).y, _mat3.getRow(0).z,
                 _mat3.getRow(1).x, _mat3.getRow(1).y, _mat3.getRow(1).z,
                 _mat3.getRow(2).x, _mat3.getRow(2).y, _mat3.getRow(2).z);
}

inline fquat glm_cast(rp3d::Quaternion _quat) {
    return fquat(_quat.w, _quat.x, _quat.y, _quat.z);
}


inline rp3d::Vector2 rp3d_cast(fvec2 _vec2) {
    return rp3d::Vector2(_vec2.x, _vec2.y);
}

inline rp3d::Vector3 rp3d_cast(fvec3 _vec3) {
    return rp3d::Vector3(_vec3.x, _vec3.y, _vec3.z);
}

inline rp3d::Matrix2x2 rp3d_cast(fmat2 _mat2) {
    return rp3d::Matrix2x2(_mat2[0][0], _mat2[0][1],
                           _mat2[1][0], _mat2[1][1]);
}

inline rp3d::Matrix3x3 rp3d_cast(fmat3 _mat3) {
    return rp3d::Matrix3x3(_mat3[0][0], _mat3[0][1], _mat3[0][2],
                           _mat3[1][0], _mat3[1][1], _mat3[1][2],
                           _mat3[2][0], _mat3[2][1], _mat3[2][2]);
}

inline rp3d::Quaternion rp3d_cast(fquat _quat) {
    return rp3d::Quaternion(_quat.x, _quat.y, _quat.z, _quat.w);
}

}
