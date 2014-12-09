#include <scenes/basicscenes.hpp>
#include <text/font.hpp>
#include <text/text.hpp>

#include <boost/timer.hpp>

using namespace sq;

void SceneFPS::render(float _ft) {
    static float fps = 60.f;
    fps = glm::mix(fps, 1.f / _ft, 0.2f);

    char rounded[8]; std::sprintf(rounded, "%.2f", fps);
    sq::draw_tiny_text(rounded, 4, Alignment::TL, {8, 10}, app.get_size());
}
