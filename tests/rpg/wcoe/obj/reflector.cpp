#include "reflector.hpp"

using namespace sqt::wcoe;

Reflector::Reflector(const string& _name, const Cell& _cell)
    : Object(ObjType::Reflector, _name, _cell) {}

void Reflector::load_from_spec(const ObjSpec& _spec) {}
