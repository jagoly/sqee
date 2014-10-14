#include <iostream>

#include <libsqee/events/basichandlers.hpp>

#include "app.hpp"
#include "scenegame.hpp"

namespace sqt {

RpgApp::RpgApp() : sq::Application({1280, 720}, false, true, true, "SQEE RPG Engine") {
    handlerIM.append("close", new sq::HandlerClose(this));
    handlerIM.append("debug", new sq::HandlerDebug(this));
    handlerIM.append("resize", new sq::HandlerResize(this));
    handlerIM.append("framelimit", new sq::HandlerFramelimit(this));

    sceneIM.append("game", new SceneGame(this));
    handlerIM.append("game", new HandlerGame(this));
}

}
