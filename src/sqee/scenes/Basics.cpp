#include <sqee/app/Application.hpp>
#include <sqee/text/Text.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/scenes/Basics.hpp>

using namespace sq;

void BasicFPSScene::render(float _ft) {
    const TextBasic tb {
        TextBasic::Flow::Positive, TextBasic::Flow::Negative,
        TextBasic::Align::Negative, TextBasic::Align::Negative,
        fvec3(1.f, 1.f, 1.f), fvec2(40.f, 50.f), true
    };

    char rounded[10];
    ft = _ft * 0.2f + ft * 0.8f;
    std::sprintf(rounded, "%.2f", 1.f / ft);

    sq::VIEWPORT(appBase->get_size());
    sq::render_text_basic(rounded, tb, 1.f, appBase->get_size());
}
