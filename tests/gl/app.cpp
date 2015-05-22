#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/handlers/basics.hpp>

#include "scenegame.hpp"
#include "app.hpp"

namespace sqt {

GlApp::GlApp() : sq::Application(true) {
    handlerIM.append("close", new sq::HandlerClose(this));
    handlerIM.append("resize", new sq::HandlerResize(this));

    sceneIM.append("game", new SceneGame(this));
    handlerIM.append("game", new HandlerGame(this));
}

}
