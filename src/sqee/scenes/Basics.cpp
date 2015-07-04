#include <sqee/app/Application.hpp>
#include <sqee/text/Text.hpp>
#include <sqee/scenes/Basics.hpp>

using namespace sq;

void SceneFPS::render(float _ft) {
    static const TextBasic tb {
        TextBasic::Flow::Positive, TextBasic::Flow::Negative,
        TextBasic::Align::Negative, TextBasic::Align::Negative,
        fvec3(1.f, 1.f, 1.f), fvec2(40.f, 50.f), true
    };

    static float ft = 1.f/60.f; ft = ft*0.8f + _ft*0.2f;
    char rounded[8]; std::sprintf(rounded, "%.2f", 1.f / ft);
    sq::render_text_basic(rounded, tb, 1.f, appBase->get_size());
}
