/*#pragma once

#include <map>

#include <rp3d/body/RigidBody.hpp>

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>
#include <sqee/maths/Quaternion.hpp>
#include <sqee/physics/VecTypes.hpp>







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







namespace sq {

class BaseBody : NonCopyable {
public:
    BaseBody(rp3d::DynamicsWorld& _world); ~BaseBody();
    std::map<uint, unique_ptr<rp3d::CollisionShape>> shapeMap;
    rp3d::DynamicsWorld& world; rp3d::RigidBody* const body;

    void set_transform(Vec3F _pos, QuatF _rot) { body->setTransform(rp3d_cast(_pos, _rot)); }
    void set_position(Vec3F _pos) { body->setTransform({rp3d_cast(_pos), body->getTransform().getPosition()}); }
    void set_rotation(Vec3F _rot) { body->setTransform({rp3d_cast(_rot), body->getTransform().getOrientation()}); }
    void set_friction(float _value) { body->getMaterial().setFrictionCoefficient(_value); }
    void set_bounciness(float _value) { body->getMaterial().setBounciness(_value); }

    Mat4F get_transform() const { return sqee_cast(body->getTransform()); }
    Vec3F get_position() const { return sqee_cast(body->getTransform().getPosition()); }
    QuatF get_rotation() const { return sqee_cast(body->getTransform().getOrientation()); }
    float get_friction() const { return body->getMaterial().getFrictionCoefficient(); }
    float get_bounciness() const { return body->getMaterial().getBounciness(); }
};

class StaticBody : public BaseBody {
public:
    StaticBody(rp3d::DynamicsWorld& _world);

    void add_BoxShape(uint _id, Vec3F _extent, Vec3F _pos, QuatF _rot);
    void add_SphereShape(uint _id, float _radius, Vec3F _pos, QuatF _rot);
    void add_ConeShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot);
    void add_CylinderShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot);
    void add_CapsuleShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot);

    void add_BoxShape(uint _id, Vec3F _extent) { add_BoxShape(_id, _extent, {}, {}); }
    void add_SphereShape(uint _id, float _radius) { add_SphereShape(_id, _radius, {}, {}); }
    void add_ConeShape(uint _id, float _radius, float _height) { add_ConeShape(_id, _radius, _height, {}, {}); }
    void add_CylinderShape(uint _id, float _radius, float _height) { add_CylinderShape(_id, _radius, _height, {}, {}); }
    void add_CapsuleShape(uint _id, float _radius, float _height) { add_CapsuleShape(_id, _radius, _height, {}, {}); }
};

class DynamicBody : public BaseBody {
public:
    DynamicBody(rp3d::DynamicsWorld& _world);

    void set_linearDamp(float _value) { body->setLinearDamping(_value); }
    void set_angularDamp(float _value) { body->setAngularDamping(_value); }
    void set_linearVelocity(Vec3F _value) { body->setLinearVelocity(rp3d_cast(_value)); }
    void set_angularVelocity(Vec3F _value) { body->setAngularVelocity(rp3d_cast(_value)); }
    void set_rollResistance(float _value) { body->getMaterial().setRollingResistance(_value); }

    float get_linearDamp() const { return body->getLinearDamping(); }
    float get_angularDamp() const { return body->getAngularDamping(); }
    Vec3F get_linearVelocity() const { return sqee_cast(body->getLinearVelocity()); }
    Vec3F get_angularVelocity() const { return sqee_cast(body->getAngularVelocity()); }
    float get_rollResistance() const { return body->getMaterial().getRollingResistance(); }
    float get_mass() const { return body->getMass(); }

    void add_BoxShape(uint _id, Vec3F _extent, Vec3F _pos, QuatF _rot, float _mass);
    void add_SphereShape(uint _id, float _radius, Vec3F _pos, QuatF _rot, float _mass);
    void add_ConeShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot, float _mass);
    void add_CylinderShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot, float _mass);
    void add_CapsuleShape(uint _id, float _radius, float _height, Vec3F _pos, QuatF _rot, float _mass);

    void add_BoxShape(uint _id, Vec3F _extent, float _mass) { add_BoxShape(_id, _extent, {}, {}, _mass); }
    void add_SphereShape(uint _id, float _radius, float _mass) { add_SphereShape(_id, _radius, {}, {}, _mass); }
    void add_ConeShape(uint _id, float _radius, float _height, float _mass) { add_ConeShape(_id, _radius, _height, {}, {}, _mass); }
    void add_CylinderShape(uint _id, float _radius, float _height, float _mass) { add_CylinderShape(_id, _radius, _height, {}, {}, _mass); }
    void add_CapsuleShape(uint _id, float _radius, float _height, float _mass) { add_CapsuleShape(_id, _radius, _height, {}, {}, _mass); }

    void apply_force(Vec3F _force, Vec3F _pos) { body->applyForce(rp3d_cast(_force), rp3d_cast(_pos)); }
    void apply_force_centre(Vec3F _force) { body->applyForceToCenterOfMass(rp3d_cast(_force)); }
};

///// Spec class to define a ReactPhysics3D entity
//class RigidBodyObj : NonCopyable {
//public:
//    void create(const string& _path);

//    enum class ShapeType { BOX, SPHERE };
//    enum class BodyType { STATIC, DYNAMIC };
//    BodyType bodyType = BodyType::STATIC;

//    float lineardamp = 0.0f;
//    float angulardamp = 0.0f;
//    float bounciness = 0.5f;
//    float friction = 0.3f;

//    struct Shape {
//        Shape(ShapeType _type) : type(_type) {}
//        ShapeType type; Vec3F offset; float mass;
//    };

//    struct BoxShape : public Shape {
//        BoxShape() : Shape(ShapeType::BOX) {}
//        Vec3F halfsize;
//    };

//    struct SphereShape : public Shape {
//        SphereShape() : Shape(ShapeType::SPHERE) {}
//        float radius;
//    };

//    vector<unique_ptr<Shape>> shapeVec;
//};

}
*/
