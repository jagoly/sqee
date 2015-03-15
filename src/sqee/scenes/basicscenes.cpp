#include "app/application.hpp"
#include "text/text.hpp"
#include "scenes/basicscenes.hpp"

using namespace sq;

void SceneFPS::update() {}

void SceneFPS::render(float _ft) {
    static float ft = 1.f/60.f;
    ft = ft*0.8f + _ft*0.2f;

    char rounded[8]; std::sprintf(rounded, "%.2f", 1.f / ft);
    sq::draw_tiny_text(rounded, 5, Alignment::TL, {8, 10}, app.get_size());
}

void SceneFPS::resize(uvec2 _size) {}
