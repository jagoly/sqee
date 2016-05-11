/*#include <rp3d/body/RigidBody.hpp>
#include <rp3d/engine/DynamicsWorld.hpp>
#include <rp3d/collision/shapes/BoxShape.hpp>
#include <rp3d/collision/shapes/SphereShape.hpp>

#include <sqee/app/Resources.hpp>
#include <sqee/physics/PhysObject.hpp>
#include <sqee/physics/VecTypes.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "ModelSimple.hpp"
#include "RigBodyBasic.hpp"

#include <sqee/debug/Logging.hpp>

using namespace sqt::wcoe;
namespace maths = sq::maths;

RigBodyBasic::RigBodyBasic(const string& _name, Cell* _cell)
    : Object(typeid(RigBodyBasic), _name, _cell) {}

void RigBodyBasic::load_from_spec(const ObjSpec& _spec) {
    _spec.set_if("physobj", PROP_physobj);
}

void RigBodyBasic::refresh() {
    if (invalid == false) return;
    // todo: don't reset values

    if ((physobj = sq::static_PhysObject().get(PROP_physobj)) == nullptr)
        physobj = sq::static_PhysObject().add(PROP_physobj),
        physobj->create(PROP_physobj);

    if (rigBody != nullptr) world.physWorld->destroyRigidBody(rigBody);

    if (objectPtr != nullptr) {
//        if (objectPtr->type == typeid(ModelSimple)) {
//            auto& castObj = static_cast<ModelSimple&>(*objectPtr);
//            positionPtr = &castObj.PROP_position;
//            rotationPtr = &castObj.PROP_rotation;
//        }
        positionCrnt = positionNext = *positionPtr;
        rotationCrnt = rotationNext = *rotationPtr;
        rigBody = world.physWorld->createRigidBody(rp3d::Transform(
            sq::rp3d_cast(*positionPtr), sq::rp3d_cast(*rotationPtr)));
    } else rigBody = world.physWorld->createRigidBody(rp3d::Transform());

    if (physobj->bodyType == sq::PhysObject::BodyType::STATIC)
        rigBody->setType(rp3d::BodyType::STATIC);

    rigBody->setLinearDamping(physobj->lineardamp);
    rigBody->setAngularDamping(physobj->angulardamp);
    rigBody->getMaterial().setBounciness(physobj->bounciness);
    rigBody->getMaterial().setFrictionCoefficient(physobj->friction);

//    for (unique_ptr<sq::PhysObject::Shape>& shape : physobj->shapeVec) {
//        if (shape->type == sq::PhysObject::ShapeType::BOX) {
//            auto& bs = static_cast<sq::PhysObject::BoxShape&>(*shape.get());
//            rigBody->addCollisionShape(rp3d::BoxShape(sq::rp3d_cast(bs.halfsize * PROP_scale)),
//                rp3d::Transform(sq::rp3d_cast(bs.offset * PROP_scale),
//                {0.f, 0.f, 0.f, 1.f}), bs.mass * PROP_scale.x);
//        } else
//        if (shape->type == sq::PhysObject::ShapeType::SPHERE) {
//            auto& ss = static_cast<sq::PhysObject::SphereShape&>(*shape.get());
//            rigBody->addCollisionShape(rp3d::SphereShape(ss.radius * PROP_scale.x),
//                rp3d::Transform(sq::rp3d_cast(ss.offset * PROP_scale),
//                {0.f, 0.f, 0.f, 1.f}), ss.mass * PROP_scale.x);
//        }
//    }

    animate(); invalid = false;
}

void RigBodyBasic::update() {
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
        *positionPtr = maths::mix(positionCrnt, positionNext, float(_accum)*24.f);
        *rotationPtr = maths::slerp(rotationCrnt, rotationNext, float(_accum)*24.f);
        objectPtr->animate();
    }
}

void RigBodyBasic::animate() {}


//void RigBodyBasic::FUNC_set_ModelSimple(ModelSimple* _object) {
//    objectPtr = _object;
//}
*/
