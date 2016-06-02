#pragma once

#include <deque>
#include <functional>

#include <sqee/builtins.hpp>
#include <sqee/misc/OrderedMap.hpp>

// Forward Declarations /////
namespace sq { class Application; }

namespace sq {

/// The SQEE Debugging Overlay
class DebugOverlay final : NonCopyable {
public:
    DebugOverlay(Application& _app);

    void tick();
    void render(float _ft);

    void toggle_active();
    OrderedMap<string, std::function<void()>> onShowFuncs;
    OrderedMap<string, std::function<void()>> onHideFuncs;

    void notify(const string& _message, uint _time);

    double accumulation = 0.0;
    bool active = true;

private:
    Application& app;

    std::deque<pair<string, uint>> notifyDeq;
    float frameTime = 1.f / 60.f;
    uint notifyTimeLeft = 0u;
};

}
