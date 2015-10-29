#pragma once

#include <deque>
#include <functional>

#include <sqee/builtins.hpp>
#include <sqee/misc/OrderedMap.hpp>

// Forward Declarations /////
namespace sq { class Application; }

namespace sq {

/// The SQEE Debug Overlay
class DebugOverlay final : NonCopyable {
public:
    DebugOverlay(const Application* _app);

    void update();
    void render(float _ft);

    void toggle_active();
    OrderedMap<string, std::function<void()>> onShowFuncs;
    OrderedMap<string, std::function<void()>> onHideFuncs;

    void notify(const string& _message, uint _time);

    double accum = 0.0;
    bool active = true;

private:
    const Application* const app;

    std::deque<pair<string, uint>> notifyDeq;
    uint notifyTimeLeft = 0u;
    float ft = 1.f / 60.f;
};

}
