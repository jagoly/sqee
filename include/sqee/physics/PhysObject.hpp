#pragma once

#include <sqee/builtins.hpp>
#include <sqee/maths/Vectors.hpp>

namespace sq {

/// Spec class to define a ReactPhysics3D entity
class PhysObject : NonCopyable {
public:
    void create(const string& _path);

    enum class ShapeType { BOX, SPHERE };
    enum class BodyType { STATIC, DYNAMIC };
    BodyType bodyType = BodyType::STATIC;

    float lineardamp = 0.0f;
    float angulardamp = 0.0f;
    float bounciness = 0.5f;
    float friction = 0.3f;

    struct Shape {
        Shape(ShapeType _type) : type(_type) {}
        ShapeType type; Vec3F offset; float mass;
    };

    struct BoxShape : public Shape {
        BoxShape() : Shape(ShapeType::BOX) {}
        Vec3F halfsize;
    };

    struct SphereShape : public Shape {
        SphereShape() : Shape(ShapeType::SPHERE) {}
        float radius;
    };

    vector<unique_ptr<Shape>> shapeVec;
};

}
