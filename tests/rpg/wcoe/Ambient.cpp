#include "Ambient.hpp"

using namespace sqt;

Ambient::Ambient() {
    ubo.reserve("colour", 4u);
    ubo.create_and_allocate();
}

void Ambient::update() {
    ubo.update("colour", &PROP_colour);
}
