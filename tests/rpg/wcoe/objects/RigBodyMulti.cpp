#include <rp3d/body/RigidBody.hpp>
#include <rp3d/engine/DynamicsWorld.hpp>
#include <rp3d/collision/shapes/BoxShape.hpp>
#include <rp3d/collision/shapes/SphereShape.hpp>

#include <sqee/physics/PhysObject.hpp>
#include <sqee/maths/General.hpp>

#include "../Cell.hpp"
#include "../World.hpp"
#include "RigBodyMulti.hpp"

using namespace sqt::wcoe;

RigBodyMulti::RigBodyMulti(const string& _name, Cell* _cell)
    : Object(typeid(RigBodyMulti), _name, _cell) {}

void RigBodyMulti::load_from_spec(const ObjSpec& _spec) {}

void RigBodyMulti::refresh() {
    if (invalid == false) return;

    animate(); invalid = false;
}

void RigBodyMulti::update() {
    bool doFinish = false;
    if (doFinish == true) animate();
}

void RigBodyMulti::calc(double _accum) {
    bool doAnim = false;
    if (doAnim == true) animate();
}

void RigBodyMulti::animate() {
}
