#include <sqee/events/basichandlers.hpp>
#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/models/animation.hpp>
#include <sqee/models/mesh.hpp>
#include <sqee/models/skin.hpp>
#include <sqee/scenes/basicscenes.hpp>
#include <sqee/scripts/console.hpp>

#include "scenemain.hpp"
#include "rpgapp.hpp"

namespace sqt {

RpgApp::RpgApp() : sq::Application({1280, 720}, true) {
    sq::res::texture_path() = "assets/textures/";
    sq::res::anim_path() = "assets/animations/";
    sq::res::mesh_path() = "assets/meshes/";
    sq::res::skin_path() = "assets/skins/";

    handlerIM.append("close", new sq::HandlerClose(*this));
    handlerIM.append("resize", new sq::HandlerResize(*this));
    handlerIM.append("framelimit", new sq::HandlerFramelimit(*this));

    sceneIM.append("game", new SceneMain(*this));
    sceneIM.append("fps", new sq::SceneFPS(*this));
    handlerIM.append("game", new HandlerMain(*this));

    sceneIM.append("console", new sq::SceneConsole(*this));
    handlerIM.prepend("console", new sq::HandlerConsole(*this));
}

}
