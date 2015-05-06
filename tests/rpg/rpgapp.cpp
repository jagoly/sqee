#include <sqee/events/basichandlers.hpp>
#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/render/animation.hpp>
#include <sqee/render/mesh.hpp>
#include <sqee/render/skin.hpp>
#include <sqee/scenes/basicscenes.hpp>
#include <sqee/scripts/scene.hpp>
#include <sqee/scripts/handler.hpp>

#include "main/scenemain.hpp"
#include "main/handlermain.hpp"
#include "main/scripting.hpp"
#include "rpgapp.hpp"

namespace sqt {

RpgApp::RpgApp() : sq::Application(true, {1280, 720}) {
    sq::res::tex2D_path() = "assets/textures/";
    sq::res::texCube_path() = "assets/textures/";
    sq::res::anim_path() = "assets/animations/";
    sq::res::mesh_path() = "assets/meshes/";
    sq::res::skin_path() = "assets/skins/";

    sceneIM.append("main", new SceneMain(*this));
    sceneIM.append("console", new sq::SceneConsole(*this));
    sceneIM.append("fps", new sq::SceneFPS(*this));
    handlerIM.append("close", new sq::HandlerClose(*this));
    handlerIM.append("resize", new sq::HandlerResize(*this));
    handlerIM.prepend("console", new sq::HandlerConsole(*this));
    handlerIM.append("main", new HandlerMain(*this));

    cs_setup_main(*cs);
    cs_setup_wcoe(*cs);
    cs_setup_rndr(*cs);
    cs->eval_file("assets/test_init.chai");
}

}
