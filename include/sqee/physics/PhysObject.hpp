#pragma once
#include <sqee/forward.hpp>

#include <sqee/misc/ResHolder.hpp>
#include <sqee/physics/RP3D.hpp>

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
        ShapeType type; fvec3 offset; float mass;
    };

    struct BoxShape : public Shape {
        BoxShape() : Shape(ShapeType::BOX) {}
        fvec3 halfsize;
    };

    struct SphereShape : public Shape {
        SphereShape() : Shape(ShapeType::SPHERE) {}
        float radius;
    };

    vector<unique_ptr<Shape>> shapeVec;
};

namespace res {
ResHolder<PhysObject>& phys();
}

}
