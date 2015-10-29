#include "../Cell.hpp"
#include "../World.hpp"
#include "MetaObject.hpp"

using namespace sqt::wcoe;

MetaObject::MetaObject(const string& _name, Cell* _cell)
    : Object(typeid(MetaObject), _name, _cell) {}

void MetaObject::load_from_spec(const ObjSpec& _spec) {}

void MetaObject::refresh() {
    if (invalid == false) return;

    animate(); invalid = false;
}

void MetaObject::update() {
    bool doFinish = false;
    if (doFinish == true) animate();
}

void MetaObject::calc(double _accum) {
    bool doAnim = false;
    if (doAnim == true) animate();
}

void MetaObject::animate() {}
