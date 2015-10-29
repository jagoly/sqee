#include <sqee/redist/gl_ext_4_2.hpp>
#include <sqee/app/DebugOverlay.hpp>
#include <sqee/app/Application.hpp>
#include <sqee/debug/Misc.hpp>
#include <sqee/gl/Drawing.hpp>
#include <sqee/text/Text.hpp>

using namespace sq;

DebugOverlay::DebugOverlay(const Application* _app) : app(_app) {}

void DebugOverlay::update() {

    if (notifyDeq.empty() == false) {
        notifyTimeLeft -= 1u;

        if (notifyTimeLeft == 0u) {
            notifyDeq.pop_front();
            if (notifyDeq.empty() == false) {
                notifyTimeLeft = notifyDeq.front().second;
            }
        }
    }
}

void DebugOverlay::render(float _ft) {
    sq::VIEWPORT(app->get_size());

    char rounded[10];
    ft = _ft * 0.2f + ft * 0.8f;
    std::sprintf(rounded, "%.2f", 1.f / ft);

    render_text_basic(rounded, app->get_size(),
                      TextBasicFlow::Positive, TextBasicFlow::Negative,
                      TextBasicAlign::Negative, TextBasicAlign::Negative,
                      Vec2F(40.f, 50.f), Vec3F(1.f, 1.f, 1.f), 1.f, true);

    if (notifyDeq.empty() == false && notifyTimeLeft != notifyDeq.front().second) {
        float a = notifyTimeLeft == 1u ? 1.f - accum * 8.f : 1.f;
        render_text_basic(notifyDeq.front().first, app->get_size(),
                          TextBasicFlow::Positive, TextBasicFlow::Negative,
                          TextBasicAlign::Positive, TextBasicAlign::Negative,
                          Vec2F(25.f, 30.f), Vec3F(1.f, 1.f, 1.f), a, true);
    }
}

void DebugOverlay::toggle_active() {
    for (auto func : (active = !active) ? onShowFuncs : onHideFuncs) func();
}

void DebugOverlay::notify(const string& _message, uint _time) {
    if (active == true) {
        notifyDeq.emplace_back(_message, _time);
        if (notifyDeq.size() == 1u) notifyTimeLeft = _time;
    }
}
