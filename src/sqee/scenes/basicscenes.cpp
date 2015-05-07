#include "sqee/app/application.hpp"
#include "sqee/text/text.hpp"
#include "sqee/scenes/basicscenes.hpp"

using namespace sq;

void SceneFPS::render(float _ft) {
    static float ft = 1.f/60.f;
    ft = ft*0.75f + _ft*0.25f;

    char rounded[8]; std::sprintf(rounded, "%.2f", 1.f / ft);
    sq::draw_tiny_text(rounded, 5, Alignment::BL, {8, 10}, app.get_size());
}
