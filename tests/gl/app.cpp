#include <sqee/events/basichandlers.hpp>

#include "app.hpp"
#include "scenegame.hpp"

namespace sqt {

GlApp::GlApp() : sq::Application({1280, 720}, true, "SQEE Tests") {
    handlerIM.append("close", new sq::HandlerClose(*this));
    handlerIM.append("resize", new sq::HandlerResize(*this));
    handlerIM.append("framelimit", new sq::HandlerFramelimit(*this));

    sceneIM.append("game", new SceneGame(*this));
    handlerIM.append("game", new HandlerGame(*this));
}

}
