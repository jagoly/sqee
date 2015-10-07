#include "../Cell.hpp"
#include "../World.hpp"
#include "MetaObject.hpp"

using namespace sqt::wcoe;

MetaObject::MetaObject(const string& _name, Cell* _cell) : Object(_name, _cell) {
}

void MetaObject::load_from_spec(const ObjSpec& _spec) {}

void MetaObject::refresh() {
    if (check_invalid() == true) {
    }

    animate();
}

void MetaObject::tick() {
    bool doFinish = false;
    if (doFinish == true) animate();
}

void MetaObject::calc(double _accum) {
    bool doAnim = false;
    if (doAnim == true) animate();
}

void MetaObject::animate() {
}
