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

inline Vec2F sqee_cast(rp3d::Vector2 _vec) {
    return Vec2F(_vec.x, _vec.y);
}

inline Vec3F sqee_cast(rp3d::Vector3 _vec) {
    return Vec3F(_vec.x, _vec.y, _vec.z);
}

inline Mat2F sqee_cast(rp3d::Matrix2x2 _mat) {
    return Mat2F({_mat.getRow(0).x, _mat.getRow(0).y},
                 {_mat.getRow(1).x, _mat.getRow(1).y});
}

inline Mat3F sqee_cast(rp3d::Matrix3x3 _mat) {
    return Mat3F({_mat.getRow(0).x, _mat.getRow(0).y, _mat.getRow(0).z},
                 {_mat.getRow(1).x, _mat.getRow(1).y, _mat.getRow(1).z},
                 {_mat.getRow(2).x, _mat.getRow(2).y, _mat.getRow(2).z});
}

inline QuatF sqee_cast(rp3d::Quaternion _quat) {
    return QuatF(_quat.x, _quat.y, _quat.z, _quat.w);
}

inline Mat4F sqee_cast(rp3d::Transform _transform) {
    Mat4F matrix(Mat3F(sqee_cast(_transform.getOrientation())));
    matrix[3] = Vec4F(sqee_cast(_transform.getPosition()), 1.f);
    return matrix;
}

inline rp3d::Vector2 rp3d_cast(Vec2F _vec) {
    return rp3d::Vector2(_vec.x, _vec.y);
}

inline rp3d::Vector3 rp3d_cast(Vec3F _vec) {
    return rp3d::Vector3(_vec.x, _vec.y, _vec.z);
}

inline rp3d::Matrix2x2 rp3d_cast(Mat2F _mat) {
    return rp3d::Matrix2x2(_mat[0][0], _mat[0][1],
                           _mat[1][0], _mat[1][1]);
}

inline rp3d::Matrix3x3 rp3d_cast(Mat3F _mat) {
    return rp3d::Matrix3x3(_mat[0][0], _mat[0][1], _mat[0][2],
                           _mat[1][0], _mat[1][1], _mat[1][2],
                           _mat[2][0], _mat[2][1], _mat[2][2]);
}

inline rp3d::Quaternion rp3d_cast(QuatF _quat) {
    return rp3d::Quaternion(_quat.x, _quat.y, _quat.z, _quat.w);
}

inline rp3d::Transform rp3d_cast(Mat4F _matrix) {
    return rp3d::Transform(rp3d_cast(Vec3F(_matrix[3])), rp3d_cast(Mat3F(_matrix)));
}

inline rp3d::Transform rp3d_cast(Vec3F _pos, QuatF _rot) {
    return rp3d::Transform(rp3d_cast(_pos), rp3d_cast(_rot));
}

}
