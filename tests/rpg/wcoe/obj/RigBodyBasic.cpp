#include <rp3d/body/RigidBody.hpp>
#include <rp3d/engine/DynamicsWorld.hpp>
#include <rp3d/collision/shapes/BoxShape.hpp>
#include <rp3d/collision/shapes/SphereShape.hpp>

#include <sqee/physics/PhysObject.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "ModelStatic.hpp"
#include "RigBodyBasic.hpp"

using namespace sqt::wcoe;

RigBodyBasic::RigBodyBasic(const string& _name, Cell* _cell)
    : Object(ObjType::RigBodyBasic, _name, _cell) {}

RigBodyBasic::~RigBodyBasic() {
    //cell->world->physWorld->destroyRigidBody(rigBody);
}

void RigBodyBasic::load_from_spec(const ObjSpec& _spec) {
    assert_string(_spec, name, "phys");

    PROP_physPath = _spec.stringMap.at("phys");
}

void RigBodyBasic::refresh() {
    if ((phys = sq::res::phys().get(PROP_physPath)) == nullptr)
        phys = sq::res::phys().add(PROP_physPath),
        phys->create(PROP_physPath);

    if (rigBody != nullptr) cell->world->physWorld->destroyRigidBody(rigBody);

    if (objectPtr != nullptr) {
        if (objectPtr->type == ObjType::ModelStatic) {
            auto& castObj = static_cast<ModelStatic&>(*objectPtr);
            positionPtr = &castObj.PROP_position;
            rotationPtr = &castObj.PROP_rotation;
        }
        positionCrnt = positionNext = *positionPtr;
        rotationCrnt = rotationNext = *rotationPtr;
        rigBody = cell->world->physWorld->createRigidBody(rp3d::Transform(
            sq::rp3d_cast(*positionPtr), sq::rp3d_cast(*rotationPtr)));
    } else rigBody = cell->world->physWorld->createRigidBody(rp3d::Transform());

    if (phys->bodyType == sq::PhysObject::BodyType::STATIC)
        rigBody->setType(rp3d::BodyType::STATIC);

    if (phys->lineardamp != 0.f) rigBody->setLinearDamping(phys->lineardamp);
    if (phys->angulardamp != 0.f) rigBody->setAngularDamping(phys->angulardamp);
    if (phys->bounciness != 0.5f) rigBody->getMaterial().setBounciness(phys->bounciness);
    if (phys->friction != 0.3f) rigBody->getMaterial().setFrictionCoefficient(phys->friction);

    for (unique_ptr<sq::PhysObject::Shape>& shape : phys->shapeVec) {
        if (shape->type == sq::PhysObject::ShapeType::BOX) {
            auto& bs = static_cast<sq::PhysObject::BoxShape&>(*shape.get());
            rigBody->addCollisionShape(rp3d::BoxShape(sq::rp3d_cast(bs.halfsize * PROP_scale)),
                rp3d::Transform(sq::rp3d_cast(bs.offset * PROP_scale),
                {0.f, 0.f, 0.f, 1.f}), bs.mass * PROP_scale.x);
        } else
        if (shape->type == sq::PhysObject::ShapeType::SPHERE) {
            auto& ss = static_cast<sq::PhysObject::SphereShape&>(*shape.get());
            rigBody->addCollisionShape(rp3d::SphereShape(ss.radius * PROP_scale.x),
                rp3d::Transform(sq::rp3d_cast(ss.offset * PROP_scale),
                {0.f, 0.f, 0.f, 1.f}), ss.mass * PROP_scale.x);
        }
    }

    animate();
}

void RigBodyBasic::tick() {
    bool doFinish = false;
    if (doFinish == true) animate();

    positionCrnt = positionNext;
    rotationCrnt = rotationNext;

    rp3d::Transform transform = rigBody->getTransform();
    positionNext = sq::glm_cast(transform.getPosition());
    rotationNext = sq::glm_cast(transform.getOrientation());
}

void RigBodyBasic::calc(double _accum) {
    bool doAnim = false;
    if (doAnim == true) animate();

    if (objectPtr != nullptr) {
        *positionPtr = glm::mix(positionCrnt, positionNext, float(24*_accum));
        *rotationPtr = glm::slerp(rotationCrnt, rotationNext, float(24*_accum));
        objectPtr->animate();
    }
}

void RigBodyBasic::animate() {
}


void RigBodyBasic::FUNC_set_ModelStatic(ModelStatic* _object) {
    objectPtr = _object;
}
