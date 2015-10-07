#include "Liquid.hpp"

using namespace sqt::wcoe;

Liquid::Liquid(const string& _name, Cell* _cell) : Object(_name, _cell) {}

void Liquid::load_from_spec(const ObjSpec& _spec) {}

void Liquid::refresh() {}

void Liquid::tick() {}

void Liquid::calc(double _accum) {}

void Liquid::animate() {}
