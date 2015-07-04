#include <reactphysics3d/body/RigidBody.h>
#include <reactphysics3d/engine/DynamicsWorld.h>
#include <reactphysics3d/collision/shapes/BoxShape.h>
#include <reactphysics3d/collision/shapes/SphereShape.h>

#include <sqee/physics/PhysObject.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "RigBodyMulti.hpp"

using namespace sqt::wcoe;

RigBodyMulti::RigBodyMulti(const string& _name, Cell* _cell)
    : Object(ObjType::RigBodyMulti, _name, _cell) {}

void RigBodyMulti::load_from_spec(const ObjSpec& _spec) {
}

void RigBodyMulti::refresh() {
    animate();
}

void RigBodyMulti::tick() {
    bool doFinish = false;
    if (doFinish == true) animate();
}

void RigBodyMulti::calc(double _accum) {
    bool doAnim = false;
    if (doAnim == true) animate();
}

void RigBodyMulti::animate() {
}
