#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/render/Camera.hpp>

#include "World.hpp"
#include "Ambient.hpp"

using namespace sqt::wcoe;

Ambient::Ambient(const World& _world) : world(_world) {
    ubo.reserve("colour", 4u);
    ubo.create_and_allocate();
}

void Ambient::refresh() {
    if (PROP_enabled == false) return;

    animate();
}

void Ambient::update() {
    if (PROP_enabled == false) return; bool doFinish = false;
    if (ANIM_colour.active() && ANIM_colour.tick()) doFinish = true;
    if (doFinish == true) animate();
}

void Ambient::calc(double _accum) {
    if (PROP_enabled == false) return; bool doAnim = false;
    if (ANIM_colour.active()) { ANIM_colour.calc(_accum); doAnim = true; }
    if (doAnim == true) animate();
}

void Ambient::animate() {
    ubo.update("colour", &PROP_colour);
}
