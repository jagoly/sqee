#include <sqee/events/basichandlers.hpp>
#include <sqee/scenes/basicscenes.hpp>

#include "app.hpp"
#include "scenegame.hpp"

namespace sqt {

RpgApp::RpgApp() : sq::Application({1200, 750}, true, "SQEE RPG Engine") {
    int val;
    gl::GetIntegerv(gl::MAX_UNIFORM_BLOCK_SIZE, &val);
    cout << val << endl;

    handlerIM.append("close", new sq::HandlerClose(*this));
    handlerIM.append("resize", new sq::HandlerResize(*this));
    handlerIM.append("framelimit", new sq::HandlerFramelimit(*this));

    sceneIM.append("game", new SceneGame(*this));
    sceneIM.append("fps", new sq::SceneFPS(*this));
    handlerIM.append("game", new HandlerGame(*this));
}

}
