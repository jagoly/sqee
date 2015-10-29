#include "../Cell.hpp"
#include "../World.hpp"
#include "Liquid.hpp"

using namespace sqt::wcoe;

Liquid::Liquid(const string& _name, Cell* _cell)
    : Object(typeid(Liquid), _name, _cell) {}

void Liquid::load_from_spec(const ObjSpec& _spec) {}

void Liquid::refresh() {
    if (invalid == false) return;

    animate(); invalid = false;
}

void Liquid::update() {}

void Liquid::calc(double _accum) {}

void Liquid::animate() {}
