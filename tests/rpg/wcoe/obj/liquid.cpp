#include <glm/gtc/type_ptr.hpp>

#include "liquid.hpp"

using namespace sqt::wcoe;

Liquid::Liquid(const string& _name, const Cell* _cell)
    : Object(ObjType::Liquid, _name, _cell) {}

void Liquid::load_from_spec(const ObjSpec& _spec) {}

void Liquid::update_from_data() {}
