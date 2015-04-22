#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/events/basichandlers.hpp>
#include <sqee/scripts/scene.hpp>
#include <sqee/scripts/handler.hpp>

#include "scenegame.hpp"
#include "app.hpp"

namespace sqt {

ChaiApp::ChaiApp() : sq::Application(true, {960, 720}) {
    handlerIM.append("close", new sq::HandlerClose(*this));
    handlerIM.append("resize", new sq::HandlerResize(*this));

    sceneIM.append("game", new SceneGame(*this));
    handlerIM.append("game", new HandlerGame(*this));

    sceneIM.append("console", new sq::SceneConsole(*this));
    handlerIM.prepend("console", new sq::HandlerConsole(*this));
}

}
