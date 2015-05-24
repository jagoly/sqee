#include <sqee/handlers/basics.hpp>
#include <sqee/redist/gl_ext_3_3.hpp>
#include <sqee/gl/textures.hpp>
#include <sqee/render/animation.hpp>
#include <sqee/render/mesh.hpp>
#include <sqee/render/skin.hpp>
#include <sqee/scenes/basics.hpp>
#include <sqee/scripts/scene.hpp>
#include <sqee/scripts/handler.hpp>

#include "main/scenemain.hpp"
#include "main/handlermain.hpp"
#include "main/scripting.hpp"
#include "rpgapp.hpp"

namespace sqt {

RpgApp::RpgApp() : sq::Application(true) {
    sq::res::tex2D_path() = "assets/textures/";
    sq::res::texCube_path() = "assets/textures/";
    sq::res::anim_path() = "assets/animations/";
    sq::res::mesh_path() = "assets/meshes/";
    sq::res::skin_path() = "assets/skins/";

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
