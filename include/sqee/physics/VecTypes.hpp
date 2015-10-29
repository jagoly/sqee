#pragma once

#include <rp3d/mathematics/Vector2.hpp>
#include <rp3d/mathematics/Vector3.hpp>
#include <rp3d/mathematics/Matrix2x2.hpp>
#include <rp3d/mathematics/Matrix3x3.hpp>
#include <rp3d/mathematics/Transform.hpp>
#include <rp3d/mathematics/Quaternion.hpp>

#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Matrices.hpp>
#include <sqee/maths/Quaternion.hpp>

namespace sq {

inline Vec2F glm_cast(rp3d::Vector2 _vec2) {
    return Vec2F(_vec2.x, _vec2.y);
}

inline Vec3F glm_cast(rp3d::Vector3 _vec3) {
    return Vec3F(_vec3.x, _vec3.y, _vec3.z);
}

//inline Mat2F glm_cast(rp3d::Matrix2x2 _mat2) {
//    return Mat2F(_mat2.getRow(0).x, _mat2.getRow(0).y,
//                 _mat2.getRow(1).x, _mat2.getRow(1).y);
//}

inline Mat3F glm_cast(rp3d::Matrix3x3 _mat3) {
    return Mat3F(_mat3.getRow(0).x, _mat3.getRow(0).y, _mat3.getRow(0).z,
                 _mat3.getRow(1).x, _mat3.getRow(1).y, _mat3.getRow(1).z,
                 _mat3.getRow(2).x, _mat3.getRow(2).y, _mat3.getRow(2).z);
}

inline QuatF glm_cast(rp3d::Quaternion _quat) {
    return QuatF(_quat.x, _quat.y, _quat.z, _quat.w);
}


inline rp3d::Vector2 rp3d_cast(Vec2F _vec2) {
    return rp3d::Vector2(_vec2.x, _vec2.y);
}

inline rp3d::Vector3 rp3d_cast(Vec3F _vec3) {
    return rp3d::Vector3(_vec3.x, _vec3.y, _vec3.z);
}

//inline rp3d::Matrix2x2 rp3d_cast(Mat2F _mat2) {
//    return rp3d::Matrix2x2(_mat2[0][0], _mat2[0][1],
//                           _mat2[1][0], _mat2[1][1]);
//}

inline rp3d::Matrix3x3 rp3d_cast(Mat3F _mat3) {
    return rp3d::Matrix3x3(_mat3[0][0], _mat3[0][1], _mat3[0][2],
                           _mat3[1][0], _mat3[1][1], _mat3[1][2],
                           _mat3[2][0], _mat3[2][1], _mat3[2][2]);
}

inline rp3d::Quaternion rp3d_cast(QuatF _quat) {
    return rp3d::Quaternion(_quat.x, _quat.y, _quat.z, _quat.w);
}

}
