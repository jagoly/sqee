#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/Textures.hpp>
#include <sqee/render/Armature.hpp>
#include <sqee/render/Mesh.hpp>
#include <sqee/render/Skin.hpp>
#include <sqee/scenes/Basics.hpp>
#include <sqee/handlers/Basics.hpp>
#include <sqee/scripts/ConsoleScene.hpp>
#include <sqee/scripts/ConsoleHandler.hpp>

#include "main/MainScene.hpp"
#include "main/MainHandler.hpp"
#include "main/Scripting.hpp"
#include "RpgApp.hpp"

namespace sqt {

RpgApp::RpgApp() : sq::Application(true) {
    sq::res::tex2D_path() = "assets/textures/";
    sq::res::texCube_path() = "assets/textures/";
    sq::res::mesh_path() = "assets/meshes/";
    sq::res::skin_path() = "assets/skins/";
    sq::res::arma_path() = "assets/armatures/";

    append_scene<SceneMain>("main");
    append_scene<sq::SceneConsole>("console");
    append_scene<sq::SceneFPS>("fps");

    append_handler<sq::HandlerConsole>("console");
    append_handler<sq::HandlerClose>("close");
    append_handler<sq::HandlerResize>("resize");
    append_handler<HandlerMain>("main");

    cs_setup_main(*cs);
    cs_setup_wcoe(*cs);
    cs_setup_rndr(*cs);
    cs->eval_file("assets/test_init.chai");
}

}
