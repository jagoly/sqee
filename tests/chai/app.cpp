#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/events/basichandlers.hpp>
#include <sqee/scripts/console.hpp>

#include "scenegame.hpp"
#include "app.hpp"

namespace sqt {

ChaiApp::ChaiApp() : sq::Application({960, 720}, true) {
    handlerIM.append("close", new sq::HandlerClose(*this));
    handlerIM.append("resize", new sq::HandlerResize(*this));
    handlerIM.append("framelimit", new sq::HandlerFramelimit(*this));

    sceneIM.append("game", new SceneGame(*this));
    handlerIM.append("game", new HandlerGame(*this));

    sceneIM.append("console", new sq::SceneConsole(*this));
    handlerIM.prepend("console", new sq::HandlerConsole(*this));
}

}
